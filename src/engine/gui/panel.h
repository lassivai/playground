#pragma once

#include "guielement.h"
#include <unordered_map>

// FIXME
static std::unordered_map<std::string, Vec2d> previousPanelPositions;

struct Panel : public GuiElement
{
  bool draggable = true;

  bool dragging = false;

  Panel() : GuiElement("unnamed") {}

  Panel(double w, double h, double x = 0, double y = 0) : GuiElement("unnamed") {
    size.set(w, h);
    pos.set(x, y);
    drawBackground = true;
    drawBorder = true;
  }

  Panel(const Vec2d &size, const Vec2d &pos = Vec2d::Zero) : GuiElement("unnamed") {
    this->size = size;
    this->pos = pos;
    drawBackground = true;
    drawBorder = true;
  }

  Panel(std::string name, double w, double h, double x, double y) : GuiElement(name) {
    size.set(w, h);
    pos.set(x, y);
    drawBackground = true;
    drawBorder = true;
  }

  Panel(std::string name) : GuiElement(name) {
    drawBackground = true;
    drawBorder = true;
  }
  
  virtual ~Panel() {
    if(name.compare("unnamed") != 0) {
      previousPanelPositions[name] = pos;
    }
  }

  bool setPreviousPosition() {
    if(previousPanelPositions.count(name) > 0) {
      pos = previousPanelPositions[name];
      return true;
    }
    return false;
  }

  void onMouseMotion(Events &events) {
    GuiElement::onMouseMotion(events);

    if(draggable && dragging) {
      pos += events.m - events.mp;
    }
  }

  void onMousePressed(Events &events) {
    GuiElement::onMousePressed(events);

    if(draggable && events.mouseDownL) {
      //if(isPointWithin(events.m) && !isPointWithinChildElements(events.m)) {
      if(isPointWithin(events.m) && !isInputBlockedInChildren()) {
        dragging = true;
        isInputGrabbed = true;
      }
    }
  }

  void onMouseReleased(Events &events) {
    GuiElement::onMouseReleased(events);
    dragging = false;
    isInputGrabbed = false;
  }
  

  virtual void onPrepareShadowTexture(GeomRenderer &geomRenderer) {
    if(drawShadow) {
      if(!shadowTexture) {
        shadowTexture = createBoxShadowTexture(geomRenderer, size.x, size.y, shadowWidth);
      }
      else if(shadowTexture->w != int(size.x+shadowWidth*2) || shadowTexture->h != int(size.y+shadowWidth*2)) {
        delete shadowTexture;
        shadowTexture = createBoxShadowTexture(geomRenderer, size.x, size.y, shadowWidth);
      }
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
};
