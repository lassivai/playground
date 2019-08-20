#pragma once
#include "engine/sketch.h"



struct TestSketch : public Sketch
{
  GlScene glScene;
  Quad quad;
  GlShader shader;

  FastNoise fastNoise;

  Polygon poly;
  int numVertices = 20;

  void onInit() {



    fastNoise.SetSeed(64573);
    fastNoise.SetNoiseType(FastNoise::SimplexFractal);


    //glSetup();

    //glScene.create();
    //shader.create("data/glsl/basic.vert", "data/glsl/hsv.frag");
    //quad.create(screenW, screenH);

    poly.create(numVertices, screenH/2, screenH/2);

    setAlphaBlending(true, sdlInterface);

    clear(0, 0, 0, 1);
    updateScreen(sdlInterface);
  }


  void onQuit() {
  }

  void onKeyUp() {
  }

  void onKeyDown() {
  }

  void onMouseWheel() {
    if(events.mouseWheel > 0) {
      numVertices++;
    }
    else {
      numVertices--;
    }
    numVertices = max(3, numVertices);

    poly.create(numVertices, screenH/2, screenH/2);
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
    clear(0, 0, 0, 1);

    setColor(1, 1, 1, 1, sdlInterface);

    poly.render(screenW/2, screenH/2, sdlInterface);
  }

};
