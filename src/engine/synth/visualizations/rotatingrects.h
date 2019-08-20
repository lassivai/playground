#pragma once
#include "../../geom_gfx.h"
#include "../../arr.h"
#include "../../util.h"
#include "../../mathl.h"
#include "../../gui/gui.h"

struct RotatingRectVisualization {
  bool isVisible = false;
  int numRects = 50;
  double rectSize1 = 0.05, rectSize2 = 0.4;
  double strokeWidth = 2.0;
  bool inverseSpectrum = true;
  double rotatingSpeed = 0.01;
  Vec4d rectFillColor = Vec4d(1, 1, 1, 0.5);
  Vec4d rectStrokeColor = Vec4d(1, 1, 1, 0.5);
  double averaging = 0;
  std::vector<double> rectAngles, rectSpectrumAssociations;
  bool symmetryPartner = false;
  int averagingMode = 0;
  
  int numStops = 0;
  double stoppingSpeed = 0;
  
  void init(GuiElement *parentGuiElement) {
    rotatingRectVisualizationPanel = new RotatingRectVisualizationPanel();
    rotatingRectVisualizationPanel->init(parentGuiElement, this);
    rotatingRectVisualizationPanel->setPosition(0, parentGuiElement->size.y + 5);
    rotatingRectVisualizationPanel->setVisible(false);
  }
  
  void update(GeomRenderer &geomRenderer, const std::vector<double> &spectrumGraph, int w, int h) {
    if(isVisible) {      
      if(rectAngles.size() != numRects) {
        rectAngles.assign(numRects, 0); 
        rectSpectrumAssociations.assign(numRects, 0); 
      }
      resizeArray(spectrumGraph, rectSpectrumAssociations);
      
      Vec2d mp(w/2, h/2);
      geomRenderer.fillColor.set(rectFillColor);
      geomRenderer.strokeColor.set(rectStrokeColor);
      geomRenderer.strokeWidth = strokeWidth;
      geomRenderer.strokeType = 1;
      geomRenderer.texture = NULL;
      
      double k = min(w, h);
      double averageAngle = 0;
      
      if(averagingMode == 2) {
        for(int i=0; i<numRects; i++) {
          averageAngle += rectAngles[i];
        }
        averageAngle = modfff(averageAngle/numRects, 2.0*PI);
      }
      
      for(int i=0; i<numRects; i++) {
        double t;
        t = map(i, 0, numRects, rectSize1, rectSize2) * k;
        int ind = inverseSpectrum ? i : numRects-1 - i;
        rectAngles[ind] += rectSpectrumAssociations[ind] * rotatingSpeed;
        rectAngles[ind] = modfff(rectAngles[ind], 0.5*PI);
        
        geomRenderer.drawRect(t, t, mp.x, mp.y, rectAngles[ind]);
        if(symmetryPartner) {
          geomRenderer.drawRect(t, t, mp.x, mp.y, -rectAngles[ind]);
        }
        
        if(averagingMode > 0) {
          if(averagingMode == 1) {
            rectAngles[ind] = rectAngles[ind] * (1.0-averaging*2.0/3.0)
                         + angleBetween(rectAngles[ind], rectAngles[max(0, ind-1)]) * averaging/3.0
                         + angleBetween(rectAngles[ind], rectAngles[min(numRects-1, ind+1)]) * averaging/3.0;
          }
          else if(averagingMode == 2) {
            rectAngles[ind] = rectAngles[ind] * (1.0-averaging) + angleBetween(rectAngles[ind], averageAngle) * averaging;
          }
          else { // FIXME
            rectAngles[ind] = rectAngles[ind] * (1.0-averaging*2.0) + modfff(rectAngles[max(0, ind-1)], 0.5*PI) * averaging + modfff(rectAngles[min(numRects-1, ind+1)], 0.5*PI) * averaging;
          }
        }
      }
    }
  }
  
  struct RotatingRectVisualizationPanel : public Panel {
    //CheckBox *showGui = NULL;
    NumberBox *numRectsGui = NULL;
    NumberBox *rectSize1Gui = NULL;
    NumberBox *rectSize2Gui = NULL;
    NumberBox *strokeWidthGui = NULL;
    CheckBox *inverseSpectrumGui = NULL;
    NumberBox *rotatingSpeedGui = NULL;
    ColorBox *rectFillColorGui = NULL;
    ColorBox *rectStrokeColorGui = NULL;
    CheckBox *symmetryPartnerGui = NULL;
    NumberBox *averagingGui = NULL;
    NumberBox *averagingModeGui = NULL;
    
    RotatingRectVisualization *rotatingRectVisualization = NULL;
    
    RotatingRectVisualizationPanel() : Panel("RotatingRectVisualization Panel") {}
    
    void init(GuiElement *parentGuiElement, RotatingRectVisualization *rotatingRectVisualization) {
      this->rotatingRectVisualization = rotatingRectVisualization;
      double line = -13, lineHeight = 23;
      parentGuiElement->addChildElement(this);
      addGuiEventListener(new PanelListener(this));
      //addChildElement(showGui = new CheckBox("Show", rotatingRectVisualization->isVisible, 10, line+=lineHeight));
      addChildElement(numRectsGui = new NumberBox("Count", rotatingRectVisualization->numRects, NumberBox::INTEGER, 0, 1<<29, 10, line+=lineHeight));
      addChildElement(rectSize1Gui = new NumberBox("Min size", rotatingRectVisualization->rectSize1, NumberBox::FLOATING_POINT, 0, 1e10, 10, line+=lineHeight));
      addChildElement(rectSize2Gui = new NumberBox("Max size", rotatingRectVisualization->rectSize2, NumberBox::FLOATING_POINT, 0, 1e10, 10, line+=lineHeight));
      addChildElement(inverseSpectrumGui = new CheckBox("Inverse", rotatingRectVisualization->inverseSpectrum, 10, line+=lineHeight));
      addChildElement(rotatingSpeedGui = new NumberBox("Speed", rotatingRectVisualization->rotatingSpeed, NumberBox::FLOATING_POINT, -1e10, 1e10, 10, line+=lineHeight));
      addChildElement(averagingModeGui = new NumberBox("Averaging mode", rotatingRectVisualization->averagingMode, NumberBox::INTEGER, 0, 3, 10, line+=lineHeight, 8));
      addChildElement(averagingGui = new NumberBox("Averaging", rotatingRectVisualization->averaging, NumberBox::FLOATING_POINT, 0, 1, 10, line+=lineHeight));
      addChildElement(rectFillColorGui = new ColorBox("Fill color", rotatingRectVisualization->rectFillColor, 10, line+=lineHeight));
      addChildElement(rectStrokeColorGui = new ColorBox("Stroke color", rotatingRectVisualization->rectStrokeColor, 10, line+=lineHeight));
      addChildElement(strokeWidthGui = new NumberBox("Stroke width", rotatingRectVisualization->strokeWidth, NumberBox::FLOATING_POINT, 0, 1e10, 10, line+=lineHeight));
      addChildElement(symmetryPartnerGui = new CheckBox("Symmetry partner", rotatingRectVisualization->symmetryPartner, 10, line+=lineHeight));
      
      
      setSize(320, line+lineHeight+10);
    }
    struct PanelListener : public GuiEventListener {
      RotatingRectVisualizationPanel *rotatingRectVisualizationPanel;
      PanelListener(RotatingRectVisualizationPanel *rotatingRectVisualizationPanel) {
        this->rotatingRectVisualizationPanel = rotatingRectVisualizationPanel;
      }
      void onValueChange(GuiElement *guiElement) {
        if(guiElement == rotatingRectVisualizationPanel->numRectsGui) {
          guiElement->getValue(rotatingRectVisualizationPanel->rotatingRectVisualization->numRects);
        }
        if(guiElement == rotatingRectVisualizationPanel->rectSize1Gui) {
          guiElement->getValue(rotatingRectVisualizationPanel->rotatingRectVisualization->rectSize1);
        }
        if(guiElement == rotatingRectVisualizationPanel->rectSize2Gui) {
          guiElement->getValue(rotatingRectVisualizationPanel->rotatingRectVisualization->rectSize2);
        }
        if(guiElement == rotatingRectVisualizationPanel->strokeWidthGui) {
          guiElement->getValue(rotatingRectVisualizationPanel->rotatingRectVisualization->strokeWidth);
        }
        if(guiElement == rotatingRectVisualizationPanel->inverseSpectrumGui) {
          guiElement->getValue(rotatingRectVisualizationPanel->rotatingRectVisualization->inverseSpectrum);
        }
        if(guiElement == rotatingRectVisualizationPanel->rotatingSpeedGui) {
          guiElement->getValue(rotatingRectVisualizationPanel->rotatingRectVisualization->rotatingSpeed);
        }
        if(guiElement == rotatingRectVisualizationPanel->rectFillColorGui) {
          guiElement->getValue(rotatingRectVisualizationPanel->rotatingRectVisualization->rectFillColor);
        }
        if(guiElement == rotatingRectVisualizationPanel->rectStrokeColorGui) {
          guiElement->getValue(rotatingRectVisualizationPanel->rotatingRectVisualization->rectStrokeColor);
        }
        if(guiElement == rotatingRectVisualizationPanel->symmetryPartnerGui) {
          guiElement->getValue(rotatingRectVisualizationPanel->rotatingRectVisualization->symmetryPartner);
        }
        if(guiElement == rotatingRectVisualizationPanel->averagingGui) {
          guiElement->getValue(rotatingRectVisualizationPanel->rotatingRectVisualization->averaging);
        }
        if(guiElement == rotatingRectVisualizationPanel->averagingModeGui) {
          guiElement->getValue(rotatingRectVisualizationPanel->rotatingRectVisualization->averagingMode);
        }
      }
    };
  };
  RotatingRectVisualizationPanel *rotatingRectVisualizationPanel = NULL;
};
