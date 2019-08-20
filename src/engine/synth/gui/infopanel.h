#pragma once
#include "../synth.h"


struct InfoPanel : public GraphPanel
{
  // FIXME use circular buffer class
  std::vector<double> cpuLoadGraph;
  std::vector<double> fpsGraph;

  double updateIntervalSecs = 0.2;
  double timer = 0;
  double cpuLoad = 0, cpuLoadTmp = 0;
  double fps = 0, fpsTmp = 0;
  double fpsMax = 60.0;
  int numOscillators = 0;

  InfoPanel() {
    cpuLoadGraph.resize(300, 0);
    fpsGraph.resize(300, 0);
    this->numHorizontalMarks = 0;
    this->numVerticalMarks = 0;
    this->horizontalAxisLimits.set(0, cpuLoadGraph.size());
    this->verticalAxisLimits.set(0, 1);
    this->numHorizontalAxisLabels = 0;
    this->horizontalAxisUnit = "s";
    this->showAxes = false;
    this->data = &fpsGraph;
    this->data2 = &cpuLoadGraph;
    //this->graphColor = Vec4d(1, 1, 1, 1);
    //this->graphColor2 = Vec4d(0.6, 0.6, 0.6, 1);
    //this->graphWidth = 1;

    this->prerenderingNeeded = true;
    
    this->setSize(200, 50 /*23*2+20*/);
  }

  void update(double cpuLoad, double dt, int numOscillators) {
    timer += dt;
    this->cpuLoadTmp += cpuLoad * dt;
    this->fpsTmp += 1.0;
    this->numOscillators = numOscillators;

    if(timer >= updateIntervalSecs) {
      this->cpuLoad = cpuLoadTmp / timer;
      this->fps = fpsTmp / timer;
      cpuLoadTmp = 0;
      fpsTmp = 0;
      fpsMax = max(fps, fpsMax);

      for(int i=0; i<cpuLoadGraph.size()-1; i++) {
        cpuLoadGraph[i] = cpuLoadGraph[i+1];
      }
      cpuLoadGraph[cpuLoadGraph.size()-1] = this->cpuLoad;

      for(int i=0; i<fpsGraph.size()-1; i++) {
        fpsGraph[i] = fpsGraph[i+1];
      }
      fpsGraph[fpsGraph.size()-1] = this->fps / fpsMax;

      prerenderingNeeded = true;
      timer = 0;
    }
  }

  virtual void onPrerender(GeomRenderer &geomRenderer, TextGl &textRenderer, const Vec2d &displacement) {
    GraphPanel::onPrerender(geomRenderer, textRenderer, displacement);

    char buf[128];
    double lineHeight = 18;
    //textRenderer.setColor(1, 1, 1, 1);
    textRenderer.setColor(0.8, 0.8, 0.8, 1);
    std::sprintf(buf, "fps %.2f", fps);
    textRenderer.print(buf, displacement.x + 10, displacement.y + 3, 10);
    
    std::sprintf(buf, "cpu load %.2f %%", cpuLoad * 100.0);
    textRenderer.print(buf, displacement.x + 10, lineHeight + displacement.y + 3, 10);


    //std::sprintf(buf, "Oscillators %d", numOscillators);
    //textRenderer.print(buf, displacement.x + 10, lineHeight*2 + displacement.y + 3, 10);
  }
};