#pragma once
#include "../launchpad.h"
#include <vector>
#include "../../synth/synth.h"
#include "equalizer_lpm.h"

struct RecordingTrackLaunchpadMode : public LaunchpadMode
{
  enum Mode {Default, Equalizer};
  Synth *synth = NULL;
  EqualizerLaunchpadMode *equalizerLaunchpadMode = NULL;
  Mode mode = Default;
  int activeRecordingTrack = 0;

  const std::vector<std::string> padAssignments = { "Start/stop playback overwriting existing active recordings (79)"
                                                    "Set amount of recordings (72-78)"
                                                    "Toggle recording 0-7 active (64-71)"
                                                    "Mute recording 0-7, [x=0, y=n]",
                                                    "Set recording y volume to x 1/3-3, [x>0, y=n]",
                                                    "Clear recording 0-7, (64-71) + [x=0, y=n]",
                                                    "Edit equalizer 0-7, (64-71) + [x=7, y=n]",
                                                    "(equalizer) set equalizer 0-7 active, (64-71)",};
                                                    
  virtual void printControls() {
    for(int i=0; i<padAssignments.size(); i++) {
      messageQueue.addMessage(to_wstring(padAssignments[i]));
    }
  }
  
  RecordingTrackLaunchpadMode(LaunchPadInterface *launchPad, Synth *synth) : LaunchpadMode(launchPad, "Recording tracks") {
    this->synth = synth;
  }

  void onUpdateGridDefault(double time, double dt) {
    for(int i=0; i<8; i++) {
      launchPad->setPadState(8, i, 0, 0, true);
      launchPad->setPadState(i, 8, 0, 0, true);
      for(int k=0; k<8; k++) {
        launchPad->setPadState(i, k, 0, 0, true);
      }
    }
    for(int i=0; i<8; i++) {
      for(int k=0; k<8; k++) {
        if(i >= synth->numAudioRecordingTracks) continue;
        int a = 0, b = 3;
        int vol = (int)round(synth->audioRecordingTracks[i].volume * 3.0);
        if(synth->audioRecordingTracks[i].isMuted) {
          a = 1;
          b = 1;
        }
        double t = fract(time*2.0*7.0/vol) * vol / 7.0;
        if(synth->audioRecordingTracks[i].isRecording && t > 1.0/8.0 * k && t < 1.0/8.0 * (k+1.0)) {
          a = 3;
          b = 3;
        }
        if(k+1 <= vol) {
          launchPad->setPadState(k, i, a, b, true);
        }
      }
    }

    for(int k=0; k<8; k++) {
      int a = 0, b = 0;
      if(k < synth->numAudioRecordingTracks) {
        b = 3;
        if(synth->audioRecordingTracks[k].isRecorded) {
          a = 3;
          b = 0;
        }
        /*if(synth->audioRecordingTracks[k].isMuted) {
          a = 1;
          b = 1;
        }*/
        /*a = 3;
        if(synth->numAudioRecordingTracks[k]->isRecording && fract(time*4.0) < 0.5) {
          a = 0;
          b = 3;
        }*/
      }
      launchPad->setPadState(8, k, a, b, true);
    }
    if(synth->isLooperPlaying) {
      launchPad->setPadState((int)clamp(synth->getLooperTime()/synth->looperTrackDuration * 8.0, 0, 7), 8, 0, 3, true);
    }
  }
  
  
  std::vector<double> volumes =  {0, 0.333, 0.666, 1.0, 1.333, 1.666, 2.0, 2.4, 3.0};
  void onActionDefault(int x, int y, LaunchPadInterface::KeyState keyState) {
    
    if(x == 0 && y >= 0 && y < 8 && keyState == LaunchPadInterface::KeyState::PressedActive) {
      for(int i=0; i<8; i++) {
        if(launchPad->arePadsPressedAndCancelRelease(64+i)) {
          if(i < synth->numAudioRecordingTracks) {
            synth->audioRecordingTracks[i].reset();
            launchPad->padKeyStates[x + y*8] = LaunchPadInterface::KeyState::ReleasedInactive;
            return;
          }
        }
      }
    }
    
                                                        
    if(x >= 0 && x < 8 && y >= 0 && y < 8 && keyState == LaunchPadInterface::KeyState::PressedActive) {
      if(y < synth->numAudioRecordingTracks) {
        if(x == 0) {
          synth->audioRecordingTracks[y].isMuted = !synth->audioRecordingTracks[y].isMuted;
        }
        else {
          if(x == 7) {
            synth->audioRecordingTracks[y].volume = synth->audioRecordingTracks[y].volume == volumes[7] ? volumes[8] : volumes[7];
          }
          else {
            synth->audioRecordingTracks[y].volume = volumes[x];
          }
        }
      }
    }
    
    if(y == 8 && keyState == LaunchPadInterface::KeyState::ReleasedInactive) {
      if(x == 7) {
        synth->looperTime = 0.0;
        synth->sequenceLooper.currentTrack = 0;
        synth->sequenceLooper.currentRepeat = 0;
        synth->setLooperTracksPlaying(!synth->isLooperPlaying);
      }
      else {
        if(synth->numAudioRecordingTracks == x + 1) {
          if(x == 0) {
            synth->numAudioRecordingTracks = clamp(1-synth->numAudioRecordingTracks, 0, 1);
          }
          if(x == 6 && synth->numAudioRecordingTracks < 8) {
            synth->numAudioRecordingTracks = synth->numAudioRecordingTracks == 7 ? 8 : 7;
          }
        }
        else {
          synth->numAudioRecordingTracks = x + 1;
        }
      }
    }
    if(x == 8 && keyState == LaunchPadInterface::KeyState::ReleasedInactive) {
      if(y < synth->numAudioRecordingTracks && !synth->isLooperPlaying) {
        synth->audioRecordingTracks[y].toggleRecording();
      }
    }
  }

  void onUpdateGrid(double time, double dt) {
    if(mode == Default) {
      onUpdateGridDefault(time, dt);
    }
    if(mode == Equalizer && equalizerLaunchpadMode) {
      equalizerLaunchpadMode->onUpdateGrid(time, dt);
      launchPad->setPadState(8, activeRecordingTrack, 3, 3, true);
    }
  }
  
  void onAction(int x, int y, LaunchPadInterface::KeyState keyState) {
    bool mainEvent = false;
    if(x == 7 && y >= 0 && y < 8 && keyState == LaunchPadInterface::KeyState::PressedActive) {
      for(int i=0; i<8; i++) {
        if(launchPad->arePadsPressedAndCancelRelease(64+i)) {
          if(i < synth->numAudioRecordingTracks) {
            if(i == activeRecordingTrack && mode == Equalizer) {
              mode = Default;
            }
            else {
              activeRecordingTrack = i;
              if(!equalizerLaunchpadMode) {
                equalizerLaunchpadMode = new EqualizerLaunchpadMode(launchPad, synth->audioRecordingTracks[activeRecordingTrack].equalizer);
                synth->audioRecordingTracks[activeRecordingTrack].equalizer->isActive = true;
              }
              else {
                equalizerLaunchpadMode->equalizer = synth->audioRecordingTracks[activeRecordingTrack].equalizer;
                synth->audioRecordingTracks[activeRecordingTrack].equalizer->isActive = true;
              }
              mode = Equalizer;
            }
            mainEvent = true;
            launchPad->padKeyStates[x + y*8] = LaunchPadInterface::KeyState::ReleasedInactive;
          }
        }
      }
    }
                                                      
    if(x == 8 && keyState == LaunchPadInterface::KeyState::ReleasedInactive) {
      activeRecordingTrack = y;
      if(mode == Equalizer && equalizerLaunchpadMode) {
        equalizerLaunchpadMode->equalizer = synth->audioRecordingTracks[activeRecordingTrack].equalizer;
        synth->audioRecordingTracks[activeRecordingTrack].equalizer->isActive = true;
        mainEvent = true;
      }
    }

    if(!mainEvent) {
      if(mode == Default) {
        onActionDefault(x , y, keyState);
      }
      if(mode == Equalizer && equalizerLaunchpadMode) {
        equalizerLaunchpadMode->onAction(x , y, keyState);
      }
    }
  }
  
  virtual void onOutputActivated(bool value) {
    if(value) {
      launchPad->setOverrideKeystates(true, false, false, false);
      launchPad->resetPad();
    }
  }
  
};

















