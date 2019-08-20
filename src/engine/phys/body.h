#pragma once

#include "chipphys.h"
#include "bodypart.h"
#include "collisionpair.h"
#include "../geometry.h"
#include "../geom_gfx.h"

//#include <cwchar>



static const std::string bodyTypeNames[3] = {"dynamic body", "kinematic body", "static body"};

static cpBool ignoreCollisionFunc(cpArbiter *arb, cpSpace *space, void *data) {
	return cpFalse;
}
static cpBool doNotIgnoreCollisionFunc(cpArbiter *arb, cpSpace *space, void *data) {
	return cpTrue;
}

struct Body
{
  static const int dynamicBody = 0, kinematicBody = 1, staticBody = 2;
  std::string name = "";
  int categoryID = 0;
  int bodyType;
  cpBody *chipBody = NULL;
  cpSpace *chipSpace = NULL;
  //cpGroup chipCollisionGroup = 0;
  std::vector<BodyPart*> bodyParts;
  GeomRenderer::RenderingSettings renderingSettingsBoundingRect;
	GeomRenderer::RenderingSettings bodyPartRenderingSettingsDefault;
  cpShapeFilter chipCollisionFilter;

  bool isBoundingRectVisible = false;
  Vec2d centerOfMass;

	double densityDefault, elasticityDefault, frictionDefault;

	//std::vector<Impulse> currentImpulses;
	//Impulse currentImpulse;

	virtual ~Body() {}

	void clearCurrentImpulses() {
    for(BodyPart *bodyPart : bodyParts) {
			bodyPart->clearCurrentImpulses();
		}
		//currentImpulse.clear();
  }

	void setDefaults(double densityDefault, double elasticityDefault, double frictionDefault, cpShapeFilter chipCollisionFilter) {
		this->densityDefault = densityDefault;
		this->elasticityDefault = elasticityDefault;
		this->frictionDefault = frictionDefault;
		this->chipCollisionFilter = chipCollisionFilter;
	}
  //double mass, momentOfIntertia;
  //Vec2d centerOfMass;

  Body() {
    setDefaultRenderingSettings();
    chipCollisionFilter = collisionFilterDefault;
  }

  Body(const Body &body) {
    name = body.name;
    categoryID = body.categoryID;
    //chipCollisionGroup = body.chipCollisionGroup;
		//chipCollisionFilter.group = body.chipCollisionGroup;
    chipCollisionFilter = body.chipCollisionFilter;
    //setCollisionGroup(chipCollisionGroup);
    isBoundingRectVisible = false;
    //bodyType = body.bodyType;
    //centerOfMass = body.centerOfMass;
    //chipBody = NULL;
    //bodyParts = ;
    //chipSpace = body.chipSpace;
    init(body.chipSpace, body.bodyType, cpSpaceContainsBody(body.chipSpace, body.chipBody));
    for(BodyPart *bodyPart : body.bodyParts) {
      addBodyPart(bodyPart);
    }

		setPosition(body.getPosition());
		setAngle(body.getAngle());

    setDefaultRenderingSettings();
  }

  void setDefaultRenderingSettings() {
    renderingSettingsBoundingRect.fillColor.set(0.3, 0, 0, 0.1);
    renderingSettingsBoundingRect.strokeColor.set(0.3, 0, 0, 0.3);
    renderingSettingsBoundingRect.strokeType = 1;
    renderingSettingsBoundingRect.strokeWidth = 4;
    renderingSettingsBoundingRect.texture = NULL;
  }

  void removeFromSpace() {
    if(cpSpaceContainsBody(chipSpace, chipBody)) {
      cpSpaceRemoveBody(chipSpace, chipBody);
      //setCollisionFilter(collisionFilterLockedOnly);
    }
  }

  void putBackToSpace() {
    if(!cpSpaceContainsBody(chipSpace, chipBody)) {
      cpSpaceAddBody(chipSpace, chipBody);
      //setCollisionFilter(collisionFilterDefault);
    }
  }

  bool removeBodyPart(BodyPart *bodyPart) {
    for(int i=0; i<bodyParts.size(); i++) {
      if(bodyParts[i] == bodyPart) {
        bodyParts[i]->remove();
        delete bodyParts[i];
        bodyParts.erase(bodyParts.begin() + i);
        i--;
      }
    }
    if(bodyParts.size() > 0) {
      updateMassAndMomentOfInertia();
      return true;
    }
    return false;
  }

  void remove() {
    for(int i=0; i<bodyParts.size(); i++) {
      bodyParts[i]->remove();
      delete bodyParts[i];
      bodyParts.erase(bodyParts.begin() + i);
      i--;
    }
    bodyParts.clear();
    if(chipBody != cpSpaceGetStaticBody(chipSpace)) {
      if(cpSpaceContainsBody(chipSpace, chipBody)) {
        cpSpaceRemoveBody(chipSpace, chipBody);
      }
      cpBodyFree(chipBody);
    }
  }

  void init(cpSpace *chipSpace, int bodyType = dynamicBody, bool addToSpace = true) {
    this->chipSpace = chipSpace;
    this->bodyType = bodyType;

    if(bodyType == kinematicBody) {
      chipBody = cpBodyNewKinematic();
      if(addToSpace) cpSpaceAddBody(chipSpace, chipBody);
    }
    else if(bodyType == staticBody) {
      chipBody = cpBodyNewStatic();
    }
    else if(bodyType == -1) {
      chipBody = cpSpaceGetStaticBody(chipSpace);
    }
    else {
      chipBody = cpBodyNew(1, 1);
      if(addToSpace) cpSpaceAddBody(chipSpace, chipBody);
    }
  }

  BodyPart *addCircle(double r, double x, double y, double rot = 0, double density = -1, double elasticity = -1, double friction = -1) {
    Circle circle(r, x, y, rot);
    return addBodyPart(&circle, density, elasticity, friction);
  }
  BodyPart *addLine(double x0, double y0, double x1, double y1, double r = 1, double density = -1, double elasticity = -1, double friction = -1) {
    Line line(x0, y0, x1, y1, r);
    return addBodyPart(&line, density, elasticity, friction);
  }
  BodyPart *addRect(double w, double h, double x, double y, double rot = 0, double density = -1, double elasticity = -1, double friction = -1) {
    Rect rect(w, h, x, y, rot);
    return addBodyPart(&rect, density, elasticity, friction);
  }
  BodyPart *addTriangle(double w, double h, double x, double y, double density = -1, double elasticity = -1, double friction = -1) {
    Triangle triangle(x-w*0.5, y-h*0.5, x, y+h*0.5, x+w*0.5, y-h*0.5);
    return addBodyPart(&triangle, density, elasticity, friction);
  }
	BodyPart *addHoop(double r, double r2, double x, double y, double rot = 0, double density = -1, double elasticity = -1, double friction = -1) {
    Hoop hoop(r, r2, x, y, rot);
    return addBodyPart(&hoop, density, elasticity, friction);
  }

  BodyPart *addPolygon(int n, double r, double x, double y, double density = -1, double elasticity = -1, double friction = -1) {
    Polygon poly;
    poly.createRegular(n, r);
    for(int i=0; i<poly.vertices.size(); i++) {
      poly.vertices[i].x += x;
      poly.vertices[i].y += y;
    }
    return addBodyPart(&poly, density, elasticity, friction);
  }

  BodyPart *addBodyPart(BodyPart *bodyPart) {
    BodyPart *newBodyPart = addBodyPart(bodyPart->geomObject, bodyPart->getDensity(), bodyPart->getElasticity(), bodyPart->getFriction());
		newBodyPart->setCollisionFilter(bodyPart->chipCollisionFilter);
		return newBodyPart;
  }

  BodyPart *addBodyPart(GeomObject *geomObject, double density = -1, double elasticity = -1, double friction = -1) {
		density = density >= 0 ? density : densityDefault;
		elasticity = elasticity >= 0 ? elasticity : elasticityDefault;
		friction = friction >= 0 ? friction : frictionDefault;

    BodyPart *bodyPart = new BodyPart();
    if(geomObject->type == GeomObject::circle) {
      Circle *c = dynamic_cast<Circle*>(geomObject);
      cpShape *chipShape = cpCircleShapeNew(chipBody, c->r, cpv(c->pos.x, c->pos.y));
      cpSpaceAddShape(chipSpace, chipShape);
      bodyPart->set(c, chipShape, chipSpace, this, density);
    }

    if(geomObject->type == GeomObject::hoop) {
      Hoop *h = dynamic_cast<Hoop*>(geomObject);
      cpShape *chipShape = cpCircleShapeNew(chipBody, h->r, cpv(h->pos.x, h->pos.y));
      cpSpaceAddShape(chipSpace, chipShape);
      bodyPart->set(h, chipShape, chipSpace, this, density);
    }

    if(geomObject->type == GeomObject::line) {
      Line *l = dynamic_cast<Line*>(geomObject);
      cpShape *chipShape = cpSegmentShapeNew(chipBody, cpv(l->a.x, l->a.y), cpv(l->b.x, l->b.y), l->bevelRadius);
      cpSpaceAddShape(chipSpace, chipShape);
      bodyPart->set(l, chipShape, chipSpace, this, density);
    }

    if(geomObject->type == GeomObject::triangle) {
      Triangle *t = dynamic_cast<Triangle*>(geomObject);
      cpVect verts[3] = {cpv(t->a.x, t->a.y), cpv(t->b.x, t->b.y), cpv(t->c.x, t->c.y)};
      cpShape *chipShape = cpPolyShapeNew(chipBody, 3, verts, cpTransformIdentity, 0.0);
      cpSpaceAddShape(chipSpace, chipShape);
      bodyPart->set(t, chipShape, chipSpace, this, density);
    }

    if(geomObject->type == GeomObject::rect) {
      Rect *r = dynamic_cast<Rect*>(geomObject);
      cpFloat x0 = -r->w*0.5, y0 = -r->h*0.5, x1 = r->w*0.5, y1 = r->h*0.5;
      cpVect verts[4] = {cpv(x0, y0), cpv(x0, y1), cpv(x1, y1), cpv(x1, y0)};
      cpShape *chipShape = cpPolyShapeNew(chipBody, 4, verts, cpTransformRigid(cpv(r->pos.x, r->pos.y), r->rot), 0.0);
      cpSpaceAddShape(chipSpace, chipShape);
      bodyPart->set(r, chipShape, chipSpace, this, density);
    }

    if(geomObject->type == GeomObject::polygon) {
      Polygon *polygon = dynamic_cast<Polygon*>(geomObject);
      polygon->triangulate();
      std::vector<cpShape*> chipShapes;

      for(Triangle &t : polygon->triangles) {
        cpVect verts[3] = {cpv(t.a.x, t.a.y), cpv(t.b.x, t.b.y), cpv(t.c.x, t.c.y)};
        cpShape *chipShape = cpPolyShapeNew(chipBody, 3, verts, cpTransformIdentity, 0.0);
        cpSpaceAddShape(chipSpace, chipShape);
        chipShapes.push_back(chipShape);
      }
      bodyPart->set(polygon, chipShapes, chipSpace, this, density);
    }

    bodyPart->setCollisionFilter(chipCollisionFilter);
    bodyPart->setElasticity(elasticity);
    bodyPart->setFriction(friction);
    bodyParts.push_back(bodyPart);
    bodyPart->updateMass();
    updateMassAndMomentOfInertia();

		return bodyPart;
  }

  void updateMassAndMomentOfInertia() {
    if(bodyType != Body::dynamicBody) return;

    /*for(BodyPart *bodyPart : bodyParts) {
      for(cpShape *chipShape : bodyPart->chipShapes) {
        cpSpaceReindexShape(chipSpace, chipShape);
      }
    }*/

    double mass = 0, momentOfIntertia = 0;
    centerOfMass.set(0, 0);

    for(BodyPart *bodyPart : bodyParts) {
      double m = bodyPart->getArea() * bodyPart->density;
      Vec2d pos = bodyPart->getCenterOfMass();
      centerOfMass += pos * m;
      mass += m;
    }
    centerOfMass /= mass;

    for(BodyPart *bodyPart : bodyParts) {

      if(bodyPart->geomObject->type == GeomObject::circle) {
        double m = bodyPart->getArea() * bodyPart->density;
        Vec2d pos = bodyPart->getCenterOfMass();
        Vec2d off = pos - centerOfMass;
        Circle *circle = dynamic_cast<Circle*>(bodyPart->geomObject);
        cpFloat moment = cpMomentForCircle(m, 0.0, circle->r, cpv(off.x, off.y));
        momentOfIntertia += moment;
        //printf("m c: %.4f\n", moment);
      }

			if(bodyPart->geomObject->type == GeomObject::hoop) {
        double m = bodyPart->getArea() * bodyPart->density;
        Vec2d pos = bodyPart->getCenterOfMass();
        Vec2d off = pos - centerOfMass;
        Hoop *hoop = dynamic_cast<Hoop*>(bodyPart->geomObject);
        cpFloat moment = cpMomentForCircle(m, hoop->r2, hoop->r, cpv(off.x, off.y));
        momentOfIntertia += moment;
        //printf("m c: %.4f\n", moment);
      }

      if(bodyPart->geomObject->type == GeomObject::line) {
        Line *line = dynamic_cast<Line*>(bodyPart->geomObject);
        //Vec2d pos = line->getCenterOfMass();
        double m = line->getArea() * bodyPart->density;
        //Vec2d off = pos - centerOfMass;
        Vec2d n = line->getNormal();
        n.setLength(line->bevelRadius);
        cpVect verts[4] = {vtoc(line->a - n ), vtoc(line->a + n), vtoc(line->b + n), vtoc(line->b - n)};
        //for(int i=0; i<4; i++)
        //printf("(%.4f, %.4f), \n", verts[i].x, verts[i].y);
        //cpVect verts[4] = {cpv(0, 0), cpv(1, 0), cpv(1, 1), cpv(1, 0)};
        //cpFloat moment = cpMomentForPoly(m, 4, verts, cpv(off.x, off.y), 0);
        cpFloat moment = cpMomentForPoly(m, 4, verts, vtoc(-centerOfMass), 0);
        momentOfIntertia += moment;
        //printf("m l: %.4f\n", moment);
      }

      if(bodyPart->geomObject->type == GeomObject::triangle) {
        Triangle *t = dynamic_cast<Triangle*>(bodyPart->geomObject);
        double m = t->getArea() * bodyPart->density;
        //Vec2d pos = t->getCenterOfMass();
        //Vec2d off = pos - centerOfMass;
        cpVect verts[3] = {cpv(t->a.x, t->a.y), cpv(t->b.x, t->b.y), cpv(t->c.x, t->c.y)};
        cpFloat moment = cpMomentForPoly(m, 3, verts, vtoc(-centerOfMass), 0.0);
        momentOfIntertia += moment;
        //printf("m t: %.4f\n", moment);
      }

      if(bodyPart->geomObject->type == GeomObject::rect) {
        Rect *r = dynamic_cast<Rect*>(bodyPart->geomObject);
        Vec2d pos = r->getCenterOfMass();
        double m = r->getArea() * bodyPart->density;
        //Vec2d off = pos - centerOfMass;
        cpFloat x0 = pos.x-r->w*0.5, y0 = pos.y-r->h*0.5, x1 = pos.x+r->w*0.5, y1 = pos.y+r->h*0.5;
        cpVect verts[4] = {cpv(x0, y0), cpv(x0, y1), cpv(x1, y1), cpv(x1, y0)};
        cpFloat moment = cpMomentForPoly(m, 4, verts, vtoc(-centerOfMass), 0.0);
        momentOfIntertia += moment;
        //printf("m r: %.4f\n", moment);
      }

      if(bodyPart->geomObject->type == GeomObject::polygon) {
        Polygon *poly = dynamic_cast<Polygon*>(bodyPart->geomObject);
        for(Triangle &t : poly->triangles) {
          double m = t.getArea() * bodyPart->density;
          //Vec2d pos = t.getCenterOfMass();
          //Vec2d off = pos - centerOfMass;
          cpVect verts[3] = {cpv(t.a.x, t.a.y), cpv(t.b.x, t.b.y), cpv(t.c.x, t.c.y)};
          cpFloat moment = cpMomentForPoly(m, 3, verts, vtoc(-centerOfMass), 0.0);
          momentOfIntertia += moment;
          //printf("m p.t: %.4f\n", moment);
        }
      }
    }
    cpBodySetCenterOfGravity(chipBody, cpv(centerOfMass.x, centerOfMass.y));
    cpBodySetMass(chipBody, mass);
    cpBodySetMoment(chipBody, momentOfIntertia);
  }


  inline void updateBody() {
    if(bodyType == Body::staticBody) {
      for(BodyPart *bodyPart : bodyParts) {
        for(cpShape *chipShape : bodyPart->chipShapes) {
          cpSpaceReindexShape(chipSpace, chipShape);
        }
			}
		}
  }

  void setDensity(double density) {
    for(BodyPart *bodyPart : bodyParts) {
      bodyPart->setDensity(density);
    }
    updateMassAndMomentOfInertia();
  }

  double getArea() {
    double area = 0;
    for(BodyPart *bodyPart : bodyParts) {
      area += bodyPart->getArea();
    }
    return area;
  }

  inline double getMass() {
    return cpBodyGetMass(chipBody);
  }
  inline double getMomentOfIntertia() {
    return cpBodyGetMoment(chipBody);
  }

  inline Vec2d getCenterOfMass() {
    return ctov(cpBodyGetCenterOfGravity(chipBody));
  }

  inline void setPosition(const Vec2d &pos) {
    setPosition(pos.x, pos.y);
  }
  inline void setPosition(double x, double y) {
    cpBodySetPosition(chipBody, cpv((cpFloat)x, (cpFloat)y));
    updateBody();
  }
  inline Vec2d getPosition() const {
    cpVect v = cpBodyGetPosition(chipBody);
    return Vec2d(v.x, v.y);
  }
  inline void translate(const Vec2d &dp) {
    translate(dp.x, dp.y);
  }
  inline void translate(double dx, double dy) {
    cpVect v = cpBodyGetPosition(chipBody);
    cpBodySetPosition(chipBody, cpv(v.x + (cpFloat)dx, v.y + (cpFloat)dy));
    updateBody();
  }

  inline void setAngle(double a) {
    cpBodySetAngle(chipBody, a);
    updateBody();
  }
  inline double getAngle() const {
    return cpBodyGetAngle(chipBody);
  }
  inline void rotate(double da) {
    cpBodySetAngle(chipBody, cpBodyGetAngle(chipBody) + da);
    updateBody();
  }

  inline void setVelocity(const Vec2d &v) {
    setVelocity(v.x, v.y);
  }
  inline void setVelocity(double x, double y) {
    cpBodySetVelocity(chipBody, cpv((cpFloat)x, (cpFloat)y));
  }
  inline Vec2d getVelocity() {
    cpVect v = cpBodyGetVelocity(chipBody);
    return Vec2d(v.x, v.y);
  }

  inline void setAnglularVelocity(double w) {
    cpBodySetAngularVelocity(chipBody, w);
  }
  inline double getAnglularVelocity() {
    return cpBodyGetAngularVelocity(chipBody);
  }

  inline void applyForceAtWorldPoint(const Vec2d &force, const Vec2d &pos) {
    applyForceAtWorldPoint(force.x, force.y, pos.x, pos.y);
  }
  inline void applyForceAtWorldPoint(double fx, double fy, double px, double py)  {
    cpBodyApplyForceAtWorldPoint(chipBody, cpv(fx, fy), cpv(px, py));
  }
  inline void applyForceAtLocalPoint(const Vec2d &force, const Vec2d &pos) {
    applyForceAtLocalPoint(force.x, force.y, pos.x, pos.y);
  }
  inline void applyForceAtLocalPoint(double fx, double fy, double px = 0, double py = 0)  {
    cpBodyApplyForceAtLocalPoint(chipBody, cpv(fx, fy), cpv(px, py));
  }
  inline void setForce(const Vec2d &v) {
    setForce(v.x, v.y);
  }
  inline void setForce(double x, double y)  {
    cpBodySetForce(chipBody, cpv(x, y));
  }
  inline Vec2d getForce() {
    cpVect v = cpBodyGetForce(chipBody);
    return Vec2d(v.x, v.y);
  }

  inline void setTorque(double t) {
    cpBodySetTorque(chipBody, t);
  }
  inline double getTorque() {
    return cpBodyGetTorque(chipBody);
  }
	inline void applyTorque(double t) {
    cpBodySetTorque(chipBody, cpBodyGetTorque(chipBody) + t);
  }

  inline void applyImpulseAtWorldPoint(const Vec2d &force, const Vec2d &pos) {
    applyImpulseAtWorldPoint(force.x, force.y, pos.x, pos.y);
  }
  inline void applyImpulseAtWorldPoint(double fx, double fy, double px, double py)  {
    cpBodyApplyImpulseAtWorldPoint(chipBody, cpv(fx, fy), cpv(px, py));
  }
  inline void applyImpulseAtLocalPoint(const Vec2d &force, const Vec2d &pos) {
    applyImpulseAtLocalPoint(force.x, force.y, pos.x, pos.y);
  }
  inline void applyImpulseAtLocalPoint(double fx, double fy, double px, double py)  {
    cpBodyApplyImpulseAtLocalPoint(chipBody, cpv(fx, fy), cpv(px, py));
  }

  inline bool isSleeping() {
    return cpBodyIsSleeping(chipBody);
  }
  inline void wakeUp() {
    cpBodyActivate(chipBody);
  }
  inline void putToSleep() {
    cpBodySleep(chipBody);
  }
  inline void wakeUpStatic() {
    cpBodyActivateStatic(chipBody, NULL);
  }

  inline void setSensor(bool value) {
    for(BodyPart *bodyPart : bodyParts) {
      for(cpShape *chipShape : bodyPart->chipShapes) {
        cpShapeSetSensor(chipShape, value);
      }
    }
  }
  // FIXME
  inline bool isSensor() {
    return false;
    //return cpShapeGetSensor(chipShape);
  }

	/*void setCollisionCategory(unsigned int belongs, unsigned int collides) {
		cpShapeFilter collisionFilterDefault = {0, ~lockedCategoryBitMask, allCategories};
	}*/

	void checkCatMask() {
		if(chipCollisionFilter.categories == 0 || chipCollisionFilter.categories == (1<<31)) {
			chipCollisionFilter.categories =  chipCollisionFilter.categories | 1;
		}
		if(chipCollisionFilter.mask == 0 || chipCollisionFilter.mask == (1<<31)) {
			chipCollisionFilter.mask =  chipCollisionFilter.mask | 1;
		}
	}

  void setCollisionFilter(cpShapeFilter filter) {
    this->chipCollisionFilter = filter;
		checkCatMask();
    for(BodyPart *bodyPart : bodyParts) {
      bodyPart->setCollisionFilter(filter);
    }
  }
  void setCollisionGroup(unsigned int group) {
    chipCollisionFilter.group = group;
    for(BodyPart *bodyPart : bodyParts) {
      bodyPart->setCollisionGroup(group);
    }
  }
	void setCollisionCategories(unsigned int categories) {
    chipCollisionFilter.categories = categories;
		checkCatMask();
    for(BodyPart *bodyPart : bodyParts) {
      bodyPart->setCollisionCategories(categories);
    }
  }
	void setCollisionMask(unsigned int mask) {
		chipCollisionFilter.mask = mask;
		checkCatMask();
    for(BodyPart *bodyPart : bodyParts) {
      bodyPart->setCollisionMask(mask);
    }
  }

	void toggleCollisionCategory(int index) {
		unsigned int q = chipCollisionFilter.categories;
		chipCollisionFilter.categories = (q & ~(1<<index)) | (~q & (1<<index));
		checkCatMask();
		setCollisionCategories(chipCollisionFilter.categories);
	}
	void toggleCollisionMask(int index) {
		unsigned int q = chipCollisionFilter.mask;
		chipCollisionFilter.mask = (q & ~(1<<index)) | (~q & (1<<index));
		checkCatMask();
		setCollisionMask(chipCollisionFilter.mask);
	}

  static void arbiterIterator(cpBody *chipBody, cpArbiter *chipArbiter, std::vector<CollisionPair> *pairs) {
    CollisionPair pair(chipArbiter);
    pairs->push_back(pair);
  }

  std::vector<CollisionPair> getCollisionPairs() {
    std::vector<CollisionPair> pairs;
    cpBodyEachArbiter(chipBody, (cpBodyArbiterIteratorFunc)arbiterIterator, &pairs);
    return pairs;
  }

  /*
  cpCollisionType cpShapeGetCollisionType(const cpShape *shape)
  void cpShapeSetCollisionType(cpShape *shape, cpCollisionType value)

  cpShapeFilter cpShapeGetFilter(const cpShape *shape)
  void cpShapeSetFilter(cpShape *shape, cpShapeFilter filter)

  cpVect cpShapeGetSurfaceVelocity(const cpShape *shape)
  void cpShapeSetSurfaceVelocity(cpShape *shape, cpVect value)
      */

  inline void setElasticity(double elasticity) {
    for(BodyPart *bodyPart : bodyParts) {
      bodyPart->setElasticity(elasticity);
    }
  }
  inline void setFriction(double friction) {
    for(BodyPart *bodyPart : bodyParts) {
      bodyPart->setFriction(friction);
    }
  }

  inline void setChipBodyUserData(void *data) {
    cpBodySetUserData(chipBody, (cpDataPointer)data);
  }
  inline cpDataPointer getChipBodyUserData() {
    return cpBodyGetUserData(chipBody);
  }

  inline Vec2d localToWorld(const Vec2d &localPos) {
    cpVect v = cpBodyLocalToWorld(chipBody, cpv(localPos.x, localPos.y));
    return Vec2d(v.x, v.y);
  }
  inline Vec2d worldToLocal(const Vec2d &worldPos) {
    cpVect v = cpBodyWorldToLocal(chipBody, cpv(worldPos.x, worldPos.y));
    return Vec2d(v.x, v.y);
  }

  inline Vec2d getAbsoluteVelocityAtWorldPoint(const Vec2d &pos) {
    cpVect v = cpBodyGetVelocityAtWorldPoint(chipBody, cpv(pos.x, pos.y));
    return Vec2d(v.x, v.y);
  }
  inline Vec2d getAbsoluteVelocityAtLocalPoint(const Vec2d &pos) {
    cpVect v = cpBodyGetVelocityAtLocalPoint(chipBody, cpv(pos.x, pos.y));
    return Vec2d(v.x, v.y);
  }

  // typedef void (*cpBodyShapeIteratorFunc)(cpBody *body, cpShape *shape, void *data)
  inline void goThroughShapes(cpBodyShapeIteratorFunc func, void *data) {
    cpBodyEachShape(chipBody, func, data);
  }

  // typedef void (*cpBodyConstraintIteratorFunc)(cpBody *body, cpConstraint *constraint, void *data)
  inline void goThroughConstraints(cpBodyConstraintIteratorFunc func, void *data) {
    cpBodyEachConstraint(chipBody, func, data);
  }

  // typedef void (*cpBodyArbiterIteratorFunc)(cpBody *body, cpArbiter *arbiter, void *data)
  inline void goThroughCollisionPairs(cpBodyArbiterIteratorFunc func, void *data) {
    cpBodyEachArbiter(chipBody, func, data);
  }

  inline BoundingRect getBoundingRect() {
    BoundingRect br; //(getPosition());
    if(bodyParts.size() == 0) return br;
    br = bodyParts[0]->getBoundingRect();
    for(int i=1; i<bodyParts.size(); i++) {
      br.expand(bodyParts[i]->getBoundingRect());
    }
    return br;
  }

  void renderBoundingRect(GeomRenderer &geomRenderer) {
    BoundingRect br = getBoundingRect();
    geomRenderer.setSettings(renderingSettingsBoundingRect);
    geomRenderer.drawRect(br);
  }

	int getNumShapesInSpace() {
		int n = 0;
		for(BodyPart *bodyPart : bodyParts) {
			n += bodyPart->getNumShapesInSpace();
		}
		return n;
	}


  void render(GeomRenderer &geomRenderer) {
    Vec2d pos = getPosition();
    double rot = getAngle();

    if(isBoundingRectVisible) {
      renderBoundingRect(geomRenderer);
    }

    glPushMatrix();
    glTranslated(pos.x, pos.y, 0);
    glRotatef(rot*180.0/PI, 0, 0, 1);

    if(bodyParts.size() == 0) {
      geomRenderer.setSettings(renderingSettingsBoundingRect);
      geomRenderer.drawRect(200, 200);
    }
    for(BodyPart *bodyPart : bodyParts) {
      bodyPart->render(geomRenderer);
    }

    if(isBoundingRectVisible) {
      Vec2d p = getCenterOfMass();
      geomRenderer.strokeWidth = 1.2;
      geomRenderer.strokeColor.set(1, 1, 1, 0.25);

      glPushMatrix();
      glTranslated(centerOfMass.x, centerOfMass.y, 0);
      geomRenderer.drawLine(-10, -10, 10, 10);
      geomRenderer.drawLine(-10, 10, 10, -10);
      glPopMatrix();
    }

    glPopMatrix();
  }

  //cpBodySetVelocityUpdateFunc(body.chipBody, func);




  std::string getInfo() {
    char buf[4096];

    std::sprintf(buf, "%s, \"%s\"\n", bodyTypeNames[bodyType].c_str(), name.c_str());
    Vec2d pos = getPosition(), vel = getVelocity();
    double av = getAnglularVelocity();
    std::sprintf(buf+strlen(buf), "pos. (%.2f, %.2f)\nvel. (%.2f, %.2f), a.vel. %.2f\n", pos.x, pos.y, vel.x, vel.y, av);
    std::sprintf(buf+strlen(buf), "mass %.4f, m.i. %.4f\n", getMass(), getMomentOfIntertia());
    for(int i=0; i<bodyParts.size(); i++) {
      BodyPart *p = bodyParts[i];
      std::sprintf(buf+strlen(buf), "%d: %s, m %.4f, a %.2f, d %.4g, e %.4f, f %.4f\n", i+1, p->geomObject->getName().c_str(), p->getMass(), p->getArea(), p->getDensity(), p->getElasticity(), p->getFriction());
    }

    std::sprintf(buf+strlen(buf), "collision group %lu\n", chipCollisionFilter.group);
    std::vector<CollisionPair> collisionPairs = getCollisionPairs();
    std::sprintf(buf+strlen(buf), "num contacts %d\n", (int)collisionPairs.size());

    return std::string(buf);
  }

  std::wstring getInfoW() {
    int s = 4096;
    wchar_t buf[s];

    std::swprintf(buf, s, L"%s, \"%s\"\n", bodyTypeNames[bodyType].c_str(), name.c_str());
    Vec2d pos = getPosition(), vel = getVelocity();
    double av = getAnglularVelocity();
    std::swprintf(buf+wcslen(buf), s, L"pos. (%.2f, %.2f)\nvel. (%.2f, %.2f), a.vel. %.2f\n", pos.x, pos.y, vel.x, vel.y, av);
    std::swprintf(buf+wcslen(buf), s, L"mass %.4f, m.i. %.4f\n", getMass(), getMomentOfIntertia());
    for(int i=0; i<bodyParts.size(); i++) {
      BodyPart *p = bodyParts[i];
      std::swprintf(buf+wcslen(buf), s, L"%d: %s, m %.4f, a %.2f, d %.4g, e %.4f, f %.4f\n", i+1, p->geomObject->getName().c_str(), p->getMass(), p->getArea(), p->getDensity(), p->getElasticity(), p->getFriction());
    }

    std::swprintf(buf+wcslen(buf), s, L"collision group %lu\n", chipCollisionFilter.group);
    std::vector<CollisionPair> collisionPairs = getCollisionPairs();
    std::swprintf(buf+wcslen(buf), s, L"num contacts %d\n", (int)collisionPairs.size());

    return std::wstring(buf);
  }

};
