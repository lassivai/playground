#pragma once
#include "mathl.h"


struct Scene
{
  double worldScale = 1.0;
  Vec2d worldPosition;

  double screenW = 0, screenH = 0, screenS = 0;
  double aspectRatio = 0;

  Scene(int screenW, int screenH, int screenS, double aspectRatio) {
    this->screenW = screenW;
    this->screenH = screenH;
    this->screenS = screenS;
    this->aspectRatio = aspectRatio;


  }



  inline Vec2d toWorld(const Vec2d &v) {
    return Vec2d(2.0*aspectRatio*v.x/screenW - aspectRatio,
                 2.0*v.y/screenH - 1.0) * worldScale + worldPosition;
  }

  inline Vec2d toScreen(const Vec2d &v) {
    return Vec2d(((v.x-worldPosition.x)/worldScale+aspectRatio)*screenW / (2.0*aspectRatio),
                 ((v.y-worldPosition.y)/worldScale+1.0)*screenH / 2.0);
  }

  inline double toScreenX(const double &x) {
    return ((x-worldPosition.x)/worldScale+aspectRatio)*screenW / (2.0*aspectRatio);

  }
  inline double toScreenY(const double &y) {
    return ((y-worldPosition.y)/worldScale+1.0)*screenH / 2.0;
  }
  inline double toScreen(const double &t) {
    return t * screenH * 0.5 / worldScale;
  }
  inline double toWorld(const double &t) {
    return t / screenH * 2.0 * worldScale;
  }


};
