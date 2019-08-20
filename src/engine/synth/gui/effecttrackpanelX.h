#pragma once
#include "../synth.h"


struct EffectTrackPanel : public Panel {
  
  const std::vector<std::string> effectNames = {"Biquad filter", "Equalizer", "Multidelay", "Reverb"};
  ListBox *effectsGui = NULL;
  
  std::vector<Panel*> effectPanels;
  
  Vec2d trackItemSize = Vec2d(320, 80);
  double trackItemStartY;
  
  std::vector<PostProcessingEffect*> *postProcessingEffects = NULL;
  
  GuiElement *parentGuiElement = NULL;
  
  DelayLine *delayLine
  
  EffectTrackPanel(std::vector<PostProcessingEffect*> *postProcessingEffects, DelayLine *delayLine, GuiElement *parentGuiElement) : Panel("Effect track panel") {
    init(postProcessingEffects, double sampleRate, parentGuiElement);
  }
  
  void init(std::vector<PostProcessingEffect*> *postProcessingEffects, DelayLine *delayLine, GuiElement *parentGuiElement) {
    this->parentGuiElement = parentGuiElement;
    this->delayLine = delayLine;
    this->postProcessingEffects = postProcessingEffects;

    double line = -13, lineHeight = 23;

    this->addGuiEventListener(new EffectTrackPanelListener(this));
    parentGuiElement->addChildElement(this);
    
    addChildElement(effectsGui = new ListBox("Add effect", 10, line+=lineHeight));
    effectsGui->setItems(effectNames);
    
    trackItemStartY = line+lineHeight;
    
    for(int i=0; i<postProcessingEffects->size(); i++) {
      Panel *panel = new Panel(trackItemSize.x, trackItemSize.y, 0, trackItemStartY+trackItemSize.y*i);
      postProcessingEffects->at(i)->initializeEffectTrackPanel(panel);
      postProcessingEffectPanels.push_back(panel);
    }
    //update();
    setSize(trackItemSize.x, trackItemStartY + trackItemSize.y * postProcessingEffects->size());
  }
  
  void onUpdate(double time, double dt) {
    /*for(int i=0; i<synth->numAudioRecordingTracks; i++) {
      char buf[128];
      std::sprintf(buf, "%.2f s", synth->audioRecordingTracks[i].recordingLength);
      recordingLengthGui[i]->setText(buf);
      if(trackGraphPanels[i]) {
        trackGraphPanels[i]->update(dt);
      }
    }*/
  }

  void update() {
    
    //setSize(trackItemSize.x, trackItemStartY + trackItemSize.y * postProcessingEffects->size());
  }

  struct EffectTrackPanelListener : public GuiEventListener {
    EffectTrackPanel *effectTrackPanel;
    EffectTrackPanelListener(EffectTrackPanel *effectTrackPanel) {
      this->effectTrackPanel = effectTrackPanel;
    }
    void onMousePressed(GuiElement *guiElement, Events &events) {
      if(guiElement == effectTrackPanel->effectsGui && guiElement->isPointWithin(events.m)) {
        int index = effectTrackPanel->effects->activeItemIndex;
        if(index == 0) {
          effectTrackPanel->postProcessingEffects->push_back(new Biquad(effectTrackPanel->delayLine.sampleRate));
        }
        else if(index == 1) {
          effectTrackPanel->postProcessingEffects->push_back(new Equalizer(effectTrackPanel->sampleRate));
        }
        else if(index == 2) {
          effectTrackPanel->postProcessingEffects->push_back(new Multidelay(effectTrackPanel->delayLine));
        }
        else if(index == 3) {
          effectTrackPanel->postProcessingEffects->push_back(new Reverb(effectTrackPanel->delayLine.sampleRate));
        }
        Panel *panel = new Panel(effectTrackPanel->trackItemSize.x, effectTrackPanel->trackItemSize.y, 0, effectTrackPanel->trackItemStartY+effectTrackPanel->trackItemSize.y*i);
        postProcessingEffects->at(i)->initializeEffectTrackPanel(panel);
        postProcessingEffectPanels.push_back(panel);

      }
    }
    
/*
    void onValueChange(GuiElement *guiElement) {
      if(guiElement == effectTrackPanel->numAudioRecordingTracksGui) {
        guiElement->getValue((void*)&effectTrackPanel->synth->numAudioRecordingTracks);
        effectTrackPanel->update();
      }

      for(int i=0; i<effectTrackPanel->synth->numAudioRecordingTracks; i++) {
        if(guiElement == effectTrackPanel->nameGui[i]) {
          guiElement->getValue((void*)&effectTrackPanel->synth->audioRecordingTracks[i].name);
        }
        if(guiElement == effectTrackPanel->volumeGui[i]) {
          guiElement->getValue((void*)&effectTrackPanel->synth->audioRecordingTracks[i].volume);
        }
        if(guiElement == effectTrackPanel->muteGui[i]) {
          
          guiElement->getValue((void*)&effectTrackPanel->synth->audioRecordingTracks[i].isMuted);

        }
        if(guiElement == effectTrackPanel->recordingModeGui[i]) {
          guiElement->getValue((void*)&effectTrackPanel->synth->audioRecordingTracks[i].recordMode);
        }
        if(guiElement == effectTrackPanel->recordGui[i]) {
          bool value = false;
          guiElement->getValue((void*)&value);
          effectTrackPanel->synth->audioRecordingTracks[i].setRecording(value);
        }
      }
    }

    void onKeyDown(GuiElement *guiElement, Events &events) {
      if(events.sdlKeyCode == SDLK_HOME) {
        for(int i=0; i<effectTrackPanel->synth->numAudioRecordingTracks; i++) {
          if(guiElement == effectTrackPanel->nameGui[i] ||
             guiElement == effectTrackPanel->volumeGui[i] ||
             guiElement == effectTrackPanel->muteGui[i] ||
             guiElement == effectTrackPanel->recordingModeGui[i] ||
             guiElement == effectTrackPanel->recordGui[i]) {
            //effectTrackPanel->synth->instruments[effectTrackPanel->synth->audioRecordingTracks[i].name]->toggleGuiVisibility(effectTrackPanel->parentGuiElement);
            effectTrackPanel->trackGraphPanels[i]->toggleVisibility();
          }
        }
      }
    }*/
  };
  
};