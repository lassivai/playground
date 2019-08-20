#pragma once
#include "../launchpad.h"
#include "../../games/minesweeper.h"


struct MineSweeperLaunchpadMode : public LaunchpadMode
{
  MineSweeper mineSweeper;
  int numMineSweeperPagesX = 1, numMineSweeperPagesY = 1;
  int mineSweeperPageLocationX = 0, mineSweeperPageLocationY = 0;
  int maxMineSweeperPageLocationX = 0, maxMineSweeperPageLocationY = 0;
  int numMineSweeperMinesPerPage = 8;
  double numberBlinkTime = 0.07;
  int maxNumberBlinked = 7;
  int blinkingCounter = 0;
  double blinkingTimer = 0;
  unsigned long frameCounter = 0;

  MineSweeperLaunchpadMode(LaunchPadInterface *launchPad) : LaunchpadMode(launchPad, "Minesweeper") {
    onInit();
  }

  void onInit() {
    mineSweeper.init(8 * numMineSweeperPagesX, 8 * numMineSweeperPagesY, numMineSweeperMinesPerPage * numMineSweeperPagesX*numMineSweeperPagesY);
    mineSweeperPageLocationX = 0;
    mineSweeperPageLocationY = 0;
    
    maxMineSweeperPageLocationX = 8 * (numMineSweeperPagesX-1);
    maxMineSweeperPageLocationY = 8 * (numMineSweeperPagesY-1);
    
    launchPad->resetPad();
  }

  void onUpdateGrid(double time, double dt) {
    frameCounter++;

    blinkingTimer += dt;
    if(blinkingTimer >= numberBlinkTime) {
      blinkingTimer = 0;
      blinkingCounter++;
      if(blinkingCounter >= maxNumberBlinked*2) {
        blinkingCounter = 0;
      }
    }
    bool flash = blinkingCounter % 2 == 1;
    
    for(int i=0; i<8; i++) {
      for(int j=0; j<8; j++) {
        int ind = mineSweeperPageLocationX + i + (mineSweeperPageLocationY + j) * mineSweeper.w;
        
        if(!mineSweeper.openedTiles[ind] && !mineSweeper.gameOver && !mineSweeper.gameWon) {
          if(mineSweeper.markedTiles[ind]) {
           launchPad->setPadState(i, j, 3, 0); 
          }
          else {
            launchPad->setPadState(i, j, 0, 0);
          }
        }
        else {
          if(mineSweeper.tiles[ind] == 0) {
            if(mineSweeper.gameOver) {
              launchPad->setPadState(i, j, 0, 1);
            }
            else if(mineSweeper.gameWon && (frameCounter/8) % 8 == j) {
              launchPad->setPadState(i, j, 3, 0);
            }
            else {
              launchPad->setPadState(i, j, 1, 0);
            }
          }
          if(mineSweeper.tiles[ind] > 0) { 
            if(flash && !(mineSweeper.tiles[ind] * 2 <= blinkingCounter)) {
              launchPad->setPadState(i, j, 3, 3);
            }
            else {
              launchPad->setPadState(i, j, 1, 1);
            }
          }
          if(mineSweeper.tiles[ind] < 0) {
            if(mineSweeper.gameOver || mineSweeper.gameWon) {
              launchPad->setPadState(i, j, 0, 3);
            }
            else if(mineSweeper.isNeghbourhoodCleared(ind)) {
              launchPad->setPadState(i, j, 1, 0);
            }
          }
        }
        
      }
    }
    for(int i=0; i<8; i++) {
      if(i == mineSweeperPageLocationX/8) {
        launchPad->setPadState(i, 8, 1, 1);
      }
      else if(i < numMineSweeperPagesX) {
        launchPad->setPadState(i, 8, 0, 1);
      }         
      else {
        if(mineSweeper.gameOver) launchPad->setPadState(i, 8, 0, 3);
        else if(mineSweeper.gameWon) launchPad->setPadState(i, 8, 3, 0);
        else launchPad->setPadState(i, 8, 0, 0);
      }
      if(i == mineSweeperPageLocationY/8) {
        launchPad->setPadState(8, i, 1, 1);
      }
      else if(i < numMineSweeperPagesY) {
        launchPad->setPadState(8, i, 0, 1);
      }
      else {
        if(mineSweeper.gameOver) launchPad->setPadState(8, i, 0, 3);
        else if(mineSweeper.gameWon) launchPad->setPadState(8, i, 3, 0);
        else launchPad->setPadState(8, i, 0, 0);
      }
    }
  }
  
  
  void onAction(int x, int y, LaunchPadInterface::KeyState keyState) {
    if((x == 8 || y == 8) && keyState == LaunchPadInterface::KeyState::ReleasedInactive) {
      if(x == 8 && y < numMineSweeperPagesY) {
        bool isAnotherKeyPressed = false;
        for(int i=0; i<8; i++) {
          if(i != y && launchPad->padKeyStates[64+i] == LaunchPadInterface::KeyState::PressedActive) {
            isAnotherKeyPressed = true;
            break;
          }
        }
        if(!isAnotherKeyPressed) {
          mineSweeperPageLocationY = y * 8;
        }
        else if(y*8 > mineSweeperPageLocationY && mineSweeperPageLocationY < maxMineSweeperPageLocationY) {
          mineSweeperPageLocationY++;
        }
        else if(y*8 < mineSweeperPageLocationY && mineSweeperPageLocationY > 0) {
          mineSweeperPageLocationY--;
        }
      }

      if(y == 8 && x < numMineSweeperPagesX) {
        bool isAnotherKeyPressed = false;
        for(int i=0; i<8; i++) {
          if(i != x && launchPad->padKeyStates[72+i] == LaunchPadInterface::KeyState::PressedActive) {
            isAnotherKeyPressed = true;
            break;
          }
        }
        if(!isAnotherKeyPressed) {
          mineSweeperPageLocationX = x * 8;
        }
        else if(x*8 > mineSweeperPageLocationX && mineSweeperPageLocationX < maxMineSweeperPageLocationX) {
          mineSweeperPageLocationX++;
        }
        else if(x*8 < mineSweeperPageLocationX && mineSweeperPageLocationX > 0) {
          mineSweeperPageLocationX--;
        }
      }

    }
  
    if(launchPad->arePadsPressed(76, 77, 78, 79)) {
      for(int i=0; i<64; i++) {
        if(launchPad->arePadsPressed(i)) {
          numMineSweeperMinesPerPage = i+1;
          break;
        }
      }
      onInit();
    }
    for(int i=0; i<8; i++) {
      for(int j=0; j<8; j++) {
        if(launchPad->arePadsPressed(64+i, 72+j) && x < 8 && y < 8 && keyState == LaunchPadInterface::KeyState::PressedActive) {
          numMineSweeperPagesX = j+1;
          numMineSweeperPagesY = i+1;
          onInit();
        }
      }
    }

    if(!mineSweeper.gameOver && !mineSweeper.gameWon && x < 8 && y < 8 && keyState == LaunchPadInterface::KeyState::ReleasedInactive) {
      bool isAnotherKeyPressed = false;
      for(int i=0; i<80; i++) {
        if(launchPad->padKeyStates[i] == LaunchPadInterface::KeyState::PressedActive) {
          isAnotherKeyPressed = true;
          break;
        }
      }
      if(isAnotherKeyPressed) {
        mineSweeper.markTile(mineSweeperPageLocationX + x, mineSweeperPageLocationY + y);
      }
      else {
        mineSweeper.openTile(mineSweeperPageLocationX + x, mineSweeperPageLocationY + y);
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

















