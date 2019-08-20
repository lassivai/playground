#pragma once
#include "panel.h"

/* TODO
 * - arbitraty number of graphs on a single panel
 *
 */

struct GraphPanel : public Panel {
  enum Interpolation { NearestNeigbour, Linear };
  Interpolation interpolation = Linear;
  
  std::vector<double> *data = NULL, *data2 = NULL;
  bool showAxes = true;
  int numHorizontalMarks = 3;
  int numVerticalMarks = 0;
  Vec2d horizontalAxisLimits, verticalAxisLimits;
  int numHorizontalAxisLabels = 2, numVerticalAxisLabels = 0;
  std::string horizontalAxisUnit = "", verticalAxisUnit = "";

  Vec4d graphColor = Vec4d(0, 0, 0, 1);
  Vec4d graphColor2 = Vec4d(0, 0, 0, 1);
  //Vec4d graphColor2 = Vec4d(0.15, 0.15, 0.15, 1);

  Vec4d labelColor;

  double graphWidth = 2;

  bool graphAsArea = false;

  std::string title = "", title2 = "";


  virtual ~GraphPanel() {}

  GraphPanel() {}

  GraphPanel(std::vector<double> *data, bool showAxes = true, int numHorizontalMarks = 9, int numVerticalMarks = 0,
  double horizontalAxisMin = 0, double horizontalAxisMax = 1, double verticalAxisMin = 0, double verticalAxisMax = 1,
  int numHorizontalAxisLabels = 2, int numVerticalAxisLabels = 0, std::string horizontalAxisUnit = "", std::string verticalAxisUnit = "") {
    this->data = data;
    this->showAxes = showAxes;
    this->numHorizontalMarks = numHorizontalMarks;
    this->numVerticalMarks = numVerticalMarks;
    this->horizontalAxisLimits.set(horizontalAxisMin, horizontalAxisMax);
    this->verticalAxisLimits.set(verticalAxisMin, verticalAxisMax);
    this->numHorizontalAxisLabels = numHorizontalAxisLabels;
    this->numVerticalAxisLabels = numVerticalAxisLabels;
    this->horizontalAxisUnit = horizontalAxisUnit;
    this->verticalAxisUnit = verticalAxisUnit;
    labelColor.set(0, 0, 0, 1);
  }

  virtual void onPrerender(GeomRenderer &geomRenderer, TextGl &textRenderer, const Vec2d &displacement) {
    geomRenderer.fillColor.set(0.1, 0.1, 0.1, 0.5);
    geomRenderer.strokeColor.set(1, 1, 1, 0.5);
    geomRenderer.strokeWidth = 0;
    geomRenderer.strokeType = 1;
    geomRenderer.drawRect(size, displacement + size * 0.5);
    //geomRenderer.fillColor.set(0.3, 0.3, 0.3, 0.5);
    geomRenderer.fillColor.set(1, 1, 1, 1);
    geomRenderer.strokeColor.set(1, 1, 1, 1);
    geomRenderer.strokeWidth = graphWidth;

    if(graphAsArea) {
      geomRenderer.strokeColor = graphColor;
      if(data && data2) {
        int n = min(data->size(), data2->size());
        
        for(int i=0; i<n; i++) {
          double x = map(i, 0.0, n-1, 6, size.x-6);
          double y1 = map(data2->at(i), verticalAxisLimits.x, verticalAxisLimits.y, size.y - 6, 6);
          double y2 = map(data->at(i), verticalAxisLimits.x, verticalAxisLimits.y, size.y - 6, 6);
          geomRenderer.drawLine(x, y1, x, y2, displacement);
        }
      }
      else if(data) {
        int n = data->size();
        
        for(int i=0; i<n; i++) {
          double x = map(i, 0.0, n-1, 6, size.x-6);
          double y1 = 0;
          double y2 = map(data->at(i), verticalAxisLimits.x, verticalAxisLimits.y, size.y - 6, 6);
          geomRenderer.drawLine(x, y1, x, y2, displacement);
        }
      }
    }
    else {
      Vec2d a, b;
      if(data2) {
        geomRenderer.strokeColor = graphColor2;
        geomRenderer.fillColor = graphColor2;
        for(int i=0; i<data2->size(); i++) {
          a.x = map(i, 0, data2->size()-1, 6, size.x-6);
          a.y = map(data2->at(i), verticalAxisLimits.x, verticalAxisLimits.y, size.y - 6, 6);
          if(i % 24 == 0) {
            geomRenderer.drawRect(5, 5, displacement.x+a.x, displacement.y+a.y, PI*0.25);
          }
          if(i > 0) {
            if(interpolation == Linear) {
              geomRenderer.drawLine(a, b, displacement);
            }
            else if(interpolation == NearestNeigbour) {
              geomRenderer.drawLine(a.x, a.y, b.x, a.y, displacement);
              geomRenderer.drawLine(b.x, a.y, b.x, b.y, displacement);
            }
          }
          b = a;
        }
      }
      if(data) {
        geomRenderer.strokeColor = graphColor;
        for(int i=0; i<data->size(); i++) {
          a.x = map(i, 0, data->size()-1, 6, size.x-6);
          a.y = map(data->at(i), verticalAxisLimits.x, verticalAxisLimits.y, size.y - 6, 6);
          if(i > 0) {
            if(interpolation == Linear) {
              geomRenderer.drawLine(a, b, displacement);
            }
            else if(interpolation == NearestNeigbour) {
              geomRenderer.drawLine(a.x, a.y, b.x, a.y, displacement);
              geomRenderer.drawLine(b.x, a.y, b.x, b.y, displacement);
            }
          }
          b = a;
        }
      }
    }
    geomRenderer.strokeColor.set(0, 0, 0, 0.45);
    geomRenderer.strokeWidth = 2;
    if(showAxes) {
      geomRenderer.drawLine(6, size.y-6, size.x-6, size.y-6, displacement);
      geomRenderer.drawLine(6, 6, 6, size.y-6, displacement);
    }

    for(int i=0; i<numHorizontalMarks; i++) {
      double x = map(i, 0, numHorizontalMarks-1, 6.0, size.x-6);
      double y0 = size.y-3, y1 = size.y-9;
      geomRenderer.drawLine(x, y0, x, y1, displacement);
    }

    for(int i=0; i<numHorizontalAxisLabels; i++) {
      double x;
      if(numHorizontalAxisLabels == 1) {
        x = size.x-6;
      }
      else {
        x = map(i, 0, numHorizontalAxisLabels-1, 6.0, size.x-6);
      }
      
      double y0 = size.y-3, y1 = size.y-9;

      //if(i > 0) geomRenderer.drawLine(x, y0, x, y1, displacement);
      double labelNumber;
      if(numHorizontalAxisLabels == 1) {
        labelNumber = horizontalAxisLimits.y;
      }
      else {
        labelNumber = map(i, 0, numHorizontalAxisLabels-1, horizontalAxisLimits.x, horizontalAxisLimits.y);
      }
      
      char buf[128];
      if(fract(labelNumber) < 0.01) {
        std::sprintf(buf, "%.0f %s", labelNumber, horizontalAxisUnit.c_str());
      }
      else {
        std::sprintf(buf, "%.2f %s", labelNumber, horizontalAxisUnit.c_str());
      }

      Vec2d d = textRenderer.getDimensions(buf, 12);
      double tx = 0;
      if(x + d.x > size.x - 10) {
        tx = size.x - 6 - (x + d.x);
      }
      if(x < 10) {
        tx = 10 - x;
      }
      if(x - d.x/2 > 10 && x + d.x/2 < size.x - 10) {
        tx = -d.x/2;
      }
      textRenderer.setColor(labelColor);
      textRenderer.print(buf, displacement.x + x + tx, displacement.y + y0 - d.y, 12);
    }
    
    if(title.size() > 0) {
      textRenderer.setColor(1, 1, 1, 0.6);
      textRenderer.print(title, displacement.x+10, displacement.y+10, 12);
    }
    if(title2.size() > 0) {
      textRenderer.setColor(1, 1, 1, 0.4);
      textRenderer.print(title2, displacement.x+10, displacement.y+size.y-23, 10);
    }

  }
  
  
};
