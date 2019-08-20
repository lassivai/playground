#pragma once
#include "panel.h"




struct FastNoisePanel : public Panel {

  FastNoise *fastNoise = NULL;
  Label *titleGui = NULL;
  ListBox *noiseTypeGui = NULL;
  ListBox *fractalTypeGui = NULL;
  ListBox *interpGui = NULL;
  ListBox *cellularDistanceFunctionGui = NULL;
  ListBox *cellularReturnTypeGui = NULL;
  NumberBox *cellularJitterGui = NULL;

  NumberBox *frequencyGui = NULL;
  NumberBox *seedGui = NULL;
  NumberBox *fractalOctavesGui = NULL;
  NumberBox *lacunarityGui = NULL;
  NumberBox *fractalGainGui = NULL;

  virtual ~FastNoisePanel() {}

  FastNoisePanel() : Panel("FastNoise panel ", 0, 0, 1, 1)  {}

  FastNoisePanel(GuiElement *parentElement, FastNoise *fastNoise, const std::string &title = "") : Panel("FastNoise panel "+title, 0, 0, 1, 1) {
    init(parentElement, fastNoise, title);
  }

  void init(GuiElement *parentElement, FastNoise *fastNoise, const std::string &title = "") {
    this->fastNoise = fastNoise;

    double line = -13, lineHeight = 23;

    if(title.size() > 0) {
      titleGui = new Label(title, 10, line+=lineHeight);
      this->addChildElement(titleGui);
    }

    noiseTypeGui = new ListBox("Noise type", 10, line+=lineHeight, 10);
    noiseTypeGui->setItems(fastNoiseTypeNames);
    noiseTypeGui->setValue(fastNoise->GetNoiseType());
    this->addChildElement(noiseTypeGui);

    fractalTypeGui = new ListBox("Fractal type", 10, line+=lineHeight, 10);
    fractalTypeGui->setItems(fastNoiseFractalTypeNames);
    fractalTypeGui->setValue(fastNoise->GetFractalType());
    this->addChildElement(fractalTypeGui);

    interpGui = new ListBox("Interpolation", 10, line+=lineHeight);
    interpGui->setItems(fastNoiseInterpNames);
    interpGui->setValue(fastNoise->GetInterp());
    this->addChildElement(interpGui);

    cellularDistanceFunctionGui = new ListBox("Cellular distance", 10, line+=lineHeight);
    cellularDistanceFunctionGui->setItems(fastNoiseCellularDistanceFunctionNames);
    cellularDistanceFunctionGui->setValue(fastNoise->GetCellularDistanceFunction());
    this->addChildElement(cellularDistanceFunctionGui);

    cellularReturnTypeGui = new ListBox("Cellular return", 10, line+=lineHeight);
    cellularReturnTypeGui->setItems(fastNoiseCellularReturnTypeNames);
    cellularReturnTypeGui->setValue(fastNoise->GetCellularReturnType());
    this->addChildElement(cellularReturnTypeGui);

    cellularJitterGui = new NumberBox("Cellular jitter", fastNoise->GetCellularJitter(), NumberBox::FLOATING_POINT, -1e10, 1e10, 10, line+=lineHeight);
    this->addChildElement(cellularJitterGui);

    frequencyGui = new NumberBox("Frequency", fastNoise->GetFrequency(), NumberBox::FLOATING_POINT, -1e10, 1e10, 10, line+=lineHeight);
    this->addChildElement(frequencyGui);

    seedGui = new NumberBox("Seed", fastNoise->GetSeed(), NumberBox::INTEGER, -1<<30, 1<<30, 10, line+=lineHeight);
    this->addChildElement(seedGui);

    fractalOctavesGui = new NumberBox("Fractal octaves", fastNoise->GetFractalOctaves(), NumberBox::INTEGER, 1, 100000, 10, line+=lineHeight);
    this->addChildElement(fractalOctavesGui);

    lacunarityGui = new NumberBox("Lacunarity", fastNoise->GetFractalLacunarity(), NumberBox::FLOATING_POINT, -1e10, 1e10, 10, line+=lineHeight);
    this->addChildElement(lacunarityGui);

    fractalGainGui = new NumberBox("Fractal gain", fastNoise->GetFractalGain(), NumberBox::FLOATING_POINT, -1e10, 1e10, 10, line+=lineHeight);
    this->addChildElement(fractalGainGui);

    this->setSize(320, line + lineHeight + 10);

    addGuiEventListener(new FastNoisePanelListener(this));

    parentElement->addChildElement(this);
  }

  void updatePanel() {
    noiseTypeGui->setValue(fastNoise->GetNoiseType());
    fractalTypeGui->setValue(fastNoise->GetFractalType());
    interpGui->setValue(fastNoise->GetInterp());
    cellularDistanceFunctionGui->setValue(fastNoise->GetCellularDistanceFunction());
    cellularReturnTypeGui->setValue(fastNoise->GetCellularReturnType());
    cellularJitterGui->setValue(fastNoise->GetCellularJitter());
    frequencyGui->setValue(fastNoise->GetFrequency());
    seedGui->setValue(fastNoise->GetSeed());
    fractalOctavesGui->setValue(fastNoise->GetFractalOctaves());
    lacunarityGui->setValue(fastNoise->GetFractalLacunarity());
    fractalGainGui->setValue(fastNoise->GetFractalGain());
  }


  struct FastNoisePanelListener : public GuiEventListener {
    FastNoisePanel *fastNoisePanel;
    FastNoisePanelListener(FastNoisePanel *fastNoisePanel) {
      this->fastNoisePanel = fastNoisePanel;
    }
    void onValueChange(GuiElement *guiElement) {
      if(guiElement == fastNoisePanel->noiseTypeGui) {
        int value;
        guiElement->getValue(&value);
        fastNoisePanel->fastNoise->SetNoiseType((FastNoise::NoiseType)value);
      }
      if(guiElement == fastNoisePanel->fractalTypeGui) {
        int value;
        guiElement->getValue(&value);
        fastNoisePanel->fastNoise->SetFractalType((FastNoise::FractalType)value);
      }
      if(guiElement == fastNoisePanel->interpGui) {
        int value;
        guiElement->getValue(&value);
        fastNoisePanel->fastNoise->SetInterp((FastNoise::Interp)value);
      }
      if(guiElement == fastNoisePanel->cellularDistanceFunctionGui) {
        int value;
        guiElement->getValue(&value);
        fastNoisePanel->fastNoise->SetCellularDistanceFunction((FastNoise::CellularDistanceFunction)value);
      }
      if(guiElement == fastNoisePanel->cellularReturnTypeGui) {
        int value;
        guiElement->getValue(&value);
        fastNoisePanel->fastNoise->SetCellularReturnType((FastNoise::CellularReturnType)value);
      }
      if(guiElement == fastNoisePanel->cellularJitterGui) {
        double value;
        guiElement->getValue(&value);
        fastNoisePanel->fastNoise->SetCellularJitter(value);
      }
      if(guiElement == fastNoisePanel->frequencyGui) {
        double value;
        guiElement->getValue(&value);
        fastNoisePanel->fastNoise->SetFrequency(value);
      }

      if(guiElement == fastNoisePanel->seedGui) {
        int value;
        guiElement->getValue(&value);
        fastNoisePanel->fastNoise->SetSeed(value);
      }
      if(guiElement == fastNoisePanel->fractalOctavesGui) {
        int value;
        guiElement->getValue(&value);
        fastNoisePanel->fastNoise->SetFractalOctaves(value);
      }
      if(guiElement == fastNoisePanel->lacunarityGui) {
        double value;
        guiElement->getValue(&value);
        fastNoisePanel->fastNoise->SetFractalLacunarity(value);
      }
      if(guiElement == fastNoisePanel->fractalGainGui) {
        double value;
        guiElement->getValue(&value);
        fastNoisePanel->fastNoise->SetFractalGain(value);
      }
    }
  };


};
