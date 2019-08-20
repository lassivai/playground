#pragma once

#include "chipphys.h"
#include "bodypart.h"
#include "collisionpair.h"
#include "body.h"
#include "../geometry.h"
#include "../geom_gfx.h"


// TODO FaceEachOtherConstraint: calculate how much time it would take to align the body
//      and stop doing it if too long


static const char *collidingStr[2] = { "not colliding", "colliding" };

static int numConstraintsCreated = 0, numConstraintsAddedToSpace = 0, numConstraintsDeleted = 0, numConstraintsRemovedFromSpace = 0;

static void printConstraintsDebug() {
  printf("created/deleted %d/%d, added/removed %d/%d\n", numConstraintsCreated, numConstraintsDeleted, numConstraintsAddedToSpace, numConstraintsRemovedFromSpace);
}

static void cre() {
  numConstraintsCreated++;
  printf("[CREATED]/deleted %d/%d, added/removed %d/%d\n", numConstraintsCreated, numConstraintsDeleted, numConstraintsAddedToSpace, numConstraintsRemovedFromSpace);
}
static void del() {
  numConstraintsDeleted++;
  printf("created/[DELETED] %d/%d, added/removed %d/%d\n", numConstraintsCreated, numConstraintsDeleted, numConstraintsAddedToSpace, numConstraintsRemovedFromSpace);
}
static void add() {
  numConstraintsAddedToSpace++;
  printf("created/deleted %d/%d, [ADDED]/removed %d/%d\n", numConstraintsCreated, numConstraintsDeleted, numConstraintsAddedToSpace, numConstraintsRemovedFromSpace);
}
static void rem() {
  numConstraintsRemovedFromSpace++;
  printf("created/deleted %d/%d, added/[REMOVED] %d/%d\n", numConstraintsCreated, numConstraintsDeleted, numConstraintsAddedToSpace, numConstraintsRemovedFromSpace);
}

static const std::string ConstraintNames[] = {"Simple motor", "Pivot joint", "Groove joint", "Constant angle constraint", "Damped spring", "Face each other constraint", "Rotary limit constraint"};

struct Constraint
{
  static const int simpleMotor = 1, pivotJoint = 2, grooveJoint = 3, constantAngleConstraint = 4, dampedSpring = 5, faceEachOtherConstraint = 6, rotaryLimitConstraint = 7;

  Body *bodyA = NULL, *bodyB = NULL;
  cpSpace *chipSpace = NULL;
  std::vector<cpConstraint*> chipConstraints;
  const int type;

  double maxForce = -1;

  Constraint(cpSpace *chipSpace, Body *bodyA, Body *bodyB, int type) : type(type) {
    this->chipSpace = chipSpace;
    this->bodyA = bodyA;
    this->bodyB = bodyB;
  }

  virtual ~Constraint() {}

  virtual void remove() {
    for(cpConstraint *chipConstraint : chipConstraints) {
      if(cpSpaceContainsConstraint(chipSpace, chipConstraint)) {
        cpSpaceRemoveConstraint(chipSpace, chipConstraint);
        //rem();
      }
      cpConstraintFree(chipConstraint);
      //del();
    }
  }

  inline void setMaxForce(double force, int ind = 0) {
    if(force < 0) force = INFINITY;
    maxForce = force;
    cpConstraintSetMaxForce(chipConstraints[ind], force);
  }
  inline double getMaxForce(int ind = 0) {
    return cpConstraintGetMaxForce(chipConstraints[ind]);
  }

  inline void setErrorBias(double bias, int ind = 0) {
    if(bias < 0) bias = INFINITY;
    cpConstraintSetErrorBias(chipConstraints[ind], bias);
  }
  inline double getErrorBias(int ind = 0) {
    return cpConstraintGetErrorBias(chipConstraints[ind]);
  }

  inline void setMaxBias(double bias, int ind = 0) {
    if(bias < 0) bias = INFINITY;
    cpConstraintSetMaxBias(chipConstraints[ind], bias);
  }
  inline double getMaxBias(int ind = 0) {
    return cpConstraintGetMaxBias(chipConstraints[ind]);
  }

  inline void setCollideBodies(bool value, int ind = 0) {
    cpConstraintSetCollideBodies(chipConstraints[ind], value);
  }
  inline bool getCollideBodies(int ind = 0) {
    return cpConstraintGetCollideBodies(chipConstraints[ind]);
  }

  inline double getImpulse(int ind = 0) {
    return cpConstraintGetImpulse(chipConstraints[ind]);
  }

  virtual void removeFromSpace() {
    for(cpConstraint *chipConstraint : chipConstraints) {
      if(cpSpaceContainsConstraint(chipSpace, chipConstraint)) {
        cpSpaceRemoveConstraint(chipSpace, chipConstraint);
        //rem();
      }
    }
  }

  virtual void putBackToSpace() {
    for(cpConstraint *chipConstraint : chipConstraints) {
      if(!cpSpaceContainsConstraint(chipSpace, chipConstraint)) {
        cpSpaceAddConstraint(chipSpace, chipConstraint);
        //add();
      }
    }
  }

  virtual Vec2d getPosition() {
    return Vec2d();
  }
  virtual void setPosition(const Vec2d &p) {}

  void translate(const Vec2d &t) {
    setPosition(getPosition()+t);
  }

  virtual void translateDefaultStatic(Body *defaultStaticBody, const Vec2d &t) {

  }

  virtual void render(GeomRenderer &geomRenderer, int highlight = 0) {}

  virtual void onBodyUpdate() {}

  virtual std::string getInfoString(int ind = 0) {
    int i = getCollideBodies(ind) ? 1 : 0;
    std::string str = collidingStr[i];
    str +="\nmax force "+std::to_string(getMaxForce(ind)) +
          "\nmax bias "+ std::to_string(getMaxBias(ind)) +
          "\nerror bias "+ std::to_string(getErrorBias(ind));
    return str;
  }

};


struct SimpleMotor : public Constraint
{
  cpConstraint *chipSimpleMotor = NULL;
  bool motorActive = false;

  double rate = 0;

  virtual ~SimpleMotor() {}
  //double rateFactor = 1;//, maxForceFactor = 1;

  SimpleMotor(cpSpace *chipSpace, Body *bodyA, Body *bodyB, int type = Constraint::simpleMotor) : Constraint(chipSpace, bodyA, bodyB, type) {
    chipSimpleMotor = cpSimpleMotorNew(bodyA->chipBody, bodyB->chipBody, 0*PI);
    //cre();
    chipConstraints.push_back(chipSimpleMotor);
    setCollideBodies(false, 0);
    setMaxForce(1.0e6);
    setActive(true);
  }

  SimpleMotor(SimpleMotor *simpleMotor, cpSpace *chipSpace, Body *bodyA, Body *bodyB, int type = Constraint::simpleMotor) : Constraint(chipSpace, bodyA, bodyB, type) {
    chipSimpleMotor = cpSimpleMotorNew(bodyA->chipBody, bodyB->chipBody, 0*PI);
    //cre();
    chipConstraints.push_back(chipSimpleMotor);
    setActive(simpleMotor->motorActive);
    setRate(simpleMotor->getRate());
    setMaxForce(simpleMotor->getMaxForce());
    setErrorBias(simpleMotor->getErrorBias());
    setMaxBias(simpleMotor->getMaxBias());
    setCollideBodies(simpleMotor->getCollideBodies());
  }

  void setActive(bool motorActive) {
    this->motorActive = motorActive;
    if(motorActive) {
      if(!cpSpaceContainsConstraint(chipSpace, chipSimpleMotor)) {
        cpSpaceAddConstraint(chipSpace, chipSimpleMotor);
        //add();
      }
    }
    else {
      if(cpSpaceContainsConstraint(chipSpace, chipSimpleMotor)) {
        cpSpaceRemoveConstraint(chipSpace, chipSimpleMotor);
        //rem();
      }
    }
  }
  inline bool isActive() {
    return motorActive;
  }

  inline void setRate(double rate) {
    this->rate = rate;
    cpSimpleMotorSetRate(chipSimpleMotor, rate/*rateFactor*/);
  }
  inline void scaleRate(double s) {
    cpSimpleMotorSetRate(chipSimpleMotor, s * rate);
  }
  inline double getRate() {
    return cpSimpleMotorGetRate(chipSimpleMotor);
  }

  inline void scaleMaxForce(double s) {
    cpConstraintSetMaxForce(chipSimpleMotor, s * maxForce);
  }


  /*inline void setRateFactor(double rateFactor) {
    this->rateFactor = rateFactor;
    cpSimpleMotorSetRate(chipSimpleMotor, rate*rateFactor);
  }*/


  Vec2d getPosition() {
    return bodyA->localToWorld(bodyA->getCenterOfMass());
  }


  void render(GeomRenderer &geomRenderer, int highlight = 0) {
    geomRenderer.strokeWidth = 1.5 + highlight * 1.5;
    if(motorActive) geomRenderer.strokeColor.set(0.2, 0.2, 0.8, 0.5);
    else geomRenderer.strokeColor.set(0.4, 0.3, 0.6, 0.4);
    geomRenderer.fillColor.set(0, 0, 0, 0);
    geomRenderer.drawCircle(10, bodyA->localToWorld(bodyA->getCenterOfMass()));
    geomRenderer.drawCircle(10, bodyB->localToWorld(bodyB->getCenterOfMass()));
  }
  static void render(GeomRenderer &geomRenderer, const Vec2d &p) {
    geomRenderer.strokeWidth = 3.0;
    geomRenderer.strokeColor.set(0.2, 0.2, 0.8, 0.5);
    geomRenderer.fillColor.set(0, 0, 0, 0);
    geomRenderer.drawCircle(10, p);
  }

  std::string getInfoString() {
    return "motor, " + Constraint::getInfoString();
  }
};


struct RotaryLimitConstraint : public Constraint
{
  cpConstraint *chipRotaryLimitJoint = NULL;
  double min, max;

  virtual ~RotaryLimitConstraint() {}

  RotaryLimitConstraint(cpSpace *chipSpace, Body *bodyA, Body *bodyB, double min, double max, int type = Constraint::rotaryLimitConstraint) : Constraint(chipSpace, bodyA, bodyB, type) {
    chipRotaryLimitJoint = cpRotaryLimitJointNew(bodyA->chipBody, bodyB->chipBody, min, max);
    //cre();
    setAngles(min, max);
    chipConstraints.push_back(chipRotaryLimitJoint);
    setCollideBodies(false, 0);
    setMaxForce(-1);
  }

  RotaryLimitConstraint(RotaryLimitConstraint *rotaryLimitConstraint, cpSpace *chipSpace, Body *bodyA, Body *bodyB, int type = Constraint::rotaryLimitConstraint) : Constraint(chipSpace, bodyA, bodyB, type) {
    chipRotaryLimitJoint = cpRotaryLimitJointNew(bodyA->chipBody, bodyB->chipBody, min, max);
    //cre();
    setAngles(rotaryLimitConstraint->min, rotaryLimitConstraint->max);
    chipConstraints.push_back(chipRotaryLimitJoint);
    setMaxForce(rotaryLimitConstraint->getMaxForce());
    setErrorBias(rotaryLimitConstraint->getErrorBias());
    setMaxBias(rotaryLimitConstraint->getMaxBias());
    setCollideBodies(rotaryLimitConstraint->getCollideBodies());
  }

  void setAngles(double min, double max) {
    this->min = min;
    this->max = max;
    cpRotaryLimitJointSetMin(chipRotaryLimitJoint, min);
    cpRotaryLimitJointSetMax(chipRotaryLimitJoint, max);
  }
  void setMinAngle(double min) {
    this->min = min;
    cpRotaryLimitJointSetMin(chipRotaryLimitJoint, min);
  }
  void setMaxAngle(double max) {
    this->max = max;
    cpRotaryLimitJointSetMax(chipRotaryLimitJoint, max);
  }
  double getMinAngle() {
    return min;
  }
  double getMaxAngle() {
    return max;
  }



  Vec2d getPosition() {
    return (bodyA->localToWorld(bodyA->getCenterOfMass()) + bodyB->localToWorld(bodyB->getCenterOfMass()))*0.5;
  }


  void render(GeomRenderer &geomRenderer, int highlight = 0) {
    Vec2d p = getPosition();
    double r = 30;

    geomRenderer.strokeWidth = 1.5 + highlight * 1.5;

    geomRenderer.strokeColor.set(0.7, 0.2, 0.2, 0.5);
    geomRenderer.drawLine(p.x, p.y, p.x + r*cos(-min), p.y + r*sin(-min));

    geomRenderer.strokeColor.set(0.2, 0.2, 0.8, 0.5);
    geomRenderer.drawLine(p.x, p.y, p.x + r*cos(-max), p.y + r*sin(-max));
  }

  static void render(GeomRenderer &geomRenderer, const Vec2d &p, double min = 0, double max = 0) {
    double r = 30;

    geomRenderer.strokeWidth = 3;

    geomRenderer.strokeColor.set(0.7, 0.2, 0.2, 0.5);
    geomRenderer.drawLine(p.x, p.y, p.x + r*cos(min), p.y + r*sin(min));

    geomRenderer.strokeColor.set(0.2, 0.2, 0.8, 0.5);
    geomRenderer.drawLine(p.x, p.y, p.x + r*cos(max), p.y + r*sin(max));
  }

  std::string getInfoString() {
    return "rotary limit, " + Constraint::getInfoString();
  }
};

struct PivotJoint : public Constraint
{
  Vec2d anchorA, anchorB;
  cpConstraint *chipPivotJoint = NULL;

  virtual ~PivotJoint() {}

  PivotJoint(cpSpace *chipSpace, Body *bodyA, Body *bodyB, int type = Constraint::pivotJoint) : Constraint(chipSpace, bodyA, bodyB, type) {
    chipPivotJoint = cpPivotJointNew(bodyA->chipBody, bodyB->chipBody, cpvzero);
    //cre();
    cpSpaceAddConstraint(chipSpace, chipPivotJoint);
    //add();
    chipConstraints.push_back(chipPivotJoint);
    setCollideBodies(false, 0);
  }

  PivotJoint(PivotJoint *pivotJoint, cpSpace *chipSpace, Body *bodyA, Body *bodyB, int type = Constraint::pivotJoint) : Constraint(chipSpace, bodyA, bodyB, type) {
    chipPivotJoint = cpPivotJointNew(bodyA->chipBody, bodyB->chipBody, cpvzero);
    //cre();
    cpSpaceAddConstraint(chipSpace, chipPivotJoint);
    //add();
    chipConstraints.push_back(chipPivotJoint);
    setAnchorsLocal(pivotJoint->anchorA, pivotJoint->anchorB);
    setMaxForce(pivotJoint->getMaxForce());
    setErrorBias(pivotJoint->getErrorBias());
    setMaxBias(pivotJoint->getMaxBias());
    setCollideBodies(pivotJoint->getCollideBodies());
  }


  inline void setAnchorsWorld(const Vec2d &pos) {
    setAnchorALocal(bodyA->worldToLocal(pos));
    setAnchorBLocal(bodyB->worldToLocal(pos));
  }
  inline void setAnchorsWorld(const Vec2d &a, const Vec2d &b) {
    setAnchorALocal(bodyA->worldToLocal(a));
    setAnchorBLocal(bodyB->worldToLocal(b));
  }

  inline void setAnchorsLocal(const Vec2d &a, const Vec2d &b) {
    anchorA = a;
    anchorB = b;
    cpPivotJointSetAnchorA(chipPivotJoint, vtoc(anchorA));
    cpPivotJointSetAnchorB(chipPivotJoint, vtoc(anchorB));
  }
  inline void setAnchorALocal(const Vec2d &p) {
    anchorA = p;
    cpPivotJointSetAnchorA(chipPivotJoint, vtoc(anchorA));
  }
  inline void setAnchorBLocal(const Vec2d &p) {
    anchorB = p;
    cpPivotJointSetAnchorB(chipPivotJoint, vtoc(anchorB));
  }

  Vec2d getPosition() {
    return bodyA->localToWorld(anchorA);
  }

  void setPosition(const Vec2d &p) {
    setAnchorsWorld(p);
  }

  virtual void translateDefaultStatic(Body *defaultStaticBody, const Vec2d &t) {
    if(bodyA == defaultStaticBody) setAnchorALocal(anchorA + t);
    if(bodyB == defaultStaticBody) setAnchorBLocal(anchorB + t);
  }


  void render(GeomRenderer &geomRenderer, int highlight = 0) {
    Vec2d a = bodyA->localToWorld(anchorA);

    geomRenderer.strokeWidth = 1.5 + highlight * 1.5;
    geomRenderer.strokeColor.set(0.2, 0.2, 0.8, 0.5);
    geomRenderer.drawLine(a.x-7, a.y-7, a.x+7, a.y+7);
    geomRenderer.drawLine(a.x-7, a.y+7, a.x+7, a.y-7);

    Vec2d b = bodyB->localToWorld(anchorB);
    geomRenderer.drawLine(b.x-7, b.y-7, b.x+7, b.y+7);
    geomRenderer.drawLine(b.x-7, b.y+7, b.x+7, b.y-7);
  }
  static void render(GeomRenderer &geomRenderer, const Vec2d &p) {
    geomRenderer.strokeWidth = 3.0;
    geomRenderer.strokeColor.set(0.2, 0.2, 0.8, 0.5);
    geomRenderer.drawLine(p.x-7, p.y-7, p.x+7, p.y+7);
    geomRenderer.drawLine(p.x-7, p.y+7, p.x+7, p.y-7);
  }

  std::string getInfoString() {
    return "pivot joint, " + Constraint::getInfoString();
  }
};


struct ConstantAngleConstraint : public SimpleMotor
{
  double angle;
  double speed = 5;
  bool squareRootedSpringFactor = true;

  virtual ~ConstantAngleConstraint() {}

  ConstantAngleConstraint(cpSpace *chipSpace, Body *bodyA, Body *bodyB, int type = Constraint::constantAngleConstraint) : SimpleMotor(chipSpace, bodyA, bodyB, type) {
    cpConstraintSetPreSolveFunc(chipSimpleMotor, preSolveFunc);
    cpConstraintSetUserData(chipSimpleMotor, this);
    updateAngle();
  }

  ConstantAngleConstraint(ConstantAngleConstraint *constantAngleConstraint, cpSpace *chipSpace, Body *bodyA, Body *bodyB, int type = Constraint::constantAngleConstraint) : SimpleMotor(constantAngleConstraint, chipSpace, bodyA, bodyB, type) {
    cpConstraintSetPreSolveFunc(chipSimpleMotor, preSolveFunc);
    cpConstraintSetUserData(chipSimpleMotor, this);
    updateAngle();
    speed = constantAngleConstraint->speed;
    squareRootedSpringFactor = constantAngleConstraint->squareRootedSpringFactor;
    setMaxForce(constantAngleConstraint->getMaxForce());
    setErrorBias(constantAngleConstraint->getErrorBias());
    setMaxBias(constantAngleConstraint->getMaxBias());
    setCollideBodies(constantAngleConstraint->getCollideBodies());
  }


  inline void setSpeed(double speed) {
    this->speed = speed;
  }
  inline void setSquareRootedSpringFactor(bool value) {
    squareRootedSpringFactor = value;
  }

  void onBodyUpdate() {
    //updateAngle();
  }

  void updateAngle() {
    double rotA = bodyA->getAngle();
    double rotB = bodyB->getAngle();
    angle = rotB - rotA;
  }

  void setAngle(double angle) {
    this->angle = angle;
  }

  static void preSolveFunc(cpConstraint *chipConstraint, cpSpace *chipSpace) {
    ConstantAngleConstraint *constConstraint = (ConstantAngleConstraint*) cpConstraintGetUserData(chipConstraint);
    double rotA = constConstraint->bodyA->getAngle();
    double rotB = constConstraint->bodyB->getAngle();
    double da = -constConstraint->angle + (rotB - rotA);

    da = modff(da, -PI, PI);
    //printf("%f, %f, %f, %f\n", constConstraint->angle, rotA, rotB, da);
    //if(constConstraint->squareRootedSpringFactor) {
      if(da > 0) constConstraint->setRate(constConstraint->speed*log(1+da));
      if(da < 0) constConstraint->setRate(-constConstraint->speed*log(1+fabs(da)));
    /*}
    else {
      constConstraint->setRate(constConstraint->speed*da);
    }*/
  }

  std::string getInfoString() {
    return "constant angle constraint, " + Constraint::getInfoString();
  }
};




// FIXME very experimental at this point
struct FaceEachOtherConstraint : public SimpleMotor
{
  ConstantAngleConstraint *constConstraintA, *constConstraintB;
  Body *bodyC = NULL;

  double fixedAngleA, fixedAngleB;

  bool disableBodyB = false;

  FaceEachOtherConstraint(cpSpace *chipSpace, Body *bodyA, Body *bodyB, Body *bodyC) : SimpleMotor(chipSpace, bodyA, bodyB, Constraint::faceEachOtherConstraint) {
    constConstraintA = new ConstantAngleConstraint(chipSpace, bodyA, bodyC);
    constConstraintB = new ConstantAngleConstraint(chipSpace, bodyB, bodyC);
    constConstraintA->setSpeed(20);
    constConstraintB->setSpeed(20);
    constConstraintA->setMaxForce(5e6);
    constConstraintB->setMaxForce(5e6);
    cpConstraintSetUserData(chipSimpleMotor, this);
    cpConstraintSetPreSolveFunc(chipSimpleMotor, preSolveFunc);
    updateAngle();
    this->bodyC = bodyC;
    setActive(false);
  }

  FaceEachOtherConstraint(FaceEachOtherConstraint *faceEachOtherConstraint, cpSpace *chipSpace, Body *bodyA, Body *bodyB, Body *bodyC) : SimpleMotor(faceEachOtherConstraint, chipSpace, bodyA, bodyB, Constraint::faceEachOtherConstraint) {
    constConstraintA = new ConstantAngleConstraint(faceEachOtherConstraint->constConstraintA, chipSpace, bodyA, bodyC);
    constConstraintB = new ConstantAngleConstraint(faceEachOtherConstraint->constConstraintB, chipSpace, bodyB, bodyC);
    cpConstraintSetUserData(chipSimpleMotor, this);
    cpConstraintSetPreSolveFunc(chipSimpleMotor, preSolveFunc);
    updateAngle();
    this->bodyC = bodyC;
    setActive(false);
    setMaxForce(faceEachOtherConstraint->getMaxForce());
    setErrorBias(faceEachOtherConstraint->getErrorBias());
    setMaxBias(faceEachOtherConstraint->getMaxBias());
    setCollideBodies(faceEachOtherConstraint->getCollideBodies());
  }

  ~FaceEachOtherConstraint() {
    delete constConstraintA;
    delete constConstraintB;
  }

  void setCollideBodies(bool value, int ind = 0) {
    SimpleMotor::setCollideBodies(value, ind);
    constConstraintA->setCollideBodies(value, ind);
    constConstraintB->setCollideBodies(value, ind);
  }

  inline void setSpeed(double speed) {
    constConstraintA->setSpeed(speed);
    constConstraintB->setSpeed(speed);
  }

  inline void setMaxForce(double maxForce) {
    SimpleMotor::setMaxForce(maxForce);
    constConstraintA->setMaxForce(maxForce);
    constConstraintB->setMaxForce(maxForce);
  }
  inline void setMaxBias(double maxBias) {
    SimpleMotor::setMaxBias(maxBias);
    constConstraintA->setMaxBias(maxBias);
    constConstraintB->setMaxBias(maxBias);
  }
  inline void setErrorBias(double errorBias) {
    SimpleMotor::setErrorBias(errorBias);
    constConstraintA->setErrorBias(errorBias);
    constConstraintB->setErrorBias(errorBias);
  }
  void removeFromSpace() {
    SimpleMotor::removeFromSpace();
    constConstraintA->removeFromSpace();
    constConstraintB->removeFromSpace();
  }

  void putBackToSpace() {
    SimpleMotor::putBackToSpace();
    constConstraintA->putBackToSpace();
    constConstraintB->putBackToSpace();
  }

  /*void setActive(bool active) {
    SimpleMotor::setActive(active);
    constConstraintA->setActive(active);
    constConstraintB->setActive(active);
  }*/

  void remove() {
    constConstraintA->remove();
    constConstraintB->remove();
    SimpleMotor::remove();
  }

  void setBodyBDisabled(bool disableBodyB) {
    this->disableBodyB = disableBodyB;
    constConstraintB->setActive(!disableBodyB);
  }

  void onBodyUpdate() {
    updateAngle();
  }

  void updateAngle() {
    Vec2d a = bodyA->getPosition();
    Vec2d b = bodyB->getPosition();
    double da2 = atan2(b.y-a.y, b.x-a.x);

    fixedAngleA = bodyA->getAngle() - da2;
    fixedAngleB = bodyB->getAngle() + PI - da2;
    //printf("d %.2f\na %.2f\n%.2f\n", da2, fixedAngleA, fixedAngleB);
  }

  static void preSolveFunc(cpConstraint *chipConstraint, cpSpace *chipSpace) {
    FaceEachOtherConstraint *faceConstraint = (FaceEachOtherConstraint*) cpConstraintGetUserData(chipConstraint);

    //double rotA = faceConstraint->bodyA->getAngle();
    //double rotB = faceConstraint->bodyB->getAngle();
    Vec2d a = faceConstraint->bodyA->getPosition();
    Vec2d b = faceConstraint->bodyB->getPosition();
    double da2 = atan2(b.y-a.y, b.x-a.x);

    faceConstraint->constConstraintA->angle = -da2 - faceConstraint->fixedAngleA;
    faceConstraint->constConstraintB->angle = -PI-da2 - faceConstraint->fixedAngleB;
  }

  void render(GeomRenderer &geomRenderer, int highlight = 0) {
    geomRenderer.strokeWidth = 1.5 + highlight * 1.5;
    if(motorActive) geomRenderer.strokeColor.set(0.2, 0.2, 0.8, 0.5);
    else geomRenderer.strokeColor.set(0.4, 0.3, 0.6, 0.4);
    geomRenderer.fillColor.set(0, 0, 0, 0);
    geomRenderer.drawCircle(10, bodyA->localToWorld(bodyA->getCenterOfMass()));
    if(disableBodyB) geomRenderer.strokeColor.set(0.5, 0.1, 0.2, 0.4);
    geomRenderer.drawCircle(10, bodyB->localToWorld(bodyB->getCenterOfMass()));
  }

  /*static void render(GeomRenderer &geomRenderer, const Vec2d &p) {
    geomRenderer.strokeWidth = 3.0;
    geomRenderer.strokeColor.set(0.2, 0.2, 0.8, 0.5);
    geomRenderer.fillColor.set(0, 0, 0, 0);
    geomRenderer.drawCircle(10, p);
  }*/


  std::string getInfoString() {
    return "constant angle constraint, " + Constraint::getInfoString();
  }
};



struct GrooveJoint : public Constraint
{
  Vec2d grooveA, grooveB, anchor;
  cpConstraint *chipGrooveJoint = NULL;

  GrooveJoint(cpSpace *chipSpace, Body *bodyA, Body *bodyB, Vec2d a, Vec2d b) : Constraint(chipSpace, bodyA, bodyB, Constraint::grooveJoint) {
    grooveA = bodyA->worldToLocal(a);
    grooveB = bodyA->worldToLocal(b);
    chipGrooveJoint = cpGrooveJointNew(bodyA->chipBody, bodyB->chipBody, vtoc(grooveA), vtoc(grooveB), cpvzero);
    //cre();
    cpSpaceAddConstraint(chipSpace, chipGrooveJoint);
    //add();
    chipConstraints.push_back(chipGrooveJoint);
    setCollideBodies(false, 0);
  }

  GrooveJoint(GrooveJoint *grooveJoint, cpSpace *chipSpace, Body *bodyA, Body *bodyB) : Constraint(chipSpace, bodyA, bodyB, Constraint::grooveJoint) {
    grooveA = grooveJoint->grooveA;
    grooveB = grooveJoint->grooveB;
    chipGrooveJoint = cpGrooveJointNew(bodyA->chipBody, bodyB->chipBody, vtoc(grooveA), vtoc(grooveB), cpvzero);
    //cre();
    cpSpaceAddConstraint(chipSpace, chipGrooveJoint);
    //add();
    chipConstraints.push_back(chipGrooveJoint);
    setMaxForce(grooveJoint->getMaxForce());
    setErrorBias(grooveJoint->getErrorBias());
    setMaxBias(grooveJoint->getMaxBias());
    setCollideBodies(grooveJoint->getCollideBodies());
  }

  inline void setGroovesWorld(const Vec2d &a, const Vec2d &b) {
    setGrooveAWorld(a);
    setGrooveBWorld(b);
  }
  inline void setGrooveAWorld(const Vec2d &a) {
    grooveA = bodyA->worldToLocal(a);
    cpGrooveJointSetGrooveA(chipGrooveJoint, vtoc(grooveA));
  }
  void setGrooveBWorld(const Vec2d &b) {
    grooveB = bodyA->worldToLocal(b);
    cpGrooveJointSetGrooveB(chipGrooveJoint, vtoc(grooveB));
  }
  void setGroovesLocal(const Vec2d &a, const Vec2d &b) {
    grooveA = a;
    grooveB = b;
    cpGrooveJointSetGrooveA(chipGrooveJoint, vtoc(grooveA));
    cpGrooveJointSetGrooveB(chipGrooveJoint, vtoc(grooveB));
  }

  inline void setAnchorWorld(const Vec2d &b) {
    setAnchorLocal(bodyB->worldToLocal(b));
  }
  inline void setAnchorLocal(const Vec2d &p) {
    anchor = p;
    cpGrooveJointSetAnchorB(chipGrooveJoint, vtoc(anchor));
  }

  Vec2d getPosition() {
    return (bodyA->localToWorld(grooveA) + bodyA->localToWorld(grooveB)) * 0.5;
  }

  void setPosition(const Vec2d &p) {
    Vec2d v = getPosition();
    grooveA = bodyA->worldToLocal(bodyA->localToWorld(grooveA) + p - v);
    grooveB = bodyA->worldToLocal(bodyA->localToWorld(grooveB) + p - v);
  }

  virtual void translateDefaultStatic(Body *defaultStaticBody, const Vec2d &t) {
    if(bodyA == defaultStaticBody) {
       setGroovesLocal(grooveA + t, grooveB + t);
     }
  }



  void render(GeomRenderer &geomRenderer, int highlight = 0) {
    Vec2d p = getPosition();

    geomRenderer.strokeWidth = 1.5 + highlight * 1.5;
    geomRenderer.strokeColor.set(0.2, 0.2, 0.8, 0.5);
    geomRenderer.drawLine(p.x-7, p.y-7, p.x+7, p.y+7);
    geomRenderer.drawLine(p.x-7, p.y+7, p.x+7, p.y-7);

    geomRenderer.strokeWidth = 1.2 + highlight * 1.5;
    geomRenderer.strokeColor.set(0.2, 0.2, 0.8, 0.3);
    geomRenderer.drawLine(bodyA->localToWorld(grooveA), bodyA->localToWorld(grooveB));
  }
  static void render(GeomRenderer &geomRenderer, const Vec2d &grooveA, const Vec2d &grooveB) {
    geomRenderer.strokeWidth = 3;
    geomRenderer.strokeColor.set(0.2, 0.2, 0.8, 0.3);
    geomRenderer.drawLine(grooveA, grooveB);
  }

  std::string getInfoString() {
    return "groove joint, " + Constraint::getInfoString();
  }
};


// FIXME something's not right

struct DampedSpring : public Constraint
{
  Vec2d anchorA, anchorB;
  double restLength, stiffness, damping;
  cpConstraint *chipDampedSpring = NULL;

  DampedSpring(cpSpace *chipSpace, Body *bodyA, Body *bodyB, Vec2d a, Vec2d b, double restLength, double stiffness, double damping) : Constraint(chipSpace, bodyA, bodyB, Constraint::dampedSpring) {
    anchorA = bodyA->worldToLocal(a);
    anchorB = bodyB->worldToLocal(b);
    chipDampedSpring = cpDampedSpringNew(bodyA->chipBody, bodyB->chipBody, vtoc(anchorA), vtoc(anchorB), restLength, stiffness, damping);
    //cre();
    setDamping(damping);
    setStiffness(stiffness);
    setRestLength(restLength);
    cpSpaceAddConstraint(chipSpace, chipDampedSpring);
    //add();
    chipConstraints.push_back(chipDampedSpring);
    setCollideBodies(false, 0);
  }

  DampedSpring(DampedSpring *dampedSpring, cpSpace *chipSpace, Body *bodyA, Body *bodyB) : Constraint(chipSpace, bodyA, bodyB, Constraint::dampedSpring) {
    anchorA = dampedSpring->anchorA;
    anchorB = dampedSpring->anchorB;
    chipDampedSpring = cpDampedSpringNew(bodyA->chipBody, bodyB->chipBody, vtoc(anchorA), vtoc(anchorB), restLength, stiffness, damping);
    //cre();
    setDamping(dampedSpring->damping);
    setStiffness(dampedSpring->stiffness);
    setRestLength(dampedSpring->restLength);

    cpSpaceAddConstraint(chipSpace, chipDampedSpring);
    //add();
    chipConstraints.push_back(chipDampedSpring);
    setMaxForce(dampedSpring->getMaxForce());
    setErrorBias(dampedSpring->getErrorBias());
    setMaxBias(dampedSpring->getMaxBias());
    setCollideBodies(dampedSpring->getCollideBodies());
  }

  inline void setDamping(double damping) {
    this->damping = damping;
    cpDampedSpringSetDamping(chipDampedSpring, damping);
  }
  inline void setStiffness(double stiffness) {
    this->stiffness = stiffness;
    cpDampedSpringSetStiffness(chipDampedSpring, stiffness);
  }
  inline void setRestLength(double restLength) {
    this->restLength = restLength;
    cpDampedSpringSetRestLength(chipDampedSpring, restLength);
  }

  inline void setAnchorsWorld(const Vec2d &a, const Vec2d &b) {
    setAnchorAWorld(a);
    setAnchorBWorld(b);
  }
  inline void setAnchorAWorld(const Vec2d &a) {
    anchorA = bodyA->worldToLocal(a);
    cpDampedSpringSetAnchorA(chipDampedSpring, vtoc(anchorA));
  }
  inline void setAnchorBWorld(const Vec2d &b) {
    anchorB = bodyB->worldToLocal(b);
    cpDampedSpringSetAnchorB(chipDampedSpring, vtoc(anchorB));
  }
  inline void setAnchorALocal(const Vec2d &a) {
    anchorA = a;
    cpDampedSpringSetAnchorA(chipDampedSpring, vtoc(anchorA));
  }
  inline void setAnchorBLocal(const Vec2d &b) {
    anchorB = b;
    cpDampedSpringSetAnchorB(chipDampedSpring, vtoc(anchorB));
  }

  Vec2d getPosition() {
    return (bodyA->localToWorld(anchorA) + bodyB->localToWorld(anchorB)) * 0.5;
  }

  void setPosition(const Vec2d &p) {
    Vec2d v = getPosition();
    anchorA = bodyA->worldToLocal(bodyA->localToWorld(anchorA) + p - v);
    anchorB = bodyB->worldToLocal(bodyB->localToWorld(anchorB) + p - v);
  }

  virtual void translateDefaultStatic(Body *defaultStaticBody, const Vec2d &t) {
    if(bodyA == defaultStaticBody) setAnchorALocal(anchorA + t);
    if(bodyB == defaultStaticBody) setAnchorBLocal(anchorB + t);
  }

  void render(GeomRenderer &geomRenderer, int highlight = 0) {
    Vec2d p = getPosition();

    geomRenderer.strokeWidth = 1.5 + highlight * 1.5;
    geomRenderer.strokeColor.set(0.2, 0.2, 0.8, 0.5);
    geomRenderer.drawLine(p.x-7, p.y-7, p.x+7, p.y+7);
    geomRenderer.drawLine(p.x-7, p.y+7, p.x+7, p.y-7);

    geomRenderer.strokeWidth = 1.2 + highlight * 1.5;
    geomRenderer.strokeColor.set(0.2, 0.2, 0.8, 0.3);
    geomRenderer.drawLine(bodyA->localToWorld(anchorA), bodyB->localToWorld(anchorB));
  }

  static void render(GeomRenderer &geomRenderer, const Vec2d &anchorA, const Vec2d &anchorB) {
    geomRenderer.strokeWidth = 3;
    geomRenderer.strokeColor.set(0.2, 0.2, 0.8, 0.3);
    geomRenderer.drawLine(anchorA, anchorB);
  }

  std::string getInfoString() {
    return "damped spring, " + Constraint::getInfoString();
  }
};
