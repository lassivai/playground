#pragma once
#include "engine/sketch.h"



struct IntegrateSketch : public Sketch
{
  std::vector<double> src, dst;
  bool cubic = false;

  void reset() {
    src.resize(Random::getInt(2, screenW/4));
    dst.resize(Random::getInt(2, screenW/4));
    for(int i=0; i<src.size(); i++) {
      src[i] = Random::getDouble(0, screenH*0.25);
    }
    resizeArray(src, dst);
  }

  void onInit() {
  }

  void onUpdate() {
    for(int i=0; i<commandQueue.commands.size(); i++) {
      Command *cmd = NULL;
    }
  }

  void onQuit() {
  }

  void onKeyUp() {
  }

  void onKeyDown() {
    if(events.sdlKeyCode == SDLK_p) {
      reset();
    }
    if(events.sdlKeyCode == SDLK_q) {
      resizeArray(src, dst, -1, -1, -1, -1, true);
    }
  }

  void onMouseWheel() {
  }

  void onMousePressed() {
  }

  void onMouseReleased() {
  }

  void onMouseMotion() {
  }

  void onReloadShaders() {
  }


  void onDraw() {
    clear(0, 0, 0, 1);

    geomRenderer.strokeColor.set(1, 1, 1, 1);
    geomRenderer.strokeWidth = 1.5;
    geomRenderer.strokeType = 1;

    for(int i=0; i<src.size(); i++) {
      double x = map(i, 0.0, src.size()-1, 0.0, screenW-1);
      double y = src[i];
      geomRenderer.drawLine(x, 0, x, y);
      if(i < src.size()-1) {
        double x2 = map(i+1, 0.0, src.size()-1, 0.0, screenW-1);
        double y2 = src[i+1];
        geomRenderer.drawLine(x, y, x2, y2);
      }
    }

    for(int i=0; i<dst.size(); i++) {
      double x = map(i, 0.0, dst.size()-1, 0.0, screenW-1);
      double y = screenH - dst[i];
      geomRenderer.drawLine(x, screenH, x, y);
      if(i < dst.size()-1) {
        double x2 = map(i+1, 0.0, dst.size()-1, 0.0, screenW-1);
        double y2 = screenH - dst[i+1];
        geomRenderer.drawLine(x, y, x2, y2);
      }
    }


    if(console.inputGrabbed) {
      console.render(sdlInterface);
    }
  }




};
