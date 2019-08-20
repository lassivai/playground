#pragma once
#include "canvasshader.h"



struct WarpShader : public CanvasShader
{
  GlShader shader;
  Quadx quadx;

  FastNoise fastNoise;

  double scale = 1.0;
  double value = 0.0, amplitude = 0.5, frequency = 2.4, gain = 0.42, lacunarity = 2.7;

  static const int maxWarpLevels = 10;
  int warpLevels = 8;
  std::vector<double> warpFreqs = std::vector<double>(maxWarpLevels);
  std::vector<double> warpAmounts = std::vector<double>(maxWarpLevels);
  std::vector<double> warpTimes = std::vector<double>(maxWarpLevels);

  std::vector<std::string> presetNames = {"Clouds", "Smoky", "Smoky ink", "Purple clouds", "Fire", "Sun",
                                          "Water", "Gray water", "Cloudy sky", "Cloudy sky 2", "Cloudy night sky"};

  int preset = 0;

  int colorMode = 0;

  Panel *panel = NULL;
  NumberBox *scaleGui = NULL, *valueGui = NULL, *frequencyGui = NULL, *amplitudeGui = NULL, *lacunarityGui = NULL, *gainGui = NULL;
  NumberBox* warpLevelsGui = NULL;
  std::vector<NumberBox*> warpFreqsGui = std::vector<NumberBox*>(maxWarpLevels, NULL);
  std::vector<NumberBox*> warpAmountsGui = std::vector<NumberBox*>(maxWarpLevels, NULL);

  NumberBox *colorModeGui = NULL;

  ListBox *presetGui = NULL;

  virtual ~WarpShader() {}

  virtual void init() {
    fastNoise.SetSeed(64573);
    fastNoise.SetNoiseType(FastNoise::SimplexFractal);

    quadx.create(1, 1, false);
    for(int i=0; i<maxWarpLevels; i++) {
      warpFreqs[i] = 0.1;
      warpAmounts[i] = 1.0;
    }
    loadShader();

    setPreset(preset);
  }

  virtual void loadShader() {
    shader.create("data/glsl/texture.vert", "data/glsl/warp.frag");
  }


  virtual void render(double w, double h, double time, const Events &events) {
    for(int i=0; i<maxWarpLevels; i++) {
      warpTimes[i] = warpFreqs[i] * time;
    }

    shader.activate();
    shader.setUniform2f("size", quadx.w, quadx.h);
    shader.setUniform1f("time", time);

    shader.setUniform1f("scale", scale);
    shader.setUniform1f("value", value);
    shader.setUniform1f("frequency", frequency);
    shader.setUniform1f("amplitude", amplitude);
    shader.setUniform1f("lacunarity", lacunarity);
    shader.setUniform1f("gain", gain);
    shader.setUniform1i("warpLevels", warpLevels);
    shader.setUniform1i("colorMode", colorMode);

    float warpFreqsf[maxWarpLevels], warpAmountsf[maxWarpLevels], warpTimesf[maxWarpLevels];
    for(int i=0; i<maxWarpLevels; i++) {
      warpFreqsf[i] = (float)warpFreqs[i];
      warpAmountsf[i] = (float)warpAmounts[i];
      warpTimesf[i] = (float)warpTimes[i];
    }

    shader.setUniform1fv("warpFreqs", maxWarpLevels, warpFreqsf);
    shader.setUniform1fv("warpTimes", maxWarpLevels, warpTimesf);
    shader.setUniform1fv("warpAmounts", maxWarpLevels, warpAmountsf);

    quadx.setSize(w, h);
    quadx.render(w/2, h/2);
    shader.deActivate();
  }


  void setPreset(int index) {
    if(index == 0) { // clouds
      colorMode = 0;
      scale = 0.6;
      value = 0.1;
      frequency = 1.6;
      amplitude = 0.51;
      gain = 0.39;
      lacunarity = 3.0;
      warpLevels = 10;
      warpFreqs[0] = 0.1;
      warpFreqs[1] = 0.1;
      warpFreqs[2] = 0.1;
      warpFreqs[3] = 0.1;
      warpFreqs[4] = 0.1;
      warpFreqs[5] = 0.1;
      warpFreqs[6] = 0.1;
      warpFreqs[7] = 0.1;
      warpFreqs[8] = 0.1;
      warpFreqs[9] = 0.06;
      warpAmounts.assign(maxWarpLevels, 1.0);
    }
    if(index == 1) { // smoky
      colorMode = 0;
      scale = 0.3;
      value = 0.15;
      frequency = 1.8;
      amplitude = 0.51;
      gain = 0.4;
      lacunarity = 3.0;
      warpLevels = 10;
      warpFreqs[0] = 0.2;
      warpFreqs[1] = 0.2;
      warpFreqs[2] = 0.1;
      warpFreqs[3] = 0.01;
      warpFreqs[4] = 0.03;
      warpFreqs[5] = 0.002;
      warpFreqs[6] = 0.006;
      warpFreqs[7] = 0.01;
      warpFreqs[8] = 0.001;
      warpFreqs[9] = 0.003;
      warpAmounts.assign(maxWarpLevels, 1.0);
    }
    if(index == 2) {
      colorMode = 1;
      scale = 0.2;
      value = -0.2;
      frequency = 1.3;
      amplitude = 0.91;
      gain = 0.31;
      lacunarity = 3.3;
      warpLevels = 10;
      warpFreqs[0] = 0.7;
      warpFreqs[1] = 0.6;
      warpFreqs[2] = 0.02;
      warpFreqs[3] = 0.02;
      warpFreqs[4] = 0.02;
      warpFreqs[5] = 0.03;
      warpFreqs[6] = 0.03;
      warpFreqs[7] = 0.01;
      warpFreqs[8] = 0.02;
      warpFreqs[9] = 0.05;
      warpAmounts.assign(maxWarpLevels, 1.0);
    }
    if(index == 3) { // purple clouds
      colorMode = 3;
      scale = 0.5;
      value = 0.0;
      frequency = 2.1;
      amplitude = 0.6;
      gain = 0.36;
      lacunarity = 2.8;
      warpLevels = 10;
      warpFreqs[0] = 0.09;
      warpFreqs[1] = 0.04;
      warpFreqs[2] = 0.02;
      warpFreqs[3] = 0.004;
      warpFreqs[4] = 0.07;
      warpFreqs[5] = 0.0006;
      warpFreqs[6] = 0.006;
      warpFreqs[7] = 0.0008;
      warpFreqs[8] = 0.04;
      warpFreqs[9] = 0.0007;
      warpAmounts.assign(maxWarpLevels, 1.0);
    }
    if(index == 4) { // fire
      colorMode = 2;
      scale = 1.0;
      value = 0.0;
      frequency = 1.0;
      amplitude = 0.8;
      gain = 0.45;
      lacunarity = 2.7;
      warpLevels = 9;
      warpFreqs[0] = 0.1;
      warpFreqs[1] = 0.1;
      warpFreqs[2] = 0.4;
      warpFreqs[3] = 0.4;
      warpFreqs[4] = 0.3;
      warpFreqs[5] = 0.09;
      warpFreqs[6] = 0.1;
      warpFreqs[7] = 0.1;
      warpFreqs[8] = 0.5;
      warpFreqs[9] = 0.4;
      warpAmounts.assign(maxWarpLevels, 1.0);
    }
    if(index == 5) { // sun
      colorMode = 2;
      scale = 2.0;
      value = -0.2;
      frequency = 1.0;
      amplitude = 0.8;
      gain = 0.45;
      lacunarity = 2.55;
      warpLevels = 10;
      warpFreqs[0] = 0.1;
      warpFreqs[1] = 0.1;
      warpFreqs[2] = 0.05;
      warpFreqs[3] = 0.03;
      warpFreqs[4] = 0.3;
      warpFreqs[5] = 0.8;
      warpFreqs[6] = 0.07;
      warpFreqs[7] = 0.09;
      warpFreqs[8] = 0.3;
      warpFreqs[9] = 0.1;
      warpAmounts.assign(maxWarpLevels, 1.0);
    }

    if(index == 6) { // water
      colorMode = 4;
      scale = 0.14;
      value = 0.4;
      frequency = 3.2;
      amplitude = 0.55;
      gain = 0.33;
      lacunarity = 3.0;
      warpLevels = 10;
      warpFreqs[0] = 0.4;
      warpFreqs[1] = 0.09;
      warpFreqs[2] = 0.008;
      warpFreqs[3] = 0.07;
      warpFreqs[4] = 0.09;
      warpFreqs[5] = 0.09;
      warpFreqs[6] = 0.09;
      warpFreqs[7] = 0.06;
      warpFreqs[8] = 0.06;
      warpFreqs[9] = 0.0006;
      warpAmounts.assign(maxWarpLevels, 1.0);
    }
    if(index == 7) { // gray water
      colorMode = 0;
      scale = 0.2;
      value = 0.2;
      frequency = 2.9;
      amplitude = 0.6;
      gain = 0.39;
      lacunarity = 2.8;
      warpLevels = 7;
      warpFreqs[0] = 0.1;
      warpFreqs[1] = 0.1;
      warpFreqs[2] = 0.1;
      warpFreqs[3] = 0.07;
      warpFreqs[4] = 0.2;
      warpFreqs[5] = 0.1;
      warpFreqs[6] = 0.05;
      warpFreqs[7] = 0.008;
      warpFreqs[8] = 0.0001;
      warpFreqs[9] = 0.0001;
      warpAmounts.assign(maxWarpLevels, 1.0);
    }
    if(index == 8) { // cloudy sky
      colorMode = 5;
      scale = 1.0;
      value = -0.3;
      frequency = 1.6;
      amplitude = 0.51;
      gain = 0.39;
      lacunarity = 3.0;
      warpLevels = 10;
      warpFreqs[0] = 0.1;
      warpFreqs[1] = 0.1;
      warpFreqs[2] = 0.1;
      warpFreqs[3] = 0.1;
      warpFreqs[4] = 0.1;
      warpFreqs[5] = 0.1;
      warpFreqs[6] = 0.06;
      warpFreqs[7] = 0.07;
      warpFreqs[8] = 0.006;
      warpFreqs[9] = 0.06;
      warpAmounts.assign(maxWarpLevels, 1.0);
    }
    if(index == 9) { // cloudy sky 2
      colorMode = 6;
      scale = 1.0;
      value = -0.3;
      frequency = 1.2;
      amplitude = 0.47;
      gain = 0.39;
      lacunarity = 3.3;
      warpLevels = 10;
      warpFreqs[0] = 0.03;
      warpFreqs[1] = 0.08;
      warpFreqs[2] = 0.1;
      warpFreqs[3] = 0.08;
      warpFreqs[4] = 0.08;
      warpFreqs[5] = 0.1;
      warpFreqs[6] = 0.06;
      warpFreqs[7] = 0.08;
      warpFreqs[8] = 0.004;
      warpFreqs[9] = 0.0004;
      warpAmounts.assign(maxWarpLevels, 1.0);
    }
    if(index == 10) { // cloudy night sky
      colorMode = 7;
      scale = 1.0;
      value = -0.3;
      frequency = 1.6;
      amplitude = 0.51;
      gain = 0.39;
      lacunarity = 3.0;
      warpLevels = 10;
      warpFreqs[0] = 0.2;
      warpFreqs[1] = 0.03;
      warpFreqs[2] = 0.05;
      warpFreqs[3] = 0.2;
      warpFreqs[4] = 0.1;
      warpFreqs[5] = 0.1;
      warpFreqs[6] = 0.08;
      warpFreqs[7] = 0.06;
      warpFreqs[8] = 0.009;
      warpFreqs[9] = 0.009;
      warpAmounts.assign(maxWarpLevels, 1.0);
    }
    if(panel) {
      colorModeGui->setValue(colorMode);
      scaleGui->setValue(scale);
      valueGui->setValue(value);
      frequencyGui->setValue(frequency);
      amplitudeGui->setValue(amplitude);
      lacunarityGui->setValue(lacunarity);
      gainGui->setValue(gain);
      warpLevelsGui->setValue(warpLevels);

      for(int i=0; i<maxWarpLevels; i++) {
        warpFreqsGui[i]->setValue(warpFreqs[i]);
      }
    }
  }


  Panel *addPanel(GuiElement *parentElement, const std::string &title = "") {
    if(panel) return panel;

    panel = new Panel(350, 590, 10, 10);
    double line = 10, lineHeight = 23;
    presetGui = new ListBox("Preset", 10, line, 12);
    presetGui->setItems(presetNames);
    presetGui->setValue(preset);
    colorModeGui = new NumberBox("Color Mode", colorMode, NumberBox::INTEGER, 0, 7, 10, line += lineHeight, 7);
    scaleGui = new NumberBox("Scale", scale, NumberBox::FLOATING_POINT, 0.0, 100000, 10, line += lineHeight, 7);
    valueGui = new NumberBox("Value", value, NumberBox::FLOATING_POINT, -1000.0, 1000, 10, line += lineHeight, 7);
    frequencyGui = new NumberBox("Frequency", frequency, NumberBox::FLOATING_POINT, -1000, 1000, 10, line += lineHeight, 7);
    amplitudeGui = new NumberBox("Amplitude", amplitude, NumberBox::FLOATING_POINT, -1000, 1000, 10, line += lineHeight, 7);
    lacunarityGui = new NumberBox("Lacunarity", lacunarity, NumberBox::FLOATING_POINT, -1000, 1000, 10, line += lineHeight, 7);
    gainGui = new NumberBox("Gain", gain, NumberBox::FLOATING_POINT, -1000, 1000, 10, line += lineHeight, 7);
    warpLevelsGui = new NumberBox("Warp levels", warpLevels, NumberBox::INTEGER, 1, maxWarpLevels, 10, line += lineHeight, 7);
    for(int i=0; i<maxWarpLevels; i++) {
      warpFreqsGui[i] = new NumberBox("Warp freq. "+std::to_string(i+1), warpFreqs[i], NumberBox::FLOATING_POINT, 0, 1000, 10, line += lineHeight, 7);
    }

    /*for(int i=0; i<maxWarpLevels; i++) {
      warpAmountsGui[i] = new NumberBox("Warp amount "+std::to_string(i), warpAmounts[i], NumberBox::FLOATING_POINT, -1000, 1000, 10, line += lineHeight);
    }*/


    panel->addChildElement(presetGui);
    panel->addChildElement(colorModeGui);
    panel->addChildElement(scaleGui);
    panel->addChildElement(valueGui);
    panel->addChildElement(frequencyGui);
    panel->addChildElement(amplitudeGui);
    panel->addChildElement(gainGui);
    panel->addChildElement(lacunarityGui);
    panel->addChildElement(warpLevelsGui);
    for(int i=0; i<maxWarpLevels; i++) {
      panel->addChildElement(warpFreqsGui[i]);
      //panel->addChildElement(warpAmountsGui[i]);
    }

    panel->addGuiEventListener(new WarpShaderPanelListener(this));

    panel->size.set(280, line + lineHeight + 10);

    parentElement->addChildElement(panel);

    return panel;
  }
  void removePanel(GuiElement *parentElement) {
    if(!panel) return;
    parentElement->deleteChildElement(panel);
    panel = NULL;
    scaleGui = NULL;
    valueGui = NULL;
    frequencyGui = NULL;
    amplitudeGui = NULL;
    lacunarityGui = NULL;
    gainGui = NULL;
    warpLevelsGui  = NULL;
    warpFreqsGui = std::vector<NumberBox*>(maxWarpLevels, NULL);
    warpAmountsGui = std::vector<NumberBox*>(maxWarpLevels, NULL);
    colorModeGui = NULL;
    presetGui = NULL;
  }
  void updatePanel() {
    colorModeGui->setValue(colorMode);
    presetGui->setValue(preset);
    scaleGui->setValue(scale);
    valueGui->setValue(value);
    frequencyGui->setValue(frequency);
    amplitudeGui->setValue(amplitude);
    lacunarityGui->setValue(lacunarity);
    gainGui->setValue(gain);
    warpLevelsGui->setValue(warpLevels);
    for(int i=0; i<maxWarpLevels; i++) {
      warpFreqsGui[i]->setValue(warpFreqs[i]);
      warpAmountsGui[i]->setValue(warpAmounts[i]);
    }
  }
  Panel *getPanel() {
    return panel;
  }

  struct WarpShaderPanelListener : public GuiEventListener {
    WarpShader *warpShader;
    WarpShaderPanelListener(WarpShader *warpShader) {
      this->warpShader = warpShader;
    }
    void onValueChange(GuiElement *guiElement) {
      if(guiElement == warpShader->presetGui) {
        guiElement->getValue((void*)&warpShader->preset);
        warpShader->setPreset(warpShader->preset);
      }
      if(guiElement == warpShader->colorModeGui) {
        guiElement->getValue((void*)&warpShader->colorMode);
      }
      if(guiElement == warpShader->scaleGui) {
        guiElement->getValue((void*)&warpShader->scale);
      }
      if(guiElement == warpShader->valueGui) {
        guiElement->getValue((void*)&warpShader->value);
      }
      if(guiElement == warpShader->frequencyGui) {
        guiElement->getValue((void*)&warpShader->frequency);
      }
      if(guiElement == warpShader->amplitudeGui) {
        guiElement->getValue((void*)&warpShader->amplitude);
      }
      if(guiElement == warpShader->lacunarityGui) {
        guiElement->getValue((void*)&warpShader->lacunarity);
      }
      if(guiElement == warpShader->gainGui) {
        guiElement->getValue((void*)&warpShader->gain);
      }
      if(guiElement == warpShader->warpLevelsGui) {
        guiElement->getValue((void*)&warpShader->warpLevels);
      }
      for(int i=0; i<maxWarpLevels; i++) {
        if(guiElement == warpShader->warpFreqsGui[i]) {
          guiElement->getValue((void*)&warpShader->warpFreqs[i]);
        }
        if(guiElement == warpShader->warpAmountsGui[i]) {
          guiElement->getValue((void*)&warpShader->warpAmounts[i]);
        }
      }
    }
  };

};










struct WarpVisualizer : public WarpShader
{
  std::vector<std::vector<double>> differenceAveragingSpectrums;
  std::vector<double> differenceAveragingSpectrum, differenceSpectrum;
  int numDifferenceAveragingSpectrums = 200;
  int differenceAveragingSpectrumsPos = 0;

  std::vector<std::vector<double>> forwardAveragingSpectrums;
  std::vector<double> forwardAveragingSpectrum, forwardSpectrum;
  int numForwardAveragingSpectrums = 50;
  int forwardAveragingSpectrumsPos = 0;


  double differenceStrength = 200;
  double forwardStrength = 400;
  double baseLevel = 0.0;

  void init() {
    WarpShader::init();

    differenceAveragingSpectrum.resize(10);
    differenceSpectrum.resize(10);

    forwardAveragingSpectrum.resize(10);
    forwardSpectrum.resize(10);
  }

  void update(const std::vector<double> &spectrum, double dt) {

    if(differenceAveragingSpectrums.size() != numDifferenceAveragingSpectrums) {
      differenceAveragingSpectrums.resize(numDifferenceAveragingSpectrums);
      for(int i=0; i<differenceAveragingSpectrums.size(); i++) {
        differenceAveragingSpectrums[i].resize(10);
      }
      differenceAveragingSpectrumsPos = 0;
    }

    if(forwardAveragingSpectrums.size() != numForwardAveragingSpectrums) {
      forwardAveragingSpectrums.resize(numForwardAveragingSpectrums);
      for(int i=0; i<forwardAveragingSpectrums.size(); i++) {
        forwardAveragingSpectrums[i].resize(10);
      }
      forwardAveragingSpectrumsPos = 0;
    }

    resizeArray(spectrum, forwardSpectrum);

    forwardAveragingSpectrums[forwardAveragingSpectrumsPos] = forwardSpectrum;
    differenceAveragingSpectrums[differenceAveragingSpectrumsPos] = forwardSpectrum;

    for(int j=0; j<10; j++) {
      forwardAveragingSpectrum[j] = 0;
      for(int i=0; i<forwardAveragingSpectrums.size(); i++) {
        forwardAveragingSpectrum[j] += forwardAveragingSpectrums[i][j];
      }
      forwardAveragingSpectrum[j] /= forwardAveragingSpectrums.size();

      differenceAveragingSpectrum[j] = 0;
      for(int i=0; i<differenceAveragingSpectrums.size(); i++) {
        differenceAveragingSpectrum[j] += differenceAveragingSpectrums[i][j];
      }
      differenceAveragingSpectrum[j] /= differenceAveragingSpectrums.size();
      differenceSpectrum[j] = (differenceAveragingSpectrums[differenceAveragingSpectrumsPos][j] - differenceAveragingSpectrum[j]);
    }

    for(int i=0; i<maxWarpLevels; i++) {
      warpTimes[i] += (differenceSpectrum[i]*differenceStrength + max(0.0, forwardAveragingSpectrum[i])*forwardStrength + baseLevel) * warpFreqs[i] * dt;
    }

    differenceAveragingSpectrumsPos = (differenceAveragingSpectrumsPos+1) % differenceAveragingSpectrums.size();
    forwardAveragingSpectrumsPos = (forwardAveragingSpectrumsPos+1) % forwardAveragingSpectrums.size();
  }


  void render(double w, double h, double time, const Events &events) {
    float warpFreqsf[maxWarpLevels], warpAmountsf[maxWarpLevels], warpTimesf[maxWarpLevels];
    for(int i=0; i<maxWarpLevels; i++) {
      warpFreqsf[i] = (float)warpFreqs[i];
      warpAmountsf[i] = (float)warpAmounts[i];
      warpTimesf[i] = (float)warpTimes[i];
    }

    quadx.setSize(w, h);

    shader.activate();

    shader.setUniform1i("colorMode", colorMode);

    shader.setUniform2f("size", quadx.w, quadx.h);
    shader.setUniform1f("time", time);

    shader.setUniform1f("scale", scale);
    shader.setUniform1f("value", value);
    shader.setUniform1f("frequency", frequency);
    shader.setUniform1f("amplitude", amplitude);
    shader.setUniform1f("lacunarity", lacunarity);
    shader.setUniform1f("gain", gain);
    shader.setUniform1i("warpLevels", warpLevels);

    shader.setUniform1fv("warpFreqs", maxWarpLevels, warpFreqsf);
    shader.setUniform1fv("warpTimes", maxWarpLevels, warpTimesf);
    shader.setUniform1fv("warpAmounts", maxWarpLevels, warpAmountsf);

    quadx.render(w/2, h/2);
    shader.deActivate();
  }

  Panel *visPanel = NULL;
  NumberBox *baseLevelGui = NULL;
  NumberBox *forwardStrengthGui = NULL;
  NumberBox *forwardAveragingGui = NULL;
  NumberBox *differenceStrengthGui = NULL;
  NumberBox *differenceAveragingGui = NULL;

  Panel *addPanel(GuiElement *parentElement, const std::string &title = "") {
    WarpShader::addPanel(parentElement, title);
    if(visPanel) return panel;

    visPanel = new Panel(350, 590, 10, 10);
    visPanel->addGuiEventListener(new WarpVisualizerPanelListener(this));

    double line = 10, lineHeight = 23;
    baseLevelGui = new NumberBox("Base level", baseLevel, NumberBox::FLOATING_POINT, -1e10, 1e10, 10, line, 7);
    forwardStrengthGui = new NumberBox("Forward strength", forwardStrength, NumberBox::FLOATING_POINT, -1e10, 1e10, 10, line += lineHeight, 7);
    forwardAveragingGui = new NumberBox("Forward averaging", numForwardAveragingSpectrums, NumberBox::INTEGER, 1, 100000, 10, line += lineHeight, 7);
    forwardAveragingGui->incrementMode = NumberBox::IncrementMode::Linear;
    differenceStrengthGui = new NumberBox("Difference strength", differenceStrength, NumberBox::FLOATING_POINT, -1e10, 1e10, 10, line += lineHeight, 7);
    differenceAveragingGui = new NumberBox("Difference averaging", numDifferenceAveragingSpectrums, NumberBox::INTEGER, 1, 100000, 10, line += lineHeight, 7);
    differenceAveragingGui->incrementMode = NumberBox::IncrementMode::Linear;

    visPanel->addChildElement(baseLevelGui);
    visPanel->addChildElement(forwardStrengthGui);
    visPanel->addChildElement(forwardAveragingGui);
    visPanel->addChildElement(differenceStrengthGui);
    visPanel->addChildElement(differenceAveragingGui);

    panel->addChildElement(visPanel);

    visPanel->size.set(280, line + lineHeight + 10);
    visPanel->setPosition(0, panel->size.y + 5);

    return panel;
  }

  void removePanel(GuiElement *parentElement) {
    WarpShader::removePanel(parentElement);
    visPanel = NULL;
    baseLevelGui = NULL;
    forwardStrengthGui = NULL;
    forwardAveragingGui = NULL;
    differenceAveragingGui = NULL;
    differenceStrengthGui = NULL;
  }
  void updatePanel() {
    WarpShader::updatePanel();
    if(visPanel) {
      baseLevelGui->setValue(baseLevel);
      forwardStrengthGui->setValue(forwardStrength);
      forwardAveragingGui->setValue(numForwardAveragingSpectrums);
      differenceAveragingGui->setValue(differenceStrength);
      differenceStrengthGui->setValue(numDifferenceAveragingSpectrums);
    }
  }

  struct WarpVisualizerPanelListener : public GuiEventListener {
    WarpVisualizer *warpVisualizer;
    WarpVisualizerPanelListener(WarpVisualizer *warpVisualizer) {
      this->warpVisualizer = warpVisualizer;
    }
    void onValueChange(GuiElement *guiElement) {
      if(guiElement == warpVisualizer->baseLevelGui) {
        guiElement->getValue((void*)&warpVisualizer->baseLevel);
      }
      if(guiElement == warpVisualizer->forwardStrengthGui) {
        guiElement->getValue((void*)&warpVisualizer->forwardStrength);
      }
      if(guiElement == warpVisualizer->forwardAveragingGui) {
        guiElement->getValue((void*)&warpVisualizer->numForwardAveragingSpectrums);
      }
      if(guiElement == warpVisualizer->differenceStrengthGui) {
        guiElement->getValue((void*)&warpVisualizer->differenceStrength);
      }
      if(guiElement == warpVisualizer->differenceAveragingGui) {
        guiElement->getValue((void*)&warpVisualizer->numDifferenceAveragingSpectrums);
      }
    }
  };

};
