#pragma once
#include "biquad.h"

/* TODO 
 *  - multi step waves
 *  - vibrato offset
 */

struct PitchChanger : public PostProcessingEffect
{
  enum VibratoWave { Triangle, Sin, Square, Saw, BackwardSaw };
  enum VibratoMode { None, Linear, Power };
  const std::vector<std::string> vibratoWaveNames = { "Triangle", "Sin", "Square", "Saw", "Backward saw" };
  const std::vector<std::string> vibratoModeNames = { "None", "Linear", "Power" };
  
  
  DelayLine delayLine;
  double sampleRate = 1;
  double sampleLength = 0;
  double transpositionMaxDelay = 1;

  double wetLevel = 1.0;
  double gain = 1.0;

  double transpositionAmount = 1;
  double transpositionDelay = 0.1;
  
  double vibratoFrequence = 1;
  double powerVibratoDepth = 1.05;
  double linearVibratoDepth = 0.05;
  
  bool dualDelaySampling = true;
  
  double maxStereoDistance;
  double stereoDistance = 0;

  VibratoMode vibratoMode = None;
  VibratoWave vibratoWave = Triangle;
  
  double transpositionPosition = 0;
  
  PitchChanger &operator=(const PitchChanger &pitchChanger) {
    delayLine.init(pitchChanger.delayLine.sampleRate, pitchChanger.delayLine.bufferLength);
    sampleRate = pitchChanger.sampleRate;
    sampleLength = pitchChanger.sampleLength;
    transpositionMaxDelay = pitchChanger.transpositionMaxDelay;

    wetLevel = pitchChanger.wetLevel;
    gain = pitchChanger.gain;

    transpositionAmount = pitchChanger.transpositionAmount;
    transpositionDelay = pitchChanger.transpositionDelay;
    
    vibratoFrequence = pitchChanger.vibratoFrequence;
    powerVibratoDepth = pitchChanger.powerVibratoDepth;
    linearVibratoDepth = pitchChanger.linearVibratoDepth;
    
    dualDelaySampling = pitchChanger.dualDelaySampling;
    
    maxStereoDistance = pitchChanger.maxStereoDistance;
    stereoDistance = pitchChanger.stereoDistance;

    vibratoMode = pitchChanger.vibratoMode;
    vibratoWave = pitchChanger.vibratoWave;
    
    transpositionPosition = pitchChanger.transpositionPosition;
    
    return *this;
  }

  virtual ~PitchChanger() {
    /* if(pitchChangerPanel) {
      GuiElement *parent = pitchChangerPanel->parent;
      parent->deleteChildElement(pitchChangerPanel);
    }*/
  }


  
  PitchChanger() : PostProcessingEffect("Pitch scaler") {
  }
  
  PitchChanger(double sampleRate) : PostProcessingEffect("Pitch scaler")  {
    this->sampleRate = sampleRate;
    sampleLength = 1.0/sampleRate;
    maxStereoDistance = 1.0/10;
    delayLine.init(sampleRate, transpositionMaxDelay+maxStereoDistance);
  }
  
  PitchChanger(double sampleRate, PitchChanger &pitchChanger) : PostProcessingEffect("Pitch scaler")  {
    this->sampleRate = sampleRate;
    sampleLength = 1.0/sampleRate;
    maxStereoDistance = 1.0/10;
    delayLine.init(sampleRate, transpositionMaxDelay+maxStereoDistance);
    transpositionAmount = pitchChanger.transpositionAmount;
    transpositionMaxDelay = pitchChanger.transpositionMaxDelay;
  }
  
  void init(double sampleRate, double transpositionMaxDelay, double transpositionDelay) {
    this->sampleRate = sampleRate;
    sampleLength = 1.0/sampleRate;
    this->transpositionMaxDelay = transpositionMaxDelay;
    this->transpositionDelay = min(transpositionDelay, transpositionMaxDelay);
    maxStereoDistance = 1.0/10;
    delayLine.init(sampleRate, transpositionMaxDelay+maxStereoDistance);
  }
  
  double time = 0;
  
  void apply(Vec2d &sample) {
    if(!isActive) return;
    
    delayLine.update(sample);
    
    double transpositionAmount = this->transpositionAmount;
    
    if(vibratoMode != None) {
      time += sampleLength;
      
      double wave = 0;
      if(vibratoWave == Triangle) {
        wave = triangleWave(time*vibratoFrequence);
      }
      else if(vibratoWave == Sin) {
        wave = sin(time*vibratoFrequence*2.0*PI);
      }
      else if(vibratoWave == Saw) {
        wave = sawWave(time*vibratoFrequence);
      }
      else if(vibratoWave == BackwardSaw) {
        wave = sawWave(-time*vibratoFrequence);
      }
      else if(vibratoWave == Square) {
        wave = squareWave(time*vibratoFrequence);
      }
      
      if(vibratoMode == Linear) {
        transpositionAmount = transpositionAmount + transpositionAmount * linearVibratoDepth * wave;
      }
      else if(vibratoMode == Power) {
        transpositionAmount = transpositionAmount * pow(powerVibratoDepth, wave);
      }
    }
    
    transpositionPosition += sampleLength;
    transpositionPosition -= sampleLength * transpositionAmount;
    
    if(transpositionPosition <= 0) {
      transpositionPosition = transpositionDelay;
    }
    else if(transpositionPosition >= transpositionDelay) {
      transpositionPosition = 0;
    }
    Vec2d a;
    
    if(stereoDistance > 0) {
      a.x = delayLine.getOutputSampleWithInterpolation(transpositionPosition).x;
      a.y = delayLine.getOutputSampleWithInterpolation(transpositionPosition+stereoDistance).y;
    }
    else {
      a = delayLine.getOutputSampleWithInterpolation(transpositionPosition);
    }
    
    if(dualDelaySampling) {
      double hw = transpositionDelay * 0.5;
      
      double transpositionPosition2 = transpositionPosition + hw;
      if(transpositionPosition2 >= transpositionDelay) {
        transpositionPosition2 -= hw * 2.0;
      }
      
      Vec2d a2 = delayLine.getOutputSampleWithInterpolation(transpositionPosition2);
      
      double x = 1.0 - min(1.0, fabs(transpositionDelay * 0.5 - transpositionPosition) / hw);
      
      a = a * x + a2 * (1.0 - x);
    }
    
    sample = a * wetLevel + sample * (1.0 - wetLevel);

    sample *= gain;
  }


  void reset() {
    delayLine.reset();
    transpositionPosition = 0;
    time = 0;
  }

  struct PitchChangerPanel : public Panel {
    //Button *isActiveGui = NULL;
    SynthTitleBar *titleBar = NULL;
    
    static const int numLayouts = 3;
    int layout = 1;
    std::vector<SynthGuiLayout> pitchGuiLayouts = std::vector<SynthGuiLayout>(numLayouts);
    
    NumberBox *wetLevelGui = NULL, *gainGui = NULL;

    CheckBox *dualDelaySamplingGui = NULL;
    NumberBox *transpositionAmountGui = NULL;
    NumberBox *transpositionMaxDelayGui = NULL;

    ListBox *vibratoModeGui = NULL;
    ListBox *vibratoWaveGui = NULL;
    NumberBox *vibratoFrequenceGui = NULL;
    NumberBox *linearVibratoDepthGui = NULL;
    NumberBox *powerVibratoDepthGui = NULL;

    NumberBox *stereoDistanceGui = NULL;
    
    
    RotaryKnob<double> *wetLevelKnob = NULL, *gainKnob = NULL;

    RotaryKnob<int> *dualDelaySamplingKnob = NULL;
    RotaryKnob<double> *transpositionAmountKnob = NULL;
    RotaryKnob<double> *transpositionMaxDelayKnob = NULL;

    RotaryKnob<long> *vibratoModeKnob = NULL;
    RotaryKnob<long> *vibratoWaveKnob = NULL;
    RotaryKnob<double> *vibratoFrequenceKnob = NULL;
    RotaryKnob<double> *linearVibratoDepthKnob = NULL;
    RotaryKnob<double> *powerVibratoDepthKnob = NULL;

    RotaryKnob<double> *stereoDistanceKnob = NULL;
    
    
    double line = 0, lineHeight = 23;
    int width = 100, widthA = 100;
    int columnA = 10, columnB = 10+widthA, columnC = 10+widthA+width, columnD = 10+widthA+width*2-25, columnE = 10+widthA+width*3-35;
    int columnF = 10+widthA+width*4;
    
    GuiElement *parentGuiElement = NULL;
    PitchChanger *pitchChanger = NULL;
    
    PitchChangerPanel(PitchChanger *pitchChanger, GuiElement *parentGuiElement) : Panel("PitchChanger panel") {
      init(pitchChanger, parentGuiElement);
    }
    
    void init(PitchChanger *pitchChanger, GuiElement *parentGuiElement) {
      this->pitchChanger = pitchChanger;
      this->parentGuiElement = parentGuiElement;
      //this->synth = synth;
      
      setSize(270, 50);

      addGuiEventListener(new PitchChangerPanelListener(this));
      parentGuiElement->addChildElement(this);
      
      /*addChildElement(new Label(pitchChanger->getName(), 10, line));
      addChildElement(isActiveGui = new CheckBox("Active", pitchChanger->isActive, 10, line += lineHeight));*/
      
      /*TitleBar *titleBar;
      addChildElement(titleBar = new TitleBar(pitchChanger->getName(), 300, 25, 0, line));
      addChildElement(isActiveGui = new Button("Power", "data/synth/textures/power.png", 290-30, 5, Button::ToggleButton, pitchChanger->isActive));
      titleBar->getGlowLevel = [this]() { return isActiveGui->getActivationLevel(); };
      line += 30;*/
      titleBar = new SynthTitleBar(pitchChanger->getName(), this, pitchChanger->isActive, layout);
      

      for(int i=0; i<numLayouts; i++) {
        addChildElement(pitchGuiLayouts[i].gui = new GuiElement("Pitch changer layout "+std::to_string(i)));
        pitchGuiLayouts[i].gui->setPosition(0, titleBar->size.y + 5);
      }
      
      // BOX LAYOUT
      
      pitchGuiLayouts[0].gui->addChildElement(wetLevelGui = new NumberBox("Wet level", pitchChanger->wetLevel, NumberBox::FLOATING_POINT, 0, 1, 10, line, 7));
      pitchGuiLayouts[0].gui->addChildElement(gainGui = new NumberBox("Gain", pitchChanger->gain, NumberBox::FLOATING_POINT, 0, 1e10, 10, line += lineHeight, 7));

      pitchGuiLayouts[0].gui->addChildElement(dualDelaySamplingGui = new CheckBox("Dual sampling", pitchChanger->dualDelaySampling, 10, line += lineHeight));
      
      pitchGuiLayouts[0].gui->addChildElement(transpositionAmountGui = new NumberBox("Transposition amount", pitchChanger->transpositionAmount, NumberBox::FLOATING_POINT, 0, 1e10, 10, line += lineHeight, 7));
      pitchGuiLayouts[0].gui->addChildElement(transpositionMaxDelayGui = new NumberBox("Max delay", pitchChanger->transpositionDelay, NumberBox::FLOATING_POINT, 0, pitchChanger->delayLine.bufferLength, 10, line += lineHeight, 7));

      pitchGuiLayouts[0].gui->addChildElement(vibratoModeGui = new ListBox("Vibrato mode", 10, line += lineHeight, 10));
      vibratoModeGui->setItems(pitchChanger->vibratoModeNames);
      vibratoModeGui->setValue(pitchChanger->vibratoMode);

      pitchGuiLayouts[0].gui->addChildElement(vibratoWaveGui = new ListBox("Vibrato wave", 10, line += lineHeight, 10));
      vibratoWaveGui->setItems(pitchChanger->vibratoWaveNames);
      vibratoWaveGui->setValue(pitchChanger->vibratoWave);

      pitchGuiLayouts[0].gui->addChildElement(vibratoFrequenceGui = new NumberBox("Vibrato frequency", pitchChanger->vibratoFrequence, NumberBox::FLOATING_POINT, 0, 1e10, 10, line += lineHeight, 7));
      pitchGuiLayouts[0].gui->addChildElement(linearVibratoDepthGui = new NumberBox("Vibrato depth linear", pitchChanger->linearVibratoDepth, NumberBox::FLOATING_POINT, 0, 1.0, 10, line += lineHeight, 6));
      pitchGuiLayouts[0].gui->addChildElement(powerVibratoDepthGui = new NumberBox("Vibrato depth power", pitchChanger->powerVibratoDepth, NumberBox::FLOATING_POINT, 1.0, 4.0, 10, line += lineHeight, 6));

      pitchGuiLayouts[0].gui->addChildElement(stereoDistanceGui = new NumberBox("Phase offset", pitchChanger->stereoDistance, NumberBox::FLOATING_POINT, 0.0, pitchChanger->maxStereoDistance, 10, line += lineHeight, 7));
      
      pitchGuiLayouts[0].gui->setSize(270, line + lineHeight + 7);
      pitchGuiLayouts[0].size = pitchGuiLayouts[0].gui->pos + pitchGuiLayouts[0].gui->size;
      
      
      // KNOB LAYOUT
      RowColumnPlacer placer(270, 5, 0);
      
      pitchGuiLayouts[1].gui->addChildElement(wetLevelKnob = new RotaryKnob<double>("Wet level", placer.progressX(), placer.getY(), RotaryKnob<double>::LimitedKnob, 0, 1, pitchChanger->wetLevel));

      pitchGuiLayouts[1].gui->addChildElement(gainKnob = new RotaryKnob<double>("Gain", placer.progressX(), placer.getY(), RotaryKnob<double>::LimitedKnob, 0, 1, pitchChanger->gain));

      pitchGuiLayouts[1].gui->addChildElement(dualDelaySamplingKnob = new RotaryKnob<int>("Dual sampling", placer.progressX(), placer.getY(), RotaryKnob<int>::LimitedKnob, 0, 1, pitchChanger->dualDelaySampling));

      pitchGuiLayouts[1].gui->addChildElement(transpositionAmountKnob = new RotaryKnob<double>("Transposition", placer.progressX(), placer.getY(), RotaryKnob<double>::LimitedKnob, 0, 6, pitchChanger->transpositionAmount));

      pitchGuiLayouts[1].gui->addChildElement(transpositionMaxDelayKnob = new RotaryKnob<double>("Max delay", placer.progressX(), placer.getY(), RotaryKnob<double>::LimitedKnob, 0, pitchChanger->delayLine.bufferLength, pitchChanger->transpositionDelay));

      pitchGuiLayouts[1].gui->addChildElement(vibratoModeKnob = new RotaryKnob<long>("Vibrato mode", placer.progressX(), placer.getY(), RotaryKnob<long>::ListKnob));
      vibratoModeKnob->setItems(pitchChanger->vibratoModeNames);
      vibratoModeKnob->setValue(pitchChanger->vibratoMode);

      pitchGuiLayouts[1].gui->addChildElement(vibratoWaveKnob = new RotaryKnob<long>("Vibrato wave", placer.progressX(), placer.getY(), RotaryKnob<long>::ListKnob));
      vibratoWaveKnob->setItems(pitchChanger->vibratoWaveNames);
      vibratoWaveKnob->setValue(pitchChanger->vibratoWave);
      
      pitchGuiLayouts[1].gui->addChildElement(vibratoFrequenceKnob = new RotaryKnob<double>("Vibrato frequency", placer.progressX(), placer.getY(), RotaryKnob<double>::LimitedKnob, 0, 30, pitchChanger->vibratoFrequence));

      pitchGuiLayouts[1].gui->addChildElement(linearVibratoDepthKnob = new RotaryKnob<double>("Vibrato depth, linear", placer.progressX(), placer.getY(), RotaryKnob<double>::LimitedKnob, 0, 1, pitchChanger->linearVibratoDepth));

      pitchGuiLayouts[1].gui->addChildElement(powerVibratoDepthKnob = new RotaryKnob<double>("Vibrato depth, power", placer.progressX(), placer.getY(), RotaryKnob<double>::LimitedKnob, 1, 4, pitchChanger->powerVibratoDepth));

      pitchGuiLayouts[1].gui->addChildElement(stereoDistanceKnob = new RotaryKnob<double>("Phase offset", placer.progressX(), placer.getY(), RotaryKnob<double>::LimitedKnob, 0, pitchChanger->maxStereoDistance, pitchChanger->stereoDistance));
      
      pitchGuiLayouts[1].gui->setSize(270, placer.getY() + placer.knobSize);
      pitchGuiLayouts[1].size = pitchGuiLayouts[1].gui->pos + pitchGuiLayouts[1].gui->size;

      
      // TITLE BAR LAYOUT
      pitchGuiLayouts[2].size = titleBar->size;
      

      update();
    }
    
    void update() {
      for(int i=0; i<numLayouts; i++) {
        pitchGuiLayouts[i].gui->setVisible(i == layout);
      }
      setSize(pitchGuiLayouts[layout].size);
    }
    

    struct PitchChangerPanelListener : public GuiEventListener {
      PitchChangerPanel *pitchChangerPanel;
      PitchChangerPanelListener(PitchChangerPanel *pitchChangerPanel) {
        this->pitchChangerPanel = pitchChangerPanel;
      }

      void onValueChange(GuiElement *guiElement) {

        if(guiElement == pitchChangerPanel->titleBar->isActiveGui) {
          guiElement->getValue((void*)&pitchChangerPanel->pitchChanger->isActive);
        }
        if(guiElement == pitchChangerPanel->titleBar->layoutGui) {
          pitchChangerPanel->layout = (pitchChangerPanel->layout+1) % pitchChangerPanel->numLayouts;
          pitchChangerPanel->update();
        }

        
        
        if(guiElement == pitchChangerPanel->wetLevelGui || guiElement == pitchChangerPanel->wetLevelKnob) {
          guiElement->getValue((void*)&pitchChangerPanel->pitchChanger->wetLevel);
        }
        if(guiElement == pitchChangerPanel->gainGui || guiElement == pitchChangerPanel->gainKnob) {
          guiElement->getValue((void*)&pitchChangerPanel->pitchChanger->gain);
        }

        if(guiElement == pitchChangerPanel->dualDelaySamplingGui || guiElement == pitchChangerPanel->dualDelaySamplingKnob) {
          guiElement->getValue((void*)&pitchChangerPanel->pitchChanger->dualDelaySampling);
        }
        
        if(guiElement == pitchChangerPanel->vibratoModeGui || guiElement == pitchChangerPanel->vibratoModeKnob) {
          guiElement->getValue((void*)&pitchChangerPanel->pitchChanger->vibratoMode);
        }
        if(guiElement == pitchChangerPanel->vibratoModeGui || guiElement == pitchChangerPanel->vibratoModeKnob) {
          guiElement->getValue((void*)&pitchChangerPanel->pitchChanger->vibratoMode);
        }
        if(guiElement == pitchChangerPanel->vibratoWaveGui || guiElement == pitchChangerPanel->vibratoWaveKnob) {
          guiElement->getValue((void*)&pitchChangerPanel->pitchChanger->vibratoWave);
        }
        
        if(guiElement == pitchChangerPanel->transpositionMaxDelayGui || guiElement == pitchChangerPanel->transpositionMaxDelayKnob) {
          guiElement->getValue((void*)&pitchChangerPanel->pitchChanger->transpositionDelay);
        }
        if(guiElement == pitchChangerPanel->transpositionAmountGui || guiElement == pitchChangerPanel->transpositionAmountKnob) {
          guiElement->getValue((void*)&pitchChangerPanel->pitchChanger->transpositionAmount);
        }

        if(guiElement == pitchChangerPanel->vibratoFrequenceGui || guiElement == pitchChangerPanel->vibratoFrequenceKnob) {
          guiElement->getValue((void*)&pitchChangerPanel->pitchChanger->vibratoFrequence);
        }
        if(guiElement == pitchChangerPanel->linearVibratoDepthGui || guiElement == pitchChangerPanel->linearVibratoDepthKnob) {
          guiElement->getValue((void*)&pitchChangerPanel->pitchChanger->linearVibratoDepth);
        }
        if(guiElement == pitchChangerPanel->powerVibratoDepthGui || guiElement == pitchChangerPanel->powerVibratoDepthKnob) {
          guiElement->getValue((void*)&pitchChangerPanel->pitchChanger->powerVibratoDepth);
        }
        if(guiElement == pitchChangerPanel->stereoDistanceGui || guiElement == pitchChangerPanel->stereoDistanceKnob) {
          guiElement->getValue((void*)&pitchChangerPanel->pitchChanger->stereoDistance);
        }
      }

    };
  };
  
  PitchChangerPanel *pitchChangerPanel = NULL;
  
  Panel *getPanel() {
    return pitchChangerPanel;
  }

  Panel *addPanel(GuiElement *parentGuiElement, const std::string &title = "") {
    return pitchChangerPanel = new PitchChangerPanel(this, parentGuiElement);
  }
  
  void removePanel(GuiElement *parentGuiElement) {
    PanelInterface::removePanel(parentGuiElement);

    if(pitchChangerPanel) {
      parentGuiElement->deleteChildElement(pitchChangerPanel);
    }
  }

  void updatePanel() {
    if(pitchChangerPanel) {
      pitchChangerPanel->update();
    }
  }
  

  static std::string getClassName() {
    return "pitchScaler";
  }
  
  virtual std::string getBlockName() {
    return getClassName();
  }  
};