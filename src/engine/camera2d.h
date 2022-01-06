#pragma once
#include "mathl.h"
#include "glwrap.h"
#include "event.h"




struct Camera2D {
  double w, h;
  Vec2d position;
  double scale = 1, rotation = 0;
  Events::InputVariableBind<double> *scaleBind, *rotationBind;
  Events::InputVariableBind<Vec2d> *positionBind;

  Matrix3d matrix, rotationScaleMatrix;
  bool updated = false;
  bool pushed = false;
  
  SDLInterface *sdlInterface = NULL;

  void init(double w, double h, Events &events, SDLInterface *sdlInterface) {
    this->w = w;
    this->h = h;
    
    this->sdlInterface = sdlInterface;

    events.cameraMatrix = &matrix;
    events.cameraMatrixRotScale = &rotationScaleMatrix;

    scaleBind = new Events::InputVariableBind<double>(&scale, Events::dragModeExponential, 0.02, Events::releaseModeToOne, Events::buttonRight, Events::modifiersLControl);
    events.inputBoundDoubles.push_back(scaleBind);

    rotationBind = new Events::InputVariableBind<double>(&rotation, Events::dragModeLinear, 0.002, Events::releaseModeToZero, Events::buttonRight, Events::modifiersLControl | Events::modifiersLShift);
    events.inputBoundDoubles.push_back(rotationBind);

    positionBind = new Events::InputVariableBind<Vec2d>(&position, Events::dragModeLinear, Vec2d(1, 1)/*Vec2d(-2.0/screenH, -2.0/screenH)*/, Events::releaseModeToMouse, Events::buttonRight, 0);
    positionBind->applyCameraMatrix = 1;
    events.inputBoundVec2ds.push_back(positionBind);
  }

  void setMouseInputActive(bool active) {
    scaleBind->active = active;
    rotationBind->active = active;
    positionBind->active = active;
  }

  void pushMatrix() {
    sdlInterface->glmMatrixStack.pushMatrix();
    sdlInterface->glmMatrixStack.translate(w*0.5, h*0.5, 0.0);
    sdlInterface->glmMatrixStack.scale(scale, scale, 1.0);
    sdlInterface->glmMatrixStack.rotate(rotation * 180.0 / PI, 0.0, 0.0, 1.0);
    sdlInterface->glmMatrixStack.translate(-w*0.5, -h*0.5, 0.0);
    sdlInterface->glmMatrixStack.translate(position.x, position.y, 0.0);

    //glPushMatrix();
    //glTranslated(w*0.5, h*0.5, 0.0);
    //glScaled(scale, scale, 1.0);
    //glRotated(rotation * 180.0 / PI, 0.0, 0.0, 1.0);
    //glTranslated(-w*0.5, -h*0.5, 0.0);
    //glTranslated(position.x, position.y, 0.0);
    updateMatrix();
    pushed = true;
  }

  void popMatrix() {
    sdlInterface->glmMatrixStack.popMatrix();
    //glPopMatrix();
    updated = false;
    pushed = false;
  }
  
  /*void pushMatrix() {
    glPushMatrix();
    glTranslated(w*0.5, h*0.5, 0.0);
    glScaled(scale, scale, 1.0);
    glRotated(rotation * 180.0 / PI, 0.0, 0.0, 1.0);
    glTranslated(-w*0.5, -h*0.5, 0.0);
    glTranslated(position.x, position.y, 0.0);
    updateMatrix();
    pushed = true;
  }

  void popMatrix() {
    glPopMatrix();
    updated = false;
    pushed = false;
  }*/

  void updateMatrix() {
    Matrix3d t0, t1, t2, r, s;
    t0.setTranslate(w*0.5, h*0.5);
    s.setScale(1.0/scale);
    r.setRotate(-rotation);
    t1.setTranslate(-w*0.5, -h*0.5);
    t2.setTranslate(-position.x, -position.y);
    matrix = t0;
    matrix *= t2;
    matrix *= r;
    matrix *= s;
    matrix *= t1;
    rotationScaleMatrix = r;
    rotationScaleMatrix *= s;
    updated = true;
  }

  inline Vec2d toCameraSpace(const Vec2d &v) {
    if(!updated) updateMatrix();
    return matrix * v;
  }

  inline Vec2d toCameraDimension(const Vec2d &v) {
    if(!updated) updateMatrix();
    return rotationScaleMatrix * v;
  }

};
