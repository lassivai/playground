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
  
  int scaleChangeNoteCounter = 0;
  bool isScale1 = true;

  enum Mode { SingleScale, TwoScales, CircleOfFifths };
  std::vector<std::string> modeNames = { "Single scale", "Two scales", "Circle of fifths" };
  Mode mode = SingleScale;
  
  int twoScalesNumNotes1 = 10, twoScalesNumNotes2 = 10;
  int keyBaseNote2 = 0;
  int scale2 = 0;
  
  int circleOfFifthsNumNotes = 20;
  
  enum ScaleChangeAid { Nothing, BaseNote, BaseChord, Cadence };
  std::vector<std::string> scaleChangeAidNames = { "Nothing", "Base note", "Base chord", "Cadence" };
  ScaleChangeAid scaleChangeAid = Nothing;
  
  double noteDuration = 0.333;
  double noteVolume = 0.75;

  bool isActive = false;

  int correctAnswers = 0, totalAnswers = 0;
  int correctAnswersIgnoreOctave = 0, totalAnswersIgnoreOctave = 0;
  bool correctAnswerForIgnoreOctave = false;

  bool playScaleChangeAidRequested = false;


  void init(Synth *synth) {
    this->synth = synth;
  }

  void resetScore() {
    correctAnswers = 0;
    totalAnswers = 0;
    correctAnswersIgnoreOctave = 0;
    totalAnswersIgnoreOctave = 0;
    scaleChangeNoteCounter = 0;
    isScale1 = true;
  }

  void randomizePitch() {
    int scale = mode == TwoScales && !isScale1 ? this->scale2 : this->scale;
    int keyBaseNote = mode == TwoScales && !isScale1 ? this->keyBaseNote2 : this->keyBaseNote;
    
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
      correctAnswers++;
      correctAnswerForIgnoreOctave = false;
      scaleChangeNoteCounter++;
      if(mode == TwoScales) {
        if((isScale1 && scaleChangeNoteCounter >= twoScalesNumNotes1) || (!isScale1 && scaleChangeNoteCounter >= twoScalesNumNotes2)) {
          scaleChangeNoteCounter = 0;
          isScale1 = !isScale1;
          playScaleChangeAidRequested = true;
        }
      }
      if(mode == CircleOfFifths) {
        if(scaleChangeNoteCounter >= circleOfFifthsNumNotes) {
          scaleChangeNoteCounter = 0;
          keyBaseNote = (keyBaseNote + 5) % 12;
          playScaleChangeAidRequested = true;
          if(keyBaseNoteGui) {
            keyBaseNoteGui->setValue(keyBaseNote);
          }
        }
      }
      randomizePitch();
      playCurrentPitch();
    }
    setScoreGuiText();
  }

  void playCurrentPitch() {
    double relTime = 0;
    if(playScaleChangeAidRequested) {
      relTime = playScaleChangeAid();
      playScaleChangeAidRequested = false;
    }
    synth->playNote(currentPitch, noteVolume, noteDuration, relTime);
  }
  
  double playScaleChangeAid() {
    int scale = mode == TwoScales && !isScale1 ? this->scale2 : this->scale;
    int keyBaseNote = mode == TwoScales && !isScale1 ? this->keyBaseNote2 : this->keyBaseNote;
    bool isMajor = scale == 2 || scale == 4;
    int third = isMajor ? 4 : 3;
    int sixth = isMajor ? 9 : 8;
    
    if(scaleChangeAid == BaseNote) {
      synth->playNote(60 + keyBaseNote, noteVolume, noteDuration*0.5, 0);
      synth->playNote(60 + keyBaseNote, noteVolume, noteDuration, noteDuration);
      return noteDuration * 4;
    }
    if(scaleChangeAid == BaseChord) {
      synth->playNote(60 + keyBaseNote, noteVolume, noteDuration, 0);
      synth->playNote(60 + keyBaseNote + third, noteVolume, noteDuration, 0);
      synth->playNote(60 + keyBaseNote + 7, noteVolume, noteDuration, 0);
      synth->playNote(60 + keyBaseNote + 12, noteVolume, noteDuration, 0);
      return noteDuration * 5;
    }
    if(scaleChangeAid == Cadence) {
      synth->playNote(60 + keyBaseNote, noteVolume, noteDuration, 0);
      synth->playNote(60 + keyBaseNote + third, noteVolume, noteDuration, 0);
      synth->playNote(60 + keyBaseNote + 7, noteVolume, noteDuration, 0);
      synth->playNote(60 + keyBaseNote + 12, noteVolume, noteDuration, 0);
      
      synth->playNote(60 + keyBaseNote, noteVolume, noteDuration, noteDuration*2);
      synth->playNote(60 + keyBaseNote + 5, noteVolume, noteDuration, noteDuration*2);
      synth->playNote(60 + keyBaseNote + sixth, noteVolume, noteDuration, noteDuration*2);
      synth->playNote(60 + keyBaseNote + 12, noteVolume, noteDuration, noteDuration*2);

      synth->playNote(60 + keyBaseNote + 2, noteVolume, noteDuration, noteDuration*4);
      synth->playNote(60 + keyBaseNote + 5, noteVolume, noteDuration, noteDuration*4);
      synth->playNote(60 + keyBaseNote + 7, noteVolume, noteDuration, noteDuration*4);
      synth->playNote(60 + keyBaseNote + 11, noteVolume, noteDuration, noteDuration*4);

      synth->playNote(60 + keyBaseNote, noteVolume, noteDuration, noteDuration*6);
      synth->playNote(60 + keyBaseNote + third, noteVolume, noteDuration, noteDuration*6);
      synth->playNote(60 + keyBaseNote + 7, noteVolume, noteDuration, noteDuration*6);
      synth->playNote(60 + keyBaseNote + 12, noteVolume, noteDuration, noteDuration*6);
      return noteDuration * (8 + 4);
    }
    return 0;
  }

  void onKeyDown(Events &events) {
    if(!isActive) return;
    if(events.sdlKeyCode == SDLK_INSERT) {
      playScaleChangeAid();
    }
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


  Panel *panel = NULL;
  NumberBox *minPitchGui = NULL, *maxPitchGui = NULL;
  ListBox *keyBaseNoteGui = NULL, *scaleGui = NULL;
  CheckBox *isActiveGui = NULL;
  Label *scoreGui = NULL, *scoreIgnoreOctaveGui = NULL;
  NumberBox *noteDurationGui = NULL;
  NumberBox *noteVolumeGui = NULL;

  ListBox *modeGui = NULL;
  ListBox *scaleChangeAidGui = NULL;
  
  ListBox *keyBaseNote2Gui = NULL, *scale2Gui = NULL;
  NumberBox *twoScalesNumNotes1Gui = NULL;
  NumberBox *twoScalesNumNotes2Gui = NULL;

  NumberBox *circleOfFifthsNumNotesGui = NULL;
  
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
    
    panel->addChildElement(modeGui = new ListBox("Mode", 10, line += lineHeight));
    modeGui->setItems(modeNames);
    modeGui->setValue(mode);

    panel->addChildElement(scaleChangeAidGui = new ListBox("Scale change aid", 10, line += lineHeight, 10));
    scaleChangeAidGui->setItems(scaleChangeAidNames);
    scaleChangeAidGui->setValue(scaleChangeAid);

    keyBaseNoteGui = new ListBox("Key base note", 10, line += lineHeight, 7);
    keyBaseNoteGui->addItems("C", "C#/Db", "D", "C#/Eb", "E", "F", "F#/Gb", "G", "G#/Ab", "A", "A#/Bb", "B");
    keyBaseNoteGui->setValue(keyBaseNote);
    
    scaleGui = new ListBox("Scale", 10, line += lineHeight);
    for(int i=0; i<scales.size(); i++) {
      scaleGui->addItems(scales[i].name);
    }
    scaleGui->setValue(scale);

    panel->addChildElement(keyBaseNote2Gui = new ListBox("Key 2 base note", 10, line += lineHeight, 7));
    keyBaseNote2Gui->addItems("C", "C#/Db", "D", "C#/Eb", "E", "F", "F#/Gb", "G", "G#/Ab", "A", "A#/Bb", "B");
    keyBaseNote2Gui->setValue(keyBaseNote2);
    
    panel->addChildElement(scale2Gui = new ListBox("Scale 2", 10, line += lineHeight));
    for(int i=0; i<scales.size(); i++) {
      scale2Gui->addItems(scales[i].name);
    }
    scale2Gui->setValue(scale2);

    panel->addChildElement(twoScalesNumNotes1Gui = new NumberBox("Two scales notes 1", twoScalesNumNotes1, NumberBox::INTEGER, 1, 1<<30, 10, line+=lineHeight, 6));
    twoScalesNumNotes1Gui->incrementMode = NumberBox::IncrementMode::Linear;
    panel->addChildElement(twoScalesNumNotes2Gui = new NumberBox("Two scales notes 2", twoScalesNumNotes2, NumberBox::INTEGER, 1, 1<<30, 10, line+=lineHeight, 6));
    twoScalesNumNotes2Gui->incrementMode = NumberBox::IncrementMode::Linear;

    panel->addChildElement(circleOfFifthsNumNotesGui = new NumberBox("Circle of fifths notes", circleOfFifthsNumNotes, NumberBox::INTEGER, 1, 1<<30, 10, line+=lineHeight, 6));
    circleOfFifthsNumNotesGui->incrementMode = NumberBox::IncrementMode::Linear;


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
      
      if(guiElement == earTrainer->modeGui) {
        guiElement->getValue(&earTrainer->mode);
      }
      if(guiElement == earTrainer->scaleChangeAidGui) {
        guiElement->getValue(&earTrainer->scaleChangeAid);
      }
      if(guiElement == earTrainer->keyBaseNote2Gui) {
        guiElement->getValue(&earTrainer->keyBaseNote2);
      }
      if(guiElement == earTrainer->scale2Gui) {
        guiElement->getValue(&earTrainer->scale2);
      }
      if(guiElement == earTrainer->twoScalesNumNotes1Gui) {
        guiElement->getValue(&earTrainer->twoScalesNumNotes1);
      }
      if(guiElement == earTrainer->twoScalesNumNotes2Gui) {
        guiElement->getValue(&earTrainer->twoScalesNumNotes2);
      }
      if(guiElement == earTrainer->circleOfFifthsNumNotesGui) {
        guiElement->getValue(&earTrainer->circleOfFifthsNumNotes);
      }
      
      
    }
  };
  
  
  struct EarTrainerMidiListener : public MidiMessageListener {
    EarTrainer *earTrainer;
    EarTrainerMidiListener(EarTrainer *earTrainer) {
      this->earTrainer = earTrainer;
    }
    virtual void onMessage(const MidiMessage &midiMessage, MidiPortType midiPortType) {
      if(earTrainer->isActive) {
        printf("EarTrainerMidiListener::onMessage(), beginning\n");
      
        midiMessage.print();

        if(midiMessage.getMessageType() == MidiMessage::NoteOn) {
          if(midiMessage.dataByte2 > 0) {
            int pitch = midiMessage.dataByte1;
            earTrainer->guessPitch(pitch);
          }
        }
        printf("EarTrainerMidiListener::onMessage(), end\n");
      }
    }
    
  };
  EarTrainerMidiListener *earTrainerMidiListener = NULL;
  
  MidiMessageListener *getMidiMessageListener() {
    return earTrainerMidiListener ? earTrainerMidiListener : earTrainerMidiListener = new EarTrainerMidiListener(this);
  }
};
