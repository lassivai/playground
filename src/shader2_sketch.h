#pragma once
#include "engine/sketch.h"


/* TODO
 * number box input for shader variables
 *  - automatic parsing of shader file and gui creation
 */


struct Shader2Sketch : public Sketch
{
  FastNoise fastNoise;

  GlShader shader;
  Texture texture, texture2, texture3;

  Quadx quadx, quadScreen;

  static const int maxPoints = 1000;
  int numPoints = 1000;
  Vec2d points[maxPoints];
/*
  double scale = 1.0;
  double value = 0.0, amplitude = 0.5, frequency = 2.4, gain = 0.42, lacunarity = 2.7;

  static const int maxWarpLevels = 10;
  int warpLevels = 8;
  double warpFreqs[maxWarpLevels], warpAmounts[maxWarpLevels];

  Panel *panel;
  NumberBox *scaleGui, *valueGui, *frequencyGui, *amplitudeGui, *lacunarityGui, *gainGui;
  NumberBox *warpLevelsGui, *warpFreqsGui[maxWarpLevels], *warpAmountsGui[maxWarpLevels];

*/
  /*double brightness = 1.0;
  double decay = 0.2;
  Vec4d lightColor;
  bool lightOn = true;
  Vec3d lightPos;*/

  //GeomRenderer geomRenderer;

/*  Panel *panel;
  NumberBox *brightnessGui, *decayGui, *lightHeightGui;
  ColorBox *lightColorGui;
  CheckBox *lightOnGui;*/

  void onInit() {

    glSetup(sdlInterface);

    fastNoise.SetSeed(64573);
    fastNoise.SetNoiseType(FastNoise::SimplexFractal);


    shader.create("data/glsl/texture.vert", "data/glsl/voro.frag");

    //texture.load("data/my_new_religion.png");
    //texture.load("data/textures/material_469.png");
    //texture2.load("data/textures/material_469_normalmap.png");

    //texture.create(screenW, screenH);
    //texture2.create(screenW, screenH);


    //geomRenderer.create();

    quadx.create(screenW, screenH, false);

    /*for(int i=0; i<maxWarpLevels; i++) {
      warpFreqs[i] = 0.1;
      warpAmounts[i] = 1.0;
    }*/


  }
/*
  void setPreset(int index) {
    if(index == 0) {
      scale = 1.0;
      value = 0.0;
      frequency = 2.5;
      amplitude = 0.5;
      gain = 0.42;
      lacunarity = 2.7;
      warpLevels = 8;
      warpFreqs[0] = 0.1;
      warpFreqs[1] = 0.1;
      warpFreqs[2] = 0.1;
      warpFreqs[3] = 0.1;
      warpFreqs[4] = 0.1;
      warpFreqs[5] = 0.1;
      warpFreqs[6] = 0.1;
      warpFreqs[7] = 0.1;
      warpFreqs[8] = 0.1;
      warpFreqs[9] = 0.1;
    }
    if(index == 1) {
      scale = 1.0;
      value = -0.2;
      frequency = 1.0;
      amplitude = 0.8;
      gain = 0.45;
      lacunarity = 2.6;
      warpLevels = 10;
      warpFreqs[0] = 0.1;
      warpFreqs[1] = 0.01;
      warpFreqs[2] = 0.04;
      warpFreqs[3] = 0.03;
      warpFreqs[4] = 0.1;
      warpFreqs[5] = 0.8;
      warpFreqs[6] = 0.6;
      warpFreqs[7] = 0.9;
      warpFreqs[8] = 0.1;
      warpFreqs[9] = 0.1;
    }
    if(index == 2) {
      scale = 2.0;
      value = -0.2;
      frequency = 1.0;
      amplitude = 0.8;
      gain = 0.45;
      lacunarity = 2.35;
      warpLevels = 10;
      warpFreqs[0] = 0.1;
      warpFreqs[1] = 0.1;
      warpFreqs[2] = 0.05;
      warpFreqs[3] = 0.03;
      warpFreqs[4] = 0.04;
      warpFreqs[5] = 0.08;
      warpFreqs[6] = 0.07;
      warpFreqs[7] = 0.09;
      warpFreqs[8] = 1.5;
      warpFreqs[9] = 0.5;
    }
    if(index == 3) {
      scale = 0.4;
      value = 0.0;
      frequency = 2.5;
      amplitude = 0.5;
      gain = 0.41;
      lacunarity = 2.35;
      warpLevels = 10;
      warpFreqs[0] = 0.3;
      warpFreqs[1] = 0.3;
      warpFreqs[2] = 0.2;
      warpFreqs[3] = 0.2;
      warpFreqs[4] = 0.3;
      warpFreqs[5] = 0.2;
      warpFreqs[6] = 0.1;
      warpFreqs[7] = 0.6;
      warpFreqs[8] = 0.3;
      warpFreqs[9] = 0.1;
    }
    if(panel) {
      scaleGui->setValue(scale);
      valueGui->setValue(value);
      frequencyGui->setValue(frequency);
      amplitudeGui->setValue(amplitude);
      lacunarityGui->setValue(lacunarity);
      gainGui->setValue(gain);
      warpLevelsGui->setValue(warpLevels);

      for(int i=0; i<maxWarpLevels; i++) {
        warpFreqsGui[i]->setValue(warpFreqs[i]);
        warpAmountsGui[i]->setValue(warpAmounts[i]);
      }
    }
  }

  void addPanel() {
    if(panel) return;

    panel = new Panel(350, 590, 10, 10);
    double line = 10, lineHeight = 30;
    scaleGui = new NumberBox("scale", scale, NumberBox::FLOATING_POINT, 0.0, 100000, 10, line);
    valueGui = new NumberBox("value", value, NumberBox::FLOATING_POINT, -1000.0, 1000, 10, line += lineHeight);
    frequencyGui = new NumberBox("frequency", frequency, NumberBox::FLOATING_POINT, -1000, 1000, 10, line += lineHeight);
    amplitudeGui = new NumberBox("amplitude", amplitude, NumberBox::FLOATING_POINT, -1000, 1000, 10, line += lineHeight);
    lacunarityGui = new NumberBox("lacunarity", lacunarity, NumberBox::FLOATING_POINT, -1000, 1000, 10, line += lineHeight);
    gainGui = new NumberBox("gain", gain, NumberBox::FLOATING_POINT, -1000, 1000, 10, line += lineHeight);
    warpLevelsGui = new NumberBox("warp levels", warpLevels, NumberBox::INTEGER, 1, maxWarpLevels, 10, line += lineHeight);
    for(int i=0; i<maxWarpLevels; i++) {
      warpFreqsGui[i] = new NumberBox("warp freq. "+std::to_string(i+1), warpFreqs[i], NumberBox::FLOATING_POINT, 0, 1000, 10, line += lineHeight);
    }

    for(int i=0; i<maxWarpLevels; i++) {
      warpAmountsGui[i] = new NumberBox("warp amount "+std::to_string(i), warpAmounts[i], NumberBox::FLOATING_POINT, -1000, 1000, 10, line += lineHeight);
    }

    panel->addChildElement(scaleGui);
    panel->addChildElement(valueGui);
    panel->addChildElement(frequencyGui);
    panel->addChildElement(amplitudeGui);
    panel->addChildElement(gainGui);
    panel->addChildElement(lacunarityGui);
    panel->addChildElement(warpLevelsGui);
    for(int i=0; i<maxWarpLevels; i++) {
      panel->addChildElement(warpFreqsGui[i]);
      panel->addChildElement(warpAmountsGui[i]);
    }

    panel->addGuiEventListener(new PropertiesListener(this));

    panel->size.set(320, line + lineHeight + 10);

    guiRoot.addChildElement(panel);
  }

  void removePanel() {
    if(!panel) return;
    guiRoot.deleteChildElement(panel);
    panel = NULL;
  }


  struct PropertiesListener : public GuiEventListener {
    ShaderSketch *shaderSketch;
    PropertiesListener(ShaderSketch *shaderSketch) {
      this->shaderSketch = shaderSketch;
    }
    void onValueChange(GuiElement *guiElement) {
      if(guiElement == shaderSketch->scaleGui) {
        guiElement->getValue((void*)&shaderSketch->scale);
      }
      if(guiElement == shaderSketch->valueGui) {
        guiElement->getValue((void*)&shaderSketch->value);
      }
      if(guiElement == shaderSketch->frequencyGui) {
        guiElement->getValue((void*)&shaderSketch->frequency);
      }
      if(guiElement == shaderSketch->amplitudeGui) {
        guiElement->getValue((void*)&shaderSketch->amplitude);
      }
      if(guiElement == shaderSketch->lacunarityGui) {
        guiElement->getValue((void*)&shaderSketch->lacunarity);
      }
      if(guiElement == shaderSketch->gainGui) {
        guiElement->getValue((void*)&shaderSketch->gain);
      }
      if(guiElement == shaderSketch->warpLevelsGui) {
        guiElement->getValue((void*)&shaderSketch->warpLevels);
      }
      for(int i=0; i<maxWarpLevels; i++) {
        if(guiElement == shaderSketch->warpFreqsGui[i]) {
          guiElement->getValue((void*)&shaderSketch->warpFreqs[i]);
        }
        if(guiElement == shaderSketch->warpAmountsGui[i]) {
          guiElement->getValue((void*)&shaderSketch->warpAmounts[i]);
        }
      }
    }
  };*/

  void onUpdate() {
    for(int i=0; i<commandQueue.commands.size(); i++) {
      Command *cmd = NULL;
    }
  }

  void randomize() {
    for(int i=0; i<maxPoints; i++) {
      points[i].randomize(0, screenW, 0, screenH);
    }
  }

  void onQuit() {
  }

  void onKeyUp() {
  }

  void onKeyDown() {
    if(events.sdlKeyCode == SDLK_r) {
      randomize();
    }
    /*if(events.sdlKeyCode == SDLK_p && events.numModifiersDown == 0) {
      if(panel) removePanel();
      else addPanel();
    }
    if(events.sdlKeyCode == SDLK_1) {
      setPreset(1);
    }
    if(events.sdlKeyCode == SDLK_2) {
      setPreset(2);
    }
    if(events.sdlKeyCode == SDLK_3) {
      setPreset(3);
    }
    if(events.sdlKeyCode == SDLK_0) {
      setPreset(0);
    }*/
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
    /*if(events.mouseDownM) {
      objectPos += events.m - events.mp;
    }
    if(events.mouseDownR) {
      Vec2d d = events.m - events.mp;
      lightPos.x += d.x;
      lightPos.y += d.y;
    }*/
  }

  void onReloadShaders() {
    shader.create("data/glsl/texture.vert", "data/glsl/voro.frag");
  }


  void onDraw() {
    //texture3.setRenderTarget();

    clear(0.18, 0, 0.05, 1);

    glLoadIdentity();



    //trianglex.set(tri);
    //trianglex.render(screenW/2, screenH/2, time, 400);

    //texture3.inactivate(0);

    shader.activate();
    shader.setUniform2f("size", quadx.w, quadx.h);
    shader.setUniform1f("time", time);
    shader.setUniform2f("mouse", events.m);

    shader.setUniform1i("numPoints", numPoints);
    shader.setUniform2fv("points", maxPoints, points);

    /*shader.setUniform1f("scale", scale);
    shader.setUniform1f("value", value);
    shader.setUniform1f("frequency", frequency);
    shader.setUniform1f("amplitude", amplitude);
    shader.setUniform1f("lacunarity", lacunarity);
    shader.setUniform1f("gain", gain);
//    warpLevels = int(time) % 6 + 1;

    //shader.setUniform1i("warpLevels", warpLevels);
    float warpFreqsf[maxWarpLevels], warpAmountsf[maxWarpLevels];
    for(int i=0; i<maxWarpLevels; i++) {
      warpFreqsf[i] = (float)warpFreqs[i];
      warpAmountsf[i] = (float)warpAmounts[i];
    }

    shader.setUniform1fv("warpFreqs", maxWarpLevels, warpFreqsf);
    shader.setUniform1fv("warpAmounts", maxWarpLevels, warpAmountsf);
*/

    //texture.activate(shader, "tex", 0);
    //texture2.activate(shader, "tex2", 1);
    quadx.render(screenW/2, screenH/2);
    shader.deActivate();
    //texture.inactivate(0);
    //texture2.inactivate(1);


    /*texture3.unsetRenderTarget();
    shaderPost.activate();
    shaderPost.setUniform2f("screenSize", screenW, screenH);
    shaderPost.setUniform1f("time", time);
    texture3.activate(shaderPost, "tex", 0);
    quadScreen.render(screenW/2, screenH/2);
    shaderPost.deActivate();
    texture3.inactivate(0);*/

    if(console.inputGrabbed) {
      console.render(sdlInterface);
    }

    //drawLine(200, 450, 500, 20, sdlInterface);

  }




};
