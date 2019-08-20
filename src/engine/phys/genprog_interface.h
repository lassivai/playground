#pragma once
#include "chipphys.h"
#include "bodypart.h"
#include "collisionpair.h"
#include "body.h"
#include "constraint.h"
#include "composite.h"
//#include "simulation.h"

/* TODO
 * - replicate and position the population
 * - reset object state to the original
 * - balance and impulse scorings
 *
 *
 *
 *
 *
 */


struct GenProgInterface {

  virtual ~GenProgInterface() {}

  virtual bool onConstraintRemoval(Constraint *constraints) {
    return false;
  }
  virtual bool onBodyRemoval(Body *body) {
    return false;
  }
  virtual bool onBodyPartRemoval(BodyPart *bodyPart) {
    return false;
  }
  virtual void onUpdate(double dt) {}
  virtual std::string toString() = 0;
};



struct SimpleMotorGenProgInterface : public GenProgInterface {
  SimpleMotor *simpleMotor = NULL;
  bool reverseOn = false, forwardOn = false;

  virtual ~SimpleMotorGenProgInterface() {}

  void set(Constraint *constraint) {
    simpleMotor = dynamic_cast<SimpleMotor*>(constraint);
    simpleMotor->scaleRate(0.0);
    simpleMotor->scaleMaxForce(0.0);
    forwardOn = false;
    reverseOn = false;
  }

  void unset() {
    if(simpleMotor) {
      simpleMotor->scaleRate(1.0);
      simpleMotor->scaleMaxForce(1.0);
    }
  }

  void reverse(double rateFactor, double forceFactor) {
    simpleMotor->scaleRate(-rateFactor);
    simpleMotor->scaleMaxForce(forceFactor);
    reverseOn = true;
    forwardOn = false;
  }

  void forward(double rateFactor, double forceFactor) {
    simpleMotor->scaleRate(rateFactor);
    simpleMotor->scaleMaxForce(forceFactor);
    forwardOn = true;
    reverseOn = false;
  }

  void stop() {
    simpleMotor->scaleRate(0.0);
    simpleMotor->scaleMaxForce(0.0);
    forwardOn = false;
    reverseOn = false;
  }

  bool onConstraintRemoval(Constraint *constraint) {
    if(dynamic_cast<SimpleMotor*>(constraint) == simpleMotor) {
      unset();
      return true;
    }
    return false;
  }

  std::string toString() {
    std::string str = "simple motor genprog interface";
    return  str;
  }
};



struct GenProgLimbInterface : public GenProgInterface {
  ConstantAngleConstraint *constantAngleConstraint = NULL;
  RotaryLimitConstraint *rotaryLimitConstraint = NULL;
  //double minAngle, maxAngle;
  double maxForce, maxSpeed;
  double restAngle;

  virtual ~GenProgLimbInterface() {}

  GenProgLimbInterface() {}

  GenProgLimbInterface(ConstantAngleConstraint *constantAngleConstraint, RotaryLimitConstraint *rotaryLimitConstraint) {
    set(constantAngleConstraint, rotaryLimitConstraint);
  }

  void set(ConstantAngleConstraint *constantAngleConstraint, RotaryLimitConstraint *rotaryLimitConstraint) {
    this->constantAngleConstraint = constantAngleConstraint;
    this->rotaryLimitConstraint = rotaryLimitConstraint;
    maxSpeed = constantAngleConstraint->speed;
    maxForce = constantAngleConstraint->getMaxForce();
    restAngle = constantAngleConstraint->angle;
  }

  void unset() {
    if(constantAngleConstraint) {
      constantAngleConstraint->setAngle(restAngle);
      constantAngleConstraint->setMaxForce(maxForce);
      constantAngleConstraint->setSpeed(maxSpeed);
    }
  }

  void setAngle(double angleFactor, double speedFactor, double forceFactor) {
    constantAngleConstraint->setAngle(map(angleFactor, 0.0, 1.0, rotaryLimitConstraint->min, rotaryLimitConstraint->max));
    constantAngleConstraint->setMaxForce(maxForce * forceFactor);
    constantAngleConstraint->setSpeed(maxSpeed * speedFactor);
  }


  bool onConstraintRemoval(Constraint *constraint) {
    if(dynamic_cast<ConstantAngleConstraint*>(constraint) == constantAngleConstraint || dynamic_cast<RotaryLimitConstraint*>(constraint) == rotaryLimitConstraint) {
      unset();
      return true;
    }
    return false;
  }

  std::string toString() {
    std::string str = "limb genprog interface";
    return  str;
  }
};


struct LimbEvent  {
  double angleFactor, speedFactor, forceFactor;
  double timeStart;//, timeEnd;

  virtual ~LimbEvent() {}

  bool operator<(const LimbEvent &other) {
    return this->timeStart < other.timeStart;
  }
};

struct GenProgLimbMovement
{
  std::vector<GenProgLimbInterface> limbInterfaces;
  std::vector<std::vector<LimbEvent>> limbEvents;

  double totalTime, minTotalTime, maxTotalTime;
  int minNumEvents, maxNumEvents;

  double mutationAmount = 0.01;
  double totalTimeMutationAmount = 0.01;

  double movementStartTime;
  double movementCurrentTime;
  bool active = false;
  int repetitionCount = 1, currentRepetitionIndex = 0;

  virtual ~GenProgLimbMovement() {}

  // FIXME not the fastest way currently
  void fetchLimbs(Composite *composite) {

    for(int i=0; i<composite->bodies.size(); i++) {
      Body *bodyA = composite->bodies[i];

      for(int j=i+1; j<composite->bodies.size(); j++) {
        Body *bodyB = composite->bodies[j];
        ConstantAngleConstraint *constantAngleConstraint = NULL;
        RotaryLimitConstraint *rotaryLimitConstraint = NULL;

        for(int k=0; k<composite->constraints.size(); k++) {
          if((composite->constraints[k]->bodyA == bodyA && composite->constraints[k]->bodyB == bodyB) ||
            (composite->constraints[k]->bodyA == bodyB && composite->constraints[k]->bodyB == bodyA)) {
            if(composite->constraints[k]->type == Constraint::constantAngleConstraint) {
              constantAngleConstraint = dynamic_cast<ConstantAngleConstraint*>(composite->constraints[k]);
            }
            if(composite->constraints[k]->type == Constraint::rotaryLimitConstraint) {
              rotaryLimitConstraint = dynamic_cast<RotaryLimitConstraint*>(composite->constraints[k]);
            }
          }
          if(constantAngleConstraint && rotaryLimitConstraint) {
            limbInterfaces.push_back(GenProgLimbInterface(constantAngleConstraint, rotaryLimitConstraint));
          }
        }
      }
    }
  }

  void setMutationAmounts(double mutationAmount, double totalTimeMutationAmount) {
    this->mutationAmount = mutationAmount;
    this->totalTimeMutationAmount = totalTimeMutationAmount;
  }

  void setInitialConditions(double minTotalTime, double maxTotalTime, int minNumEvents, int maxNumEvents = -1) {
    limbEvents.clear();
    if(minNumEvents < 1) minNumEvents = 1;
    if(maxNumEvents < minNumEvents) maxNumEvents = minNumEvents;
    this->minNumEvents = minNumEvents;
    this->maxNumEvents = maxNumEvents;
    this->minTotalTime = minTotalTime;
    this->maxTotalTime = maxTotalTime;

    randomize();
  }

  void randomize() {
    this->totalTime = Random::getDouble(minTotalTime, maxTotalTime);

    for(int i=0; i<limbInterfaces.size(); i++) {
      limbEvents.push_back(std::vector<LimbEvent>());
      int numEvents = Random::getInt(minNumEvents, maxNumEvents);

      for(int k=0; k<numEvents; k++) {
        LimbEvent le;
        le.angleFactor = Random::getDouble();
        le.speedFactor = Random::getDouble();
        le.forceFactor = Random::getDouble();
        le.timeStart = Random::getDouble() * totalTime;
        limbEvents[i].push_back(le);
      }
      sort(limbEvents[i]);
    }
  }

  void mutate() {
    totalTime *= Random::getDouble(1.0-totalTimeMutationAmount, 1.0+totalTimeMutationAmount);
    totalTime = clamp(totalTime, minTotalTime, maxTotalTime);

    for(int i=0; i<limbEvents.size(); i++) {
      for(int k=0; k<limbEvents[i].size(); k++) {
        limbEvents[i][k].angleFactor *= Random::getDouble(1.0-mutationAmount, 1.0+mutationAmount);
        limbEvents[i][k].speedFactor *= Random::getDouble(1.0-mutationAmount, 1.0+mutationAmount);
        limbEvents[i][k].forceFactor *= Random::getDouble(1.0-mutationAmount, 1.0+mutationAmount);
        limbEvents[i][k].timeStart *= Random::getDouble(1.0-mutationAmount, 1.0+mutationAmount);
        limbEvents[i][k].angleFactor = clamp(limbEvents[i][k].angleFactor, 0.0, 1.0);
        limbEvents[i][k].speedFactor = clamp(limbEvents[i][k].speedFactor, 0.0, 1.0);
        limbEvents[i][k].forceFactor = clamp(limbEvents[i][k].forceFactor, 0.0, 1.0);
        limbEvents[i][k].timeStart = clamp(limbEvents[i][k].timeStart, 0.0, totalTime);
      }
      sort(limbEvents[i]);
    }
  }



  void start(int repetitionCount = 1) {
    movementCurrentTime = 0;
    active = true;
    currentRepetitionIndex = 0;
    this->repetitionCount = repetitionCount;
  }

  void onFinish() {

  }

  void onUpdate(double dt) {
    if(!active) return;

    for(int i=0; i<limbEvents.size(); i++) {
      for(int k=limbEvents[i].size()-1; k>=0; k--) {
        if(limbEvents[i][k].timeStart <= movementCurrentTime) {
          limbInterfaces[i].setAngle(limbEvents[i][k].angleFactor, limbEvents[i][k].speedFactor, limbEvents[i][k].forceFactor);
          break;
        }
      }
    }
    movementCurrentTime += dt;

    if(movementCurrentTime > totalTime) {
      movementCurrentTime = 0;
      currentRepetitionIndex++;
    }

    if(repetitionCount > 0 && currentRepetitionIndex >= repetitionCount) {
      onFinish();
      active = false;
    }
  }

};

struct BodySnapshot {
  Vec2d position, velocity;
  double angle, angularVelocity;
  double distanceFromGround;

  virtual ~BodySnapshot() {}

  BodySnapshot() {}

  BodySnapshot(Body *body) {
    position = body->getPosition();
    velocity = body->getVelocity();
    angle = body->getAngle();
    angularVelocity = body->getAnglularVelocity();
    //distanceFromGround = body->getNearestObject(direction); etc.
  }
};




struct GenProgScoring {
  static const int endScoreMode = 0, minScoreMode = 1, maxScoreMode = 2, averageScoreMode = 3;
  int scoringMode = 0;

  double minScore, maxScore, averageScore, totalScore;
  double startScore, endScore;
  int numMeasurements;
  double score, normalizedScore;
  double scoreWeight = 1;

  BodySnapshot initialBodyState, currentBodyState;

  virtual ~GenProgScoring() {}

  virtual double getScore() {
    return 0;
  }

  double getFinalScore() {
    if(scoringMode == 1) return minScore;
    if(scoringMode == 2) return maxScore;
    if(scoringMode == 3) return averageScore;
    return endScore;
  }

  void start(const BodySnapshot &initialBodyState) {
    this->initialBodyState = initialBodyState;
    this->currentBodyState = initialBodyState;
    double q = getScore();
    startScore = q;
    minScore = q;
    maxScore = q;
    totalScore = q;
    endScore = q;
    averageScore = q;
    score = q;
    normalizedScore = 0;
    numMeasurements++;
  }

  void update(const BodySnapshot &currentBodyState) {
    this->currentBodyState = currentBodyState;
    double q = getScore();
    minScore = min(minScore, q);
    maxScore = max(maxScore, q);
    totalScore += q;
    endScore = q;
    numMeasurements++;
    averageScore = totalScore / numMeasurements;
    score = getFinalScore(); // FIXME
  }
};

struct GenProgVelocityScoring : public GenProgScoring {
  Vec2d expectedVelocity;
  bool negation;

  virtual ~GenProgVelocityScoring() {}

  GenProgVelocityScoring(const Vec2d &expectedVelocity, bool negation = false) {
    this->expectedVelocity = expectedVelocity;
    this->negation = negation;
  }

  double getScore() {
    double q = dot(currentBodyState.velocity, expectedVelocity) - dot(initialBodyState.velocity, expectedVelocity);
    return negation ? -q : q;
  }
};

struct GenProgBalanceScoring : public GenProgScoring {
  bool negation;

  bool useEndScoreOnly = false;

  virtual ~GenProgBalanceScoring() {}

  GenProgBalanceScoring(bool useEndScoreOnly, bool negation = false) {
    this->useEndScoreOnly = useEndScoreOnly;
    this->negation = negation;
  }

  double getScore() {
    // TODO Check if the center of mass is within the collision points supporting the body/composite
    double q = 1;
    return negation ? -q : q;
  }
};

struct GenProgImpulseScoring : public GenProgScoring {
  bool negation;
  bool useMaxImpulse = false;

  std::vector<Impulse> impulses;

  virtual ~GenProgImpulseScoring() {}

  GenProgImpulseScoring(bool useMaxImpulse, bool negation = false) {
    this->negation = negation;
    this->useMaxImpulse = useMaxImpulse;
  }

  /*double getScore(const BodySnapshot &initialBodyState, const BodySnapshot &currentBodyState) {
    // TODO calculate the max and total impulses
    double q = 1;
    return negation ? -q : q;
  }*/

  double getScore() {
    // TODO calculate the max and total impulses
    double q = 1;
    return negation ? -q : q;
  }

};

struct GenProgKineticEnergyScoring : public GenProgScoring {
  bool negation;

  virtual ~GenProgKineticEnergyScoring() {}

  GenProgKineticEnergyScoring(bool negation = false) {
    this->negation = negation;
  }

  double getScore() {
    // TODO calculate the both directional and rotational kinetic energies
    double q = 1;
    return negation ? -q : q;
  }
};




struct GenProgDistanceScoring : public GenProgScoring {
  static const int vectorDistance = 0, xDistance = 1, yDistance = 2, xDistanceLeft = 3, xDistanceRight = 4, yDistanceDown = 5, yDistanceUp = 6;
  int mode;
  bool negation;

  virtual ~GenProgDistanceScoring() {}

  GenProgDistanceScoring(int mode = 0, bool negation = false) {
    this->mode = mode;
    this->negation = negation;
  }

  double getScore() {
    double q = 0;
    if(mode == vectorDistance) {
      q = distance(initialBodyState.position, currentBodyState.position);
    }
    if(mode == xDistance) {
      q = fabs(currentBodyState.position.x - initialBodyState.position.x);
    }
    if(mode == yDistance) {
      q = fabs(currentBodyState.position.y - initialBodyState.position.y);
    }
    if(mode == xDistanceLeft) {
      q = initialBodyState.position.x - currentBodyState.position.x;
    }
    if(mode == xDistanceRight) {
      q = currentBodyState.position.x - initialBodyState.position.x;
    }
    if(mode == yDistanceUp) {
      q = initialBodyState.position.y - currentBodyState.position.y;
    }
    if(mode == yDistanceDown) {
      q = currentBodyState.position.y - initialBodyState.position.y;
    }
    return negation ? -q : q;
  }

};

struct GenProgSpeedScoring : public GenProgScoring {
  bool negation;

  virtual ~GenProgSpeedScoring() {}

  GenProgSpeedScoring(bool negation = false) {
    this->negation = negation;
  }

  double getScore() {
    double q = (currentBodyState.velocity - initialBodyState.velocity).length();
    return negation ? -q : q;
  }
};



struct GenProgComposite : public Composite
{
  virtual ~GenProgComposite() {}

  //std::unordered_map<std::string, GenProgLimbMovement> genProgLimbMovement;
  GenProgLimbMovement genProgLimbMovement;
  std::vector<GenProgScoring*> genProgScorings;
  std::vector<BodySnapshot> initialBodyStates, currentBodyStates, originalBodyStates;

  //double score = 0;
  //std::vector<double> score = 0;


  GenProgComposite(Composite *composite) : Composite(composite->defaultStaticBody, Composite::genProg) {
    /*bodies = std::vector<Body*>(composite->bodies);
    constraints = std::vector<Constraint*>(composite->constraints);
    GenProgComposite *genProgComposite = dynamic_cast<GenProgComposite*>(composite);*/
    bodies = composite->bodies;
    constraints = composite->constraints;
    defaultStaticBody = composite->defaultStaticBody;

    GenProgComposite *genProgComposite = dynamic_cast<GenProgComposite*>(composite);
    if(genProgComposite) {
      genProgLimbMovement = genProgComposite->genProgLimbMovement;
      /*genProgLimbMovement = std::unordered_map<std::string, GenProgLimbMovement>(genProgComposite->genProgLimbMovement);*/
    }
    else {
      fetchGeneticFeatures();
    }

    setCurrentStatesAsOriginalBodyStates();
  }

  void fetchGeneticFeatures() {
    genProgLimbMovement.fetchLimbs(this);
  }

  /*GenProgComposite *getReplicate(Simulation *simulation) {
    return simulation->replicateComposite(this);
    //Composite *composite = simulation->replicateComposite(this);
    //return new GenProgComposite(composite);
  }*/

  void setCurrentStatesAsOriginalBodyStates() {
    originalBodyStates.clear();
    for(int i=0; i<bodies.size(); i++) {
      originalBodyStates.push_back(BodySnapshot(bodies[i]));
    }
  }

  // FIXME this thing needs proper place to run, not to mess with the simulation
  void goToOriginalBodyStates() {
    for(int i=0; i<bodies.size(); i++) {
      bodies[i]->setPosition(originalBodyStates[i].position);
      bodies[i]->setVelocity(originalBodyStates[i].velocity);
      bodies[i]->setAngle(originalBodyStates[i].angle);
      bodies[i]->setAnglularVelocity(originalBodyStates[i].angularVelocity);
    }
  }

  void setInitialConditions(double minTotalTime, double maxTotalTime, int minNumEvents, int maxNumEvents = -1) {
    genProgLimbMovement.setInitialConditions(minTotalTime, maxTotalTime, minNumEvents, maxNumEvents);
  }

  void randomize() {
    genProgLimbMovement.randomize();
  }

  void mutate() {
    genProgLimbMovement.mutate();
  }

  void startMovement(int count) {
    initialBodyStates.clear();
    for(int i=0; i<bodies.size(); i++) {
      initialBodyStates.push_back(BodySnapshot(bodies[i]));
    }
    //score = 0;
    for(int i=0; i<genProgScorings.size(); i++) {
      genProgScorings[i]->start(initialBodyStates[i]);
    }
    genProgLimbMovement.start(count);
  }


  void onUpdate(double dt) {
    genProgLimbMovement.onUpdate(dt);

    currentBodyStates.clear();
    for(int i=0; i<bodies.size(); i++) {
      currentBodyStates.push_back(BodySnapshot(bodies[i]));
    }

    for(int i=0; i<genProgScorings.size(); i++) {
      GenProgImpulseScoring *genProgImpulseScoring;
      if(genProgImpulseScoring = dynamic_cast<GenProgImpulseScoring*>(genProgScorings[i])) {
        genProgImpulseScoring->impulses.clear();
        for(Body *body : bodies) {
          for(BodyPart *bodyPart : body->bodyParts) {
            genProgImpulseScoring->impulses.push_back(bodyPart->currentImpulse);
          }
        }
      }
      genProgScorings[i]->update(currentBodyStates[i]);
    }

    /*for(int i=0; i<genProgScorings.size(); i++) {
      score += genProgScorings->getFinalScore();
    }*/
  }


  /*void onFinish() {
    score = 0;
    for(int i=0; i<genProgScorings.size(); i++) {
      score += genProgScorings->getFinalScore();
    }
  }*/

  void render(GeomRenderer &geomRenderer) {
    if(bodies.size() == 0) return;

    BoundingRect br = getBoundingRect();

    geomRenderer.fillColor.set(0.12, 0.4, 0.12, 0.5);
    geomRenderer.strokeColor.set(0.2, 0.6, 0.2, 0.7);
    geomRenderer.strokeWidth = 4;
    geomRenderer.strokeType = 1;
    geomRenderer.texture = NULL;
    geomRenderer.drawRect(br);
  }

};
