#pragma once

#include "guielement.h"



struct CheckBox : public GuiElement
{
  int textSize = 12;
  std::string label;
  Vec4d labelColor;
  Vec4d inputBackgroundColor;
  Vec4d inputBorderColor;
  Vec4d selectedColor;

  Vec2d padding;
  bool value = false;

  Vec2d boxSize, labelSize;

  double gap;

  Vec2d boxPosition;


  virtual ~CheckBox() {}

  CheckBox(const std::string &label, bool value, double x, double y, int textSize = 12, const Vec4d &overlayColor = Vec4d(1, 1, 1, 1)) : GuiElement(label) {
    this->label = label;
    this->pos.set(x, y);
    //this->padding.set(paddingX, paddingY);

    labelColor.set(0.8, 0.8, 0.8, 0.8);
    inputBackgroundColor.set(0.12, 0.12, 0.12, 0.85);
    inputBorderColor.set(0.5, 0.5, 0.5, 0.8);
    selectedColor.set(0.8, 0.8, 0.8, 0.6);

    this->textSize = textSize;
    this->overlayColor = overlayColor;

    gap = textSize * 0.66;

    this->value = value;

    drawShadow = true;

    blockParentInputOnMouseHover = true;
  }

  CheckBox(const std::string &label, bool value, LayoutPlacer &layoutPlacer, int textSize = 12, const Vec4d &overlayColor = Vec4d(1, 1, 1, 1)) : GuiElement(label) {
    this->label = label;
    //this->pos.set(x, y);
    //this->padding.set(paddingX, paddingY);

    labelColor.set(0.8, 0.8, 0.8, 0.8);
    inputBackgroundColor.set(0.12, 0.12, 0.12, 0.85);
    inputBorderColor.set(0.5, 0.5, 0.5, 0.8);
    selectedColor.set(0.8, 0.8, 0.8, 0.6);

    this->textSize = textSize;
    this->overlayColor = overlayColor;

    gap = textSize * 0.66;

    this->value = value;

    drawShadow = true;

    blockParentInputOnMouseHover = true;

    this->size.y = 23;
    layoutPlacer.setPosition(this);
  }

  void setTextSize(int size) {
    textSize = size;
  }

  virtual void onPrepareShadowTexture(GeomRenderer &geomRenderer) {
    if(drawShadow) {
      if(!shadowTexture) {
        shadowTexture = createBoxShadowTexture(geomRenderer, boxSize.x, boxSize.y, shadowWidth);
      }
      else if(shadowTexture->w != int(boxSize.x+shadowWidth*2) || shadowTexture->h != int(boxSize.y+shadowWidth*2)) {
        delete shadowTexture;
        shadowTexture = createBoxShadowTexture(geomRenderer, boxSize.x, boxSize.y, shadowWidth);
      }
      shadowPosition = boxPosition;
    }
  }

  void onMousePressed(Events &events) {
    GuiElement::onMousePressed(events);

    if(events.mouseButton == 0 && isPointWithin(events.m)) {
      isInputGrabbed = true;
      value = !value;
      onValueChange(this);
    }
  }

  void onMouseReleased(Events &events) {
    GuiElement::onMouseReleased(events);
    isInputGrabbed = false;
  }

  virtual void onPrepare(GeomRenderer &geomRenderer, TextGl &textRenderer) override {
    labelSize = textRenderer.getDimensions(label, textSize) + padding * 2.0;
    boxSize.set(labelSize.y*0.5, labelSize.y*0.5);
    size.set(labelSize.x + boxSize.x*1.5 + gap, labelSize.y);

    boxPosition = Vec2d(labelSize.x + gap, boxSize.y*0.25);
  }

  void onPrerender(GeomRenderer &geomRenderer, TextGl &textRenderer, const Vec2d &displacement) {

    textRenderer.setColor(labelColor);
    textRenderer.print(label, displacement + padding, textSize);

    geomRenderer.texture = NULL;
    geomRenderer.fillColor = inputBackgroundColor;
    geomRenderer.strokeColor = inputBorderColor;
    geomRenderer.strokeType = 1;
    geomRenderer.strokeWidth = 1;
    geomRenderer.drawRect(boxSize, displacement + boxSize*0.5 + boxPosition);

    if(value) {
      geomRenderer.fillColor = selectedColor;
      geomRenderer.strokeColor = selectedColor;
      //geomRenderer.strokeType = 1;
      //geomRenderer.strokeWidth = 1;
      geomRenderer.drawRect(boxSize - Vec2d(6, 6), displacement + boxSize*0.5 + boxPosition);

      //geomRenderer.strokeType = 1;
      //geomRenderer.strokeWidth = 1;

    }
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
  bool getValue() {
    return value;
  }
  void setValue(bool value) {
    prerenderingNeeded = true;
    this->value = value;
  }

};
