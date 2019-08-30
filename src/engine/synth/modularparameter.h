#pragma once
#include "synthconstants.h"
#include "../gui/gui.h"
#include "../mathl.h"


// I am now prototyping one idea, but this kind of feature might be nice enough to have for almost every parameter in the synth instruments
// It would mean rewriting of much of the code, once again :|
// That midi channel could be utilized in either midi key or any other knob or slider input on the midi controller
// I was thinking about making parameters controllable by microphone too! That would allow quite diverse kind of control.
// One might create completely artificial voice for oneself for example. I am not very far from there, but not yet.



struct ModularParameter {
  struct ModularTrackPanel : public Panel {
    NumberBox *modulatorGui, *envelopeGui, *midiGui, *voiceGui, *voiceWetLevelGui;
    RangeBox *rangeGui;
    
    ModularParameter *modularParameter;
    
    ModularTrackPanel(ModularParameter *modularParameter) : Panel("Modular parameter panel '" + modularParameter->name + "'") {
      init(modularParameter);
    }
    
    void init(ModularParameter *modularParameter) {
      this->modularParameter = modularParameter;
      double x = 0, y = 0;
      addChildElement(modulatorGui = new NumberBox("Mod", modularParameter->modulator, NumberBox::INTEGER, -1, maxNumModulators, x, y, 2));
      modulatorGui->incrementMode = NumberBox::IncrementMode::Linear;
      modulatorGui->setValidValueRange(0, 16);
      addChildElement(envelopeGui = new NumberBox("Env", modularParameter->envelope, NumberBox::INTEGER, -1, maxNumEnvelopes, x+85, y, 2));
      envelopeGui->incrementMode = NumberBox::IncrementMode::Linear;
      envelopeGui->setValidValueRange(0, 16);
      //addChildElement(voiceGui = new NumberBox("Voice", modularParameter->voice, NumberBox::INTEGER, -1, maxNumModulators, x+85*2-5, y, 2));
      //voiceGui->incrementMode = NumberBox::IncrementMode::Linear;
      //addChildElement(voiceWetLevelGui = new NumberBox("", modularParameter->wetLevel, NumberBox::FLOATING_POINT, 0, 1, x+85*2-5, y, 6));
      //voiceWetLevelGui->incrementMode = NumberBox::IncrementMode::Linear;
      //voiceWetLevelGui->linearIncrementAmount = 0.1;
      addChildElement(midiGui = new NumberBox("Midi", modularParameter->midi, NumberBox::INTEGER, -1, 1<<29, x+85*2, y, 2));
      midiGui->incrementMode = NumberBox::IncrementMode::Linear;
      midiGui->setValidValueRange(0, 1<<29);
      addChildElement(rangeGui = new RangeBox("Range", modularParameter->range, modularParameter->valueLimits, true, x+85*3, y, 6));
      addGuiEventListener(new ModularTrackPanelListener(this));
    }
    
    void update() {
      modulatorGui->setValue(modularParameter->modulator);
      envelopeGui->setValue(modularParameter->envelope);
      midiGui->setValue(modularParameter->midi);
      rangeGui->setValue(modularParameter->range);
    }
    
    struct ModularTrackPanelListener : public GuiEventListener {
      ModularTrackPanel *modularTrackPanel;
      ModularTrackPanelListener(ModularTrackPanel *modularTrackPanel) {
        this->modularTrackPanel = modularTrackPanel;
      }
      void onValueChange(GuiElement *guiElement) {
        if(guiElement == modularTrackPanel->modulatorGui) {
          guiElement->getValue((void*)&modularTrackPanel->modularParameter->modulator);
          modularTrackPanel->modularParameter->checkConnected();
          modularTrackPanel->sendMessage("");
        }
        if(guiElement == modularTrackPanel->envelopeGui) {
          guiElement->getValue((void*)&modularTrackPanel->modularParameter->envelope);
          modularTrackPanel->modularParameter->checkConnected();
          modularTrackPanel->sendMessage("");
        }
        if(guiElement == modularTrackPanel->voiceGui) {
          guiElement->getValue((void*)&modularTrackPanel->modularParameter->voice);
          modularTrackPanel->modularParameter->checkConnected();
          modularTrackPanel->sendMessage("");
        }
        if(guiElement == modularTrackPanel->midiGui) {
          guiElement->getValue((void*)&modularTrackPanel->modularParameter->midi);
          modularTrackPanel->modularParameter->checkConnected();
          modularTrackPanel->sendMessage("");
        }
        if(guiElement == modularTrackPanel->voiceWetLevelGui) {
          guiElement->getValue((void*)&modularTrackPanel->modularParameter->wetLevel);
        }
        if(guiElement == modularTrackPanel->rangeGui) {
          guiElement->getValue((void*)&modularTrackPanel->modularParameter->range);
        }
      }
    };
  };

  struct ParameterListener : public GuiEventListener {
    ModularParameter *modularParameter;
    ParameterListener(ModularParameter *modularParameter) {
      this->modularParameter = modularParameter;
    }
    void onValueChange(GuiElement *guiElement) {
      if(guiElement == modularParameter->valueGui) {
        guiElement->getValue((void*)&modularParameter->value);
      }
    }
  };
  
  /*static std::string getClassName() {
    return "modulator";
  }

  virtual std::string getBlockName() {
    return getClassName();
  }


  virtual void decodeParameters() {
    getVectorParameter("range", range);
    getNumericParameter("modulator", modulator);
    getNumericParameter("envelope", envelope);
    getNumericParameter("pan", pan);
  }

  virtual void encodeParameters() {
    clearParameters();
    putVectorParameter("range", range);
    putNumericParameter("modulator", modulator);
    putNumericParameter("envelope", envelope);
    putNumericParameter("pan", pan);
    //putNumericParameter("midi", midi);

  }*/
  
  
  double *originalValue = NULL;
  double value = 0;
  Vec2d range;
  Vec2d valueLimits;
  std::string name;
  int modulator = -1, envelope = -1, midi = -1, voice = -1;
  double wetLevel = 0.5;

  bool isConnected = false;
  
  ModularTrackPanel *modularTrackPanel = NULL;
  
  GuiEventListener *guiEventListener = NULL;
  
  bool checkConnected() {
    isConnected = modulator >= 0 || envelope >= 0 || midi >= 0 || voice >= 0;

    if(!isConnected && originalValue) {
      value = *originalValue;
    }
    printf("modular parameter '%s' connected = %d, value = %f\n", name.c_str(), isConnected, value);
    return isConnected;
  }

  NumberBox *valueGui = NULL;

  ModularParameter() {}

  ModularParameter(const std::string &name, double *value, double min, double max, double rangeMin, double rangeMax) {
    init(name, value, min, max, rangeMin, rangeMax);
  }
  ModularParameter(const std::string &name, double *value, double min, double max) {
    init(name, value, min, max, *value, *value);
  }

  ModularParameter &init(const std::string &name, double *value, double min, double max, GuiEventListener *guiEventListener = NULL) {
    this->name = name;
    this->originalValue = value;
    valueLimits.set(min, max);
    range.set(clamp(*value, min, max), clamp(*value, min, max));
    this->guiEventListener = guiEventListener;
    modulator = -1;
    envelope = -1;
    midi = -1;
    voice = -1;
    
    return *this;
  }

  ModularParameter &init(const std::string &name, double *value, double min, double max, double rangeMin, double rangeMax, GuiEventListener *guiEventListener = NULL) {
    this->name = name;
    this->originalValue = value;
    valueLimits.set(min, max);
    range.set(clamp(rangeMin, min, max), clamp(rangeMax, min, max));
    this->guiEventListener = guiEventListener;
    modulator = -1;
    envelope = -1;
    midi = -1;
    voice = -1;
    return *this;
  }

  Panel *getModularTrackPanel() {
    if(!modularTrackPanel) {
      modularTrackPanel = new ModularTrackPanel(this);
      if(guiEventListener) {
        modularTrackPanel->addGuiEventListener(guiEventListener);
      }
    }
    else {
      modularTrackPanel->update();
    }
    return modularTrackPanel;
  }

  
  inline void operator=(double value) {
    this->value = value;
  }
  inline void operator+=(double value) {
    this->value += value;
  }
  inline void operator-=(double value) {
    this->value -= value;
  }
  inline void operator*=(double value) {
    this->value *= value;
  }
  inline void operator/=(double value) {
    this->value /= value;
  }

  inline double operator+(double value) const {
    return this->value + value;
  }
  inline double operator-(double value) const {
    return this->value - value;
  }
  inline double operator*(double value) const {
    return this->value * value;
  }
  inline double operator/(double value) const {
    return this->value / value;
  }

  inline bool operator==(double value) const {
    return this->value == value;
  }
  inline bool operator!=(double value) const {
    return this->value != value;
  }
  inline bool operator>(double value) const {
    return this->value > value;
  }
  inline bool operator>=(double value) const {
    return this->value >= value;
  }
  inline bool operator<(double value) const {
    return this->value < value;
  }
  inline bool operator<=(double value) const {
    return this->value <= value;
  }
  
  inline double operator-() const {
    return -value;
  }





};
