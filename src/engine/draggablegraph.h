#pragma once
#include "gui/gui.h"
#include "geom_gfx.h"
#include "textgl.h"
#include "mathl.h"
#include "textfileparser.h"

struct DraggableGraph : public HierarchicalTextFileParser {
  //static constexpr const std::string ClassName = "draggableGraph";
  
  struct DraggableGraphPanel : public Panel {
    
    DraggableGraph *draggableGraph = NULL;
    
    
    virtual ~DraggableGraphPanel() {}
 
    DraggableGraphPanel(DraggableGraph *draggableGraph) {
      this->draggableGraph = draggableGraph;
      setSize(draggableGraph->w + draggableGraph->borderWidth * 2, draggableGraph->h + draggableGraph->borderWidth * 2);
      addGuiEventListener(new DraggableGraphPanelListener(this));
    }
    virtual void onRender(GeomRenderer &geomRenderer, TextGl &textRenderer) {
      if(draggableGraph->isInfoHover) {
        Vec2d displ = absolutePos + Vec2d(draggableGraph->borderWidth, draggableGraph->borderWidth);
        
        geomRenderer.texture = NULL;
        geomRenderer.strokeType = 1;
        geomRenderer.fillColor.set(0., 0., 0, 0.4);
          
        double x = round(map(draggableGraph->infoHoverDataPoint, 0.0, draggableGraph->numDataPoints-1, 0.0, draggableGraph->w-1));
        double y = map(draggableGraph->infoHoverValue, draggableGraph->verticalLimits.x, draggableGraph->verticalLimits.y, 0, 1.0);
        
        geomRenderer.strokeWidth = 1;
        geomRenderer.strokeColor.set(0, 0, 0, 0.4);
        geomRenderer.drawLine(x, draggableGraph->h-1, x, draggableGraph->h - y * draggableGraph->h, displ);
        
        geomRenderer.strokeWidth = 2;
        geomRenderer.strokeColor.set(0, 0, 0, 1);
        geomRenderer.drawRect(6, 6, x+displ.x, draggableGraph->h - y * draggableGraph->h+displ.y, PI*0.25);

        
        geomRenderer.strokeWidth = 1;
        geomRenderer.strokeColor.set(1, 1, 1, 0.5);
        geomRenderer.fillColor.set(0, 0, 0, 0.75);
        
        std::string eventStr = std::to_string(draggableGraph->infoHoverDataPoint) + " - " + std::to_string(draggableGraph->infoHoverValue);
        Vec2d eventStrSize = textRenderer.getDimensions(eventStr, 10);
        Vec2d size = eventStrSize + Vec2d(10, 0);
        geomRenderer.drawRectCorner(size.x, size.y, draggableGraph->infoHoverPos.x+15, draggableGraph->infoHoverPos.y+20);
        textRenderer.setColor(1, 1, 1, 0.9);
        textRenderer.print(eventStr, draggableGraph->infoHoverPos.x+3+15, draggableGraph->infoHoverPos.y+20, 10);
      }
    }
    
    virtual void onPrerender(GeomRenderer &geomRenderer, TextGl &textRenderer, const Vec2d &displacement) {
      Vec2d displ = displacement + Vec2d(draggableGraph->borderWidth, draggableGraph->borderWidth);
      geomRenderer.fillColor.set(0.1, 0.1, 0.1, 0.5);
      geomRenderer.strokeWidth = 0;
      geomRenderer.strokeType = 1;
      geomRenderer.drawRect(size, displacement + size * 0.5);
      
      if(draggableGraph->title.size() > 0) {
        textRenderer.setColor(1, 1, 1, 1);
        textRenderer.print(draggableGraph->title, displacement.x + 12, displacement.y + 12, 12);
      }
      
      draggableGraph->onDraw(geomRenderer, textRenderer, displ);
      
      if(draggableGraph->gridDensity > 0) {
        geomRenderer.strokeColor.set(0, 0, 0, 0.8);
        geomRenderer.strokeWidth = 1;
        
        int minY = (int)draggableGraph->verticalLimits.x;
        int maxY = (int)draggableGraph->verticalLimits.y + 1;
        double density = 1;
        if(abs(maxY-minY) * draggableGraph->gridDensity < draggableGraph->h / 10) {
          density = 1.0/draggableGraph->gridDensity;
        }
        if(abs(maxY - minY) < draggableGraph->h / 10) {
          for(double y=minY; y<=maxY; y+=density) {
            if(y < draggableGraph->verticalLimits.x || y > draggableGraph->verticalLimits.y) continue;
            double ym = map(y, draggableGraph->verticalLimits.x, draggableGraph->verticalLimits.y, 0.0, draggableGraph->h-1);
            if(fabs(fract(y+1e6)) < density*0.5) geomRenderer.strokeWidth = 2.5;
            else geomRenderer.strokeWidth = 0.5;
            geomRenderer.drawLine(0, draggableGraph->h-1-ym, draggableGraph->w-1, draggableGraph->h-1-ym, displ);
          }
        }
      }
      

      geomRenderer.fillColor.set(1, 1, 1, 1);
      geomRenderer.strokeColor.set(1, 1, 1, 1);
      geomRenderer.strokeWidth = 2;
      
      Vec2d scale(draggableGraph->w, draggableGraph->h);
      
      for(int i=0; i<draggableGraph->w-1; i++) {
        geomRenderer.drawLine(draggableGraph->drawableCurve[i] * scale, draggableGraph->drawableCurve[i+1] * scale, displ);
      }
      
      /*geomRenderer.fillColor.set(0.3, 0.3, 1, 1);
      geomRenderer.strokeColor.set(0.3, 0.3, 1, 0.8);
      
      if(draggableGraph->w > draggableGraph->numDataPoints*2) {
        for(int i=0; i<draggableGraph->numDataPoints; i++) {
          double x = round(map(i, 0.0, draggableGraph->numDataPoints-1, 0.0, draggableGraph->w-1));
          if(draggableGraph->drawDataPointLines) {
            geomRenderer.strokeWidth = 1;
            geomRenderer.strokeColor.set(1, 1, 1, 0.4);
            geomRenderer.drawLine(x, draggableGraph->h-1, x, draggableGraph->dataPointsOnCurve[i].y*scale.y, displ);
          }
          if(draggableGraph->w / draggableGraph->numDataPoints >= draggableGraph->dataPointMarkSize) {
            geomRenderer.strokeWidth = 2;
            geomRenderer.strokeColor.set(0, 0, 0, 1);
            geomRenderer.drawRect(draggableGraph->dataPointMarkSize, draggableGraph->dataPointMarkSize, x+displ.x, draggableGraph->dataPointsOnCurve[i].y*scale.y+displ.y, PI*0.25);
          }
        }
      }*/
      geomRenderer.strokeWidth = 1;
      geomRenderer.strokeColor.set(0, 0, 0, 1);
      geomRenderer.fillColor.set(1, 1, 1, 1);
      for(int i=0; i<draggableGraph->curve.size(); i++) {
        double r = 4;
        
        if(i == draggableGraph->closestPointIndex || i == draggableGraph->draggingPointIndex) {
          r = 6;
        }
        geomRenderer.drawCircle(r, draggableGraph->curve[i]*scale + displ);
      }

      draggableGraph->graphRendered = true;
    }

    struct DraggableGraphPanelListener : public GuiEventListener {
      DraggableGraphPanel *draggableGraphPanel = NULL;
      
      DraggableGraphPanelListener(DraggableGraphPanel *draggableGraphPanel) {
        this->draggableGraphPanel = draggableGraphPanel;
      }
      virtual void onMousePressed(GuiElement *guiElement, Events &events) {
        if(guiElement != draggableGraphPanel || !guiElement->isPointWithin(events.m)) return;
        if(draggableGraphPanel->draggableGraph->onMousePressed(events)) {
          draggableGraphPanel->dragging = false;
          draggableGraphPanel->isInputGrabbed = true;
        }
      }
      virtual void onMouseReleased(GuiElement *guiElement, Events &events) {
        if(guiElement != draggableGraphPanel) return;
        draggableGraphPanel->draggableGraph->onMouseReleased(events);
        draggableGraphPanel->isInputGrabbed = false;
      }
      virtual void onMouseMotion(GuiElement *guiElement, Events &events) {
        if(guiElement != draggableGraphPanel) return;
        draggableGraphPanel->draggableGraph->onMouseMotion(events);
        if(draggableGraphPanel->draggableGraph->draggingPointIndex != -1) {
          draggableGraphPanel->dragging = false;
        }
      }
      virtual void onMouseWheel(GuiElement *guiElement, Events &events) {
        if(guiElement != draggableGraphPanel || !guiElement->isPointWithin(events.m)) return;
        draggableGraphPanel->draggableGraph->onMouseWheel(events);
        draggableGraphPanel->isInputGrabbed = false;
      }
    };
  };

  virtual ~DraggableGraph() {}
  
  Vec2d horizontalLimits, verticalLimits;
  
  int numDataPoints = 0;
  //int numFreeCurvePoints = 0;
  std::vector<CurvePoint> curve;
  std::vector<Vec2d> drawableCurve;
  std::vector<Vec2d> dataPointsOnCurve;

  int draggingPointIndex = -1;
  int closestPointIndex = -1;
  double maxDragDistance = 10;

  double w = 500, h = 500;
  double borderWidth = 6;

  DraggableGraphPanel *draggableGraphPanel = NULL;
  GuiElement *parentGuiElement = NULL;
  
  int gridDensity = 0;
  
  double dataPointMarkSize = 7;
  bool drawDataPointLines = false;
  
  std::string title = "";
  
  bool isInfoHover = false;
  int infoHoverDataPoint = 0;
  double infoHoverValue = 0;
  Vec2d infoHoverPos;
  
  
  DraggableGraph() {
    verticalLimits.set(0, 1);
    drawableCurve.resize(w);
    dataPointsOnCurve.resize(numDataPoints);
    curve.resize(2);
  }
  
  virtual void setCurvePoint(int index, double x, double y) {
    if(index >= 0 && index < curve.size()) {
      
      //curve[index].x = clamp(x*(w-1.0), 0, w-1.0);
      //curve[index].y = clamp((1.0-y)*(h-1.0), 0, h-1.0);
      curve[index].x = clamp(x, 0, 1.0);
      curve[index].y = clamp(1.0-y, 0, 1.0);
    }
  }
  
  virtual double getDataPointValue(int index) {
    if(index >= 0 && index < dataPointsOnCurve.size()) {
      if(fabs(verticalLimits.x - verticalLimits.y) != 0.0) {
        return map(1-dataPointsOnCurve[index].y, 0.0, 1, verticalLimits.x, verticalLimits.y);
      }
    }
    return verticalLimits.x;
  }
  
  virtual void update() {
    if(drawableCurve.size() != w) {
      drawableCurve.resize(w);
    }
    if(dataPointsOnCurve.size() != numDataPoints) {
      dataPointsOnCurve.resize(numDataPoints);
    }
    if(curve.size() < 2) {
      curve.resize(2);
      curve[0].set(0, 1);
      curve[1].set(1, 1);
    }
    /*printf("draggraph '%s' update:\n", title.c_str());
    for(int i=0; i<curve.size(); i++) {
      printf("%f, %f, %f\n", curve[i].x, curve[i].y, curve[i].cubicInterpolationCurvature);
    }*/
    Vec2d draggingPoint;
    if(draggingPointIndex != -1) draggingPoint = curve[draggingPointIndex];
    
    std::vector<double> curveXs(curve.size());
    for(int i=0; i<curve.size(); i++) {
      curveXs[i] = curve[i].x;
    }
    insertionSort(curveXs, curve);
    
    
    if(draggingPointIndex != -1) {
      for(int i=0; i<curve.size(); i++) {
        if(draggingPoint == curve[i]) {
          draggingPointIndex = i;
          break;
        }
      }
    }
    
    int currentCurvePoint = 0;
    Vec2d p, pp;
    for(int i=0; i<w; i++) {
      for(int k=0; k<curve.size()-1; k++) {
        if(i >= curve[k].x*(w-1.0) && i <= curve[k+1].x * (w-1.0)) {
          currentCurvePoint = k;
          break;
        }
      }
      double fraction = (i/(w-1.0) - curve[currentCurvePoint].x) / (curve[currentCurvePoint+1].x - curve[currentCurvePoint].x);
      
      Vec2d p0 = curve[max(currentCurvePoint-1, 0)];
      Vec2d p1 = curve[currentCurvePoint];
      Vec2d p2 = curve[min(currentCurvePoint+1, curve.size()-1)];
      Vec2d p3 = curve[min(currentCurvePoint+2, curve.size()-1)];

      Vec2d p = catmullRom(fraction, curve[currentCurvePoint].cubicInterpolationCurvature, p0, p1, p2, p3);
      drawableCurve[i] = p;
      //if(i > 0) geomRenderer.drawLine(p, pp, displ);
      //pp = p;
    }
    
    for(int i=0; i<numDataPoints; i++) {
      double x = map(i, 0.0, numDataPoints-1, 0, w-1);
      //currentCurvePoint = clamp(int(x), 0, w-1);
      for(int k=0; k<drawableCurve.size()-1; k++) {
        if(x >= drawableCurve[k].x*(w-1.0) && x <= drawableCurve[k+1].x * (w-1.0)) {
          currentCurvePoint = k;
          break;
        }
      }
      
      double fraction = (x/(w-1.0) - drawableCurve[currentCurvePoint].x) / (drawableCurve[currentCurvePoint+1].x - drawableCurve[currentCurvePoint].x);
      
      Vec2d p1 = drawableCurve[currentCurvePoint];
      Vec2d p2 = drawableCurve[currentCurvePoint+1];

      dataPointsOnCurve[i].y = clamp(p1.y * (1.0-fraction) + p2.y * fraction, min(p1.y, p2.y), max(p1.y, p2.y));
    }
    /*for(int i=0; i<numDataPoints; i++) {
      double x = map(i, 0.0, numDataPoints-1, 0, w-1);
      
      for(int k=0; k<curve.size()-1; k++) {
        if(x >= curve[k].x && x <= curve[k+1].x) {
          currentCurvePoint = k;
          break;
        }
      }
      //printf("debugging at DraggableGraph.update() 6\n");
      //double t = x * cos(atan2(curve[currentCurvePoint+1].y - curve[currentCurvePoint].y, curve[currentCurvePoint+1].x - curve[currentCurvePoint].x));
      double fraction = (x - curve[currentCurvePoint].x) / (curve[currentCurvePoint+1].x - curve[currentCurvePoint].x);
      
      Vec2d p0 = curve[max(currentCurvePoint-1, 0)];
      Vec2d p1 = curve[currentCurvePoint];
      Vec2d p2 = curve[min(currentCurvePoint+1, curve.size()-1)];
      Vec2d p3 = curve[min(currentCurvePoint+2, curve.size()-1)];
      //printf("debugging at DraggableGraph.update() 7\n");
      Vec2d p = catmullRom(fraction, 0.5, p0, p1, p2, p3);
      dataPointsOnCurve[i] = p;
      printf("p %d: %f\n", i, p.y);
      //if(i > 0) geomRenderer.drawLine(p, pp, displ);
      //pp = p;
    }*/
    //printf("debugging at DraggableGraph.update() 7\n");
    if(draggableGraphPanel) {
      draggableGraphPanel->prerenderingNeeded = true;
    }
    
    onUpdate();

  }
  
  
  void draw() {
    if(draggableGraphPanel) {
      draggableGraphPanel->prerenderingNeeded = true;
    }
  }
  
  // FIXME
  virtual void onDraw(GeomRenderer &geomRenderer, TextGl &textRenderer, const Vec2d &displacement) {}
  virtual void onUpdate() {}
  
  virtual void init(GuiElement *parentGuiElement) {
    this->parentGuiElement = parentGuiElement;
    draggableGraphPanel = new DraggableGraphPanel(this);
    parentGuiElement->addChildElement(draggableGraphPanel);
  }
  
  bool visible = false;
  bool graphRendered = false;
  
  virtual void setVisible(bool visible) {
    this->visible = visible;
    if(visible) {
      update();
    }
    else {
      graphRendered = false;
    }
    if(draggableGraphPanel) {
      draggableGraphPanel->setVisible(visible);
    }
  }
  virtual void toggleVisibility() {
    setVisible(!visible);
  }
  virtual void setLimits(double xmin, double xmax, double ymin, double ymax) {
    horizontalLimits.set(xmin, xmax);
    verticalLimits.set(ymin, ymax);
    update();
  }
  virtual void setVerticalLimits(double ymin, double ymax) {
    verticalLimits.set(ymin, ymax);
    update();
  }
  virtual void setHorizontalLimits(double xmin, double xmax) {
    horizontalLimits.set(xmin, xmax);
    update();
  }
  virtual void setNumDataPoints(int numDataPoints) {
    this->numDataPoints = numDataPoints;
    update();
  }
  
  /*void setNumFreeCurvePoints(int numFreeCurvePoints) {  
    numFreeCurvePoints = max(0, numFreeCurvePoints);
    
    this->numFreeCurvePoints = numFreeCurvePoints;
    if(curve.size() < numFreeCurvePoints+2) {
      curve.resize(curve.size()+2)
    }
    if(curveFreePoints.size() < numFreeCurvePoints) {
      curveFreePoints.resize(numFreeCurvePoints)
    }
    curve[0] = curveLeft;
    curve[curve.size()-1] = curveRight;
    for(int i=1; i<curve.size()-1; i++) {
      curve[i] = curveFreePoints[i-1];
    }
  }*/
  
  void updateInfoHover(const Events &events) {
    isInfoHover = false;
    //if(events.numModifiersDown > 0) {
      if(draggableGraphPanel->isPointWithin(events.m)) {
        isInfoHover = true;
        infoHoverDataPoint = (int)round(clamp(map(events.m.x, draggableGraphPanel->absolutePos.x+borderWidth, draggableGraphPanel->absolutePos.x + draggableGraphPanel->size.x - borderWidth, 0, numDataPoints-1), 0, numDataPoints-1));
        infoHoverValue = getDataPointValue(infoHoverDataPoint);
        infoHoverPos = events.m;
        //printf("info hover: %d - %f\n", infoHoverDataPoint, infoHoverValue);
      }
      //printf("...\n");
    //}
  }
  
  virtual void onMouseWheel(const Events &events) {
    if(!draggableGraphPanel || !graphRendered) return;
    if(closestPointIndex != -1) {
      curve[closestPointIndex].cubicInterpolationCurvature += 0.1 * events.mouseWheel;
      curve[closestPointIndex].cubicInterpolationCurvature = clamp(curve[closestPointIndex].cubicInterpolationCurvature, 0.0, 1.0);
      update();
      updateInfoHover(events);
    }
  }
  
  bool onMousePressed(const Events &events) {
    if(!draggableGraphPanel || !graphRendered) return false;
    Vec2d mp = events.m - draggableGraphPanel->absolutePos - Vec2d(borderWidth, borderWidth);
    mp.x /= w-1;
    mp.y /= h-1;

    bool actionHappened = false;
    if(events.mouseButton == 0 && draggingPointIndex == -1 && closestPointIndex != -1) {
      draggingPointIndex = closestPointIndex;
      actionHappened = true;
    }
    if(events.mouseButton == 1 && draggingPointIndex == -1 && events.numModifiersDown == 0) {
      if(closestPointIndex == -1) {
        for(int i=0; i<curve.size()-1; i++) {
          if(mp.x > curve[i].x && mp.x < curve[i+1].x) {
            curve.insert(curve.begin()+i+1, mp);
            closestPointIndex = i+1;
            actionHappened = true;
            break;
          }
        }
      }
      else if(closestPointIndex > 0 && closestPointIndex < curve.size()-1){
        curve.erase(curve.begin()+closestPointIndex);
        closestPointIndex = -1;
        actionHappened = true;
      }
    }
    if(events.mouseButton == 2 && closestPointIndex != -1 && events.numModifiersDown == 0) {
      curve[closestPointIndex].cubicInterpolationCurvature = 0.5;
      actionHappened = true;
    }
    
    if(events.lControlDown && events.mouseNowDownM) {
      for(int i=curve.size()-2; i>=1; i--) {
        curve.erase(curve.begin()+i);
      }
      actionHappened = true;
    }
    if(events.lControlDown && events.mouseNowDownR) {
      if(curve.size() != numDataPoints) {
        for(int i=curve.size()-2; i>=1; i--) {
          curve.erase(curve.begin()+i);
        }
        for(int i=0; i<curve.size(); i++) {
          curve[i].y = 1;
          curve[i].cubicInterpolationCurvature = 0;
        }
        for(int i=1; i<numDataPoints-1; i++) {
          curve.insert(curve.begin()+i, CurvePoint((double)i/(numDataPoints-1), 1, 0));
        }
      }
      else {
        for(int i=1; i<curve.size()-1; i++) {
          curve[i].x = (double)i/(numDataPoints-1);
          curve[i].cubicInterpolationCurvature = 0;
        }
        int i = round(clamp(mp.x*(curve.size()-1), 0, curve.size()-1));
        curve[i].y = clamp(mp.y, 0, 1);
      }
      actionHappened = true;
      
      draggingBars = true;
    }
    
    if(actionHappened) {
      update();
    }
    
    updateInfoHover(events);
    
    return actionHappened;
  }
  
  bool draggingBars = false;
  
  virtual void onMouseReleased(const Events &events) {
    if(!draggableGraphPanel || !graphRendered) return;
    
    if(events.mouseButton == 0) {
      draggingPointIndex = -1;
    }
    
    draggingBars = false;
  }

  
  virtual void onMouseMotion(const Events &events) {
    if(!draggableGraphPanel || !graphRendered) return;
    Vec2d mp = events.m - draggableGraphPanel->absolutePos - Vec2d(borderWidth, borderWidth);
    Vec2d md = events.md;
    mp.x /= (w-1.0);
    mp.y /= (h-1.0);
    md.x /= (w-1.0);
    md.y /= (h-1.0);
    
    
    
    if(events.lControlDown || events.rControlDown) {
      //md *= 0.1;
      
    }
    
    if(events.lShiftDown || events.rShiftDown) {
      md *= 0.1;
    }
    
    int previousClosestPointIndex = closestPointIndex;
    closestPointIndex = -1;
    
    if(draggingBars) {
      int i = round(clamp(mp.x*(curve.size()-1), 0, curve.size()-1));
      curve[i].y = clamp(mp.y, 0, 1);
      update();
    }
    else {
      if(draggingPointIndex == -1) {
        double minDistance = 1e10;
        for(int i=0; i<curve.size(); i++) {
          double d = distance(mp, curve[i]);
          if(d < maxDragDistance / max(w, h) && d < minDistance) {
            minDistance = d;
            closestPointIndex = i;
          }
        }
        if(closestPointIndex != previousClosestPointIndex) {
          draggableGraphPanel->prerenderingNeeded = true;
        }
      }
      else {
        curve[draggingPointIndex].y += md.y;
        
        if(draggingPointIndex != 0 && draggingPointIndex != curve.size() - 1) {
          curve[draggingPointIndex].x += md.x;
          curve[draggingPointIndex].x = clamp(curve[draggingPointIndex].x, 1.0/w, 1.0-1.0/w);
        }
        
        if((events.lAltDown || events.rAltDown) && gridDensity > 0) {
          double q = map(h-1.0-curve[draggingPointIndex].y, 0, 1.0, verticalLimits.x, verticalLimits.y);
          q = round(q*gridDensity) / gridDensity;
          curve[draggingPointIndex].y = 1.0-map(q, verticalLimits.x, verticalLimits.y, 0, 1.0);
        }
        curve[draggingPointIndex].y = clamp(curve[draggingPointIndex].y, 0, 1);
        
        update();
      }
    }
    
    updateInfoHover(events);
  }
  
  static std::string getClassName() {
    return "draggableGraph";
  }
  
  virtual std::string getBlockName() {
    return getClassName();
  }
  
  virtual void decodeParameters() {

    getNumericParameter("numDataPoints", numDataPoints);
    getVectorParameter("xRange", horizontalLimits);
    getVectorParameter("yRange", verticalLimits);
    getVectorParameter("curve", curve);

    if(curve.size() < 2) {
      curve.resize(2);
    }
    curve[0].x = 0;
    curve[curve.size()-1].x = 1.0;
    
    for(int i=0; i<curve.size(); i++) {
      curve[i].x = clamp(curve[i].x, 0, 1);
      curve[i].y = clamp(curve[i].y, 0, 1);
    }

    update();
  }
  
  virtual void encodeParameters() {
    clearParameters();
    putNumericParameter("numDataPoints", numDataPoints);
    putVectorParameter("xRange", horizontalLimits);
    putVectorParameter("yRange", verticalLimits);
    putVectorParameter("curve", curve);
    /*doubleParameters.push_back(DoubleParameter("curve"));
    for(int i=0; i<curve.size(); i++) {
      doubleParameters[doubleParameters.size()-1].values.push_back(clamp(curve[i].x, 0, w-1.0) / (w-1.0));
      doubleParameters[doubleParameters.size()-1].values.push_back(clamp(curve[i].y, 0, h-1.0) / (h-1.0));
    }*/
  }


  
};
