#pragma once
#include "engine/sketch.h"



struct PostSketch : public Sketch
{
  FastNoise fastNoise;

  GlShader shader, shaderPost;
  Texture texture, texture2, texture3;

  Quadx quadx, quadScreen;

  double scale = 1.0;
  double rotation = 0.0;

  void onInit() {

    scaleBind->active = true;
    positionBind->active = true;

    Events::InputVariableBind<double> *scaleBind = new Events::InputVariableBind<double>(&scale, Events::dragModeExponential, 0.02, Events::buttonRight, 0);
    Events::InputVariableBind<double> *rotationBind = new Events::InputVariableBind<double>(&rotation, Events::dragModeLinear, 0.2, Events::buttonMiddle, 0);

    events.inputBoundDoubles.push_back(scaleBind);
    events.inputBoundDoubles.push_back(rotationBind);

    glSetup(sdlInterface);

    fastNoise.SetSeed(64573);
    fastNoise.SetNoiseType(FastNoise::SimplexFractal);

    //glSetup(sdlInterface);
    //glScene.create();
    shader.create("data/glsl/texture.vert", "data/glsl/texture2.frag");
    shaderPost.create("data/glsl/texture.vert", "data/glsl/texture3.frag");

    //texture.load("data/losing_my_religion2.png");
    texture.load("data/my_new_religion.png");
    //texture2.load("data/my_new_religion2.png");
    texture2.load("data/textures/0001.png");
    /*texture.create(256, 256);
    for(int i=0; i<texture.w; i++) {
      for(int j=0; j<texture.h; j++) {
        double x = map(i, 0, texture.w, -1.0, 1.0);
        double y = map(j, 0, texture.h, -1.0, 1.0);
        double r = 1.0 - clamp(sqrt(x*x + y*y), 0.0, 1.0);
        //double r = (double)i/texture.w;
        //double r = 1.0;
        texture.setPixel(i, j, r, r, r, 1.0);
      }
    }
    texture.applyPixels();*/
    //img.load("data/losing_my_religion2.png", sdlInterface);

    printf("%d x %d\n", texture.w, texture.h);
    quadx.create(texture.w, texture.h, false);

    texture3.create(screenW, screenH);
    quadScreen.create(screenW, screenH);

    clear(0, 0, 0, 1);
    updateScreen(sdlInterface);

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
  }

  void onMouseWheel() {
  }

  void onMousePressed() {
  }

  void onMouseReleased() {
  }

  void onMouseMotion() {
  }




  void onDraw() {
    texture3.setRenderTarget();

    clear(0, 0, 0, 1);
    setColor(1, 1, 1, 1, sdlInterface);

    textRenderer.print("Testing 123", 10, 120, 20, 1);

    glLoadIdentity();
/*
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho( 0.0, sdlInterface->screenW, sdlInterface->screenH, 0.0, 1.0, -1.0 );

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
*/

    shader.activate();
    shader.setUniform2f("screenSize", texture2.w, texture2.h);
    texture.activate(shader, "tex1", 0);
    texture2.activate(shader, "tex2", 1);
    quadx.render((double)events.mouseX, (double)events.mouseY, rotation, scale);
    shader.deActivate();
    texture.inactivate(0);
    texture2.inactivate(1);

    texture3.unsetRenderTarget();

    shaderPost.activate();
    shaderPost.setUniform2f("screenSize", screenW, screenH);
    shaderPost.setUniform1f("time", time);
    texture3.activate(shaderPost, "tex", 0);
    quadScreen.render(screenW/2, screenH/2);
    shaderPost.deActivate();
    texture3.inactivate(0);

    if(events.textInput.inputGrabbed) {
      console.render(sdlInterface);
    }

    //drawLine(200, 450, 500, 20, sdlInterface);

    textRenderer.print("Testing 123", 10, 20, 20, 1);

    /*double y = 10;
    for(int i = 65; i >= 6; i-=1) {
      //double scale = (double)i/65;
      wchar_t buf[256];
      std::swprintf(buf, 256, L"%d, %.2f, Testing 123 µ€$¤", i, 1.0/timer.dt);
      Vec2d size = fontxx.getDimensions(buf, i, 1.0);
      fontxx.print(buf, 10, y + scene->worldPosition.y*5000, i, 1.0, sdlInterface);
      y += size.y;
    }*/

  }




};
