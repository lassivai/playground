#pragma once
#include "../engine/sketch.h"

struct Grain {
  Vec2d pos;
};

static const wchar_t *flameNames[51] = { L"none", L"linear", L"sinusoidal", L"spherical", L"swirl", L"horseshoe", L"polar", L"handkerchief", L"heart", L"disc", L"spiral", L"hyperbolic", L"diamond", L"ex", L"julia", L"bent", L"waves", L"fishey", L"popcorn", L"exponential", L"power", L"cosine", L"rings", L"fan", L"blob", L"pdj", L"fan2", L"rings2", L"eyefish", L"bubble", L"cylinder", L"perspective", L"noise", L"julian", L"juliascope", L"blur", L"gaussian", L"radialblur", L"pie", L"ngon", L"curl", L"rectangles", L"arch", L"tangent", L"square", L"rays", L"blade", L"secant", L"twintrian", L"cross", L"custom"};

static std::vector<std::string> flameStrNames = { "none", "linear", "sinusoidal", "spherical", "swir", "horseshoe", "polar", "handkerchief", "heart", "disc", "spiral", "hyperbolic", "diamond", "ex", "julia", "bent", "waves", "fishey", "popcorn", "exponential", "power", "cosine", "rings", "fan", "blob", "pdj", "fan2", "rings2", "eyefish", "bubble", "cylinder", "perspective", "noise", "julian", "juliascope", "blur", "gaussian", "radialblur", "pie", "ngon", "cur", "rectangles", "arch", "tangent", "square", "rays", "blade", "secant", "twintrian", "cross", "custom"};

static const int numFlamePars[50] = { 7, // 00 linear
                                      7, // 01 sinudoidal
                                      7, // 02 spherical
                                      7, // 03 swirl
                                      7, // 04 horseshoe
                                      7, // 05 polar
                                      7, // 06 handkerchief
                                      7, // 07 heart
                                      7, // 08 disc
                                      7, // 09 spiral
                                      7, // 10 hyperbolic
                                      7, // 11 diamond
                                      7, // 12 ex
                                      7, // 13 julia
                                      7, // 14 bent
                                      10, // 15 waves (b, c, f)
                                      7, // 16 fishey
                                      9, // 17 popcorn (c, f)
                                      7, // 18 exponential
                                      7, // 19 power
                                      7, // 20 cosine
                                      8, // 21 rings (c)
                                      9, // 22 fan (c, f)
                                      10, // 23 blob (p1, p2, p3)
                                      11, // 24 PDJ (p1, p2, p3, p4)
                                      9, // 25 fan2 (p1, p2)
                                      8, // 26 rings2 (p)
                                      7, // 27 eyefish
                                      7, // 28 bubble
                                      7, // 29 cylinder
                                      9, // 30 perspective (p1, p2)
                                      7, // 31 noise
                                      9, // 32 julian (p1, p2)
                                      9, // 33 juliascope (p1, p2)
                                      7, // 34 blur
                                      7, // 35 gaussian
                                      8, // 36 radialblur (p1)
                                      10, // 37 pie (p1, p2, p3)
                                      11, // 38 ngon (p1, p2, p3, p4)
                                      9, // 39 curl (p1, p2)
                                      9, // 40 rectangles (p1, p2)
                                      7, // 41 arch
                                      7, // 42 tangent
                                      7, // 43 square
                                      7, // 44 rays
                                      7, // 45 blade
                                      7, // 46 secant
                                      7, // 47 twintrian
                                      7, // 48 cross
                                      10
                                    };

struct FlameParameters {
  static const int numPars = 14;
  int variation = -1;
  double p[numPars];

  std::vector<double> p3d = std::vector<double>(numPars + 6);

  void reset() {
    variation = -1;
    for(int i=0; i<numPars; i++) {
      p[i] = 0;
    }
    for(int i=0; i<p3d.size(); i++) {
      p3d[i] = 0;
    }
  }

  int getNumParameters() {
    return variation < 0 ? 0 : numFlamePars[clamp(variation, 0, 49)];
  }

  std::string getVariationName() {
    return flameStrNames[clamp(variation+1, 0, flameStrNames.size()-1)];
  }

  const wchar_t *getVariationNameWchar() {
    return flameNames[clamp(variation+1, 0, flameStrNames.size()-1)];
  }

  template<class Archive>
  void serialize(Archive &ar, const std::uint32_t version) {
    if(version == 1) {
      ar(variation, p);
    }
    if(version == 2) {
      ar(variation, p, p3d);
    }
  }
};
CEREAL_CLASS_VERSION(FlameParameters, 2);

struct FractalParameters
{
  static const int numFlameVariations = 67;
  static const int numActiveVariations = 10;
  static const int numColorArgs = 3;
  //static const int numArgs = 14;

  Vec2d location;
  double scale = 1.0;
  double maxIter = 100.0, minIter = 20;
  double mandelbrotPower = 20;
  Vec2d juliaPoint;

  Vec3d location3d;
  Vec3d juliapPoint3d;
  Vec2d xRange, yRange, zRange;
  Vec3i voxelGridSize = Vec3i(60, 60, 60);

  FlameParameters flamePars[numActiveVariations];
  double colorArg[numColorArgs];


  bool useColorMap = false;
  ColorMap *colorMap;

  //double arg[numArgs];

  int mandelbrotMode = 0, numMandelbrotModes = 4;
  int postEffectMode = 0, numPostEffectModes = 3;
  int softwareRenderMode = 2, numSoftwareRenderModes = 4;

  bool frozenTime = false;
  int colorMode = 0, numColorModes = 3;
  int mappingMode = 0, numMappingModes = 2;
  int histogramMappingMode = 0, numHistogramMappingModes = 6;

  int maxIterationRoutePoints = 1000;
  int numIterationRoutePoints = 0;
  Vec2d iterationRoute[1000];
  bool showIterationRoute = false;
  bool updateIterationRoute = false, updateIterationRouteRequested = false;

  double frameDuration = 2;
  double interpolationCurvature = 0.5;

  int activeFlamePars = 0;

  bool viewChanged = true;

  unsigned long numSamples = 0, numSamplesFiltered = 0, numPixelsMapped = 0;

  int w = 1, h = 1;
  double aspectRatio = 1;

  unsigned int renderingStartedMillis = 0;
  unsigned int renderingEndedMillis = 0;


  FractalParameters() {}


  template<class Archive>
  void serialize(Archive &ar, const std::uint32_t version) {
    //printf("FractalParameters.serialize, version %d\n", version);
    if(version == 1) {
      ar(location, scale, maxIter, minIter,
         mandelbrotPower,
         juliaPoint,
         flamePars,
         colorArg,
         useColorMap,
         mandelbrotMode, numMandelbrotModes,
         postEffectMode, numPostEffectModes,
         softwareRenderMode, numSoftwareRenderModes,
         frozenTime,
         colorMode, numColorModes,
         mappingMode, numMappingModes,
         histogramMappingMode, numHistogramMappingModes,
         numIterationRoutePoints,
         showIterationRoute, updateIterationRoute, updateIterationRouteRequested,
         frameDuration,
         interpolationCurvature,
         activeFlamePars);
     }
     if(version == 2) {
       ar(location, scale, maxIter, minIter,
          mandelbrotPower,
          juliaPoint,
          flamePars,
          colorArg,
          useColorMap,
          mandelbrotMode, numMandelbrotModes,
          postEffectMode, numPostEffectModes,
          softwareRenderMode, numSoftwareRenderModes,
          frozenTime,
          colorMode, numColorModes,
          mappingMode, numMappingModes,
          histogramMappingMode, numHistogramMappingModes,
          numIterationRoutePoints,
          showIterationRoute, updateIterationRoute, updateIterationRouteRequested,
          frameDuration,
          interpolationCurvature,
          activeFlamePars,
          location3d, juliapPoint3d, voxelGridSize);
      }
  }


  template<class T>
  struct FructInputVariableBind : public Events::InputVariableBind<T> {
    FractalParameters *fractPars;
    FructInputVariableBind(FractalParameters *fractPars, T *variable, double *scale, int mode, T factor, int mouseButtons, int modifiers) :
      Events::InputVariableBind<T> (variable, scale, mode, factor, mouseButtons, modifiers) {
      this->fractPars = fractPars;
    }

    FructInputVariableBind(FractalParameters *fractPars, T *variable, double *scale, int dragMode, T factor, int releaseMode, int mouseButtons, int modifiers) :
      Events::InputVariableBind<T> (variable, scale, dragMode, factor, releaseMode, mouseButtons, modifiers) {
      this->fractPars = fractPars;
    }

    FructInputVariableBind(FractalParameters *fractPars, T *variable, int dragMode, T factor, int releaseMode, int mouseButtons, int modifiers) :
      Events::InputVariableBind<T> (variable, dragMode, factor, releaseMode, mouseButtons, modifiers) {
      this->fractPars = fractPars;
    }

    /*void onMouseWheel() { fractPars->viewChanged = true; }
    void onMouseMotion() { fractPars->viewChanged = true; }
    void onMousePressed() { fractPars->viewChanged = true; }
    void onMouseReleased() { fractPars->viewChanged = true; }*/
    
    // there was a mystery bug I couldn't fix, let's see... Previosly it crashed with this thing. Now it seems to be working.
    //  That the fractal isn't updated every time something is done with mouse, but only if a relevant parameter is changed
    virtual void onValueChanged() {
      fractPars->viewChanged = true;
    }
  };


  struct FlameInputVariableBind : public FructInputVariableBind<double> {
    int flameParInd;

    FlameInputVariableBind(FractalParameters *fractPars, int flameParInd, int mouseButtons, int modifiers) :
      FructInputVariableBind<double> (fractPars, NULL, &fractPars->scale, Events::dragModeExponential, 0.001, Events::releaseModeRoundNegate, mouseButtons, modifiers) {
      this->fractPars = fractPars;
      this->flameParInd = flameParInd;
    }

    void preAction() {
      if(fractPars->softwareRenderMode == 2) {
        variable = &fractPars->flamePars[fractPars->activeFlamePars].p[flameParInd];
      }
      if(fractPars->softwareRenderMode == 3) {
        variable = &fractPars->flamePars[fractPars->activeFlamePars].p3d[flameParInd];
      }
    }
  };

  struct ColorInputVariableBind : public FructInputVariableBind<double> {
    int postActionMode;
    std::string str;
    
    ColorInputVariableBind(std::string str, FractalParameters *fractPars, double *variable, int postActionMode, int mouseButtons, int modifiers) :
      FructInputVariableBind<double>(fractPars, variable, &fractPars->scale, Events::dragModeLinear, 0.001, Events::releaseModeRoundNegate, mouseButtons, modifiers) {
      this->postActionMode = postActionMode;
      this->str = str;
    }

    virtual void onValueChanged() {
      FructInputVariableBind<double>::onValueChanged();
      if(postActionMode == 0) {
        *variable = modff(*variable, -TAU, TAU);
      }
      else {
        *variable = clamp(*variable, 0.0, 1.0);
      }
    }
  };

  std::vector<Events::InputVariableBind<double>*> inputBoundDoubles;
  std::vector<Events::InputVariableBind<Vec2d>*> inputBoundVec2ds;
  bool isInputActive = true;

  void setInputActive(bool isActive) {
    this->isInputActive = isActive;
    for(auto &t : inputBoundDoubles) {
      t->active = isActive;
    }
    for(auto &t : inputBoundVec2ds) {
      t->active = isActive;
    }
  }

  /*int getNumFlameParameters(int variationIndex) {
    if(variationIndex < 0 || variationIndex >= 10) return 0;
    return flamePars[variationIndex].getNumParameters() + ();
  }*/

  bool isUnderMouseDragging() {
    bool ret = false;
    for(auto &t : inputBoundDoubles) {
      ret = (t->active && t->dragging) || ret;
    }
    for(auto &t : inputBoundVec2ds) {
      ret = (t->active && t->dragging) || ret;
    }
    return ret;
  }

  void init(Events &e, int w, int h) {
    this->w = w;
    this->h = h;
    aspectRatio = (double)w/h;

    for(int n=1; n<numActiveVariations; n++) {
      for(int i=1; i<numFlamePars[n]; i++) {
        flamePars[n].p[i] = Random::getDouble(0.5, 10);
      }
      flamePars[n].p[0] = 0.95;
    }
    flamePars[3].variation = 13;

    colorArg[0] = 0;
    colorArg[1] = 0;
    colorArg[2] = 0;

    inputBoundDoubles.push_back(new FructInputVariableBind<double>(this, &scale, Events::dragModeExponential, 0.02, Events::releaseModeToOne, Events::buttonRight, 0));

    inputBoundDoubles.push_back(new FructInputVariableBind<double>(this, &maxIter, Events::dragModeExponential, 0.01, Events::releaseModeToOne, Events::buttonMiddle, 0));

    inputBoundDoubles.push_back(new FructInputVariableBind<double>(this, &mandelbrotPower, Events::dragModeExponential, 0.0025, Events::releaseModeRoundNegate, Events::buttonLeft, Events::modifiersLControl));

    inputBoundVec2ds.push_back(new FructInputVariableBind<Vec2d>(this, &location, &scale, Events::dragModeLinear, Vec2d(-2.0/e.scene->screenH, 2.0/e.scene->screenH), Events::releaseModeToMouseNormCoords, Events::buttonLeft, 0));

    inputBoundVec2ds.push_back(new FructInputVariableBind<Vec2d>(this, &juliaPoint,  Events::dragModeToPointNormCoords, Vec2d(1,1 ), Events::releaseModeToZero, Events::buttonLeft, Events::modifiersLShift));


    int lc = Events::modifiersLControl, ls = Events::modifiersLShift, la = Events::modifiersLAlt;
    int rc = Events::modifiersRControl, rs = Events::modifiersRShift, ra = Events::modifiersRAlt;
    int bl = Events::buttonLeft, bm = Events::buttonMiddle, br = Events::buttonRight;

    inputBoundDoubles.push_back(new ColorInputVariableBind("color arg 0", this, &colorArg[0], 0, bl, lc | ls));
    inputBoundDoubles.push_back(new ColorInputVariableBind("color arg 1", this, &colorArg[1], 0, bl, lc | la));
    inputBoundDoubles.push_back(new ColorInputVariableBind("color arg 2", this, &colorArg[2], 1, bl, ls | la));

    inputBoundDoubles.push_back(new FlameInputVariableBind(this, 0, bm, lc | ls | la));
    inputBoundDoubles.push_back(new FlameInputVariableBind(this, 1, bm, lc));
    inputBoundDoubles.push_back(new FlameInputVariableBind(this, 2, bm, ls));
    inputBoundDoubles.push_back(new FlameInputVariableBind(this, 3, bm, la));
    inputBoundDoubles.push_back(new FlameInputVariableBind(this, 4, bm, lc | ls));
    inputBoundDoubles.push_back(new FlameInputVariableBind(this, 5, bm, lc | la));
    inputBoundDoubles.push_back(new FlameInputVariableBind(this, 6, bm, ls | la));
    inputBoundDoubles.push_back(new FlameInputVariableBind(this, 7, br, lc));
    inputBoundDoubles.push_back(new FlameInputVariableBind(this, 8, br, ls));
    inputBoundDoubles.push_back(new FlameInputVariableBind(this, 9, br, la));
    inputBoundDoubles.push_back(new FlameInputVariableBind(this, 10, br, lc | ls));
    inputBoundDoubles.push_back(new FlameInputVariableBind(this, 11, br, lc | la));
    inputBoundDoubles.push_back(new FlameInputVariableBind(this, 12, br, ls | la));
    inputBoundDoubles.push_back(new FlameInputVariableBind(this, 13, br, lc | ls | la));
    inputBoundDoubles.push_back(new FlameInputVariableBind(this, 14, bm, rc));
    inputBoundDoubles.push_back(new FlameInputVariableBind(this, 15, bm, rs));
    inputBoundDoubles.push_back(new FlameInputVariableBind(this, 16, bm, ra));
    inputBoundDoubles.push_back(new FlameInputVariableBind(this, 17, bm, rc | rs));
    inputBoundDoubles.push_back(new FlameInputVariableBind(this, 18, bm, rc | ra));
    inputBoundDoubles.push_back(new FlameInputVariableBind(this, 19, bm, rs | ra));


    e.inputBoundDoubles.insert(e.inputBoundDoubles.end(), inputBoundDoubles.begin(), inputBoundDoubles.end());
    e.inputBoundVec2ds.insert(e.inputBoundVec2ds.end(), inputBoundVec2ds.begin(), inputBoundVec2ds.end());


    colorMap = new ColorMap();
    colorMap->colors.push_back(Vec3d(0, 0, 0));
    colorMap->colors.push_back(Vec3d(0.5, 0, 0.15));


  }

  void reset() {
    location.set(0, 0);
    scale = 1.0;
    maxIter = 100.0;
    minIter = 20;
    mandelbrotPower = 20;
    juliaPoint.set(0, 0);

    for(int i=0; i<numActiveVariations; i++) {
      flamePars[i].reset();
    }

    for(int i=0; i<numColorArgs; i++) {
      colorArg[i] = 0;
    }

    useColorMap = false;

    mandelbrotMode = 0;
    numMandelbrotModes = 4;
    postEffectMode = 0;
    numPostEffectModes = 3;
    softwareRenderMode = 2;
    numSoftwareRenderModes = 3;

    frozenTime = false;
    colorMode = 0;
    numColorModes = 3;
    mappingMode = 0;
    numMappingModes = 2;
    histogramMappingMode = 0;
    numHistogramMappingModes = 6;

    maxIterationRoutePoints = 1000;
    numIterationRoutePoints = 0;
    for(int i=0; i<1000; i++) {
      iterationRoute[i].set(0, 0);
    }
    showIterationRoute = false;
    updateIterationRoute = false;
    updateIterationRouteRequested = false;

    frameDuration = 2;
    interpolationCurvature = 0.5;

    activeFlamePars = 0;

    viewChanged = true;

    numSamples = 0;
    numSamplesFiltered = 0;
    numPixelsMapped = 0;

    //w = 1, h = 1;
    //aspectRatio = 1;

    renderingStartedMillis = 0;
    renderingEndedMillis = 0;
  }


  void updateColorMap() {
    colorMap->prepareLookUpTable();
    colorMap->prepareColorBar(8, h/4);
  }

  void set(const FractalParameters &fractPars) {
    //memcpy(this, &fractPars, sizeof(FractalParameters));
    location = fractPars.location;
    scale = fractPars.scale;
    maxIter = fractPars.maxIter;
    minIter = fractPars.minIter;
    mandelbrotPower = fractPars.mandelbrotPower;
    juliaPoint = fractPars.juliaPoint;

    for(int i=0; i<numActiveVariations; i++) {
      flamePars[i] = fractPars.flamePars[i];
    }

    for(int i=0; i<numColorArgs; i++) {
      colorArg[i] = fractPars.colorArg[i];
    }

    useColorMap = fractPars.useColorMap;
    //colorMap = ColorMap(fractPars.colorMap);
    colorMap = fractPars.colorMap;

    mandelbrotMode = fractPars.mandelbrotMode;
    postEffectMode = fractPars.postEffectMode;
    softwareRenderMode = fractPars.softwareRenderMode;

    frozenTime = fractPars.frozenTime;
    colorMode = fractPars.colorMode;
    mappingMode = fractPars.mappingMode;
    histogramMappingMode = fractPars.histogramMappingMode;


    numIterationRoutePoints = fractPars.numIterationRoutePoints;
    for(int i=0; i<maxIterationRoutePoints; i++) {
      iterationRoute[i] = fractPars.iterationRoute[i];
    }
    showIterationRoute = fractPars.showIterationRoute;
    updateIterationRoute = fractPars.updateIterationRoute; updateIterationRouteRequested = fractPars.updateIterationRouteRequested;

    frameDuration = fractPars.frameDuration;
    activeFlamePars = fractPars.activeFlamePars;
    viewChanged = fractPars.viewChanged;

    numSamples = fractPars.numSamples;
    numSamplesFiltered = fractPars.numSamplesFiltered;
    numPixelsMapped = fractPars.numPixelsMapped;

     w = fractPars.w;
     h = fractPars.h;
    aspectRatio = fractPars.aspectRatio;

    renderingStartedMillis = fractPars.renderingStartedMillis;
    renderingEndedMillis = fractPars.renderingEndedMillis;

  }

  void set(const FractalParameters *fractPars) {
    set(*fractPars);
  }
  void setColors(const FractalParameters &fractPars) {
    for(int i=0; i<numColorArgs; i++) {
      colorArg[i] = fractPars.colorArg[i];
    }
    useColorMap = fractPars.useColorMap;
  }

  void setView(const FractalParameters &fractPars) {
    location = fractPars.location;
    scale = fractPars.scale;
  }

  FractalParameters *getCopy() {
    FractalParameters *fractPars = new FractalParameters();
    memcpy(fractPars, this, sizeof(FractalParameters));
    return fractPars;
  }



  Vec3d getColor(double v) {
    if(useColorMap) {
      return colorMap->getRGB(v);
    }
    else {
      double ha = colorArg[0];
      double hr = colorArg[1];
      double hs = colorArg[2];
      return hslToRgb(ha+v*hr, hs, v);
    }
  }

  inline Vec2d toPixel(Vec2d &v) {
    double px = map(v.x-location.x, -aspectRatio*scale, aspectRatio*scale, 0, w-1);
    double py = map(v.y+location.y, -scale, scale, 0, h-1);
    return Vec2d(px, py);
  }

  inline int getPixelIndex(Vec2d &v) {
    int px = int(map(v.x-location.x, -aspectRatio*scale, aspectRatio*scale, 0, w-1));
    if(px >= 0 && px < w) {
      int py = int(map(v.y+location.y, -scale, scale, 0, h-1));
      if(py >= 0 && py < h ) {
        return px + py*w;
      }
    }
    return -1;
  }

  inline int getVoxelIndex(Vec3d &v) {
    int px = int(map(v.x-location3d.x, xRange.x*scale, xRange.y*scale, 0, voxelGridSize.x-1));
    if(px >= 0 && px < voxelGridSize.x) {
      int py = int(map(v.y+location3d.y, yRange.x*scale, yRange.y*scale, 0, voxelGridSize.y-1));
      if(py >= 0 && py < voxelGridSize.y) {
        int pz = int(map(v.z+location3d.z, zRange.x*scale, zRange.y*scale, 0, voxelGridSize.z-1));
        if(pz >= 0 && pz < voxelGridSize.z) {
          return px + py*voxelGridSize.y + pz*voxelGridSize.y*voxelGridSize.z;
        }
      }
    }
    return -1;
  }

};
CEREAL_CLASS_VERSION(FractalParameters, 2);
