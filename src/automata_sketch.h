#pragma once
#include "engine/sketch.h"


struct TestSketch : public Sketch
{
  GlScene glScene;
  Quad quad;
  GlShader shader;

  Image img;

  unsigned int *buffer;

  FastNoise fastNoise;

  unsigned int A = 0, B = 0, C = 0;

  void onInit() {

    fastNoise.SetSeed(Random::getInt());
    fastNoise.SetFrequency(0.1);
    fastNoise.SetInterp(FastNoise::Quintic);
    fastNoise.SetNoiseType(FastNoise::SimplexFractal);
    fastNoise.SetFractalOctaves(4);

    img.create(screenW, screenH, sdlInterface);
    buffer = new unsigned int[screenW*screenH];

    A = White;
    B = Black;
    C = Gray50;

    unsigned int *pixels;
    img.accessPixels(&pixels);
    for(int i=0; i<img.w; i++) {
      for(int j=0; j<img.h; j++) {
        float f = map(fastNoise.GetNoise(i, j), -1, 1, -0.65, 1.65);
        int k = f < 0 ? B : (f > 1 ? A : C);
        pixels[i+j*img.w] = k;
      }
    }
    img.applyPixels();

    //glSetup();

    //glScene.create();
    //shader.create("data/glsl/basic.vert", "data/glsl/hsv.frag");
    //quad.create(screenW, screenH);


  }

  void onQuit() {
    delete [] buffer;
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


  inline unsigned int readPixel(int x, int y, int w, int h, unsigned int *pixels) {
    if(x >= w) x -= w;
    if(y >= h) y -= h;
    if(x < 0) x += w;
    if(y < 0) y += h;
    return pixels[x + y*w];
  }
  inline void putPixel(unsigned int v, int x, int y, int w, int h, unsigned int *pixels) {
    pixels[x + y*w] = v;
  }

  inline void automaton1(int x, int y, int w, int h, unsigned int *pixelsRead, unsigned int *pixelsWrite) {
    unsigned int k = readPixel(x, y, w, h, pixelsRead);
    int nA = 0, nB = 0, nC = 0;
    for(int i=-1; i<2; i++) {
      for(int j=-1; j<2; j++) {
        if(i == 0 && j == 0) continue;
        unsigned int p = readPixel(x+i, y+j, w, h, pixelsRead);
        if(p == A) nA ++;
        else if(p == B) nB ++;
        else nC++;
      }
    }

    if(k == B) {
      if(nB < 2 && nA > 4 && nA < 7)
        putPixel(A, x, y, w, h, pixelsWrite);
      else if(nB < 5)
        putPixel(B, x, y, w, h, pixelsWrite);
      else
        putPixel(C, x, y, w, h, pixelsWrite);
    }
    if(k == A) {
      if(nA < 2 && nB > 4 && nB < 7)
        putPixel(B, x, y, w, h, pixelsWrite);
      else if(nA < 5)
        putPixel(A, x, y, w, h, pixelsWrite);
      else
        putPixel(C, x, y, w, h, pixelsWrite);
    }
    if(k == C) {
      if(nA > nB && nA > 3)
        putPixel(A, x, y, w, h, pixelsWrite);
      else if(nA < nB && nB > 3)
        putPixel(B, x, y, w, h, pixelsWrite);
      else if(nB+nA >= nC)
        putPixel(nA > nB ? A : (nA < nB ? B : C), x, y, w, h, pixelsWrite);
      else
        putPixel(C, x, y, w, h, pixelsWrite);
    }
  }


  void onDraw() {
    //clear(0, 0, 0, 1);

    /*shader.activate();

    shader.setUniform2f("screenSize", screenW, screenH);
    shader.setUniform2f("mousePos", events.mouseX, events.mouseY);
    shader.setUniform1f("time", time);
    quad.render();
    shader.deActivate();*/

    unsigned int *pixels;
    img.accessPixels(&pixels);
    for(int i=0; i<img.w; i++) {
      for(int j=0; j<img.h; j++) {
        int k = pixels[i+j*img.w];
        automaton1(i, j, img.w, img.h, pixels, buffer);
      }
    }
    memcpy(pixels, buffer, screenW*screenH*sizeof(unsigned int));
    img.applyPixels();

    img.render(sdlInterface);

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
