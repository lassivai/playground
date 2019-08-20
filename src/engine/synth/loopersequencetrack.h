#pragma once
#include "note.h"
#include <vector>


struct LooperSequenceTrack
{
  int trackNumber;
  int numNotes;
  std::vector<TrackNote> notes;
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
    this->notes = std::vector<TrackNote>(looperSequenceTrack->notes);
    this->sampleRate = looperSequenceTrack->sampleRate;
    //memcpy(this->notes, looperSequenceTrack->notes, sizeof(Note)*numNotes);
  }

  void setNumNotes(int numNotes) {
    this->numNotes = numNotes;
    if(numNotes > notes.size()) {
      notes.resize(numNotes);
    }
  }

  TrackNote *startNote(double pitch, double startTime, double volume, int instrumentIndex, int instrumentTrackIndex) {
    if(numNotes > 0) {
      counter = (counter+1) % numNotes;
      notes[counter].set(sampleRate, pitch, startTime, volume, instrumentIndex);
    }
    else {
      if(counter < notes.size()-1) {
        counter += 1;
        notes[counter] = TrackNote(sampleRate, pitch, startTime, volume, instrumentIndex, instrumentTrackIndex);
      }
      else {
        notes.push_back(TrackNote(sampleRate, pitch, startTime, volume, instrumentIndex, instrumentTrackIndex));
        counter = notes.size() - 1;
      }
    }
    //notes[counter].print();
    return &notes[counter];
  }

  TrackNote *startNote(const TrackNote &note, double startTime = -1) {
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

  /*TrackNote *getMostRecentNote() {
    return &notes[counter];
  }*/

  void transposeTrack(double steps, int instrumentTrackIndex = -1) {
    for(int i=0; i<numNotes; i++) {
      if(instrumentTrackIndex == -1 || instrumentTrackIndex == notes[i].instrumentTrackIndex) {
        notes[i].transpose(steps);
      }
    }
  }
};
