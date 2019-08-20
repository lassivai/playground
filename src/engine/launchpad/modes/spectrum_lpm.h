#pragma once
#include "../launchpad.h"
#include <vector>


struct SpectrumLaunchpadMode : public LaunchpadMode
{
  double spectrumScaling = 5.0;
  std::vector<double> *srcSpectrum = NULL;
  std::vector<double> padSpectrum = std::vector<double>(8);
  std::vector<double> previousLevels = std::vector<double>(8);

  SpectrumLaunchpadMode(LaunchPadInterface *launchPad, std::vector<double> *srcSpectrum) : LaunchpadMode(launchPad, "Spectrum") {
    this->srcSpectrum = srcSpectrum;
  }

  void onUpdateGrid(double time, double dt) {
    
    resizeArray(*srcSpectrum, padSpectrum);
    scaleArray(padSpectrum, spectrumScaling);
    
    for(int i=0; i<8; i++) {
      int level = (int)round(padSpectrum[i] * 8.0);
      previousLevels[i] = max(padSpectrum[i] * 8.0, min(9.0, previousLevels[i]));
      for(int k=0; k<8; k++) {
        if(level >= (k+1)) {
          launchPad->setPadState(i, 7-k, 3, 3, true);
        }
        else if(previousLevels[i] > k+1) {
          launchPad->setPadState(i, 7-k, 0, 1, true);
        }
        else {
          launchPad->setPadState(i, 7-k, 0, 0, true);
        }
      }
      previousLevels[i] -= 2.0 * dt;
      previousLevels[i] = max(previousLevels[i], 0);
    }
    for(int i=0; i<8; i++) {
      launchPad->setPadState(8, i, 0, 0, true);
      launchPad->setPadState(i, 8, 0, 0, true);
    }
  }
  
  void onAction(int x, int y, LaunchPadInterface::KeyState keyState) {
  }
  
  virtual void onOutputActivated(bool value) {
    if(value) {
      launchPad->setOverrideKeystates(true, false, false, false);
      launchPad->resetPad();
    }
  }
  
};

















