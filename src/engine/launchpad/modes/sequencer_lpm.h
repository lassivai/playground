#pragma once
#include "../launchpad.h"
#include "../../synth/synth.h"

/* TODO
 *  - putting notes on the current looper position while playing (DONE)
 *  - some sort of way to allow fixed setting of notes to arbitrary time positions (DONE)
 *  - drumpad file io (DONE)
 *    - automatically load the associated instruments (not done)
 *  - bug that causes a crash somewhere (FIXED)
 *    - probably either in here or in drumpad related things
 *    - debug without this first, controlling drumpad with a keyboard
 *
 *  - apply changes to gui elements appropriately
 *  - playing of more than 8 (64) instruments simultaneously in drumpad, when not recording (DONE)
 *  - volume option to individual notes (hold the note on the pad and choose the volume from "east")
 *  - volume option to all notes of a given pad
 *  - option to choose default volume, which would be applied to all subsequent notes
 *  - copy paste pad
 */


struct SequencerLaunchpadMode : public LaunchpadMode
{
  Synth *synth;

  std::vector<Vec2i> grid = std::vector<Vec2i>(8*8);
  std::vector<Vec2i> north = std::vector<Vec2i>(8);
  std::vector<Vec2i> east = std::vector<Vec2i>(8);

  int verticalZoomMode = 1;
  int horizontalZoomMode = 0;
  int verticalPosition = 0;
  
  Vec2d timeRange;
  Vec2i noteRange;
  
  int instrumentTrackIndex = 0;
  
  int drumPadModeMeasure = -1;
  int drumPadModePadPage = 0;
  
  int recordingMode = 0;
  
  const std::vector<std::string> padAssignments = { "Select instrument track 0-7, (64-71)"
                                                    "Cancel last note (64, 65)",
                                                    "Clear instrument looper notes (64, 66)",
                                                    "Clear all looper notes (64, 65, 66)",
                                                    "Change recording mode (78, 79)",
                                                    "To start (72), <- loop (73), <- beat (74), <- subdiv (75)",
                                                    "To end (79), -> loop (78), -> beat (77), -> subdiv (76)",
                                                    "Looper duration 1-64 sec, (72, 73) + [n]",
                                                    "Looper measures 1-64, (72, 74) + [n]",
                                                    "Looper subdivs 1-64, (72, 75) + [n]",
                                                    "Note length 1-64, (72, 76) + [n]",
                                                    "Instrument volume = 0-4, (73, 74) + [n]",
                                                    "Instrument 0-63, (73, 75) + [n]",
                                                    "Instrument 64-127, (73, 76) + [n]",
                                                    "Instrument 128-191, (73, 77) + [n]",
                                                    "Instrument 192-255, (73, 78) + [n]",
                                                    "Instrument 256-319, (73, 79) + [n]",
                                                    "(drumpad) Measure 0-63, (71) + [n]",
                                                    "(drumpad) Pad page 0-7, (64-71) + [x=7, y=n]",
                                                    "(drumpad) Clear pad notes 0-7, (64-71) + [x=6, y=n]",
                                                    "(drumpad playing) Play pad note 0-63, [n]",
                                                    "(drumpad recording) Record/clear pad note 0-7, [y]",
                                                    "(other instrument) Change vertical zoom mode (70, 71)",
                                                    "(other instrument) Change horizontal zoom mode (69, 70)",
                                                    "(other instrument) Vertical position (64-71)",
                                                    "(other instrument) Play note y + vertical position [y]"};
  virtual void printControls() {
    for(int i=0; i<padAssignments.size(); i++) {
      messageQueue.addMessage(to_wstring(padAssignments[i]));
    }
  }
  
  
  SequencerLaunchpadMode(LaunchPadInterface *launchPad, Synth *synth, GuiElement *parentGuiElement) : LaunchpadMode(launchPad, "Sequencer", parentGuiElement) {
    this->synth = synth;
  }


  void onUpdateGrid(double time, double dt) {
/*
    memset(grid.data(), 0, grid.size() * sizeof(Vec2i));
    
    double loopFraction = synth->getLooperTime()/synth->looperTrackDuration;

    LooperSequenceTrack *looperSequenceTrack = synth->getActiveLooperSequenceTrack();
    //LooperSequenceTrack *looperSequenceTrack = synth->sequenceLooper.tracks[instrumentTrackIndex];

    DrumPad *drumPad = dynamic_cast<DrumPad*>(synth->instruments[synth->instrumentTracks[instrumentTrackIndex].instrumentIndex]);

    
    timeRange.set(0, synth->looperTrackDuration);
    int maxX = 8;
    
    if(drumPad) {
      if(drumPadModeMeasure >= 0 && drumPadModeMeasure < synth->measuresPerLooperTrack) {
        double t = synth->looperTrackDuration / synth->measuresPerLooperTrack;
        timeRange.set(drumPadModeMeasure * t, (drumPadModeMeasure+1) * t);
      }
    }
    else {
      if(horizontalZoomMode == 1) {
        int n = synth->measuresPerLooperTrack * synth->subdivisionsPerMeasure;
        if(n <= 8) {
          maxX = n;
          timeRange.set(0, synth->looperTrackDuration);
        }
        else {
          double td = (double)maxX * synth->looperTrackDuration / n;
          double t0 = clamp(synth->getLooperTime() - td*0.5, 0.0, synth->looperTrackDuration-td);
          timeRange.set(t0, t0+td);
        }
      }
    }
    
    noteRange.set(synth->screenKeyboardMinNote, synth->screenKeyboardMaxNote);
    
    if(drumPad) {
      noteRange.set(drumPad->pitchOffset + drumPadModePadPage*8, drumPad->pitchOffset + 7 + drumPadModePadPage*8);
    }
    else {
      if(verticalZoomMode == 1) {
        int minNote = 127, maxNote = 0;
        for(int i=0; i<looperSequenceTrack->notes.size(); i++) {
          if(looperSequenceTrack->notes[i].volume > 0) {
            double pitch = looperSequenceTrack->notes[i].pitch;
            if(drumPad) {
              pitch = drumPad->pitchOffset + looperSequenceTrack->notes[i].padIndex;
            }
            int note = (int)pitch;
            minNote = min(note, minNote);
            maxNote = max(note, maxNote);
          }
        }
        if(minNote < maxNote) {
          noteRange.set(max(minNote, synth->screenKeyboardMinNote), min(maxNote, synth->screenKeyboardMaxNote));
        }
      }
      if(verticalZoomMode == 2 || verticalZoomMode == 3) {
        if(verticalZoomMode == 2) {
          double time = synth->getLooperTime();
          double closestStartTime = 0;
          int closestStartTimeNoteIndex = -1;
          for(int i=0; i<looperSequenceTrack->notes.size(); i++) {
            if(looperSequenceTrack->notes[i].volume > 0 && fabs(time - looperSequenceTrack->notes[i].startTime) < fabs(time - closestStartTime)) {
              closestStartTime = looperSequenceTrack->notes[i].startTime;
              closestStartTimeNoteIndex = i;
            }
          }
          if(closestStartTimeNoteIndex >= 0) {
            int verticalPositionNew = 0;
            int n = 0;
            for(int i=0; i<looperSequenceTrack->notes.size(); i++) {
              if(looperSequenceTrack->notes[i].volume > 0 && closestStartTime == looperSequenceTrack->notes[i].startTime) {
                double pitch = looperSequenceTrack->notes[i].pitch;
                if(drumPad) {
                  pitch = drumPad->pitchOffset + looperSequenceTrack->notes[i].padIndex;
                }
                int verticalPositionTmp = (int)clamp(pitch-4, 0, 127-7);
                if(verticalPositionTmp > verticalPositionNew) {
                  verticalPositionNew = verticalPositionTmp;
                }
              }
            }
            if(verticalPositionNew < verticalPosition || verticalPositionNew > verticalPosition + 7) {
              verticalPosition = verticalPositionNew;
            }
          }
        }
        noteRange.set(verticalPosition, verticalPosition+7);
      }
    }

    for(int i=0; i<looperSequenceTrack->notes.size(); i++) {
      if(looperSequenceTrack->notes[i].volume > 0 && looperSequenceTrack->notes[i].startTime > timeRange.x && looperSequenceTrack->notes[i].startTime < timeRange.y) {
        Vec2i col(1, 0);
        if(looperSequenceTrack->notes[i].noteLength != 0) {
          
          double pitch = looperSequenceTrack->notes[i].pitch;
          if(drumPad) {
            pitch = drumPad->pitchOffset + looperSequenceTrack->notes[i].padIndex;
          }
          if(pitch < noteRange.x || pitch > noteRange.y) continue;
          
          int x0 = (int)clamp(map(looperSequenceTrack->notes[i].startTime, timeRange.x, timeRange.y, 0.0, maxX), 0, 7.99);

          int y = (int)clamp(map(pitch, noteRange.x, noteRange.y, 0.0, 7.99), 0, 7.99);
          
          if(synth->getLooperTime() > looperSequenceTrack->notes[i].startTime && synth->getLooperTime() < looperSequenceTrack->notes[i].startTime + looperSequenceTrack->notes[i].noteLength) {
            if(y >= 0 || y < 8) {
              col.x = 3;
              col.y = 3;
            }
            else {
              col.x = 3;
              col.y = 0;
            }
          }
          grid[x0 + y*8].x = max(grid[x0 + y*8].x, col.x);
          grid[x0 + y*8].y = max(grid[x0 + y*8].y, col.y);
        }
      }
    }

    for(int i=0; i<8; i++) {
      for(int j=0; j<8; j++) {
        if(horizontalZoomMode == 1) {
          double t = timeRange.x + (timeRange.y-timeRange.x)/8.0 * i;
          t = t / synth->looperTrackDuration * synth->measuresPerLooperTrack * synth->subdivisionsPerMeasure;
          int it = (int)t;
          if(it % synth->subdivisionsPerMeasure == 0) {
            grid[i+j*8].x = max(grid[i+j*8].x, 1);
            grid[i+j*8].y = max(grid[i+j*8].y, 1);
          }
        }
        if((verticalZoomMode == 2 || verticalZoomMode == 3) && !drumPad) {
          int note = noteRange.x + 7 - j;
          if((note + synth->keyBaseNote) % 12 == 0) {
            grid[i+j*8].x = max(grid[i+j*8].x, 1);
            grid[i+j*8].y = max(grid[i+j*8].y, 1);
          }
        }
        launchPad->setPadState(i, j, grid[i+j*8].x, grid[i+j*8].y); 
      }
    }
    
    memset(north.data(), 0, north.size() * sizeof(Vec2i));
    memset(east.data(), 0, east.size() * sizeof(Vec2i));
    
    if(synth->isLooperPlaying) {
      north[(int)clamp(loopFraction * 8.0, 0, 7)].y = 3;
    }
    if(drumPad && drumPadModeMeasure >= 0 && drumPadModeMeasure < synth->measuresPerLooperTrack) {
      if(fract(time*4.0) < 0.5) {
        north[clamp(drumPadModeMeasure, 0, 7)].x = 3;
      }
    }
    else {
      north[clamp(synth->sequenceLooper.currentTrack % 7, 0, 7)].x = 3;
    }
    east[clamp(instrumentTrackIndex, 0, 7)].x = 3;
    
    if(drumPad) {
      east[clamp(drumPadModePadPage, 0, 7)].y = 3;
    }
    else {
      east[clamp(7 - (int)map(verticalPosition, synth->screenKeyboardMinNote, synth->screenKeyboardMaxNote, 0.0, 7.99), 0, 7)].y = 3;
    }
    
    if(synth->isRecordingMode > 0) {
      for(int i=0; i<8; i++) {
        if((recordingMode == 1 && fract(time) < 0.5) || (recordingMode == 2 && fract(time+0.5*i) < 0.5)) {
          north[i].y = max(north[i].y, 1);
        }
      }
    }
    
    for(int i=0; i<8; i++) {
      launchPad->setPadState(8, i, east[i].x, east[i].y); 
      launchPad->setPadState(i, 8, north[i].x, north[i].y); 
    }
      */
  }
  
  void onAction(int x, int y, LaunchPadInterface::KeyState keyState) {/*
    DrumPad *drumPad = dynamic_cast<DrumPad*>(synth->instruments[synth->instrumentTracks[instrumentTrackIndex].instrumentIndex]);
    LooperSequenceTrack *looperSequenceTrack = synth->getActiveLooperSequenceTrack();
    
    if(launchPad->arePadsPressedAndCancelRelease(64, 65, 66)) {
      looperSequenceTrack->reset();
    }
    if(launchPad->arePadsPressedAndCancelRelease(64, 66)) {
      looperSequenceTrack->reset(instrumentTrackIndex);
    }
    if(launchPad->arePadsPressedAndCancelRelease(64, 65)) {
      looperSequenceTrack->cancelNote();
    }
    if(launchPad->arePadsPressedAndCancelRelease(78, 79)) {
      recordingMode = (recordingMode+1) % 3;
      synth->isRecordingMode = recordingMode != 0;
    }
    
    
    if(y == 8 && keyState == LaunchPadInterface::KeyState::ReleasedInactive) {
      if(x == 0) {
        synth->toBeginning();
      }
      if(x == 1) {
        synth->previousLoop();
      }
      if(x == 2) {
        synth->progressLoopTimeByMeasures(-1);
      }
      if(x == 3) {
        synth->progressLoopTimeByMeasureSubdivisions(-1);
      }
      if(x == 4) {
        synth->progressLoopTimeByMeasureSubdivisions(1);
      }
      if(x == 5) {
        synth->progressLoopTimeByMeasures(1);
      }
      if(x == 6) {
        synth->nextLoop();
      }
      if(x == 7) {
        synth->togglePlay();
      }
    }
    


    if(x >= 0 && x < 8 && y >= 0 && y < 8 && keyState == LaunchPadInterface::KeyState::PressedActive) {
      if(launchPad->arePadsPressedAndCancelRelease(72, 73)) {
        synth->setLoopDuration(1 + x + y*8);

        launchPad->padKeyStates[x+y*8] = LaunchPadInterface::KeyState::ReleasedInactive;
        return;
      }
      if(launchPad->arePadsPressedAndCancelRelease(72, 74)) {
        synth->measuresPerLooperTrack = 1 + x + y*8;
        launchPad->padKeyStates[x+y*8] = LaunchPadInterface::KeyState::ReleasedInactive;
        return;
      }
      if(launchPad->arePadsPressedAndCancelRelease(72, 75)) {
        synth->subdivisionsPerMeasure = 1 + x + y*8;
        launchPad->padKeyStates[x+y*8] = LaunchPadInterface::KeyState::ReleasedInactive;
        return;
      }
      if(launchPad->arePadsPressedAndCancelRelease(72, 76)) {
        synth->noteValueInverse = 1 + x + y*8;
        launchPad->padKeyStates[x+y*8] = LaunchPadInterface::KeyState::ReleasedInactive;
        return;
      }

      if(instrumentTrackIndex >= 0 && instrumentTrackIndex < synth->numInstrumentTracks) {
        int i = x + y*8;
        if(launchPad->arePadsPressedAndCancelRelease(73, 74)) {
          if(i == 0) {
            synth->instrumentTracks[instrumentTrackIndex].isMuted = !synth->instrumentTracks[instrumentTrackIndex].isMuted;
          }
          else {
            synth->instrumentTracks[instrumentTrackIndex].volume = ((double)i / 63) * 4.0;
          }
          launchPad->padKeyStates[x+y*8] = LaunchPadInterface::KeyState::ReleasedInactive;
          return;
        }
        if(launchPad->arePadsPressedAndCancelRelease(73, 75)) {
          synth->instrumentTracks[instrumentTrackIndex].instrumentIndex = i;
          launchPad->padKeyStates[x+y*8] = LaunchPadInterface::KeyState::ReleasedInactive;
          return;
        }
        if(launchPad->arePadsPressedAndCancelRelease(73, 76)) {
          synth->instrumentTracks[instrumentTrackIndex].instrumentIndex = 64 + i;
          launchPad->padKeyStates[x+y*8] = LaunchPadInterface::KeyState::ReleasedInactive;
          return;
        }
        if(launchPad->arePadsPressedAndCancelRelease(73, 77)) {
          synth->instrumentTracks[instrumentTrackIndex].instrumentIndex = 2*64 + i;
          launchPad->padKeyStates[x+y*8] = LaunchPadInterface::KeyState::ReleasedInactive;
          return;
        }
        if(launchPad->arePadsPressedAndCancelRelease(73, 78)) {
          synth->instrumentTracks[instrumentTrackIndex].instrumentIndex = 3*64 + i;
          launchPad->padKeyStates[x+y*8] = LaunchPadInterface::KeyState::ReleasedInactive;
          return;
        }
        if(launchPad->arePadsPressedAndCancelRelease(73, 79)) {
          synth->instrumentTracks[instrumentTrackIndex].instrumentIndex = 4*64 + i;
          launchPad->padKeyStates[x+y*8] = LaunchPadInterface::KeyState::ReleasedInactive;
          return;
        }
        synth->instrumentTracks[instrumentTrackIndex].instrumentIndex = clamp(synth->instrumentTracks[instrumentTrackIndex].instrumentIndex, 0, synth->instruments.size()-1);
      }
    }
    
                              
    if(drumPad) {
      if(x >= 0 && x < 8 && y == 0 && keyState == LaunchPadInterface::KeyState::PressedActive) {
        for(int i=0; i<8; i++) {
          if(launchPad->arePadsPressedAndCancelRelease(72+i)) {
            if(drumPadModeMeasure == i) {
              drumPadModeMeasure = -1;
            }
            else {
              drumPadModeMeasure = i;
            }
            launchPad->padKeyStates[x+y*8] = LaunchPadInterface::KeyState::ReleasedInactive;
            return;
          }
        }
      }
      if(x == 7 && y >= 0 && y < 8 && keyState == LaunchPadInterface::KeyState::PressedActive) {
        for(int i=0; i<8; i++) {
          if(launchPad->arePadsPressedAndCancelRelease(64+i)) {
            drumPadModePadPage = i;
            launchPad->padKeyStates[x+y*8] = LaunchPadInterface::KeyState::ReleasedInactive;
            return;
          }
        }
      }

      if(x == 6 && y >= 0 && y < 8 && keyState == LaunchPadInterface::KeyState::PressedActive) {
        for(int i=0; i<8; i++) {
          if(launchPad->arePadsPressedAndCancelRelease(64+i)) {
            for(int k=0; k<looperSequenceTrack->notes.size(); k++) {
              if(looperSequenceTrack->notes[k].padIndex == drumPadModePadPage*8 + i) {
                looperSequenceTrack->notes[k].reset();
              }
            }
            launchPad->padKeyStates[x+y*8] = LaunchPadInterface::KeyState::ReleasedInactive;
            return;
          }
        }
      }
      if(x >= 0 && x < 8 && y >= 0 && y < 8 && keyState == LaunchPadInterface::KeyState::PressedActive) {
        int pad = recordingMode == 0 ? x + y*8: y + drumPadModePadPage * 8;
        
        if(pad < drumPad->numPads) {
          double time = -1, time2 = -1;
          bool noteRemoved = false;
          
          if(recordingMode == 1) {
            if(synth->subdivisionsPerMeasure % 2 == 0) {
              time = timeRange.x + (timeRange.y-timeRange.x) / 8.0 * x;
              time2 = timeRange.x + (timeRange.y-timeRange.x) / 8.0 * (x+1);
            }
            else {
              time = (double) x / synth->subdivisionsPerMeasure * synth->looperTrackDuration / synth->measuresPerLooperTrack;
              time2 = (double) (x+1) / synth->subdivisionsPerMeasure * synth->looperTrackDuration / synth->measuresPerLooperTrack;
            }

            for(int i=0; i<looperSequenceTrack->notes.size(); i++) {
              //if(looperSequenceTrack->notes[i].volume > 0 && pad == drumPad->pitchOffset + looperSequenceTrack->notes[i].padIndex && synth->roundLoopTimeNoteValueInverse(time) == synth->roundLoopTimeNoteValueInverse(looperSequenceTrack->notes[i].startTime)) {
              if(looperSequenceTrack->notes[i].volume > 0 && pad == looperSequenceTrack->notes[i].padIndex && looperSequenceTrack->notes[i].startTime > time && looperSequenceTrack->notes[i].startTime < time2) {
                looperSequenceTrack->notes[i].reset();
                noteRemoved = true;
              }
            }
          }
          if(!noteRemoved) {
            Note *note = synth->startInstrumentTrackNote(pad+drumPad->pitchOffset, synth->defaultNoteVolume, instrumentTrackIndex, time);
            if(note) {
              note->noteLength = 1.0/synth->noteValueInverse*synth->looperTrackDuration/synth->measuresPerLooperTrack;
              note->keyHoldDuration = note->noteLength;
              note->isHolding = false;
              //note->noteActualLength = synth->instruments[synth->instrumentTracks[instrumentTrackIndex].instrumentIndex]->getNoteActualLength(*note);
              note->noteActualLength = synth->instruments[drumPad->pads[pad].instrumentIndex]->getNoteActualLength(*note);
            }
          }
        }
      }

    }
    // FIXME
    else {
      //printf("1. %d %d %d\n", x, y, keyState);


      //if(synth->isLooperPlaying) {
        if(x >= 0 && x < 8 && y >= 0 && y < 8 && keyState == LaunchPadInterface::KeyState::PressedActive) {
          int pitch = 32 + x + y*8;

          Note *note = synth->startInstrumentTrackNote(pitch, synth->defaultNoteVolume, instrumentTrackIndex);
          note->noteActualLength = synth->instruments[synth->instrumentTracks[instrumentTrackIndex].instrumentIndex]->getNoteActualLength(*note);
        //}
      }
  
    }*/
  }
  
  
  virtual void onOutputActivated(bool value) {
    if(value) {
      launchPad->setOverrideKeystates(true, false, false, false);
      launchPad->resetPad();
    }
  }

  
};

















