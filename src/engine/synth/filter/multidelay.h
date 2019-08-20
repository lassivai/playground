#pragma once
#include "postprocessingeffect.h"


/* TODO
 *  - more stereo stuff
 *  - feedforward not working the way it should, probably seperate delay line for it
 *  - todo floating point delays getting interpolated samples from delay line
 */
 
struct MultiDelay : public PostProcessingEffect {
  /*double reverbLength = 2;
  double exponentialDecay = -4.0;
  double linearDecay = 0.4;
  int numSamples = 20;*/
  
  enum DelayDistribution { Random, RandomPrimes, Even, EvenPrimes, ProgressivePrimes };
  const std::vector<std::string> delayDistributionNames = { "Random", "Random primes", "Even", "Even primes", "Progressive primes" };
  
  static const int maxDelays = 100;
  int numInputDelays = 1, numOutputDelays = 1;
  std::vector<int> inputDelays, outputDelays;
  std::vector<double> inputDelayFactors, outputDelayFactors;
  double factorSum = 0;

  //static const int numPresets = 3;

  double wetLevel = 0.7;

  double gain = 1.0;
  
  int feedForwardMode = 0;
  double feedForwardFactor = 0.5;

  virtual ~MultiDelay() {
    if(panel) {
      GuiElement *parent = panel->parent;
      parent->deleteChildElement(panel);
    }
  }


  MultiDelay(DelayLine *delayLine, const MultiDelay &multiDelay) : PostProcessingEffect(delayLine, "Multidelay") {
    //printf("(debugging) at MultiDelay::MultiDelay(DelayLine *delayLine, const MultiDelay &multiDelay)\n");
    inputDelays.resize(maxDelays);
    outputDelays.resize(maxDelays);
    inputDelayFactors.resize(maxDelays);
    outputDelayFactors.resize(maxDelays);

    this->wetLevel = multiDelay.wetLevel;
    this->gain = multiDelay.gain;
    this->numInputDelays = multiDelay.numInputDelays;
    this->numOutputDelays = multiDelay.numOutputDelays;

    for(int i=0; i<inputDelays.size(); i++) {
      this->inputDelays[i] = multiDelay.inputDelays[i];
    }
    for(int i=0; i<outputDelays.size(); i++) {
      this->outputDelays[i] = multiDelay.outputDelays[i];
    }
    for(int i=0; i<inputDelayFactors.size(); i++) {
      this->inputDelayFactors[i] = multiDelay.inputDelayFactors[i];
    }
    for(int i=0; i<outputDelayFactors.size(); i++) {
      this->outputDelayFactors[i] = multiDelay.outputDelayFactors[i];
    }
  }


  MultiDelay(DelayLine *delayLine) : PostProcessingEffect(delayLine, "Multidelay") {
    inputDelays.resize(maxDelays);
    outputDelays.resize(maxDelays);
    inputDelayFactors.resize(maxDelays, 1.0);
    outputDelayFactors.resize(maxDelays, 1.0);
    //setPreset(3);
  }



  void distributeDelays(Vec2d delayRange, DelayDistribution distibution = DelayDistribution::Random) {
    if(numInputDelays < 1 && numOutputDelays < 1) return;

    int minInputDelay = (int)clamp((delayRange.x * delayLine->sampleRate), 0, delayLine->bufferSize-1);
    int maxInputDelay = (int)clamp((delayRange.y * delayLine->sampleRate), 0, delayLine->bufferSize-1);
    int minOutputDelay = (int)clamp((delayRange.x * delayLine->sampleRate), 0, delayLine->bufferSize-1);
    int maxOutputDelay = (int)clamp((delayRange.y * delayLine->sampleRate), 0, delayLine->bufferSize-1);
    double minInputFactor = 1.0, maxInputFactor = 1.0;
    double minOutputFactor = 1.0, maxOutputFactor = 1.0;

    if(minInputDelay > maxInputDelay) minInputDelay = maxInputDelay;
    if(minOutputDelay > maxOutputDelay) minOutputDelay = maxOutputDelay;
    
    if(distibution == ProgressivePrimes) {
      unsigned long minInd, maxInd;
      unsigned long minPrime = primes.getClosestPrime(minInputDelay, minInd);
      unsigned long maxPrime = primes.getClosestPrime(maxInputDelay, maxInd);
      primes.ensureMaxPrime(maxPrime);
      
      if(numInputDelays == 1) {
        inputDelays[0] = primes.getNthPrime((maxInd+minInd) / 2);
      }
      else {
        inputDelays[0] = minPrime;
        inputDelays[numInputDelays-1] = maxPrime;
        for(int i=1; i<numInputDelays-1; i++) {
          int n = (int)map(n, 0.0, numInputDelays-1.0, minInd, maxInd);
          inputDelays[i] = primes.getNthPrime(n);
        }
      }
      
      minPrime = primes.getClosestPrime(minOutputDelay, minInd);
      maxPrime = primes.getClosestPrime(maxOutputDelay, maxInd);
      if(numOutputDelays == 1) {
        outputDelays[0] = primes.getNthPrime((maxInd+minInd) / 2);
      }
      else {
        inputDelays[0] = minPrime;
        inputDelays[numInputDelays-1] = maxPrime;
        for(int i=1; i<numOutputDelays-1; i++) {
          int n = (int)map(n, 0.0, numOutputDelays-1.0, minInd, maxInd);
          outputDelays[i] = primes.getNthPrime(n);
        }
      }
    }
    else {
      for(int i=0; i<numInputDelays; i++) {        
        if(distibution == Random) {
          inputDelays[i] = Random::getInt(minInputDelay, maxInputDelay);
        }
        else if(distibution == RandomPrimes) {
          inputDelays[i] = primes.getClosestPrime(Random::getInt(minInputDelay, maxInputDelay));
        }
        else if(distibution == Even) {
          inputDelays[i] = (int)map(i, 0.0, numInputDelays-1, minInputDelay, maxInputDelay);
        }
        else if(distibution == EvenPrimes) {
          inputDelays[i] = primes.getClosestPrime((int)map(i, 0.0, numInputDelays-1, minInputDelay, maxInputDelay));
        }
      }
      for(int i=0; i<numOutputDelays; i++) {        
        if(distibution == Random) {
          outputDelays[i] = Random::getInt(minOutputDelay, maxOutputDelay);
        }
        else if(distibution == RandomPrimes) {
          outputDelays[i] = primes.getClosestPrime(Random::getInt(minOutputDelay, maxOutputDelay));
        }
        else if(distibution == Even) {
          outputDelays[i] = (int)map(i, 0.0, numOutputDelays-1, minOutputDelay, maxOutputDelay);
        }
        else if(distibution == EvenPrimes) {
          outputDelays[i] = primes.getClosestPrime((int)map(i, 0.0, numOutputDelays-1, minOutputDelay, maxOutputDelay));
        }
      }
    }
    
    for(int i=0; i<numInputDelays; i++) {
      inputDelayFactors[i] = Random::getDouble(minInputFactor, maxInputFactor);
    }
    for(int i=0; i<numOutputDelays; i++) {
      outputDelayFactors[i] = Random::getDouble(minOutputFactor, maxOutputFactor);
    }
    
    if(panel) {
      updatePanel();
    }
    prepare();
  }

  /*void setPreset(int index = 0) {
    if(index == 1) {
      numInputDelays = 4;
      numOutputDelays = 4;
      inputDelays[0] = 4000;
      inputDelays[1] = 9000;
      inputDelays[2] = 22000;
      inputDelays[3] = 30000;
      outputDelays[0] = 10000;
      outputDelays[1] = 15000;
      outputDelays[2] = 25000;
      outputDelays[3] = 32000;
      inputDelayFactors[0] = 1;
      inputDelayFactors[1] = 1;
      inputDelayFactors[2] = 1;
      inputDelayFactors[3] = 1;
      outputDelayFactors[0] = -1;
      outputDelayFactors[1] = -1;
      outputDelayFactors[2] = -1;
      outputDelayFactors[3] = -1;
    }
    if(index == 2) {
      if(numInputDelays < 1 && numOutputDelays < 1) return;

      int minInputDelay = 0, maxInputDelay = delayLine->bufferSize;
      int minOutputDelay = 0, maxOutputDelay = delayLine->bufferSize;
      double minInputFactor = 0.0, maxInputFactor = 1.0;
      double minOutputFactor = 0.0, maxOutputFactor = 1.0;

      // FIXME some utility function to wrap this kind of stuff, and do it better
      if(numInputDelays > 0) {
        inputDelaysGui[0]->getValue(&maxInputDelay);
        inputDelayFactorsGui[0]->getValue(&maxInputFactor);
      }
      if(numInputDelays > 1) {
        inputDelaysGui[1]->getValue(&minInputDelay);
        inputDelayFactorsGui[1]->getValue(&minInputFactor);
      }
      int tmpMinDelay = minInputDelay;
      double tmpMinFactor = minInputFactor;
      maxInputDelay = max(minInputDelay, maxInputDelay);
      maxInputFactor = max(minInputFactor, maxInputFactor);
      minInputDelay = min(minInputDelay, tmpMinDelay);
      minInputFactor = min(minInputFactor, tmpMinFactor);

      if(numOutputDelays > 0) {
        outputDelaysGui[0]->getValue(&maxOutputDelay);
        outputDelayFactorsGui[0]->getValue(&maxOutputFactor);
      }
      if(numOutputDelays > 1) {
        outputDelaysGui[1]->getValue(&minOutputDelay);
        outputDelayFactorsGui[1]->getValue(&minOutputFactor);
      }
      tmpMinDelay = minOutputDelay;
      tmpMinFactor = minOutputFactor;
      maxOutputDelay = max(minOutputDelay, maxOutputDelay);
      maxOutputFactor = max(minOutputFactor, maxOutputFactor);
      minOutputDelay = min(minOutputDelay, tmpMinDelay);
      minOutputFactor = min(minOutputFactor, tmpMinFactor);

      for(int i=0; i<numInputDelays; i++) {
        inputDelays[i] = Random::getInt(minInputDelay, maxInputDelay);
        inputDelayFactors[i] = Random::getDouble(minInputFactor, maxInputFactor);
      }
      for(int i=0; i<numOutputDelays; i++) {
        outputDelays[i] = Random::getInt(minOutputDelay, maxOutputDelay);
        outputDelayFactors[i] = Random::getDouble(minOutputFactor, maxOutputFactor);
      }
    }
    if(index == 3) {
      numInputDelays = 0;
      numOutputDelays = 1;
      outputDelays[0] = 1;
      outputDelayFactors[0] = 1;
      wetLevel = 0.9;
      gain = 1.1;
    }

    if(panel) {
      updatePanel();
    }
    prepare();
  }*/

   void prepare() {
     factorSum = 0;
     for(int i=0; i<numInputDelays; i++) {
       factorSum += fabs(inputDelayFactors[i]);
     }
     for(int i=0; i<numOutputDelays; i++) {
       factorSum += fabs(outputDelayFactors[i]);
     }
   }

   void apply(Vec2d &sample) {
     if(!isActive) return;

     Vec2d r;

     for(int i=0; i<numInputDelays; i++) {
       r += delayLine->getInputSample(inputDelays[i]) * inputDelayFactors[i];
       //delayLine->feedForward(sample * inputDelayFactors[i], inputDelays[i]);
     }
     
     for(int i=0; i<numOutputDelays; i++) {
       if(feedForwardMode == 1) {
         r += delayLine->getOutputSample(outputDelays[i]) * outputDelayFactors[i] * feedForwardFactor;         
         delayLine->feedForward(-sample.x*outputDelayFactors[i] * feedForwardFactor, -sample.y*outputDelayFactors[i] * feedForwardFactor, outputDelays[i]);
       }
       else if(feedForwardMode == 2) {
         r += delayLine->getOutputSample(outputDelays[i]) * outputDelayFactors[i] * feedForwardFactor;         
         delayLine->feedForward(-sample.y*outputDelayFactors[i] * feedForwardFactor, -sample.x*outputDelayFactors[i] * feedForwardFactor, outputDelays[i]);
       }
       else {
         r += delayLine->getOutputSample(outputDelays[i]) * outputDelayFactors[i];         
       }
     }
     if(factorSum != 0) r /= factorSum;

     sample.x = (1.0-wetLevel) * sample.x + wetLevel * r.x;
     sample.y = (1.0-wetLevel) * sample.y + wetLevel * r.y;

     sample *= gain;
   }
  /*void update() {
    if(!isActive) return;

    Vec2d r;

    for(int i=0; i<numInputDelays; i++) {
      r += delayLine->getInputSample(inputDelays[i]) * inputDelayFactors[i];
    }
    for(int i=0; i<numOutputDelays; i++) {
      r += delayLine->getOutputSample(outputDelays[i]) * outputDelayFactors[i];
    }
    if(factorSum != 0) r /= factorSum;

    delayLine->buffer[delayLine->bufferPos].x = (1.0-wetLevel) * delayLine->buffer[delayLine->bufferPos].x + wetLevel * r.x;
    delayLine->buffer[delayLine->bufferPos].y = (1.0-wetLevel) * delayLine->buffer[delayLine->bufferPos].y + wetLevel * r.y;

    delayLine->buffer[delayLine->bufferPos] *= gain;
  }*/

  /*std::string getName() {
    return "Multidelay";
  }*/
  SynthTitleBar *titleBar = NULL;

  Button *isActiveGui = NULL;
  NumberBox *numInputDelaysGui = NULL, *numOutputDelaysGui = NULL;
  std::vector<NumberBox*> inputDelaysGui = std::vector<NumberBox*>(maxDelays, NULL);
  std::vector<NumberBox*> outputDelaysGui = std::vector<NumberBox*>(maxDelays, NULL);
  std::vector<NumberBox*> inputDelayFactorsGui = std::vector<NumberBox*>(maxDelays, NULL);
  std::vector<NumberBox*> outputDelayFactorsGui = std::vector<NumberBox*>(maxDelays, NULL);
  NumberBox *wetLevelGui = NULL, *gainGui = NULL;

  //ListBox *presetsGui = NULL;


  Panel *getPanel() {
    return panel;
  }

  Panel *addPanel(GuiElement *parentElement, const std::string &title = "") {
    double line = 0, lineHeight = 23;

    if(!panel) {
      panel = new Panel(340, 320, 60, 10);

      panel->addGuiEventListener(new FilterTestPanelListener(this));

      /*Label *label = new Label(getName(), 10, line);
      isActiveGui = new CheckBox("Active", isActive, 10, line += lineHeight);*/
      
      /*TitleBar *titleBar;
      panel->addChildElement(titleBar = new TitleBar(getName(), 340, 25, 0, line));
      panel->addChildElement(isActiveGui = new Button("Power", "data/synth/textures/power.png", 340-30, 5, Button::ToggleButton, isActive));
      titleBar->getGlowLevel = [this]() { return isActiveGui->getActivationLevel(); };
      line += 30;*/
      
      titleBar = new SynthTitleBar(getName(), panel, isActive);
      line += titleBar->size.y + 5;
      
      wetLevelGui = new NumberBox("Wet level", wetLevel, NumberBox::FLOATING_POINT, 0, 1, 10, line, 7);
      gainGui = new NumberBox("Gain", gain, NumberBox::FLOATING_POINT, 0, 1e10, 10, line += lineHeight, 7);
      numInputDelaysGui = new NumberBox("Input count", numInputDelays, NumberBox::INTEGER, 0, maxDelays, 10, line += lineHeight, 4);
      numOutputDelaysGui = new NumberBox("Output count", numOutputDelays, NumberBox::INTEGER, 0, maxDelays, 10, line += lineHeight, 4);

      /*presetsGui = new ListBox("Presets", 10, line += lineHeight, 4);
      for(int i=0; i<numPresets; i++) {
        presetsGui->addItems(std::to_string(i+1));
      }*/

      //panel->addChildElement(label);
      //panel->addChildElement(isActiveGui);
      panel->addChildElement(wetLevelGui);
      panel->addChildElement(gainGui);
      panel->addChildElement(numInputDelaysGui);
      panel->addChildElement(numOutputDelaysGui);
      //panel->addChildElement(presetsGui);

      parentElement->addChildElement(panel);
    }
    else {
      for(int i=0; i<maxDelays; i++) {
        panel->deleteChildElement(inputDelaysGui[i]);
        panel->deleteChildElement(inputDelayFactorsGui[i]);
        panel->deleteChildElement(outputDelaysGui[i]);
        panel->deleteChildElement(outputDelayFactorsGui[i]);
      }
      wetLevelGui->setValue(wetLevel);
      gainGui->setValue(gain);
      numInputDelaysGui->setValue(numInputDelays);
      numOutputDelaysGui->setValue(numOutputDelays);
      line += titleBar->size.y + 5 + lineHeight * 3;
    }


    for(int i=0; i<numInputDelays; i++) {
      //inputDelaysGui[i] = new NumberBox("In"+std::to_string(i+1), inputDelays[i], NumberBox::INTEGER, 0, delayLine->bufferSize, 10, line += lineHeight, 4);
      inputDelaysGui[i] = new NumberBox("In "+std::to_string(i+1)+" ", inputDelays[i]/delayLine->sampleRate, NumberBox::FLOATING_POINT, 1.0/delayLine->sampleRate, delayLine->bufferLength, 10, line += lineHeight, 7);
      if(inputDelays[i] <= 10) {
        inputDelaysGui[i]->incrementMode == NumberBox::IncrementMode::Linear;
        inputDelaysGui[i]->linearIncrementAmount = 1.0/delayLine->sampleRate;
      }
            
      inputDelayFactorsGui[i] = new NumberBox("gain "+std::to_string(i+1), inputDelayFactors[i], NumberBox::FLOATING_POINT, -1000000, 1000000, 10+160, line, 7);
      
      panel->addChildElement(inputDelaysGui[i]);
      panel->addChildElement(inputDelayFactorsGui[i]);
    }
    for(int i=0; i<numOutputDelays; i++) {
      //outputDelaysGui[i] = new NumberBox("Out "+std::to_string(i+1), outputDelays[i], NumberBox::INTEGER, 0, delayLine->bufferSize, 10, line += lineHeight, 4);
      outputDelaysGui[i] = new NumberBox("Out "+std::to_string(i+1), outputDelays[i]/delayLine->sampleRate, NumberBox::FLOATING_POINT, 1.0/delayLine->sampleRate, delayLine->bufferLength, 10, line += lineHeight, 7);
      if(outputDelays[i] <= 10) {
        outputDelaysGui[i]->incrementMode == NumberBox::IncrementMode::Linear;
        outputDelaysGui[i]->linearIncrementAmount = 1.0/delayLine->sampleRate;
      }

      outputDelayFactorsGui[i] = new NumberBox("gain "+std::to_string(i+1), outputDelayFactors[i], NumberBox::FLOATING_POINT, -1000000, 1000000, 10+160, line, 7);
      panel->addChildElement(outputDelaysGui[i]);
      panel->addChildElement(outputDelayFactorsGui[i]);
    }

    panel->setSize(340, line + lineHeight + 7);

    return panel;
  }

  void removePanel(GuiElement *parentElement) {
    PanelInterface::removePanel(parentElement);

    if(panel) {
      parentElement->deleteChildElement(panel);
      panel = NULL;
      isActiveGui = NULL;
      numInputDelaysGui = NULL;
      numOutputDelaysGui = NULL;
      inputDelaysGui.assign(maxDelays, NULL);
      outputDelaysGui.assign(maxDelays, NULL);
      inputDelayFactorsGui.assign(maxDelays, NULL);
      outputDelayFactorsGui.assign(maxDelays, NULL);
      wetLevelGui = NULL;
      gainGui = NULL;

      //presetsGui = NULL;
    }
  }

  void updatePanel() {
    if(panel) {
      addPanel(NULL);
    }
  }


  struct FilterTestPanelListener : public GuiEventListener {
    MultiDelay *multiDelay;
    FilterTestPanelListener(MultiDelay *multiDelay) {
      this->multiDelay = multiDelay;
    }
    void onAction(GuiElement *guiElement) {
      /*if(guiElement == multiDelay->presetsGui) {
        int index;
        guiElement->getValue(&index);
        multiDelay->setPreset(index+1);
      }*/
    }
    void onValueChange(GuiElement *guiElement) {
      if(guiElement == multiDelay->titleBar->isActiveGui) {
        guiElement->getValue((void*)&multiDelay->isActive);
      }
      /*if(guiElement == multiDelay->titleBar->layoutGui) {
        multiDelay->layout = (multiDelay->layout+1) % multiDelay->numLayouts;
        multiDelay->update();
      }*/
      
      /*if(guiElement == multiDelay->isActiveGui) {
        guiElement->getValue(&multiDelay->isActive);
      }*/
      if(guiElement == multiDelay->wetLevelGui) {
        guiElement->getValue(&multiDelay->wetLevel);
      }
      if(guiElement == multiDelay->gainGui) {
        guiElement->getValue(&multiDelay->gain);
      }
      if(guiElement == multiDelay->numInputDelaysGui) {
        int n = multiDelay->numInputDelays;
        guiElement->getValue(&multiDelay->numInputDelays);
        if(n != multiDelay->numInputDelays) multiDelay->updatePanel();
      }
      if(guiElement == multiDelay->numOutputDelaysGui) {
        int n = multiDelay->numOutputDelays;
        guiElement->getValue(&multiDelay->numOutputDelays);
        if(n != multiDelay->numOutputDelays) multiDelay->updatePanel();
      }
      for(int i=0; i<multiDelay->numInputDelays; i++) {
        if(guiElement == multiDelay->inputDelaysGui[i]) {
          double value;
          guiElement->getValue(&value);
          multiDelay->inputDelays[i] = (int)round(value * multiDelay->delayLine->sampleRate);
          if(multiDelay->inputDelays[i] <= 10) {
            multiDelay->inputDelaysGui[i]->incrementMode == NumberBox::IncrementMode::Linear;
            multiDelay->inputDelaysGui[i]->linearIncrementAmount = 1.0/multiDelay->delayLine->sampleRate;
          }
          else {
            multiDelay->inputDelaysGui[i]->incrementMode == NumberBox::IncrementMode::Default;
          }
        }
        if(guiElement == multiDelay->inputDelayFactorsGui[i]) {
          guiElement->getValue(&multiDelay->inputDelayFactors[i]);
        }
      }
      for(int i=0; i<multiDelay->numOutputDelays; i++) {
        if(guiElement == multiDelay->outputDelaysGui[i]) {
          double value;
          guiElement->getValue(&value);
          multiDelay->outputDelays[i] = (int)round(value * multiDelay->delayLine->sampleRate);
          if(multiDelay->outputDelays[i] <= 10) {
            multiDelay->outputDelaysGui[i]->incrementMode == NumberBox::IncrementMode::Linear;
            multiDelay->outputDelaysGui[i]->linearIncrementAmount = 1.0/multiDelay->delayLine->sampleRate;
          }
          else {
            multiDelay->outputDelaysGui[i]->incrementMode == NumberBox::IncrementMode::Default;
          }
        }
        if(guiElement == multiDelay->outputDelayFactorsGui[i]) {
          guiElement->getValue(&multiDelay->outputDelayFactors[i]);
        }
      }
      multiDelay->prepare();
    }
  };
  
  
  
  
  
  
  /*void applyPreset(int index) {
    if(!presets || index < 0 || index >= presets->size()) return;
    applyParameters(presets->at(index).doubleParameters, presets->at(index).stringParameters);
  }  */
  
  void applyParameters(const std::vector<DoubleParameter> &doubleParameters, const std::vector<StringParameter> &stringParameters) {

    getNumericParameter("gain", gain);
    getNumericParameter("wet", wetLevel);
    double sampleRateConversion = delayLine->sampleRate;;
    getNumericParameter("samplerate", sampleRateConversion);
    
    sampleRateConversion = delayLine->sampleRate / sampleRateConversion;
    
    inputDelays.clear();
    outputDelays.clear();
    inputDelayFactors.clear();
    outputDelayFactors.clear();
    
    for(int i=0; i<doubleParameters.size(); i++) {
      if(doubleParameters[i].values.size() > 1 && doubleParameters[i].name.compare("in") == 0) {
        // TODO floating point interpolation without truncation/rounding
        if(doubleParameters[i].values[0] <= 10) {
          inputDelays.push_back((int)doubleParameters[i].values[0]);
        }
        else {
          inputDelays.push_back((int)(doubleParameters[i].values[0]*sampleRateConversion));
        }
        inputDelayFactors.push_back(doubleParameters[i].values[1]);
      }
      else if(doubleParameters[i].values.size() > 1 && doubleParameters[i].name.compare("out") == 0) {
        if(doubleParameters[i].values[0] <= 10) {
          outputDelays.push_back((int)doubleParameters[i].values[0]);
        }
        else {
          outputDelays.push_back((int)(doubleParameters[i].values[0]*sampleRateConversion));
        }
        outputDelayFactors.push_back(doubleParameters[i].values[1]);
      }
    }
    numInputDelays = inputDelays.size();
    numOutputDelays = outputDelays.size();

    prepare();
  }
  
  static std::string getClassName() {
    return "multiDelay";
  }
  
  virtual std::string getBlockName() {
    return getClassName();
  }  
  
  virtual void decodeParameters() {
    applyParameters(doubleParameters, stringParameters);
  }

  virtual void encodeParameters() {
    clearParameters();

    putNumericParameter("gain", gain);
    putNumericParameter("wet", wetLevel);
    putNumericParameter("samplerate", delayLine->sampleRate);
    
    for(int i=0; i<numInputDelays; i++) {
      putNumericParameter("in", {(double)inputDelays[i], inputDelayFactors[i]});
    }
    for(int i=0; i<numOutputDelays; i++) {
      putNumericParameter("out", {(double)outputDelays[i], outputDelayFactors[i]});
    }

    /*if(presetName.size() > 0) {
      putStringParameter("presetName", presetName);
    }*/
  }
  
  
  
  
  
  
  
  
  
};
