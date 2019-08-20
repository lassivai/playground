#pragma once
#include "engine/sketch.h"


struct ColorsSketch : public Sketch
{
  GlScene glScene;
  Quad quad;
  GlShader shader;

  void onInit() {
    glSetup();

    glScene.create();
    shader.create("data/glsl/basic.vert", "data/glsl/hsv.frag");
    quad.create(screenW, screenH);

  }

  void onQuit() {
  }

  void onKeyUp() {
  }

  void onKeyDown() {

  }


  void onMousePressed() {

  }

  void onMouseReleased() {

  }

  void onMouseMotion() {

  }

  void onUpdate() {

  }

  void onDraw() {
    //clear(0, 0, 0, 1);

    shader.activate();

    shader.setUniform2f("screenSize", screenW, screenH);
    shader.setUniform2f("mousePos", events.mouseX, events.mouseY);
    shader.setUniform1f("time", time);
    quad.render();

    shader.deActivate();

  }

};
