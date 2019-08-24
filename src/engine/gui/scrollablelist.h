#pragma once

#include "textbox.h"


struct ScrollableList : public GuiElement
{
  std::vector<std::string> items;
  int activeItemIndex = 0;
  std::string labelText = "";

  int labelTextSize = 12, inputTextSize = 10;
  
  int scrollPosition = 0;
  int maxScrollPosition = 0;

  Vec4d labelColor, inputColor, inputColorSelected;
  Vec4d inputBackgroundColor;
  Vec4d inputBorderColor;
  Vec4d inputBackgroundColorSelected;
  Vec4d inputBorderColorSelected;
  Vec4d activeItemTextColor, activeItemBackgroundColor, activeItemBorderColor;
  double itemHeight = 18;
  double itemWidth = 0;
  
  double scrollBarWidth = 16;
  double scrollBarBlockWidth = 14, scrollBarBlockHeight = 28;
  bool showScrollBar = true;
  
  
  
  

  virtual ~ScrollableList() {}

  ScrollableList(const std::string &labelText, double w, double h, double x, double y) : GuiElement(labelText) {
    this->labelText = labelText;
    setPosition(x, y);
    setSize(w, h);
    drawBackground = true;
    drawBorder = true;
    labelColor.set(0.8, 0.8, 0.8, 0.8);
    
    inputColor.set(0.9, 0.9, 0.9, 0.9);
    inputBackgroundColor.set(0.12, 0.12, 0.12, 0.85);
    inputBorderColor.set(0.5, 0.5, 0.5, 0.8);
    
    inputColorSelected.set(0.9, 0.9, 0.9, 0.9);
    inputBackgroundColorSelected.set(0.04, 0.04, 0.04, 0.92);
    inputBorderColorSelected.set(0.75, 0.75, 0.75, 0.8);

    activeItemTextColor.set(1, 1, 1, 1);
    activeItemBackgroundColor.set(0.08, 0.08, 0.08, 0.92);
    activeItemBorderColor.set(0.75, 0.75, 0.75, 0.5);
    
    blockParentInputOnMouseHover = true;
  }

  virtual void setSize(int w, int h) {
    GuiElement::setSize(w, h);
    itemWidth = max(0, w - scrollBarWidth);
    updateScrollPosMax();
  }

  void updateScrollPosMax() {
    maxScrollPosition = max(0, items.size() - floor(size.y/itemHeight));
    scrollPosition = clamp(scrollPosition, 0, maxScrollPosition);
  }
  
  void setActiveItemIndex(int activeItemIndex) {
    if(activeItemIndex != this->activeItemIndex) {
      this->activeItemIndex = activeItemIndex;
      prerenderingNeeded = true;
      if(scrollPosition > activeItemIndex) {
        scrollPosition = max(0, activeItemIndex);
      }
      if(scrollPosition -1 + floor(size.y/itemHeight) < activeItemIndex) {
        scrollPosition = clamp(activeItemIndex + 1 - floor(size.y/itemHeight), 0, maxScrollPosition);
      }
    }
  }

  void clearItems() {
    items.clear();
    activeItemIndex = 0;
    prerenderingNeeded = true;
  }

  void setItems(const std::vector<std::string> &items) {
    this->items = std::vector<std::string>(items);
    activeItemIndex = clamp(activeItemIndex, 0, items.size()-1);
    //inputText = items[activeItemIndex];
    prerenderingNeeded = true;
    updateScrollPosMax();
  }

  void addItems(const std::string item0, const std::string item1 = "", const std::string item2 = "", const std::string item3 = "", const std::string item4 = "", const std::string item5 = "", const std::string item6 = "", const std::string item7 = "", const std::string item8 = "", const std::string item9 = "", const std::string item10 = "", const std::string item11 = "") {
    items.push_back(item0);
    if(item1.size() > 0) items.push_back(item1);
    if(item2.size() > 0) items.push_back(item2);
    if(item3.size() > 0) items.push_back(item3);
    if(item4.size() > 0) items.push_back(item4);
    if(item5.size() > 0) items.push_back(item5);
    if(item6.size() > 0) items.push_back(item6);
    if(item7.size() > 0) items.push_back(item7);
    if(item8.size() > 0) items.push_back(item8);
    if(item9.size() > 0) items.push_back(item9);
    if(item10.size() > 0) items.push_back(item10);
    if(item11.size() > 0) items.push_back(item11);
    //inputText = items[activeItemIndex];
    prerenderingNeeded = true;
    activeItemIndex = clamp(activeItemIndex, 0, items.size()-1);
    updateScrollPosMax();
  }

  void onMousePressed(Events &events) {
    GuiElement::onMousePressed(events);

    if(isPointWithin(events.m)) {
      isInputGrabbed = true;
      prerenderingNeeded = true;
      
      if(events.mouseButton == 0) {
        int index = floor((events.m.y - (absolutePos.y+2)) / itemHeight) + scrollPosition;
        if(events.m.y > 25 && index >= 0 && index < items.size()) {
          activeItemIndex = index;
          onValueChange(this);
        }
        else {
          activeItemIndex = -1;
          onValueChange(this);
        }
      }
    }
    else if(isInputGrabbed) {
      isInputGrabbed = false;
      prerenderingNeeded = true;
    }
  }
  
  void onMouseMotion(Events &events) {
    GuiElement::onMouseMotion(events);
    
    if(isInputGrabbed && isPointWithin(events.m)) {
      if(events.mouseDownL) {
        int index = floor((events.m.y - (absolutePos.y+2)) / itemHeight) + scrollPosition;
        prerenderingNeeded = true;
        if(events.m.y > 25 && index >= 0 && index < items.size() && index != activeItemIndex) {
          activeItemIndex = index;
          onValueChange(this);
        }
      }
    }
  }
  
  
  void onMouseReleased(Events &events) {
    GuiElement::onMouseReleased(events);

    if(isPointWithin(events.m)) {
      if(events.mouseButton == 0) {
        isInputGrabbed = true;
        prerenderingNeeded = true;
        int index = floor((events.m.y - (absolutePos.y+2)) / itemHeight) + scrollPosition;
        if(events.m.y > 25 && index >= 0 && index < items.size()) {
          activeItemIndex = index;
          onValueChange(this);
          onAction(this);
        }
        else {
          activeItemIndex = -1;
          onValueChange(this);
          onAction(this);
        }
      }
    }
  }
  
  void onKeyDown(Events &events) {
    GuiElement::onKeyDown(events);
    if(isInputGrabbed) {
      
      int previousActiveItemIndex = activeItemIndex;
      
      if(events.sdlKeyCode == SDLK_HOME) {
        scrollPosition = 0;
        prerenderingNeeded = true;
      }
      if(events.sdlKeyCode == SDLK_END) {
        scrollPosition = maxScrollPosition;
        prerenderingNeeded = true;
      }
      if(events.sdlKeyCode == SDLK_PAGEUP) {
        scrollPosition = clamp(scrollPosition-(int)(size.y/itemHeight), 0, maxScrollPosition);
        prerenderingNeeded = true;
      }
      if(events.sdlKeyCode == SDLK_PAGEDOWN) {
        scrollPosition = clamp(scrollPosition+(int)(size.y/itemHeight), 0, maxScrollPosition);
        prerenderingNeeded = true;
      }
      
      if(events.sdlKeyCode == SDLK_UP) {
        if(activeItemIndex >= scrollPosition && activeItemIndex < (int)(scrollPosition+size.y/itemHeight)) {
          activeItemIndex = (activeItemIndex+items.size()-1) % items.size();
        }
        else {
          activeItemIndex = clamp((int)(scrollPosition+size.y/itemHeight)-1, 0, items.size()-1);
        }
      }
      if(events.sdlKeyCode == SDLK_DOWN) {
        if(activeItemIndex >= scrollPosition && activeItemIndex < (int)(scrollPosition+size.y/itemHeight)) {
          activeItemIndex = (activeItemIndex+items.size()+1) % items.size();
        }
        else {
          activeItemIndex = clamp(scrollPosition, 0, items.size()-1);
        }
      }
      if(events.sdlKeyCode == SDLK_RIGHT) {
        onAction(this);
      }

      if(activeItemIndex != previousActiveItemIndex) {
        onValueChange(this);
        if(scrollPosition > activeItemIndex) {
          scrollPosition = activeItemIndex;
        }
        if(scrollPosition -1+ floor(size.y/itemHeight) < activeItemIndex) {
          scrollPosition = clamp(activeItemIndex + 1- floor(size.y/itemHeight), 0, maxScrollPosition);
        }
      }
    }
  }

  void onTextInput(Events &events) {
  }
  
  virtual void getTypedValue(int &value) {
    value = activeItemIndex;
  }
  
  void getValue(void *value) {
    if(activeItemIndex >= items.size()) {
      *(int*)value = -1;
    }
    else {
      *(int*)value = activeItemIndex;
    }
  }

  void setValue(int value) {
    prerenderingNeeded = true;

    activeItemIndex = value;
    /*if(activeItemIndex >= items.size()) {
      inputText = "";
    }
    else {
      inputText = items[activeItemIndex];
    }*/
  }

  void onMouseWheel(Events &events) {
    GuiElement::onMouseWheel(events);

    if(!isInputGrabbed) return;
    
    if(events.lControlDown) {
      int previousActiveItemIndex = activeItemIndex;
      activeItemIndex = (activeItemIndex+items.size()+clamp(events.mouseWheel, -1, 1)) % items.size();

      if(activeItemIndex != previousActiveItemIndex) {
        /*if(activeItemIndex >= items.size()) {
          inputText = "";
        }
        else {
          inputText = items[activeItemIndex];
        }*/
        onValueChange(this);
      }
      if(scrollPosition > activeItemIndex) {
        scrollPosition = activeItemIndex;
      }
      if(scrollPosition + floor(size.y/itemHeight) < activeItemIndex) {
        scrollPosition = clamp(activeItemIndex - floor(size.y/itemHeight), 0, maxScrollPosition);
      }
    }
    else {
      scrollPosition = clamp(scrollPosition-events.mouseWheel, 0, maxScrollPosition);
    }
    prerenderingNeeded = true;
    //printf("ScrollableList::onMouseWheel(), %d\n", scrollPosition);
    /*int previousActiveItemIndex = activeItemIndex;
    activeItemIndex = (activeItemIndex+items.size()+clamp(events.mouseWheel, -1, 1)) % items.size();

    if(activeItemIndex != previousActiveItemIndex) {
      if(activeItemIndex >= items.size()) {
        inputText = "";
      }
      else {
        inputText = items[activeItemIndex];
      }
      onValueChange(this);
    }*/
  }

  /*void render(GeomRenderer &geomRenderer, TextGl &textRenderer) {
    GuiElement::render(geomRenderer, textRenderer);

    if(showCursor && isInputGrabbed && (SDL_GetTicks() / 500) % 2 == 0) {
      Vec2d ip = absolutePos + inputPos + padding + inputPadding;
      geomRenderer.strokeColor.set(1, 1, 1, 0.5);
      geomRenderer.strokeType = 1;
      geomRenderer.strokeWidth = 1;
      geomRenderer.drawLine(ip.x + cursorX, ip.y + 2, ip.x + cursorX, ip.y + inputSize.y - 4);
    } 
  }*/

  void onPrerender(GeomRenderer &geomRenderer, TextGl &textRenderer, const Vec2d &displacement) {
    
    geomRenderer.texture = NULL;
    geomRenderer.fillColor = isInputGrabbed ? inputBackgroundColorSelected : inputBackgroundColor;
    geomRenderer.strokeColor = isInputGrabbed ? inputBorderColorSelected : inputBorderColor;
    geomRenderer.strokeType = 1;
    geomRenderer.strokeWidth = 1;
    geomRenderer.drawRect(size, displacement + size*0.5);
    
    Vec2d textPos;
    /*
    textRenderer.setColor(labelColor);
    textRenderer.print(labelText, textPos, labelTextSize);
    textPos.y += itemHeight+2;*/
    
    for(int i=scrollPosition; i<items.size(); i++) {
      if(i == activeItemIndex) {
        geomRenderer.fillColor = activeItemBackgroundColor;
        geomRenderer.strokeColor = activeItemBorderColor;
        geomRenderer.drawRect(itemWidth, itemHeight, displacement.x+textPos.x+itemWidth*0.5, displacement.y+textPos.y+itemHeight*0.5);
        textRenderer.setColor(activeItemTextColor);
      }
      else {
        textRenderer.setColor(inputColor);
      }
            
      textRenderer.print(items[i], displacement+textPos + Vec2d(3, 2), inputTextSize);
      textPos.y += itemHeight;
      
      if(textPos.y+itemHeight >= size.y) break;
    }
    /*if(showScrollBar) {
      geomRenderer.fillColor.set(0.04, 0.04, 0.04, 0.8);
      geomRenderer.strokeColor.set(0.8, 0.8, 0.8, 0.8);
      geomRenderer.drawRect(scrollBarWidth, size.y, size.x - scrollBarWidth*0.5 + displacement.x, size.y*0.5 + displacement.y);
      
      geomRenderer.fillColor.set(0.04, 0.04, 0.04, 0.8);
      geomRenderer.strokeColor.set(0.8, 0.8, 0.8, 0.8);
      
      geomRenderer.drawRect(scrollBarWidth, size.y, size.x - scrollBarWidth*0.5 + displacement.x, size.y*0.5 + displacement.y);
    }*/

  }

};
