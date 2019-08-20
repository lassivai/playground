#pragma once
#include "chipphys.h"
#include "bodypart.h"
#include "../geometry.h"
#include "../geom_gfx.h"

struct CollisionPair {
  cpArbiter *chipArbiter = NULL;

  int numPoints;
  Vec2d points[2];
  double depths[2];
  Vec2d normal;
  BodyPart *bodyPartA = NULL, *bodyPartB = NULL;
  bool isInitialContact = false;
  Vec2d totalImpulse;
  double totalKineticEnergy;
  double friction, elasticity;
  Vec2d surfaceVelocity;

  virtual ~CollisionPair() {}

  CollisionPair() {}
  CollisionPair(cpArbiter *chipArbiter, bool initValues = true) {
    if(initValues) init(chipArbiter);
    else this->chipArbiter = chipArbiter;
  }

  void init(cpArbiter *chipArbiter) {
    this->chipArbiter = chipArbiter;
    numPoints = min(2, cpArbiterGetCount(chipArbiter));
    for(int i=0; i<numPoints; i++) {
      points[i] = ctov(cpArbiterGetPointA(chipArbiter, i));
      depths[i] = cpArbiterGetDepth(chipArbiter, i);
    }
    normal = ctov(cpArbiterGetNormal(chipArbiter));
    cpShape *chipShapeA, *chipShapeB;
    cpArbiterGetShapes(chipArbiter, &chipShapeA, &chipShapeB);

    bodyPartA = dynamic_cast<BodyPart*>((BodyPart*)cpShapeGetUserData(chipShapeA));
    bodyPartB = dynamic_cast<BodyPart*>((BodyPart*)cpShapeGetUserData(chipShapeA));
    isInitialContact = cpArbiterIsFirstContact(chipArbiter);
    totalImpulse = ctov(cpArbiterTotalImpulse(chipArbiter));
    totalKineticEnergy = cpArbiterTotalKE(chipArbiter);
    surfaceVelocity = ctov(cpArbiterGetSurfaceVelocity(chipArbiter));
    friction = cpArbiterGetFriction(chipArbiter);
    elasticity = cpArbiterGetRestitution(chipArbiter);
  }
};
