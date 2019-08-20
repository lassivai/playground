#pragma once

#include <string>
#include <vector>

#include "../gui/gui.h"
#include "synth.h"



struct Scale {
  std::string name;
  std::vector<int> notes;
};

struct EarTrainer : public PanelInterface
{
  Scale minor = {"Minor", {0, 2, 3, 5, 7, 8, 10 }};
  Scale minorHarmonic = {"Minor harmonic", {0, 2, 3, 5, 7, 8, 11}};
  Scale major = {"Major", {0, 2, 4, 5, 7, 9, 11}};
  Scale minorPentatonic = {"Minor pentatonic", {0, 3, 5, 7, 10 }};
  Scale majorPentatonic = {"Major pentatonic", {0, 2, 4, 7, 9 }};
  Scale chromatic = {"Chromatic", {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}};
  std::vector<Scale> scales = { minor, minorHarmonic, major, minorPentatonic, majorPentatonic, chromatic };

  Synth *synth;
  int minPitch = 60 - 12, maxPitch = 60 + 12 + 7;
  int currentPitch = 60;
  int keyBaseNote = 0;
  int scale = 0;

  double noteDuration = 0.333;
  double noteVolume = 0.75;

  bool isActive = false;

  int correctAnswers = 0, totalAnswers = 0;
  int correctAnswersIgnoreOctave = 0, totalAnswersIgnoreOctave = 0;
  bool correctAnswerForIgnoreOctave = false;

  Panel *panel = NULL;
  NumberBox *minPitchGui = NULL, *maxPitchGui = NULL;
  ListBox *keyBaseNoteGui = NULL, *scaleGui = NULL;
  CheckBox *isActiveGui = NULL;
  Label *scoreGui = NULL, *scoreIgnoreOctaveGui = NULL;
  NumberBox *noteDurationGui = NULL;
  NumberBox *noteVolumeGui = NULL;


  void init(Synth *synth) {
    this->synth = synth;
  }

  void resetScore() {
    correctAnswers = 0;
    totalAnswers = 0;
    correctAnswersIgnoreOctave = 0;
    totalAnswersIgnoreOctave = 0;
  }

  void randomizePitch() {
    while(true) {
      currentPitch = Random::getInt(minPitch, maxPitch);
      int relativePitch = (currentPitch - keyBaseNote) % 12;
      for(int i=0; i<scales[scale].notes.size(); i++) {
        if(relativePitch == scales[scale].notes[i]) {
          return;
        }
      }
    }
  }

  void guessPitch(int pitch) {
    totalAnswers++;
    if(!correctAnswerForIgnoreOctave) {
      totalAnswersIgnoreOctave++;
    }

    if(pitch % 12 == currentPitch % 12 && !correctAnswerForIgnoreOctave) {
      correctAnswerForIgnoreOctave = true;
      correctAnswersIgnoreOctave++;
    }
    if(pitch == currentPitch) {
      randomizePitch();
      playCurrentPitch();
      correctAnswers++;
      correctAnswerForIgnoreOctave = false;
    }

    setScoreGuiText();
  }

  void playCurrentPitch() {
    synth->playNote(currentPitch, 0.75, noteDuration, 0);
    //printf("(debugging) at EarTrainer::playCurrentPitch()...\n");
  }

  void onKeyDown(Events &events) {
    if(!isActive) return;
    if(events.sdlKeyCode == SDLK_PAGEUP) {
      playCurrentPitch();
    }
    if(events.sdlKeyCode == SDLK_PAGEDOWN) {
      randomizePitch();
      playCurrentPitch();
    }
  }

  void setScoreGuiText() {
    if(!scoreGui) return;
    char buf[128];
    std::sprintf(buf, "Score %d/%d = %.2f %% absolute", correctAnswers, totalAnswers, totalAnswers == 0 ? 0 : ((double)correctAnswers/totalAnswers*100.0));
    scoreGui->setText(buf);

    std::sprintf(buf, "Score %d/%d = %.2f %% ignore octave", correctAnswersIgnoreOctave, totalAnswersIgnoreOctave, totalAnswersIgnoreOctave == 0 ? 0 : ((double)correctAnswersIgnoreOctave/totalAnswersIgnoreOctave*100.0));
    scoreIgnoreOctaveGui->setText(buf);
  }

  Panel *getPanel() {
    return panel;
  }

  Panel *addPanel(GuiElement *parentElement, const std::string &title = "") {
    if(panel) return panel;
    panel = new Panel(50, 50, 50, 50);
    panel->addGuiEventListener(new EarTrainerPanelListener(this));
    double line = 10, lineHeight = 23;
    Label *label = new Label("Ear trainer", 10, line);
    isActiveGui = new CheckBox("Active", isActive, 10, line+=lineHeight);
    noteDurationGui = new NumberBox("Note duration", noteDuration, NumberBox::FLOATING_POINT, 0, 10, 10, line+=lineHeight);
    noteVolumeGui = new NumberBox("Note volume", noteVolume, NumberBox::FLOATING_POINT, 0, 10, 10, line+=lineHeight);
    minPitchGui = new NumberBox("Min pitch", minPitch, NumberBox::INTEGER, 0, 128, 10, line+=lineHeight);
    maxPitchGui = new NumberBox("Max pitch", maxPitch, NumberBox::INTEGER, 0, 128, 10, line+=lineHeight);
    minPitchGui->incrementMode = NumberBox::IncrementMode::Linear;
    maxPitchGui->incrementMode = NumberBox::IncrementMode::Linear;
    keyBaseNoteGui = new ListBox("Key base note", 10, line += lineHeight);
    keyBaseNoteGui->addItems("C", "C#/Db", "D", "C#/Eb", "E", "F", "F#/Gb", "G", "G#/Ab", "A", "A#/Bb", "B");
    keyBaseNoteGui->setValue(keyBaseNote);


    scaleGui = new ListBox("Scale", 10, line += lineHeight);
    for(int i=0; i<scales.size(); i++) {
      scaleGui->addItems(scales[i].name);
    }
    scaleGui->setValue(scale);

    scoreGui = new Label("", 10, line+=lineHeight);
    scoreIgnoreOctaveGui = new Label("", 10, line+=lineHeight);
    resetScore();
    setScoreGuiText();

    panel->addChildElement(label);
    panel->addChildElement(isActiveGui);
    panel->addChildElement(minPitchGui);
    panel->addChildElement(maxPitchGui);
    panel->addChildElement(keyBaseNoteGui);
    panel->addChildElement(scaleGui);
    panel->addChildElement(scoreGui);
    panel->addChildElement(scoreIgnoreOctaveGui);
    panel->addChildElement(noteDurationGui);
    panel->addChildElement(noteVolumeGui);
    panel->size.set(320, line + lineHeight + 10);

    if(parentElement) {
      parentElement->addChildElement(panel);
    }

    return panel;
  }

  void removePanel(GuiElement *parentElement) {
    if(parentElement) parentElement->deleteChildElement(panel);
    else {
      panel->finalize();
      delete panel;
    }
    panel = NULL;
  }

  void updatePanel() {

  }

  struct EarTrainerPanelListener : public GuiEventListener {
    EarTrainer *earTrainer;
    EarTrainerPanelListener(EarTrainer *earTrainer) {
      this->earTrainer = earTrainer;
    }
    void onValueChange(GuiElement *guiElement) {
      if(guiElement == earTrainer->isActiveGui) {
        guiElement->getValue(&earTrainer->isActive);
      }
      if(guiElement == earTrainer->noteDurationGui) {
        guiElement->getValue(&earTrainer->noteDuration);
      }
      if(guiElement == earTrainer->noteVolumeGui) {
        guiElement->getValue(&earTrainer->noteVolume);
      }
      if(guiElement == earTrainer->minPitchGui) {
        guiElement->getValue(&earTrainer->minPitch);
      }
      if(guiElement == earTrainer->maxPitchGui) {
        guiElement->getValue(&earTrainer->maxPitch);
      }
      if(guiElement == earTrainer->keyBaseNoteGui) {
        guiElement->getValue((void*)&earTrainer->keyBaseNote);
      }
      if(guiElement == earTrainer->scaleGui) {
        guiElement->getValue((void*)&earTrainer->scale);
      }
    }
  };
};
