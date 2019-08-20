#pragma once

#include "textbox.h"


struct ListBox : public TextBox
{
  std::vector<std::string> items;
  int activeItemIndex = 0;

  virtual ~ListBox() {}

  ListBox(const std::string &labelText, double x = 0, double y = 0, int inputTextWidth = 15) : TextBox(labelText, "", x, y, inputTextWidth) {
    showCursor = false;
  }
  ListBox(const std::string &labelText, LayoutPlacer &layoutPlacer, int inputTextWidth = 15) : TextBox(labelText, "", layoutPlacer, inputTextWidth) {
    showCursor = false;
  }

  void clearItems() {
    items.clear();
    activeItemIndex = 0;
    prerenderingNeeded = true;
  }

  void setItems(const std::vector<std::string> &items) {
    this->items = std::vector<std::string>(items);
    activeItemIndex = 0;
    inputText = items[activeItemIndex];
    prerenderingNeeded = true;
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
    inputText = items[activeItemIndex];
    prerenderingNeeded = true;
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
    if(activeItemIndex >= items.size()) {
      inputText = "";
    }
    else {
      inputText = items[activeItemIndex];
    }
  }

  void onMouseWheel(Events &events) {
    GuiElement::onMouseWheel(events);

    if(!isInputGrabbed) return;

    int previousActiveItemIndex = activeItemIndex;
    activeItemIndex = (activeItemIndex+items.size()+clamp(events.mouseWheel, -1, 1)) % items.size();

    if(activeItemIndex != previousActiveItemIndex) {
      if(activeItemIndex >= items.size()) {
        inputText = "";
      }
      else {
        inputText = items[activeItemIndex];
      }
      onValueChange(this);
    }
  }

};
