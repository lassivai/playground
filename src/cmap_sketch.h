#pragma once
#include "engine/sketch.h"




struct TestSketch : public Sketch
{
  GlScene glScene;
  Quad quad;
  GlShader shader;

  Image img, timg;

  //std::vector<Vec3d> colors;
  ColorMap cmap;

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
    //img.load("data/flame-70.png", sdlInterface);
    //timg.create(img.w, img.h, sdlInterface);
    timg.create(screenW, screenH, sdlInterface);

    fastNoise.SetSeed(64573);
    fastNoise.SetNoiseType(FastNoise::SimplexFractal);


    cmap.colors.push_back(Vec3d(0, 0, 0));
    cmap.colors.push_back(Vec3d(1, 1, 1));

    //glSetup();

    //glScene.create();
    //shader.create("data/glsl/basic.vert", "data/glsl/hsv.frag");
    //quad.create(screenW, screenH);

    setAlphaBlending(true, sdlInterface);

    clear(0, 0, 0, 1);
    updateScreen(sdlInterface);
  }

  void applyColorMap() {
    unsigned int *src, *dst;
    timg.accessPixels(&dst);
    for(int i=0; i<screenW; i++) {
      for(int j=0; j<screenH; j++) {
        double d = fastNoise.GetNoise(i, j) * 0.5 + 0.5;
        dst[i+j*screenW] = toRGBA(cmap.getRGB(d));
      }
    }
    timg.applyPixels();
    /*unsigned int *src, *dst;
    img.accessPixels(&src);
    timg.accessPixels(&dst);
    for(int i=0; i<img.w*img.h; i++) {
      dst[i] = toRGBA(ColorMap::getRGB(Random::getDouble(), colors));
    }
    img.applyPixels();
    timg.applyPixels();*/
  }

  void onQuit() {
  }

  void onKeyUp() {
  }

  void onKeyDown() {
    if(events.sdlKeyCode == SDLK_x) {
      cmap.initPositions();
    }
    if(events.sdlKeyCode == SDLK_z) {
      cmap.initPositions();
      for(int i=0; i<cmap.positions.size(); i++) {
        cmap.positions[i] = Random::getDouble();
      }
      sort(cmap.positions);
    }
    for(int i=0; i<cmap.positions.size(); i++) {
      printf("%.4f ", cmap.positions[i]);
    }
    printf("\n");
    applyColorMap();



  }

  void onMouseWheel() {
    if(events.mouseWheel > 0) {
      cmap.colors.push_back(Vec3d());
    }
    if(events.mouseWheel < 0 && cmap.colors.size() > 0) {
      cmap.colors.erase(cmap.colors.end());
    }
    applyColorMap();
  }

  void onMousePressed() {
    for(int i=0; i<cmap.colors.size(); i++) {
      cmap.colors[i].randomize();
    }

    applyColorMap();
  }

  void onMouseReleased() {

  }

  void onMouseMotion() {

  }

  void onUpdate() {

  }




  void onDraw() {
    //clear(0, 0, 0, 1);

    timg.render(sdlInterface);

    for(int i=0; i<100; i++) {
      double v = i / 99.0;
      setColor(cmap.getRGB(v), sdlInterface);
      double x = map(i, 0, 100, screenW*0.25, screenW*0.75);
      fillRect(x, screenH-10, 4.0, sdlInterface);
    }

    setColor(0, 0, 0, 1, sdlInterface);
    for(int i=0; i<cmap.positions.size(); i++) {
      double x = map(cmap.positions[i], 0.0, 1.0, screenW*0.25, screenW*0.75);
      //double x = (cmap.positions[i] * 0.5 + 0.25) * screenW;
      fillRect(x, screenH-5, 4, sdlInterface);
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
