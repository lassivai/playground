#pragma once

#include "note.h"

struct InstrumentTrack {
  int instrumentIndex = 0;
  double volume = 1.0;
  int midiPortIndex = 0;
  int midiInChannel = 0;
  int midiOutChannel = 0;
  bool isMuted = false;
  //bool isSolo = false;
  
  std::vector<Note> notes;
};
