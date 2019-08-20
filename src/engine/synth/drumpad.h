#pragma once

#include "instrument.h"

/*  TODO
 *   - all referenced instruments within
 */


static const int maxNumPads = 128;

struct DrumPad : public Instrument
{
  struct DrumPadPanel : public Panel
  {
    DrumPad *drumPad = NULL;
    NumberBox *numPadsGui = NULL;
    NumberBox *pitchOffsetGui = NULL;
    
    TextBox *drumPadNameGui = NULL;
    std::vector<ListBox*> instrumentNameGuis = std::vector<ListBox*>(maxNumPads, NULL);
    std::vector<NumberBox*> pitchGuis = std::vector<NumberBox*>(maxNumPads, NULL);
    std::vector<NumberBox*> volumeGuis = std::vector<NumberBox*>(maxNumPads, NULL);

    double lineHeight = 23;
    int width = 80, widthA = 175;
    int columnA = 10, columnB = 10+widthA, columnC = 10+widthA+width, columnD = 10+widthA+width*2-25, columnE = 10+widthA+width*3-35;


    GuiElement *parentGuiElement = NULL;
    //Synth *synth = NULL;

    /*DrumPadPanel(DrumPad *drumPad, Synth *synth, GuiElement *parentGuiElement) : Panel("Drum pad panel") {
      init(drumPad, synth, parentGuiElement);
    }*/
    DrumPadPanel(DrumPad *drumPad, GuiElement *parentGuiElement) : Panel("Drum pad panel") {
      init(drumPad, parentGuiElement);
    }

    void init(DrumPad *drumPad, GuiElement *parentGuiElement) {
      this->drumPad = drumPad;
      this->parentGuiElement = parentGuiElement;

      this->addGuiEventListener(new DrumPadPanelListener(this));
      parentGuiElement->addChildElement(this);
      this->setPosition(0, parentGuiElement->size.y + 5);

      double line = 10;
      
      drumPadNameGui = new TextBox("Drumpad", drumPad->name, 10, line);
      this->addChildElement(drumPadNameGui);
      
      numPadsGui = new NumberBox("Pads", drumPad->numPads, NumberBox::INTEGER, 1, maxNumPads, 10, line+=lineHeight, 4);
      numPadsGui->incrementMode = NumberBox::IncrementMode::Linear;
      addChildElement(numPadsGui);

      pitchOffsetGui = new NumberBox("PitchOffset", drumPad->pitchOffset, NumberBox::INTEGER, 0, 120, 120, line, 4);
      pitchOffsetGui->incrementMode = NumberBox::IncrementMode::Linear;
      addChildElement(pitchOffsetGui);
      
      addChildElement(new Label("Instrument", columnA, line+=lineHeight));
      addChildElement(new Label("Pitch", columnB, line));
      addChildElement(new Label("Volume", columnC, line));
      
      for(int i=0; i<maxNumPads; i++) {
        line += lineHeight;
        instrumentNameGuis[i] = new ListBox(std::to_string(i+1), columnA, line, 12);
        for(int k=0; k<drumPad->instruments->size(); k++) {
          instrumentNameGuis[i]->addItems(drumPad->instruments->at(k)->name);
        }
        instrumentNameGuis[i]->setValue(drumPad->pads[i].instrumentIndex);
        addChildElement(instrumentNameGuis[i]);
        
        pitchGuis[i] = new NumberBox("", drumPad->pads[i].pitch, NumberBox::FLOATING_POINT, 0, 1e6, columnB, line, 6);
        addChildElement(pitchGuis[i]);
        
        volumeGuis[i] = new NumberBox("", drumPad->pads[i].volume, NumberBox::FLOATING_POINT, 0, 1e6, columnC, line, 6);
        addChildElement(volumeGuis[i]);
      }
      
      //setSize(columnD+30, 10 + lineHeight * (drumPad->numPads+2) + 10);
      update();
    }
    
    void update() {
      numPadsGui->setValue(drumPad->numPads);
      pitchOffsetGui->setValue(drumPad->pitchOffset);
      
      for(int i=0; i<instrumentNameGuis.size(); i++) {
        instrumentNameGuis[i]->clearItems();
        for(int k=0; k<drumPad->instruments->size(); k++) {
          instrumentNameGuis[i]->addItems(drumPad->instruments->at(k)->name);
        }
        instrumentNameGuis[i]->setValue(drumPad->pads[i].instrumentIndex);
        
        instrumentNameGuis[i]->setVisible(i < drumPad->numPads);
        pitchGuis[i]->setVisible(i < drumPad->numPads);
        volumeGuis[i]->setVisible(i < drumPad->numPads);
      }
      
      setSize(columnD+30, 10 + lineHeight * (drumPad->numPads+3) + 10);
    }


    struct DrumPadPanelListener : public GuiEventListener {
      DrumPadPanel *drumPadPanel;
      DrumPadPanelListener(DrumPadPanel *drumPadPanel) {
        this->drumPadPanel = drumPadPanel;
      }
      void onValueChange(GuiElement *guiElement) {
        if(guiElement == drumPadPanel->drumPadNameGui) {
          guiElement->getValue((void*)&drumPadPanel->drumPad->name);
        }
        if(guiElement == drumPadPanel->numPadsGui) {
          guiElement->getValue((void*)&drumPadPanel->drumPad->numPads);
          drumPadPanel->update();
        }
        if(guiElement == drumPadPanel->pitchOffsetGui) {
          guiElement->getValue((void*)&drumPadPanel->drumPad->pitchOffset);
        }

        for(int i=0; i<drumPadPanel->instrumentNameGuis.size(); i++) {
          if(guiElement == drumPadPanel->instrumentNameGuis[i]) {
            int prevIndex = drumPadPanel->drumPad->pads[i].instrumentIndex;
            guiElement->getValue((void*)&drumPadPanel->drumPad->pads[i].instrumentIndex);
            if(prevIndex == drumPadPanel->drumPad->pads[i].instrumentIndex) return;
            int dir = (drumPadPanel->drumPad->pads[i].instrumentIndex < prevIndex && !(drumPadPanel->drumPad->pads[i].instrumentIndex >= 0 && prevIndex == drumPadPanel->drumPad->instruments->size()-1)) || (prevIndex >= 0 && drumPadPanel->drumPad->pads[i].instrumentIndex == drumPadPanel->drumPad->instruments->size()-1) ? -1 : 1;
            
            //drumPadPanel->synth->updateActiveInstruments();
            //drumPadPanel->synth->resetRecordedTrackNotes(i);
            while(drumPadPanel->drumPad->instruments->at(drumPadPanel->drumPad->pads[i].instrumentIndex)->instrumentType != Instrument::InstrumentType::DefaultInstrument) {
              drumPadPanel->drumPad->pads[i].instrumentIndex = (drumPadPanel->drumPad->pads[i].instrumentIndex + drumPadPanel->drumPad->instruments->size() + dir) % drumPadPanel->drumPad->instruments->size();
            }
            if(drumPadPanel->instrumentNameGuis[i]->items.size() != drumPadPanel->drumPad->instruments->size()) {
              drumPadPanel->update();
            }
            drumPadPanel->instrumentNameGuis[i]->setValue(drumPadPanel->drumPad->pads[i].instrumentIndex);
            drumPadPanel->drumPad->pads[i].referenceInstrumentName = drumPadPanel->drumPad->instruments->at(drumPadPanel->drumPad->pads[i].instrumentIndex)->name;
          }
          if(guiElement == drumPadPanel->volumeGuis[i]) {
            guiElement->getValue((void*)&drumPadPanel->drumPad->pads[i].volume);
          }
          if(guiElement == drumPadPanel->pitchGuis[i]) {
            guiElement->getValue((void*)&drumPadPanel->drumPad->pads[i].pitch);
          }
        }
      }
    };
  };

  
  struct Pad : public HierarchicalTextFileParser {
    std::string referenceInstrumentName;
    int instrumentIndex = 0;
    double pitch = 64;
    double volume = 0.75;
    
    static std::string getClassName() {
      return "pad";
    }
    
    virtual std::string getBlockName() {
      return getClassName();
    }
    
    virtual void decodeParameters() {
      getStringParameter("referenceInstrumentName", referenceInstrumentName);
      getNumericParameter("pitch", pitch);
      getNumericParameter("volume", volume);
    }

    virtual void encodeParameters() {
      clearParameters();
      putStringParameter("referenceInstrumentName", referenceInstrumentName);
      putNumericParameter("pitch", pitch);
      putNumericParameter("volume", volume);
    }

  };
  
  std::vector<Instrument*> *instruments = NULL;
  int pitchOffset = 24;
  
  DrumPadPanel *drumPadPanel = NULL;
  //const int maxNumPads = 16;
  int numPads = 8;
  std::vector<Pad> pads = std::vector<Pad>(maxNumPads);
  
  virtual ~DrumPad() {
    if(drumPadPanel) {
      GuiElement *parent = drumPadPanel->parentGuiElement;
      parent->deleteChildElement(drumPadPanel);
    }
  }

  DrumPad(std::vector<Instrument*> *instruments) {
    instrumentType = InstrumentType::CompositePads;
    this->instruments = instruments;
  }

  virtual void toggleGuiVisibility(GuiElement *parentGuiElement) {
    if(!drumPadPanel) {
      drumPadPanel = new DrumPadPanel(this, parentGuiElement);
    }
    else {
      drumPadPanel->toggleVisibility();
    }
  }
  
  void update() {
    for(int i=0; i<numPads; i++) {
      pads[i].instrumentIndex = 0;
      for(int k=0; k<instruments->size(); k++) {
        if(pads[i].referenceInstrumentName.compare(instruments->at(k)->name) == 0) {
          pads[i].instrumentIndex = k;
          break;
        }
      }
      //printf("pad %d. %s %d\n", i, pads[i].referenceInstrumentName.c_str(), pads[i].instrumentIndex);
    }
    if(drumPadPanel) {
      drumPadPanel->update();
    }
  }
  
  

  static std::string getClassName() {
    return "drumPad";
  }
  
  virtual std::string getBlockName() {
    return getClassName();
  }
  
  virtual void encodeParameters() {
    clearParameters();
    
    putStringParameter("name", name);
    putNumericParameter("volume", volume);
    putNumericParameter("numPads", numPads);
    
    for(int i=0; i<numPads; i++) {
      pads[i].encodeParameters();
    }
  }
  
  virtual void decodeParameters() {
    getStringParameter("name", name);
    getNumericParameter("volume", volume);
    getNumericParameter("numPads", numPads);
  }

  virtual void onPrintParameters(std::string &content, int level = 0) {
    for(int i=0; i<numPads; i++) {
      pads[i].printParameters(content, level);
    }
  }

  virtual void onParse() {
    numPads = 0;
  }

  virtual int onNewBlock(const std::string &blockName, int blockStartInd, int blockLevel) {
    if(blockName == Pad::getClassName()) {
      if(numPads < maxNumPads) {
        numPads++;
        return pads[numPads-1].parse(content, blockStartInd, blockLevel);
      }
    }
    return 0;
  }
  
  
  
  
};

