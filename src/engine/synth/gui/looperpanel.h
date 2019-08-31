#pragma once
#include "../synth.h"


struct LooperPanel : public Panel {
  CheckBox *editLooperTrackGui = NULL, *isLooperPlayingGui = NULL;
  NumberBox *loopDurationGui = NULL;
  NumberBox *tempoGui = NULL;
  NumberBox *currentLoopGui = NULL;
  NumberBox *numLooperTracksGui = NULL;
  NumberBox *currentLoopRepeatGui = NULL;
  NumberBox *numLooperRepeatsGui = NULL;
  //NumberBox *maxNumSequenceLooperTrackNotesGui = NULL;
  NumberBox *measuresPerLoopGui = NULL;
  NumberBox *beatsPerMeasureGui = NULL;
  NumberBox *subdivisionsPerMeasureGui = NULL;
  NumberBox *noteValueInverseGui = NULL;
  CheckBox *autoStepForwardGui = NULL;
  CheckBox *recordNoteSamplesGui = NULL;

  NumberBox *defaultNoteVolumeGui = NULL;
  CheckBox *applyDefaultVolumeToKeyboardsGui = NULL;
  
  Synth *synth = NULL;
  
  LooperPanel(Synth *synth, GuiElement *parentGuiElement) : Panel("Loop panel") {
    init(synth, parentGuiElement);
  }

  void init(Synth *synth, GuiElement *parentGuiElement) {
    this->synth = synth;

    double line = 10, lineHeight = 23;
    isLooperPlayingGui = new CheckBox("Playing", synth->isLooperPlaying, 10, line);
    editLooperTrackGui = new CheckBox("Recording", synth->isRecordingMode, 10, line+=lineHeight);

    /*currentLoopGui = new NumberBox("Current loop", synth->sequenceLooper.currentTrack+1, NumberBox::INTEGER, 0, 1<<31, 10, line += lineHeight);
    currentLoopGui->setInputEnabled(false);
    numLooperTracksGui = new NumberBox("Loop count", synth->sequenceLooper.numTracks, NumberBox::INTEGER, 1, synth->sequenceLooper.maxNumTracks, 10, line += lineHeight);*/
    //loopProgressionGui = new CheckBox("Progress loops", 10, line += lineHeight);

    /*currentLoopRepeatGui = new NumberBox("Current repeat", synth->sequenceLooper.currentRepeat+1, NumberBox::INTEGER, 0, 1<<31, 10, line += lineHeight);
    currentLoopRepeatGui->setInputEnabled(false);
    numLooperRepeatsGui = new NumberBox("Repeat count", synth->sequenceLooper.numRepeats, NumberBox::INTEGER, 0, 3600, 10, line += lineHeight);*/
    measuresPerLoopGui = new NumberBox("Measures", synth->measuresPerLooperTrack, NumberBox::INTEGER, 1, 1000000, 10, line+=lineHeight, 11);
    measuresPerLoopGui->incrementMode = NumberBox::IncrementMode::Linear;

    beatsPerMeasureGui = new NumberBox("Beats per measure", synth->beatsPerMeasure, NumberBox::INTEGER, 1, 1000000, 10, line+=lineHeight, 11);
    beatsPerMeasureGui->incrementMode = NumberBox::IncrementMode::Linear;

    tempoGui = new NumberBox("Tempo", synth->tempo, NumberBox::FLOATING_POINT, 1.0, 100000, 10, line += lineHeight);
    tempoGui->incrementMode = NumberBox::IncrementMode::LinearAboveOne;

    loopDurationGui = new NumberBox("Duration", synth->looperTrackDuration, NumberBox::FLOATING_POINT, 0.01, 100000, 10, line += lineHeight, 11);
    loopDurationGui->incrementMode = NumberBox::IncrementMode::Linear;

    subdivisionsPerMeasureGui = new NumberBox("Subdivisions", synth->subdivisionsPerMeasure, NumberBox::INTEGER, 1, 10000000, 10, line+=lineHeight, 9);
    subdivisionsPerMeasureGui->incrementMode = NumberBox::IncrementMode::Linear;

    noteValueInverseGui = new NumberBox("Note length fraction", synth->noteValueInverse, NumberBox::FLOATING_POINT, pow(2.0, -6), pow(2.0, 10), 10, line+=lineHeight, 9);
    noteValueInverseGui->incrementMode = NumberBox::IncrementMode::Linear;

    autoStepForwardGui = new CheckBox("Auto step forward", synth->progressOnNoteStartWhenPaused, 10, line+=lineHeight);

    /*maxNumSequenceLooperTrackNotesGui = new NumberBox("Max num notes", synth->sequenceLooper.maxNumTrackNotes, NumberBox::INTEGER, 1, 1<<30, 10, line+=lineHeight, 9);
    maxNumSequenceLooperTrackNotesGui->incrementMode = NumberBox::IncrementMode::LinearAboveOne;*/

    defaultNoteVolumeGui = new NumberBox("Note volume", synth->defaultNoteVolume, NumberBox::FLOATING_POINT, 0, 1.0, 10, line += lineHeight, 6);
    applyDefaultVolumeToKeyboardsGui = new CheckBox("Forced", synth->applyDefaultVolumeTokeyboards, 10+200, line);

    recordNoteSamplesGui = new CheckBox("Buffered track notes", synth->recordTrackNotes, 10, line+=lineHeight);

    this->addChildElement(isLooperPlayingGui);
    this->addChildElement(editLooperTrackGui);
    /*this->addChildElement(currentLoopGui);
    this->addChildElement(numLooperTracksGui);
    this->addChildElement(currentLoopRepeatGui);
    this->addChildElement(numLooperRepeatsGui);*/
    this->addChildElement(loopDurationGui);
    this->addChildElement(tempoGui);
    //this->addChildElement(maxNumSequenceLooperTrackNotesGui);
    this->addChildElement(measuresPerLoopGui);
    this->addChildElement(beatsPerMeasureGui);
    this->addChildElement(subdivisionsPerMeasureGui);
    this->addChildElement(noteValueInverseGui);
    this->addChildElement(autoStepForwardGui);
    this->addChildElement(defaultNoteVolumeGui);
    this->addChildElement(applyDefaultVolumeToKeyboardsGui);
    this->addChildElement(recordNoteSamplesGui);

    this->addGuiEventListener(new LooperPanelListener(this));

    this->setSize(320, line + lineHeight + 10);

    parentGuiElement->addChildElement(this);
  }
  
  void onUpdate() {
    //currentLoopGui->setValue(synth->sequenceLooper.currentTrack+1);
    //currentLoopRepeatGui->setValue(synth->sequenceLooper.currentRepeat+1);
    isLooperPlayingGui->setValue(synth->isLooperPlaying);
    editLooperTrackGui->setValue(synth->isRecordingMode);
  }


  struct LooperPanelListener : public GuiEventListener {
    LooperPanel *looperPanel;
    LooperPanelListener(LooperPanel *looperPanel) {
      this->looperPanel = looperPanel;
    }
    void onValueChange(GuiElement *guiElement) {
      if(guiElement == looperPanel->editLooperTrackGui) {
        guiElement->getValue((void*)&looperPanel->synth->isRecordingMode);
      }
      if(guiElement == looperPanel->autoStepForwardGui) {
        guiElement->getValue((void*)&looperPanel->synth->progressOnNoteStartWhenPaused);
      }
      if(guiElement == looperPanel->measuresPerLoopGui) {
        guiElement->getValue((void*)&looperPanel->synth->measuresPerLooperTrack);
        looperPanel->synth->setLoopDuration(60.0 * looperPanel->synth->measuresPerLooperTrack * looperPanel->synth->beatsPerMeasure / looperPanel->synth->tempo);
        looperPanel->loopDurationGui->setValue(looperPanel->synth->looperTrackDuration);
        looperPanel->synth->updateNoteLengths();
        looperPanel->synth->updateNoteSequencerRects();
      }
      if(guiElement == looperPanel->beatsPerMeasureGui) {
        guiElement->getValue((void*)&looperPanel->synth->beatsPerMeasure);
        looperPanel->synth->setLoopDuration(60.0 * looperPanel->synth->measuresPerLooperTrack * looperPanel->synth->beatsPerMeasure / looperPanel->synth->tempo);
        looperPanel->loopDurationGui->setValue(looperPanel->synth->looperTrackDuration);
        looperPanel->synth->updateNoteLengths();
        looperPanel->synth->updateNoteSequencerRects();
      }
      if(guiElement == looperPanel->subdivisionsPerMeasureGui) {
        guiElement->getValue((void*)&looperPanel->synth->subdivisionsPerMeasure);
        //synthSketch->synth->updateNoteSequencerRects();
      }
      if(guiElement == looperPanel->noteValueInverseGui) {
        guiElement->getValue((void*)&looperPanel->synth->noteValueInverse);
      }
      if(guiElement == looperPanel->isLooperPlayingGui) {
        bool value;
        guiElement->getValue((void*)&value);
        looperPanel->synth->setLooperTracksPlaying(value);
      }
      /*if(guiElement == looperPanel->maxNumSequenceLooperTrackNotesGui) {
        int value;
        guiElement->getValue((void*)&value);
        looperPanel->synth->sequenceLooper.setMaxNumTrackNotes(value);
      }*/
      if(guiElement == looperPanel->currentLoopGui) {
        guiElement->getValue((void*)&looperPanel->synth->sequenceLooper.currentTrack);
      }
      if(guiElement == looperPanel->numLooperTracksGui) {
        guiElement->getValue((void*)&looperPanel->synth->sequenceLooper.numTracks);
        if(looperPanel->synth->sequenceLooper.currentTrack >= looperPanel->synth->sequenceLooper.numTracks) {
          looperPanel->synth->sequenceLooper.currentTrack = looperPanel->synth->sequenceLooper.numTracks-1;
          looperPanel->synth->sequenceLooper.currentRepeat = 0;
        }
      }
      if(guiElement == looperPanel->currentLoopRepeatGui) {
        guiElement->getValue((void*)&looperPanel->synth->sequenceLooper.currentRepeat);
      }
      if(guiElement == looperPanel->numLooperRepeatsGui) {
        guiElement->getValue((void*)&looperPanel->synth->sequenceLooper.numRepeats);
        looperPanel->synth->sequenceLooper.currentRepeat = 0;
      }
      if(guiElement == looperPanel->loopDurationGui) {
        double value;
        guiElement->getValue((void*)&value);
        looperPanel->synth->setLoopDuration(value);
        //looperPanel->synth->updateNoteLengths();
        looperPanel->tempoGui->setValue(looperPanel->synth->tempo);
      }
      if(guiElement == looperPanel->tempoGui) {
        double value;
        guiElement->getValue((void*)&value);
        looperPanel->synth->setTempo(value);
        //looperPanel->synth->updateNoteLengths();
        looperPanel->loopDurationGui->setValue(looperPanel->synth->looperTrackDuration);
      }
      if(guiElement == looperPanel->defaultNoteVolumeGui) {
        looperPanel->getValue((void*)&looperPanel->synth->defaultNoteVolume);
      }
      if(guiElement == looperPanel->applyDefaultVolumeToKeyboardsGui) {
        looperPanel->getValue((void*)&looperPanel->synth->applyDefaultVolumeTokeyboards);
      }
      if(guiElement == looperPanel->recordNoteSamplesGui) {
        guiElement->getValue((void*)&looperPanel->synth->recordTrackNotes);
        looperPanel->synth->resetRecordedTrackNotes();
      }
    }
  };
};