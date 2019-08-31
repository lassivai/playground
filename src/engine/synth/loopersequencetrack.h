#pragma once
#include "note.h"
#include <vector>

struct LooperSequenceTrack
{
  int trackNumber;
  std::vector<Note> notes;

  double sampleRate = 1;

  LooperSequenceTrack(int trackNumber, double sampleRate) {
    this->trackNumber = trackNumber;
    this->sampleRate = sampleRate;

    reset();
  }

  LooperSequenceTrack(LooperSequenceTrack *looperSequenceTrack) {
    this->trackNumber = looperSequenceTrack->trackNumber;
    this->notes = std::vector<Note>(looperSequenceTrack->notes);
    this->sampleRate = looperSequenceTrack->sampleRate;
    //memcpy(this->notes, looperSequenceTrack->notes, sizeof(Note)*numNotes);
  }


  Note *startNote(double pitch, double startTime, double volume, int instrumentIndex, int instrumentTrackIndex) {
    notes.push_back(Note(sampleRate, pitch, startTime, volume, instrumentIndex, instrumentTrackIndex));
    return &notes[notes.size()-1];
  }

  Note *startNote(Note note, double startTime = -1) {
    printf("Starting note at looper track 1...\n");
    notes.push_back(note);
    printf("Starting note at looper track 2...\n");
    if(startTime >= 0) {
      notes[notes.size()-1].startTime = startTime;
    }
    printf("Starting note at looper track 3...\n");
    return &notes[notes.size()-1];
  }

  // FIXME
  double cancelNote() {
    double startTime = 0;
    if(notes.size() > 0) {
      startTime = notes[notes.size()-1].startTime;
      notes[notes.size()-1].reset();
      notes.erase(notes.begin()+notes.size()-1);
    }
    return startTime;
  }
  void print() {
    for(int i=0; i<notes.size(); i++) {
      printf("note %d: %f\n", i, notes[i].volume);
    }
  }
  void resetNote(int index) {
    if(index >= 0 && index < notes.size()) {
      notes.erase(notes.begin()+index);
    }
  }
  void reset() {
    for(int i=0; i<notes.size(); i++) {
      notes[i].reset();
    }
    notes.clear();
  }

  void reset(int instrumentTrackIndex) {
    for(int i=0; i<notes.size(); i++) {
      if(notes[i].instrumentTrackIndex == instrumentTrackIndex) {
        notes[i].reset();
        notes.erase(notes.begin()+i);
      }
    }
  }

  
  void transposeTrack(double steps, int instrumentTrackIndex = -1) {
    for(int i=0; i<notes.size(); i++) {
      if(instrumentTrackIndex == -1 || instrumentTrackIndex == notes[i].instrumentTrackIndex) {
        notes[i].transpose(steps);
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
