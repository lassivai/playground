#pragma once
#include <SDL2/SDL.h>
#include <string>


/* TODO
 * get rid of sdl timer
 */


struct FrameTimer
{
  double time = 0;
  double dt = 0;
  long frameCount = 0;

  double resetTime = 0;

  double fps = 0;
  double timeAtLastFpsUpdate = 0;
  int fpsUpdateFreq = 30;

  inline double getTime() {
    return (double)SDL_GetTicks()*0.001;
  }

  void update() {
    frameCount++;
    double timeNow = getTime()  - resetTime;
    dt = timeNow - this->time;
    this->time = timeNow;

    if(frameCount % fpsUpdateFreq == 0) {
      double timeElapsed = timeNow - timeAtLastFpsUpdate;
      fps = fpsUpdateFreq / timeElapsed;
      timeAtLastFpsUpdate = timeNow;
    }
  }

  void reset() {
    resetTime = getTime();
  }


};
