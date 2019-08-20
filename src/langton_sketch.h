#pragma once
#include "engine/sketch.h"


struct LangtonSketch : public Sketch
{
  GlScene glScene;
  Quad quad;
  GlShader shader;

  Image img;

  unsigned int *buffer;
  static const int n = 32;
  int x[n], y[n], a[n];
  int w, h;

  void onInit() {
    w = screenW*16;
    h = screenH*16;
    buffer = new unsigned int[w*h];
    for(int i=0; i<w*h; i++) buffer[i] = 0;



    img.create(screenW, screenH, sdlInterface);


    unsigned int *pixels;
    img.accessPixels(&pixels);
    for(int i=0; i<img.w; i++) {
      for(int j=0; j<img.h; j++) {
        pixels[i+j*img.w] = White;
      }
    }
    img.applyPixels();

    for(int i=0; i<n/2; i++) {
      x[i] = w * i * 2/ n;
      y[i] = h-100;
      a[i] = 0;
    }
    for(int i=0; i<n/2; i++) {
      x[i+n/2] = w * i * 2 / n;
      y[i+n/2] = -100+h;
      a[i+n/2] = 2;
    }
    //glSetup();

    //glScene.create();
    //shader.create("data/glsl/basic.vert", "data/glsl/hsv.frag");
    //quad.create(screenW, screenH);

    clear(0, 0, 0, 1);
    updateScreen(sdlInterface);
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


  void onDraw() {
    //clear(1, 1, 1, 1);

    /*shader.activate();

    shader.setUniform2f("screenSize", screenW, screenH);
    shader.setUniform2f("mousePos", events.mouseX, events.mouseY);
    shader.setUniform1f("time", time);
    quad.render();
    shader.deActivate();*/

    //setColor(0, 0, 0, 1, sdlInterface);

    unsigned int *pixels;
    img.accessPixels(&pixels);
    for(int i=0; i<10000; i++) {
      for(int p = 0; p<n; p++) {

        unsigned int k = buffer[x[p]+y[p]*w];

        if(k % 2 == 0) {
          a[p]++; if(a[p] == 4) a[p] = 0;
        }
        else {
          a[p]--; if(a[p] == -1) a[p] = 3;
        }

        int px = (int)floor((double)x[p]/w*img.w);
        int py = (int)floor((double)y[p]/h*img.h);
        float v = 0.03+0.02*k;
        px = clamp(px, 0, img.w);
        py = clamp(py, 0, img.h);
        pixels[px+py*img.w] = toRGBA(1-v, 1-v, 1-v, 1.0);

        buffer[x[p]+y[p]*w]++;

        if(a[p] == 0) y[p]--;
        if(a[p] == 1) x[p]++;
        if(a[p] == 2) y[p]++;
        if(a[p] == 3) x[p]--;
        if(x[p] >= w) x[p] = 0;
        if(y[p] >= h) y[p] = 0;
        if(x[p] < 0) x[p] = w - 1;
        if(y[p] < 0) y[p] = h - 1;
      }
    }
/*
    for(int i=0; i<10000; i++) {
      for(int p = 0; p<n; p++) {

        unsigned int k = buffer[x[p]+y[p]*w];
        int px = (int)floor((double)x[p]/w*img.w);
        int py = (int)floor((double)y[p]/h*img.h);
        px = clamp(px, 0, img.w);
        py = clamp(py, 0, img.h);

        float v = toR(pixels[px + py*img.w]);
        if(v == 0) v = 0.2;
        else v = v * 1.5;

        if(k == White) {
          a[p]++; if(a[p] == 4) a[p] = 0;
          buffer[x[p]+y[p]*w] = Black;
          pixels[px+py*img.w] = White;
        }
        if(k == Black) {
          a[p]--; if(a[p] == -1) a[p] = 3;
          buffer[x[p]+y[p]*w] = White;
          pixels[px+py*img.w] = White;
        }
        if(a[p] == 0) y[p]--;
        if(a[p] == 1) x[p]++;
        if(a[p] == 2) y[p]++;
        if(a[p] == 3) x[p]--;
        if(x[p] >= w) x[p] = 0;
        if(y[p] >= h) y[p] = 0;
        if(x[p] < 0) x[p] = w - 1;
        if(y[p] < 0) y[p] = h - 1;
      }
    }
*/

    img.applyPixels();
    img.render(sdlInterface);
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
