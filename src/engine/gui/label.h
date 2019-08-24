#pragma once

#include "guielement.h"




struct Label : public GuiElement
{
  int textSize = 12;
  std::string text;
  Vec4d labelColor;
  Vec2d padding = Vec2d(1, 1);

  virtual ~Label() {}

  Label(const std::string &text, double x, double y, int textSize = 12, const Vec4d &labelColor = Vec4d(0.75, 0.75, 0.75, 0.75)) : GuiElement(text) {
    this->text = text;
    this->pos.set(x, y);
    //this->padding.set(paddingX, paddingY);
    this->labelColor = labelColor;
    prerenderingNeeded = true;
    this->textSize = textSize;
    //this->overlayColor = overlayColor;
  }

  Label(const std::string &text, LayoutPlacer &layoutPlacer, int textSize = 12, const Vec4d &labelColor = Vec4d(0.75, 0.75, 0.75, 0.75)) : GuiElement(text) {
    this->text = text;
    //this->pos.set(x, y);
    //this->padding.set(paddingX, paddingY);
    this->labelColor = labelColor;
    prerenderingNeeded = true;
    this->textSize = textSize;
    //this->overlayColor = overlayColor;
    this->size.y = 23;
    layoutPlacer.setPosition(this);
  }

  void setText(const std::string &text) {
    this->text = text;
    prerenderingNeeded = true;
  }

  void onPrepare(GeomRenderer &geomRenderer, TextGl &textRenderer) {
    size = textRenderer.getDimensions(text, textSize) + padding * 2.0;
  }

  void onPrerender(GeomRenderer &geomRenderer, TextGl &textRenderer, const Vec2d &displacement) {
    //Vec2d d(0, geomRenderer.screenH - prerenderedGuiElement.h);

    //GuiElement::render(geomRenderer, textRenderer);
    textRenderer.setColor(labelColor);
    textRenderer.print(text, displacement + padding, textSize);
  }

  /*void render(GeomRenderer &geomRenderer, TextGl &textRenderer) {
    GuiElement::render(geomRenderer, textRenderer);
    textRenderer.setColor(1, 1, 1, 1);
    textRenderer.print(text, absolutePos + padding, textSize);
  }*/
};


