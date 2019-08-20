#pragma once

#include <string>
#include <vector>

#include "../gui/gui.h"
#include "synth.h"



struct Scale {
  std::string name;
  std::vector<int> notes;
};

struct EarTrainerPoly : public PanelInterface
{
  Scale minor = {"Minor", {0, 2, 3, 5, 7, 8, 10 }};
  Scale minorHarmonic = {"Minor harmonic", {0, 2, 3, 5, 7, 8, 11}};
  Scale major = {"Major", {0, 2, 4, 5, 7, 9, 11}};
  Scale minorPentatonic = {"Minor pentatonic", {0, 3, 5, 7, 10 }};
  Scale majorPentatonic = {"Major pentatonic", {0, 2, 4, 7, 9 }};
  Scale chromatic = {"Chromatic", {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}};
  std::vector<Scale> scales = { minor, minorHarmonic, major, minorPentatonic, majorPentatonic, chromatic };

  Synth *synth = NULL;
  int minPitch = 60 - 12, maxPitch = 60 + 12 + 7;
  int keyBaseNote = 0;
  int scale = 0;

  std::vector<std::string> polyphonicModeNames = {"Melodic random", "Melodic random within octave", "Melodic random scale run", "Melodic up/down scale run",
                       "Melodic chord up", "Melodic chord down", "Melodic chord up or down", "Melodic interval",
                       "Simultaneous random", "Simultaneous interval", "Simultaneous basic chord", "Simultaneous inverted chord"};

  enum PolyphonicMode {MelodicRandom, MelodicRandomWithinOctave, MelodicRandomScaleRun, MelodicUpOrDownScaleRun,
                       MelodicChordUp, MelodicChordDown, MelodicChordUpOrDown, MelodicInterval,
                       SimultaneousRandom, SimultaneousInterval, SimultaneousBasicChord, SimultaneousInvertedChord};

  bool isSimultaneousMode() {
    return polyphonicMode >= PolyphonicMode::SimultaneousRandom;
  }

  PolyphonicMode polyphonicMode = PolyphonicMode::MelodicRandom;

  int interval = 3;
  int numberOfChordThirds = 3;
  bool chordNotesNextToEachOther = true;
  int numberOfNotes = 1;
  std::vector<int> currentPitches = {60};
  int currentPitchesGuessCounter = 0;

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

  ListBox *polyphonicModeGui = NULL;
  NumberBox *intervalGui = NULL;
  NumberBox *numberOfChordThirdsGui = NULL;
  CheckBox *chordNotesNextToEachOtherGui = NULL;
  NumberBox *numberOfNotesGui = NULL;


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

    if(pitch % 12 == currentPitches[currentPitchesGuessCounter] % 12 && !correctAnswerForIgnoreOctave) {
      correctAnswerForIgnoreOctave = true;
      correctAnswersIgnoreOctave++;
    }
    if(pitch == currentPitches[currentPitchesGuessCounter]) {
      currentPitchesGuessCounter++;
      correctAnswers++;
      correctAnswerForIgnoreOctave = false;
      if(currentPitchesGuessCounter == numberOfNotes) {
        currentPitchesGuessCounter = 0;
        randomizePitch();
        playCurrentPitch();
      }
    }
    else if(numberOfNotes > 1) {
      playCurrentPitch(true);
    }

    setScoreGuiText();
  }

  void playCurrentPitch(bool emphasizeCurrentPolyNote = false) {
    for(int i=0; i<currentPitches.size(); i++) {
      double t = isSimultaneousMode() ? 0 : noteDuration * i;
      double vol = noteVolume;
      if(emphasizeCurrentPolyNote) {
        vol = i == currentPitchesGuessCounter ? noteVolume * 1.25 : noteVolume * 0.5;
      }
      synth->playNote(currentPitches[i], vol, noteDuration, t);
    }
  }

  void onKeyDown(Events &events) {
    if(!isActive) return;
    if(events.sdlKeyCode == SDLK_HOME) {
      playCurrentPitch();
    }
    if(events.sdlKeyCode == SDLK_END) {
      randomizePitch();
      playCurrentPitch();
    }
  }

  void setScoreGuiText() {
    if(!scoreGui) return;
    char buf[128];
    std::sprintf(buf, "Score %d/%d = %.2f %% absolute", correctAnswers, totalAnswers, totalAnswers == 0 ? 0 : ((double)correctAnswers/totalAnswers*100.0));
    scoreGui->setText(buf);

    /*std::sprintf(buf, "Score %d/%d = %.2f %% ignore octave", correctAnswersIgnoreOctave, totalAnswersIgnoreOctave, totalAnswersIgnoreOctave == 0 ? 0 : ((double)correctAnswersIgnoreOctave/totalAnswersIgnoreOctave*100.0));
    scoreIgnoreOctaveGui->setText(buf);*/
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

    numberOfNotesGui = new NumberBox("Number of notes", numberOfNotes, NumberBox::INTEGER, 1, 128, 10, line+=lineHeight);
    numberOfNotesGui->incrementMode = NumberBox::IncrementMode::Linear;

    polyphonicModeGui = new ListBox("Poly mode", 10, line += lineHeight);
    polyphonicModeGui->setItems(polyphonicModeNames);
    polyphonicModeGui->setValue(polyphonicMode);

    intervalGui = new NumberBox("Interval (semitones)", interval, NumberBox::INTEGER, 2, 64, 10, line+=lineHeight);
    intervalGui->incrementMode = NumberBox::IncrementMode::Linear;
    numberOfChordThirdsGui = new NumberBox("Thirds in chords", numberOfChordThirds, NumberBox::INTEGER, 2, 10, 10, line+=lineHeight);
    numberOfChordThirdsGui->incrementMode = NumberBox::IncrementMode::Linear;
    chordNotesNextToEachOtherGui = new CheckBox("Chord notes connected", chordNotesNextToEachOther, 10, line+=lineHeight);


    scoreGui = new Label("", 10, line+=lineHeight);
    //scoreIgnoreOctaveGui = new Label("", 10, line+=lineHeight);
    resetScore();
    setScoreGuiText();

    panel->addChildElement(label);
    panel->addChildElement(isActiveGui);
    panel->addChildElement(minPitchGui);
    panel->addChildElement(maxPitchGui);
    panel->addChildElement(keyBaseNoteGui);
    panel->addChildElement(scaleGui);
    panel->addChildElement(scoreGui);
    //panel->addChildElement(scoreIgnoreOctaveGui);
    panel->addChildElement(noteDurationGui);
    panel->addChildElement(noteVolumeGui);

    panel->addChildElement(numberOfNotesGui);
    panel->addChildElement(polyphonicModeGui);
    panel->addChildElement(intervalGui);
    panel->addChildElement(numberOfChordThirdsGui);
    panel->addChildElement(chordNotesNextToEachOtherGui);


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

    minPitchGui = NULL;
    maxPitchGui = NULL;
    keyBaseNoteGui = NULL;
    scaleGui = NULL;
    isActiveGui = NULL;
    scoreGui = NULL;
    scoreIgnoreOctaveGui = NULL;
    noteDurationGui = NULL;
    noteVolumeGui = NULL;

    polyphonicModeGui = NULL;
    intervalGui = NULL;
    numberOfChordThirdsGui = NULL;
    chordNotesNextToEachOtherGui = NULL;
    numberOfNotesGui = NULL;
  }

  void updatePanel() {

  }

  struct EarTrainerPanelListener : public GuiEventListener {
    EarTrainerPoly *earTrainer;
    EarTrainerPanelListener(EarTrainerPoly *earTrainer) {
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

      if(guiElement == earTrainer->polyphonicModeGui) {
        guiElement->getValue(&earTrainer->polyphonicMode);
      }
      if(guiElement == earTrainer->intervalGui) {
        guiElement->getValue(&earTrainer->interval);
      }
      if(guiElement == earTrainer->numberOfChordThirdsGui) {
        guiElement->getValue(&earTrainer->numberOfChordThirds);
      }
      if(guiElement == earTrainer->chordNotesNextToEachOtherGui) {
        guiElement->getValue(&earTrainer->chordNotesNextToEachOther);
      }
      if(guiElement == earTrainer->numberOfNotesGui) {
        guiElement->getValue(&earTrainer->numberOfNotes);
      }
    }
  };
};
