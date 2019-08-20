#pragma once
#include "../synth.h"





struct SynthPanel : public Panel {
  NumberBox *masterVolumeGui = NULL ;
  CheckBox *isStereoGui = NULL;
  CheckBox *playAudioInputGui = NULL;
  NumberBox *inputVolumeGui = NULL ;
  NumberBox *maxNumNotesGui = NULL;
  NumberBox *maxNumNotesPresampledGui = NULL;
  ListBox *sequencerModeGui = NULL;

  Synth *synth = NULL;
  GuiElement *parentGuiElement = NULL;

  SynthPanel(Synth *synth, GuiElement *parentGuiElement) : Panel("Synth panel") {
    init(synth, parentGuiElement);
  }

  void init(Synth *synth, GuiElement *parentGuiElement) {
    this->synth = synth;
    this->parentGuiElement = parentGuiElement;

    this->addGuiEventListener(new SynthPanelListener(this));
    parentGuiElement->addChildElement(this);

    double line = 10, lineHeight = 23;

    masterVolumeGui = new NumberBox("Master volume", synth->masterVolume, NumberBox::FLOATING_POINT, 0.0, 1e10, 10, line, 8);
    maxNumNotesGui = new NumberBox("Max notes", synth->numCurrentlyPlayingNotes, NumberBox::INTEGER, 1, synth->maxCurrentlyPlayingNotes, 10, line+=lineHeight, 4);
    maxNumNotesGui->incrementMode = NumberBox::IncrementMode::Linear;

    maxNumNotesPresampledGui = new NumberBox("Max notes presampled", synth->numCurrentlyPlayingNotesPresampled, NumberBox::INTEGER, 1, synth->maxCurrentlyPlayingNotesPresampled, 10+150, line, 4);
    maxNumNotesPresampledGui->incrementMode = NumberBox::IncrementMode::Linear;

    //isStereoGui = new CheckBox("Stereo", synth->isStereo, 10+340+40, line);

    inputVolumeGui = new NumberBox("Input volume", synth->inputVolume, NumberBox::FLOATING_POINT, 0.0, 1e10, 10, line+=lineHeight, 8);
    playAudioInputGui = new CheckBox("Play input", synth->playAudioInput, 10+230, line);

    sequencerModeGui = new ListBox("Sequencer mode", 10, line+=lineHeight);
    sequencerModeGui->addItems("None", "Loop tracks", "Song");
    sequencerModeGui->setValue(synth->mode);

    this->addChildElement(masterVolumeGui);
    this->addChildElement(playAudioInputGui);
    this->addChildElement(inputVolumeGui);
    this->addChildElement(maxNumNotesGui);
    this->addChildElement(maxNumNotesPresampledGui);
    this->addChildElement(sequencerModeGui);

    this->setSize(410, line + lineHeight + 10);
  }

  struct SynthPanelListener : public GuiEventListener {
    SynthPanel *synthPanel;
    SynthPanelListener(SynthPanel *synthPanel) {
      this->synthPanel = synthPanel;
    }
    void onValueChange(GuiElement *guiElement) {
      if(guiElement == synthPanel->masterVolumeGui) {
        guiElement->getValue((void*)&synthPanel->synth->masterVolume);
      }
      if(guiElement == synthPanel->maxNumNotesGui) {
        guiElement->getValue((void*)&synthPanel->synth->requestedNumberOfNotes);
        synthPanel->synth->synthThreadDisableRequested = true;
      }
      if(guiElement == synthPanel->maxNumNotesPresampledGui) {
        guiElement->getValue((void*)&synthPanel->synth->requestedNumberOfNotesPresampled);
        synthPanel->synth->synthThreadDisableRequested = true;
      }
      if(guiElement == synthPanel->isStereoGui) {
        guiElement->getValue((void*)&synthPanel->synth->isStereo);
      }
      if(guiElement == synthPanel->playAudioInputGui) {
        guiElement->getValue((void*)&synthPanel->synth->playAudioInput);
      }
      if(guiElement == synthPanel->sequencerModeGui) {
        guiElement->getValue((void*)&synthPanel->synth->mode);
      }
      if(guiElement == synthPanel->inputVolumeGui) {
        guiElement->getValue((void*)&synthPanel->synth->inputVolume);
      }

    }

  };
};

