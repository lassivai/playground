#pragma once
#include "engine/sketch.h"


/* TODO
 * number box input for shader variables
 *  - automatic parsing of shader file and gui creation
 */




struct ShaderSketch : public Sketch
{
  CanvasShader *canvasShader = NULL;

  void onInit() {
    canvasShader = new MandelbrotVisualizer();
    canvasShader->init();
    canvasShader->init();
    canvasShader->init(events, screenW, screenH);
    canvasShader->setActive(true);
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
    if(events.sdlKeyCode == SDLK_p && events.numModifiersDown == 0) {
      if(canvasShader->getPanel()) canvasShader->removePanel(&guiRoot);
      else canvasShader->addPanel(&guiRoot);
    }
  }

  void onMouseWheel() {
/*    lightPos.z += events.mouseWheel;
    lightPos.z = max(lightPos.z, 0.0);
    lightHeightGui->setValue(lightPos.z);*/
  }

  void onMousePressed() {
  }

  void onMouseReleased() {
  }

  void onMouseMotion() {
  }

  void onReloadShaders() {
    canvasShader->loadShader();
  }


  void onDraw() {
    canvasShader->render(screenW, screenH, time, events);

    if(console.inputGrabbed) {
      console.render(sdlInterface);
    }
  }




};
