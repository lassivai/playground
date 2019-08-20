#pragma once

#include "../rtmidi_init.h"
#include "../geom_gfx.h"
#include <vector>


struct PadState {
  unsigned char greenLevel;
  unsigned char redLevel;
  bool flashing;
  unsigned char doubleBuffered;

  PadState() {}
  PadState(unsigned char greenLevel, unsigned char redLevel, bool flashing, unsigned char doubleBuffered) {
    this->greenLevel = greenLevel;
    this->redLevel = redLevel;
    this->flashing = flashing;
    this->doubleBuffered = doubleBuffered;
  }

  PadState &operator=(unsigned char state) {
    set(state);
    return *this;
  }

  void set(unsigned char greenLevel, unsigned char redLevel, bool flashing, bool doubleBuffered) {
    this->greenLevel = greenLevel;
    this->redLevel = redLevel;
    this->flashing = flashing;
    this->doubleBuffered = doubleBuffered;
  }

  void set(unsigned char state) {
    this->greenLevel = state / 64;
    this->redLevel = state & 3;
    this->flashing = ((state & ~3) & 15) == 8 ? 1 : 0;
    this->doubleBuffered = ((state & ~3) & 15) == 0 ? 1 : 0;
  }

  unsigned char getStateAsByte() {
    unsigned char state = (greenLevel & 3) << 4 | (flashing ? 1<<3 : 0) | (doubleBuffered ? 3<<2 : 0) | (redLevel & 3);
    return state;
  }

};

struct LaunchPadListener {
  virtual ~LaunchPadListener() {}
  virtual void onAction(int x, int y, int keyState);
};


struct LaunchPadInterface
{
  enum KeyState { PressedActive, ReleasedActive, PressedInactive, ReleasedInactive, Holding };

  enum PadAccessMode { ALL_PADS, SQUARE_PADS, ROUND_PADS, ROUND_PADS_RIGHT, ROUND_PADS_TOP };
  MidiInterface *midiInterface = NULL;
  int midiPortIndex = -1;
  static const int numSquarePads = 64, numRoundPadsRight = 8, numRoundPadsTop = 8, numPadsTotal = 80;

  std::vector<unsigned char> padStatesByte = std::vector<unsigned char>(numPadsTotal);
  std::vector<PadState> padStates = std::vector<PadState>(numPadsTotal);

  std::vector<KeyState> padKeyStates = std::vector<KeyState>(80, KeyState::ReleasedInactive);

  std::vector<LaunchPadListener*> launchPadListeners;
  
  std::vector<bool> toggleButtonPads = std::vector<bool>(80, false);

  PadState defaultPressedActivePadState = {3, 0, false, true};
  PadState defaultReleasedActivePadState = {0, 3, false, true};
  PadState defaultPressedInactivePadState = {0, 1, false, true};
  PadState defaultReleasedInactivePadState = {0, 0, false, true};

  bool overridePressedActiveKeyState = false;
  bool overrideReleasedActiveKeyState = false;
  bool overridePressedInactiveKeyState = false;
  bool overrideReleasedInactiveKeyState = false;
  
  void setOverrideKeystates(bool overridePressedActiveKeyState = false,
  bool overrideReleasedActiveKeyState = false,
  bool overridePressedInactiveKeyState = false,
  bool overrideReleasedInactiveKeyState = false) {
    this->overridePressedActiveKeyState = overridePressedActiveKeyState;
    this->overrideReleasedActiveKeyState = overrideReleasedActiveKeyState;
    this->overridePressedInactiveKeyState = overridePressedInactiveKeyState;
    this->overrideReleasedInactiveKeyState = overrideReleasedInactiveKeyState;
  }

  bool padReseted = true;
  
  bool arePadsPressed(int indexA, int indexB = -1, int indexC = -1, int indexD = -1, int indexE = -1, int indexF = -1, int indexG = -1) {
    bool ret = true;
    if(indexA >= 0 && indexA < 80) {
      ret = ret && padKeyStates[indexA] == KeyState::PressedActive;
    }
    if(indexB >= 0 && indexB < 80) {
      ret = ret && padKeyStates[indexB] == KeyState::PressedActive;
    }
    if(indexC >= 0 && indexC < 80) {
      ret = ret && padKeyStates[indexC] == KeyState::PressedActive;
    }
    if(indexD >= 0 && indexD < 80) {
      ret = ret && padKeyStates[indexD] == KeyState::PressedActive;
    }
    if(indexE >= 0 && indexE < 80) {
      ret = ret && padKeyStates[indexE] == KeyState::PressedActive;
    }
    if(indexF >= 0 && indexF < 80) {
      ret = ret && padKeyStates[indexF] == KeyState::PressedActive;
    }
    if(indexG >= 0 && indexG < 80) {
      ret = ret && padKeyStates[indexG] == KeyState::PressedActive;
    }
    return ret;
  }
  
  bool isSquareGridReleased(int excludeIndex = -1) {
    for(int i=0; i<64; i++) {
      if(i == excludeIndex) continue;
      if(padKeyStates[i] == KeyState::PressedActive || padKeyStates[i] == KeyState::Holding) {
        return false;
      }
    }
    return true;
  }
  
  bool areRoundPadsReleased(int excludeIndex = -1) {
    for(int i=0; i<8; i++) {
      if(i == excludeIndex) continue;
      if(padKeyStates[64+i] == KeyState::PressedActive || padKeyStates[64+i] == KeyState::Holding) {
        return false;
      }
      if(padKeyStates[72+i] == KeyState::PressedActive || padKeyStates[72+i] == KeyState::Holding) {
        return false;
      }
    }
    return true;
  }
  
  bool arePadsPressedAndCancelRelease(int indexA, int indexB = -1, int indexC = -1, int indexD = -1, int indexE = -1, int indexF = -1, int indexG = -1) {
    bool ret = true;
    if(indexA >= 0 && indexA < 80) {
      ret = ret && (padKeyStates[indexA] == KeyState::PressedActive || padKeyStates[indexA] == KeyState::Holding);
    }
    if(indexB >= 0 && indexB < 80) {
      ret = ret && (padKeyStates[indexB] == KeyState::PressedActive || padKeyStates[indexB] == KeyState::Holding);
    }
    if(indexC >= 0 && indexC < 80) {
      ret = ret && (padKeyStates[indexC] == KeyState::PressedActive || padKeyStates[indexC] == KeyState::Holding);
    }
    if(indexD >= 0 && indexD < 80) {
      ret = ret && (padKeyStates[indexD] == KeyState::PressedActive || padKeyStates[indexD] == KeyState::Holding);
    }
    if(indexE >= 0 && indexE < 80) {
      ret = ret && (padKeyStates[indexE] == KeyState::PressedActive || padKeyStates[indexE] == KeyState::Holding);
    }
    if(indexF >= 0 && indexF < 80) {
      ret = ret && (padKeyStates[indexF] == KeyState::PressedActive || padKeyStates[indexF] == KeyState::Holding);
    }
    if(indexG >= 0 && indexG < 80) {
      ret = ret && (padKeyStates[indexG] == KeyState::PressedActive || padKeyStates[indexG] == KeyState::Holding);
    }
    if(ret) {
      if(indexA >= 0 && indexA < 80) {
        padKeyStates[indexA] = KeyState::Holding;
      }
      if(indexB >= 0 && indexB < 80) {
        padKeyStates[indexB] = KeyState::Holding;
      }
      if(indexC >= 0 && indexC < 80) {
        padKeyStates[indexC] = KeyState::Holding;
      }
      if(indexD >= 0 && indexD < 80) {
        padKeyStates[indexD] = KeyState::Holding;
      }
      if(indexE >= 0 && indexE < 80) {
        padKeyStates[indexE] = KeyState::Holding;
      }
      if(indexF >= 0 && indexF < 80) {
        padKeyStates[indexF] = KeyState::Holding;
      }
      if(indexG >= 0 && indexG < 80) {
        padKeyStates[indexG] = KeyState::Holding;
      }
    }
    return ret;
  }


  void setToggleButtons(bool squarePadAsToggle, bool roundRighPadsAsToggle, bool roundTopPadsAsToggle) {
    for(int i=0; i<numSquarePads; i++) {
      toggleButtonPads[i] = squarePadAsToggle;
    }
    for(int i=0; i<8; i++) {
      toggleButtonPads[64 + i] = roundRighPadsAsToggle;
      toggleButtonPads[72 + i] = roundTopPadsAsToggle;
    }
  }

  void init(MidiInterface *midiInterface) {
    this->midiInterface = midiInterface;
    resetPad();
  }

  virtual ~LaunchPadInterface() {}

  virtual void finish() {
    resetPad();
    for(int i=0; i<launchPadListeners.size(); i++) {
      delete launchPadListeners[i];
    }
  }


  void setPadState(int column, int row, unsigned char greenLevel, unsigned char redLevel, bool doubleBuffered = true, bool flashing = false) {
    if(column >= 0 && column < 8 && row >= 0 && row < 8) {
      padStates[row*8 + column].set(greenLevel, redLevel, flashing, doubleBuffered);
      padStatesByte[row * 8 + column] = padStates[row*8 + column].getStateAsByte();
      if(!doubleBuffered) {
        printBits(padStatesByte[row * 8 + column]);
        midiInterface->sendPortMessage(midiPortIndex, 144, row*16 + column, padStatesByte[row * 8 + column]);
        padReseted = false;
      }
    }
    else if(column == 8 && row >= 0 && row < 8) {
      padStates[64+row].set(greenLevel, redLevel, flashing, doubleBuffered);
      padStatesByte[64 + row] = padStates[64+row].getStateAsByte();
      if(!doubleBuffered) {
        midiInterface->sendPortMessage(midiPortIndex, 144, row*16 + column, padStatesByte[64 + row]);
        padReseted = false;
      }
    }
    else if(column >= 0 && column < 8 && row == 8) {
      padStates[72+column].set(greenLevel, redLevel, flashing, doubleBuffered);
      padStatesByte[72 + column] = padStates[72+column].getStateAsByte();
      if(!doubleBuffered) {
        midiInterface->sendPortMessage(midiPortIndex, 176, 104+column, padStatesByte[72 + column]);
        padReseted = false;
      }
    }
  }

  void fill(unsigned char greenLevel, unsigned char redLevel, bool flashing, bool doubleBuffered, PadAccessMode mode = ALL_PADS) {
    if(mode == ALL_PADS || mode == SQUARE_PADS) {
      for(int i=0; i<numSquarePads; i++) {
        padStates[i].set(greenLevel, redLevel, flashing, doubleBuffered);
        padStatesByte[i] = padStates[i].getStateAsByte();
        int row = i / 8;
        int column = i % 8;
        if(!doubleBuffered) {
          midiInterface->sendPortMessage(midiPortIndex, 144, row*16 + column, padStatesByte[i]);
          padReseted = false;
        }
      }
    }
    if(mode == ALL_PADS || mode == ROUND_PADS || mode == ROUND_PADS_RIGHT) {
      for(int i=0; i<numRoundPadsRight; i++) {
        padStates[64 + i].set(greenLevel, redLevel, flashing, doubleBuffered);
        padStatesByte[64 + i] = padStates[64 + i].getStateAsByte();
        int row = i / 8;
        if(!doubleBuffered) {
          midiInterface->sendPortMessage(midiPortIndex, 144, row*16 + 8, padStatesByte[64 + i]);
          padReseted = false;
        }
      }
    }
    if(mode == ALL_PADS || mode == ROUND_PADS || mode == ROUND_PADS_TOP) {
      for(int i=0; i<numRoundPadsTop; i++) {
        padStates[72 + i].set(greenLevel, redLevel, flashing, doubleBuffered);
        padStatesByte[72 + i] = padStates[72 + i].getStateAsByte();
        if(!doubleBuffered) {
          midiInterface->sendPortMessage(midiPortIndex, 176, 104+i, padStatesByte[72 + i]);
          padReseted = false;
        }
      }
    }
  }
  
  void addLaunchPadListener(LaunchPadListener *launchPadListener) {
    launchPadListeners.push_back(launchPadListener);
  }
  
  void onActionBase(int x, int y, bool pressed) {
    KeyState keyState;
    int index = -1;
    
    if(x >= 0 && x < 8 && y >= 0 && y < 8) {
      index = x + y*8;
    }
    else if(x == 8 && y >= 0 && y < 8) {
      index = 64 + y;
    }
    else if(y == 8 && x >= 0 && x < 8) {
      index = 72 + x;
    }
    else return;
    
    if(pressed) {
      if(!toggleButtonPads[index] || padKeyStates[index] != ReleasedActive) {
        keyState = PressedActive;
      }
      else if(padKeyStates[index] != ReleasedInactive) {
        keyState = PressedInactive;
      }
    }
    else {
      if(!toggleButtonPads[index] || padKeyStates[index] != PressedActive) {
        if(padKeyStates[index] == ReleasedInactive) return;
        if(padKeyStates[index] == Holding) {
          padKeyStates[index] = ReleasedInactive;
          return;
        }
        keyState = ReleasedInactive;
      }
      else if(padKeyStates[index] != PressedInactive) {
        keyState = ReleasedActive;
      }
    }
    padKeyStates[index] = keyState;
    
    //printf(">> %d, %d, %d\n", x, y, keyState);
    
    this->onAction(x, y, keyState);
    
    for(int i=0; i<launchPadListeners.size(); i++) {
      launchPadListeners[i]->onAction(x, y, keyState);
    }
  }
  
  virtual void onAction(int x, int y, KeyState keyState) {}

  // TODO I think the midi interface should dedicate the whole channel range for each Midi device, multiple interfaces thus needed
  // DONE finally!
  

  /*void readMidiMessages() {
    MidiInterface::MidiPort *midiPort = NULL;
    if(midiPortIndex >= 0 && midiPortIndex < midiInterface->midiPorts.size() && midiInterface->midiPorts[midiPortIndex].type == MidiInterface::MidiPort::Type::LaunchPad) {
      midiPort = &midiInterface->midiPorts[midiPortIndex];
    }
    if(!midiPort) return;
    
    for(int i=0; i<midiPort->midiMessages.size(); i++) {
      //if(midiPort->midiMessages->at(i)->getMessageChannel() != 0) continue;
      
      int x = -1, y = -1;
      //printf("LP message %d: ", i); midiPort->midiMessages->at(i)->print();
      bool isKeyOn = midiPort->midiMessages->at(i)->getMessageType() == MidiMessage::NoteOn;
      bool isKeyOff = midiPort->midiMessages->at(i)->getMessageType() == MidiMessage::NoteOff;
      if(isKeyOn || isKeyOff) {
        x = midiPort->midiMessages->at(i)->dataByte1 & 15;
        y = midiPort->midiMessages->at(i)->dataByte1 / 16;
        if(x < 0 || x >= 9 || y < 0 || y >= 8) {
          printf("Warning at LaunchPadInterface::readMidiMessages(), grid button out of range\n");
        }
        else {
          onActionBase(x, y, isKeyOn);
        }
      }
      bool isLiveButton = midiPort->midiMessages->at(i)->getMessageType() == MidiMessage::ControlChange;
      if(isLiveButton) {
        x = midiPort->midiMessages->at(i)->dataByte1 - 104;
        y = 8;
        if(x < 0 || x >= 8) {
          printf("Warning at LaunchPadInterface::readMidiMessages(), live button out of range\n");
        }
        else {
          onActionBase(x, y, midiPort->midiMessages->at(i)->dataByte2 > 0);
        }
      }
    }
  }*/

  void renderPad(GeomRenderer &geomRenderer, double x, double y, double w, double h, double time) {
    PadState *padState = NULL;
    KeyState *keyState = NULL;
    
    geomRenderer.strokeColor.set(0, 0, 0, 1);
    geomRenderer.fillColor.set(0, 0, 0, 0.5);
    
    geomRenderer.drawRect(w, h, x, y);
    
    double cw = w / 9.0;
    double ch = h / 9.0;
    double pw = cw*0.9;
    double ph = cw*0.9;
    double pr = min(pw, ph) * 0.5;
    double x0 = x - w*0.5;
    double y0 = y - h*0.5;
    
    for(int i=0; i<9; i++) {
      for(int j=0; j<9; j++) {
        if(i == 8 && j == 8) continue;
        bool square = false;
        if(i < 8 && j < 8) {
          padState = &padStates[i + j*8];
          keyState = &padKeyStates[i + j*8];
          square = true;
        }
        else if(i == 8) {
          padState = &padStates[64 + j];
          keyState = &padKeyStates[64 + j];
        }
        else if(j == 8) {
          padState = &padStates[72 + i];
          keyState = &padKeyStates[72 + i];
        }
        
        geomRenderer.strokeWidth = 1.0;
        
        int x = i;
        int y = j < 8 ? j+1 : 0;
        
        if(*keyState == KeyState::PressedActive) {
          if(overridePressedActiveKeyState) padState = &defaultPressedActivePadState;
          
          geomRenderer.strokeWidth = 2.0;
          geomRenderer.strokeColor.set(1, 1, 1, 1);
        }
        if(*keyState == KeyState::PressedInactive) {
          if(overridePressedInactiveKeyState) padState = &defaultPressedInactivePadState;
          
          geomRenderer.strokeColor.set(1, 1, 1, 1);
        }
        if(*keyState == KeyState::ReleasedActive) {
          if(overrideReleasedActiveKeyState) padState = &defaultReleasedActivePadState;
          
          geomRenderer.strokeColor.set(1, 0.2, 0.2, 1);
        }
        if(*keyState == KeyState::ReleasedInactive) {
          if(overrideReleasedInactiveKeyState) padState = &defaultReleasedInactivePadState;
          
          geomRenderer.strokeColor.set(0, 0, 0, 1);
        }
        
        if(padState->flashing && sin(time*2.0*PI*7) < 0) {
          geomRenderer.fillColor.set(0.2, 0.2, 0.2, 0.8);
        }
        else {
          geomRenderer.fillColor.set(0.2 + 0.8 * padState->redLevel / 3.0, 0.2 + 0.8 * padState->greenLevel / 3.0, 0.2, 0.8);
        }
        
        if(square) {
          geomRenderer.drawRect(pw, ph, x0 + cw*0.5 + cw*x + (cw-pw)*0.5, y0 + ch*0.5 + ch*y + (ch-ph)*0.5);
        }
        else {
          geomRenderer.drawCircle(pr, x0 + cw*0.5 + cw*x + (cw-pr*2.0)*0.5, y0 + ch*0.5 + ch*y + (ch-pr*2.0)*0.5);
        }
        if(!padState->doubleBuffered) {
          geomRenderer.strokeWidth = 1.0;
          geomRenderer.strokeColor.set(0, 0, 0, 1);
          geomRenderer.drawLine(cw*x, ch*y, (cw+1)*x, (ch+1)*y);
          geomRenderer.drawLine(cw*x, (ch+1)*y, (cw+1)*x, ch*y);
        }
      }
    }
  }

  void updatePad() {
    setAllPadStates();
    flipPadBuffers();
  }

  void setAllPadStates() {
    if(midiPortIndex < 0) return;
    
    for(int i=0; i<numPadsTotal; i+=2) {
      unsigned char padStateA = padStatesByte[i], padStateB = padStatesByte[i+1];
      
      if(overridePressedActiveKeyState && padKeyStates[i] == KeyState::PressedActive) {
        padStateA = defaultPressedActivePadState.getStateAsByte();
      }
      else if(overridePressedInactiveKeyState && padKeyStates[i] == KeyState::PressedInactive) {
        padStateA = defaultPressedInactivePadState.getStateAsByte();
      }
      else if(overrideReleasedActiveKeyState && padKeyStates[i] == KeyState::ReleasedActive) {
        padStateA = defaultReleasedActivePadState.getStateAsByte();
      }
      else if(overrideReleasedInactiveKeyState && padKeyStates[i] == KeyState::ReleasedInactive) {
        padStateA = defaultReleasedInactivePadState.getStateAsByte();
      }

      if(overridePressedActiveKeyState && padKeyStates[i+1] == KeyState::PressedActive) {
        padStateB = defaultPressedActivePadState.getStateAsByte();
      }
      else if(overridePressedInactiveKeyState && padKeyStates[i+1] == KeyState::PressedInactive) {
        padStateB = defaultPressedInactivePadState.getStateAsByte();
      }
      else if(overrideReleasedActiveKeyState && padKeyStates[i+1] == KeyState::ReleasedActive) {
        padStateB = defaultReleasedActivePadState.getStateAsByte();
      }
      else if(overrideReleasedInactiveKeyState && padKeyStates[i+1] == KeyState::ReleasedInactive) {
        padStateB = defaultReleasedInactivePadState.getStateAsByte();
      }
      
      midiInterface->sendPortMessage(midiPortIndex, 146, padStateA, padStateB);
    }
    padReseted = false;
  }

  void resetPad() {
    if(midiPortIndex < 0) return;
    
    midiInterface->sendPortMessage(midiPortIndex, 176, 0, 0);
    padReseted = true;
    for(int i=0; i<numPadsTotal; i++) {
      padKeyStates[i] = KeyState::ReleasedInactive;
      padStates[i].set(0, 0, true, false);
      padStatesByte[i] = padStates[i].getStateAsByte();
    }
  }

  void resetPadAndSetLeds(int brightness) {
    if(midiPortIndex < 0) return;
    
    unsigned char dataByte2 = 0;
    if(brightness == 1) dataByte2 = 125;
    if(brightness == 2) dataByte2 = 126;
    if(brightness == 3) dataByte2 = 127;
    midiInterface->sendPortMessage(midiPortIndex, 176, 0, dataByte2);
    for(int i=0; i<padKeyStates.size(); i++) {
      padKeyStates[i] = KeyState::ReleasedInactive;
    }
    padReseted = true;
  }
  int dutyCycleNumerator = 1, dutyCycleDenominator = 5;


  void setDutyCycle(double dutyCycle) {
    int numerator, denominator;
    if(dutyCycle <= 1.0/18.0) {
      numerator = 1;
      denominator = 18;
    }
    else if(dutyCycle >= 16.0/3.0) {
      numerator = 16;
      denominator = 3;
    }
    else {
      // TODO find out more elegant way
      double closestDeviation = 1e10;
      for(int i=1; i<=16; i++) {
        for(int j=3; j<=18; j++) {
          double t = (double)i/j;
          if(t < closestDeviation) {
            numerator = i;
            denominator = j;
            closestDeviation = fabs(dutyCycle-t);
          }
        }
      }
    }
    this->dutyCycleNumerator = numerator;
    this->dutyCycleDenominator = denominator;
    setDutyCycle(numerator, denominator);
  }

  void setDutyCycle(int numerator, int denominator) {
    if(midiPortIndex < 0) return;
    this->dutyCycleNumerator = numerator;
    this->dutyCycleDenominator = denominator;

    unsigned char dataByte1 = 0, dataByte2 = 0;

    if(numerator < 9) {
      dataByte1 = 30;
      dataByte2 = 16 * (numerator-1) + denominator-3;
    }
    else {
      dataByte1 = 31;
      dataByte2 = 16 * (numerator-9) + denominator-3;
    }

    midiInterface->sendPortMessage(midiPortIndex, 176, dataByte1, dataByte2);
  }


  int bufferState = 0;
  bool isFlashingPads = false;

  void flipPadBuffers() {
    if(midiPortIndex < 0) return;
    unsigned char dataByte2 = 1<<4 | 1<<1 | (bufferState & 1);
    midiInterface->sendPortMessage(midiPortIndex, 176, 0, dataByte2);
    bufferState = 1 - bufferState;
  }

  void setFlashingPads(bool isFlashingPads) {
    if(midiPortIndex < 0) return;
    this->isFlashingPads = isFlashingPads;
    unsigned char dataByte2 = 1<<4 | (isFlashingPads ? 1<<2 : 0);
    midiInterface->sendPortMessage(midiPortIndex, 176, 0, dataByte2);
  }

  void copyPadBuffers() {
    if(midiPortIndex < 0) return;
    unsigned char dataByte2 = 1<<4 | 1 <<3;
    midiInterface->sendPortMessage(midiPortIndex, 176, 0, dataByte2);
  }

  void copyAndFlipPadBuffers() {
    if(midiPortIndex < 0) return;
    unsigned char dataByte2 = 1<<4 | 1<<3 | 1<<1 | (bufferState & 1);
    midiInterface->sendPortMessage(midiPortIndex, 176, 0, dataByte2);
    bufferState = 1 - bufferState;
  }


  struct LaunchPadMidiListener : public MidiMessageListener {
    LaunchPadInterface *launchpad;
    LaunchPadMidiListener(LaunchPadInterface *launchpad) {
      this->launchpad = launchpad;
    }
    virtual void onMessage(const MidiMessage &midiMessage, MidiPortType midiPortType) {
      printf("LaunchPadMidiListener::onMessages(), beginning\n");

      if(midiPortType == MidiPortType::LaunchPad) {
        int x = -1, y = -1;

        bool isKeyOn = midiMessage.getMessageType() == MidiMessage::NoteOn;
        bool isKeyOff = midiMessage.getMessageType() == MidiMessage::NoteOff;
        if(isKeyOn || isKeyOff) {
          x = midiMessage.dataByte1 & 15;
          y = midiMessage.dataByte1 / 16;
          if(x < 0 || x >= 9 || y < 0 || y >= 8) {
            printf("Warning at LaunchPadInterface::LaunchPadMidiListener::onMessages(), grid button out of range\n");
          }
          else {
            launchpad->onActionBase(x, y, isKeyOn);
          }
        }
        bool isLiveButton = midiMessage.getMessageType() == MidiMessage::ControlChange;
        if(isLiveButton) {
          x = midiMessage.dataByte1 - 104;
          y = 8;
          if(x < 0 || x >= 8) {
            printf("Warning at LaunchPadInterface::LaunchPadMidiListener::onMessages(), live button out of range\n");
          }
          else {
            launchpad->onActionBase(x, y, midiMessage.dataByte2 > 0);
          }
        }
      }
      printf("LaunchPadMidiListener::onMessages(), end\n");
    }
  };
  LaunchPadMidiListener *launchPadMidiListener = NULL;
  MidiMessageListener *getMidiMessageListener() {
    return launchPadMidiListener ? launchPadMidiListener : launchPadMidiListener = new LaunchPadMidiListener(this);
  }
};







struct LaunchpadMode {
  LaunchPadInterface *launchPad = NULL;
  GuiElement *parentGuiElement = NULL;
  std::string name;
  
  virtual ~LaunchpadMode() {}
  
  LaunchpadMode(LaunchPadInterface *launchPad, const std::string &name, GuiElement *parentGuiElement = NULL) {
    this->launchPad = launchPad;
    this->name = name;
    this->parentGuiElement = parentGuiElement;
  }
  
  virtual void onInit() {}
  virtual void onUpdateGrid(double time, double dt) {}
  virtual void onAction(int x, int y, LaunchPadInterface::KeyState keyState) {}
  virtual void onOutputActivated(bool value) {
    if(value) {
      launchPad->setOverrideKeystates(true, true, true, true);
      launchPad->resetPad();
    }
  }
  virtual void onInputActivated(bool value) {}
  
  virtual void printControls() {}
};








