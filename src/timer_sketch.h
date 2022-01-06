#pragma once
#include "engine/sketch.h"


struct TimerSketch : public Sketch
{
  Quadx quad;
  GlShader shader;

  FastNoise fastNoise;

  Synth *synth;

  Texture bar;

  double totalTime = 1;
  double timer = 0;
  bool timerRunning = false;

  bool showHours = false;

  int prevMin = 0;

  int fontSize = 65;

  const char *minSoundModes[3] = {"complex", "simple", "nothing"};
  int minSoundMode = 1;
  double minSoundVolume = 0.3;
  bool endSound = true;

  void onInit() {

    if(cliArgs.hasKey("-hours")) {
      showHours = true;
    }


    fastNoise.SetSeed(64573);
    fastNoise.SetNoiseType(FastNoise::SimplexFractal);


    double sampleRate = 44100;
    int framesPerBuffer = 128;
    double suggestedOutputLatency = -1;
    int suggestedAudioDevice = -1;

    if(cliArgs.numValues("-samplerate") > 0) {
      sampleRate = atof(cliArgs.getValues("-samplerate")[0].c_str());
    }
    if(cliArgs.numValues("-framesperbuffer") > 0) {
      framesPerBuffer = atof(cliArgs.getValues("-framesperbuffer")[0].c_str());
    }
    if(cliArgs.numValues("-latency") > 0) {
      suggestedOutputLatency = atof(cliArgs.getValues("-latency")[0].c_str());
    }
    if(cliArgs.numValues("-device") > 0) {
      suggestedAudioDevice = atoi(cliArgs.getValues("-device")[0].c_str());
    }
    synth = new Synth();
    synth->setup(sampleRate, framesPerBuffer, suggestedOutputLatency, screenW, screenH, suggestedAudioDevice);
    paInterface = synth;


    //synth->loadInstrumentFromFile("synth0006");
    synth->loadInstrumentFromFile("percussive0012");

    synth->activeInstrumentIndex = synth->instruments.size() - 1;

    synth->getInstrument()->isActive = true;

    synth->numCurrentlyPlayingNotes = 40;
/*
    if(synth->instruments.size() == 0) {
      synth->addInstrument();
    }
    synth->getInstrument()->numVoices = 1;
    synth->getInstrument()->voices[0].waveForm.type = WaveForm::Type::Sin;
    synth->getInstrument()->voices[0].waveForm.waveFormArgs[WaveForm::Type::Sin][0] = 9.0;
    synth->getInstrument()->voices[0].waveForm.partialSet.numPartials = 5;

    synth->getInstrument()->numEnvelopes = 1;
    synth->getInstrument()->envelopes[0].setPreset(epAHE02);

    synth->getInstrument()->voices[0].update();*/

    //glSetup();

    //glScene.create();
    //shader.create("data/glsl/basic.vert", "data/glsl/hsv.frag");
    //quad.create(screenW, screenH);

    bar.create(10, 10);
    bar.clear(1, 1, 1, 1);

    //setAlphaBlending(true, sdlInterface);

  }


  void onQuit() {
  }

  void onKeyUp() {
  }

  void onKeyDown() {
    if(events.sdlKeyCode == SDLK_t) {
      //synth->startNote(60);
    }
    if(events.sdlKeyCode == SDLK_DOWN) {
      timer -= 60;
      timer = max(timer, 0.0);
    }
    if(events.sdlKeyCode == SDLK_UP) {
      timer += 60;
    }
    if(events.sdlKeyCode == SDLK_LEFT) {
      timer -= 1;
      timer = max(timer, 0.0);
    }
    if(events.sdlKeyCode == SDLK_RIGHT) {
      timer += 1;
    }
    if(events.sdlKeyCode == SDLK_SPACE) {
      timerRunning = !timerRunning;
      synth->stopAllNotes(false);
      int s = (int) timer;
      int hours = s / 60 / 60;
      int mins = (s-hours*60*60) / 60;
      double secs = timer - hours*60*60 - mins*60;
      prevMin = mins;
    }
    if(events.sdlKeyCode == SDLK_z) {
      timer = ((int)timer)/60 * 60;
    }
    if(events.sdlKeyCode == SDLK_x) {
      timer = 0;
    }
    if(events.sdlKeyCode == SDLK_c) {
      totalTime = timer;
    }

    if(events.sdlKeyCode == SDLK_h) {
      showHours = !showHours;
    }

    if(events.sdlKeyCode == SDLK_f && events.noModifiers) {
      fontSize = max(fontSize-4, 13);
    }
    if(events.sdlKeyCode == SDLK_f && events.lShiftDown) {
      fontSize = fontSize+4;
    }

    if(events.sdlKeyCode == SDLK_e) {
      endSound = !endSound;
      printf("end alert %s\n", endSound ? "enabled" : "disabled");
    }
    if(events.sdlKeyCode == SDLK_w) {
      minSoundMode = (minSoundMode+1) % 3;
      minSoundVolume = map(events.mouseX, 0.0, screenW-1, 0.0, 1.0);
      printf("minute change alert %s (vol %.4f)\n", minSoundModes[minSoundMode], minSoundVolume);
    }
  }

  void onMouseWheel() {
    if(events.mouseWheel < 0) {
      timer -= events.lControlDown || events.rControlDown ? 1 : 60;
      timer = max(timer, 0.0);
    }
    if(events.mouseWheel > 0) {
      timer += events.lControlDown || events.rControlDown ? 1 : 60;
    }
  }

  void onMousePressed() {

  }

  void onMouseReleased() {
  }

  void onMouseMotion() {
  }

  void onUpdate() {

  }
  double scale1[6] = {2, 3, 5, 7, 8, 11};
  double scale2[5] = {0, 2, 4, 7, 9};

  void onDraw() {
    if(timerRunning && timer >= 0) {
      timer -= dt;
    }
    if(timer < 0) {
      timerRunning = false;
      timer = 0;
      if(endSound) {
        double t = 1.2;
        for(int i=0; i<6; i++) {
          synth->playNote(60, minSoundVolume, 0, t*i + 0);
          synth->playNote(60, minSoundVolume, 0, t*i + 0.2);
          synth->playNote(60, minSoundVolume, 0, t*i + 0.4);
          synth->playNote(60, minSoundVolume, 0, t*i + 0.6);
        }
        /*double p = 60.0-24.0-12.0;
        int s = 5;
        for(int i=0; i<26; i++) {
          int ind = i % s;
          if(ind == 0) p += 12.0;
          //synth->startNoteRelativeTime(p+scale2[ind], 0.6, 0.15*i);
          synth->playNote(p+scale2[ind], minSoundVolume, 0.3, 0.15*i);
        }*/
      }
    }

    wchar_t buf[256];

    int s = (int) timer;
    int hours = s / 60 / 60;
    int mins = (s-hours*60*60) / 60;
    double secs = timer - hours*60*60 - mins*60;

    if(timerRunning && mins != prevMin) {
      prevMin = mins;
      if(minSoundMode == 0) {
        double t = 1.8;
        for(int i=0; i<6; i++) {
          synth->playNote(60, minSoundVolume, 0, t*i + 0);
          synth->playNote(60, minSoundVolume, 0, t*i + 0.3);
          synth->playNote(60, minSoundVolume, 0, t*i + 0.6);
          synth->playNote(60, minSoundVolume, 0, t*i + 0.9);
        }

/*        double p = 60.0-36.0;
        int s = 6;
        for(int i=0; i<5; i++) {
          int ind = i % s;
          if(ind == 0) {
            p += 12.0;
          }
          //synth->startNoteRelativeTime(p+scale1[ind], minSoundVolume, 0.1*i);
          synth->playNote(p+scale1[ind], minSoundVolume, 0.1, 0.1*i);
          if(i != 11) {
            //synth->startNoteRelativeTime(p+scale1[ind], minSoundVolume, 0.1*(7-i));
            synth->playNote(p+scale1[ind], minSoundVolume, 0.1, 0.1*(7-i));
          }
        }*/
      }
      if(minSoundMode == 1) {
        synth->playNote(60, minSoundVolume, 0, 0);
        synth->playNote(60, minSoundVolume, 0, 0.2);
        synth->playNote(60, minSoundVolume, 0, 0.4);

        /*synth->playNote(60-36, minSoundVolume, 0.5, 0);
        synth->playNote(60-36+7, minSoundVolume, 0.5, 0);
        synth->playNote(60-24, minSoundVolume, 0.5, 0);
        synth->playNote(60-24+3, minSoundVolume, 0.5, 0);
        synth->playNote(60-12, minSoundVolume, 0.5, 0);
        synth->playNote(60, minSoundVolume, 0.5, 0);
        synth->playNote(60+3, minSoundVolume, 0.5, 0);
        synth->playNote(60+12, minSoundVolume, 0.5, 0);
        synth->playNote(60+12+7, minSoundVolume, 0.5, 0);*/
      }
    }
    sdlInterface->glmMatrixStack.loadIdentity();
    //glLoadIdentity();


    bool inverseColors = mins % 2 == 0;

    if(inverseColors) clear(1, 1, 1, 1);
    else clear(0, 0, 0, 1);

    if(showHours) {
      std::swprintf(buf, 256, L"%2d:%02d:%05.2f", hours, mins, secs);
    }

    else {
      std::swprintf(buf, 256, L"%02d:%05.2f", mins, secs);
    }

    Vec2d size = textRenderer.getDimensions(buf, fontSize, 1);

    if(!inverseColors) textRenderer.setColor(1, 1, 1, 1);
    else textRenderer.setColor(0, 0, 0, 1);



    textRenderer.print(buf, screenW*0.5 - size.x*0.5, screenH*0.5 - size.y*0.36, fontSize, 1);

    if(timer > 0 && totalTime > 0) {
      if(!inverseColors) setColor(1, 1, 1, 1, sdlInterface);
      else setColor(0, 0, 0, 1, sdlInterface);
      double x = map(timer, totalTime, 0.0, 0.0, screenW-1.0);
      //fillRectCorners(x, screenH-20, screenW, screenH, sdlInterface);
      //bar.renderCorners(x, screenH-20, screenW+1, screenH);
    }

    setColor(1, 1, 1, 1, sdlInterface);
  }





};
