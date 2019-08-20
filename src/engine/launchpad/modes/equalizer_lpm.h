#pragma once
#include "../launchpad.h"
#include <vector>
#include "../../synth/synth.h"
#include "../../synth/filter/eq.h"

struct EqualizerLaunchpadMode : public LaunchpadMode
{
  std::vector<double> eqValues;
  Equalizer *equalizer = NULL;

  EqualizerLaunchpadMode(LaunchPadInterface *launchPad, Equalizer *equalizer) : LaunchpadMode(launchPad, "Equalizer") {
    this->equalizer = equalizer;
    eqValues.resize(maxNumPeakingEqs);
  }

  void onUpdateGrid(double time, double dt) {
    
    double maxGain = 16;
    
    for(int i=0; i<equalizer->numPeakingEqs; i++) {
      if(fabs(equalizer->peakingEqs[i].dbGain) > maxGain) {
        maxGain = fabs(equalizer->peakingEqs[i].dbGain);
      }
    }
    for(int i=0; i<8; i++) {
      for(int k=0; k<8; k++) {
        launchPad->setPadState(i, k, 0, 0, true);
      }
    }
    for(int i=0; i<8; i++) {
      if(equalizer->peakingEqs[i].dbGain == 0) {
        launchPad->setPadState(i, 3, 0, 1, true);
        launchPad->setPadState(i, 4, 0, 1, true);
      }
      else {
        double p = max(1, 4*fabs(equalizer->peakingEqs[i].dbGain) / maxGain);
        for(int k=0; k<p; k++) {
          if(equalizer->peakingEqs[i].dbGain > 0) {
            launchPad->setPadState(i, 3-k, 0, 3, true);
          }
          else {
            launchPad->setPadState(i, 4+k, 0, 3, true);
          }
        }
      }
    }
    if(equalizer->isActive) {
      for(int i=0; i<8; i++) {
        launchPad->setPadState(8, i, 0, 1, true);
        launchPad->setPadState(i, 8, 0, 1, true);
      }
    }
    else {
      for(int i=0; i<8; i++) {
        launchPad->setPadState(8, i, 0, 0, true);
        launchPad->setPadState(i, 8, 0, 0, true);
      }
    }
  }
  
  std::vector<double> dbDelta = {10, 4, 4, 0, 0, -1, -4, -10};
  
  void onAction(int x, int y, LaunchPadInterface::KeyState keyState) {
    if(x >= 0 && x < 8 && y >= 0 && y < 8 && keyState == LaunchPadInterface::KeyState::PressedActive) {
      if(x < equalizer->numPeakingEqs) {
        if(dbDelta[y] == 0) {
          equalizer->peakingEqs[x].dbGain = 0;
        }
        else {
          equalizer->peakingEqs[x].dbGain += dbDelta[y];
        }
        equalizer->peakingEqs[x].prepare();
        equalizer->updatePanel();
      }
    }
    if(x == 7 && y == 8 && keyState == LaunchPadInterface::KeyState::ReleasedInactive) {
      equalizer->isActive = !equalizer->isActive;
      equalizer->updatePanel();
    }
  }
  
  virtual void onOutputActivated(bool value) {
    if(value) {
      launchPad->setOverrideKeystates(true, false, false, false);
      launchPad->resetPad();
    }
  }
  
};

















