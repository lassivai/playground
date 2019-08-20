#pragma once
#include "colors.h"
#include "gfxwrap.h"
#include "util.h"

struct ColorMap
{
  std::vector<Vec3d> colors;
  std::vector<double> positions;

  int lookUpTableSize = 1000;
  std::vector<Vec3d> lookUpTable;
  bool lookUpTableReady = false;

  Texture colorBar;
  bool colorBarReady = false;

  ColorMap() {}

  ColorMap(const ColorMap &cmap) {
    this->lookUpTableSize = cmap.lookUpTableSize;
    this->lookUpTableReady = false;
    this->colorBarReady = false;
    this->colors = std::vector<Vec3d>(cmap.colors);
    this->positions = std::vector<double>(cmap.positions);
    //this->lookUpTable = std::vector<Vec3d>(cmap.lookUpTable);
  }

  void initPositions() {
    positions.clear();
    double r = 1.0 / (colors.size()-1);
    double t = 0;
    for(int i=0; i<colors.size(); i++) {
      positions.push_back(t);
      t += r;
    }
  }

  Vec3d getRGB(double v) {

    if(colors.size() == 0) {
      return Vec3d();
    }
    if(colors.size() == 1) {
      return colors[0];
    }

    v = clamp(v, 0.0, 0.99999);

    if(lookUpTableReady) {
      int ind = (int)clamp(v*(lookUpTableSize-1), 0.0, lookUpTable.size()-1.0);
      if(ind < 0 || ind >= lookUpTable.size()) printf("osagöl %d, %lu, %f, %f\n", ind, lookUpTable.size(), v*(lookUpTableSize-1), v);
      return lookUpTable[ind];
      //return Vec3d();
    }


    if(colors.size() == positions.size()) {
      if(v <= positions[0]) {
        return colors[0];
      }

      if(v >= positions[positions.size()-1]) {
        return colors[positions.size()-1];
      }

      int i = 0;

      while(i < positions.size()-1 && positions[i+1] < v) {
        i++;
      }

      double r = positions[i+1] - positions[i];
      v = (v - positions[i]) / r;
      i = clamp(i, 0, (int)colors.size()-2);
      return colors[i+1] * v + colors[i] * (1-v);
    }

    else {
      int t = int(v * (colors.size()-1));
      double r = 1.0/(colors.size()-1);
      v = (v - t*r) / r;
      t = clamp(t, 0, (int)colors.size()-2);
      return colors[t+1] * v + colors[t] * (1-v);
    }
  }

  void prepareLookUpTable() {
    lookUpTableSize = colors.size() * 1000;
    prepareLookUpTable(lookUpTableSize);
  }

  void prepareLookUpTable(int lookUpTableSize) {
    this->lookUpTableSize = lookUpTableSize;
    lookUpTable.clear();
    lookUpTable.resize(lookUpTableSize);
    lookUpTableReady = false;

    for(int i=0; i<lookUpTableSize; i++) {
      double v = (double)i/(lookUpTableSize-1.0);
      lookUpTable[i] = getRGB(v);
    }
    lookUpTableReady = true;
  }

  void prepareColorBar(int w, int h) {
    prepareColorBar(w, h, 1, Vec3d(0, 0, 0));
  }

  void prepareColorBar(int w, int h, int borderWidth, Vec3d borderColor) {
    colorBar.create(w, h);
    bool vertical = h > w;
    Vec3d color;
    for(int i=0; i<w; i++) {
      for(int j=0; j<h; j++) {
        if(i < borderWidth || j < borderWidth || w-i <= borderWidth || h-j <= borderWidth) {
          color = borderColor;
        }
        else {
          if(vertical) {
            //color = getRGB(map(h-1-j, borderWidth, h-1.0-borderWidth, 0.0, 1.0));
            color = getRGB(map(j, borderWidth, h-1.0-borderWidth, 0.0, 1.0));
          }
          else {
            color = getRGB(map(i, borderWidth, w-1.0-borderWidth, 0.0, 1.0));
          }
        }
        colorBar.setPixel(i, j, color);
      }
    }
    colorBar.applyPixels();
    colorBarReady = true;
  }

  static Vec3d getRGB(double v, std::vector<Vec3d> colors)
  {
    if(colors.size() == 0) {
      return Vec3d();
    }
    if(colors.size() == 1) {
      return colors[0];
    }

    v = clamp(v, 0.0, 0.99999);
    int t = int(v * (colors.size()-1));
    double r = 1.0/(colors.size()-1);
    v = (v - t*r) / r;
    return colors[t+1] * v + colors[t] * (1-v);
  }
};
