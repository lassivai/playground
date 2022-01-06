#pragma once
#include "engine/sketch.h"



struct BumpSketch : public Sketch
{
  FastNoise fastNoise;

  GlShader shader;
  Texture texture, texture2, texture3;

  Quadx quadx, quadScreen;

  double scale = 1.0;
  double rotation = 0.0;

  double brightness = 1.0;
  double decay = 0.2;
  Vec4d lightColor;
  bool lightOn = true;

  //GeomRenderer geomRenderer;

  Vec3d lightPos;
  Vec2d objectPos;

  Panel *panel;
  NumberBox *brightnessGui, *decayGui, *lightHeightGui;
  ColorBox *lightColorGui;
  CheckBox *lightOnGui;

  void onInit() {

    glSetup(sdlInterface);

    fastNoise.SetSeed(64573);
    fastNoise.SetNoiseType(FastNoise::SimplexFractal);


    shader.create("data/glsl/texture.vert", "data/glsl/texturex.frag");

    //texture.load("data/my_new_religion.png");
    texture.load("data/textures/material_469.png");
    texture2.load("data/textures/material_469_normalmap.png");

    //texture.create(screenW, screenH);
    //texture2.create(screenW, screenH);


    //geomRenderer.create();

    quadx.create(texture.w, texture.h, false);

    objectPos.set(screenW*0.5, screenH*0.5);
    lightPos.set(screenW*0.5, screenH*0.5, 10.0);
    lightColor.set(1, 1, 1, 1);

    panel = new Panel(400, 200, 30, 30);
    brightnessGui = new NumberBox("brightness", brightness, NumberBox::FLOATING_POINT, 0, 10000, 10, 10);
    decayGui = new NumberBox("decay", decay, NumberBox::FLOATING_POINT, 0, 10000, 10, 40);
    lightHeightGui = new NumberBox("height", lightPos.z, NumberBox::FLOATING_POINT, 0, 10000, 10, 70);
    lightColorGui = new ColorBox("color", 1, 1, 1, 1, 10, 100);
    lightOnGui = new CheckBox("light on", true, 10, 130);

    panel->addChildElement(brightnessGui);
    panel->addChildElement(decayGui);
    panel->addChildElement(lightHeightGui);
    panel->addChildElement(lightColorGui);
    panel->addChildElement(lightOnGui);
    panel->addGuiEventListener(new PropertiesListener(this));
    guiRoot.addChildElement(panel);

  }

  struct PropertiesListener : public GuiEventListener {
    ShaderSketch *shaderSketch;
    PropertiesListener(ShaderSketch *shaderSketch) {
      this->shaderSketch = shaderSketch;
    }
    void onValueChange(GuiElement *guiElement) {
      if(guiElement->name.compare("brightness") == 0) {
        guiElement->getValue((void*)&shaderSketch->brightness);
      }
      if(guiElement->name.compare("decay") == 0) {
        guiElement->getValue((void*)&shaderSketch->decay);
      }
      if(guiElement->name.compare("height") == 0) {
        guiElement->getValue((void*)&shaderSketch->lightPos.z);
      }
      if(guiElement->name.compare("color") == 0) {
        guiElement->getValue((void*)&shaderSketch->lightColor);
      }
      if(guiElement->name.compare("light on") == 0) {
        guiElement->getValue((void*)&shaderSketch->lightOn);
      }
    }
  };


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
    lightPos.z += events.mouseWheel;
    lightPos.z = max(lightPos.z, 0.0);
    lightHeightGui->setValue(lightPos.z);
  }

  void onMousePressed() {
  }

  void onMouseReleased() {
  }

  void onMouseMotion() {
    if(events.mouseDownM) {
      objectPos += events.m - events.mp;
    }
    if(events.mouseDownR) {
      Vec2d d = events.m - events.mp;
      lightPos.x += d.x;
      lightPos.y += d.y;
    }
  }

  void onReloadShaders() {
    shader.create("data/glsl/texture.vert", "data/glsl/texturex.frag");
  }


  void onDraw() {
    //texture3.setRenderTarget();

    clear(0.18, 0, 0.05, 1);

    sdlInterface->glmMatrixStack.loadIdentity();
    //glLoadIdentity();



    //trianglex.set(tri);
    //trianglex.render(screenW/2, screenH/2, time, 400);

    //texture3.inactivate(0);

    shader.activate();
    shader.setUniform2f("pos", objectPos);
    shader.setUniform2f("size", quadx.w, quadx.h);
    shader.setUniform3f("lightPos", lightPos);
    shader.setUniform1f("brightness", brightness);
    shader.setUniform1f("decay", decay);
    shader.setUniform4f("lightColor", lightColor);
    shader.setUniform1i("lightOn", (int)lightOn);
    texture.activate(shader, "tex", 0);
    texture2.activate(shader, "tex2", 1);
    quadx.render(objectPos.x, objectPos.y, rotation, scale);
    shader.deActivate();
    texture.inactivate(0);
    texture2.inactivate(1);


    /*texture3.unsetRenderTarget();
    shaderPost.activate();
    shaderPost.setUniform2f("screenSize", screenW, screenH);
    shaderPost.setUniform1f("time", time);
    texture3.activate(shaderPost, "tex", 0);
    quadScreen.render(screenW/2, screenH/2);
    shaderPost.deActivate();
    texture3.inactivate(0);*/

    //if(events.textInput.inputGrabbed) {
      //console.render(sdlInterface);
    //}

    //drawLine(200, 450, 500, 20, sdlInterface);

  }




};
