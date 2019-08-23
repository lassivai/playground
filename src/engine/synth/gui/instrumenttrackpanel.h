#pragma once
#include "../synth.h"

/* TODO
 *  - oprimize column widths
 */

/*  Small review for each instrument
 *  - current volume level, in case of drumpad: volumes for each drum
 *  - if no notes playing waveform of one cycle, otherwise current waveform of whole instrument notes added
 *  - number of live synthesized and pre-recorded notes currently playing
 *
 */

struct InstrumentPreviewPanel : public Panel {
  Synth *synth = NULL;
  int instrumentTrackIndex = 0;
  //Note note;
  
  InstrumentPreviewPanel(Synth *synth, int instrumentTrackIndex) : Panel("Instrument preview panel") {
    init(synth, instrumentTrackIndex);
    drawBorder = false;
    drawBackground = false;
    draggable = false;
  }
  
  void init(Synth *synth, int instrumentTrackIndex) {
    this->synth = synth;
    this->instrumentTrackIndex = instrumentTrackIndex;
    setSize(200, 23);
  }

  /*void initNote(Instrument *instrument, double sampleRate) {
    int instrumentIndex = synth->instrumentTracks[instrumentTrackIndex].instrumentIndex;
    if(instrumentIndex != note.instrumentIndex || !note.isInitialized) {
      note.set(sampleRate, freqToNote(100), 0, 1.0, instrumentIndex);
      note.isHolding = true;
      note.prepare(sampleRate);
      instrument->initializeNote(note);
    }
  }*/


  virtual void onRender(GeomRenderer &geomRenderer, TextGl &textRenderer) {
    if(!isVisible) return;
    
    int instrumentIndex = synth->instrumentTracks[instrumentTrackIndex].instrumentIndex;
    Instrument *instrument = synth->instruments[instrumentIndex];
    
    geomRenderer.texture = NULL;
    geomRenderer.strokeType = 1;
    geomRenderer.strokeWidth = 1;
    
    double x = 3, y = 4;
    double sampleRate = instrument->delayLine.sampleRate;
    Vec2d rms = instrument->delayLine.getRMS(0.05)*2.0;
    
    double minDbLebel = -90;
    double leftDb = rms.x == 0 ? 0 : clamp(amplitudeTodB(rms.x) - minDbLebel, 0, -minDbLebel) / (-minDbLebel);
    double rightDb = rms.y == 0 ? 0 : clamp(amplitudeTodB(rms.y) - minDbLebel, 0, -minDbLebel) / (-minDbLebel);
    double leftVol = clamp(amplitudeToVol(rms.x), 0, 1);
    double rightVol = clamp(amplitudeToVol(rms.y), 0, 1);
    
    double tooHighVol = amplitudeToVol(0.666);
    
    geomRenderer.fillColor.set(1, 1, 1, 0.2);
    geomRenderer.strokeColor.set(1, 1, 1, 0.2);
    geomRenderer.drawRectCorner(50, 7, absolutePos.x + x, absolutePos.y + 1);
    geomRenderer.drawRectCorner(50, 7, absolutePos.x + x, absolutePos.y + 1 + 7 + 2);

    /*geomRenderer.fillColor.set(1, 0, 0, 0.2);
    geomRenderer.strokeColor.set(1, 0, 0, 0.2);
    geomRenderer.drawRectCorner(50*(1-tooHighVol), 7, absolutePos.x + x + 50*tooHighVol, absolutePos.y + 1);
    geomRenderer.drawRectCorner(50*(1-tooHighVol), 7, absolutePos.x + x + 50*tooHighVol, absolutePos.y + 1 + 7 + 2);*/
    
    geomRenderer.strokeColor.set(1, 1, 1, 0.8);
    
    if(leftVol < tooHighVol) {
      geomRenderer.fillColor.set(1, 1, 1, 0.7);
      geomRenderer.strokeColor.set(1, 1, 1, 0.7);
    }
    else {
      geomRenderer.fillColor.set(1, 0, 0, 0.5);
      geomRenderer.strokeColor.set(1, 0, 0, 0.5);
    }
    geomRenderer.drawRectCorner(50*leftVol, 7, absolutePos.x + x, absolutePos.y + 1);
    
    if(rightVol < tooHighVol) {
      geomRenderer.fillColor.set(1, 1, 1, 0.7);
      geomRenderer.strokeColor.set(1, 1, 1, 0.7);
    }
    else {
      geomRenderer.fillColor.set(1, 0, 0, 0.5);
      geomRenderer.strokeColor.set(1, 0, 0, 0.5);
    }
    
    geomRenderer.drawRectCorner(50*rightVol, 7, absolutePos.x + x, absolutePos.y + 1 + 7 + 2);
    
    
    
    if(instrument->instrumentType == Instrument::InstrumentType::DefaultInstrument) {
      std::vector<Vec2d> waveForm(140);
      int numLiveNotes = 0, numRecordedNotes = 0;
      for(int i=0; i<synth->numCurrentlyPlayingNotes; i++) {
        if(synth->currentlyPlayingNotes[i].volume >= 0 && synth->currentlyPlayingNotes[i].isInitialized) {
          if(synth->currentlyPlayingNotes[i].instrumentIndex == instrumentIndex) {
            numLiveNotes++;
          }
        }
      }
      for(int i=0; i<synth->numCurrentlyPlayingNotesPresampled; i++) {
        if(synth->currentlyPlayingNotesPresampled[i].volume >= 0 && synth->currentlyPlayingNotesPresampled[i].isInitialized) {
          if(synth->currentlyPlayingNotesPresampled[i].instrumentIndex == instrumentIndex) {
            numRecordedNotes++;
          }
        }
      }
      
      
      for(int i=0; i<numLiveNotes + numRecordedNotes; i++) {
        int k = i % ((int)size.x / 4 - 1);
        int j = i / ((int)size.x / 4 - 1);
        if(i < numLiveNotes) {
          geomRenderer.fillColor.set(1, 1, 1, 0.8);
          geomRenderer.strokeColor.set(1, 1, 1, 0.8);
        }
        else {
          geomRenderer.fillColor.set(1, 1, 1, 0.44);
          geomRenderer.strokeColor.set(1, 1, 1, 0.44);
        }
        
        geomRenderer.drawRectCorner(2, 2, absolutePos.x + x + k * 4 + (i/5)*3, absolutePos.y + size.y - 3 - j * 4);
      }
      
      x += 57;

      if(instrument->isUpdating()) {
        textRenderer.setColor(1, 1, 1, 0.4);
        textRenderer.print("Updating...", absolutePos.x + x, absolutePos.y, 10);
      }
      else {
        double minValue = 1e10, maxValue = -1e10;
        if(leftDb <= 0) {
          Note note;
          instrument->preparePreviewNote(note);
          
          double dt = (double)1.0/waveForm.size()/note.frequency;
          for(int i=0; i<waveForm.size(); i++) {
            Vec2d sampleOut;
            instrument->getSample(sampleOut, Vec2d(), dt, 0.1+dt*i, note);
            //synth->inputDelayLine.getOutputSampleInSeconds((dt*i)/100.0).x
            waveForm[i] = sampleOut;
            minValue = min(waveForm[i].x, minValue);
            maxValue = max(waveForm[i].x, maxValue);
          }
        }
        else {
          instrument->delayLine.getWaveForm(waveForm, 100, 0.8, 1.0/50.0, 1);
          for(int i=0; i<waveForm.size(); i++) {
            minValue = min(waveForm[i].x, minValue);
            maxValue = max(waveForm[i].x, maxValue);
          }
        }
        if(maxValue > minValue) {
          for(int i=0; i<waveForm.size(); i++) {
            waveForm[i].x = map(waveForm[i].x, minValue, maxValue, -1, 1);
          }
        }
        geomRenderer.strokeColor.set(1, 1, 1, 0.7);
        for(int i=1; i<waveForm.size(); i++) {
          geomRenderer.drawLine(absolutePos.x + x + i-1, absolutePos.y + size.y*0.5-waveForm[i-1].x*(size.y*0.5-2),
                                absolutePos.x + x + i,   absolutePos.y + size.y*0.5-waveForm[i].x*(size.y*0.5-2));
        }
      }
    }
    else if(instrument->instrumentType == Instrument::InstrumentType::CompositePads) {
      DrumPad *drumPad = dynamic_cast<DrumPad*>(instrument);
      if(!drumPad) printf("Error at InstrumentSnapShotPanel, invalid drumpad %d\n", instrumentTrackIndex);
      int maxNumPads = 16;

      x += 57;
      double wd = (size.x - x) / max(drumPad->numPads, 16);
      
      geomRenderer.strokeColor.set(1, 1, 1, 0.9);
      textRenderer.setColor(0, 0, 0, 0.9);
      int numLiveNotes = 0, numRecordedNotes = 0;
      
      for(int i=0; i<drumPad->numPads; i++) {
        int drumIndex = drumPad->pads[i].instrumentIndex;
        Instrument *drum = synth->instruments[drumIndex];
        if(!drum) printf("Error at InstrumentSnapShotPanel, invalid drum %d\n", instrumentTrackIndex);
        
        
        for(int i=0; i<synth->numCurrentlyPlayingNotes; i++) {
          if(synth->currentlyPlayingNotes[i].volume >= 0 && synth->currentlyPlayingNotes[i].isInitialized) {
            if(synth->currentlyPlayingNotes[i].instrumentIndex == drumIndex) {
              numLiveNotes++;
            }
          }
        }
        for(int i=0; i<synth->numCurrentlyPlayingNotesPresampled; i++) {
          if(synth->currentlyPlayingNotesPresampled[i].volume >= 0 && synth->currentlyPlayingNotesPresampled[i].isInitialized) {
            if(synth->currentlyPlayingNotesPresampled[i].instrumentIndex == drumIndex) {
              numRecordedNotes++;
            }
          }
        }

        Vec2d rms = drum->delayLine.getRMS(0.05);
        double r = clamp(max(rms.x, rms.y), 0, 1);
        //double p = r == 0 ? 0 : clamp(amplitudeTodB(r) - minDbLebel, 0, -minDbLebel) / (-minDbLebel);
        double p = amplitudeToVol(r);
        //Vec2d dim = textRenderer.getDimensions(RomanNumeral::get(i+1), 10);
        //textRenderer.print(RomanNumeral::get(i+1), absolutePos.x + x + (i+0.5) * wd - dim.x*0.5, absolutePos.y + 3, 10);
        
        geomRenderer.fillColor.set(1, 1, 1, 0.2);
        geomRenderer.strokeColor.set(1, 1, 1, 0.2);
        geomRenderer.drawRectCorner(wd-4, (size.y-4), absolutePos.x + x+i*wd+2, 2 + absolutePos.y);
        
        if(r < 0.9) geomRenderer.fillColor.set(1, 1, 1, 0.7);
        else geomRenderer.fillColor.set(0.7, 0, 0, 0.8);
        
        
        geomRenderer.drawRectCorner(wd-4, (size.y-4)*p, absolutePos.x + x+i*wd+2, 2 + absolutePos.y +(size.y-4) * (1.0-p));
      }
      x -= 57;
      for(int i=0; i<numLiveNotes + numRecordedNotes; i++) {
        int k = i % ((int)size.x / 4 - 1);
        int j = i / ((int)size.x / 4 - 1);
        if(i < numLiveNotes) {
          geomRenderer.fillColor.set(1, 1, 1, 0.8);
          geomRenderer.strokeColor.set(1, 1, 1, 0.8);
        }
        else {
          geomRenderer.fillColor.set(1, 1, 1, 0.44);
          geomRenderer.strokeColor.set(1, 1, 1, 0.44);
        }
        
        geomRenderer.drawRectCorner(2, 2, absolutePos.x + x + k * 4 + (i/5)*3, absolutePos.y + size.y - 3 - j * 4);
      }
            
    }
  }
};

struct InstrumentTrackPanel : public Panel {
  
  NumberBox *numInstrumentTracksGui = NULL;
  CheckBox *showMidiDevicesGui = NULL;
  bool showMidiDevices = true;
  std::vector<CheckBox*> activeInstrumentTrackGuis = std::vector<CheckBox*>(Synth::maxNumInstrumentTracks, NULL);
  std::vector<Button*> activeInstrumentOpenGuiGuis = std::vector<Button*>(Synth::maxNumInstrumentTracks, NULL);
  std::vector<ListBox*> activeInstrumentNameGuis = std::vector<ListBox*>(Synth::maxNumInstrumentTracks, NULL);
  std::vector<NumberBox*> activeInstrumentVolumeGuis = std::vector<NumberBox*>(Synth::maxNumInstrumentTracks, NULL);
  std::vector<CheckBox*> activeInstrumentMuteGuis = std::vector<CheckBox*>(Synth::maxNumInstrumentTracks, NULL);
  std::vector<ListBox*> activeInstrumentMidiPortGuis = std::vector<ListBox*>(Synth::maxNumInstrumentTracks, NULL);
  std::vector<NumberBox*> activeInstrumentMidiInGuis = std::vector<NumberBox*>(Synth::maxNumInstrumentTracks, NULL);
  std::vector<NumberBox*> activeInstrumentMidiOutGuis = std::vector<NumberBox*>(Synth::maxNumInstrumentTracks, NULL);

  Label *instrumentNameLabel, *volumeLabel, *muteLabel, *midiPortLabel, *midiChannelInLabel, *midiChannelOutLabel;

  std::vector<InstrumentPreviewPanel*> instrumentPreviewPanels = std::vector<InstrumentPreviewPanel*>(Synth::maxNumInstrumentTracks, NULL);

  double line = 6, lineHeight = 23;
  int width = 80, widthA = 175+24;
  int columnA = 6, columnA2 = 6+widthA+9 - 20+30, columnB = 6+widthA+9+30, columnC = 6+widthA+width+2+4+30, columnD = 6+widthA+width*2-31+2+4+30,
      columnE = 6+widthA*2+width*(3)-42+4+30, columnF = 6+widthA*2+width*(3.4)-37+4+30, columnG = 6+widthA*2+width*(3.4)-37+width*0.4+2+4+30;
  
  Synth *synth = NULL;
  GuiElement *parentGuiElement = NULL;
  
  /*void onUpdate(double time, double dt) {
    for(int i=0; i<synth->maxNumInstrumentTracks; i++) {
      if(i >= synth->numInstrumentTracks && synth->instruments[synth->instrumentTracks[i].instrumentIndex]->getPanel()) {
        if(synth->instruments[synth->instrumentTracks[i].instrumentIndex]->getPanel()->isVisible) {
          synth->instruments[synth->instrumentTracks[i].instrumentIndex]->getPanel()->setVisible(false);
        }
      }
      if(synth->instruments[synth->instrumentTracks[i].instrumentIndex]->getPanel()) {
        if(!synth->instruments[synth->instrumentTracks[i].instrumentIndex]->getPanel()->isVisible && activeInstrumentOpenGuiGuis[i]->getValue()) {
          activeInstrumentOpenGuiGuis[i]->setValue(false);
        }
      }
    }
  }*/
  
  InstrumentTrackPanel(Synth *synth, MidiInterface *midiInterface, GuiElement *parentGuiElement) : Panel("Instrument track panel") {
    init(synth, midiInterface, parentGuiElement);
  }
  
  void init(Synth *synth, MidiInterface *midiInterface, GuiElement *parentGuiElement) {
    this->parentGuiElement = parentGuiElement;
    this->synth = synth;

    addGuiEventListener(new InstrumentTrackPanelListener(this));
    parentGuiElement->getRoot()->addGuiEventListener(new InstrumentNameListener(this));
    
    parentGuiElement->addChildElement(this);
    numInstrumentTracksGui = new NumberBox("Instruments", synth->numInstrumentTracks, NumberBox::INTEGER, 0, synth->maxNumInstrumentTracks, 10, line, 2);
    numInstrumentTracksGui->incrementMode = NumberBox::IncrementMode::Linear;
    addChildElement(numInstrumentTracksGui);
    numInstrumentTracksGui->setTextSize(10);
    numInstrumentTracksGui->labelColor.set(1, 1, 1, 0.9);

    addChildElement(showMidiDevicesGui = new CheckBox("Input devices", showMidiDevices, columnD, line, 10));
    showMidiDevicesGui->labelColor.set(1, 1, 1, 0.9);

    addChildElement(volumeLabel = new Label("Volume", columnB, line, 10, Vec4d(1, 1, 1, 0.9)));
    addChildElement(muteLabel = new Label("Mute", columnC, line, 10, Vec4d(1, 1, 1, 0.9)));
    //addChildElement(midiPortLabel = new Label("Midi port", columnD, line, 10, Vec4d(1, 1, 1, 0.9)));
    addChildElement(midiChannelInLabel = new Label("In", columnE, line, 10, Vec4d(1, 1, 1, 0.9)));
    addChildElement(midiChannelOutLabel = new Label("Out", columnF, line, 10, Vec4d(1, 1, 1, 0.9)));

    for(int i=0; i<synth->maxNumInstrumentTracks; i++) {
      line += lineHeight;

      activeInstrumentNameGuis[i] = new ListBox(std::to_string(i+1), columnA, line, 15);
      for(int k=0; k<synth->instruments.size(); k++) {
        activeInstrumentNameGuis[i]->addItems(synth->instruments[k]->name);
      }
      activeInstrumentNameGuis[i]->setValue(synth->instrumentTracks[i].instrumentIndex);
      addChildElement(activeInstrumentNameGuis[i]);

      addChildElement(activeInstrumentOpenGuiGuis[i] = new Button("Open Instrument GUI", "data/synth/textures/gui.png", columnA2, line+2, Button::ToggleButton));
      activeInstrumentOpenGuiGuis[i]->pressedOverlayColor.set(1, 1, 1, 0.22);

      activeInstrumentVolumeGuis[i] = new NumberBox("", synth->instrumentTracks[i].volume, NumberBox::FLOATING_POINT, 0, 1e6, columnB, line, 6);
      addChildElement(activeInstrumentVolumeGuis[i]);

      activeInstrumentMuteGuis[i] = new CheckBox("", synth->instrumentTracks[i].isMuted, columnC+6, line);
      addChildElement(activeInstrumentMuteGuis[i]);

      activeInstrumentMidiPortGuis[i] = new ListBox("", columnD, line, 21);
      for(int k=0; k<midiInterface->midiPorts.size(); k++) {
        activeInstrumentMidiPortGuis[i]->addItems(midiInterface->midiPorts[k]->name);
      }
      activeInstrumentMidiPortGuis[i]->setValue(synth->instrumentTracks[i].midiPortIndex);
      addChildElement(activeInstrumentMidiPortGuis[i]);

      activeInstrumentMidiInGuis[i] = new NumberBox("", synth->instrumentTracks[i].midiInChannel, NumberBox::INTEGER, 0, 16, columnE, line, 2);
      activeInstrumentMidiInGuis[i]->incrementMode = NumberBox::IncrementMode::Linear;
      addChildElement(activeInstrumentMidiInGuis[i]);

      activeInstrumentMidiOutGuis[i] = new NumberBox("", synth->instrumentTracks[i].midiOutChannel, NumberBox::INTEGER, 0, 16, columnF, line, 2);
      activeInstrumentMidiOutGuis[i]->incrementMode = NumberBox::IncrementMode::Linear;
      addChildElement(activeInstrumentMidiOutGuis[i]);
    }

    for(int i=0; i<synth->numInstrumentTracks; i++) {
      activeInstrumentNameGuis[i]->clearItems();
      for(int k=0; k<synth->instruments.size(); k++) {
        activeInstrumentNameGuis[i]->addItems(synth->instruments[k]->name);
      }
      activeInstrumentNameGuis[i]->setValue(synth->instrumentTracks[i].instrumentIndex);
    }
    synth->updateActiveInstruments();

    for(int i=0; i<synth->maxNumInstrumentTracks; i++) {
      addChildElement(instrumentPreviewPanels[i] = new InstrumentPreviewPanel(synth, i));
      instrumentPreviewPanels[i]->setPosition(columnG, activeInstrumentNameGuis[0]->pos.y + i * lineHeight);
    }

    update();
    setSize(instrumentPreviewPanels[0]->pos.x + instrumentPreviewPanels[0]->size.x+6, 6 + lineHeight * (synth->numInstrumentTracks+1) + 6);
    
    /*if(guiControlPanel) {
      setPosition(10, guiControlPanel->pos.y + guiControlPanel->size.y + 10);
    }*/
  }


  void update() {
    numInstrumentTracksGui->setValue(synth->numInstrumentTracks);
    
    for(int i=0; i<activeInstrumentNameGuis.size(); i++) {
      if(i < synth->numInstrumentTracks) {
        activeInstrumentNameGuis[i]->clearItems();
        for(int k=0; k<synth->instruments.size(); k++) {
          activeInstrumentNameGuis[i]->addItems(synth->instruments[k]->name);
        }
        activeInstrumentNameGuis[i]->setValue(synth->instrumentTracks[i].instrumentIndex);
        activeInstrumentVolumeGuis[i]->setValue(synth->instrumentTracks[i].volume);
        activeInstrumentOpenGuiGuis[i]->setValue(synth->instruments[synth->instrumentTracks[i].instrumentIndex]->getPanel() && synth->instruments[synth->instrumentTracks[i].instrumentIndex]->getPanel()->isVisible);
        activeInstrumentMuteGuis[i]->setValue(synth->instrumentTracks[i].isMuted);
        activeInstrumentMidiPortGuis[i]->setValue(synth->instrumentTracks[i].midiPortIndex);
        activeInstrumentMidiInGuis[i]->setValue(synth->instrumentTracks[i].midiInChannel);
        activeInstrumentMidiOutGuis[i]->setValue(synth->instrumentTracks[i].midiOutChannel);
        instrumentPreviewPanels[i]->setPosition(showMidiDevices ? columnG : columnD-9, instrumentPreviewPanels[i]->pos.y);
      }
      activeInstrumentNameGuis[i]->setVisible(i < synth->numInstrumentTracks);
      activeInstrumentVolumeGuis[i]->setVisible(i < synth->numInstrumentTracks);
      activeInstrumentOpenGuiGuis[i]->setVisible(i < synth->numInstrumentTracks);
      activeInstrumentMuteGuis[i]->setVisible(i < synth->numInstrumentTracks);
      activeInstrumentMidiPortGuis[i]->setVisible(i < synth->numInstrumentTracks && showMidiDevices);
      activeInstrumentMidiInGuis[i]->setVisible(i < synth->numInstrumentTracks && showMidiDevices);
      activeInstrumentMidiOutGuis[i]->setVisible(i < synth->numInstrumentTracks && showMidiDevices);
      instrumentPreviewPanels[i]->setVisible(i < synth->numInstrumentTracks);
    }
    
    //midiPortLabel->setVisible(showMidiDevices);
    midiChannelInLabel->setVisible(showMidiDevices);
    midiChannelOutLabel->setVisible(showMidiDevices);
    
    setSize(instrumentPreviewPanels[0]->pos.x + instrumentPreviewPanels[0]->size.x+6, 6 + lineHeight * (synth->numInstrumentTracks+1) + 6);
  }

  
  struct InstrumentNameListener : public GuiEventListener {
    InstrumentTrackPanel *instrumentTrackPanel = NULL;
    InstrumentNameListener(InstrumentTrackPanel *instrumentTrackPanel) : instrumentTrackPanel(instrumentTrackPanel) {}
    
    virtual void onMessage(GuiElement *guiElement,const std::string &message, void *userData) override {
      printf("InstrumentNameListener.onMessage(), '%s'\n", message.c_str());
      if(message == "instrumentNameChanged") {
        for(int i=0; i<instrumentTrackPanel->activeInstrumentNameGuis.size(); i++) {
          if(i < instrumentTrackPanel->synth->numInstrumentTracks) {
            instrumentTrackPanel->activeInstrumentNameGuis[i]->clearItems();
            for(int k=0; k<instrumentTrackPanel->synth->instruments.size(); k++) {
              instrumentTrackPanel->activeInstrumentNameGuis[i]->addItems(instrumentTrackPanel->synth->instruments[k]->name);
            }
          }
          instrumentTrackPanel->activeInstrumentNameGuis[i]->setValue(instrumentTrackPanel->synth->instrumentTracks[i].instrumentIndex);
        }
        //instrumentTrackPanel->update();
        /*Instrument *instrument = dynamic_cast<Instrument*>((Instrument*)userData);
        if(instrument) {
          instrumentTrackPanel->update();
          for(int i=0; i<instrumentTrackPanel->synth->numInstrumentTracks; i++) {
            if(instrumentTrackPanel->synth->instruments[instrumentTrackPanel->synth->instrumentTracks[i].instrumentIndex] == instrument) {
              instrumentTrackPanel->activeInstrumentNameGuis[i]->setValue(instrument->name);
              instrumentTrackPanel->update();
            }
          }
        }*/
      }
    }
  };
  
  
  
  
  struct InstrumentTrackPanelListener : public GuiEventListener {
    InstrumentTrackPanel *instrumentTrackPanel;
    InstrumentTrackPanelListener(InstrumentTrackPanel *instrumentTrackPanel) {
      this->instrumentTrackPanel = instrumentTrackPanel;
    }

    void onValueChange(GuiElement *guiElement) {
      if(guiElement == instrumentTrackPanel->numInstrumentTracksGui) {
        guiElement->getValue((void*)&instrumentTrackPanel->synth->numInstrumentTracks);
        instrumentTrackPanel->update();
      }
      if(guiElement == instrumentTrackPanel->showMidiDevicesGui) {
        guiElement->getValue((void*)&instrumentTrackPanel->showMidiDevices);
        instrumentTrackPanel->showMidiDevicesGui->overlayColor = instrumentTrackPanel->showMidiDevices ? Vec4d(1, 1, 1, 1) : Vec4d(1, 1, 1, 0.5);
        instrumentTrackPanel->update();
      }

      for(int i=0; i<instrumentTrackPanel->synth->numInstrumentTracks; i++) {
        if(guiElement == instrumentTrackPanel->activeInstrumentNameGuis[i]) {
          guiElement->getValue((void*)&instrumentTrackPanel->synth->instrumentTracks[i].instrumentIndex);
          instrumentTrackPanel->synth->activeInstrumentTrackIndex = i;
          instrumentTrackPanel->synth->updateActiveInstruments();
          instrumentTrackPanel->synth->resetRecordedTrackNotes(i);
        }
        if(guiElement == instrumentTrackPanel->activeInstrumentVolumeGuis[i]) {
          guiElement->getValue((void*)&instrumentTrackPanel->synth->instrumentTracks[i].volume);
        }
        
        if(guiElement == instrumentTrackPanel->activeInstrumentOpenGuiGuis[i]) {
          instrumentTrackPanel->synth->instruments[instrumentTrackPanel->synth->instrumentTracks[i].instrumentIndex]->toggleGuiVisibility(instrumentTrackPanel->parentGuiElement);
        }
        
        if(guiElement == instrumentTrackPanel->activeInstrumentMuteGuis[i]) {
          guiElement->getValue((void*)&instrumentTrackPanel->synth->instrumentTracks[i].isMuted);
        }
        if(guiElement == instrumentTrackPanel->activeInstrumentMidiPortGuis[i]) {
          guiElement->getValue((void*)&instrumentTrackPanel->synth->instrumentTracks[i].midiPortIndex);
        }
        if(guiElement == instrumentTrackPanel->activeInstrumentMidiInGuis[i]) {
          guiElement->getValue((void*)&instrumentTrackPanel->synth->instrumentTracks[i].midiInChannel);
        }
        if(guiElement == instrumentTrackPanel->activeInstrumentMidiOutGuis[i]) {
          guiElement->getValue((void*)&instrumentTrackPanel->synth->instrumentTracks[i].midiOutChannel);
        }
      }
    }

    /*void onKeyDown(GuiElement *guiElement, Events &events) {
      if(events.sdlKeyCode == SDLK_HOME) {
        for(int i=0; i<instrumentTrackPanel->synth->numInstrumentTracks; i++) {
          if(guiElement == instrumentTrackPanel->activeInstrumentNameGuis[i] ||
             guiElement == instrumentTrackPanel->activeInstrumentVolumeGuis[i] ||
             guiElement == instrumentTrackPanel->activeInstrumentMuteGuis[i] ||
             guiElement == instrumentTrackPanel->activeInstrumentMidiInGuis[i] ||
             guiElement == instrumentTrackPanel->activeInstrumentMidiOutGuis[i]) {
            instrumentTrackPanel->synth->instruments[instrumentTrackPanel->synth->instrumentTracks[i].instrumentIndex]->toggleGuiVisibility(instrumentTrackPanel->parentGuiElement);
          }
        }
      }
    }*/
  };
  
};
