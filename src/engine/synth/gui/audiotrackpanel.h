#pragma once
#include "../synth.h"





struct AudioRecordingTrackPanel : public Panel {


  
  NumberBox *numAudioRecordingTracksGui = NULL;
  std::vector<TextBox*> nameGui;
  std::vector<NumberBox*> volumeGui;
  std::vector<CheckBox*> muteGui;
  std::vector<ListBox*> recordingModeGui;
  std::vector<CheckBox*> recordGui;
  std::vector<Label*> recordingLengthGui;
  //std::vector<TrackGraphPanel*> trackGraphPanels;
  
  double line = 10, lineHeight = 23;
  int width = 80, widthA = 225;
  int columnA = 10, columnB = 10+widthA, columnC = 10+widthA+width, columnD = 10+widthA+width*2-25, columnE = 10+widthA+width*3-5;
  int columnF = 10+widthA+width*4-30;
  
  Synth *synth = NULL;
  GuiElement *parentGuiElement = NULL;
  
  AudioRecordingTrackPanel(Synth *synth, GuiElement *parentGuiElement) : Panel("Instrument track panel") {
    init(synth, parentGuiElement);
  }
  
  void init(Synth *synth, GuiElement *parentGuiElement) {
    this->parentGuiElement = parentGuiElement;
    this->synth = synth;

    this->addGuiEventListener(new AudioRecordingTrackPanelListener(this));
    parentGuiElement->addChildElement(this);
    numAudioRecordingTracksGui = new NumberBox("Audio tracks", synth->numAudioRecordingTracks, NumberBox::INTEGER, 0, synth->maxAudioRecordingTracks, 10, line, 4);
    numAudioRecordingTracksGui->incrementMode = NumberBox::IncrementMode::Linear;
    addChildElement(numAudioRecordingTracksGui);
    
    addChildElement(new Label("Track", columnA, line+=lineHeight));
    addChildElement(new Label("Volume", columnB, line));
    addChildElement(new Label("Mute", columnC, line));
    addChildElement(new Label("Rec mode", columnD, line));
    addChildElement(new Label("Record", columnE, line));
    addChildElement(new Label("Length", columnF, line));
    
    nameGui.resize(synth->maxAudioRecordingTracks);
    volumeGui.resize(synth->maxAudioRecordingTracks);
    muteGui.resize(synth->maxAudioRecordingTracks);
    recordingModeGui.resize(synth->maxAudioRecordingTracks);
    recordGui.resize(synth->maxAudioRecordingTracks);
    recordingLengthGui.resize(synth->maxAudioRecordingTracks);
    
    //trackGraphPanels.resize(synth->maxAudioRecordingTracks);
    
    for(int i=0; i<synth->maxAudioRecordingTracks; i++) {
      line += lineHeight;
      
      nameGui[i] = new TextBox(std::to_string(i+1) + ". ", synth->audioRecordingTracks[i].name, columnA, line);
      addChildElement(nameGui[i]);

      volumeGui[i] = new NumberBox("", synth->audioRecordingTracks[i].volume, NumberBox::FLOATING_POINT, 0, 1e6, columnB, line, 6);
      addChildElement(volumeGui[i]);

      muteGui[i] = new CheckBox("", synth->audioRecordingTracks[i].isMuted, columnC, line);
      addChildElement(muteGui[i]);

      recordingModeGui[i] = new ListBox("", columnD, line, 6);
      recordingModeGui[i]->setItems(synth->audioRecordingTracks[i].recordModeNames);
      recordingModeGui[i]->setValue(synth->audioRecordingTracks[i].recordMode);
      addChildElement(recordingModeGui[i]);

      recordGui[i] = new CheckBox("", synth->audioRecordingTracks[i].isRecording, columnE, line);
      addChildElement(recordGui[i]);
      
      recordingLengthGui[i] = new Label("0.00 s", columnF, line);
      addChildElement(recordingLengthGui[i]);
      
      /*trackGraphPanels[i] = new TrackGraphPanel(&synth->audioRecordingTracks[i]);
      trackGraphPanels[i]->setVisible(false);
      addChildElement(trackGraphPanels[i]);*/
    }

    update();
    //setSize(columnE + width, 10 + lineHeight * (synth->numAudioRecordingTracks+2) + 10);
    
    /*if(guiControlPanel) {
      setPosition(10, guiControlPanel->pos.y + guiControlPanel->size.y + 10);
    }*/
  }
  
  void onUpdate(double time, double dt) {
    for(int i=0; i<synth->numAudioRecordingTracks; i++) {
      char buf[128];
      std::sprintf(buf, "%.2f s", synth->audioRecordingTracks[i].recordingLength);
      recordingLengthGui[i]->setText(buf);
      /*if(trackGraphPanels[i]) {
        trackGraphPanels[i]->update(dt);
      }*/
    }
  }

  void update() {
    numAudioRecordingTracksGui->setValue(synth->numAudioRecordingTracks);
    
    for(int i=0; i<nameGui.size(); i++) {
      if(i < synth->numAudioRecordingTracks) {
        nameGui[i]->setValue(synth->audioRecordingTracks[i].name);
        volumeGui[i]->setValue(synth->audioRecordingTracks[i].volume);
        muteGui[i]->setValue(synth->audioRecordingTracks[i].isMuted);
        recordingModeGui[i]->setValue(synth->audioRecordingTracks[i].recordMode);
        recordGui[i]->setValue(synth->audioRecordingTracks[i].isRecording);
        
        char buf[128];
        std::sprintf(buf, "%.2f s", synth->audioRecordingTracks[i].recordingLength);

        recordingLengthGui[i]->setText(buf);
      }
      nameGui[i]->setVisible(i < synth->numAudioRecordingTracks);
      volumeGui[i]->setVisible(i < synth->numAudioRecordingTracks);
      muteGui[i]->setVisible(i < synth->numAudioRecordingTracks);
      recordingModeGui[i]->setVisible(i < synth->numAudioRecordingTracks);
      recordGui[i]->setVisible(i < synth->numAudioRecordingTracks);
      recordingLengthGui[i]->setVisible(i < synth->numAudioRecordingTracks);
    }
    
    setSize(columnF + width, 10 + lineHeight * (synth->numAudioRecordingTracks+2) + 10);
  }

  struct AudioRecordingTrackPanelListener : public GuiEventListener {
    AudioRecordingTrackPanel *audioRecordingTrackPanel;
    AudioRecordingTrackPanelListener(AudioRecordingTrackPanel *audioRecordingTrackPanel) {
      this->audioRecordingTrackPanel = audioRecordingTrackPanel;
    }

    void onValueChange(GuiElement *guiElement) {
      if(guiElement == audioRecordingTrackPanel->numAudioRecordingTracksGui) {
        guiElement->getValue((void*)&audioRecordingTrackPanel->synth->numAudioRecordingTracks);
        audioRecordingTrackPanel->update();
      }

      for(int i=0; i<audioRecordingTrackPanel->synth->numAudioRecordingTracks; i++) {
        if(guiElement == audioRecordingTrackPanel->nameGui[i]) {
          guiElement->getValue((void*)&audioRecordingTrackPanel->synth->audioRecordingTracks[i].name);
        }
        if(guiElement == audioRecordingTrackPanel->volumeGui[i]) {
          guiElement->getValue((void*)&audioRecordingTrackPanel->synth->audioRecordingTracks[i].volume);
        }
        if(guiElement == audioRecordingTrackPanel->muteGui[i]) {
          
          guiElement->getValue((void*)&audioRecordingTrackPanel->synth->audioRecordingTracks[i].isMuted);

        }
        if(guiElement == audioRecordingTrackPanel->recordingModeGui[i]) {
          guiElement->getValue((void*)&audioRecordingTrackPanel->synth->audioRecordingTracks[i].recordMode);
        }
        if(guiElement == audioRecordingTrackPanel->recordGui[i]) {
          bool value = false;
          guiElement->getValue((void*)&value);
          audioRecordingTrackPanel->synth->audioRecordingTracks[i].setRecording(value);
        }
      }
    }

    void onKeyDown(GuiElement *guiElement, Events &events) {
      if(events.sdlKeyCode == SDLK_HOME) {
        for(int i=0; i<audioRecordingTrackPanel->synth->numAudioRecordingTracks; i++) {
          if(guiElement == audioRecordingTrackPanel->nameGui[i] ||
             guiElement == audioRecordingTrackPanel->volumeGui[i] ||
             guiElement == audioRecordingTrackPanel->muteGui[i] ||
             guiElement == audioRecordingTrackPanel->recordingModeGui[i] ||
             guiElement == audioRecordingTrackPanel->recordGui[i]) {
            //audioRecordingTrackPanel->synth->instruments[audioRecordingTrackPanel->synth->audioRecordingTracks[i].name]->toggleGuiVisibility(audioRecordingTrackPanel->parentGuiElement);
            //audioRecordingTrackPanel->trackGraphPanels[i]->toggleVisibility();
            if(audioRecordingTrackPanel->hasChildElement(audioRecordingTrackPanel->synth->audioRecordingTracks[i].getPanel())) {
              audioRecordingTrackPanel->synth->audioRecordingTracks[i].getPanel()->toggleVisibility();
            }
            else {
              audioRecordingTrackPanel->addChildElement(audioRecordingTrackPanel->synth->audioRecordingTracks[i].getPanel());
            }
          }
        }
      }
    }
  };
  
};