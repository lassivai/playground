#pragma once

#include "guielement.h"

struct MenuItem : public GuiElement {
  MenuItem(const std::string &name) : GuiElement(name) {}
  void setText(std::string text) {
    this->name = text;
  }
};

struct MenuButton : public MenuItem {
  //std::string shortcutKeys;
  int textSize = 12;
  Vec4d labelColor = Vec4d(0.75, 0.75, 0.75, 0.75);
  Vec4d labelColorSelected = Vec4d(0.9, 0.9, 0.9, 0.9);
  Vec4d backgroundColor = Vec4d(0, 0, 0, 0);
  Vec4d backgroundColorSelected = Vec4d(0.2, 0.2, 0.2, 0.93);
  
  
  MenuButton(const std::string &name, double width = 200, double height = 23, int textSize = 12) : MenuItem(name) {
    setSize(width, height);
    this->textSize = textSize;
  }
  
  //virtual void onPrerender(GeomRenderer &geomRenderer, TextGl &textRenderer, const Vec2d &displacement) {
  
  // FIXME should be prerendered
  virtual void onRender(GeomRenderer &geomRenderer, TextGl &textRenderer) {
    if(isMouseHover) {
      geomRenderer.fillColor = backgroundColorSelected;
      geomRenderer.strokeType = 0;
      geomRenderer.texture = NULL;
      geomRenderer.drawRectCorner(size, absolutePos);
    }
    textRenderer.setColor(isMouseHover ? labelColorSelected : labelColor);
    textRenderer.print(name, absolutePos.x + 10, absolutePos.y+2, textSize);
  }
  
  void onMousePressed(Events &events) {
    GuiElement::onMousePressed(events);

    if(isPointWithin(events.m)) {
      onAction(this);
    }
  }
};

struct MenuDivider : public MenuItem {
  MenuDivider(double height = 6, const std::string &name = "Menu divider") : MenuItem(name) {
    setSize(height, height);
  }
  
  virtual void onPrerender(GeomRenderer &geomRenderer, TextGl &textRenderer, const Vec2d &displacement) {
    geomRenderer.strokeColor.set(1, 1, 1, 0.1);
    geomRenderer.strokeType = 1;
    geomRenderer.strokeWidth = 1;
    geomRenderer.drawLine(10, size.y*0.5, size.x-10, size.y*0.5, displacement);
  }
};



struct Menu : public GuiElement
{
  int defaultTextSize = 12;
  std::string text;
  //bool textChanged = true;
  Vec4d labelColor;
  Vec4d labelColorSelected;
  Vec4d backgroundColor;
  Vec4d backgroundColorSelected;
  //Vec2d padding = Vec2d(1, 1);
  //Texture *labelTexture = NULL;
  //Texture *glowTexture = NULL;

  //std::string title;


  virtual ~Menu() {
    releaseMouse();
  }
  
  //std::function<double()> getGlowLevel = []() { return 1; };

  double minWidth = 200;

  Menu(const std::string &name, double minWidth = 200, double x = 0, double y = 0, int defaultTextSize = 12) : GuiElement(name) {
    this->minWidth = minWidth;
    this->pos.set(x, y);
    this->defaultTextSize = defaultTextSize;
    //this->overlayColor = overlayColor;
    setSize(minWidth, 23);
    
    drawShadow = true;
    setVisible(false);
    zLayer = 10;
  }
  
  virtual void setVisible(bool isVisible) {
    GuiElement::setVisible(isVisible);
    
    if(isVisible && !mouseCapturedForChildren) {
      captureMouseForChildren();
    }
    if(!isVisible && mouseCapturedForChildren) {
      releaseMouse();
    }
    closeRequested = false;
    readyToReceiveEvents = false;
  }

  MenuButton *addMenuButton(const std::string &text) {
    MenuButton *menuButton = new MenuButton(text, minWidth, defaultTextSize*2.2);
    addChildElement(menuButton);
    return menuButton;
  }
  MenuDivider *addMenuDivider() {
    MenuDivider *menuDivider = new MenuDivider(6);
    addChildElement(menuDivider);
    return menuDivider;
  }

  virtual void addChildElement(GuiElement *guiElement) {
    GuiElement::addChildElement(guiElement);
    
    double w = minWidth, h = 0;
    
    for(int i=0; i<children.size(); i++) {
      children[i]->setPosition(0, h);
      w = max(w, children[i]->size.x);
      h += children[i]->size.y;
    }
    for(int i=0; i<children.size(); i++) {
      if(dynamic_cast<MenuItem*>(children[i])) {
        children[i]->setSize(w, children[i]->size.y);
      }
    }
    setSize(w, h);
  }

  
  virtual void onPrepareShadowTexture(GeomRenderer &geomRenderer) {
    if(drawShadow) {
      if(!shadowTexture) {
        shadowWidth = 30;
        shadowDisplacement.set(0, 0);
        shadowTexture = createBoxShadowTexture(geomRenderer, size.x, size.y, shadowWidth, 0.7, 3);
      }
      else if(shadowTexture->w != int(size.x+shadowWidth*2) || shadowTexture->h != int(size.y+shadowWidth*2)) {
        delete shadowTexture;
        shadowTexture = createBoxShadowTexture(geomRenderer, size.x, size.y, shadowWidth, 0.7, 3);
      }
      //shadowPosition.set(inputPos + padding);
    }
  }
  
  virtual void onPrerender(GeomRenderer &geomRenderer, TextGl &textRenderer, const Vec2d &displacement) {
    geomRenderer.texture = NULL;
    geomRenderer.fillColor.set(0., 0., 0., 0.95);
    geomRenderer.strokeColor.set(0.5, 0.5, 0.5, 0.95);
    geomRenderer.strokeType = 1;
    geomRenderer.strokeWidth = 1;
    geomRenderer.drawRectCorner(size, displacement);
    //Vec2d d(0, geomRenderer.screenH - prerenderedGuiElement.h);

    //GuiElement::render(geomRenderer, textRenderer);
    /*Vec2d dim = textRenderer.getDimensions(text, textSize);
    textRenderer.setColor(labelColor);
    textRenderer.print(text, displacement + size*0.5 - dim*0.5, textSize);*/
    /*printf("%s: %f x %f, %f %f\n", name.c_str(), size.x, size.y, pos.x, pos.y);
    for(int i=0; i<children.size(); i++) {
      printf("%d %s: %f x %f, %f %f\n", i, children[i]->name.c_str(), children[i]->size.x, children[i]->size.y, children[i]->pos.x, children[i]->pos.y);
    }*/
  }
  
  virtual void onPrepare(GeomRenderer &geomRenderer, TextGl &textRenderer) {
  }
  
  virtual void onRender(GeomRenderer &geomRenderer, TextGl &textRenderer) {
  }
  
  virtual void onMousePressed(Events &events) {
    GuiElement::onMousePressed(events);
    
    if(readyToReceiveEvents && !isPointWithin(events.m)) {
      closeRequested = true;
    }
  }
};
