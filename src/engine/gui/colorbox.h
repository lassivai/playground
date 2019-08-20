#pragma once

#include "textbox.h"


struct ColorBox : public TextBox
{
  Vec4d color;

  virtual ~ColorBox() {}

  ColorBox(const std::string &labelText, double r, double g, double b, double a, double x = 0, double y = 0, int inputTextWidth = 8) : TextBox(labelText, "", x, y, inputTextWidth) {
    color.set(r, g, b, a);
    inputText = toHexadecimalColor(color, 2);
    showCursor = false;
  }
  ColorBox(const std::string &labelText, const Vec4d &color, double x = 0, double y = 0, int inputTextWidth = 8) : TextBox(labelText, "", x, y, inputTextWidth) {
    this->color = color;
    inputText = toHexadecimalColor(color, 2);
    showCursor = false;
  }


  void onKeyDown(Events &events) {
    GuiElement::onKeyDown(events);
    if(isInputGrabbed) {
      if(events.sdlKeyCode == SDLK_RETURN) {
        isInputGrabbed = false;
        onAction(this);
      }
    }
  }

  void onTextInput(Events &events) {
  }

  virtual void getTypedValue(Vec4d &value) {
    value = color;
  }

  void getValue(void *value) {
    *(Vec4d*)value = color;
  }

  void setValue(Vec4d color) {
    this->color = color;
    inputText = toHexadecimalColor(color);
  }

  void onMouseWheel(Events &events) {
    GuiElement::onMouseWheel(events);

    if(!isInputGrabbed) return;

    double increment = 1.0/16.0;

    if(events.lAltDown) {
      increment = 1.0/256.0;
    }

    if(events.lControlDown && !events.lShiftDown) {
      color.x += events.mouseWheel * increment;
      color.x = clamp(color.x, 0.0, 1.0);
    }
    if(!events.lControlDown && events.lShiftDown) {
      color.y += events.mouseWheel * increment;
      color.y = clamp(color.y, 0.0, 1.0);
    }
    if(events.lControlDown && events.lShiftDown) {
      color.z += events.mouseWheel * increment;
      color.z = clamp(color.z, 0.0, 1.0);
    }
    if(!events.lControlDown && !events.lShiftDown) {
      color.w += events.mouseWheel * increment;
      color.w = clamp(color.w, 0.0, 1.0);
    }

    inputText = toHexadecimalColor(color, 2);

    inputBackgroundColorSelected = color;
    inputBorderColorSelected.set(color.x, color.y, color.z, 1.0);
    
    Vec3d hsl = rgbToHsl(color.x, color.y, color.z);
    
    if(hsl.z < 0.5 || color.w < 0.5) {
      inputColorSelected.set(1, 1, 1, 1.0);
    }
    else {
      inputColorSelected.set(0, 0, 0, 1.0);
    }
    
    onValueChange(this);
  }

};
