#pragma once
#include "../gui/gui.h"
#include "../textfileparser.h"

struct VoiceCrossModulation : public HierarchicalTextFileParser {
  struct VoiceCrossModulationPanel : public Panel {
    //std::vector<ModularParameter*> modularParameters;
    std::vector<Label*> horizontalLabels, verticalLabels;
    std::vector<NumberBox*> amountsGui;
    std::vector<NumberBox*> frequencyFactorsGui;
    
    ListBox *voiceCrossModulationModeGui;
    
    VoiceCrossModulation *voiceCrossModulation;
    
    VoiceCrossModulationPanel() : Panel("Voice cross modulation panel") {}
    
    VoiceCrossModulationPanel(VoiceCrossModulation *voiceCrossModulation) : Panel("Voice cross modulation panel") {
      init(voiceCrossModulation);
    }
    
    double cellWidth = 85;
    double line = 10, lineHeight = 23;
    
    void init(VoiceCrossModulation *voiceCrossModulation) {
      this->voiceCrossModulation = voiceCrossModulation;
      
      horizontalLabels.resize(maxNumVoices);
      verticalLabels.resize(maxNumVoices);
      amountsGui.resize(maxNumVoices*maxNumVoices);
      frequencyFactorsGui.resize(maxNumVoices*maxNumVoices);
      
      addChildElement(voiceCrossModulationModeGui = new ListBox("Voice cross modulation", 10, line, 15));
      voiceCrossModulationModeGui->setItems(voiceCrossModulationModeNames);
      voiceCrossModulationModeGui->setValue(voiceCrossModulation->mode);
      
      for(int i=0; i<maxNumVoices; i++) {
        addChildElement(verticalLabels[i] = new Label(std::to_string(i+1), 10, line + lineHeight*2 + i*lineHeight));
        addChildElement(horizontalLabels[i] = new Label(std::to_string(i+1), 10 + cellWidth*(i+1)-cellWidth/2, line+lineHeight));
      }
      
      for(int y=0; y<maxNumVoices; y++) {
        for(int x=0; x<maxNumVoices; x++) {
          addChildElement(amountsGui[x+y*maxNumVoices] = new NumberBox("", voiceCrossModulation->amplitudeFactors[x+y*maxNumVoices], NumberBox::FLOATING_POINT, 0, 1, 10 + (x) * cellWidth+25, line + lineHeight*2 + y*lineHeight, 6, 6));
          //addChildElement(amountsGui[x+y*maxNumVoices] = new NumberBox("", intstument->frequencyFactors[x+y*maxNumVoices], NumberBox::FLOATING_POINT, 10 + (x+1) * cellWidth, line + lineHeight*2 + y*lineHeight*2, 0, 1, 6, 6));
        }
      }
      
      addGuiEventListener(new VoiceCrossModulationPanelListener(this));
      
      update();
    }
    
    void update() {
      
      voiceCrossModulationModeGui->setValue(voiceCrossModulation->mode);
      
      for(int y=0; y<maxNumVoices; y++) {
        verticalLabels[y]->setVisible(y < voiceCrossModulation->numVoices);
        horizontalLabels[y]->setVisible(y < voiceCrossModulation->numVoices);
        for(int x=0; x<maxNumVoices; x++) {
          amountsGui[x+y*maxNumVoices]->setVisible(x < voiceCrossModulation->numVoices && y < voiceCrossModulation->numVoices);
          if(voiceCrossModulation->mode == VoiceCrossModulationMode::CrossModulationAmplitude || voiceCrossModulation->mode == VoiceCrossModulationMode::CrossModulationRing) {
            amountsGui[x+y*maxNumVoices]->setLimits(0, 1);
            amountsGui[x+y*maxNumVoices]->setValue(voiceCrossModulation->amplitudeFactors[x+y*maxNumVoices]);
          }
          if(voiceCrossModulation->mode == VoiceCrossModulationMode::CrossModulationFrequency) {
            amountsGui[x+y*maxNumVoices]->setLimits(0, 1e10);
            amountsGui[x+y*maxNumVoices]->setValue(voiceCrossModulation->frequencyFactors[x + y*maxNumVoices]);
          }
        }
      }
      
      if(voiceCrossModulation->numVoices == 0) {
        setSize(400, 10+lineHeight+10);
      }
      else {
        setSize(max(400, 10 + cellWidth*(voiceCrossModulation->numVoices))+25, 10 + lineHeight*(voiceCrossModulation->numVoices+2)+10);
      }
    }
    
    struct VoiceCrossModulationPanelListener : public GuiEventListener {
      VoiceCrossModulationPanel *voiceCrossModulationPanel;
      VoiceCrossModulationPanelListener(VoiceCrossModulationPanel *voiceCrossModulationPanel) {
        this->voiceCrossModulationPanel = voiceCrossModulationPanel;
      }
      virtual void onValueChange(GuiElement *guiElement) {
        if(guiElement == voiceCrossModulationPanel->voiceCrossModulationModeGui) {
          guiElement->getValue((void*)&voiceCrossModulationPanel->voiceCrossModulation->mode);
          voiceCrossModulationPanel->update();
        }
        for(int y=0; y<voiceCrossModulationPanel->voiceCrossModulation->numVoices; y++) {
          for(int x=0; x<voiceCrossModulationPanel->voiceCrossModulation->numVoices; x++) {
            if(guiElement == voiceCrossModulationPanel->amountsGui[x+y*maxNumVoices]) {
              if(voiceCrossModulationPanel->voiceCrossModulation->mode == VoiceCrossModulationMode::CrossModulationAmplitude || voiceCrossModulationPanel->voiceCrossModulation->mode == VoiceCrossModulationMode::CrossModulationRing) {
                guiElement->getValue((void*)&voiceCrossModulationPanel->voiceCrossModulation->amplitudeFactors[x + y*maxNumVoices]);
              }
              if(voiceCrossModulationPanel->voiceCrossModulation->mode == VoiceCrossModulationMode::CrossModulationFrequency) {
                guiElement->getValue((void*)&voiceCrossModulationPanel->voiceCrossModulation->frequencyFactors[x + y*maxNumVoices]);
              }
              voiceCrossModulationPanel->voiceCrossModulation->checkConnected(y);
            }
          }
        }
      }
    };
    
    struct PanelUpdateListener : public GuiEventListener {
      VoiceCrossModulationPanel *voiceCrossModulationPanel;
      PanelUpdateListener(VoiceCrossModulationPanel *voiceCrossModulationPanel) {
        this->voiceCrossModulationPanel = voiceCrossModulationPanel;
      }
      virtual void onMessage(GuiElement *guiElement, const std::vector<std::string> &message) {
        if(message.size() > 1 && message[0] == "voiceschange") {
          toInt(message[1], voiceCrossModulationPanel->voiceCrossModulation->numVoices);
          voiceCrossModulationPanel->update();
        }
      }
    };
    
  };
  
  VoiceCrossModulationPanel *voiceCrossModulationPanel = NULL;
  
  VoiceCrossModulationPanel *getPanel() {
    return voiceCrossModulationPanel ? voiceCrossModulationPanel : new VoiceCrossModulationPanel(this);
  }

  std::vector<double> amplitudeFactors;
  std::vector<double> frequencyFactors;

  std::vector<bool> isVoiceConnected;
  VoiceCrossModulationMode mode = VoiceCrossModulationMode::CrossModulatationNone;
  
  int numVoices = 0;
  
  void reset() {
    numVoices = 0;
    mode = VoiceCrossModulationMode::CrossModulatationNone;
    amplitudeFactors.assign(maxNumVoices*maxNumVoices, 0);
    frequencyFactors.assign(maxNumVoices*maxNumVoices, 0);
    isVoiceConnected.assign(maxNumVoices*maxNumVoices, false);
    isVoiceConnected.assign(maxNumVoices, false);
  }

  void checkConnected() {
    for(int i=0; i<numVoices; i++) {
      checkConnected(i);
    }
  }
  
  void checkConnected(int voiceIndex) {
    if(voiceIndex < 0 || voiceIndex >= numVoices) return;
    
    if(mode == VoiceCrossModulationMode::CrossModulationAmplitude || mode == VoiceCrossModulationMode::CrossModulationRing) {
      isVoiceConnected[voiceIndex] = false;
      for(int i=0; i<numVoices; i++) {
        if(amplitudeFactors[i + voiceIndex*maxNumVoices] > 0) {
          isVoiceConnected[voiceIndex] = true;
          break;
        }
      }
    }
    if(mode == VoiceCrossModulationMode::CrossModulationFrequency) {
      isVoiceConnected[voiceIndex] = false;
      for(int i=0; i<numVoices; i++) {
        if(frequencyFactors[i + voiceIndex*maxNumVoices] > 0) {
          isVoiceConnected[voiceIndex] = true;
          break;
        }
      }
    }
  }
  
  VoiceCrossModulation() {
    amplitudeFactors.assign(maxNumVoices*maxNumVoices, 0);
    frequencyFactors.assign(maxNumVoices*maxNumVoices, 0);
    isVoiceConnected.assign(maxNumVoices*maxNumVoices, false);
  }
  
  VoiceCrossModulation &operator=(const VoiceCrossModulation &voiceCrossModulation) {
    amplitudeFactors = voiceCrossModulation.amplitudeFactors;
    frequencyFactors = voiceCrossModulation.frequencyFactors;

    isVoiceConnected = voiceCrossModulation.isVoiceConnected;
    mode = voiceCrossModulation.mode;
    
    numVoices = voiceCrossModulation.numVoices;
    
    return *this;
  }
  
  void setNumVoices(int numVoices) {
    this->numVoices = numVoices;

    checkConnected();
  }

  static std::string getClassName() {
    return "voiceCrossModulation";
  }

  virtual std::string getBlockName() {
    return getClassName();
  }


  virtual void decodeParameters() {
    std::string modeName;
    getStringParameter("mode", modeName);
    for(int i=0; i<voiceCrossModulationModeNames.size(); i++) {
      if(modeName == voiceCrossModulationModeNames[i]) mode = (VoiceCrossModulationMode)i;
    }
    getNumericParameter("numVoices", numVoices);
    numVoices = min(numVoices, maxNumVoices);
    
    if(mode == VoiceCrossModulationMode::CrossModulationAmplitude || mode == VoiceCrossModulationMode::CrossModulationRing) {
      for(int y=0; y<numVoices; y++) {
        for(int x=0; x<numVoices; x++) {
          getNumericParameter("amounts", amplitudeFactors[x+y*maxNumVoices], x+y*numVoices);
        }
      }
    }

    if(mode == VoiceCrossModulationMode::CrossModulationFrequency) {
      for(int y=0; y<numVoices; y++) {
        for(int x=0; x<numVoices; x++) {
          getNumericParameter("amounts", frequencyFactors[x+y*maxNumVoices], x+y*numVoices);
        }
      }
    }
    checkConnected();
  }

  virtual void encodeParameters() {
    clearParameters();
    putStringParameter("mode", voiceCrossModulationModeNames[mode]);
    putNumericParameter("numVoices", numVoices);
    if(mode == VoiceCrossModulationMode::CrossModulationAmplitude || mode == VoiceCrossModulationMode::CrossModulationRing) {
      //putNumericParameter("amounts", amplitudeFactors);
      doubleParameters.push_back(DoubleParameter("amounts"));
      for(int y=0; y<numVoices; y++) {
        for(int x=0; x<numVoices; x++) {
          doubleParameters[doubleParameters.size()-1].values.push_back(amplitudeFactors[x+y*maxNumVoices]);
        }
      }
    }
    if(mode == VoiceCrossModulationMode::CrossModulationFrequency) {
      doubleParameters.push_back(DoubleParameter("amounts"));
      for(int y=0; y<numVoices; y++) {
        for(int x=0; x<numVoices; x++) {
          doubleParameters[doubleParameters.size()-1].values.push_back(frequencyFactors[x+y*maxNumVoices]);
        }
      }
    }

  }

};