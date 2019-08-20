#pragma once
#include "../engine/sketch.h"
#include "fructs.h"
#include "flame.h"
#include "flame3d.h"
#include "anim.h"


struct FractalSketch : public Sketch
{
  const std::string fractalBasePath = "data/sketches/fractal/";
  const std::string fractalParametersBasePath = "data/sketches/fractal/fractalParameters/";
  const std::string animationParametersBasePath = "data/sketches/fractal/animationParameters/";
  const std::string fractalParametersFileExtension = ".frp";
  const std::string animationParametersFileExtension = ".fra";

  // FIXME
  /*GlScene glScene;
  Quad quad;*/
  //Quadx screenQ
  //Image renderTargetImg;
  Texture renderTargetImg;
  GlShader mandelbrotShader, synthShader;

  static const int numRenderThreadBufferImgs = 6;
  //Texture **renderThreadBufferImgs;
  // TODO a struct for the seperate arrays
  std::vector<Texture*> renderThreadBufferImgs;
  Texture renderThreadBufferImg;
  bool imgReady[numRenderThreadBufferImgs];
  bool imgRenderInProgress[numRenderThreadBufferImgs];
  bool imgRenderQuitRequested[numRenderThreadBufferImgs];
  int softwareRenderPreviewIndex = 3;
  int softwareRenderPreviewIndexChangeRequest = 0;

  static const int numRenderThreads2 = 5;
  bool renderInProgress2[numRenderThreads2];
  bool renderQuitRequested2[numRenderThreads2];

  unsigned long *renderThreadCombinedBuffer2 = NULL;

  Vec3d *renderThreadBuffer2 = NULL;

  unsigned long *tmpScreenBuffer = NULL;


  FractalParameters fractPars;
  FractalParameters fractParsTmp, fractParsTmpColors, fractParsTmpView;
  FractalAnim anim;

  bool renderBufferTransferred = false;

  bool useShader = false;

  bool showSynth = false;

  Synth *synth;



  int numGrains = 120000;
  Grain *dust = NULL;

  bool showInfo = true;

  //int fractPars.activeFlamePars = 0;

  bool blackText = false;

  Scene3D *scene3D = NULL;
  Vec3i requestedVoxelGridSize;
  std::vector<unsigned long> voxels;
  bool view3dActive = false;

  CommandTemplate saveFractParsCmdTmpl, loadFractParsCmdTmpl;
  CommandTemplate saveFractAnimCmdTmpl, loadFractAnimCmdTmpl;
  CommandTemplate resetAnimationSettingsCmdTmpl, clearAnimationCmdTmpl;

  CommandTemplate setVoxelGridSizeCmdTmpl;

  bool debugRendering = false;

  ~FractalSketch() {
    for(int i=0; i<numRenderThreadBufferImgs; i++) delete renderThreadBufferImgs[i];
    //delete [] renderThreadBufferImgs;

    delete [] renderThreadCombinedBuffer2;
    delete [] tmpScreenBuffer;
    //for(int i=0; i<numRenderThreadBufferImgs2; i++) delete renderThreadBufferImgs2[i];
    //delete [] renderThreadBufferImgs2;

    if(dust) delete [] dust;

  }

  void onInit() {

    saveFractParsCmdTmpl.addArgument("filename", STR_STRING);
    saveFractParsCmdTmpl.finishInitialization("saveFractalParameters");
    commandQueue.addCommandTemplate(&saveFractParsCmdTmpl);
    loadFractParsCmdTmpl.addArgument("filename", STR_STRING);
    loadFractParsCmdTmpl.finishInitialization("loadFractalParameters");
    commandQueue.addCommandTemplate(&loadFractParsCmdTmpl);

    saveFractAnimCmdTmpl.addArgument("filename", STR_STRING);
    saveFractAnimCmdTmpl.finishInitialization("saveAnimationParameters");
    commandQueue.addCommandTemplate(&saveFractAnimCmdTmpl);
    loadFractAnimCmdTmpl.addArgument("filename", STR_STRING);
    loadFractAnimCmdTmpl.finishInitialization("loadAnimationParameters");
    commandQueue.addCommandTemplate(&loadFractAnimCmdTmpl);


    setVoxelGridSizeCmdTmpl.addArgument("width", STR_INT, "60");
    setVoxelGridSizeCmdTmpl.addArgument("height", STR_INT, "60");
    setVoxelGridSizeCmdTmpl.addArgument("depth", STR_INT, "60");
    setVoxelGridSizeCmdTmpl.finishInitialization("setVoxelGridSize");
    commandQueue.addCommandTemplate(&setVoxelGridSizeCmdTmpl);

    resetAnimationSettingsCmdTmpl.finishInitialization("resetAnimationSettings");
    commandQueue.addCommandTemplate(&resetAnimationSettingsCmdTmpl);
    clearAnimationCmdTmpl.finishInitialization("clearAnimation");
    commandQueue.addCommandTemplate(&clearAnimationCmdTmpl);

    glSetup();


    //renderThreadBufferImgs = new Texture* [numRenderThreadBufferImgs];
    renderThreadBufferImg.create(screenW, screenH);
    renderThreadBufferImgs.resize(numRenderThreadBufferImgs);

    for(int i=0; i<numRenderThreadBufferImgs; i++) {
      int n = 1<<i;
      //renderThreadBufferImgs[i] = new Image(screenW/n, screenH/n, sdlInterface);
      renderThreadBufferImgs[i] = new Texture(screenW/n, screenH/n);
      imgReady[i] = false;
      imgRenderInProgress[i] = false;
      imgRenderQuitRequested[i] = false;
    }
    renderThreadCombinedBuffer2 = new unsigned long[screenW*screenH];
    tmpScreenBuffer = new unsigned long[screenW*screenH];

    renderThreadBuffer2 = new Vec3d[screenW*screenH];

    /*for(int i=0; i<numRenderThreadBufferImgs2; i++) {
      int n = 1<<i;
      renderThreadBufferImgs2[i] = new Image(screenW, screenH, sdlInterface);
      imgReady2[i] = false;
      renderInProgress2[i] = false;
      renderQuitRequested2[i] = false;
    }*/

    /*SDL_Surface *surf = SDL_LoadBMP("test.bmp");
    IMG_SavePNG(surf, "testing.png");
    SDL_FreeSurface(surf);*/

    /*for(int i=0; i<7; i++) {
      fractPars.arg[i] = 1.0;
    }*/
    synth = new Synth();
    synth->setup();
    delete paInterface;
    paInterface = synth;
    paInterface->stop();


    //glScene.create();
    mandelbrotShader.create("data/glsl/basic.vert", "data/glsl/fract.frag");
    synthShader.create("data/glsl/basic.vert", "data/glsl/simplesynth.frag");
    //quad.create(screenW, screenH);


    renderTargetImg.create(screenW, screenH);
    /*renderTargetImg.create(screenW, screenH, sdlInterface);
    unsigned int *pixels;
    renderTargetImg.accessPixels(&pixels);
    for(int i=0; i<renderTargetImg.w*renderTargetImg.h; i++) {
      double v = Random::getDouble(0.2, 0.8);
      pixels[i] = toRGBA(v, v, v, 1.0);
    }
    renderTargetImg.applyPixels();*/
    dust = new Grain[numGrains];
    for(int i=0; i<numGrains; i++) {
      dust[i].pos.randomize(0, screenW, 0, screenH);
    }

    fractPars.init(events, screenW, screenH);
    //fractPars.updateColorMap();
    fractParsTmp.set(fractPars);
    fractParsTmpColors.set(fractPars);
    fractParsTmpView.set(fractPars);


    //Vec2d dimp = text.print("testing 123", 10, 10, 10, 1, sdlInterface);
    //Vec2d dimd = text.getDimensions("testing 123", 10, 1, sdlInterface);

    //printf("Testing dimensions: %.2f x %.2f, %.2f x %.2f\n", dimp.x, dimp.y, dimd.x, dimd.y);


    //commandQueue.addCommand("loadFractalParameters test0002");

    // FIXME
    scene3D = new Scene3D(screenW, screenH);
    scene3D->voxelGrid.setSize(fractPars.voxelGridSize.x, fractPars.voxelGridSize.y, fractPars.voxelGridSize.z);
    voxels.resize(scene3D->voxelGrid.voxels.size());
    fractPars.xRange = scene3D->voxelGrid.xRange;
    fractPars.yRange = scene3D->voxelGrid.yRange;
    fractPars.zRange = scene3D->voxelGrid.zRange;
    requestedVoxelGridSize = fractPars.voxelGridSize;
  }


  void onQuit() {

  }

  void onKeyUp() {
    if(fractPars.softwareRenderMode == 3 && view3dActive) {
      scene3D->onKeyUp(events);
    }
  }


  void onKeyDown() {

    bool lc = events.lControlDown;
    bool ls = events.lShiftDown;
    bool la = events.lAltDown;

    if(events.sdlKeyCode == SDLK_F1 && events.noModifiers) {
      showInfo = !showInfo;
    }
    if(events.sdlKeyCode == SDLK_F4) {
      blackText = !blackText;
    }
    if(events.sdlKeyCode == SDLK_F2) {
      showSynth = !showSynth;
    }
    if(showSynth) {
      synth->onKeyDownLooperControls(events);
      synth->onKeyDownKeyboardPiano(events);
    }
    else {
      if(events.sdlKeyCode == SDLK_F3) {
        useShader = !useShader;
        fractPars.viewChanged = true;
      }

      if(events.sdlKeyCode == SDLK_c && events.noModifiers) {
        fractPars.colorMode = (fractPars.colorMode+1) % fractPars.numColorModes;
      }

      if(events.sdlKeyCode == SDLK_v && events.noModifiers) {
        fractPars.mandelbrotMode = (fractPars.mandelbrotMode+1) % fractPars.numMandelbrotModes;
        fractPars.viewChanged = true;
      }
      if(events.sdlKeyCode == SDLK_t && events.noModifiers) {
        fractPars.showIterationRoute = !fractPars.showIterationRoute;
        fractPars.updateIterationRouteRequested = fractPars.showIterationRoute;
      }

      if(events.sdlKeyCode == SDLK_c && lc && !ls && !la) {
        fractParsTmp.set(fractPars);
      }
      if(events.sdlKeyCode == SDLK_v && lc && !ls && !la) {
        fractPars.set(fractParsTmp);
        fractPars.viewChanged = true;
        //fractPars.updateColorMap();
      }
      if(events.sdlKeyCode == SDLK_c && lc && ls && !la) {
        fractParsTmpColors.setColors(fractPars);
      }
      if(events.sdlKeyCode == SDLK_v && lc && ls && !la) {
        fractPars.setColors(fractParsTmpColors);
        fractPars.updateColorMap();
      }
      if(events.sdlKeyCode == SDLK_c && lc && !ls && la) {
        fractParsTmpView.setColors(fractPars);
      }
      if(events.sdlKeyCode == SDLK_v && lc && !ls && la) {
        fractPars.setColors(fractParsTmpView);
        fractPars.viewChanged = true;
      }
      if(events.sdlKeyCode == SDLK_r && events.lShiftDown) {
        anim.interpolateColorsByRGB = !anim.interpolateColorsByRGB;
      }
      if(events.sdlKeyCode == SDLK_s && events.lShiftDown) {
        anim.saveRecordedFrames();
      }
      if(events.sdlKeyCode == SDLK_s && events.lControlDown && events.numModifiersDown == 1) {
        std::string typeStr("mandelbrot");
        if(!useShader && fractPars.softwareRenderMode == 2) {
          typeStr = "flame";
          for(int i=0; i<fractPars.numActiveVariations; i++) {
            if(fractPars.flamePars[i].variation >= 0) {
              typeStr += "-" + std::to_string(fractPars.flamePars[i].variation);
            }
          }
        }
        std::string filename = "output/images/" + typeStr + "_" + getTimeString() + ".png";
        renderThreadBufferImg.saveToFile(filename);
      }
      if(events.sdlKeyCode == SDLK_SPACE && events.noModifiers) {
        anim.togglePlay();
      }
      if(events.sdlKeyCode == SDLK_SPACE && events.lControlDown) {
        anim.startRecording(fractPars);
      }
      if(events.sdlKeyCode == SDLK_RETURN && events.noModifiers) {
        anim.addKeyFrame(fractPars);
      }
      if(events.sdlKeyCode == SDLK_RETURN && events.lControlDown) {
        anim.insertKeyFrame(fractPars);
      }
      if(events.sdlKeyCode == SDLK_BACKSPACE) {
        anim.removeKeyFrame(fractPars);
      }
      if(events.sdlKeyCode == SDLK_DELETE) {
        anim.removeCurrentKeyFrame(fractPars);
      }
      if(events.sdlKeyCode == SDLK_LEFT && events.noModifiers) {
        anim.previousKeyFrame(fractPars);
      }
      if(events.sdlKeyCode == SDLK_RIGHT && events.noModifiers) {
        anim.nextKeyFrame(fractPars);
      }
      if(events.sdlKeyCode == SDLK_LEFT && events.lControlDown) {
        anim.previousFrame(fractPars);
      }
      if(events.sdlKeyCode == SDLK_RIGHT && events.lControlDown) {
        anim.nextFrame(fractPars);
      }
      if(events.sdlKeyCode == SDLK_LEFT && events.lShiftDown) {
        anim.animationProgressionRenderingTime *= 3.0/4.0;
      }
      if(events.sdlKeyCode == SDLK_RIGHT && events.lShiftDown) {
        anim.animationProgressionRenderingTime *= 4.0/3.0;
      }
      if(events.sdlKeyCode == SDLK_LEFT && events.rControlDown) {
        anim.numFractionalFramesPerSec--;
        anim.numFractionalFramesPerSec = max(1, anim.numFractionalFramesPerSec);
      }
      if(events.sdlKeyCode == SDLK_RIGHT && events.rControlDown) {
        anim.numFractionalFramesPerSec++;
      }
      if(events.sdlKeyCode == SDLK_k && events.noModifiers) {
        debugRendering = !debugRendering;
        if(debugRendering) printf("Debug rendering on...\n");
        else printf("Debug rendering off...\n");
      }
      if(events.sdlKeyCode == SDLK_m && events.noModifiers) {
        anim.applyKeyFrame(fractPars);
      }
      if(events.sdlKeyCode == SDLK_m && events.lControlDown) {
        anim.markSwapFrame();
      }
      if(events.sdlKeyCode == SDLK_m && events.lShiftDown) {
        anim.swapKeyFrames(fractPars);
        fractPars.updateColorMap();
      }

      if(events.sdlKeyCode == SDLK_n && events.noModifiers) {
        fractPars.useColorMap = !fractPars.useColorMap;
      }

      if(events.sdlKeyCode == SDLK_n && events.lControlDown && !events.lAltDown && !events.lShiftDown) {
        fractPars.colorMap->colors.push_back(Vec3d(1, 1, 1));
        fractPars.updateColorMap();
      }
      if(events.sdlKeyCode == SDLK_n && events.lShiftDown && !events.lControlDown) {
        if(fractPars.colorMap->colors.size() > 0) {
          fractPars.colorMap->colors.erase(fractPars.colorMap->colors.end());
        }
        fractPars.updateColorMap();
      }
      if(events.sdlKeyCode == SDLK_n && events.lAltDown && !events.lControlDown) {
        for(int i=0; i<fractPars.colorMap->colors.size(); i++) {
          fractPars.colorMap->colors[i].randomize();
        }
        fractPars.updateColorMap();
      }
      if(events.sdlKeyCode == SDLK_n && events.lAltDown && events.lControlDown) {
        for(int i=1; i<fractPars.colorMap->colors.size()-1; i++) {
          fractPars.colorMap->colors[i].randomize();
        }
        fractPars.colorMap->colors[0].set(0, 0, 0);
        fractPars.colorMap->colors[fractPars.colorMap->colors.size()-1].set(1, 1, 1);
        fractPars.updateColorMap();
      }
      if(events.sdlKeyCode == SDLK_n && events.lControlDown && events.lShiftDown) {
        std::reverse(fractPars.colorMap->colors.begin(), fractPars.colorMap->colors.end());
        fractPars.updateColorMap();
      }

      if(useShader) {
        if(events.sdlKeyCode == SDLK_a && events.noModifiers) {
          fractPars.postEffectMode = (fractPars.postEffectMode+1) % fractPars.numPostEffectModes;
        }
        if(events.sdlKeyCode == SDLK_s && events.noModifiers) {
          fractPars.frozenTime = !fractPars.frozenTime;
        }
        if(events.sdlKeyCode == SDLK_d && events.noModifiers) {
          fractPars.mappingMode = (fractPars.mappingMode+1) % fractPars.numMappingModes;
        }
      }

      if(!useShader) {
        if(events.sdlKeyCode == SDLK_x && events.noModifiers) {
          fractPars.softwareRenderMode = (fractPars.softwareRenderMode+1) % fractPars.numSoftwareRenderModes;
          fractPars.viewChanged = true;
        }

        if(fractPars.softwareRenderMode > 0) {
          if(events.sdlKeyCode == SDLK_z && events.noModifiers) {
            fractPars.histogramMappingMode = (fractPars.histogramMappingMode+1) % fractPars.numHistogramMappingModes;
          }
        }

        if(fractPars.softwareRenderMode == 0) {
          if(events.sdlKeyCode == SDLK_r && events.noModifiers) {
            for(int i=0; i<numGrains; i++) {
              dust[i].pos.randomize(0, screenW, 0, screenH);
            }
          }
        }

        if(fractPars.softwareRenderMode == 2) {
          int k = events.sdlKeyCode - SDLK_1;
          if(k >= 0 && k <= 11) {
            double t = events.lShiftDown ? 10 : 1;
            bool keepSignsUnchanged = events.lAltDown;
            int id = events.lControlDown ? 7 : 0;
            if(keepSignsUnchanged) {
              for(int i=id; i<id+7; i++) {
                int sign = fractPars.flamePars[fractPars.activeFlamePars].p[i] < 0 ? -1 : 1;
                double d = sign * Random::getDouble(0, k*t);
                fractPars.flamePars[fractPars.activeFlamePars].p[i] = d;
              }
            }
            else {
              for(int i=id; i<id+7; i++) {
                fractPars.flamePars[fractPars.activeFlamePars].p[i] = Random::getDouble(-k*t, k*t);
              }
            }
            fractPars.viewChanged = true;
          }
        }
        if(fractPars.softwareRenderMode == 3) {
          int k = events.sdlKeyCode - SDLK_1;
          if(k >= 0 && k <= 11) {
            double t = events.lShiftDown ? 10 : 1;
            bool keepSignsUnchanged = events.lAltDown;
            int startInd = events.lControlDown ? 13 : 0;
            int endInd = events.lControlDown ? 13+7 : 13;
            if(keepSignsUnchanged) {
              for(int i=startInd; i<endInd; i++) {
                int sign = fractPars.flamePars[fractPars.activeFlamePars].p3d[i] < 0 ? -1 : 1;
                double d = sign * Random::getDouble(0, k*t);
                fractPars.flamePars[fractPars.activeFlamePars].p3d[i] = d;
              }
            }
            else {
              for(int i=startInd; i<endInd; i++) {
                fractPars.flamePars[fractPars.activeFlamePars].p3d[i] = Random::getDouble(-k*t, k*t);
              }
            }
            fractPars.viewChanged = true;
          }
        }
      }

    }

    if(events.sdlKeyCode == SDLK_o) {
      view3dActive = !view3dActive;
      if(view3dActive) {
        /*for(int i=0; i<renderQuitRequested2.size(); i++) {
          //renderQuitRequested2 = true;
        }*/
      }
      fractPars.setInputActive(!view3dActive);
    }

    if(fractPars.softwareRenderMode == 3 && view3dActive) {
      scene3D->onKeyDown(events);
    }

  }

  void onMouseWheel() {
    if(fractPars.softwareRenderMode == 2 || fractPars.softwareRenderMode == 3) {
      if(events.lControlDown) {
        int v = fractPars.flamePars[fractPars.activeFlamePars].variation;
        v = clamp(v + events.mouseWheel, -1, FractalParameters::numFlameVariations-1);
        if(v != fractPars.flamePars[fractPars.activeFlamePars].variation) {
          fractPars.flamePars[fractPars.activeFlamePars].variation = v;
          fractPars.viewChanged = true;
        }
      }
      else {
        fractPars.activeFlamePars = (fractPars.numActiveVariations+fractPars.activeFlamePars-events.mouseWheel) % fractPars.numActiveVariations;
      }
    }
  }


  void onMousePressed() {
    if(showSynth) {
      synth->onMousePressedScreenKeys(events, screenW, screenH);
    }
  }

  void onMouseReleased() {
  }

  void onMouseMotion() {
    if(fractPars.softwareRenderMode == 3 && view3dActive) {
      scene3D->onMouseMotion(events);
    }
  }


  struct RenderThreadData {
    int imgIndex;
    FractalSketch *sketch;

    RenderThreadData(int imgIndex, FractalSketch *sketch) {
      this->imgIndex = imgIndex;
      this->sketch = sketch;
    }
  };

  static int renderThread(void *data) {
    RenderThreadData *d = (RenderThreadData*)data;
    FractalSketch *sketch = d->sketch;
    FractalParameters *fractPars = &sketch->fractPars;
    int n = d->imgIndex;
    Texture *img = sketch->renderThreadBufferImgs[n];
    sketch->imgRenderInProgress[n] = true;
    sketch->imgRenderQuitRequested[n] = false;
    //int t = SDL_GetTicks();

    //img->prepareIndirectPixelArray();
    for(int i=0; i<img->w && !sketch->imgRenderQuitRequested[n]; i++) {
      double x = map(i, 0.0, img->w-1, -fractPars->aspectRatio, fractPars->aspectRatio);
      x = x * fractPars->scale + fractPars->location.x;
      for(int j=0; j<img->h && !sketch->imgRenderQuitRequested[n]; j++) {
        double y = map(j, 0.0, img->h-1, -1.0, 1.0);
        y = y*fractPars->scale - fractPars->location.y;
        int iter = mandelbrotIterator(x, y, fractPars);
        double q = (double)iter/fractPars->maxIter;
        if(fractPars->colorMode == 2) {
          //img->pixels[i+j*img->w] = toRGBA(0.2+0.8*q, 0, 0.1-0.2*q, 1.0);
          img->setPixel(i, j, 0.2+0.8*q, 0, 0.1-0.2*q, 1.0);
        }
        else {
          img->setPixel(i, j, q, q, q, 1.0);
        }
      }
    }

    if(!sketch->imgRenderQuitRequested[n]) {
      //t = SDL_GetTicks() - t;
      //printf("Rendering finished (%d x %d), duration %.02f s\n", img->w, img->h, double(0.001*t));
      sketch->imgReady[n] = true;
    }
    sketch->imgRenderInProgress[n] = false;
    delete d;

    return 1;
  }


  static int renderThread2(void *data) {
    RenderThreadData *d = (RenderThreadData*)data;
    FractalSketch *sketch = d->sketch;
    FractalParameters *fractPars = &sketch->fractPars;
    int n = d->imgIndex;
    //printf("Pixel visit rendering started %d\n", n);
    sketch->renderInProgress2[n] = true;
    sketch->renderQuitRequested2[n] = false;

    //FractalParameters fractParsFilter;
    //memcpy(&fractParsFilter, fractPars, sizeof(FractalParameters));
    //fractParsFilter.mandelbrotMode = 2;
    if(fractPars->softwareRenderMode == 1) {
      while(!sketch->renderQuitRequested2[n]) {
        double x = Random::getDouble(-2, 2);
        double y = Random::getDouble(-2, 2);
        if(mandelbrotIterator(x, y, fractPars) < int(fractPars->maxIter)) {
          mandelbrotIterator2(x, y, sketch->renderThreadCombinedBuffer2, fractPars);
        }
        else {
          fractPars->numSamplesFiltered++;
        }
        fractPars->numSamples++;
      }
    }
    if(fractPars->softwareRenderMode == 2) {
      while(!sketch->renderQuitRequested2[n]) {
        double x = Random::getDouble(-1, 1);
        double y = Random::getDouble(-1, 1);
        fractalFlameIterator(x, y, sketch->renderThreadBuffer2, fractPars);
        //Vec2d p(10000, 100000);
        //p.randomizeLengthAndDirection(2, 2);
        //fractalFlameIterator(x, y, sketch->renderThreadBuffer2, fractPars);
        fractPars->numSamples++;
      }
    }
    if(fractPars->softwareRenderMode == 3) {
      Vec3d p;
      while(!sketch->renderQuitRequested2[n]) {
        while(sketch->view3dActive) SDL_Delay(50);
        if(sketch->debugRendering) {
          sketch->voxels[Random::getInt(0, sketch->voxels.size()-1)]++;
        }
        else {
          p.x = Random::getDouble(-1, 1);
          p.y = Random::getDouble(-1, 1);
          p.z = Random::getDouble(-1, 1);
          fractalFlameIterator3D(p, sketch->voxels, fractPars);
        }
        //Vec2d p(10000, 100000);
        //p.randomizeLengthAndDirection(2, 2);
        //fractalFlameIterator(x, y, sketch->renderThreadBuffer2, fractPars);
        fractPars->numSamples++;
      }
    }

    fractPars->renderingEndedMillis = SDL_GetTicks();
    double t = (fractPars->renderingEndedMillis-fractPars->renderingStartedMillis)*0.001;

    //printf("Pixel visit rendering finished %d\n", n);

//    sketch->imgReady2[n] = true;
    sketch->renderInProgress2[n] = false;
    delete d;

    return 1;
  }

  static inline int mandelbrotIterator(double x, double y, double maxIter) {
    double real = 0, imag = 0;
    int i = 0;
    while(real*real + imag*imag < 4 && i < maxIter) {
      double tmpReal = real*real - imag*imag + x;
      imag = 2.0*real*imag + y;
      real = tmpReal;
      i++;
    }
    return i;
  }

  static inline int mandelbrotIterator(double x, double y, double maxIter, double power, Vec2d juliaPoint) {
    Vec2d z(x, y);
    Vec2d c = juliaPoint.isZero() ? Vec2d(x, y) : juliaPoint;
    int i = 0;
    while(z.x*z.x + z.y*z.y < 4 && i < maxIter) {
      z.complexPower(power);
      z.add(c);
      i++;
    }
    return i;
  }

  static inline int mandelbrotIterator(double x, double y, FractalParameters *fractPars) {
    Vec2d c(x, y);
    Vec2d w(x, y), tmpW(0, 0);

    if(fractPars->mandelbrotMode == 1) {
      w.set(0,0);
    }
    if(fractPars->juliaPoint.x != 0.0 || fractPars->juliaPoint.y != 0) {
      c.set(fractPars->juliaPoint);
    }
    if(fractPars->mandelbrotMode % 2 == 1) {
      double m = 1.0/(c.x*c.x + c.y*c.y);
      c.set(c.x*m, -c.y*m);
    }

    int i = 0;

    if(fractPars->mandelbrotMode < 2) {
      while(w.x*w.x + w.y*w.y < 4.0 && i < int(fractPars->maxIter)) {
        tmpW.x = w.x*w.x - w.y*w.y + c.x;
        tmpW.y = 2.0 * w.x * w.y + c.y;
        w.x = tmpW.x;
        w.y = tmpW.y;
        i++;
      }
    }
    if(fractPars->mandelbrotMode > 1) {
      while(w.x*w.x + w.y*w.y < 40.0 && i < int(fractPars->maxIter)) {
        tmpW.x = w.x*w.x - w.y*w.y + c.x;
        tmpW.y = 2.0 * w.x * w.y + c.y;
        double m = 1.0/(tmpW.x*tmpW.x + tmpW.y*tmpW.y);
        w.x = tmpW.x * m;
        w.y = -tmpW.y * m;
        i++;
      }
    }

    return i;
  }

  static inline int mandelbrotIterator2(double x, double y, unsigned long *pixels, FractalParameters *fractPars) {
    Vec2d c(x, y);
    Vec2d w(x, y), tmpW(0, 0);

    if(fractPars->mandelbrotMode == 1) {
      w.set(0,0);
    }
    if(fractPars->juliaPoint.x != 0.0 || fractPars->juliaPoint.y != 0) {
      c.set(fractPars->juliaPoint);
    }
    if(fractPars->mandelbrotMode % 2 == 1) {
      double m = 1.0/(c.x*c.x + c.y*c.y);
      c.set(c.x*m, -c.y*m);
    }

    int i = 0;

    if(fractPars->mandelbrotMode < 2) {
      while(w.x*w.x + w.y*w.y < 4.0 && i < int(fractPars->maxIter)) {
        tmpW.x = w.x*w.x - w.y*w.y + c.x;
        tmpW.y = 2.0 * w.x * w.y + c.y;
        w.x = tmpW.x;
        w.y = tmpW.y;

        int x = int(map(w.x-fractPars->location.x, -fractPars->aspectRatio*fractPars->scale, fractPars->aspectRatio*fractPars->scale, 0, fractPars->w-1));
        if(x >= 0 && x < fractPars->w) {
          int y = int(map(w.y+fractPars->location.y, -1*fractPars->scale, 1*fractPars->scale, 0, fractPars->h-1));
          if(y >= 0 && y < fractPars->h ) {
            pixels[x+y*fractPars->w]++;
          }
        }

        i++;
      }
    }
    if(fractPars->mandelbrotMode > 1) {
      while(w.x*w.x + w.y*w.y < 40.0 && i < int(fractPars->maxIter)) {
        tmpW.x = w.x*w.x - w.y*w.y + c.x;
        tmpW.y = 2.0 * w.x * w.y + c.y;
        double m = 1.0/(tmpW.x*tmpW.x + tmpW.y*tmpW.y);
        w.x = tmpW.x * m;
        w.y = -tmpW.y * m;

        int x = int(map(w.x-fractPars->location.x, -fractPars->aspectRatio*fractPars->scale, fractPars->aspectRatio*fractPars->scale, 0, fractPars->w-1));
        if(x >= 0 && x < fractPars->w) {
          int y = int(map(w.y+fractPars->location.y, -1*fractPars->scale, 1*fractPars->scale, 0, fractPars->h-1));
          if(y >= 0 && y < fractPars->h ) {
            pixels[x+y*fractPars->w]++;
          }
        }


        i++;
      }
    }

    return i;
  }


  void onUpdate() {

    for(int i=0; i<commandQueue.commands.size(); i++) {
      Command *cmd = NULL;
      if(cmd = commandQueue.popCommand(&setVoxelGridSizeCmdTmpl)) {
        int w, h, d;
        saveFractParsCmdTmpl.fillValues(cmd, &w, &h, &d);
        requestedVoxelGridSize.x = w;
        requestedVoxelGridSize.y = h;
        requestedVoxelGridSize.z = d;
        delete cmd;
      }

      if(cmd = commandQueue.popCommand(&saveFractParsCmdTmpl)) {
        std::string filename;
        saveFractParsCmdTmpl.fillValues(cmd, &filename);
        saveCerealBinary(fractPars, fractalParametersBasePath + filename + fractalParametersFileExtension);
        delete cmd;
      }
      if(cmd = commandQueue.popCommand(&loadFractParsCmdTmpl)) {
        std::string filename;
        loadFractParsCmdTmpl.fillValues(cmd, &filename);
        loadCerealBinary(fractPars, fractalParametersBasePath + filename + fractalParametersFileExtension);
        fractPars.viewChanged = true;
        delete cmd;
      }
      if(cmd = commandQueue.popCommand(&saveFractAnimCmdTmpl)) {
        std::string filename;
        saveFractAnimCmdTmpl.fillValues(cmd, &filename);
        saveCerealBinary(anim, animationParametersBasePath + filename + animationParametersFileExtension);
        delete cmd;
      }
      if(cmd = commandQueue.popCommand(&loadFractAnimCmdTmpl)) {
        std::string filename;
        loadFractAnimCmdTmpl.fillValues(cmd, &filename);
        loadCerealBinary(anim, animationParametersBasePath + filename + animationParametersFileExtension);
        fractPars.viewChanged = true;
        for(FractalParameters &fp : anim.fractParsKeyFrames) {
          fp.w = fractPars.w;
          fp.h = fractPars.h;
          fp.aspectRatio = fractPars.aspectRatio;
          fp.colorMap = fractPars.colorMap;
          fp.viewChanged = true;
        }
        anim.resetSettings();
        anim.getCurrentKeyFrame(fractPars);
        delete cmd;
      }

      if(cmd = commandQueue.popCommand(&clearAnimationCmdTmpl)) {
        anim.clear();
        delete cmd;
      }
      if(cmd = commandQueue.popCommand(&resetAnimationSettingsCmdTmpl)) {
        anim.resetSettings();
        delete cmd;
      }


    }

    /*for(int i=0; i<14; i++) {
      fractPars.flamePars[fractPars.activeFlamePars].p[i] = fractPars.arg[i];
    }*/

    fractPars.minIter = fractPars.mandelbrotPower;

    anim.update(fractPars, &renderThreadBufferImg);

    if(fractPars.viewChanged) {
      for(int i=0; i<numRenderThreadBufferImgs; i++) {
        imgReady[i] = false;
        imgRenderQuitRequested[i] = true;
        renderQuitRequested2[i] = true;
      }
      fractPars.viewChanged = false;
      fractPars.updateIterationRouteRequested = true;
    }
    if(fractPars.softwareRenderMode > 0 && !view3dActive) {
      int n = 0;
      for(int i=0; i<numRenderThreads2; i++) {
        if(!renderInProgress2[i]) n++;
      }
      if(n == numRenderThreads2) {
        //memset(renderThreadCombinedBuffer2, 0, screenW*screenH*sizeof(unsigned long));
// FIXME
        if(fractPars.softwareRenderMode == 3) {
          if(requestedVoxelGridSize != fractPars.voxelGridSize) {
            fractPars.voxelGridSize = requestedVoxelGridSize;
            scene3D->voxelGrid.setSize(requestedVoxelGridSize.x, requestedVoxelGridSize.y, requestedVoxelGridSize.z);
            fractPars.xRange = scene3D->voxelGrid.xRange;
            fractPars.yRange = scene3D->voxelGrid.yRange;
            fractPars.zRange = scene3D->voxelGrid.zRange;
            voxels.resize(requestedVoxelGridSize.x * requestedVoxelGridSize.y * requestedVoxelGridSize.z);
          }

          voxels.assign(voxels.size(), 0);
          //printf("Resetting...\n");
        }
        else {
          memset(renderThreadBuffer2, 0, screenW*screenH*sizeof(Vec3d));
        }

        fractPars.numSamples = 0;
        fractPars.numSamplesFiltered = 0;
        fractPars.numPixelsMapped = 0;
        fractPars.renderingStartedMillis = SDL_GetTicks();
        for(int i=0; i<numRenderThreads2; i++) {
          RenderThreadData *input = new RenderThreadData(i, this);
          SDL_Thread *thread = SDL_CreateThread(renderThread2, "Pixel visit render thread", input);
          SDL_DetachThread(thread);
        }
        anim.readyToRecord();
      }
    }
    else {
      for(int i=0; i<numRenderThreadBufferImgs-1; i++) {
        if(!imgReady[i] && !imgRenderInProgress[i]) {
          RenderThreadData *input = new RenderThreadData(i, this);
          SDL_Thread *thread = SDL_CreateThread(renderThread, "Iteration count render thread", input);
          SDL_DetachThread(thread);
        }
      }
    }

    synth->onUpdateGetMidiMessages(midiInterface);

  }

  void onDraw() {
    clear(0, 0, 0, 1);

    if(!useShader && !showSynth) {
      double t = (SDL_GetTicks()-fractPars.renderingStartedMillis)*0.001;
      if(fractPars.softwareRenderMode == 3) {
        if(timer.frameCount % 10 == 0) {
           unsigned long minVal = -1, maxVal = 0;

           for(int i=0; i<voxels.size(); i++) {
             if(voxels[i] == 0) continue;
             minVal = min(voxels[i], minVal);
             maxVal = max(voxels[i], maxVal);
           }
           minVal = min(minVal, maxVal);
           double minV = log(1.0 + minVal);
           double maxV  = log(1.0 + maxVal);

           fractPars.numPixelsMapped = 0;

           if(maxV == minV) {
             memset(scene3D->voxelGrid.voxels.data(), 0, scene3D->voxelGrid.voxels.size() * sizeof(scene3D->voxelGrid.voxels[0]));
           }
           else {
             for(int i=0; i<voxels.size(); i++) {
               double r = map(log(1.0+voxels[i]), minV, maxV, 0.0, 1.0);

               Vec3d col;
               if(std::isnan(r)) {
                 printf("nan value at rendered buffer...\n");
                 break;
               }
               if(anim.numKeyFrames > 1 && anim.frameFractionCounter > 0 && anim.interpolateColorsByRGB) {
                 anim.interpolateRGB(col, r);
               }
               else {
                 col = fractPars.getColor(r);
               }
               scene3D->voxelGrid.voxels[i].x = col.x;
               scene3D->voxelGrid.voxels[i].y = col.y;
               scene3D->voxelGrid.voxels[i].z = col.z;
               scene3D->voxelGrid.voxels[i].w = rgbToAlpha(col.x, col.y, col.z);
             }
           }

         }
         scene3D->updateView();
         scene3D->update(time);
        scene3D->draw(0, 0, screenW, screenH);
        if(view3dActive) {
          scene3D->printInfo(textRenderer, 0, 0, screenW, screenH, 1, true);
        }
      }
      else if(fractPars.softwareRenderMode > 0) {
        if((t < 3) || (t > 3 && t < 6 && timer.frameCount % 2 == 0)
           || (t > 6 && t < 15 && timer.frameCount % 4 == 0)
           || (t > 15 && timer.frameCount % 60 == 0) ) {

          //Texture *img = &renderThreadBufferImg;
          //unsigned int *pixels;
          //renderThreadBufferImg.accessPixels(&pixels);

          if(fractPars.softwareRenderMode == 1) {
            unsigned long minVal = -1, maxVal = 0;

            if(fractPars.histogramMappingMode % 2 == 1) {
              int size = screenW*screenH;
              memcpy(tmpScreenBuffer, renderThreadCombinedBuffer2, size*sizeof(unsigned long));
              std::sort(tmpScreenBuffer, tmpScreenBuffer+size);

              minVal = tmpScreenBuffer[screenW*4];
              maxVal = tmpScreenBuffer[size-screenW/4];
            }
            else {
              for(int i=0; i<screenW*screenH; i++) {
                minVal = min(minVal, renderThreadCombinedBuffer2[i]);
                maxVal = max(maxVal, renderThreadCombinedBuffer2[i]);
              }
            }
            fractPars.numPixelsMapped = 0;
            for(int i=0; i<screenW*screenH; i++) {
              fractPars.numPixelsMapped += renderThreadCombinedBuffer2[i];
            }


            if(fractPars.histogramMappingMode / 2 == 0) {
              minVal = sqrt(minVal);
              maxVal = sqrt(maxVal);
              for(int i=0; i<renderThreadBufferImg.w*renderThreadBufferImg.h; i++) {
                double q = map(sqrt(renderThreadCombinedBuffer2[i]), minVal, maxVal, 0.0, 1.0);
                //pixels[i] = toRGBA(q, q, q, 1.0);
                renderThreadBufferImg.setPixel(i, q, q, q, 1.0);
              }
            }
            if(fractPars.histogramMappingMode / 2 == 1) {
              minVal = log(1+minVal);
              maxVal = log(1+maxVal);
              for(int i=0; i<renderThreadBufferImg.w*renderThreadBufferImg.h; i++) {
                double q = map(log(1+renderThreadCombinedBuffer2[i]), minVal, maxVal, 0.0, 1.0);
                //pixels[i] = toRGBA(q, q, q, 1.0);
                renderThreadBufferImg.setPixel(i, q, q, q, 1.0);
              }
            }
            if(fractPars.histogramMappingMode / 2 == 2) {
              for(int i=0; i<renderThreadBufferImg.w*renderThreadBufferImg.h; i++) {
                double q = map(renderThreadCombinedBuffer2[i], minVal, maxVal, 0.0, 1.0);
                //pixels[i] = toRGBA(q, q, q, 1.0);
                renderThreadBufferImg.setPixel(i, q, q, q, 1.0);
              }
            }
          }

          if(fractPars.softwareRenderMode == 2) {
            Vec3d minVals(pow(2, 62)), maxVals(0);

            for(int i=0; i<screenW*screenH; i++) {
              double v = renderThreadBuffer2[i].x;
              if(v == 0) continue;
              minVals.x = min(minVals.x, v);
              maxVals.x = max(maxVals.x, v);
            }
            minVals.x = min(minVals.x, maxVals.x);
            minVals.set(log(1+minVals.x), log(1+minVals.y), log(1+minVals.z));
            maxVals.set(log(1+maxVals.x), log(1+maxVals.y), log(1+maxVals.z));
            //for(int i=0; i<screenW*screenH; i++) {
            //  minVals.min(renderThreadBuffer2[i]);
            //  maxVals.max(renderThreadBuffer2[i]);
            //}
            //minVals.set(log(1+minVals.x), log(1+minVals.y), log(1+minVals.z));
            //maxVals.set(log(1+maxVals.x), log(1+maxVals.y), log(1+maxVals.z));

            //double maxV = maxVals.max();
            //double minV = minVals.min();
            double maxV = maxVals.x;
            double minV = minVals.x;

            fractPars.numPixelsMapped = 0;

            if(maxV == minV) {
              //printf("maxV == minV...\n");
            }
            else {
              for(int i=0; i<renderThreadBufferImg.w*renderThreadBufferImg.h; i++) {
                //fractPars.numPixelsMapped += (int)round(renderThreadBuffer2[i].a);
                double r = map(log(1+renderThreadBuffer2[i].x), minV, maxV, 0.0, 1.0);
                //double g = map(log(1+renderThreadBuffer2[i].y), minV, maxV, 0.0, 1.0);
                //double b = map(log(1+renderThreadBuffer2[i].z), minV, maxV, 0.0, 1.0);
                //pixels[i] = toRGBA(r, g, b, 1.0);
                //Vec3d col = hslToRgb(map(r, 0, 0.5, -PI/4, PI/4), 0.5, r);
                //Vec3d col = hslToRgb(PI*4.0/3.0-map(r, 0, 1.0, 0, PI/3.0), 0.5, r);
                //double ha = fractPars.colorArg[0];
                //double hr = fractPars.colorArg[1];
                //double hs = fractPars.colorArg[2];
                //Vec3d col = hslToRgb(ha+r*hr, hs, r);
                Vec3d col;
                if(std::isnan(r)) {
                  printf("nan value at rendered buffer...\n");
                  break;
                }
                if(anim.numKeyFrames > 1 && anim.frameFractionCounter > 0 && anim.interpolateColorsByRGB) {
                  anim.interpolateRGB(col, r);
                }
                else {
                  col = fractPars.getColor(r);
                }

                //pixels[i] = toRGBA(col);
                renderThreadBufferImg.setPixel(i, col);
              }
            }
          }

          renderThreadBufferImg.applyPixels();

          //imgReady[0] = true;
        }
        //if(imgReady[0]) {
          renderThreadBufferImg.render();
        //}
        /*for(int j=0; j<50; j++) {
          for(int i=0; i<50; i++) {
            double x = map(i, 0, 50, 0, screenW-1);
            double y = map(j, 0, 50, 0, screenH-1);
            double h = map(i, 0.0, 50.0, 0.0, TAU);
            double s = map(j, 0.0, 50.0, 0.0, 1);
            //printf("");
            Vec3d col = hslToRgb(h, 0.5, s);
            setColor(col.x, col.y, col.z, 1.0, sdlInterface);
            drawCircle(x, y, 5, sdlInterface);
          }
        }*/
        if(fractPars.showIterationRoute) {
          if(fractPars.updateIterationRouteRequested) {
            fractPars.updateIterationRouteRequested = false;
            fractPars.updateIterationRoute = true;
            fractalFlameIterator(fractPars.juliaPoint.x, fractPars.juliaPoint.y, renderThreadBuffer2, &fractPars);
          }

          Vec2d pp = fractPars.toPixel(fractPars.juliaPoint);
          setColor(0.5, 0, 0, 1.0, sdlInterface);
          drawCircle(pp, 4, sdlInterface);
          for(int i=0; i<fractPars.numIterationRoutePoints; i++) {
            Vec2d p = fractPars.toPixel(fractPars.iterationRoute[i]);
            if(i >= fractPars.minIter) {
              setColor(0.3, 0.3, 0.7, 1.0, sdlInterface);
              drawLine(pp, p, sdlInterface);
            }
            if(i < fractPars.minIter) setColor(0.8, 0, 0, 1.0, sdlInterface);
            else setColor(0.0, 0.0, 0.8, 1.0, sdlInterface);
            drawCircle(p, 4, sdlInterface);
            pp.set(p);
          }
        }

      }
      else {
        bool noneReady = true;
        for(int i=0; i<numRenderThreadBufferImgs; i++) {
          if(imgReady[i]) noneReady = false;
        }
        if(noneReady) {
          if(softwareRenderPreviewIndexChangeRequest == 1 && softwareRenderPreviewIndex < numRenderThreadBufferImgs-1) {
            softwareRenderPreviewIndex++;
          }
          if(softwareRenderPreviewIndexChangeRequest == -1 && softwareRenderPreviewIndex > 0) {
            softwareRenderPreviewIndex--;
          }
          int t = SDL_GetTicks();
          Texture *img = renderThreadBufferImgs[softwareRenderPreviewIndex];
          //unsigned int *pixels;
          //img->accessPixels(&pixels);
          for(int i=0; i<img->w; i++) {
            double x = map(i, 0.0, img->w-1, -aspectRatio, aspectRatio);
            x = x * fractPars.scale + fractPars.location.x;
            for(int j=0; j<img->h; j++) {
              double y = map(j, 0.0, img->h-1, -1.0, 1.0);
              y = y*fractPars.scale - fractPars.location.y;
              int iter = mandelbrotIterator(x, y, &fractPars);
              double q = (double)iter/fractPars.maxIter;
              if(fractPars.colorMode == 2) {
                //pixels[i+j*img->w] = toRGBA(0.2+0.8*q, 0, 0.1-0.2*q, 1.0);
                img->setPixel(i, j, 0.2+0.8*q, 0, 0.1-0.2*q, 1.0);
              }
              else {
                img->setPixel(i, j, q, q, q, 1.0);
              }
            }
          }
          img->applyPixels();
          imgReady[softwareRenderPreviewIndex] = true;
          t = SDL_GetTicks() - t;
          if(t > 1000/60*2) {
            softwareRenderPreviewIndexChangeRequest = 1;
          }
          else if(t < 1000/60/2) {
            softwareRenderPreviewIndexChangeRequest = -1;
          }
          else {
            softwareRenderPreviewIndexChangeRequest = 0;
          }
        }
        //img->prepareIndirectPixelArray();
        //img->applyIndirectPixelArray();
        //img->render(sdlInterface);
        Texture *refImg = renderThreadBufferImgs[softwareRenderPreviewIndex];
        for(int i=0; i<numRenderThreadBufferImgs; i++) {
          if(i < softwareRenderPreviewIndex && (events.mouseDownL || events.mouseDownR || events.mouseDownM)) continue;
          if(imgReady[i]) {
            //renderThreadBufferImgs[i]->applyIndirectPixelArray();
            // FIXME might not be necessary...
            renderThreadBufferImgs[i]->applyPixels();
            refImg = renderThreadBufferImgs[i];
  //          renderThreadBufferImgs[i]->render(sdlInterface);
            //if(timer.frameCount%10 == 0)
            //printf("Blitting rendered image %d\n",i);
            break;
          }
        }
        clear(0, 0, 0, 1);
        double w = refImg->w;
        double h = refImg->h;
        //unsigned int *pixels;
        //refImg->accessPixels(&pixels);
        setColor(0.7, 0.7, 0.7, 1.0, sdlInterface);
        for(int i=0; i<numGrains; i++) {
          Vec2d &pos = dust[i].pos;
          int x = int(pos.x/screenW*w);
          int y = int(pos.y/screenH*h);
          int r = refImg->getPixelR(x, y) * 255.0;
          double xt = Random::getDouble(-1, 1) * (260-r) / 4.0;
          double yt = Random::getDouble(-1, 1) * (260-r) / 4.0;
          pos.add(xt, yt);
          if(pos.x < 0) pos.x = 0;
          if(pos.x >= screenW) pos.x = screenW-1;
          if(pos.y < 0) pos.y = 0;
          if(pos.y >= screenH) pos.y = screenH-1;
          // FIXME
          drawPoint(pos, sdlInterface);
        }
        //refImg->render();
      }

    }
    else {
      GlShader &shader = showSynth ? synthShader : mandelbrotShader;

      shader.activate();

      shader.setUniform2f("screenSize", screenW, screenH);
      shader.setUniform2f("mousePos", events.mouseX, events.mouseY);
      shader.setUniform1d("time", fractPars.frozenTime ? synth->getMostRecentNote()->startTime : synth->getPaTime());

      if(!showSynth) {
        shader.setUniform2d("location", fractPars.location.x, -fractPars.location.y);
        shader.setUniform1d("scale", fractPars.scale);
        shader.setUniform1d("maxIter", fractPars.maxIter);
        shader.setUniform2d("juliaPoint", fractPars.juliaPoint.x, fractPars.juliaPoint.y);
        shader.setUniform1i("mandelbrotMode", fractPars.mandelbrotMode);
        shader.setUniform1i("mappingMode", fractPars.mappingMode);
        shader.setUniform1i("colorMode", fractPars.colorMode);
        shader.setUniform1i("postEffectMode", fractPars.postEffectMode);
      }

      synth->setShaderUniforms(shader);


      if(!showSynth && fractPars.postEffectMode > 0) {
        /*renderTargetImg.bindGlTexture();
        setRenderTarget(renderTargetImg, sdlInterface);
        quad.render();
        setDefaultRenderTarget(sdlInterface);
        renderTargetImg.render(sdlInterface);
        renderTargetImg.unbindGlTexture();*/
        renderTargetImg.setRenderTarget();
        renderTargetImg.render();
        renderTargetImg.unsetRenderTarget();
        renderTargetImg.render();
      }
      else {
        //quad.render();
      }
      shader.deActivate();


      if(showSynth) {
        //synth->drawSynthNoteLayer(screenW, screenH, sdlInterface);
        // FIXME
        //synth->drawSynthNoteLayer(screenW, screenH, geomRenderer);
      }

    }
    if(!showSynth && showInfo && !view3dActive) {
      if(blackText) {
        textRenderer.setColor(0, 0, 0, 1);
      }
      else {
        textRenderer.setColor(1, 1, 1, 1);
      }
      std::wstring str = L"fps " + std::to_wstring(int(round(1.0/dt)));
      wchar_t buf[256];
      std::swprintf(buf, 256, L"\n(%.4g, %.4g), %.4g, %d", fractPars.location.x, fractPars.location.y, 1.0/fractPars.scale, int(fractPars.maxIter));
      str += buf;
      if(fractPars.softwareRenderMode == 2) {
      //std::swprintf(buf, 256, L"\n(%.4g, %.4g), %.4g, %d", fractPars.location.x, //fractPars.location.y, 1.0/fractPars.scale, int(fractPars.maxIter));
        str += L", " + std::to_wstring((int)fractPars.minIter);
      }

      if(!fractPars.juliaPoint.isZero()) {
        std::swprintf(buf, 256, L", julia (%.4g, %.4g)", fractPars.juliaPoint.x, fractPars.juliaPoint.y);
        str += buf;
      }

      if(!useShader && fractPars.softwareRenderMode > 0) {
        double d = (SDL_GetTicks() - fractPars.renderingStartedMillis) / 1000.0;
        int t = int(d);
        int h = t / (60*60);
        int m = (t - h*60*60) / 60;
        int s = (t - h*60*60 - m*60);
        str += L"\n";
        if(h > 0) str += std::to_wstring(h) + L" h ";
        if(h > 0 || m > 0) str += std::to_wstring(m) + L" m ";
        str += std::to_wstring(s) + L" s";
        double sampleRate = fractPars.numSamples/d;
        std::swprintf(buf, 256, L", %.2g w/s\n#w %.2g, #p %.2g", sampleRate, (double)fractPars.numSamples, (double)fractPars.numPixelsMapped);
        str += buf;
      }

      if(fractPars.softwareRenderMode == 2 || fractPars.softwareRenderMode == 3) {
        str += L"\n";
        for(int i=0; i<fractPars.numActiveVariations; i++) {

          if(fractPars.flamePars[i].variation < 0) {
            if(i == fractPars.activeFlamePars) str += L"- ";
            else str += L". ";
          }
          else {
            if(i == fractPars.activeFlamePars) str += L"*";
            str += std::to_wstring(fractPars.flamePars[i].variation) + L" ";
          }
        }
        int var = fractPars.flamePars[fractPars.activeFlamePars].variation;
        if(var >= 0) {
          str += L"\n";
          str += fractPars.flamePars[fractPars.activeFlamePars].getVariationNameWchar();
        }
        for(int i=0; i<fractPars.numActiveVariations; i++) {
          if(fractPars.flamePars[i].variation < 0) continue;
          str += L"\n";
          if(i == fractPars.activeFlamePars) str += L"*";
          if(fractPars.softwareRenderMode == 2) {
            std::swprintf(buf, 256, L"%.2g, (%.2g %.2g %.2g), (%.2g %.2g %.2g)", fractPars.flamePars[i].p[0], fractPars.flamePars[i].p[1], fractPars.flamePars[i].p[2], fractPars.flamePars[i].p[3], fractPars.flamePars[i].p[4], fractPars.flamePars[i].p[5], fractPars.flamePars[i].p[6]);
            str += buf;
            for(int j=7; j<numFlamePars[min(fractPars.flamePars[i].variation, 49)]; j++) {
              std::swprintf(buf, 256, L", %.2g", fractPars.flamePars[i].p[j]);
              str += buf;
            }
          }
          if(fractPars.softwareRenderMode == 3) {
            std::swprintf(buf, 256, L"%.2g, (%.2g %.2g %.2g %.2g), (%.2g %.2g %.2g %.2g), (%.2g %.2g %.2g %.2g)", fractPars.flamePars[i].p3d[0], fractPars.flamePars[i].p3d[1], fractPars.flamePars[i].p3d[2], fractPars.flamePars[i].p3d[3], fractPars.flamePars[i].p3d[4], fractPars.flamePars[i].p3d[5], fractPars.flamePars[i].p3d[6], fractPars.flamePars[i].p3d[7], fractPars.flamePars[i].p3d[8], fractPars.flamePars[i].p3d[9], fractPars.flamePars[i].p3d[10], fractPars.flamePars[i].p3d[11], fractPars.flamePars[i].p3d[12]);
            str += buf;
            for(int j=7+6; j<6+numFlamePars[min(fractPars.flamePars[i].variation, 49)]; j++) {
              std::swprintf(buf, 256, L", %.2g", fractPars.flamePars[i].p3d[j]);
              str += buf;
            }
          }

        }
        str += L"\n";
        std::swprintf(buf, 256, L"h %.2g, r %.2g, s %.2g", fractPars.colorArg[0], fractPars.colorArg[1], fractPars.colorArg[2]);
        str += buf;

        str += L"\n";

        if(anim.numKeyFrames > 0) {
          if(anim.animationPlaying) {
            if(anim.animationRecordMode > 0) {
              str += L"> [rec] ";
            }
            else {
              str += L"> ";
            }
          }
          else str += L"|| ";

          std::swprintf(buf, 256, L"frame %d/%d (%d/%d)", anim.keyFrame+1, anim.numKeyFrames, anim.frameFractionCounter+1, anim.numFractionalFrames);
          str += buf;

          str += L"\n";

          std::swprintf(buf, 256, L"dur %.2g s", anim.getCurrentKeyFrame().frameDuration);
          str += buf;

          std::swprintf(buf, 256, L", fps %d", anim.numFractionalFramesPerSec);
          str += buf;

          str += L"\n";

          /*if(anim.animationProgressionMode == anim.AnimationProgressionModeBasic) {
            std::swprintf(buf, 256, L" ", );
            str += buf;
          }*/
          if(anim.animationProgressionMode == anim.AnimationProgressionModeRenderingTime) {
            std::swprintf(buf, 256, L"rend. time %.2g s, ", anim.animationProgressionRenderingTime);
            str += buf;
          }
          if(anim.animationProgressionMode == anim.AnimationProgressionModeNumSamples) {
            std::swprintf(buf, 256, L"num samples %lu, ", anim.animationProgressionNumSamples);
            str += buf;
          }

          if(anim.interpolateColorsByRGB) {
            str += L"rgb interp.";
          }
          else {
            str += L"hsv interp.";
          }

        }

      }
      textRenderer.print(str, 10, 10, 10, 1, sdlInterface);

      if(!events.textInput.inputGrabbed) {
        if(messageQueue.tmpQueue.size() > 0) {
          std::wstring msgs;
          for(int i=0; i<messageQueue.tmpQueue.size(); i++) {
            msgs += messageQueue.tmpQueue[i]->str + L"\n";
          }
          Vec2d dim = textRenderer.getDimensions(msgs, 10, 1.0);
          double x = screenW-1-dim.x-10, y = 10;
          textRenderer.print(msgs, x, y, 10, 1);
        }
      }

      if(fractPars.useColorMap && fractPars.colorMap->colorBarReady) {
        fractPars.colorMap->colorBar.render(10, screenH - 10 - fractPars.colorMap->colorBar.h);
      }
    }

    if(events.textInput.inputGrabbed) {
      console.render(sdlInterface);
    }

  }






};
