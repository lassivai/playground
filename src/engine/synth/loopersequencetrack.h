#pragma once
#include "note.h"
#include <vector>

struct LooperSequenceTrack
{
  int trackNumber;
  std::vector<SequencerNote> sequencerNotes;

  double sampleRate = 1;

  LooperSequenceTrack(int trackNumber, double sampleRate) {
    this->trackNumber = trackNumber;
    this->sampleRate = sampleRate;

    reset();
  }

  LooperSequenceTrack(LooperSequenceTrack *looperSequenceTrack) {
    this->trackNumber = looperSequenceTrack->trackNumber;
    this->sequencerNotes = std::vector<SequencerNote>(looperSequenceTrack->sequencerNotes);
    this->sampleRate = looperSequenceTrack->sampleRate;
    //memcpy(this->sequencerNotes, looperSequenceTrack->sequencerNotes, sizeof(Note)*numNotes);
  }

  SequencerNote *startNote(double pitch, double lengthInSecs, double startTime, double volume, int instrumentTrackIndex) {
    sequencerNotes.push_back(SequencerNote(pitch, lengthInSecs, startTime, volume, instrumentTrackIndex));
    return &sequencerNotes[sequencerNotes.size()-1];
  }
  /*Note *startNote(double pitch, double startTime, double volume, int instrumentIndex, int instrumentTrackIndex) {
    sequencerNotes.push_back(Note(sampleRate, pitch, startTime, volume, instrumentIndex, instrumentTrackIndex));
    return &sequencerNotes[sequencerNotes.size()-1];
  }*/

  /*SequencerNote *startNote(Note note, double startTime = -1) {
    printf("Starting note at looper track 1...\n");
    sequencerNotes.push_back(note);
    printf("Starting note at looper track 2...\n");
    if(startTime >= 0) {
      sequencerNotes[sequencerNotes.size()-1].startTime = startTime;
    }
    printf("Starting note at looper track 3...\n");
    return &sequencerNotes[sequencerNotes.size()-1];
  }*/

  // FIXME
  double cancelNote() {
    double startTime = 0;
    if(sequencerNotes.size() > 0) {
      startTime = sequencerNotes[sequencerNotes.size()-1].startTime;
      sequencerNotes[sequencerNotes.size()-1].reset();
      sequencerNotes.erase(sequencerNotes.begin()+sequencerNotes.size()-1);
    }
    return startTime;
  }
  void print() {
    for(int i=0; i<sequencerNotes.size(); i++) {
      printf("note %d: %f\n", i, sequencerNotes[i].volume);
    }
  }
  void resetNote(int index) {
    if(index >= 0 && index < sequencerNotes.size()) {
      sequencerNotes.erase(sequencerNotes.begin()+index);
    }
  }
  void reset() {
    for(int i=0; i<sequencerNotes.size(); i++) {
      sequencerNotes[i].reset();
    }
    sequencerNotes.clear();
  }

  void reset(int instrumentTrackIndex) {
    for(int i=0; i<sequencerNotes.size(); i++) {
      if(sequencerNotes[i].instrumentTrackIndex == instrumentTrackIndex) {
        sequencerNotes[i].reset();
        sequencerNotes.erase(sequencerNotes.begin()+i);
      }
    }
  }

  
  void transposeTrack(double steps, int instrumentTrackIndex = -1) {
    for(int i=0; i<sequencerNotes.size(); i++) {
      if(instrumentTrackIndex == -1 || instrumentTrackIndex == sequencerNotes[i].instrumentTrackIndex) {
        sequencerNotes[i].transpose(steps);
      }
    }
  }
};
/*struct LooperSequenceTrack
{
  int trackNumber;
  int numNotes;
  std::vector<Note> notes;
  int counter = 0;

  double sampleRate = 1;

  LooperSequenceTrack(int numNotes, int trackNumber, double sampleRate) {
    this->numNotes = numNotes;
    this->trackNumber = trackNumber;
    this->sampleRate = sampleRate;
    if(numNotes > 0) {
      notes.resize(numNotes);
    }
    reset();
  }

  LooperSequenceTrack(LooperSequenceTrack *looperSequenceTrack) {
    this->trackNumber = looperSequenceTrack->trackNumber;
    this->numNotes = looperSequenceTrack->numNotes;
    this->counter = looperSequenceTrack->counter;
    this->notes = std::vector<Note>(looperSequenceTrack->notes);
    this->sampleRate = looperSequenceTrack->sampleRate;
    //memcpy(this->notes, looperSequenceTrack->notes, sizeof(Note)*numNotes);
  }

  void setNumNotes(int numNotes) {
    this->numNotes = numNotes;
    if(numNotes > notes.size()) {
      notes.resize(numNotes);
    }
  }

  Note *startNote(double pitch, double startTime, double volume, int instrumentIndex, int instrumentTrackIndex) {
    if(numNotes > 0) {
      counter = (counter+1) % numNotes;
      notes[counter].set(sampleRate, pitch, startTime, volume, instrumentIndex);
    }
    else {
      if(counter < notes.size()-1) {
        counter += 1;
        notes[counter] = Note(sampleRate, pitch, startTime, volume, instrumentIndex, instrumentTrackIndex);
      }
      else {
        notes.push_back(Note(sampleRate, pitch, startTime, volume, instrumentIndex, instrumentTrackIndex));
        counter = notes.size() - 1;
      }
    }
    //notes[counter].print();
    return &notes[counter];
  }

  Note *startNote(const Note &note, double startTime = -1) {
    if(numNotes > 0) {
      counter = (counter+1) % numNotes;
      notes[counter] = note;
    }
    else {
      if(counter < notes.size()-1) {
        counter += 1;
        notes[counter] = note;
      }
      else {
        notes.push_back(note);
        counter = notes.size() - 1;
      }
    }
    if(startTime >= 0) {
      notes[counter].startTime = startTime;
    }
    //notes[counter].print();
    return &notes[counter];
  }

  // FIXME
  double cancelNote() {
    double r = 0;
    if(numNotes > 1) {
      r = notes[counter].startTime;
      notes[counter].reset();
      counter = (numNotes+counter-1) % numNotes;
    }
    else if(counter > -1) {
      r = notes[counter].startTime;
      notes[counter].reset();
      counter--;
    }
    return r;
  }

  void reset() {
    for(int i=0; i<numNotes; i++) {
      notes[i].reset();
    }
  }

  void reset(int instrumentTrackIndex) {
    for(int i=0; i<numNotes; i++) {
      if(notes[i].instrumentTrackIndex == instrumentTrackIndex) {
        notes[i].reset();
      }
    }
  }

  
  void transposeTrack(double steps, int instrumentTrackIndex = -1) {
    for(int i=0; i<numNotes; i++) {
      if(instrumentTrackIndex == -1 || instrumentTrackIndex == notes[i].instrumentTrackIndex) {
        notes[i].transpose(steps);
      }
    }
  }
};*/
