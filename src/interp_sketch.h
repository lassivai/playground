#pragma once
#include "engine/sketch.h"


struct InterpSketch : public Sketch
{
  GlScene glScene;
  Quad quad;
  GlShader shader;

  Image img;

  static const int n = 10;
  Vec2d p[n];

  FastNoise fastNoise;

  void onInit() {
  /*  img.create(screenW, screenH, sdlInterface);


    unsigned int *pixels;
    img.accessPixels(&pixels);
    for(int i=0; i<img.w; i++) {
      for(int j=0; j<img.h; j++) {
        pixels[i+j*img.w] = White;
      }
    }
    img.applyPixels();
*/
    for(int i=0; i<n; i++) {
      p[i].randomize(0, screenW-1, 0, screenH-1);
    }

    fastNoise.SetSeed(64573);
    //fastNoise.SetFrequency(100.0);
    //fastNoise.SetInterp(FastNoise::Linear);
    fastNoise.SetNoiseType(FastNoise::SimplexFractal);

    //glSetup();

    //glScene.create();
    //shader.create("data/glsl/basic.vert", "data/glsl/hsv.frag");
    //quad.create(screenW, screenH);

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
    setColor(0, 0, 0, 0.2, sdlInterface);
    fillRectCorners(0, 0, screenW-1, screenH-1, sdlInterface);

    int numFractions = 400;

    int iFract = int(fract(time)*numFractions);

    for(int i=0; i<10; i++) {
      Vec2d t;
      p[i].set(screenW*(fastNoise.GetNoise(10*time+3554*i, 1)*0.5+0.5),
               screenH*(fastNoise.GetNoise(10*time+3554*i, 10)*0.5+0.5));

      for(int f=0; f<numFractions; f++) {
        Vec2d v = catmullRom((double)f/numFractions, 0.1, p[(i+n-1)%n], p[i], p[(i+1)%n], p[(i+2)%n]);
        if(f == iFract) {
          setColor(1, 0.2, 0.2, 1.0, sdlInterface);
          fillRect(v, 2, sdlInterface);
        }
        else {
          setColor(0.2, 0.2, 1.0, 1.0, sdlInterface);
          fillRect(v, 1, sdlInterface);
        }
      }
    }

    /*shader.activate();

    shader.setUniform2f("screenSize", screenW, screenH);
    shader.setUniform2f("mousePos", events.mouseX, events.mouseY);
    shader.setUniform1f("time", time);
    quad.render();
    shader.deActivate();*/

    //setColor(0, 0, 0, 1, sdlInterface);


    //img.applyPixels();
    //img.render(sdlInterface);
    //memcpy(pixels, buffer, screenW*screenH*sizeof(unsigned int));

    //img.render(sdlInterface);

    /*unsigned int *pixels;
    img.accessPixels(&pixels);
    for(int i=0; i<img.w; i++) {
      float x = map(i, 0, img.w-1, -aspectRatio, aspectRatio);
      x = pos.x + x * scale;
      for(int j=0; j<img.h; j++) {
        float y = map(j, 0, img.h-1, -1, 1);
        y = pos.y + y * scale;
        float q = fastNoise.GetNoise(x, y)/2.0 + 0.5;
        pixels[i+j*img.w] = toRGBA(q, q, q, 1.0);
      }
    }
    img.applyPixels();

    img.render(sdlInterface);*/
  }

};
