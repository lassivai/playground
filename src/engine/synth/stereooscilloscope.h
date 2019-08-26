#pragma once


struct StereoOscilloscope {
  
  struct StereoOscilloscopePanel : public Panel {
    StereoOscilloscope *stereoOscilloscope = NULL;
    virtual ~StereoOscilloscopePanel() {}
    StereoOscilloscopePanel(StereoOscilloscope *stereoOscilloscope) : Panel("Stereo oscilloscope panel") {
      this->stereoOscilloscope = stereoOscilloscope;
      setSize(80, 80);
      //addGuiEventListener(new DraggableGraphPanelListener(this));
    }
    virtual void onRender(GeomRenderer &geomRenderer, TextGl &textRenderer) {
      //if(stereoOscilloscope->displayMode != StereoOscilloscope::DisplayMode::Panel) return;
      if(!isVisible) return;
      
      int currentPhase = stereoOscilloscope->trailPosition;
      Vec2d mp(absolutePos.x + size.x/2, absolutePos.y + size.y/2);
      geomRenderer.strokeColor.set(1, 1, 1, 0.7);
      geomRenderer.strokeWidth = 1;
      geomRenderer.strokeType = 1;
      
      double maxDistanceSquared = 0;
      for(int i=0; i<stereoOscilloscope->trailLength; i++) {
        Vec2d p = stereoOscilloscope->trail[(stereoOscilloscope->maxTrailLength+currentPhase-i-1) % stereoOscilloscope->maxTrailLength];
        maxDistanceSquared = max(maxDistanceSquared, p.x*p.x + p.y*p.y);
      }
      double d = maxDistanceSquared > 0.00000001 ? sqrt(maxDistanceSquared) : 1;
      
      double scaling = min(size.x-10, size.y-10) * 0.5 / d;
      
      geomRenderer.startRendering(GeomRenderer::RendererType::FastStrokeRenderer);
      
      for(int i=1; i<stereoOscilloscope->trailLength; i++) {
        geomRenderer.drawLine(mp + stereoOscilloscope->trail[(stereoOscilloscope->maxTrailLength+currentPhase-i-1) % stereoOscilloscope->maxTrailLength] * scaling,
                              mp + stereoOscilloscope->trail[(stereoOscilloscope->maxTrailLength+currentPhase-i) % stereoOscilloscope->maxTrailLength] * scaling);
      }
      geomRenderer.endRendering();
    }
  };
  
  
  std::vector<Vec2d> trail;
  int maxTrailLength = 44100;
  int trailLength = 1000, trailPosition = 0;
  double scaling = 1;
  bool normalizeSize = false;
  
  enum DisplayMode { Nothing, Background, Panel };
  DisplayMode displayMode = Background;
  
  StereoOscilloscopePanel *stereoOscilloscopePanel = NULL;
  
  StereoOscilloscope() {
    trail.resize(maxTrailLength);
  }
  
  StereoOscilloscopePanel *getPanel() {
    return stereoOscilloscopePanel ? stereoOscilloscopePanel : stereoOscilloscopePanel = new StereoOscilloscopePanel(this);
  }
  
  void update(const Vec2d &sample) {
    trail[trailPosition].set(sample.x - sample.y, sample.x + sample.y);
    trailPosition = (trailPosition+1) % maxTrailLength;
  }
  
  void render(GeomRenderer &geomRenderer, double w, double h, bool isDarkGraphColors, double volumeLevel = 1) {
    if(displayMode == Background) {
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
  }
};