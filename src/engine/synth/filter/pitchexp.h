#pragma once
#include "biquad.h"

/* TODO 
 *  - multi step waves
 *  - vibrato offset
 */

struct PitchExperiment : public PostProcessingEffect
{
  DelayLine delayLine;
  double sampleRate = 1;
  double sampleLength = 0;
  double transpositionMaxDelay = 1;

  double wetLevel = 1.0;
  double gain = 1.0;

  double transpositionAmount = 1;
  double transpositionDelay = 0;
  
  double vibratoFrequence = 1;
  double powerVibratoDepth = 1.05;
  double linearVibratoDepth = 0.05;
  
  bool dualDelaySampling = false;
  
  enum VibratoWave { Triangle, Sin, Square, Saw, BackwardSaw };
  enum VibratoMode { None, Linear, Power };
  const std::vector<std::string> vibratoWaveNames = { "Triangle", "Sin", "Square", "Saw", "Backward saw" };
  const std::vector<std::string> vibratoModeNames = { "None", "Linear", "Power" };
  
  enum Experiment { NoExperiment, Steps, Exp1, Exp2, Exp3, Exp4, Exp5 };
  const std::vector<std::string> experimentNames = { "None", "Steps", "Stereo cross 1", "Stereo cross 2", "Stereo cross 3", "Power", "Sin distort", "NumExperiments" };
  Experiment experiment = NoExperiment;
  int numSteps = 20;
  int currentStep = 0;
  Vec2d currentStepSample;
  
  double crossFactor = 0; // 0 - 1
  double powFactor = 0; // > 0
  
  double trigDistortFactor = 20.0;
  double trigDistortVolume = 0.1;
  
  
  VibratoMode vibratoMode = None;
  VibratoWave vibratoWave = Triangle;
  
  double transpositionPosition = 0;
  
  PitchExperiment() : PostProcessingEffect("Pitch experiment") {
  }
  
  PitchExperiment(double sampleRate) : PostProcessingEffect("Pitch experiment")  {
    this->sampleRate = sampleRate;
    sampleLength = 1.0/sampleRate;
    delayLine.init(sampleRate, transpositionMaxDelay);
  }
  
  PitchExperiment(double sampleRate, PitchExperiment &pitchExperiment) : PostProcessingEffect("Pitch experiment")  {
    this->sampleRate = sampleRate;
    sampleLength = 1.0/sampleRate;
    delayLine.init(sampleRate, transpositionMaxDelay);
    transpositionAmount = pitchExperiment.transpositionAmount;
    transpositionMaxDelay = pitchExperiment.transpositionMaxDelay;
  }
  
  void init(double sampleRate, double transpositionMaxDelay, double transpositionDelay) {
    this->sampleRate = sampleRate;
    sampleLength = 1.0/sampleRate;
    this->transpositionMaxDelay = transpositionMaxDelay;
    this->transpositionDelay = min(transpositionDelay, transpositionMaxDelay);
    delayLine.init(sampleRate, transpositionMaxDelay);
  }
  
  double time = 0;
  
  void apply(Vec2d &sample) {
    if(!isActive) return;
    
    delayLine.update(sample);
    
    if(transpositionDelay > 0) {
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
      
      Vec2d a = delayLine.getOutputSampleWithInterpolation(transpositionPosition);
      
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
      
      //sample = a * wetLevel + sample * (1.0 - wetLevel);
      sample = a;
    }
    
    if(experiment != NoExperiment) {
      Vec2d sampleTmp;
      if(experiment == Steps) { 
        // this was the primary thing I had in mind, let's see. Not too interesting. Maybe useful as a bulding block for more elaborate things
        // Or just as plain distortian similarly as those others...
        if(currentStep == 0) currentStepSample = sample;
        sampleTmp = currentStepSample;
        currentStep = (currentStep+1) % numSteps;
      }
      if(experiment == Exp1) {
        sampleTmp.x = sample.x * (1.0 - crossFactor) + sample.y * crossFactor;
        sampleTmp.y = sample.y * (1.0 - crossFactor) + sample.x * crossFactor;
      }
      if(experiment == Exp2) {
        sampleTmp.x = sample.x * (1.0 - crossFactor + sample.y * crossFactor);
        sampleTmp.y = sample.y * (1.0 - crossFactor + sample.x * crossFactor);
      }
      if(experiment == Exp3) {
        sampleTmp.x = sample.x * (1.0 + sample.y * crossFactor);
        sampleTmp.y = sample.y * (1.0 + sample.x * crossFactor);
      }
      if(experiment == Exp4) {
        sampleTmp.x = sign(sample.x) * pow(fabs(sample.x), powFactor);
        sampleTmp.y = sign(sample.y) * pow(fabs(sample.y), powFactor);
      }
      if(experiment == Exp5) {
        sampleTmp.x = sin(sample.x*trigDistortFactor) * trigDistortVolume;
        sampleTmp.y = sin(sample.y*trigDistortFactor) * trigDistortVolume;
      }
      sample = sampleTmp * wetLevel + sample * (1.0 - wetLevel);
    }

    
    

    sample *= gain;
  }


  void reset() {
    delayLine.reset();
    transpositionPosition = 0;
    time = 0;
  }

  struct PitchChangerPanel : public Panel {
    CheckBox *isActiveGui = NULL;
    NumberBox *wetLevelGui = NULL, *gainGui = NULL;

    CheckBox *dualDelaySamplingGui = NULL;
    NumberBox *transpositionAmountGui = NULL;
    NumberBox *transpositionMaxDelayGui = NULL;

    ListBox *vibratoModeGui = NULL;
    ListBox *vibratoWaveGui = NULL;
    NumberBox *vibratoFrequenceGui = NULL;
    NumberBox *linearVibratoDepthGui = NULL;
    NumberBox *powerVibratoDepthGui = NULL;

    ListBox *experimentGui = NULL;
    NumberBox *numStepsGui = NULL;
    NumberBox *crossFactorGui = NULL;
    NumberBox *powFactorGui = NULL;
    NumberBox *trigDistortFactorGui = NULL;
    NumberBox *trigDistortVolumeGui = NULL;
    
    double line = 10, lineHeight = 23;
    int width = 100, widthA = 100;
    int columnA = 10, columnB = 10+widthA, columnC = 10+widthA+width, columnD = 10+widthA+width*2-25, columnE = 10+widthA+width*3-35;
    int columnF = 10+widthA+width*4;
    
    GuiElement *parentGuiElement = NULL;
    PitchExperiment *pitchExperiment = NULL;
    
    PitchChangerPanel(PitchExperiment *pitchExperiment, GuiElement *parentGuiElement) : Panel("PitchExperiment panel") {
      init(pitchExperiment, parentGuiElement);
    }
    
    void init(PitchExperiment *pitchExperiment, GuiElement *parentGuiElement) {
      this->pitchExperiment = pitchExperiment;
      this->parentGuiElement = parentGuiElement;
      //this->synth = synth;
      
      Experiment experiment = NoExperiment;
      int numSteps = 20;
      
      double crossFactor = 0; // 0 - 1
      double powFactor = 0; // > 0
      
      double trigDistortFactor = 20.0;
      double trigDistortVolume = 0.1;


      addGuiEventListener(new PitchChangerPanelListener(this));
      parentGuiElement->addChildElement(this);
      
      addChildElement(new Label(pitchExperiment->getName(), 10, line));
      addChildElement(isActiveGui = new CheckBox("Active", pitchExperiment->isActive, 10, line += lineHeight));
      addChildElement(wetLevelGui = new NumberBox("Wet level", pitchExperiment->wetLevel, NumberBox::FLOATING_POINT, 0, 1, 10, line += lineHeight, 8));
      addChildElement(gainGui = new NumberBox("Gain", pitchExperiment->gain, NumberBox::FLOATING_POINT, 0, 1e10, 10, line += lineHeight, 8));

      addChildElement(experimentGui = new ListBox("Experiment", 10, line += lineHeight));
      experimentGui->setItems(pitchExperiment->experimentNames);
      experimentGui->setValue(pitchExperiment->experiment);

      addChildElement(numStepsGui = new NumberBox("Steps", pitchExperiment->numSteps, NumberBox::INTEGER, 1, 1<<29, 10, line += lineHeight, 8));
      addChildElement(crossFactorGui = new NumberBox("Cross factor", pitchExperiment->crossFactor, NumberBox::FLOATING_POINT, 0, 1, 10, line += lineHeight, 8));
      addChildElement(powFactorGui = new NumberBox("Pow factor", pitchExperiment->powFactor, NumberBox::FLOATING_POINT, 1e-6, 1e10, 10, line += lineHeight, 8));
      addChildElement(trigDistortFactorGui = new NumberBox("Sin distort factor", pitchExperiment->trigDistortFactor, NumberBox::FLOATING_POINT, -1e10, 1e10, 10, line += lineHeight, 8));
      addChildElement(trigDistortVolumeGui = new NumberBox("Sin distort volume", pitchExperiment->trigDistortVolume, NumberBox::FLOATING_POINT, 0, 1, 10, line += lineHeight, 8));


      addChildElement(dualDelaySamplingGui = new CheckBox("Dual sampling", pitchExperiment->dualDelaySampling, 10, line += lineHeight));
      
      addChildElement(transpositionAmountGui = new NumberBox("Transposition amount", pitchExperiment->transpositionAmount, NumberBox::FLOATING_POINT, 0, 1e10, 10, line += lineHeight, 8));
      addChildElement(transpositionMaxDelayGui = new NumberBox("Transposition max delay", pitchExperiment->transpositionDelay, NumberBox::FLOATING_POINT, 0, pitchExperiment->delayLine.bufferLength, 10, line += lineHeight, 8));

      addChildElement(vibratoModeGui = new ListBox("Vibrato mode", 10, line += lineHeight));
      vibratoModeGui->setItems(pitchExperiment->vibratoModeNames);
      vibratoModeGui->setValue(pitchExperiment->vibratoMode);

      addChildElement(vibratoWaveGui = new ListBox("Vibrato wave", 10, line += lineHeight));
      vibratoWaveGui->setItems(pitchExperiment->vibratoWaveNames);
      vibratoWaveGui->setValue(pitchExperiment->vibratoWave);

      addChildElement(vibratoFrequenceGui = new NumberBox("Vibrato frequency", pitchExperiment->vibratoFrequence, NumberBox::FLOATING_POINT, 0, 1e10, 10, line += lineHeight, 8));
      addChildElement(linearVibratoDepthGui = new NumberBox("Vibrato depth (linear)", pitchExperiment->linearVibratoDepth, NumberBox::FLOATING_POINT, 0, 1.0, 10, line += lineHeight, 8));
      addChildElement(powerVibratoDepthGui = new NumberBox("Vibrato depth (power)", pitchExperiment->powerVibratoDepth, NumberBox::FLOATING_POINT, 1.0, 4.0, 10, line += lineHeight, 8));
      
      

      update();
      setSize(320, line + lineHeight + 10);
      

    }
    
    void update() {
      
    }

    struct PitchChangerPanelListener : public GuiEventListener {
      PitchChangerPanel *pitchChangerPanel;
      PitchChangerPanelListener(PitchChangerPanel *pitchChangerPanel) {
        this->pitchChangerPanel = pitchChangerPanel;
      }

      void onValueChange(GuiElement *guiElement) {

        if(guiElement == pitchChangerPanel->isActiveGui) {
          guiElement->getValue((void*)&pitchChangerPanel->pitchExperiment->isActive);
        }
        if(guiElement == pitchChangerPanel->wetLevelGui) {
          guiElement->getValue((void*)&pitchChangerPanel->pitchExperiment->wetLevel);
        }
        if(guiElement == pitchChangerPanel->gainGui) {
          guiElement->getValue((void*)&pitchChangerPanel->pitchExperiment->gain);
        }

        if(guiElement == pitchChangerPanel->experimentGui) {
          guiElement->getValue((void*)&pitchChangerPanel->pitchExperiment->experiment);
        }
        if(guiElement == pitchChangerPanel->numStepsGui) {
          guiElement->getValue((void*)&pitchChangerPanel->pitchExperiment->numSteps);
        }
        if(guiElement == pitchChangerPanel->crossFactorGui) {
          guiElement->getValue((void*)&pitchChangerPanel->pitchExperiment->crossFactor);
        }
        if(guiElement == pitchChangerPanel->powFactorGui) {
          guiElement->getValue((void*)&pitchChangerPanel->pitchExperiment->powFactor);
        }
        if(guiElement == pitchChangerPanel->trigDistortFactorGui) {
          guiElement->getValue((void*)&pitchChangerPanel->pitchExperiment->trigDistortFactor);
        }
        if(guiElement == pitchChangerPanel->trigDistortVolumeGui) {
          guiElement->getValue((void*)&pitchChangerPanel->pitchExperiment->trigDistortVolume);
        }


        if(guiElement == pitchChangerPanel->dualDelaySamplingGui) {
          guiElement->getValue((void*)&pitchChangerPanel->pitchExperiment->dualDelaySampling);
        }
        
        if(guiElement == pitchChangerPanel->vibratoModeGui) {
          guiElement->getValue((void*)&pitchChangerPanel->pitchExperiment->vibratoMode);
        }
        if(guiElement == pitchChangerPanel->vibratoModeGui) {
          guiElement->getValue((void*)&pitchChangerPanel->pitchExperiment->vibratoMode);
        }
        if(guiElement == pitchChangerPanel->vibratoWaveGui) {
          guiElement->getValue((void*)&pitchChangerPanel->pitchExperiment->vibratoWave);
        }
        
        if(guiElement == pitchChangerPanel->transpositionMaxDelayGui) {
          guiElement->getValue((void*)&pitchChangerPanel->pitchExperiment->transpositionDelay);
        }
        if(guiElement == pitchChangerPanel->transpositionAmountGui) {
          guiElement->getValue((void*)&pitchChangerPanel->pitchExperiment->transpositionAmount);
        }

        if(guiElement == pitchChangerPanel->vibratoFrequenceGui) {
          guiElement->getValue((void*)&pitchChangerPanel->pitchExperiment->vibratoFrequence);
        }
        if(guiElement == pitchChangerPanel->linearVibratoDepthGui) {
          guiElement->getValue((void*)&pitchChangerPanel->pitchExperiment->linearVibratoDepth);
        }
        if(guiElement == pitchChangerPanel->powerVibratoDepthGui) {
          guiElement->getValue((void*)&pitchChangerPanel->pitchExperiment->powerVibratoDepth);
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
  
  
  
  
};