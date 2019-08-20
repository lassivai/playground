#pragma once
#include "../gui/gui.h"

/* TODO
 * - mandelbrot etc.
 *
 */


struct CanvasShader : public PanelInterface
{
  virtual ~CanvasShader() {}
  virtual void init() {}
  virtual void init(Events &events, int w, int h) {}
  virtual void loadShader() {}
  virtual void render(double w, double h, double time, const Events &events) {}
  virtual Panel *addPanel(GuiElement *parentElement, const std::string &title = "") = 0;
  virtual void removePanel(GuiElement *parentElement) = 0;
  virtual void updatePanel() = 0;
  virtual Panel *getPanel() = 0;
  virtual void setActive(bool isActive) {}
};
