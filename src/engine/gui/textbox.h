#pragma once

#include "guielement.h"



struct TextBox : public GuiElement
{
  int labelTextSize = 12, inputTextSize = 12;
  std::string labelText = "";
  std::string inputText = "";

  Vec2d padding;
  int inputTextWidth;
  double gap;

  Vec4d labelColor, inputColor, inputColorSelected;

  bool drawInputBackground = true, drawInputBorder = true;
  Vec4d inputBackgroundColor;
  Vec4d inputBorderColor;
  
  Vec4d inputBackgroundColorSelected;
  Vec4d inputBorderColorSelected;

  Vec4d invalidTextColor;
  
  double inputBorderWidth = 1;
  bool isInputBorder = false;
  Vec2d inputPadding;

  bool showCursor = true;

  bool editingEnabled = true;

  Vec2d inputPos, inputSize;
  std::string textToCursorTmp;
  double cursorX;

  bool loseFocusOnAction = true;

  int textAlignmentX = 0;

  /*bool drawGlow = true;
  Texture *glowTexture = NULL;
  int glowWidth = 10;
  Vec2d glowPosition;*/

  double fadeDuration = 0.2;
  double hoverTimer = 0, pressedTimer = 0;
  
  bool isValidValue = true;
  
  
  virtual void onUpdate(double time, double dt) {
    hoverTimer += isMouseHover ? dt : -dt;
    hoverTimer = clamp(hoverTimer, 0, fadeDuration);
    
    if(hoverTimer > 0 && !isMouseHover) {
      hoverTimer -= dt;
      hoverTimer = clamp(hoverTimer, 0, fadeDuration);
      prerenderingNeeded = true;
    }
    else if(hoverTimer < fadeDuration && isMouseHover) {
      hoverTimer += fadeDuration;
      hoverTimer = clamp(hoverTimer, 0, fadeDuration);
      prerenderingNeeded = true;
    }
    
    if(pressedTimer > 0 && !isInputGrabbed) {
      pressedTimer -= dt;
      pressedTimer = clamp(pressedTimer, 0, fadeDuration);
      prerenderingNeeded = true;
    }
    else if(pressedTimer < fadeDuration && isInputGrabbed) {
      pressedTimer += dt;
      pressedTimer = clamp(pressedTimer, 0, fadeDuration);
      prerenderingNeeded = true;
    }
    //pressedTimer += isInputGrabbed ? dt : -dt;
    //pressedTimer = clamp(pressedTimer, 0, fadeDuration);

  }
  

  virtual ~TextBox() {
    if(shadowTexture) delete shadowTexture;
  }

  TextBox() {
    blockParentInputOnMouseHover = true;
  }

  TextBox(const std::string &labelText, const std::string &inputText, double x, double y, int inputTextWidth = 15, double paddingX = 0, double paddingY = 0, double inputPaddingX = 5, double inputPaddingY = 0) : GuiElement(labelText) {
    this->labelText = labelText;
    this->inputText = inputText;
    this->pos.set(x, y);
    this->padding.set(paddingX, paddingY);
    this->inputPadding.set(inputPaddingX, inputPaddingY);
    this->inputTextWidth = inputTextWidth;
    gap = labelTextSize;
    labelColor.set(0.8, 0.8, 0.8, 0.8);
    inputColor.set(0.9, 0.9, 0.9, 0.9);
    inputColorSelected.set(1, 1, 1, 1);
    inputBackgroundColor.set(0.12, 0.12, 0.12, 0.85);
    inputBorderColor.set(0.5, 0.5, 0.5, 0.8);
    inputBackgroundColorSelected.set(0.04, 0.04, 0.04, 0.92);
    inputBorderColorSelected.set(0.75, 0.75, 0.75, 0.8);
    invalidTextColor.set(0.9, 0.9, 0.9, 0.3);
    
    drawInputBorder = false;
    drawShadow = true;
    
    blockParentInputOnMouseHover = true;
  }
  
  bool absoluteInputTextWidth = false;
  
  TextBox(const std::string &labelText, const std::string &inputText, LayoutPlacer &layoutPlacer, int inputTextWidth = 15, double paddingX = 0, double paddingY = 0, double inputPaddingX = 5, double inputPaddingY = 0) : GuiElement(labelText) {
    this->labelText = labelText;
    this->inputText = inputText;
    //this->pos.set(x, y);
    this->padding.set(paddingX, paddingY);
    this->inputPadding.set(inputPaddingX, inputPaddingY);
    this->inputTextWidth = inputTextWidth;
    gap = labelTextSize;
    labelColor.set(0.8, 0.8, 0.8, 0.8);
    inputColor.set(0.9, 0.9, 0.9, 0.9);
    inputColorSelected.set(1, 1, 1, 1);
    inputBackgroundColor.set(0.12, 0.12, 0.12, 0.85);
    inputBorderColor.set(0.5, 0.5, 0.5, 0.8);
    inputBackgroundColorSelected.set(0.04, 0.04, 0.04, 0.92);
    inputBorderColorSelected.set(0.75, 0.75, 0.75, 0.8);
    invalidTextColor.set(0.9, 0.9, 0.9, 0.3);
    
    drawInputBorder = false;
    drawShadow = true;
    
    blockParentInputOnMouseHover = true;
    
    this->size.y = 23;
    
    absoluteInputTextWidth = true;
    layoutPlacer.setPosition(this);
  }
  
  void setTextSize(int textSize) {
    labelTextSize = textSize;
    inputTextSize = textSize;
  }

  void setEditingEnabled(bool value) {
    showCursor = value;
    editingEnabled = value;
  }


  void onMousePressed(Events &events) {
    GuiElement::onMousePressed(events);

    if(events.mouseButton == 0 && isPointWithin(events.m)) {
      isInputGrabbed = true;
      events.textInput.setInputText(inputText);
      textToCursorTmp = events.textInput.getTextToCursor();
      prerenderingNeeded = true;
    }
    else if(isInputGrabbed) {
      isInputGrabbed = false;
      prerenderingNeeded = true;
    }
  }

  void onKeyDown(Events &events) {
    GuiElement::onKeyDown(events);
    if(isInputGrabbed) {
      if(events.sdlKeyCode == SDLK_RETURN) {
        isInputGrabbed = !loseFocusOnAction;
        onAction(this);
      }
    }
  }

  virtual void onTextInput(Events &events) {
    if(isInputGrabbed && editingEnabled) {
      textToCursorTmp = events.textInput.getTextToCursor();
      if(inputText.compare(events.textInput.inputText) != 0) {
        inputText = events.textInput.inputText;
        onValueChange(this);
      }
    }
  }

  void setValidValue(bool isValidValue) {
    this->isValidValue = isValidValue;
    prerenderingNeeded = true;
  }

  void onPrepare(GeomRenderer &geomRenderer, TextGl &textRenderer) {
    //GuiElement::onPrepare(geomRenderer, textRenderer);
    
    Vec2d labelSize = labelText.size() == 0 ? Vec2d(0, 0) : textRenderer.getDimensions(labelText, labelTextSize);
    
    double gap = labelText.size() > 0 ? this->gap : 0;
    
    if(absoluteInputTextWidth) {
      inputSize.x = max(0, size.x - labelSize.x - gap);
      inputSize.y = labelSize.y* 0.71;
      size.y = (inputSize.y) + padding.y*2.0;
      //size.x = labelSize.x + inputSize.x + padding.x*2.0 + (labelText.size() > 0 ? gap : 0);
    }
    else {
      Vec2d inputTextDimensions = textRenderer.getDimensions(inputText, inputTextSize);
      if(inputTextWidth < 0) {
         inputSize = inputTextDimensions;
       }
       else {
         inputSize = textRenderer.getDimensions("A", inputTextSize);
         inputSize.x *= inputTextWidth;
       }
       inputSize.y *= 0.71;
       inputSize += inputPadding * 2.0;
       // NOTE after upgrading to ubuntu 18.04 from 16.04, labelSize.y has changed from the previous 30 to 60 (?!)
       size.y = (inputSize.y) + padding.y*2.0;
       size.x = labelSize.x + inputSize.x + padding.x*2.0 + (labelText.size() > 0 ? gap : 0);
     }

    inputPos = Vec2d(labelSize.x + gap, 0);

    cursorX = textRenderer.getDimensions(textToCursorTmp, inputTextSize).x;

    //textAlignmentX = int(inputSize.x - inputTextDimensions.x);
  }

  void onRender(GeomRenderer &geomRenderer, TextGl &textRenderer) {
    //GuiElement::render(geomRenderer, textRenderer);

    if(showCursor && isInputGrabbed && (SDL_GetTicks() / 500) % 2 == 0) {
      //Vec2d ip = absolutePos + inputPos + padding + Vec2d(-inputPadding.x, inputPadding.y);
      geomRenderer.strokeColor.set(1, 1, 1, 0.5);
      geomRenderer.strokeType = 1;
      geomRenderer.strokeWidth = 1;
      
      if(absoluteInputTextWidth) {
        Vec2d d = textRenderer.getDimensions(inputText, inputTextSize);
        geomRenderer.drawLine(absolutePos.x + size.x - 6 - d.x + cursorX, absolutePos.y + 2, absolutePos.x + size.x - 6 - d.x + cursorX, absolutePos.y + inputSize.y - 4);
      }
      else {
        Vec2d ip = absolutePos + inputPos + padding + inputPadding;
        geomRenderer.drawLine(ip.x + cursorX + textAlignmentX, ip.y + 2, ip.x + cursorX + textAlignmentX, ip.y + inputSize.y - 4);
      }
    }
    
    /*Vec4d col = mix(Vec4d(1, 1, 1, 0), Vec4d(1, 1, 1, 1), hoverTimer/fadeDuration);
    if(col.w > 0) {
      glowTexture->render(absolutePos+glowPosition, col);
    }*/
    
    /*Vec4d col2;

    if(value && !isInputGrabbed) {
      col2 = pressedOverlayColor;
    }
    else {
      col2 = mix(Vec4d(0.3, 0.3, 0.3, 1), pressedOverlayColor, pressedTimer/fadeDuration);
    }

    texture->render(rect.pos+absolutePos, col2);*/
    
    
  }
  
  virtual void onPrepareShadowTexture(GeomRenderer &geomRenderer) {
    if(drawShadow) {
      if(!shadowTexture) {
        shadowTexture = createBoxShadowTexture(geomRenderer, inputSize.x, inputSize.y, shadowWidth);
      }
      else if(shadowTexture->w != int(inputSize.x+shadowWidth*2) || shadowTexture->h != int(inputSize.y+shadowWidth*2)) {
        delete shadowTexture;
        shadowTexture = createBoxShadowTexture(geomRenderer, inputSize.x, inputSize.y, shadowWidth);
      }
      shadowPosition.set(inputPos + padding);
    }
    
    /*if(drawGlow) {
      if(!glowTexture) {
        glowTexture = createBoxShadowTexture(geomRenderer, inputSize.x, inputSize.y, glowWidth);
      }
      else if(glowTexture->w != int(inputSize.x+glowWidth*2) || glowTexture->h != int(inputSize.y+glowWidth*2)) {
        delete glowTexture;
        glowTexture = createBoxShadowTexture(geomRenderer, inputSize.x, inputSize.y, glowWidth);
      }
      glowPosition.set(inputPos - Vec2d(glowWidth, glowWidth));
    }*/
  }

  void onPrerender(GeomRenderer &geomRenderer, TextGl &textRenderer, const Vec2d &displacement) {
    
    if(drawInputBackground || drawInputBorder) {
      geomRenderer.texture = NULL;
      //geomRenderer.fillColor = drawInputBackground ? (isInputGrabbed ? inputBackgroundColorSelected : inputBackgroundColor) : Vec4d::Zero;
      double a = hoverTimer/fadeDuration;
      double b = pressedTimer/fadeDuration;
      geomRenderer.fillColor = mix(inputBackgroundColor, Vec4d(0.4, 0.4, 0.4, 0.92), a);
      if(b > 0) {
        geomRenderer.fillColor = mix(geomRenderer.fillColor, inputBackgroundColorSelected, b);
      }
      geomRenderer.strokeColor = geomRenderer.fillColor;
      geomRenderer.strokeType = 0;
      geomRenderer.strokeWidth = 0;
      /*geomRenderer.strokeColor = !drawInputBorder ? geomRenderer.fillColor : (isInputGrabbed ? inputBorderColorSelected : inputBorderColor);
      geomRenderer.strokeType = 1;
      geomRenderer.strokeWidth = inputBorderWidth;*/

      geomRenderer.drawRect(round(inputSize), round(displacement + inputPos + padding + inputSize*0.5));
    }

    textRenderer.setColor(labelColor);
    textRenderer.print(labelText, displacement + padding, labelTextSize);
    //Vec2d ip = inputPos + padding + Vec2d(-inputPadding.x, inputPadding.y);
    Vec2d ip = inputPos + padding + inputPadding;
    textRenderer.setColor(isInputGrabbed ? inputColorSelected : (isValidValue ? inputColor : invalidTextColor));
    
    if(absoluteInputTextWidth) {
      Vec2d d = textRenderer.getDimensions(inputText, inputTextSize);
      textRenderer.print(inputText, displacement + Vec2d(size.x - 6 - d.x, 0), inputTextSize);
    }
    else {
      textRenderer.print(inputText, displacement + ip + Vec2d(textAlignmentX, 0), inputTextSize);
    }

  }

  virtual void getTypedValue(std::string &value) {
    value = inputText;
  }

  void getValue(void *value) {
    *(std::string*)value = inputText;
  }

  void getValue(std::string &str) {
    str = inputText;
  }

  std::string getValue() {
    return inputText;
  }

  void setValue(const std::string &str) {
    prerenderingNeeded = true;
    inputText = str;
  }
};
