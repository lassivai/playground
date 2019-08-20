#pragma once
#include "postprocessingeffect.h"

/*
 * Notch: all-pass settings, local memory, bandwidth 0.1 - 6, wet 0.5
 * Notch: notch settings, local memory, bandwidth 0.1 - 6, wet 1.0
 * Band-pass: bandwidth 0.01-1, cascade 2-10, wet 0.1-0.4
 * High-pass: local memory, bandwidth 0-10, wet 1.0
 * Low-pass: local memory, bandwidth 0-10, wet 1.0
 *
 *
 */




struct BiquadFilter : public PostProcessingEffect {

  enum Type { None, LowPass1, HighPass1, LowPass, HighPass, BandPass, Notch, LowShelving, HighShelving, PeakingEQ, AllPass, Random};
  const std::vector<std::string> typeNames = { "None", "Low-pass (1. order)", "High-pass (1. order)", "Low-pass", "High-pass", "Band-pass", "Notch", "Low shelving", "High shelving", "Peaking EQ", "All-pass" };
  //static const int numTypes = 8;



  Type type[2] = {LowPass, LowPass};
  double frequency[2] = {1500, 1500}, dbGain[2] = {0.5, 0.5}, bandwidth[2] = {1, 1}, qualityFactor[2] = {1.0, 1.0};
  bool useLocalMemory[2] = {true, true};
  int cascadeLevel[2] = {1, 1};

  Vec2d x0[2], x1[2], x2[2], y0[2], y1[2], y2[2];
  double a0[2], a1[2], a2[2], b0[2], b1[2], b2[2];

  double wetLevel = 1.0;
  double gain = 1.0;

  const double ln2 = log(2.0) * 0.5;

  int lowestFilterResponseGraphFrequency = 1;
  int highestFilterResponseGraphFrequency = 16000;
  int filterResponseGraphSize = 600;
  std::vector<double> filterResponseGraph, filterResponseGraphNeutral;
  std::vector<double> filterResponseGraphNormalized;

  FFTW3Interface filterResponseGraphFFTW;
  int sampleClipSize;
  std::vector<double> sampleClip;

  BiquadFilter(DelayLine *delayLine, const BiquadFilter &biquadFilter) : PostProcessingEffect(delayLine, "Biquad filter") {
    sampleClipSize = (int)(delayLine->sampleRate);
    sampleClip.resize(sampleClipSize);
    filterResponseGraph.resize(filterResponseGraphSize);
    filterResponseGraphNeutral.resize(filterResponseGraphSize);
    filterResponseGraphNormalized.resize(filterResponseGraphSize);

    this->type = biquadFilter.type;;

    this->frequency = biquadFilter.frequency;
    this->dbGain = biquadFilter.dbGain;
    this->bandwidth = biquadFilter.bandwidth;
    this->qualityFactor = biquadFilter.qualityFactor;

    /*this->x0 = biquadFilter.x0;
    this->x1 = biquadFilter.x1;
    this->x2 = biquadFilter.x2;
    this->y0 = biquadFilter.y0;
    this->y1 = biquadFilter.y1;
    this->y2 = biquadFilter.y2;

    this->a0 = biquadFilter.a0;
    this->a1 = biquadFilter.a1;
    this->a2 = biquadFilter.a2;
    this->b0 = biquadFilter.b0;
    this->b1 = biquadFilter.b1;
    this->b2 = biquadFilter.b2;*/

    this->useLocalMemory = biquadFilter.useLocalMemory;
    this->cascadeLevel = biquadFilter.cascadeLevel;


    for(int i=0; i<filterResponseGraphSize; i++) {
      double f = map(i, 0.0, filterResponseGraphSize-1, lowestFilterResponseGraphFrequency, highestFilterResponseGraphFrequency);
      for(int k=0; k<sampleClipSize; k++) {
        sampleClip[k] += sin(2.0*PI * f * k / delayLine->sampleRate);
      }
    }
    prepareFilterResponseGraphNeutral();
    prepare();
  }


  BiquadFilter(DelayLine *delayLine) : PostProcessingEffect(delayLine, "Biquad filter") {
    sampleClipSize = (int)(delayLine->sampleRate);
    sampleClip.resize(sampleClipSize);
    filterResponseGraph.resize(filterResponseGraphSize);
    filterResponseGraphNeutral.resize(filterResponseGraphSize);
    filterResponseGraphNormalized.resize(filterResponseGraphSize);

    for(int i=0; i<filterResponseGraphSize; i++) {
      double f = map(i, 0.0, filterResponseGraphSize-1, lowestFilterResponseGraphFrequency, highestFilterResponseGraphFrequency);
      for(int k=0; k<sampleClipSize; k++) {
        sampleClip[k] += sin(2.0*PI * f * k / delayLine->sampleRate);
      }
    }
    prepareFilterResponseGraphNeutral();
    prepare();
  }

  void prepareFilterResponseGraphNeutral() {

    a0 = b0 = 1;
    a1 = a2 = b1 = b2 = 0;

    std::vector<Vec2d> xs = {Vec2d(), Vec2d(), Vec2d()};
    std::vector<Vec2d> ys = {Vec2d(), Vec2d(), Vec2d()};

    filterResponseGraphFFTW.initialize(sampleClipSize, true);
    for(int i=0; i<sampleClipSize; i++) {
      xs[0] = sampleClip[i];
      apply(xs, ys);
      filterResponseGraphFFTW.input[i] = xs[0].x;
    }

    filterResponseGraphFFTW.transform();

    for(int i=0; i<filterResponseGraphSize; i++) {
      int k = (int)map(i, 0, filterResponseGraphSize-1, lowestFilterResponseGraphFrequency, highestFilterResponseGraphFrequency);
      k = clamp(k, 0, sampleClipSize-1);
      filterResponseGraphNeutral[i] = filterResponseGraphFFTW.output2[k].x;
    }

    //this->type = typeTmp;
  }

  void prepareFilterResponseGraph() {
    std::vector<Vec2d> xs = {Vec2d(), Vec2d(), Vec2d()};
    std::vector<Vec2d> ys = {Vec2d(), Vec2d(), Vec2d()};

    filterResponseGraphFFTW.initialize(sampleClipSize, true);
    for(int i=0; i<sampleClipSize; i++) {
      xs[0] = sampleClip[i];
      apply(xs, ys);
      filterResponseGraphFFTW.input[i] = xs[0].x;
    }

    filterResponseGraphFFTW.transform();

    for(int i=0; i<filterResponseGraphSize; i++) {
      int k = (int)map(i, 0, filterResponseGraphSize-1, lowestFilterResponseGraphFrequency, highestFilterResponseGraphFrequency);
      k = clamp(k, 0, sampleClipSize-1);
      filterResponseGraph[i] = filterResponseGraphFFTW.output2[k].x;
      filterResponseGraphNormalized[i] = filterResponseGraph[i] / filterResponseGraphNeutral[i];
    }

    if(graphPanel) {
      graphPanel->prerenderingNeeded = true;
    }
  }


  // TODO normalize by dividing other factors by a0
   void prepare() {
     double w0 = 2.0 * PI * frequency / delayLine->sampleRate;
     double alpha = sin(w0) * sinh(ln2 * bandwidth * w0 / sin(w0));

     if(type == Type::None) {
       b0 = 1;
       b1 = 0;
       b2 = 0;
       a0 = 1;
       a1 = 0;
       a2 = 0;
     }
     if(type == Type::Random) {
       b0 = Random::getDouble(-3, 3);
       b1 = Random::getDouble(-3, 3);
       b2 = Random::getDouble(-3, 3);
       a0 = Random::getDouble(-3, 3);
       a1 = Random::getDouble(-3, 3);
       a2 = Random::getDouble(-3, 3);
     }
     if(type == Type::LowPass1) {
       double K = tan(w0*0.5);
       b0 = K/(1.0+K);
       b1 = K/(1.0+K);
       b2 = 0;
       a0 = 1;
       a1 = -(1.0-K)/(1.0+K);
       a2 = 0;
     }
     if(type == Type::HighPass1) {
       double K = tan(w0*0.5);
       b0 = 1.0/(1.0+K);
       b1 = -1.0/(1.0+K);
       b2 = 0;
       a0 = 1;
       a1 = -(1.0-K)/(1.0+K);
       a2 = 0;
     }
     if(type == Type::LowPass) {
       double cosw0 = cos(w0);
       b0 = (1.0 - cosw0) * 0.5;
       b1 = 1.0 - cosw0;
       b2 = (1.0 - cosw0) * 0.5;
       a0 = 1.0 + alpha;
       a1 = -2.0 * cosw0;
       a2 = 1.0 - alpha;
     }
     if(type == Type::HighPass) {
       double cosw0 = cos(w0);
       b0 = (1.0 + cosw0) * 0.5;
       b1 = -1.0 - cosw0;
       b2 = (1.0 + cosw0) * 0.5;
       a0 = 1.0 + alpha;
       a1 = -2.0 * cosw0;
       a2 = 1.0 - alpha;
     }
     if(type == Type::BandPass) {
       double sinw0 = sin(w0);
       b0 = sinw0 * 0.5;
       b1 = 0;
       b2 = -sinw0 * 0.5;
       a0 = 1.0 + alpha;
       a1 = -2.0 * cos(w0);
       a2 = 1.0 - alpha;
     }
     if(type == Type::Notch) {
       double cosw0 = cos(w0);
       b0 = 1;
       b1 = -2.0 * cosw0;
       b2 = 1;
       a0 = 1.0 + alpha;
       a1 = -2.0 * cosw0;
       a2 = 1.0 - alpha;
     }
     if(type == Type::AllPass) {
       double cosw0 = cos(w0);
       b0 = 1.0 - alpha;
       b1 = -2.0 * cosw0;
       b2 = 1.0 + alpha;
       a0 = 1.0 + alpha;
       a1 = -2.0 * cosw0;
       a2 = 1.0 - alpha;
     }
     if(type == Type::PeakingEQ) {
       double A = sqrt(pow(10.0, dbGain / 20.0));
       double cosw0 = cos(w0);
       b0 = 1.0 + alpha * A;
       b1 = -2.0 * cosw0;
       b2 = 1.0 - alpha * A;
       a0 = 1.0 + alpha / A;
       a1 = -2.0 * cosw0;
       a2 = 1.0 - alpha / A;
     }
     if(type == Type::LowShelving) {
       double A = sqrt(pow(10.0, dbGain / 20.0));
       double beta = sqrt(A) / qualityFactor;
       double cosw0 = cos(w0);
       double sinw0 = sin(w0);
       b0 = A * ((A+1) - (A-1)*cosw0 + beta*sinw0);
       b1 = 2*A * ((A-1) - (A+1)*cosw0);
       b2 = A * ((A+1) - (A-1)*cosw0 - beta*sinw0);
       a0 = ((A+1) + (A-1)*cosw0 + beta*sinw0);
       a1 = -2*A * ((A-1) + (A+1)*cosw0);
       a2 = ((A+1) + (A-1)*cosw0 - beta*sinw0);
     }
     if(type == Type::HighShelving) {
       double A = sqrt(pow(10.0, dbGain / 20.0));
       double beta = sqrt(A) / qualityFactor;
       double cosw0 = cos(w0);
       double sinw0 = sin(w0);
       b0 = A * ((A+1) + (A-1)*cosw0 + beta*sinw0);
       b1 = -2*A * ((A-1) + (A+1)*cosw0);
       b2 = A * ((A+1) + (A-1)*cosw0 - beta*sinw0);
       a0 = ((A+1) - (A-1)*cosw0 + beta*sinw0);
       a1 = 2*A * ((A-1) - (A+1)*cosw0);
       a2 = ((A+1) - (A-1)*cosw0 - beta*sinw0);
     }
     if(a0 == 0) {
       a0 = 1e-10;
     }
     a1 /= a0;
     a2 /= a0;
     b0 /= a0;
     b1 /= a0;
     b2 /= a0;

     prepareFilterResponseGraph();
   }

   void reset() {
     x0 = 0;
     x1 = 0;
     x2 = 0;
     y0 = 0;
     y1 = 0;
     y2 = 0;
   }

  void apply(std::vector<Vec2d> &xs, std::vector<Vec2d> &ys) {
    for(int i=0; i<cascadeLevel; i++) {
      //ys[0] = (xs[0]*b0 + xs[1]*b1 + xs[2]*b2 - ys[1]*a1 - ys[2]*a2) / a0;
      ys[0] = xs[0]*b0 + xs[1]*b1 + xs[2]*b2 - ys[1]*a1 - ys[2]*a2;
      ys[2] = ys[1];
      ys[1] = ys[0];
      xs[2] = xs[1];
      xs[1] = xs[0];

      xs[0] = xs[0] * (1.0-wetLevel) + ys[0] * wetLevel;
      xs[0] *= gain;
    }
  }

  void update() {
    if(!isActive) return;

    if(useLocalMemory) {
      x0 = delayLine->getOutputSample(0);
      for(int i=0; i<cascadeLevel; i++) {
        //y0 = (x0*b0 + x1*b1 + x2*b2 - y1*a1 - y2*a2) / a0;
        y0 = x0*b0 + x1*b1 + x2*b2 - y1*a1 - y2*a2;
        y2 = y1;
        y1 = y0;
        x2 = x1;
        x1 = x0;

        delayLine->buffer[delayLine->bufferPos] = x0 * (1.0-wetLevel) + y0 * wetLevel;
        delayLine->buffer[delayLine->bufferPos] *= gain;
        x0 = delayLine->buffer[delayLine->bufferPos];
      }
    }
    else {
      x1 = delayLine->getOutputSample(1);
      x2 = delayLine->getOutputSample(2);
      y0 = delayLine->getOutputSample(0);
      y1 = delayLine->getOutputSample(1);
      y2 = delayLine->getOutputSample(2);

      for(int i=0; i<cascadeLevel; i++) {
        x0 = delayLine->getOutputSample(0);
        y0 = (x0*b0 + x1*b1 + x2*b2 - y1*a1 - y2*a2) / a0;

        delayLine->buffer[delayLine->bufferPos] = x0 * (1.0-wetLevel) + y0 * wetLevel;
        delayLine->buffer[delayLine->bufferPos] *= gain;
      }
    }
  }


  CheckBox *isActiveGui = NULL;
  NumberBox *frequencyGui = NULL;
  NumberBox *dbGainGui = NULL;
  NumberBox *bandwidthGui = NULL;
  NumberBox *qualityFactorGui = NULL;
  NumberBox *wetLevelGui = NULL, *gainGui = NULL;
  CheckBox *useLocalMemoryGui = NULL;
  NumberBox *cascadeLevelGui = NULL;
  ListBox *typeGui = NULL;
  //ListBox *presetsGui;
  GraphPanel *graphPanel = NULL;

  Panel *getPanel() {
    return panel;
  }

  Panel *addPanel(GuiElement *parentElement, const std::string &title = "") {
    double line = 10, lineHeight = 23;

    if(!panel) {
      panel = new Panel(320, 320, 60, 10);

      panel->addGuiEventListener(new BiquadFilterPanelListener(this));

      Label *label = new Label(getName(), 10, line);
      isActiveGui = new CheckBox("Active", isActive, 10, line += lineHeight);
      wetLevelGui = new NumberBox("Wet level", wetLevel, NumberBox::FLOATING_POINT, 0, 1, 10, line += lineHeight);
      gainGui = new NumberBox("Gain", gain, NumberBox::FLOATING_POINT, 0, 1e10, 10, line += lineHeight);

      typeGui = new ListBox("Type", 10, line += lineHeight);
      typeGui->setItems(typeNames);
      typeGui->setValue(type);

      //useLocalMemoryGui = new CheckBox("Local memory", useLocalMemory, 10, line += lineHeight);

      frequencyGui = new NumberBox("Frequency", frequency, NumberBox::FLOATING_POINT, -1e6, delayLine->sampleRate*0.5, 10, line += lineHeight);
      frequencyGui->defaultIncrementModePowerShift = -1;
      bandwidthGui = new NumberBox("Bandwidth", bandwidth, NumberBox::FLOATING_POINT, -1e6, delayLine->sampleRate*0.25, 10, line += lineHeight);
      dbGainGui = new NumberBox("db gain", dbGain, NumberBox::FLOATING_POINT, -1e6, 1e10, 10, line += lineHeight);
      qualityFactorGui = new NumberBox("Quality factor", qualityFactor, NumberBox::FLOATING_POINT, -1e6, 1e10, 10, line += lineHeight);
      cascadeLevelGui = new NumberBox("Cascade level", cascadeLevel, NumberBox::INTEGER, 1, 10000, 10, line += lineHeight);

      //graphPanel = new GraphPanel(this);
      graphPanel = new GraphPanel(&filterResponseGraphNormalized, true, 9, 0, 1, 16000, 0, 2, 3, 0, "Hz", "");
      graphPanel->setSize(600, 250);

      /*presetsGui = new ListBox("Presets", 10, line += lineHeight);
      for(int i=0; i<numPresets; i++) {
        presetsGui->addItems(std::to_string(i+1));
      }*/

      panel->addChildElement(label);
      panel->addChildElement(isActiveGui);
      panel->addChildElement(wetLevelGui);
      panel->addChildElement(gainGui);
      panel->addChildElement(typeGui);
      panel->addChildElement(frequencyGui);
      panel->addChildElement(dbGainGui);
      panel->addChildElement(bandwidthGui);
      panel->addChildElement(qualityFactorGui);
      //panel->addChildElement(useLocalMemoryGui);
      panel->addChildElement(cascadeLevelGui);
      panel->addChildElement(graphPanel);

      parentElement->addChildElement(panel);

      panel->setSize(320, line + lineHeight + 10);
    }
    else {
      isActiveGui->setValue(isActive);
      wetLevelGui->setValue(wetLevel);
      gainGui->setValue(gain);
      typeGui->setValue(type);
      frequencyGui->setValue(frequency);
      dbGainGui->setValue(dbGain);
      bandwidthGui->setValue(bandwidth);
      useLocalMemoryGui->setValue(useLocalMemory);
      cascadeLevelGui->setValue(cascadeLevel);
      qualityFactorGui->setValue(qualityFactor);
    }

    return panel;
  }

  void removePanel(GuiElement *parentElement) {
    PanelInterface::removePanel(parentElement);

    if(panel) {
      parentElement->deleteChildElement(panel);
      panel = NULL;
      isActiveGui = NULL;
      frequencyGui = NULL;
      dbGainGui = NULL;
      bandwidthGui = NULL;
      wetLevelGui = NULL;
      gainGui = NULL;
      typeGui = NULL;
      useLocalMemoryGui = NULL;
      cascadeLevelGui = NULL;
      graphPanel = NULL;
      qualityFactorGui = NULL;
    }
  }

  void updatePanel() {
    if(panel) {
      addPanel(NULL);
    }
  }


  struct BiquadFilterPanelListener : public GuiEventListener {
    BiquadFilter *biquadFilter;
    BiquadFilterPanelListener(BiquadFilter *biquadFilter) {
      this->biquadFilter = biquadFilter;
    }
    void onValueChange(GuiElement *guiElement) {
      if(guiElement == biquadFilter->typeGui) {
        guiElement->getValue(&biquadFilter->type);
        biquadFilter->prepare();
      }
      if(guiElement == biquadFilter->isActiveGui) {
        guiElement->getValue(&biquadFilter->isActive);
        biquadFilter->prepareFilterResponseGraph();
      }
      if(guiElement == biquadFilter->wetLevelGui) {
        guiElement->getValue(&biquadFilter->wetLevel);
        biquadFilter->prepareFilterResponseGraph();
      }
      if(guiElement == biquadFilter->gainGui) {
        guiElement->getValue(&biquadFilter->gain);
        biquadFilter->prepareFilterResponseGraph();
      }
      if(guiElement == biquadFilter->useLocalMemoryGui) {
        guiElement->getValue(&biquadFilter->useLocalMemory);
        biquadFilter->prepareFilterResponseGraph();
      }
      if(guiElement == biquadFilter->cascadeLevelGui) {
        guiElement->getValue(&biquadFilter->cascadeLevel);
        biquadFilter->prepareFilterResponseGraph();
      }
      if(guiElement == biquadFilter->frequencyGui) {
        guiElement->getValue(&biquadFilter->frequency);
        biquadFilter->prepare();
      }
      if(guiElement == biquadFilter->dbGainGui) {
        guiElement->getValue(&biquadFilter->dbGain);
        biquadFilter->prepare();
      }
      if(guiElement == biquadFilter->bandwidthGui) {
        guiElement->getValue(&biquadFilter->bandwidth);
        biquadFilter->prepare();
      }
      if(guiElement == biquadFilter->qualityFactorGui) {
        guiElement->getValue(&biquadFilter->qualityFactor);
        biquadFilter->prepare();
      }
      biquadFilter->reset();
    }
  };
};
