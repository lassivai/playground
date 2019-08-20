#pragma once

#include <vector>
#include <cstdio>

struct MineSweeper {
  int w, h;
  int numMines;
  std::vector<int> tiles;
  std::vector<bool> openedTiles;
  std::vector<bool> markedTiles;
  std::vector<int> mineLocations;
  int numOpenedTiles = 0;
  
  bool gameOver = false;
  bool gameWon = false;
  
  void init(int w, int h, int numMines) {
    this->w = w;
    this->h = h;
    this->numMines = min(numMines, w*h/2);
    tiles.assign(w*h, 0);
    openedTiles.assign(w*h, false);
    markedTiles.assign(w*h, false);
    mineLocations.assign(this->numMines, 0);
    
    for(int i=0; i<this->numMines; i++) {
      bool done = false;
      while(!done) {
        done = true;
        mineLocations[i] = Random::getInt(0, w*h);
        for(int k=0; k<i; k++) {
          if(mineLocations[i] == mineLocations[k]) {
            done = false;
            break;
          }
        }
      }
      tiles[mineLocations[i]] = -1;
    }
    for(int i=0; i<tiles.size(); i++) {
      if(tiles[i] == -1) continue;
      int numMinesAround = 0;
      for(int j=0; j<3; j++) {
        int x = i%w + j - 1;
        if(x < 0 || x >= w) continue;
        
        for(int k=0; k<3; k++) {
          int y = i/w + k - 1;
          if(y < 0 || y >= h) continue;
          if(x == 0 && y == 0) continue;
          
          if(tiles[x + y*w] == -1) {
            numMinesAround++;
          }
        }
      }
      tiles[i] = numMinesAround;
    }
    gameOver = false;
    gameWon = false;
    numOpenedTiles = 0;
  }
  
  void reset() {
    init(w, h, numMines);
  }
  
  void markTile(int x, int y) {
    if(x < 0 || x >= w || y < 0 || y >= h) {
      printf("Error at MineSweeper::markTile(), invalid coordinate %d %d (%d x %d)\n", x, y, w, h);
      return;
    }
    markedTiles[x + y*w] = !markedTiles[x + y*w];
  }
  
  void openTile(int x, int y) {
    if(x < 0 || x >= w || y < 0 || y >= h) {
      printf("Error at MineSweeper::openTile(), invalid coordinate %d %d (%d x %d)\n", x, y, w, h);
      return;
    }
    if(!openedTiles[x + y*w]) {
      openedTiles[x + y*w] = true;
      numOpenedTiles++;
      
      if(tiles[x + y*w] < 0) {
        gameOver = true;
      }
      else if(tiles[x + y*w] == 0) {
        openAdjacentTiles(x, y);
      }
    }
    if(numOpenedTiles + numMines == w*h && !gameOver) {
      gameWon = true;
    }
  }
  
  void openAdjacentTiles(int x, int y) {
    if(x-1 >= 0 && !openedTiles[x-1 + y*w]) {
      openedTiles[x-1 + y*w] = true;
      numOpenedTiles++;
      if(tiles[x-1 + y*w] == 0) {
        openAdjacentTiles(x-1, y);
      }
    }
    if(x+1 < w && !openedTiles[x+1 + y*w]) {
      openedTiles[x+1 + y*w] = true;
      numOpenedTiles++;
      if(tiles[x+1 + y*w] == 0) {
        openAdjacentTiles(x+1, y);
      }
    }
    if(y-1 >= 0 && !openedTiles[x + (y-1)*w]) {
      openedTiles[x + (y-1)*w] = true;
      numOpenedTiles++;
      if(tiles[x + (y-1)*w] == 0) {
        openAdjacentTiles(x, y-1);
      }
    }
    if(y+1 < h && !openedTiles[x + (y+1)*w]) {
      openedTiles[x + (y+1)*w] = true;
      numOpenedTiles++;
      if(tiles[x + (y+1)*w] == 0) {
        openAdjacentTiles(x, y+1);
      }
    }
  }
  
  bool isNeghbourhoodCleared(int index) {
    for(int j=0; j<3; j++) {
      int x = index%w + j - 1;
      if(x < 0 || x >= w) continue;
      
      for(int k=0; k<3; k++) {
        int y = index/w + k - 1;
        if(y < 0 || y >= h) continue;
        if(x == 0 && y == 0) continue;
        
        if(!openedTiles[x + y*w]) {
          return false;
        }
      }
    }
    return true;
  }
  
};
