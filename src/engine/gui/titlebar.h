#pragma once

#include "guielement.h"




struct TitleBar : public GuiElement
{
  int textSize = 12;
  std::string text;
  bool textChanged = true;
  Vec4d labelColor;
  //Vec2d padding = Vec2d(1, 1);
  Texture *labelTexture = NULL;
  Texture *glowTexture = NULL;

  //std::string title;

  virtual ~TitleBar() {}
  
  std::function<double()> getGlowLevel = []() { return 1; };

  TitleBar(const std::string &text, double width, double height = 31, double x = 0, double y = 0, int textSize = 10, const Vec4d &labelColor = Vec4d(0.5, 0.5, 0.5, 0.75)) : GuiElement(text + " titlebar") {
    this->text = text;
    toUpperCase(this->text);
    this->pos.set(x, y);
    //this->padding.set(paddingX, paddingY);
    this->labelColor = labelColor;
    prerenderingNeeded = true;
    this->textSize = textSize;
    //this->overlayColor = overlayColor;
    setSize(width, height);
  }
  
  
  /*bool isPointWithin(const Vec2d &p) {
    return false;
  }*/

  void setText(const std::string &text) {
    this->text = text;
    textChanged = true;
    prerenderingNeeded = true;
  }

  Vec2d round(const Vec2d &v) {
    return Vec2d(std::round(v.x), std::round(v.y));
  }
  virtual void onPrerender(GeomRenderer &geomRenderer, TextGl &textRenderer, const Vec2d &displacement) {
    //Vec2d d(0, geomRenderer.screenH - prerenderedGuiElement.h);

    //GuiElement::render(geomRenderer, textRenderer);
    /*Vec2d dim = textRenderer.getDimensions(text, textSize);
    textRenderer.setColor(labelColor);
    textRenderer.print(text, displacement + size*0.5 - dim*0.5, textSize);*/
  }
  
  virtual void onPrepare(GeomRenderer &geomRenderer, TextGl &textRenderer) {
    if(textChanged) {
      if(labelTexture) {
        delete labelTexture;
        labelTexture = NULL;
      }
      if(glowTexture) {
        delete glowTexture;
        glowTexture = NULL;
      }
    }
    if(!labelTexture) {
      labelTexture = createLabelTexture(geomRenderer, textRenderer, text, textSize, Vec4d(1, 1, 1, 1));
    }
    if(!glowTexture) {
      glowTexture = createSpriteShadowTexture(geomRenderer, *labelTexture, 15, 7, 1.5, Vec4d(1, 1, 1, 1));
    }
    textChanged = false;
  }
  
  virtual void onRender(GeomRenderer &geomRenderer, TextGl &textRenderer) {
    Vec4d overlay(0, 0, 0, 1);
    Vec4d overlay2 = labelColor;
    
    if(getGlowLevel) {
      overlay = mix(Vec4d(1, 1, 1, 0), Vec4d(1, 1, 1, 1), getGlowLevel());
      overlay2 = mix(labelColor, Vec4d(1, 1, 1, 1), getGlowLevel());
    }
    
    glowTexture->renderCenter(absolutePos.x+size.x*0.5, absolutePos.y+size.y*0.5+4, overlay);
    labelTexture->renderCenter(absolutePos.x+size.x*0.5, absolutePos.y+size.y*0.5+4, overlay2);
    
    if(!parent || parent->size.y != size.y) {
      geomRenderer.texture = NULL;
      geomRenderer.fillColor = drawBackground ? backgroundColor : Vec4d::Zero;
      geomRenderer.strokeColor = borderColor;

      //geomRenderer.fillColor = Vec4d(0.0, 0.0, 0.0, 1);
      //geomRenderer.strokeColor = Vec4d(0.0, 0.0, 0.0, 1);
      //geomRenderer.strokeType = drawBorder ? 1 : 0;
      geomRenderer.strokeType = 1;
      geomRenderer.strokeWidth = 1;
      //geomRenderer.drawRect(size, d+size*0.5);
      //geomRenderer.drawLine(0, size.y-1, size.x, size.y-1, round(absolutePos));    
      //geomRenderer.startRendering(GeomRenderer::RendererType::FastStrokeRenderer);
      //geomRenderer.endRendering();
      geomRenderer.drawLine(0, size.y-1, size.x, size.y-1, absolutePos);    
      //geomRenderer.endRendering();
      //printf("at %s:\n", parent->name.c_str());
      //geomRenderer.print();
      
      // FIXME Title bar line at note biquad filter don't show if not rendered with something else. Fast stroke rendering also helps.
      // The bug arose when added fast stroke rendering at envelope preview panel.
      geomRenderer.drawRect(0, 0, 0, 0);    
    }

  }

  /*void render(GeomRenderer &geomRenderer, TextGl &textRenderer) {
    GuiElement::render(geomRenderer, textRenderer);
    textRenderer.setColor(1, 1, 1, 1);
    textRenderer.print(text, absolutePos + padding, textSize);
  }*/
};
