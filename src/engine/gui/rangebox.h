#pragma once

#include "guielement.h"


struct RangeBox : public GuiElement
{
  Vec2d range;
  bool allowCrossing;
  NumberBox *rangeMinGui, *rangeMaxGui;
  bool initialized = false;
  bool absoluteInputTextWidth = false;

  virtual ~RangeBox() {}

  RangeBox(const std::string &name, const Vec2d &range, const Vec2d &rangeLimits, bool allowCrossing, double x, double y, int inputTextWidth = 15, int numberOfDecimals = 6) : GuiElement(name) {
    pos.set(x, y);
    drawBackground = false;
    drawBorder = false;
    //double w = (1+numberOfDecimals) * 23 + name.size();
    addChildElement(rangeMinGui = new NumberBox(name, range.x, NumberBox::FLOATING_POINT, rangeLimits.x, rangeLimits.y, 0, 0, inputTextWidth, numberOfDecimals, true));
    addChildElement(rangeMaxGui = new NumberBox("", range.y, NumberBox::FLOATING_POINT, rangeLimits.x, rangeLimits.y, 0, 0, inputTextWidth, numberOfDecimals, true));
    
    addGuiEventListener(new RangeBoxListener(this));
    this->allowCrossing = allowCrossing;
  }

  RangeBox(const std::string &name, const Vec2d &range, const Vec2d &rangeLimits, bool allowCrossing, LayoutPlacer &layoutPlacer, int inputTextWidth = 15, int numberOfDecimals = 6) : GuiElement(name) {
    //pos.set(x, y);
    drawBackground = false;
    drawBorder = false;
    //double w = (1+numberOfDecimals) * 23 + name.size();
    addChildElement(rangeMinGui = new NumberBox(name, range.x, NumberBox::FLOATING_POINT, rangeLimits.x, rangeLimits.y, 0, 0, inputTextWidth, numberOfDecimals, true));
    addChildElement(rangeMaxGui = new NumberBox("", range.y, NumberBox::FLOATING_POINT, rangeLimits.x, rangeLimits.y, 0, 0, inputTextWidth, numberOfDecimals, true));
    rangeMinGui->absoluteInputTextWidth = true;
    rangeMaxGui->absoluteInputTextWidth = true;
    
    addGuiEventListener(new RangeBoxListener(this));
    this->allowCrossing = allowCrossing;
    
    this->size.y = 23;
    absoluteInputTextWidth = true;
    layoutPlacer.setPosition(this);
  }
  
  void onPrepare(GeomRenderer &geomRenderer, TextGl &textRenderer) {
    if(!initialized) {
      if(absoluteInputTextWidth) {
        Vec2d labelSize = rangeMinGui->labelText.size() == 0 ? Vec2d(0, 0) : textRenderer.getDimensions(rangeMinGui->labelText, rangeMinGui->labelTextSize);
        
        rangeMinGui->setSize((size.x-labelSize.x-4) / 2.0 + labelSize.x, size.y);
        rangeMaxGui->setSize((size.x-labelSize.x-4) / 2.0, size.y);
        printf("size.x %f, size.y %f, labelSize.x %f, pos.x %f, pos.y %f\n,", size.x, size.y, labelSize.x, pos.x, pos.y);
      }
      
      rangeMaxGui->setPosition(rangeMinGui->pos.x + rangeMinGui->size.x + 4, rangeMinGui->pos.y);
      
      initialized = true;
    }
  }

  virtual void getValue(void *value) {
    *(Vec2d*)value = this->range;
  }
  
  virtual void setValue(const Vec2d &value) {
    this->range = value;
    rangeMinGui->setValue(range.x);
    rangeMaxGui->setValue(range.y);
  }

  struct RangeBoxListener : public GuiEventListener {
    RangeBox *rangeBox;
    RangeBoxListener(RangeBox *rangeBox) {
      this->rangeBox = rangeBox;
    }
    void onValueChange(GuiElement *guiElement) {
      if(guiElement == rangeBox->rangeMinGui) {
        guiElement->getValue((void*)&rangeBox->range.x);
        if(!rangeBox->allowCrossing) {
          if(rangeBox->range.x > rangeBox->range.y) {
            rangeBox->range.y = rangeBox->range.x;
            rangeBox->rangeMaxGui->setValue(rangeBox->range.y);
          }
        }
        rangeBox->onValueChange(rangeBox);
      }
      if(guiElement == rangeBox->rangeMaxGui) {
        guiElement->getValue((void*)&rangeBox->range.y);
        if(!rangeBox->allowCrossing) {
          if(rangeBox->range.y < rangeBox->range.x) {
            rangeBox->range.x = rangeBox->range.y;
            rangeBox->rangeMinGui->setValue(rangeBox->range.x);
          }
        }
        rangeBox->onValueChange(rangeBox);
      }
    }
  };

};
