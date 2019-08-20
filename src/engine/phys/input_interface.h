#pragma once
#include "chipphys.h"
#include "bodypart.h"
#include "collisionpair.h"
#include "body.h"
#include "constraint.h"
#include <SDL2/SDL.h>


struct InputInterface {
  virtual void onKeyDown(Events &events) {}
  virtual void onKeyUp(Events &events) {}
  virtual std::string toString() = 0;

  virtual ~InputInterface() {}

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
};


struct SimpleMotorController : public InputInterface {
  SimpleMotor *simpleMotor = NULL;
  bool reverseOn = false, forwardOn = false;
  int reverseKey, forwardKey;

  virtual ~SimpleMotorController() {}

  void set(Constraint *constraint, int reverseKey = 0, int forwardKey = 0) {
    simpleMotor = dynamic_cast<SimpleMotor*>(constraint);
    simpleMotor->scaleRate(0.0);
    simpleMotor->scaleMaxForce(0.0);
    this->reverseKey = reverseKey;
    this->forwardKey = forwardKey;
    forwardOn = false;
    reverseOn = false;
  }

  void unset() {
    if(simpleMotor) {
      simpleMotor->scaleRate(1.0);
      simpleMotor->scaleMaxForce(1.0);
    }
  }

  void onKeyDown(Events &events) {
    if(simpleMotor && !reverseOn && !forwardOn) {
      if(events.sdlKeyCode == reverseKey) {
        simpleMotor->scaleRate(-1.0);
        simpleMotor->scaleMaxForce(1.0);
        reverseOn = true;
      }
      if(events.sdlKeyCode == forwardKey) {
        simpleMotor->scaleRate(1.0);
        simpleMotor->scaleMaxForce(1.0);
        forwardOn = true;
      }
    }
  }

  void onKeyUp(Events &events) {
    if(simpleMotor) {
      if(events.sdlKeyCode == reverseKey && reverseOn) {
        simpleMotor->scaleRate(0.0);
        simpleMotor->scaleMaxForce(0.0);
        reverseOn = false;
      }
      if(events.sdlKeyCode == forwardKey && forwardOn) {
        simpleMotor->scaleRate(0.0);
        simpleMotor->scaleMaxForce(0.0);
        forwardOn = false;
      }
    }
  }

  bool onConstraintRemoval(Constraint *constraint) {
    if(dynamic_cast<SimpleMotor*>(constraint) == simpleMotor) {
      unset();
      return true;
    }
    return false;
  }

  std::string toString() {
    std::string str = "simple motor controller, ";
    str += std::string(SDL_GetKeyName(reverseKey)) + ", " + std::string(SDL_GetKeyName(forwardKey));
    return  str;
  }
};


struct SimpleCharacterController : public InputInterface {
  Body *body = NULL;
  BodyPart *foot = NULL;
  bool leftOn = false, rightOn = false;
  bool readyToJump = false;
  int leftKey, rightKey, jumpKey;
  double maxLateralSpeed = 10, lateralForce = 10, jumpForce = 100;

  double lastGroundTouchTime = 0;
  double groundTouchTimeTolerance = 15.0/60.0;

  virtual ~SimpleCharacterController() {}

  void set(Body *body, BodyPart *foot, double maxLateralSpeed, double lateralForce, double jumpForce, int leftKey, int rightKey, int jumpKey) {
    this->foot = foot;
    this->body = body;
    this->leftKey = leftKey;
    this->rightKey = rightKey;
    this->jumpKey = jumpKey;
    this->maxLateralSpeed = maxLateralSpeed;
    this->lateralForce = lateralForce;
    this->jumpForce = jumpForce;
  }

  void unset() {

  }

  bool isTouchingGround() {
    std::vector<CollisionPair> collisionPairs = foot->parentBody->getCollisionPairs();
    if(collisionPairs.size() > 0) {
      for(CollisionPair &collisionPair : collisionPairs) {
        if(collisionPair.bodyPartA == foot || collisionPair.bodyPartB == foot) {
          lastGroundTouchTime = (double)SDL_GetTicks()*0.001;
          return true;
        }
      }
    }
    return (double)SDL_GetTicks()*0.001 - lastGroundTouchTime <= groundTouchTimeTolerance;
  }

  void onKeyDown(Events &events) {
    if(events.sdlKeyCode == leftKey) {
      leftOn = true;
    }
    if(events.sdlKeyCode == rightKey) {
      rightOn = true;
    }

    if(events.sdlKeyCode == jumpKey) {
      if(readyToJump && isTouchingGround()) {
        body->applyForceAtLocalPoint(0, -jumpForce);
        readyToJump = false;
      }
    }
  }

  void onKeyUp(Events &events) {
    if(events.sdlKeyCode == leftKey) {
      leftOn = false;
    }
    if(events.sdlKeyCode == rightKey) {
      rightOn = false;
    }
  }

  void onUpdate(double dt) {
    if(leftOn) {
      if(body->getVelocity().x > -maxLateralSpeed) {
        //if(isTouchingGround()) {
          body->applyForceAtLocalPoint(-lateralForce*dt, 0);
        //}
      }
    }
    if(rightOn) {
      if(body->getVelocity().x < maxLateralSpeed) {
        //if(isTouchingGround()) {
          body->applyForceAtLocalPoint(lateralForce*dt, 0);
        //}
      }
    }
    if(body->getVelocity().y >= 0) {
      readyToJump = true;
    }

  }

  bool onBodyRemoval(Body *body) {
    if(body == foot->parentBody || body == this->body) {
      unset();
      return true;
    }
    return false;
  }

  bool onBodyPartRemoval(BodyPart *bodyPart) {
    if(bodyPart == foot) {
      unset();
      return true;
    }
    return false;
  }


  std::string toString() {
    std::string str = "simple character controller, ";
    str += std::string(SDL_GetKeyName(leftKey)) + ", " + std::string(SDL_GetKeyName(rightKey)) + ", " + std::string(SDL_GetKeyName(jumpKey));
    return  str;
  }
};
