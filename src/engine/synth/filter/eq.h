#pragma once
#include "biquad.h"

static const int maxNumPeakingEqs = 20;

struct Equalizer : public PostProcessingEffect {
  
  std::vector<BiquadFilter> peakingEqs;
  int numPeakingEqs = 8;
  std::vector<double> frequencies;
  std::vector<double> gains;
  std::vector<double> bandwidths;
  
  double sampleRate = 0;
  double wetLevel = 1.0;
  double gain = 1.0;
  
  virtual ~Equalizer() {
    if(equalizerPanel) {
      GuiElement *parent = equalizerPanel->parent;
      parent->deleteChildElement(equalizerPanel);
    }
  }

  
  Equalizer(double sampleRate) : PostProcessingEffect("Equalizer")  {
    this->sampleRate = sampleRate;
    peakingEqs.resize(maxNumPeakingEqs);
    frequencies = {60, 200, 400, 900, 2000, 4500, 10000, 15000};
    gains = {0, 0, 0, 0, 0, 0, 0, 0};
    bandwidths = {2, 2, 2, 2, 2, 2, 2, 2};
    
    for(int i=0; i<maxNumPeakingEqs; i++) {
      int k = min(i, 7);
      peakingEqs[i].set(sampleRate, BiquadFilter::Type::PeakingEQ, frequencies[k], bandwidths[k], gains[k]);
    }
  }
  
  void setPeakingEq(int index, double frequency, double gain, double bandwidth) {
    if(index >= 0 && index < maxNumPeakingEqs) {
      peakingEqs[index].set(sampleRate, BiquadFilter::Type::PeakingEQ, frequency, bandwidth, gain);
    }
  }
  
  
  Equalizer(double sampleRate, Equalizer &equalizer) : PostProcessingEffect("Equalizer")  {
    this->sampleRate = sampleRate;
    numPeakingEqs = equalizer.numPeakingEqs;
    wetLevel = equalizer.wetLevel;
    gain = equalizer.gain;
    peakingEqs.resize(maxNumPeakingEqs);
    bandwidths = equalizer.bandwidths;
    frequencies = equalizer.frequencies;
    gains = equalizer.gains;
    
    for(int i=0; i<maxNumPeakingEqs; i++) {
      peakingEqs[i].set(sampleRate, BiquadFilter::Type::PeakingEQ, equalizer.peakingEqs[i].frequency, equalizer.peakingEqs[i].bandwidth, equalizer.peakingEqs[i].dbGain);
    }
  }
  
  void apply(Vec2d &sample) {
    if(!isActive) return;
    for(int i=0; i<numPeakingEqs; i++) {
      peakingEqs[i].wetLevel = wetLevel;
      peakingEqs[i].gain = gain;
      peakingEqs[i].apply(sample);
    }
  }

  void reset() {
    for(int i=0; i<peakingEqs.size(); i++) {
      peakingEqs[i].reset();
    }
  }

  struct EqualizerPanel : public Panel {
    //Button *isActiveGui = NULL;
    
    SynthTitleBar *titleBar = NULL;
    
    static const int numLayouts = 3;
    int layout = 1;
    std::vector<SynthGuiLayout> eqLayouts = std::vector<SynthGuiLayout>(numLayouts);
    
    NumberBox *wetLevelGui = NULL, *gainGui = NULL;
    NumberBox *numChannelsGui = NULL;
    std::vector<NumberBox*> frequencyGuis = std::vector<NumberBox*>(maxNumPeakingEqs, NULL);
    std::vector<NumberBox*> gainGuis = std::vector<NumberBox*>(maxNumPeakingEqs, NULL);
    std::vector<NumberBox*> bandwidthGuis = std::vector<NumberBox*>(maxNumPeakingEqs, NULL);

    RotaryKnob<double> *wetLevelKnob = NULL, *gainKnob = NULL;
    RotaryKnob<long> *numChannelsKnob = NULL;
    std::vector<RotaryKnob<double>*> frequencyKnobs = std::vector<RotaryKnob<double>*>(8, NULL);
    std::vector<RotaryKnob<double>*> gainKnobs = std::vector<RotaryKnob<double>*>(8, NULL);
    std::vector<RotaryKnob<double>*> bandwidthKnobs = std::vector<RotaryKnob<double>*>(8, NULL);

    
  //  CheckBox *graphAsPitchesGui = NULL;
    
    //ListBox *presetsGui;
    //GraphPanel *graphPanel = NULL;
    
    double line = 0, lineHeight = 23;
    int width = 85, widthA = 85;
    int columnA = 10, columnB = 10+widthA, columnC = 10+widthA+width, columnD = 10+widthA+width*2-25, columnE = 10+widthA+width*3-35;
    int columnF = 10+widthA+width*4;
    int knobSize = 32;
    GuiElement *parentGuiElement = NULL;
    Equalizer *equalizer = NULL;
    
    EqualizerPanel(Equalizer *equalizer, GuiElement *parentGuiElement) : Panel("Equalizer panel") {
      init(equalizer, parentGuiElement);
    }
    
    void init(Equalizer *equalizer, GuiElement *parentGuiElement) {
      this->equalizer = equalizer;
      this->parentGuiElement = parentGuiElement;
      //this->synth = synth;
      setSize(270, 50);
      
      titleBar = new SynthTitleBar(equalizer->getName(), this, equalizer->isActive, layout);
      line += titleBar->size.y + 5;
      
      for(int i=0; i<eqLayouts.size(); i++) {
        addChildElement(eqLayouts[i].gui = new GuiElement("EQ gui layout "+std::to_string(i)));
        eqLayouts[i].gui->setPosition(0, line);
      }

      addGuiEventListener(new EqualizerPanelListener(this));
      parentGuiElement->addChildElement(this);
      
      
      // BOX LAYOUT
      line = 0;
      
      eqLayouts[0].gui->addChildElement(wetLevelGui = new NumberBox("Wet level", equalizer->wetLevel, NumberBox::FLOATING_POINT, 0, 1, 10, line, 7));
      eqLayouts[0].gui->addChildElement(gainGui = new NumberBox("Gain", equalizer->gain, NumberBox::FLOATING_POINT, 0, 1e10, 10, line += lineHeight, 7));
      eqLayouts[0].gui->addChildElement(numChannelsGui = new NumberBox("Bands", equalizer->numPeakingEqs, NumberBox::INTEGER, 1, maxNumPeakingEqs, 10, line += lineHeight, 4));
      numChannelsGui->incrementMode = NumberBox::IncrementMode::Linear;

      eqLayouts[0].gui->addChildElement(new Label("Frequency", columnA, line+=lineHeight));
      eqLayouts[0].gui->addChildElement(new Label("Gain", columnB, line));
      eqLayouts[0].gui->addChildElement(new Label("Bandwidth", columnC, line));

      eqLayouts[0].gui->setSize(max(columnC + width, 270), line + lineHeight +10);
      eqLayouts[0].size = eqLayouts[0].gui->pos + eqLayouts[0].gui->size;
      
      for(int i=0; i<frequencyGuis.size(); i++) {
        line += lineHeight;
        frequencyGuis[i] = new NumberBox("", equalizer->peakingEqs[i].frequency, NumberBox::FLOATING_POINT, -equalizer->sampleRate*0.5, equalizer->sampleRate*0.5, columnA, line, 6, 6, true);
        eqLayouts[0].gui->addChildElement(frequencyGuis[i]);
        eqLayouts[0].gui->addChildElement(gainGuis[i] = new NumberBox("", equalizer->peakingEqs[i].dbGain, NumberBox::FLOATING_POINT, -1e6, 1e6, columnB, line, 6, 6, true));
        gainGuis[i]->incrementMode = NumberBox::IncrementMode::Linear;
        eqLayouts[0].gui->addChildElement(bandwidthGuis[i] = new NumberBox("", equalizer->peakingEqs[i].bandwidth, NumberBox::FLOATING_POINT, -1e6, 1e6, columnC, line, 6, 6, true));
      }
      
      // KNOB LAYOUT
      RowColumnPlacer placer(270, 5, 0);
      knobSize = placer.knobSize;
      
      eqLayouts[1].gui->addChildElement(wetLevelKnob = new RotaryKnob<double>("Wet level", placer.progressX(), placer.getY(), RotaryKnob<double>::LimitedKnob, 0, 1, equalizer->wetLevel));
      eqLayouts[1].gui->addChildElement(gainKnob = new RotaryKnob<double>("Gain", placer.progressX(), placer.getY(), RotaryKnob<double>::LimitedKnob, 0, 1, equalizer->gain));
      eqLayouts[1].gui->addChildElement(numChannelsKnob = new RotaryKnob<long>("Bands", placer.progressX(), placer.getY(), RotaryKnob<long>::LimitedKnob, 1, min(8, maxNumPeakingEqs), equalizer->numPeakingEqs));
      
      placer.reset(); 
      placer.line = placer.knobSize;
      
      for(int i=0; i<frequencyKnobs.size(); i++) {
        eqLayouts[1].gui->addChildElement(frequencyKnobs[i] = new RotaryKnob<double>("Frequency "+std::to_string(i+1), placer.progressX(), placer.getY(), RotaryKnob<double>::LimitedKnob, 1, equalizer->sampleRate*0.5-1, equalizer->peakingEqs[i].frequency));
        frequencyKnobs[i]->valueMappingFunction = frequencyExpMapper;
      }
      for(int i=0; i<gainKnobs.size(); i++) {
        eqLayouts[1].gui->addChildElement(gainKnobs[i] = new RotaryKnob<double>("Gain "+std::to_string(i+1), placer.progressX(), placer.getY(), RotaryKnob<double>::LimitedKnob, -20, 20, equalizer->peakingEqs[i].dbGain));
      }
      for(int i=0; i<bandwidthKnobs.size(); i++) {
        eqLayouts[1].gui->addChildElement(bandwidthKnobs[i] = new RotaryKnob<double>("Bandwidth "+std::to_string(i+1), placer.progressX(), placer.getY(), RotaryKnob<double>::LimitedKnob, 0.01, 20, equalizer->peakingEqs[i].bandwidth));
      }
      eqLayouts[1].gui->setSize(placer.width, placer.getY() + placer.knobSize);
      eqLayouts[1].size = eqLayouts[1].gui->pos + eqLayouts[1].gui->size;
      
      // TITLE BAR LAYOUT
      eqLayouts[2].size.set(270, titleBar->size.y);

      update();
      
      
      for(int i=0; i<maxNumPeakingEqs; i++) {
        equalizer->peakingEqs[i].initGraphPanel(150, 100, false, false);
        equalizer->peakingEqs[i].graphPanel->setPosition(size.x + 5 + 155 * (i % 3), 105 * (i / 3));
        equalizer->peakingEqs[i].graphPanel->showAxes = false;
        equalizer->peakingEqs[i].graphPanel->numHorizontalMarks = 3;
        equalizer->peakingEqs[i].graphPanel->numHorizontalAxisLabels = 1;
        //equalizer->peakingEqs[i].graphPanel->horizontalAxisLimits.set(0, equalizer->peakingEqs[i].frequency*2);
        //int k = (int)((equalizer->peakingEqs[i].frequency*0.00125 + 1.0));
        //equalizer->peakingEqs[i].graphPanel->horizontalAxisLimits.set(0, k*1000);
        equalizer->peakingEqs[i].prepareFilterResponseGraph();
        addChildElement(equalizer->peakingEqs[i].graphPanel);
      }
      
      /*if(guiControlPanel) {
        setPosition(10, guiControlPanel->pos.y + guiControlPanel->size.y + 10);
      }*/
    }

    void update() {
      titleBar->isActiveGui->setValue(equalizer->isActive);
      if(layout == 0) {
        numChannelsGui->setValue(equalizer->numPeakingEqs);
        wetLevelGui->setValue(equalizer->wetLevel);
        gainGui->setValue(equalizer->gain);
        for(int i=0; i<maxNumPeakingEqs; i++) {
          if(i < equalizer->numPeakingEqs) {
            frequencyGuis[i]->setValue(equalizer->peakingEqs[i].frequency);
            gainGuis[i]->setValue(equalizer->peakingEqs[i].dbGain);
            bandwidthGuis[i]->setValue(equalizer->peakingEqs[i].bandwidth);
          }
          frequencyGuis[i]->setVisible(i < equalizer->numPeakingEqs);
          gainGuis[i]->setVisible(i < equalizer->numPeakingEqs);
          bandwidthGuis[i]->setVisible(i < equalizer->numPeakingEqs);
        }
        
        setSize(eqLayouts[layout].size.x, eqLayouts[layout].size.y + equalizer->numPeakingEqs*lineHeight);
        titleBar->setSize(size.x, titleBar->size.y);
      }
      if(layout == 1) {
        //numChannelsKnob->setValue(equalizer->numPeakingEqs);
        wetLevelKnob->setValue(equalizer->wetLevel);
        gainKnob->setValue(equalizer->gain);
        for(int i=0; i<min(8, maxNumPeakingEqs); i++) {
          if(i < equalizer->numPeakingEqs) {
            frequencyKnobs[i]->setValue(equalizer->peakingEqs[i].frequency);
            gainKnobs[i]->setValue(equalizer->peakingEqs[i].dbGain);
            bandwidthKnobs[i]->setValue(equalizer->peakingEqs[i].bandwidth);
          }
          frequencyKnobs[i]->setVisible(i < equalizer->numPeakingEqs);
          gainKnobs[i]->setVisible(i < equalizer->numPeakingEqs);
          bandwidthKnobs[i]->setVisible(i < equalizer->numPeakingEqs);
        }
        
        
        //setSize(eqLayouts[layout].size.x, eqLayouts[layout].size.y + equalizer->numPeakingEqs*knobSize);
        setSize(eqLayouts[layout].size);
        titleBar->setSize(size.x, titleBar->size.y);
      }
      if(layout == 2) {
        setSize(eqLayouts[layout].size);
        titleBar->setSize(size.x, titleBar->size.y);
      }
      
      for(int i=0; i<numLayouts; i++) {
        eqLayouts[i].gui->setVisible(i == layout);
      }
    }

    struct EqualizerPanelListener : public GuiEventListener {
      EqualizerPanel *equalizerPanel;
      EqualizerPanelListener(EqualizerPanel *equalizerPanel) {
        this->equalizerPanel = equalizerPanel;
      }

      void onValueChange(GuiElement *guiElement) {
        if(guiElement == equalizerPanel->titleBar->isActiveGui) {
          guiElement->getValue((void*)&equalizerPanel->equalizer->isActive);
        }
        if(guiElement == equalizerPanel->titleBar->layoutGui) {
          equalizerPanel->layout = (equalizerPanel->layout+1) % equalizerPanel->numLayouts;
          equalizerPanel->update();
        }
        
        if(guiElement == equalizerPanel->numChannelsGui || guiElement == equalizerPanel->numChannelsKnob) {
          guiElement->getValue((void*)&equalizerPanel->equalizer->numPeakingEqs);
          equalizerPanel->update();
        }
        if(guiElement == equalizerPanel->wetLevelGui || guiElement == equalizerPanel->wetLevelKnob) {
          guiElement->getValue((void*)&equalizerPanel->equalizer->wetLevel);
        }
        if(guiElement == equalizerPanel->gainGui || guiElement == equalizerPanel->gainKnob) {
          guiElement->getValue((void*)&equalizerPanel->equalizer->gain);
        }

        for(int i=0; i<equalizerPanel->equalizer->numPeakingEqs; i++) {
          if(guiElement == equalizerPanel->frequencyGuis[i] || guiElement == equalizerPanel->frequencyKnobs[i]) {
            guiElement->getValue((void*)&equalizerPanel->equalizer->peakingEqs[i].frequency);
            //equalizerPanel->equalizer->peakingEqs[i].graphPanel->horizontalAxisLimits.set(0, equalizerPanel->equalizer->peakingEqs[i].frequency*2);
            //int k = (int)((equalizerPanel->equalizer->peakingEqs[i].frequency*0.00125 + 1.0));
            //equalizerPanel->equalizer->peakingEqs[i].graphPanel->horizontalAxisLimits.set(0, k*1000);
            equalizerPanel->equalizer->peakingEqs[i].prepare();
          }
          if(guiElement == equalizerPanel->gainGuis[i] || guiElement == equalizerPanel->gainKnobs[i]) {
            guiElement->getValue((void*)&equalizerPanel->equalizer->peakingEqs[i].dbGain);
            equalizerPanel->equalizer->peakingEqs[i].prepare();
          }
          if(guiElement == equalizerPanel->bandwidthGuis[i] || guiElement == equalizerPanel->bandwidthKnobs[i]) {
            guiElement->getValue((void*)&equalizerPanel->equalizer->peakingEqs[i].bandwidth);
            equalizerPanel->equalizer->peakingEqs[i].prepare();
          }
        }
      }

      void onKeyDown(GuiElement *guiElement, Events &events) {
        if(events.sdlKeyCode == SDLK_HOME) {
          for(int i=0; i<equalizerPanel->equalizer->numPeakingEqs; i++) {
            if(guiElement == equalizerPanel->frequencyGuis[i] ||
               guiElement == equalizerPanel->gainGuis[i] ||
               guiElement == equalizerPanel->bandwidthGuis[i]) {
               for(int k=0; k<equalizerPanel->equalizer->numPeakingEqs; k++) {
                 equalizerPanel->equalizer->peakingEqs[k].graphPanel->toggleVisibility();
               }
            }
          }
        }
      }
    };
  };
  EqualizerPanel *equalizerPanel = NULL;
  
  Panel *getPanel() {
    return equalizerPanel;
  }

  Panel *addPanel(GuiElement *parentGuiElement, const std::string &title = "") {
    return equalizerPanel = new EqualizerPanel(this, parentGuiElement);
  }
  
  void removePanel(GuiElement *parentGuiElement) {
    PanelInterface::removePanel(parentGuiElement);

    if(equalizerPanel) {
      parentGuiElement->deleteChildElement(equalizerPanel);
      equalizerPanel = NULL;
    }
  }

  void updatePanel() {
    if(equalizerPanel) {
      equalizerPanel->update();
    }
  }
  
  
  
  void applyPreset(int index) {
    if(!presets || index < 0 || index >= presets->size()) return;
    applyParameters(presets->at(index).doubleParameters, presets->at(index).stringParameters);
  }  
  
  void applyParameters(const std::vector<DoubleParameter> &doubleParameters, const std::vector<StringParameter> &stringParameters) {

    getNumericParameter("gain", gain);
    getNumericParameter("wet", wetLevel);

    numPeakingEqs = 0;
    for(int i=0; i<doubleParameters.size(); i++) {
      if(doubleParameters[i].values.size() > 2 && doubleParameters[i].name.compare("channel") == 0) {
        setPeakingEq(numPeakingEqs, doubleParameters[i].values[0], doubleParameters[i].values[1], doubleParameters[i].values[2]);
        numPeakingEqs++;
      }
    }
  }
  
  static std::string getClassName() {
    return "equalizer";
  }
  
  virtual std::string getBlockName() {
    return getClassName();
  }
  
  virtual void decodeParameters() {
    applyParameters(doubleParameters, stringParameters);
  }

  virtual void encodeParameters() {
    clearParameters();

    //stringParameters.push_back(StringParameter("type", "reverb"));
    putNumericParameter("gain", gain);
    putNumericParameter("wet", wetLevel);
    
    for(int i=0; i<numPeakingEqs; i++) {
      putNumericParameter("channel", {peakingEqs[i].frequency, peakingEqs[i].dbGain, peakingEqs[i].bandwidth});
    }
    
    if(presetName.size() > 0) {
      putStringParameter("presetName", presetName);
    }
  }
  
  
  
  
};