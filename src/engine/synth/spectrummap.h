#pragma once


struct SpectrumMap
{
  Texture spectrumImg[2];
  int activeSpectrumImg = 0;
  int spectrumImgCurrentOffset = 0;
  int updateInterval = 1;

  double spectrumMapScaling = 1;

  enum SpectrumShareMode { ShareOriginal, ShareAdjusted, DontShare };
  std::vector<std::string> spectrumShareModeNames = { "Share original", "Share adjusted", "Use custom" };
  SpectrumShareMode spectrumShareMode = ShareOriginal;

  //bool shareGraphSpectrumTransform = false;

  const std::vector<std::string> spectrum2dColorMapNames = {"Grayscale", "Warmth"};
  int colorMode = 2;
  Vec3d customGradientParameters = Vec3d(-0.45, 2.0, 0.6);
  std::vector<ColorMap> spectrum2dColorMaps;

  std::vector<double> spectrum;

  FFTW3Interface delayLineFFTW;

  bool isInputActive = false;

  unsigned long frameCounter = 0;

  void init(GuiElement *parentGuiElement) {
    delayLineFFTW.initialize(1024*64, false);

    addPanel(parentGuiElement);
    panel->setVisible(false);
  }


  void prepareSpectrumColorMaps() {
    spectrum2dColorMaps.resize(2);

    spectrum2dColorMaps[0].colors.push_back(Vec3d(0, 0, 0));
    spectrum2dColorMaps[0].colors.push_back(Vec3d(1, 1, 1));
    spectrum2dColorMaps[0].prepareLookUpTable();
    spectrum2dColorMaps[0].prepareColorBar(20, 256);

    spectrum2dColorMaps[1].colors.push_back(Vec3d(0, 0, 0));
    spectrum2dColorMaps[1].colors.push_back(Vec3d(36.0/255.0, 37.0/255.0, 255.0/255.0));
    spectrum2dColorMaps[1].colors.push_back(Vec3d(131.0/255.0, 0/255.0, 234.0/255.0));
    spectrum2dColorMaps[1].colors.push_back(Vec3d(210.0/255.0, 16.0/255.0, 16.0/255.0));
    spectrum2dColorMaps[1].colors.push_back(Vec3d(1, 1, 0));
    spectrum2dColorMaps[1].colors.push_back(Vec3d(1, 1, 1));
    spectrum2dColorMaps[1].positions.push_back(0);
    spectrum2dColorMaps[1].positions.push_back(0.25);
    spectrum2dColorMaps[1].positions.push_back(0.5);
    spectrum2dColorMaps[1].positions.push_back(0.75);
    spectrum2dColorMaps[1].positions.push_back(0.999);
    spectrum2dColorMaps[1].positions.push_back(1.0);
    spectrum2dColorMaps[1].prepareLookUpTable();
    spectrum2dColorMaps[1].prepareColorBar(20, 256);
  }


  void render() {
    spectrumImg[activeSpectrumImg].render(0, spectrumImg[activeSpectrumImg].h-spectrumImgCurrentOffset);
    spectrumImg[1-activeSpectrumImg].render(0, -spectrumImgCurrentOffset);
  }


  void update(int w, int h, const std::vector<double> &spectrumGraphOriginal, const std::vector<double> &spectrumGraph, DelayLine &delayLine, Vec2d spectrumLimits, bool spectrumAsNotes, int volumeUnit) {
    if(frameCounter++ % max(1, updateInterval) != 0) return;

    if(spectrum.size() != w) {
      spectrum.resize(w);
    }

    if(spectrumShareMode == ShareOriginal && spectrumGraph.size() > 0) {
      if(spectrumGraphOriginal.size() != w) {
        resizeArray(spectrumGraphOriginal, spectrum);
      }
      else {
        spectrum = spectrumGraphOriginal;
      }
      scaleArray(spectrum, spectrumMapScaling);
    }
    else if(spectrumShareMode == ShareAdjusted && spectrumGraph.size() > 0) {
      if(spectrumGraph.size() != w) {
        resizeArray(spectrumGraph, spectrum);
      }
      else {
        spectrum = spectrumGraph;
      }
      scaleArray(spectrum, spectrumMapScaling);
    }
    else {
      for(int i=0; i<delayLineFFTW.getSize(); i++) {
        Vec2d v = delayLine.getOutputSample(i);
        delayLineFFTW.input[i] = v.x;
      }
      delayLineFFTW.transform();

      if(spectrumAsNotes) {
        double minNote = freqToNote(spectrumLimits.x);
        double maxNote = freqToNote(spectrumLimits.y);

        for(int i=0; i<spectrum.size(); i++) {
          double noteBegin = map(i, 0.0, w-1, minNote, maxNote);
          double noteEnd = map(i+1, 0.0, w-1, minNote, maxNote);
          double frequencyMin = noteToFreq(noteBegin);
          double frequencyMax = noteToFreq(noteEnd);
          int f0 = (int)map(frequencyMin, 0.0, delayLine.sampleRate/2.0, 0, delayLineFFTW.output.size()/2);
          int f1 = (int)map(frequencyMax, 0.0, delayLine.sampleRate/2.0, 0, delayLineFFTW.output.size()/2);
          double t = 0;
          for(int k=f0; k<=f1; k++) {
            if(k < 0 || k >= delayLineFFTW.output.size()) {
              printf("Error at SpectrumMap::updateSpectrumImage(), invalid frequency at spectrum at notes conversion, %d\n", k);
            }
            else {
              t += delayLineFFTW.output[k];
            }
          }
          t = t / (1.0 + f1 - f0);
          spectrum[i] = t;
        }
      }
      else {
        int f0 = (int)map(spectrumLimits.x, 0.0, delayLine.sampleRate/2.0, 0, delayLineFFTW.output.size()/2);
        int f1 = (int)map(spectrumLimits.y, 0.0, delayLine.sampleRate/2.0, 0, delayLineFFTW.output.size()/2);

        resizeArray(delayLineFFTW.output, spectrum, f0, f1);
      }
      scaleArray(spectrum, spectrumMapScaling);

      for(int i=0; i<spectrum.size(); i++) {
        if(std::isnan(spectrum[i])) {
          spectrum[i] = 0.0;
        }
        if(volumeUnit == 1) {
          if(spectrum[i] <= 1.0e-5) {
            spectrum[i] = 0;
          }
          else {
            spectrum[i] = log10(spectrum[i]);
            spectrum[i] = (spectrum[i] + 5.0) / 5.0;
          }
        }
      }
    }




    if(spectrum2dColorMaps.size() == 0) {
      prepareSpectrumColorMaps();
    }
    for(int i=0; i<2; i++) {
      if(spectrumImg[i].w != w || spectrumImg[i].h != h) {
        spectrumImg[i].create(w, h);
        spectrumImg[i].clear(0, 0, 0, 1);
      }
    }

    for(int i=0; i<w; i++) {
      //double v = spectrum[map(i, 0, spectrumImg[activeSpectrumImg].w-1, 0, spectrum.size()-1)];
      double v = spectrum[i];
      v = clamp(v, 0.0, 1.0);

      Vec3d color;

      if(colorMode < spectrum2dColorMapNames.size()) {
        color = spectrum2dColorMaps[colorMode].getRGB(v);
      }
      else {
        color = hslToRgb(customGradientParameters.x + customGradientParameters.y*v, customGradientParameters.z, v);
      }
      spectrumImg[activeSpectrumImg].setPixel(i, spectrumImg[activeSpectrumImg].h-1-spectrumImgCurrentOffset, color);
    }

    spectrumImg[activeSpectrumImg].applyPixels(0, spectrumImg[activeSpectrumImg].h-1-spectrumImgCurrentOffset, spectrumImg[activeSpectrumImg].w, 1);

    spectrumImgCurrentOffset++;
    if(spectrumImgCurrentOffset >= spectrumImg[activeSpectrumImg].h) {
      spectrumImgCurrentOffset = 0;
      activeSpectrumImg = 1 - activeSpectrumImg;
    }
  }

  void setActive(bool isActive) {
    this->isInputActive = isActive;
  }


  void onKeyDown(const Events &events) {
    if(!isInputActive) return;

    if(events.sdlKeyCode == SDLK_HOME && panel) {
      panel->toggleVisibility();
    }
  }


  Panel *panel = NULL;
  NumberBox *spectrumResolutionGui = NULL;

  //CheckBox *shareGraphSpectrumTransformGui = NULL;
  ListBox *spectrumShareModeGui = NULL;

  const std::vector<std::string> colorModeNames = { spectrum2dColorMapNames[0], spectrum2dColorMapNames[1], "Custom Gradient" };
  ListBox *colorModeGui = NULL;
  NumberBox *spectrumMapUpdateLagGui = NULL;
  //NumberBox *spectrumAveragingGui = NULL;

  NumberBox *spectrumMapScalingGui = NULL;

  NumberBox *customGradientPar1Gui = NULL;
  NumberBox *customGradientPar2Gui = NULL;
  NumberBox *customGradientPar3Gui = NULL;


  void addPanel(GuiElement *parentElement) {
    if(panel) return;

    panel = new Panel("Spectrum map panel", 350, 590, 10, 100);
    panel->addGuiEventListener(new SpectrumMapPanelListener(this));
    parentElement->addChildElement(panel);

    double line = 10, lineHeight = 23;

    //shareGraphSpectrumTransformGui = new CheckBox("Use spectrum from graph", shareGraphSpectrumTransform, 10, line);
    //panel->addChildElement(shareGraphSpectrumTransformGui);
    panel->addChildElement(spectrumShareModeGui = new ListBox("Spectrum share", 10, line, 14));
    spectrumShareModeGui->setItems(spectrumShareModeNames);
    spectrumShareModeGui->setValue(spectrumShareMode);

    spectrumResolutionGui = new NumberBox("Spectrum transform size", delayLineFFTW.getSize(), NumberBox::INTEGER, 128, 1<<24, 10, line+=lineHeight, 8);
    spectrumResolutionGui->incrementMode = NumberBox::IncrementMode::Power;
    panel->addChildElement(spectrumResolutionGui);

    colorModeGui = new ListBox("Color mode", 10, line+=lineHeight, 14);
    colorModeGui->setItems(colorModeNames);
    colorModeGui->setValue(colorMode);
    panel->addChildElement(colorModeGui);

    spectrumMapScalingGui = new NumberBox("Spectrum map scaling", spectrumMapScaling, NumberBox::FLOATING_POINT, -1e10, 1e10, 10, line += lineHeight, 8);
    panel->addChildElement(spectrumMapScalingGui);

    spectrumMapUpdateLagGui = new NumberBox("Update interval", updateInterval, NumberBox::INTEGER, 1, 100000, 10, line += lineHeight, 8);
    spectrumMapUpdateLagGui->incrementMode = NumberBox::IncrementMode::Linear;
    panel->addChildElement(spectrumMapUpdateLagGui);

    customGradientPar1Gui = new NumberBox("Custom gradient", customGradientParameters.x, NumberBox::FLOATING_POINT, -1e10, 1e10, 10, line += lineHeight, 4, 3);
    customGradientPar2Gui = new NumberBox("", customGradientParameters.y, NumberBox::FLOATING_POINT, -1e10, 1e10, 225, line, 4, 3);
    customGradientPar3Gui = new NumberBox("", customGradientParameters.z, NumberBox::FLOATING_POINT, 0, 1, 305, line, 4, 3);
    customGradientPar1Gui->incrementMode = NumberBox::IncrementMode::Linear;
    customGradientPar1Gui->linearIncrementAmount = 0.1;
    customGradientPar2Gui->incrementMode = NumberBox::IncrementMode::Linear;
    customGradientPar2Gui->linearIncrementAmount = 0.1;
    customGradientPar3Gui->incrementMode = NumberBox::IncrementMode::Linear;
    customGradientPar3Gui->linearIncrementAmount = 0.1;
    panel->addChildElement(customGradientPar1Gui);
    panel->addChildElement(customGradientPar2Gui);
    panel->addChildElement(customGradientPar3Gui);


    panel->size.set(410, line + lineHeight + 10);
  }

  void updatePanel() {
    if(panel) {
      spectrumResolutionGui->setValue(delayLineFFTW.getSize());
      //shareGraphSpectrumTransformGui->setValue(shareGraphSpectrumTransform);
      spectrumShareModeGui->setValue(spectrumShareMode);
      colorModeGui->setValue(colorMode);
      spectrumMapUpdateLagGui->setValue(updateInterval);
      spectrumMapScalingGui->setValue(spectrumMapScaling);
      customGradientPar1Gui->setValue(customGradientParameters.x);
      customGradientPar2Gui->setValue(customGradientParameters.y);
      customGradientPar3Gui->setValue(customGradientParameters.z);
    }
  }

  void removePanel(GuiElement *parentElement) {
    if(!panel) return;
    parentElement->deleteChildElement(panel);
    panel = NULL;
    spectrumResolutionGui = NULL;
    //shareGraphSpectrumTransformGui = NULL;
    spectrumShareModeGui = NULL;
    colorModeGui = NULL;
    spectrumMapUpdateLagGui = NULL;
    spectrumMapScalingGui = NULL;
    customGradientPar1Gui = NULL;
    customGradientPar2Gui = NULL;
    customGradientPar3Gui = NULL;
  }

  struct SpectrumMapPanelListener : public GuiEventListener {
    SpectrumMap *spectrumMap;
    SpectrumMapPanelListener(SpectrumMap *spectrumMap) {
      this->spectrumMap = spectrumMap;
    }
    void onValueChange(GuiElement *guiElement) {
      /*if(guiElement == spectrumMap->shareGraphSpectrumTransformGui) {
        guiElement->getValue((void*)&spectrumMap->shareGraphSpectrumTransform);
      }*/
      if(guiElement == spectrumMap->spectrumShareModeGui) {
        guiElement->getValue((void*)&spectrumMap->spectrumShareMode);
      }
      if(guiElement == spectrumMap->colorModeGui) {
        guiElement->getValue((void*)&spectrumMap->colorMode);
      }
      if(guiElement == spectrumMap->spectrumMapUpdateLagGui) {
        guiElement->getValue((void*)&spectrumMap->updateInterval);
      }
      if(guiElement == spectrumMap->spectrumMapScalingGui) {
        guiElement->getValue((void*)&spectrumMap->spectrumMapScaling);
      }
      if(guiElement == spectrumMap->customGradientPar1Gui) {
        guiElement->getValue((void*)&spectrumMap->customGradientParameters.x);
      }
      if(guiElement == spectrumMap->customGradientPar2Gui) {
        guiElement->getValue((void*)&spectrumMap->customGradientParameters.y);
      }
      if(guiElement == spectrumMap->customGradientPar3Gui) {
        guiElement->getValue((void*)&spectrumMap->customGradientParameters.z);
      }
      if(guiElement == spectrumMap->spectrumResolutionGui) {
        int value;
        guiElement->getValue((void*)&value);
        spectrumMap->delayLineFFTW.initialize(value, false);
        //spectrumMap->delayLineFFTWRight.initialize(value, false);
      }
    }

  };











};
