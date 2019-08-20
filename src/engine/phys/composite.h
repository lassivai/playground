#pragma once

#include "chipphys.h"
#include "bodypart.h"
#include "collisionpair.h"
#include "body.h"
#include "constraint.h"
#include "input_interface.h"
#include "../geometry.h"
#include "../geom_gfx.h"
#include "../util.h"



struct Composite
{
  static const int plain = 0, genProg = 1;
  const int type;
  std::vector<Body*> bodies;
  std::vector<Constraint*> constraints;

  // FIXME if you come up with a better idea than including this here, go ahead
  Body *defaultStaticBody = NULL;

  virtual ~Composite() {}

  Composite(Body *defaultStaticBody, int type = 0) : type(type) {
    this->defaultStaticBody = defaultStaticBody;
  }

  bool containsBody(Body *body) {
    return contains(bodies, body);
  }
  bool containsConstraint(Constraint *constraint) {
    return contains(constraints, constraint);
  }

  void translate(const Vec2d &t) {
    for(Body *body : bodies) {
      body->translate(t);
    }

    for(Constraint *constraint : constraints) {
      constraint->translateDefaultStatic(defaultStaticBody, t);
    }

  }

  BoundingRect getBoundingRect() {
    BoundingRect br;
    if(bodies.size() == 0) return br;

    bool initialized = false;
    for(int i=0; i<bodies.size(); i++) {
      if(bodies[i]->bodyParts.size() == 0) continue;
      if(!initialized) {
        br = bodies[i]->getBoundingRect();
        initialized = true;
      }
      else {
        br.expand(bodies[i]->getBoundingRect());
      }
    }
    return br;
  }

  virtual void onUpdate(double dt) {}

  void render(GeomRenderer &geomRenderer) {
    if(bodies.size() == 0) return;

    BoundingRect br = getBoundingRect();

    geomRenderer.fillColor.set(0.4, 0.12, 0.4, 0.5);
    geomRenderer.strokeColor.set(0.6, 0.2, 0.6, 0.7);
    geomRenderer.strokeWidth = 4;
    geomRenderer.strokeType = 1;
    geomRenderer.texture = NULL;
    geomRenderer.drawRect(br);
  }

};
