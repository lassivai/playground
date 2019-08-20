#pragma once
#include <ctime>
#include <ctgmath>
#include <cwchar>
#include <string>
#include <vector>
#include <algorithm>
#include <utility>
#include <locale>
#include <limits>
#include <thread>
#include <mutex>
#include <initializer_list>

#include <eigen3/Eigen/Dense>
//#include "FastNoise/FastNoise.h"

// TODO fix the include system (appropriate includes into each file)

#include "util.h"
#include "mathl.h"

#include "cereal_init.h"
#include "sdl_init.h"
#include "portaudiointerface.h"
#include "rtmidi_init.h"
#include "glwrap.h"
#include "gfxwrap.h"
#include "fftw3wrap.h"
#include "textfileparser.h"
#include "fastnoisewrap.h"


#include "texture.h"
//#include "image.h"
#include "scene.h"
#include "geometry.h"
#include "geomath.h"
#include "event.h"

#include "message.h"
#include "command.h"

#include "arr.h"
#include "colors.h"
#include "geom_gfx.h"
#include "colormap.h"
#include "cliargs.h"
#include "timer.h"
//#include "fontxx.h"
#include "textgl.h"
#include "alg.h"
#include "stringparsing.h"
#include "console.h"
#include "camera2d.h"
#include "shape.h"
#include "shapex.h"
#include "phys/simulation.h"
#include "phys/genprog_interface.h"
#include "gui/gui.h"
#include "synth/synth.h"
#include "synth/gui/synthgui.h"
#include "synth/eartrainer.h"
#include "synth/visualizations/rotatingrects.h"
#include "synth/audioplayer.h"
//#include "sndfileio.h"
#include "games/minesweeper.h"

#include "launchpad/launchpad.h"

#include "launchpad/synthlaunchpad.h"

#include "canvasshaders/canvasshader.h"
#include "canvasshaders/warpshader.h"
#include "canvasshaders/mandelbrotshader.h"
#include "canvasshaders/flamevisualizer.h"
#include "voxeltracer/volray.h"



struct FpsPanel : public GraphPanel
{
  std::vector<double> fpsGraph;

  double updateIntervalSecs = 0.2;
  double timer = 0;
  double fps = 0, fpsTmp = 0;
  double fpsMax = 60.0;

  FpsPanel() {
    fpsGraph.resize(300, 0);
    this->numHorizontalMarks = 0;
    this->numVerticalMarks = 0;
    this->horizontalAxisLimits.set(0, fpsGraph.size());
    this->verticalAxisLimits.set(0, 1);
    this->numHorizontalAxisLabels = 0;
    this->horizontalAxisUnit = "s";
    this->data = &fpsGraph;
    this->prerenderingNeeded = true;
  }

  void update(double dt) {
    timer += dt;
    this->fpsTmp += 1.0;

    if(timer >= updateIntervalSecs) {
      this->fps = fpsTmp / timer;
      fpsTmp = 0;
      fpsMax = max(fps, fpsMax);

      for(int i=0; i<fpsGraph.size()-1; i++) {
        fpsGraph[i] = fpsGraph[i+1];
      }
      fpsGraph[fpsGraph.size()-1] = this->fps / fpsMax;

      prerenderingNeeded = true;
      timer = 0;
    }
  }

  void onPrerender(GeomRenderer &geomRenderer, TextGl &textRenderer, const Vec2d &displacement) {
    GraphPanel::onPrerender(geomRenderer, textRenderer, displacement);

    textRenderer.setColor(1, 1, 1, 1);
    char buf[128];
    double lineHeight = 18;
    std::sprintf(buf, "FPS %.2f", fps);
    textRenderer.print(buf, displacement.x + 10, displacement.y + 6, 10);
  }
};




struct Sketch
{
  int screenW = 1200, screenH = 800, screenS = 0;
  int fullScreen = 0;
  int frameless = 1;
  int useOpenGL = 1;
  int inputGrabbed = 0;
  int alwaysOnTop = 0;
  bool minimalFont = false;
  double aspectRatio;
  SDLInterface *sdlInterface; // TODO get rid of the pointer
  PortAudioInterface *paInterface;
  MidiInterface midiInterface;
  Events events;
  CommandLineArguments cliArgs;

  FastNoise basicNoise;

  const std::string infoMessage = "Playground 0.1.1 2018-2019 Lassi Palmujoki\n" \
                                  "Experiments with math, physics, visuals, audio and magic";

  int breakTimeMillis = 0;
  
  //Fontxx fontxx;
  TextGl textRenderer;
  GeomRenderer geomRenderer;

  Console console;

  FrameTimer timer;
  double time, dt;

  Scene *scene;

  // FIXME these shoudlnt't be here
  Events::InputVariableBind<double> *scaleBind;
  Events::InputVariableBind<Vec2d> *positionBind;


  CommandTemplate quitCmdTmplt, helpCmdTmplt;
  CommandTemplate printTextInputToTerminalCmdTmplt;
  CommandTemplate setTextSizePromptCmdTmplt, setTextSizeMessagesCmdTmplt;
  CommandTemplate reloadShadersCmdTmpl;
  CommandTemplate setVsyncCmdTmpl;
  CommandTemplate printGuiTreeCmdTmpl;

  GuiElement guiRoot;

  FpsPanel *fpsPanel = nullptr;

  const std::string fftwWisdomFilename = "data/fftw_wisdom";

  virtual ~Sketch() {
    delete paInterface;
    delete sdlInterface;
    delete scene;
    delete scaleBind;
    delete positionBind;
  }

  void init(CommandLineArguments &cliArgs)
  {
    setlocale(LC_ALL, "fi_FI.utf8");

    this->cliArgs = cliArgs;

    if(cliArgs.hasKey("-size")) {
      int w = 0, h = 0;
      if(cliArgs.numValues("-size") > 0) {
        w = atoi(cliArgs.getValues("-size")[0].c_str());
        h = w;
        if(cliArgs.numValues("-size") > 1) {
          h = atoi(cliArgs.getValues("-size")[1].c_str());
        }
      }
      if(w > 0 && h > 0) {
        screenW = w;
        screenH = h;
      }
    }

    if(cliArgs.hasKey("-fullscreen")) {
      fullScreen = 2;
      if(cliArgs.numValues("-fullscreen") > 0) {
        int t = atoi(cliArgs.getValues("-fullscreen")[0].c_str());
        if(t == 1) {
          fullScreen = 1;
        }
      }
    }
    if(cliArgs.hasKey("-frameless")) {
      frameless = true;
    }
    if(cliArgs.hasKey("-ontop")) {
      alwaysOnTop = 1;
    }
    if(cliArgs.hasKey("-nograb")) {
      inputGrabbed = 0;
    }
    if(cliArgs.hasKey("-minfont")) {
      minimalFont = true;
    }

    if(fftw_import_wisdom_from_filename(fftwWisdomFilename.c_str())) {
      printf("Imported FFTW wisdom from file '%s'.\n", fftwWisdomFilename.c_str());
    }

    sdlInterface = new SDLInterface();

    sdlInterface->setup(screenW, screenH, fullScreen, useOpenGL, frameless, 0, inputGrabbed, alwaysOnTop);
    screenW = sdlInterface->screenW;
    screenH = sdlInterface->screenH;
    screenS = sdlInterface->screenS;
    aspectRatio = sdlInterface->aspectRatio;

    glSetup(sdlInterface);

    scene = new Scene(screenW, screenH, screenS, aspectRatio);

    events.init(scene);


    scaleBind = new Events::InputVariableBind<double>(&scene->worldScale, Events::dragModeExponential, 0.02, Events::releaseModeToOne, Events::buttonRight, Events::modifiersLControl | Events::modifiersLShift | Events::modifiersLAlt);
    scaleBind->active = false;
    events.inputBoundDoubles.push_back(scaleBind);

    positionBind = new Events::InputVariableBind<Vec2d>(&scene->worldPosition, &scene->worldScale, Events::dragModeLinear, Vec2d(-2.0/screenH, -2.0/screenH), Events::releaseModeToMouseNormCoords, Events::buttonLeft, Events::modifiersLControl | Events::modifiersLShift | Events::modifiersLAlt);
    positionBind->active = false;
    events.inputBoundVec2ds.push_back(positionBind);


    paInterface = new PortAudioInterface();

    midiInterface.setup();

    // FIXME this is a hack without which the new font renderer won't work (fix SDL or OpenGL initiation)
    SDL_DestroyTexture(SDL_CreateTexture(sdlInterface->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 1, 1));

    if(!minimalFont) {
      textRenderer.load("data/fonts/asana/asana_math_regular_65.fnt", "data/fonts/asana/asana_math_regular_65.PNG");

      textRenderer.load("data/fonts/asana/asana_math_regular_48.fnt", "data/fonts/asana/asana_math_regular_48.PNG");

      textRenderer.load("data/fonts/asana/asana_math_regular_32.fnt", "data/fonts/asana/asana_math_regular_32.PNG");

      textRenderer.load("data/fonts/asana/asana_math_regular_24.fnt", "data/fonts/asana/asana_math_regular_24.PNG");

      textRenderer.load("data/fonts/asana/asana_math_regular_20.fnt", "data/fonts/asana/asana_math_regular_20.PNG");

      textRenderer.load("data/fonts/asana/asana_math_regular_17.fnt", "data/fonts/asana/asana_math_regular_17.PNG");

      textRenderer.load("data/fonts/asana/asana_math_regular_14.fnt", "data/fonts/asana/asana_math_regular_14.PNG");


      textRenderer.load("data/fonts/asana/asana_math_regular_10.fnt", "data/fonts/asana/asana_math_regular_10.PNG");
    }


    textRenderer.load("data/fonts/asana/asana_math_regular_12.fnt", "data/fonts/asana/asana_math_regular_12.PNG");

    console.init(&textRenderer, sdlInterface);

    quitCmdTmplt.finishInitialization("quit");
    commandQueue.addCommandTemplate(&quitCmdTmplt);

    helpCmdTmplt.addArgument("topic", STR_STRING, "commands");
    helpCmdTmplt.finishInitialization("help");
    commandQueue.addCommandTemplate(&helpCmdTmplt);

    printTextInputToTerminalCmdTmplt.addArgument("on", STR_BOOL, "true");
    printTextInputToTerminalCmdTmplt.finishInitialization("printTextInputToTerminal");
    commandQueue.addCommandTemplate(&printTextInputToTerminalCmdTmplt);

    setTextSizePromptCmdTmplt.addArgument("size", STR_INT, "12");
    setTextSizePromptCmdTmplt.finishInitialization("setTextSizePrompt");
    commandQueue.addCommandTemplate(&setTextSizePromptCmdTmplt);

    setTextSizeMessagesCmdTmplt.addArgument("size", STR_INT, "10");
    setTextSizeMessagesCmdTmplt.finishInitialization("setTextSizeMessages");
    commandQueue.addCommandTemplate(&setTextSizeMessagesCmdTmplt);

    reloadShadersCmdTmpl.finishInitialization("reloadShaders");
    commandQueue.addCommandTemplate(&reloadShadersCmdTmpl);

    printGuiTreeCmdTmpl.finishInitialization("printGuiTree");
    commandQueue.addCommandTemplate(&printGuiTreeCmdTmpl);

    setVsyncCmdTmpl.addArgument("value", STR_BOOL);
    setVsyncCmdTmpl.finishInitialization("setVsync");
    commandQueue.addCommandTemplate(&setVsyncCmdTmpl);

    basicNoise.SetSeed(64573);
    basicNoise.SetNoiseType(FastNoise::SimplexFractal);

    Random::init();
    timer.reset();

    // FIXME hyi vittu korjaa tää ja äkkiä
    drawLine(200, 450, 500, 20, sdlInterface);
    clear(0, 0, 0, 1);
    updateScreen(sdlInterface);

    geomRenderer.create();
    geomRenderer.screenW = screenW;
    geomRenderer.screenH = screenH;
    //glViewport

    std::locale::global(std::locale("en_US.UTF8"));

    guiRoot.name = "Gui root";

    fpsPanel = new FpsPanel();
    fpsPanel->setSize(100, 60);
    fpsPanel->setPosition(10, 10);
    fpsPanel->setVisible(false);
    guiRoot.addChildElement(fpsPanel);

    
    initOscillatorGlobals();

    onInit();

    //SDL_CaptureMouse(SDL_TRUE);
  }

  void update(){

    for(int i=0; i<commandQueue.commands.size(); i++) {
      Command *cmd = NULL;

      if(cmd = commandQueue.popCommand(&quitCmdTmplt)) {
        events.quitRequested = true;
        delete cmd;
      }

      if(cmd = commandQueue.popCommand(&helpCmdTmplt)) {
        std::string value;
        helpCmdTmplt.fillValues(cmd, &value);
        if(value.compare("info") == 0) {
          messageQueue.addMessage(infoMessage);
        }
        if(value.compare("commands") == 0) {

          for(int i=0; i<commandQueue.commandTemplates.size(); i++) {
            messageQueue.addMessage(commandQueue.commandTemplates[i]->strTemplate);
          }
        }
        delete cmd;
      }

      if(cmd = commandQueue.popCommand(&printTextInputToTerminalCmdTmplt)) {
        bool value = false;
        printTextInputToTerminalCmdTmplt.fillValues(cmd, &value);
        events.textInput.printToTerminal = value;
        delete cmd;
      }

      if(cmd = commandQueue.popCommand(&setVsyncCmdTmpl)) {
        bool value = false;
        setVsyncCmdTmpl.fillValues(cmd, &value);
        sdlInterface->setVsync(value);
        delete cmd;
      }

      if(cmd = commandQueue.popCommand(&setTextSizePromptCmdTmplt)) {
        int value = 0;
        setTextSizePromptCmdTmplt.fillValues(cmd, &value);
        console.textSizePrompt = value;
        console.init(&textRenderer, sdlInterface);
        delete cmd;
      }

      if(cmd = commandQueue.popCommand(&setTextSizeMessagesCmdTmplt)) {
        int value = 0;
        setTextSizeMessagesCmdTmplt.fillValues(cmd, &value);
        console.textSizeMessages = value;
        console.init(&textRenderer, sdlInterface);
        delete cmd;
      }

      if(cmd = commandQueue.popCommand(&reloadShadersCmdTmpl)) {
        reloadShaders();
        delete cmd;
      }
      
      if(cmd = commandQueue.popCommand(&printGuiTreeCmdTmpl)) {
        guiRoot.print();
        delete cmd;
      }
    }

    timer.update();
    time = timer.time;
    dt = timer.dt;
    pollSdlEvents(events);
    midiInterface.update();
    paInterface->update();

    if(fpsPanel->isVisible) {
      fpsPanel->update(dt);
    }

    onUpdate();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
    onDraw();
    //glLoadIdentity();
    if(guiRoot.checkInputGrab() || console.inputGrabbed) {
      if(!events.textInput.inputGrabbed) {
        events.startTextInput();
      }
    }
    else if(events.textInput.inputGrabbed) {
      events.endTextInput();
    }
    // FIXME get rid of prepare()
    guiRoot.update(time, dt);
    guiRoot.prepare(geomRenderer, textRenderer);
    
    double currentZLayer = 0, nextZLayer = 0;
    while(true) {
      currentZLayer = nextZLayer;
      guiRoot.render(geomRenderer, textRenderer, currentZLayer, nextZLayer);
      if(currentZLayer == nextZLayer) {
        break;
      }
    }
    
    
    updateScreen(sdlInterface);
    messageQueue.update();
    
    std::this_thread::sleep_for(std::chrono::milliseconds(breakTimeMillis));
  }

  void hibernate() {
      bool done = false;
      printf("Hibernation started\n");
      while(!done) {
        pollSdlEvents(events);
        if(events.sdlKeyCode == SDLK_SPACE) done = true;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
      }
      printf("Hibernation ended\n");
  }
  
  void quit(){
    if(fftw_export_wisdom_to_filename(fftwWisdomFilename.c_str())) {
      printf("Exported FFTW wisdom to file '%s'.\n", fftwWisdomFilename.c_str());
    }

    onQuit();
    guiRoot.finalize();
    paInterface->quit();
    midiInterface.quit();
    sdlInterface->quit();
  }

  double noiset(double freq, int id = 0) {
    double d = sin(id) * pow(id, 2) * id;
    return basicNoise.GetNoise(d * 432.446, d + time*freq);
  }
  double noisetn(double freq, int id = 0) {
    double d = sin(id) * pow(id, 2) * id;
    return (basicNoise.GetNoise(d * 432.446, d + time*freq) + 1.0) * 0.5;
  }
  void reloadShaders() {
    geomRenderer.reloadShaders();
    initBoxShadowShader(true);
    initSpriteShadowShader(true);
    onReloadShaders();
  }
  
  virtual void onInit() {}
  virtual void onQuit() {}
  virtual void onUpdate() {}
  virtual void onDraw() {}
  virtual void onReloadShaders() {}

  virtual void onMouseMotion() {}
  virtual void onMousePressed() {}
  virtual void onMouseReleased() {}
  virtual void onMouseWheel() {}
  virtual void onKeyUp() {}
  virtual void onKeyDown() {}
  virtual void onQuitRequested() {}



  void pollSdlEvents(Events &events)
  {
    SDL_Event e;
    while(SDL_PollEvent(&e)) {
      if(e.type == SDL_QUIT) {
        events.quitRequested = true;
        this->onQuitRequested();
      }
      if(e.type == SDL_MOUSEMOTION) {
        events.mp.x = events.pMouseX = events.mouseX;
        events.mp.y = events.pMouseY = events.mouseY;
        events.mwp = events.mw;
        events.m.x = events.mouseX = e.motion.x;
        events.m.y = events.mouseY = e.motion.y;
        events.md.x = e.motion.xrel;
        events.md.y = e.motion.yrel;
        /*events.mouseDragL = e.motion.state & SDL_BUTTON_LMASK > 0;
        events.mouseDragM = e.motion.state & SDL_BUTTON_MMASK > 0;
        events.mouseDragR = e.motion.state & SDL_BUTTON_RMASK > 0;*/

        if(events.cameraMatrix) {
          events.mw = (*events.cameraMatrix) * events.m;
          //printf("%.2f, %.2f, %.2f, %.2f\n", events.m.x, events.mw.x, events.m.y, events.mw.y);
        }
        else {
          events.mw = events.m;
        }

        guiRoot.onMouseMotion(events);
        guiRoot.checkInputGrab();

        if(!guiRoot.isInputGrabbedAnywhere) {
          for(auto &m : events.inputBoundDoubles) { m->mouseMotion(events); }
          for(auto &m : events.inputBoundVec2ds) { m->mouseMotion(events); }
          this->onMouseMotion();
        }
        events.md.x = 0;
        events.md.y = 0;
      }

      if(e.type == SDL_MOUSEBUTTONDOWN) {
        if(e.button.button == SDL_BUTTON_LEFT) {
          events.mouseDownL = true;
          events.mouseNowDownL = true;
          events.mouseButton = 0;
        }
        if(e.button.button == SDL_BUTTON_MIDDLE) {
          events.mouseDownM = true;
          events.mouseNowDownM = true;
          events.mouseButton = 2;
        }
        if(e.button.button == SDL_BUTTON_RIGHT) {
          events.mouseDownR = true;
          events.mouseNowDownR = true;
          events.mouseButton = 1;
        }
        events.sdlMouseButton = e.button.button;


        events.dragStartM = events.m;

        guiRoot.onMousePressed(events);
        guiRoot.checkInputGrab();

        if(!guiRoot.isInputGrabbedAnywhere) {
          for(auto &m : events.inputBoundDoubles) { m->mousePressed(events); }
          for(auto &m : events.inputBoundVec2ds) { m->mousePressed(events); }
          this->onMousePressed();
        }

        if(e.button.button == SDL_BUTTON_LEFT) events.mouseNowDownL = false;
        if(e.button.button == SDL_BUTTON_MIDDLE) events.mouseNowDownM = false;
        if(e.button.button == SDL_BUTTON_RIGHT) events.mouseNowDownR = false;
        events.mouseButton = -1;
      }

      if(e.type == SDL_MOUSEBUTTONUP) {
        if(e.button.button == SDL_BUTTON_LEFT) {
          events.mouseUpL = true;
          events.mouseDownL = false;
          events.mouseButton = 0;
        }
        if(e.button.button == SDL_BUTTON_MIDDLE) {
          events.mouseUpM = true;
          events.mouseDownM = false;
          events.mouseButton = 2;
        }
        if(e.button.button == SDL_BUTTON_RIGHT) {
          events.mouseUpR = true;
          events.mouseDownR = false;
          events.mouseButton = 1;
        }

        guiRoot.onMouseReleased(events);
        guiRoot.checkInputGrab();

        if(!guiRoot.isInputGrabbedAnywhere) {
          for(auto &m : events.inputBoundDoubles) { m->mouseReleased(events); }
          for(auto &m : events.inputBoundVec2ds) { m->mouseReleased(events); }
          this->onMouseReleased();
        }


        if(e.button.button == SDL_BUTTON_LEFT) events.mouseUpL = false;
        if(e.button.button == SDL_BUTTON_MIDDLE) events.mouseUpM = false;
        if(e.button.button == SDL_BUTTON_RIGHT) events.mouseUpR = false;
        events.mouseButton = -1;
      }

      if(e.type == SDL_MOUSEWHEEL) {
        events.mouseWheel = e.wheel.y;


        if(!guiRoot.isInputGrabbedAnywhere) {
          for(auto &m : events.inputBoundDoubles) { m->mouseWheel(events); }
          for(auto &m : events.inputBoundVec2ds) { m->mouseWheel(events); }
          console.onMouseWheel(events);
          this->onMouseWheel();
        }

        guiRoot.onMouseWheel(events);
      }

      if(e.type == SDL_KEYUP) {
        events.sdlKeyCode = e.key.keysym.sym;
        if(events.sdlKeyCode == SDLK_LSHIFT) {
          events.lShiftDown = false;
        }
        if(events.sdlKeyCode == SDLK_RSHIFT) {
          events.rShiftDown = false;
        }
        if(events.sdlKeyCode == SDLK_LCTRL) {
          events.lControlDown = false;
        }
        if(events.sdlKeyCode == SDLK_RCTRL) {
          events.rControlDown = false;
        }
        if(events.sdlKeyCode == SDLK_LALT) {
          events.lAltDown = false;
        }
        if(events.sdlKeyCode == SDLK_RALT) {
          events.rAltDown = false;
        }
        events.numModifiersDown = 0;

        if(events.lShiftDown) {
          events.numModifiersDown++;
        }
        if(events.rShiftDown) {
          events.numModifiersDown++;
        }
        if(events.lControlDown) {
          events.numModifiersDown++;
        }
        if(events.rControlDown) {
          events.numModifiersDown++;
        }
        if(events.lAltDown) {
          events.numModifiersDown++;
        }
        if(events.rAltDown) {
          events.numModifiersDown++;
        }

        if(events.numModifiersDown == 0) {
          events.noModifiers = true;
        }
        else {
          events.noModifiers = false;
        }

        if(events.textInput.inputGrabbed) {
          //events.textInput.onKeyUp();
        }
        else {
          if(!guiRoot.isInputGrabbedAnywhere) {
            this->onKeyUp();
          }
        }

        guiRoot.onKeyUp(events);

      }

      if(e.type == SDL_KEYDOWN) {
        events.sdlKeyCode = e.key.keysym.sym;
        if(events.noModifiers && events.sdlKeyCode == SDLK_ESCAPE) {
          inputGrabbed = 1-inputGrabbed;
          SDL_SetWindowGrab(sdlInterface->window, (SDL_bool)inputGrabbed);
        }
        if(events.lControlDown && events.sdlKeyCode == SDLK_ESCAPE) {
          events.quitRequested = true;
        }

        if(events.sdlKeyCode == SDLK_LSHIFT) {
          events.lShiftDown = true;
        }
        if(events.sdlKeyCode == SDLK_RSHIFT) {
          events.rShiftDown = true;
        }
        if(events.sdlKeyCode == SDLK_LCTRL) {
          events.lControlDown = true;
        }
        if(events.sdlKeyCode == SDLK_RCTRL) {
          events.rControlDown = true;
        }
        if(events.sdlKeyCode == SDLK_LALT) {
          events.lAltDown = true;
        }
        if(events.sdlKeyCode == SDLK_RALT) {
          events.rAltDown = true;
        }
        events.numModifiersDown = 0;

        if(events.lShiftDown) {
          events.numModifiersDown++;
        }
        if(events.rShiftDown) {
          events.numModifiersDown++;
        }
        if(events.lControlDown) {
          events.numModifiersDown++;
        }
        if(events.rControlDown) {
          events.numModifiersDown++;
        }
        if(events.lAltDown) {
          events.numModifiersDown++;
        }
        if(events.rAltDown) {
          events.numModifiersDown++;
        }

        if(events.numModifiersDown == 0) {
          events.noModifiers = true;
        }
        else {
          events.noModifiers = false;
        }

        if(events.textInput.inputGrabbed) {
          events.textInput.onKeyDown(events);
          guiRoot.onTextInput(events);
        }
        else {
          if(!guiRoot.isInputGrabbedAnywhere) {
            this->onKeyDown();
          }
        }
        if(console.inputGrabbed) {
          console.onTextInput(events);
        }
        
        if(events.sdlKeyCode == SDLK_h && events.lControlDown && events.lShiftDown) {
            hibernate();
        }
        if(events.sdlKeyCode == SDLK_b && events.lControlDown && events.lShiftDown) {
            breakTimeMillis += 3;
            if(breakTimeMillis > 30) breakTimeMillis = 0;
            printf("Break time millis %d\n", breakTimeMillis);
        }
    
    
        if(events.sdlKeyCode == SDLK_f && (events.rControlDown && events.lControlDown)) {
          fpsPanel->toggleVisibility();
        }

        if(events.sdlKeyCode == SDLK_p && (events.rControlDown || events.lControlDown)) {
          guiRoot.disableInputGrab();
          console.minimalInterface = events.lShiftDown || events.rShiftDown;
          console.inputGrabbed = !console.inputGrabbed;
          events.textInput.isCommandPrompt = console.inputGrabbed;
          if(console.inputGrabbed) {
            events.textInput.setInputText(console.inputText, console.inputTextToCursor);
          }
        }
        if(events.sdlKeyCode == SDLK_r && events.rControlDown && events.lControlDown) {
          guiRoot.print();
        }

        guiRoot.onKeyDown(events);
      }
      if(e.type == SDL_TEXTINPUT) {
          //events.inputText = e.text.text;
          //printf("INPUT\n%s\n\n", events.inputText.c_str());
          if(events.textInput.inputGrabbed) {
            events.textInput.onTextInput(e.text.text);
            if(console.inputGrabbed) {
              console.onTextInput(events);
            }
            else {
              guiRoot.onTextInput(events);
            }
          }
      }


      // NOTE not working...
      /*if(e.type == SDL_TEXTEDITING) {
          events.inputTextComposition = e.edit.text;
          events.inputTextStart = e.edit.start;
          events.inputTextSelectionLength = e.edit.length;
          events.inputTextCursorPos = e.edit.start;
          printf("EDITING\n%s\nstart: %d\nlength: %d\ncursor: %d\n\n", events.inputTextComposition.c_str(), events.inputTextStart, events.inputTextSelectionLength, events.inputTextCursorPos);
      }*/

    }
  }

};
