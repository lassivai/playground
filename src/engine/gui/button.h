#pragma once

#include "guielement.h"
#include "../texture.h"



struct Button : public GuiElement
{
  // TODO shared pointer
  static std::unordered_map<std::string, Texture*> buttonTextures;
  
  enum Mode { PressButton, ToggleButton };
  Mode mode = PressButton;
  /*int textSize = 12;
  std::string label;
  Vec4d labelColor;
  Vec4d inputBackgroundColor;
  Vec4d inputBorderColor;
  Vec4d selectedColor;

  Vec2d padding;
  bool value = false;
  
  Vec2d boxSize, labelSize;
  
  double gap;

  Vec2d boxPosition;*/
  
  Vec4d defaultOverlayColor;
  Vec4d inactiveOverlayColor;
  Vec4d hoverOverlayColor;
  Vec4d pressedOverlayColor;
  
  // TODO shared pointer
  Texture *texture = NULL;
  Texture *hoverTexture = NULL;
  //Texture *inactivateTexture = NULL;
  //Texture *pressedTexture = NULL;
  Rect rect;
  
  bool value = false;
  
  double fadeDuration = 0.2;
  
  double hoverTimer = 0, pressedTimer = 0;

  void loadTexture(const std::string &filename) {
    /*if(buttonTextures.count(filename) > 0) {
      texture = buttonTextures[filename];
    }
    else {*/
      texture = new Texture(25, 25);
      
      if(!texture->load(filename, false)) {
        printf("Error at Button::loadTexture(), name '%s', failed to load file '%s'!\n", name.c_str(), filename.c_str());
        texture->clear(1, 0, 1, 1);
      }
      //buttonTextures[filename] = texture;
    //}
    
//    hoverTexture = createSpriteShadowTexture(geomRenderer, texture, 6, 2, Vec4d(1, 1, 1, 1));
    //pressedTexture = texture;
    //inactivateTexture = texture;
    
    rect.set(texture->w, texture->h, texture->w*0.5, texture->h*0.5);
    this->setSize(texture->w, texture->h);
    //rect.set(w, h, x+25*4, line);
  }

  virtual ~Button() {
    delete texture;
    delete hoverTexture;
  }

  Button(const std::string &name, const std::string &textureFilename, double x, double y, Mode mode = PressButton, bool value = false) : GuiElement(name) {
    this->pos.set(x, y);
    this->mode = mode;
    this->value = value;
    //this->padding.set(paddingX, paddingY);
    
    defaultOverlayColor.set(0.3, 0.3, 0.3, 1);
    inactiveOverlayColor.set(0, 0, 0, 0.3);
    hoverOverlayColor.set(1, 1, 1, 1);
    pressedOverlayColor.set(1, 1, 1, 0.75);
    
  
    loadTexture(textureFilename);
    
    //drawShadow = true;
    blockParentInputOnMouseHover = true;
  }

  Button(const std::string &name, const std::string &textureFilename, RowColumnPlacer &placer, Mode mode = PressButton, bool value = false) : GuiElement(name) {
    this->mode = mode;
    this->value = value;
    //this->padding.set(paddingX, paddingY);
    
    defaultOverlayColor.set(0.3, 0.3, 0.3, 1);
    inactiveOverlayColor.set(0, 0, 0, 0.3);
    hoverOverlayColor.set(1, 1, 1, 1);
    pressedOverlayColor.set(1, 1, 1, 0.75);
    
  
    loadTexture(textureFilename);
    
    //drawShadow = true;
    blockParentInputOnMouseHover = true;
    
    placer.setPosition(this);
  }
  virtual void onUpdate(double time, double dt) {
    hoverTimer += isMouseHover ? dt : -dt;
    hoverTimer = clamp(hoverTimer, 0, fadeDuration);
    
    
    pressedTimer += isInputGrabbed || value ? dt : -dt;
    pressedTimer = clamp(pressedTimer, 0, fadeDuration);

  }

  double getActivationLevel() {
    return pressedTimer / fadeDuration;
  }

  virtual void onPrepareShadowTexture(GeomRenderer &geomRenderer) {
    /*if(drawShadow && texture) {
      if(!shadowTexture) {
        //shadowTexture = createBoxShadowTexture(geomRenderer, boxSize.x, boxSize.y, shadowWidth);
        shadowTexture = createSpriteShadowTexture(geomRenderer, texture, shadowWidth);
      }
      else if(shadowTexture->w != int(boxSize.x+shadowWidth*2) || shadowTexture->h != int(boxSize.y+shadowWidth*2)) {
        delete shadowTexture;
        shadowTexture = createBoxShadowTexture(geomRenderer, boxSize.x, boxSize.y, shadowWidth);
      }
      shadowPosition = boxPosition;
    }*/
  }

  void onMousePressed(Events &events) {
    GuiElement::onMousePressed(events);

    if(events.mouseButton == 0 && isPointWithin(events.m)) {
      isInputGrabbed = true;
      if(mode == PressButton) {
        value = true;
        onValueChange(this);
        onAction(this);
        value = false;
      }
      else if(mode == ToggleButton) {
        value = !value;
        onAction(this);
        onValueChange(this);
      }
    }
  }

  void onMouseReleased(Events &events) {
    GuiElement::onMouseReleased(events);
    isInputGrabbed = false;
  }

  void prepare(GeomRenderer &geomRenderer, TextGl &textRenderer) {
    GuiElement::prepare(geomRenderer, textRenderer);

    if(!hoverTexture) {
      hoverTexture = createSpriteShadowTexture(geomRenderer, *texture, 15, 7, 2, Vec4d(1, 1, 1, 1), true);
    }
    /*labelSize = textRenderer.getDimensions(label, textSize) + padding * 2.0;
    boxSize.set(labelSize.y*0.5, labelSize.y*0.5);
    size.set(labelSize.x + boxSize.x*1.5 + gap, labelSize.y);
    
    boxPosition = Vec2d(labelSize.x + gap, boxSize.y*0.25);*/
  }


  virtual void onRender(GeomRenderer &geomRenderer, TextGl &textRenderer) {
    Vec4d col = mix(Vec4d(1, 1, 1, 0), hoverOverlayColor, hoverTimer/fadeDuration);
    hoverTexture->render(rect.pos+absolutePos, col);
    
    Vec4d col2;

    if(value && !isInputGrabbed) {
      col2 = pressedOverlayColor;
    }
    else {
      col2 = mix(defaultOverlayColor, pressedOverlayColor, pressedTimer/fadeDuration);
    }

    texture->render(rect.pos+absolutePos, col2);
  }

  void onPrerender(GeomRenderer &geomRenderer, TextGl &textRenderer, const Vec2d &displacement) {
    
    /*textRenderer.setColor(labelColor);
    textRenderer.print(label, displacement + padding, textSize);
    
    geomRenderer.texture = NULL;
    geomRenderer.fillColor = inputBackgroundColor;
    geomRenderer.strokeColor = inputBorderColor;
    geomRenderer.strokeType = 1;
    geomRenderer.strokeWidth = 1;
    geomRenderer.drawRect(boxSize, displacement + boxSize*0.5 + boxPosition);
    */
    /*if(value) {
      geomRenderer.fillColor = selectedColor;
      geomRenderer.strokeColor = selectedColor;
      //geomRenderer.strokeType = 1;
      //geomRenderer.strokeWidth = 1;
      geomRenderer.drawRect(boxSize*0.66, displacement + boxSize*0.5 + boxPosition);

      //geomRenderer.strokeType = 1;
      //geomRenderer.strokeWidth = 1;

    }*/
  }
  /*void render(GeomRenderer &geomRenderer, TextGl &textRenderer) {
    GuiElement::render(geomRenderer, textRenderer);
    textRenderer.setColor(1, 1, 1, 1);
    textRenderer.print(label + (value ? " [X]" : " [-]"), absolutePos + padding, textSize);
  }*/

  virtual void getTypedValue(bool &value) {
    value = this->value;
  }
  
  void getValue(void *value) {
    *(bool*)value = this->value;
  }
  bool getValue() const {
    return value;
  }

  void setValue(bool value) {
    prerenderingNeeded = true;
    this->value = value;
  }

};
