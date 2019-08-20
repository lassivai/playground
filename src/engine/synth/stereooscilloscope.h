#pragma once


struct StereoOscilloscope {
  std::vector<Vec2d> trail;
  int maxTrailLength = 44100;
  int trailLength = 1000, trailPosition = 0;
  double scaling = 1;
  bool normalizeSize = false;
  
  StereoOscilloscope() {
    trail.resize(maxTrailLength);
  }
  
  void update(const Vec2d &sample) {
    trail[trailPosition].set(sample.x - sample.y, sample.x + sample.y);
    trailPosition = (trailPosition+1) % maxTrailLength;
  }
  
  void render(GeomRenderer &geomRenderer, double w, double h, bool isDarkGraphColors, double volumeLevel = 1) {
    int currentPhase = trailPosition;
    Vec2d mp(w/2, h/2);
    if(isDarkGraphColors) geomRenderer.strokeColor.set(0, 0, 0, 1);
    else geomRenderer.strokeColor.set(1, 1, 1, 0.9);
    geomRenderer.strokeWidth = 2;
    geomRenderer.strokeType = 1;

    double scaling = 800 * 0.5 * this->scaling;

    if(normalizeSize) {
      if(volumeLevel > 0) {
        scaling /= volumeLevel;
      }
    }

    for(int i=1; i<trailLength; i++) {
      geomRenderer.drawLine(mp + trail[(maxTrailLength+currentPhase-i-1) % maxTrailLength] * scaling,
                            mp + trail[(maxTrailLength+currentPhase-i) % maxTrailLength] * scaling);
    }
  }
};