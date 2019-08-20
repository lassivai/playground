#pragma once

#include "chipphys.h"
#include "bodypart.h"
#include "collisionpair.h"
#include "body.h"
#include "constraint.h"
#include "composite.h"
#include "input_interface.h"
#include "genprog_interface.h"
#include "../geometry.h"
#include "../geom_gfx.h"

/*
 * TODO
 * - OpenGL array objects or something for bodies/bodyparts
 * - point gravity, mutual gravity
 * - polygon collision detection might be little bit more robust if concave polygon
 *   would be divided into bigger convex polygons than triangles (currently small triangles
 *   in polygons get stuck when penetreting other bodies)
 * - more debugging for polygon (apparently still causes some crashes, at least the holed ones)
 * - static polygons as bevelled lines instead of triangles(?)
 * - detection and alleviation of stuck bodies
 * - some sort of space ship controller
 *
 */



struct NearestObjectQuery {
  static const int nothingFound = 0, notTouching = 1, touching = 2;
  Body *body = NULL;
  BodyPart *bodyPart = NULL;
  cpShape *chipShape = NULL;
  int result = 0;
  double distance;
  Vec2d queryPoint;
  Vec2d closestPoint;
  Vec2d pointOnTheShape;
  Vec2d gradient;
};


struct CollisionGroup {
  std::vector<Body*> bodies;
  unsigned int id;
  BoundingRect br;
  GeomRenderer::RenderingSettings renderingSettings;

  void setDefaultRenderingSettings() {
    renderingSettings.fillColor.set(0.3, 0.1, 0.15, 0.3);
    renderingSettings.strokeColor.set(0.15, 0.05, 0.07, 0.6);
    renderingSettings.strokeType = 1;
    renderingSettings.strokeWidth = 7;
  }

  CollisionGroup(std::vector<Body*> bodies, int id) {
    this->bodies = bodies;
    set(id);
  }
  CollisionGroup(Body* body, int id) {
    bodies.push_back(body);
    set(id);
  }
  void set(int id) {
    this->id = id;
    updateBoundingRect();
    setDefaultRenderingSettings();
    for(Body *body : bodies) {
      body->setCollisionGroup(id);
    }
  }

  void addBody(Body *body) {
    bodies.push_back(body);
    updateBoundingRect();
    body->setCollisionGroup(id);
  }

  void removeBody(Body *body) {
    for(int i=0; i<bodies.size(); i++) {
      if(body == bodies[i]) {
        body->setCollisionGroup(0);
        bodies.erase(bodies.begin() + i);
        i--;
      }
    }
    updateBoundingRect();
  }

  bool containsBody(Body *body) {
    for(int i=0; i<bodies.size(); i++) {
      if(body == bodies[i]) {
        return true;
      }
    }
    return false;
  }

  bool isOwner(Body *body) {
    return bodies.size() > 0 && body == bodies[0];
  }
  bool isEmpty() {
    return bodies.size() == 0;
  }

  void clear() {
    for(int i=0; i<bodies.size(); i++) {
      bodies[i]->setCollisionGroup(0);
    }
    bodies.clear();
  }

  void updateBoundingRect() {
    if(bodies.size() > 0) {
      br = bodies[0]->getBoundingRect();
      for(int i=1; i<bodies.size(); i++) {
        br.expand(bodies[i]->getBoundingRect());
      }
    }
  }

  void render(GeomRenderer &geomRenderer) {
    geomRenderer.setSettings(renderingSettings);
    geomRenderer.drawRect(br);
  }
};




static void calculateImpulsesFunc(cpArbiter *arb, cpSpace *space, void *data) {
  cpShape *a, *b;
  cpArbiterGetShapes(arb, &a, &b);

  if(a && b) {
    BodyPart *bodyPartA = dynamic_cast<BodyPart*>((BodyPart*)cpShapeGetUserData(a));
    BodyPart *bodyPartB = dynamic_cast<BodyPart*>((BodyPart*)cpShapeGetUserData(b));
    Impulse imp;
    Vec2d v = ctov(cpArbiterTotalImpulse(arb));
    imp.impulse = v;
    imp.impulseMagnitude = v.length();
    imp.kineticEnergyLost = cpArbiterTotalKE(arb);

    bodyPartA->currentImpulse.add(imp);
    bodyPartB->currentImpulse.add(imp);
    /*bodyPartA->parentBody->currentImpulses.push_back(imp);
    bodyPartB->parentBody->currentImpulses.push_back(imp);*/
  }
}



struct Simulation
{
  cpSpace *chipSpace = NULL;
  std::vector<Body*> bodies;
  std::vector<Constraint*> constraints;

  std::vector<Composite*> composites;

  Body *defaultStaticBody = NULL;

  bool renderBoundingRects = false;
  bool renderContactInfo = false;
  bool renderConstraints = false;

  std::vector<CollisionGroup*> collisionGroups;
  int collisionGroupIDCounter = 1;

  double timeStep = 1.0/60.0;

  int numExtraIterations = 0;

  double densityDefault = 0.001, elasticityDefault = 0.5, frictionDefault = 0.5;
  std::string objectNameDefault = "handmade";
  cpShapeFilter chipCollisionFilterDefault = { 0, 1, (unsigned int)-1 };


  bool simulationPaused = false;

  std::unordered_map<int, InputInterface*> inputInterfaces;

  virtual ~Simulation() {}

  Simulation() {}


  void addComposite(Composite *composite) {
    composites.push_back(composite);
  }

  Composite *createComposite(Body *body) {
    Composite *composite = new Composite(defaultStaticBody);
    addToComposite(composite, body);
    composites.push_back(composite);
    return composite;
  }

  bool addToComposite(Composite *composite, Body *body) {
    if(contains(composite->bodies, body)) return false;

    composite->bodies.push_back(body);

    for(Constraint *constraint : constraints) {
      if(contains(composite->constraints, constraint)) continue;

      if(constraint->bodyA == body || constraint->bodyB == body) {
        composite->constraints.push_back(constraint);

        if(constraint->bodyB != body && constraint->bodyB != defaultStaticBody) {
        //if(constraint->bodyB != body) {
          if(!contains(composite->bodies, constraint->bodyB)) {
            addToComposite(composite, constraint->bodyB);
          }
        }
        if(constraint->bodyA != body && constraint->bodyA != defaultStaticBody) {
        //if(constraint->bodyA != body) {
          if(!contains(composite->bodies, constraint->bodyA)) {
            addToComposite(composite, constraint->bodyA);
          }
        }
      }
    }
    return true;
  }

  void removeComposite(Composite *composite) {
    for(int i=0; i<composites.size(); i++) {
      if(composites[i] == composite) {
        composites.erase(composites.begin() + i);
        delete composite;
        return;
      }
    }
  }



  Composite *replicateComposite(Composite *composite) {
    Composite *compositeReplicate = NULL;

    if(composite->type == Composite::genProg) {
      compositeReplicate = new GenProgComposite(composite);
    }
    else {
      compositeReplicate = new Composite(defaultStaticBody);
    }

    for(Body *body : composite->bodies) {
      compositeReplicate->bodies.push_back(replicateBody(body));
    }

    for(Constraint *constraint : composite->constraints) {
      if(constraint->type == Constraint::faceEachOtherConstraint) continue;
      Body *bodyA = NULL, *bodyB = NULL;
      int a = -1, b = -1;
      for(int i=0; i<composite->bodies.size(); i++) {
        if(constraint->bodyA == composite->bodies[i]) a = i;
        if(constraint->bodyB == composite->bodies[i]) b = i;
      }

      if(a == -1 && constraint->bodyA == defaultStaticBody && b >= 0) {
        bodyA = defaultStaticBody;
        bodyB = compositeReplicate->bodies[b];
      }
      if(b == -1 && constraint->bodyB == defaultStaticBody && a >= 0) {
        bodyB = defaultStaticBody;
        bodyA = compositeReplicate->bodies[a];
      }
      if(a >= 0 && b >= 0) {
        bodyA = compositeReplicate->bodies[a];
        bodyB = compositeReplicate->bodies[b];
      }
      if(bodyA && bodyB) {
        //printf("%d, %d\n", a, b);
        compositeReplicate->constraints.push_back(replicateConstraint(constraint, bodyA, bodyB));
      }
    }

    return compositeReplicate;
  }

  std::vector<Composite*> replicateCompositeGrid(Composite *composite, int nx, int ny, Vec2d dp) {
    std::vector<Composite*> compositeReplicates;

    BoundingRect br = composite->getBoundingRect();
    dp.add(br.getWidth(), br.getHeight());
    dp.y = -dp.y;

    bool pyramid = ny <= 0;
    if(pyramid) {
      for(int j=0; j<nx; j++) {
        for(int i=0; i<nx-j; i++) {
          if(i == 0 && j == 0) continue;
          Composite *replicate = replicateComposite(composite);
          replicate->translate(dp * Vec2d(0.5*j+i, j));
          compositeReplicates.push_back(replicate);
        }
      }
    }
    else {
      for(int i=0; i<nx; i++) {
        for(int j=0; j<ny; j++) {
          if(i == 0 && j == 0) continue;
          Composite *replicate = replicateComposite(composite);
          replicate->translate(dp * Vec2d(i, j));
          compositeReplicates.push_back(replicate);
        }
      }
    }
    return compositeReplicates;
  }

  void init(double g = 100, int numIterations = 20, double sleepTime = 0.5, double slop = 0.1) {
    chipSpace = cpSpaceNew();
    cpSpaceSetIterations(chipSpace, numIterations);
    cpSpaceSetGravity(chipSpace, cpv(0, g));
    cpSpaceSetSleepTimeThreshold(chipSpace, sleepTime);
    cpSpaceSetCollisionSlop(chipSpace, slop);
    cpSpaceSetDamping(chipSpace, 1);
    //cpSpaceSetIdleSpeedThreshold(cpSpace *space, cpFloat value)
    //cpSpaceSetCollisionBias(cpSpace *space, cpFloat value)
    //cpSpaceSetCollisionPersistence(cpSpace *space, cpTimestamp value) // no. of frames collision data keeped
    //cpSpaceSetUserData(cpSpace *space, cpDataPointer value)
    defaultStaticBody = new Body();
    defaultStaticBody->init(chipSpace, -1);

    cpCollisionHandler *handler = cpSpaceAddWildcardHandler(chipSpace, 1);
		handler->postSolveFunc = calculateImpulsesFunc;
  }

  /*
  	typedef int (*cpCollisionBeginFunc)(cpArbiter *arb, struct cpSpace *space, cpDataPointer data)
  	typedef int (*cpCollisionPreSolveFunc)(cpArbiter *arb, cpSpace *space, cpDataPointer data)
  	typedef void (*cpCollisionPostSolveFunc)(cpArbiter *arb, cpSpace *space, cpDataPointer data)
  	typedef void (*cpCollisionSeparateFunc)(cpArbiter *arb, cpSpace *space, cpDataPointer data)

  	struct cpCollisionHandler {
      cpCollisionType typeA, typeB;
      cpCollisionBeginFunc beginFunc;
      cpCollisionPreSolveFunc preSolveFunc;
      cpCollisionPostSolveFunc postSolveFunc;
      cpCollisionSeparateFunc separateFunc;
      cpDataPointer userData;
  };*/

	/*void f() {
		cpCollisionHandler *handler = cpSpaceAddCollisionHandler(chipSpace, 1, 1);
		handler->postSolveFunc = ;
	}*/

  void update(double dt) {
    int n = 1+numExtraIterations;
    if(timeStep > 0) dt = timeStep;
    dt = dt / n;
    for(int i=0; i<n; i++) {

      for(auto &intInputInterfacePair : inputInterfaces) {
        intInputInterfacePair.second->onUpdate(dt);
      }
      for(Body *body : bodies) {
        body->clearCurrentImpulses();
      }

      cpSpaceStep(chipSpace, dt);

      for(Body *body : bodies) {
        // body->onUpdate(dt);
        // TODO do something with the impulses, crash stuff, etc.
      }

      for(Composite *composite : composites) {
        composite->onUpdate(dt);
      }
    }

    // Things go wrong when removing and adding things back to space, this solves the problem
    if(collisionIgnoreHackActive) {
      collisionIgnoreHackActive = false;
      cpCollisionHandler *handler = cpSpaceAddDefaultCollisionHandler(chipSpace);
      handler->beginFunc = doNotIgnoreCollisionFunc;
    }

    /*for(Constraint *constraint : constraints) {
      if(constraint->type == Constraint::constantAngleConstraint) {
        ConstantAngleConstraint *constConstraint = dynamic_cast<ConstantAngleConstraint*>(constraint);
        if(constConstraint->faceEachOther) {
          Vec2d a = constConstraint->bodyA->getPosition();
          Vec2d b = constConstraint->bodyB->getPosition();
          double da2 = atan2(b.y-a.y, b.x-a.x);
          //da2 = -constConstraint->angle + da2;
          constConstraint->bodyA->rotate(-constConstraint->bodyA->getAngle()+da2);
          constConstraint->bodyB->rotate(-constConstraint->bodyB->getAngle()+da2);
          constConstraint->bodyA->requestBodyUpdate();
          constConstraint->bodyB->requestBodyUpdate();
        }
      }
    }*/

    /*for(Body *body : bodies) {
      if(body->bodyUpdateRequested) {
        body->updateBody();
      }
    }*/
  }

  void setNumIterations(int numIterations) {
    cpSpaceSetIterations(chipSpace, numIterations);
  }
  void setGravity(double x, double y) {
    cpSpaceSetGravity(chipSpace, cpv(x, y));
  }
  Vec2d getGravity() {
    return ctov(cpSpaceGetGravity(chipSpace));
  }

  void setSleepTimeThreshold(double time) {
    cpSpaceSetSleepTimeThreshold(chipSpace, time);
  }

  void setCollisionSlop(double slop) {
    cpSpaceSetCollisionSlop(chipSpace, slop);
  }

  void setDamping(double damping) {
    cpSpaceSetDamping(chipSpace, damping);
  }

  void setTimeStep(double timeStep) {
    this->timeStep = timeStep;
  }

  void setSimulationPaused(bool simulationPaused) {
    this->simulationPaused = simulationPaused;
  }


  // TODO somehow replicate the pattern of constraints?
  Body *replicateBody(Body *body, const Vec2d &dp, double rot = 0) {
    Body *replicate = new Body(*body);
    replicate->translate(dp);
    replicate->rotate(rot);
    bodies.push_back(replicate);
    return replicate;
  }

  Body *replicateBody(Body *body) {
    Body *replicate = new Body(*body);
    bodies.push_back(replicate);
    return replicate;
  }

  std::vector<Body*> replicateBodyGrid(Body *body, int nx, int ny, const Vec2d &pos, Vec2d dp) {
    std::vector<Body*> replicates;
    if(dp == Vec2d::Zero) {
      BoundingRect br = body->getBoundingRect();
      dp.set(br.getWidth(), -br.getHeight());
    }
    bool pyramid = ny <= 0;
    if(pyramid) {
      for(int j=0; j<nx; j++) {
        for(int i=0; i<nx-j; i++) {
          Body *replicate = new Body(*body);
          replicate->setPosition(pos + dp * Vec2d(0.5*j+i, j));
          bodies.push_back(replicate);
          replicates.push_back(replicate);
        }
      }
    }
    else {
      for(int i=0; i<nx; i++) {
        for(int j=0; j<ny; j++) {
          Body *replicate = new Body(*body);
          replicate->setPosition(pos + dp * Vec2d(i, j));
          bodies.push_back(replicate);
          replicates.push_back(replicate);
        }
      }
    }
    return replicates;
  }

  std::vector<Body*> replicateBodyRing(Body *body, int n, const Vec2d &ringCenter, double r, double da, bool fixedAngle = true) {
    std::vector<Body*> replicates;
    for(int i=0; i<n; i++) {
      double a = da * i;
      Body *replicate = new Body(*body);
      Vec2d t(r * cos(a), r * sin(a));
      replicate->setPosition(ringCenter + t);
      if(!fixedAngle) {
        replicate->setAngle(body->getAngle() + a);
      }
      bodies.push_back(replicate);
      replicates.push_back(replicate);
    }
    return replicates;
  }

  Body *seperateBodyPartToNewBody(Body *body, BodyPart *bodyPart) {
    bool contains = false;

    Body *newBody = addBody(body->bodyType);
    newBody->addBodyPart(bodyPart);
    newBody->setPosition(body->getPosition());
    newBody->setAngle(body->getAngle());

    for(int i=0; i<body->bodyParts.size(); i++) {
      if(body->bodyParts[i] == bodyPart) {
        //body->bodyParts.erase(body->bodyParts.begin() + i);
        body->removeBodyPart(bodyPart);
        contains = true;
        break;
      }
    }
    if(!contains) return NULL;

    return newBody;
  }

  void combineBodies(Body *bodyA, Body *bodyB) {
    for(BodyPart *bodyPart : bodyB->bodyParts) {
      bodyA->addBodyPart(bodyPart);
    }
    removeBody(bodyB);
  }



  Body *addBody(int bodyType, GeomObject *geomObject = NULL, double density = -1, double elasticity = -1, double friction = -1) {
    density = density >= 0 ? density : densityDefault;
    elasticity = elasticity >= 0 ? elasticity : elasticityDefault;
    friction = friction >= 0 ? friction : frictionDefault;

    Body *body = new Body();
    body->init(chipSpace, bodyType, !simulationPaused);
    body->name = objectNameDefault;
    body->setDefaults(density, elasticity, friction, chipCollisionFilterDefault);
    if(geomObject) {
      body->addBodyPart(geomObject);
    }
    bodies.push_back(body);
    return body;
  }

  int getNumShapesInSpace() {
    int n = 0;
    for(Body *body : bodies) {
      n += body->getNumShapesInSpace();
    }
    return n;
  }

  void removeFromSpace(Body *body) {
    body->removeFromSpace();
    for(int i=0; i<constraints.size(); i++) {
      if(constraints[i]->bodyA == body || constraints[i]->bodyB == body) {
        constraints[i]->removeFromSpace();
      }
    }
  }

  void putBackToSpace(Body *body) {
    body->putBackToSpace();
    for(int i=0; i<constraints.size(); i++) {
      if(constraints[i]->bodyA == body || constraints[i]->bodyB == body) {
        constraints[i]->putBackToSpace();
      }
    }
  }
  void removeEverythingFromSpace() {
    cpCollisionHandler *handler = cpSpaceAddDefaultCollisionHandler(chipSpace);
    handler->beginFunc = ignoreCollisionFunc;
    collisionIgnoreHackActive = true;

    for(Body *body : bodies) {
      body->removeFromSpace();
    }
    for(Constraint *constraint : constraints) {
      constraint->removeFromSpace();
    }
  }
  bool collisionIgnoreHackActive = false;

  void putEverythingBackToSpace() {

    for(Body *body : bodies) {
      body->putBackToSpace();
    }
    for(Constraint *constraint : constraints) {
      constraint->putBackToSpace();
    }
  }

  Constraint *replicateConstraint(Constraint *constraint, Body *bodyA, Body *bodyB) {
    Constraint *replicateConstraint = NULL;

    if(constraint->type == Constraint::pivotJoint) {
      replicateConstraint = new PivotJoint(dynamic_cast<PivotJoint*>(constraint), chipSpace, bodyA, bodyB);
    }
    if(constraint->type == Constraint::simpleMotor) {
      replicateConstraint = new SimpleMotor(dynamic_cast<SimpleMotor*>(constraint), chipSpace, bodyA, bodyB);
    }
    if(constraint->type == Constraint::rotaryLimitConstraint) {
      replicateConstraint = new RotaryLimitConstraint(dynamic_cast<RotaryLimitConstraint*>(constraint), chipSpace, bodyA, bodyB);
    }
    if(constraint->type == Constraint::constantAngleConstraint) {
      replicateConstraint = new ConstantAngleConstraint(dynamic_cast<ConstantAngleConstraint*>(constraint), chipSpace, bodyA, bodyB);
    }
    if(constraint->type == Constraint::faceEachOtherConstraint) {
      replicateConstraint = new FaceEachOtherConstraint(dynamic_cast<FaceEachOtherConstraint*>(constraint), chipSpace, bodyA, bodyB, defaultStaticBody);
    }
    if(constraint->type == Constraint::grooveJoint) {
      replicateConstraint = new GrooveJoint(dynamic_cast<GrooveJoint*>(constraint), chipSpace, bodyA, bodyB);
    }
    if(constraint->type == Constraint::dampedSpring) {
      replicateConstraint = new DampedSpring(dynamic_cast<DampedSpring*>(constraint), chipSpace, bodyA, bodyB);
    }

    if(replicateConstraint) {
      constraints.push_back(replicateConstraint);
    }

    return replicateConstraint;
  }

  PivotJoint *addPivotJoint(Body *bodyA, Body *bodyB, const Vec2d &anchor) {
    PivotJoint *pivotJoint = new PivotJoint(chipSpace, bodyA, bodyB ? bodyB : defaultStaticBody);
    pivotJoint->setAnchorsWorld(anchor);
    constraints.push_back(pivotJoint);
    return pivotJoint;
  }
  SimpleMotor *addSimpleMotor(Body *bodyA, Body *bodyB) {
    SimpleMotor *simpleMotor = new SimpleMotor(chipSpace, bodyA, bodyB ? bodyB : defaultStaticBody);
    constraints.push_back(simpleMotor);
    return simpleMotor;
  }
  RotaryLimitConstraint *addRotaryLimitConstraint(Body *bodyA, Body *bodyB) {
    bodyB = bodyB ? bodyB : defaultStaticBody;
    double a = bodyB->getAngle() - bodyA->getAngle();
    RotaryLimitConstraint *rotaryLimitConstraint = new RotaryLimitConstraint(chipSpace, bodyA, bodyB, a, a);
    constraints.push_back(rotaryLimitConstraint);
    return rotaryLimitConstraint;
  }
  ConstantAngleConstraint *addConstantAngleConstraint(Body *bodyA, Body *bodyB) {
    ConstantAngleConstraint *cac = new ConstantAngleConstraint(chipSpace, bodyA, bodyB ? bodyB : defaultStaticBody);
    constraints.push_back(cac);
    return cac;
  }
  FaceEachOtherConstraint *addFaceEachOtherConstraint(Body *bodyA, Body *bodyB) {
    FaceEachOtherConstraint *feoc = new FaceEachOtherConstraint(chipSpace, bodyA, bodyB, defaultStaticBody);
    constraints.push_back(feoc);
    return feoc;
  }
  GrooveJoint *addGrooveJoint(Body *bodyA, Body *bodyB, const Vec2d &grooveA, const Vec2d &grooveB) {
    GrooveJoint *grooveJoint = new GrooveJoint(chipSpace, bodyB ? bodyB : defaultStaticBody, bodyA, grooveA, grooveB);
    constraints.push_back(grooveJoint);
    return grooveJoint;
  }
  DampedSpring *addDampedSpring(Body *bodyA, Body *bodyB, const Vec2d &anchorA, const Vec2d &anchorB) {
    DampedSpring *dampedSpring = new DampedSpring(chipSpace, bodyA, bodyB ? bodyB : defaultStaticBody, anchorA, anchorB, distance(anchorA, anchorB), 200, 10);
    constraints.push_back(dampedSpring);
    return dampedSpring;
  }




  void setSimpleMotorController(int index, bool on, Constraint *constraint, int reverseKey, int forwardKey) {
    SimpleMotorController *simpleMotorController = NULL;

    if(inputInterfaces.count(index) > 0) {
      simpleMotorController = dynamic_cast<SimpleMotorController*>(inputInterfaces[index]);
    }
    else {
      simpleMotorController = new SimpleMotorController();
      inputInterfaces[index] = simpleMotorController;
    }
    if(simpleMotorController) {
      if(on && constraint) {
        simpleMotorController->set(constraint, forwardKey, reverseKey);
      }
      else {
        simpleMotorController->unset();
        inputInterfaces.erase(index);
      }
    }
  }

  void setSimpleCharacterController(int index, bool on, Body *body, BodyPart *foot, double maxLateralSpeed, double lateralForce, double jumpForce, int leftKey, int rightKey, int jumpKey) {
    SimpleCharacterController *simpleCharacterController = NULL;

    if(inputInterfaces.count(index) > 0) {
      simpleCharacterController = dynamic_cast<SimpleCharacterController*>(inputInterfaces[index]);
    }
    else {
      simpleCharacterController = new SimpleCharacterController();
      inputInterfaces[index] = simpleCharacterController;
    }
    if(simpleCharacterController) {
      if(on && body && foot) {
        simpleCharacterController->set(body, foot, maxLateralSpeed, lateralForce, jumpForce, leftKey, rightKey, jumpKey);
      }
      else {
        simpleCharacterController->unset();
        inputInterfaces.erase(index);
      }
    }
  }

  void onKeyUp(Events &events) {
    for(auto &intInputInterfacePair : inputInterfaces) {
      intInputInterfacePair.second->onKeyUp(events);
    }
  }

  void onKeyDown(Events &events) {
    for(auto &intInputInterfacePair : inputInterfaces) {
      intInputInterfacePair.second->onKeyDown(events);
    }
  }

  void getNearestObject(const Vec2d &p, NearestObjectQuery &nearestObjectQuery, cpShapeFilter chipFilter = collisionFilterDefault, double maxDistance = 999999) {
    cpPointQueryInfo info;

    if(bodies.size() == 0 || getNumShapesInSpace() == 0) {
      nearestObjectQuery.result = NearestObjectQuery::nothingFound;
      nearestObjectQuery.body = NULL;
      nearestObjectQuery.bodyPart = NULL;
      nearestObjectQuery.chipShape = NULL;
      return;
    }

    bool found = cpSpacePointQueryNearest(chipSpace, vtoc(p), maxDistance, chipFilter, &info);
    nearestObjectQuery.queryPoint = p;
    BodyPart *bodyPart = dynamic_cast<BodyPart*>((BodyPart*)cpShapeGetUserData(info.shape));


    if(!found || !info.shape || !bodyPart) {
      nearestObjectQuery.result = NearestObjectQuery::nothingFound;
      nearestObjectQuery.body = NULL;
      nearestObjectQuery.bodyPart = NULL;
      nearestObjectQuery.chipShape = NULL;
    }
    else {
      nearestObjectQuery.bodyPart = bodyPart;
      nearestObjectQuery.body = nearestObjectQuery.bodyPart->parentBody;
      nearestObjectQuery.chipShape = (cpShape*)info.shape;
      nearestObjectQuery.distance = info.distance;
      nearestObjectQuery.pointOnTheShape = ctov(info.point);
      nearestObjectQuery.closestPoint = info.distance <= 0 ? p :nearestObjectQuery.pointOnTheShape;
      nearestObjectQuery.gradient = ctov(info.gradient);
      if(info.distance <= 0) nearestObjectQuery.result = NearestObjectQuery::touching;
      else nearestObjectQuery.result = NearestObjectQuery::notTouching;
    }
  }


  // FIXME
  void onBodyUpdate() {
    for(Constraint *constraint : constraints) {
      constraint->onBodyUpdate();
    }
  }


  void render(GeomRenderer &geomRenderer) {
    if(renderBoundingRects) {
      for(Body *body : bodies) {
        body->renderBoundingRect(geomRenderer);
      }
    }
    for(Body *body : bodies) {
      body->render(geomRenderer);
    }
    if(renderConstraints) {
      for(Constraint *constraint : constraints) {
        constraint->render(geomRenderer);
      }
      for(Composite *composite : composites) {
        composite->render(geomRenderer);
      }
    }

    int numCollisionPairs = chipSpace->arbiters->num;
    if(renderContactInfo) {
      for(int i=0; i<numCollisionPairs; i++) {
        CollisionPair pair((cpArbiter*)chipSpace->arbiters->arr[i]);
        for(int i=0; i<pair.numPoints; i++) {
          geomRenderer.strokeType = 0;
          geomRenderer.fillColor.set(0.5, 0.5, 0.5, 0.5);
          geomRenderer.drawCircle(3, pair.points[i]);
        }
      }
    }
  }


  void addCollisionGroup(const std::vector<Body*> &bodies) {
    if(bodies.size() > 0) {
      collisionGroups.push_back(new CollisionGroup(bodies, collisionGroupIDCounter));
      collisionGroupIDCounter++;
    }
  }
  void addCollisionGroup(Body *body) {
    collisionGroups.push_back(new CollisionGroup(body, collisionGroupIDCounter));
    collisionGroupIDCounter++;
  }

  void removeCollisionGroup(Body *body) {
    for(int i=0; i<collisionGroups.size(); i++) {
      if(collisionGroups[i]->containsBody(body)) {
        collisionGroups[i]->clear();
        collisionGroups.erase(collisionGroups.begin()+i);
        i--;
      }
    }
  }
  void removeCollisionGroup(CollisionGroup *collisionGroup) {
    for(int i=0; i<collisionGroups.size(); i++) {
      if(collisionGroups[i] == collisionGroup) {
        collisionGroups[i]->clear();
        collisionGroups.erase(collisionGroups.begin()+i);
        i--;
      }
    }
  }
  CollisionGroup *getCollisionGroup(Body *body) {
    for(int i=0; i<collisionGroups.size(); i++) {
      if(collisionGroups[i]->containsBody(body)) {
        return collisionGroups[i];
      }
    }
    return NULL;
  }


  // FIXME time to use hash maps or something to get rid of those loops...
  int removeBody(Body *body) {

    for(auto it = inputInterfaces.begin(); it != inputInterfaces.end(); ) {
      bool match = it->second->onBodyRemoval(body);
      if(match) it = inputInterfaces.erase(it);
      else it++;
    }

    for(int i=0; i<composites.size(); i++) {
      if(composites[i]->containsBody(body)) {
        removeComposite(composites[i]);
        i--;
      }
    }

    int numRemoved = 0;
    for(int i=0; i<bodies.size(); i++) {
      if(bodies[i] == body)  {
        for(int k=0; k<constraints.size(); k++) {
          if(body == constraints[k]->bodyA || body == constraints[k]->bodyB) {
            constraints[k]->remove();
            delete constraints[k];
            constraints.erase(constraints.begin() + k);
            k--;
          }
        }
        for(int k=0; k<collisionGroups.size(); k++) {
          if(collisionGroups[k]->containsBody(body)) {
            collisionGroups[k]->removeBody(body);
            if(collisionGroups[k]->bodies.size() == 0) {
              delete collisionGroups[k];
              collisionGroups.erase(collisionGroups.begin() + k);
              k--;
            }
          }
        }
        bodies[i]->remove();
        delete bodies[i];
        bodies.erase(bodies.begin() + i);
        i--;
        numRemoved++;
      }
    }
    return numRemoved;
  }

  void removeConstraint(Constraint *constraint) {
    for(auto it = inputInterfaces.begin(); it != inputInterfaces.end(); ) {
      bool match = it->second->onConstraintRemoval(constraint);
      if(match) it = inputInterfaces.erase(it);
      else it++;
    }

    for(int i=0; i<composites.size(); i++) {
      if(composites[i]->containsConstraint(constraint)) {
        removeComposite(composites[i]);
        i--;
      }
    }

    for(int i=0; i<constraints.size(); i++) {
      if(constraints[i] == constraint)  {
        constraints[i]->remove();
        delete constraints[i];
        constraints.erase(constraints.begin() + i);
        i--;
      }
    }
  }


  void clearBodies(int bodyType = 0) {
    for(int i=0; i<bodies.size(); i++) {
      if(bodies[i]->bodyType == bodyType || bodyType < 0)  {
        i -= removeBody(bodies[i]);
      }
    }
  }
  void clearBodies(const std::string &name) {
    for(int i=0; i<bodies.size(); i++) {
      if(bodies[i]->name.compare(name) ==  0)  {
        i -= removeBody(bodies[i]);
      }
    }
  }

  void clearConstraints() {
    for(int i=0; i<constraints.size(); i++) {
      constraints[i]->remove();
      delete constraints[i];
      constraints.erase(constraints.begin() + i);
      i--;
    }
  }

  void finalize() {
    clearBodies();
    defaultStaticBody->remove();
    clearConstraints();
    cpSpaceFree(chipSpace);
  }



};
