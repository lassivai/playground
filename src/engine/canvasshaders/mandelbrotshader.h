#pragma once
#include "canvasshader.h"
#include "../../fractal/fructs.h"

struct MandelbrotVisualizer : public CanvasShader
{
  GlShader mandelbrotShader;
  GlShader multisamplingRestrictionShader;
  GlShader previewShader;
  Quadx quad;

  Texture renderTargetImg;
  Texture iterationStorageImg;
  Texture multisamplingRestrictionImg;
  Texture iterationMinMaxImg;

  FractalParameters fractPars;

  bool rememberIterations = false;

  std::vector<double> spectrum;
  std::vector<float> spectrumf;

  int currentPass = 0;

  bool usePreview = false;
  double previewIterationFactor = 0.2;


  Panel *panel = NULL;
  NumberBox *scaleGui = NULL;
  NumberBox *locationXGui = NULL, *locationYGui = NULL;
  NumberBox *juliaPointXGui = NULL, *juliaPointYGui = NULL;
  NumberBox *maxIterationGui = NULL;

  NumberBox *mandelbrotModeGui = NULL;
  NumberBox *colorModeGui = NULL;
  NumberBox *mappingModeGui = NULL;
  NumberBox *spectrumResolutionGui = NULL;
  NumberBox *postProcessingModeGui = NULL;
  CheckBox *rememberIterationsGui = NULL;

  CheckBox *useMultisamplingGui = NULL;
  NumberBox *multisamplingLevelGui = NULL;
  NumberBox *multisamplingRangeGui = NULL;
  NumberBox *multisamplingRangePowerGui = NULL;

  CheckBox *usePreviewGui = NULL;
  NumberBox *previewIterationFactorGui = NULL;

  double multisamplingRange = 0.5;
  double multisamplingRangePower = 1.0;
  bool useMultisampling = true;
  int multisamplingLevel = 10;
  int spectrumResolution = 1000;

  virtual ~MandelbrotVisualizer() {}

  virtual void init(Events &events, int w, int h) {
    renderTargetImg.create(w, h);
    iterationStorageImg.create(w, h, Texture::PixelType::FLOAT);
    iterationMinMaxImg.create(2, 1, Texture::PixelType::UINT);
    multisamplingRestrictionImg.create(w, h, Texture::PixelType::UINT);

    fractPars.init(events, w, h);
    fractPars.setInputActive(false);

    quad.create(1, 1, false);
    loadShader();
  }

  virtual void loadShader(){
    mandelbrotShader.create("data/glsl/basic.vert", "data/glsl/fract2.frag");
    multisamplingRestrictionShader.create("data/glsl/basic.vert", "data/glsl/fractMultRest.frag");
    previewShader.create("data/glsl/basic.vert", "data/glsl/fract2Preview.frag");
  }

  void setActive(bool isActive) {
    fractPars.setInputActive(isActive);
  }

  void update(const std::vector<double> spectrum) {
    if(this->spectrum.size() != spectrumResolution) {
      this->spectrum.resize(spectrumResolution);
    }
    resizeArray(spectrum, this->spectrum);
    normalizeArray(this->spectrum);
    
    spectrumf.resize(spectrum.size());
    for(int i=0; i<spectrum.size(); i++) {
      spectrumf[i] = spectrum[i];
    }
  }

  virtual void render(double w, double h, double time, const Events &events) {
    bool previewActive = usePreview && fractPars.isUnderMouseDragging();

    if(!previewActive && fractPars.viewChanged) {
      updatePanel();
      fractPars.viewChanged = false;
      currentPass = 1;
      iterationMinMaxImg.uintPixels[0] = -1;
      iterationMinMaxImg.uintPixels[1] = 0;
      iterationMinMaxImg.applyPixels();
    }

    GlShader &mandelbrotShader = previewActive ? previewShader : this->mandelbrotShader;

    mandelbrotShader.activate();
    mandelbrotShader.setUniform2f("screenSize", w, h);
    //mandelbrotShader.setUniform2f("mousePos", events.mouseX, events.mouseY);
    mandelbrotShader.setUniform1f("time", time);
    mandelbrotShader.setUniform2d("location", fractPars.location.x, -fractPars.location.y);
    mandelbrotShader.setUniform1d("scale", fractPars.scale);
    mandelbrotShader.setUniform1d("maxIter", fractPars.maxIter);
    mandelbrotShader.setUniform2d("juliaPoint", fractPars.juliaPoint.x, fractPars.juliaPoint.y);
    mandelbrotShader.setUniform1i("mandelbrotMode", fractPars.mandelbrotMode);
    mandelbrotShader.setUniform1i("mappingMode", fractPars.mappingMode);
    mandelbrotShader.setUniform1i("colorMode", fractPars.colorMode);
    mandelbrotShader.setUniform1i("multisamplingLevel", useMultisampling ? multisamplingLevel : 0);
    mandelbrotShader.setUniform1f("multisamplingRange", multisamplingRange);
    mandelbrotShader.setUniform1f("multisamplingRangePower", multisamplingRangePower);
    mandelbrotShader.setUniform1i("postEffectMode", fractPars.postEffectMode);
    mandelbrotShader.setUniform1fv("spectrum", spectrumf.size(), spectrumf.data());
    mandelbrotShader.setUniform1i("spectrumResolution", spectrumResolution);
    mandelbrotShader.setUniform1i("pass", rememberIterations && !previewActive ? currentPass : 0);
    mandelbrotShader.setUniform1i("previewActive", previewActive ? 1 : 0);
    mandelbrotShader.setUniform1f("previewIterationFactor", previewIterationFactor);

    quad.setSize(w, h);

    if(!previewActive) {
      iterationStorageImg.activateImageStorage(mandelbrotShader, "iterationImage", 1);
      iterationMinMaxImg.activateImageStorage(mandelbrotShader, "iterationMinMaxImage", 2);
    }

    if(fractPars.postEffectMode > 0) {
      renderTargetImg.activate(mandelbrotShader, "tex", 0);
      renderTargetImg.setRenderTarget();
      quad.render();
      if(rememberIterations && !previewActive) currentPass++;
      mandelbrotShader.setUniform1i("pass", rememberIterations && !previewActive ? currentPass : 0);
      renderTargetImg.unsetRenderTarget();
      quad.render();
      if(rememberIterations && !previewActive) currentPass++;
      renderTargetImg.inactivate(0);
    }
    else {
      quad.render();
      if(rememberIterations && !previewActive) currentPass++;
    }

    if(!previewActive) {
      iterationMinMaxImg.inactivate(2);
      iterationStorageImg.inactivate(1);
    }

    mandelbrotShader.deActivate();

    if(currentPass == 2) {
      multisamplingRestrictionShader.activate();
      iterationStorageImg.activateImageStorage(multisamplingRestrictionShader, "iterationImage", 1);
      multisamplingRestrictionImg.activateImageStorage(multisamplingRestrictionShader, "multisamplingRestrictionImage", 0);
      multisamplingRestrictionShader.setUniform1i("maxIter", (int)fractPars.maxIter);
      quad.render();
      multisamplingRestrictionImg.inactivate(0);
      iterationStorageImg.inactivate(1);
      multisamplingRestrictionShader.deActivate();
    }


    if(previewActive) fractPars.viewChanged = true;
  }




  Panel *addPanel(GuiElement *parentElement, const std::string &title = "") {
    if(panel) return panel;

    panel = new Panel(350, 590, 10, 10);
    double line = 10, lineHeight = 23;

    scaleGui = new NumberBox("Scale", fractPars.scale, NumberBox::FLOATING_POINT, -1e10, 1e10, 10, line, 10);
    locationXGui = new NumberBox("Location", fractPars.location.x, NumberBox::FLOATING_POINT, -1e10, 1e10, 10, line += lineHeight, 8);
    locationYGui = new NumberBox("", fractPars.location.y, NumberBox::FLOATING_POINT, -1e10, 1e10, 190, line, 8);
    juliaPointXGui = new NumberBox("Julia point", fractPars.juliaPoint.x, NumberBox::FLOATING_POINT, -1e10, 1e10, 10, line += lineHeight, 8);
    juliaPointYGui = new NumberBox("", fractPars.juliaPoint.y, NumberBox::FLOATING_POINT, -1e10, 1e10, 210, line, 8);
    maxIterationGui = new NumberBox("Max iterations", fractPars.maxIter, NumberBox::FLOATING_POINT, 0, 1e10, 10, line += lineHeight, 10);
    mandelbrotModeGui = new NumberBox("Fractal formula", fractPars.mandelbrotMode, NumberBox::INTEGER, 0, fractPars.numMandelbrotModes-1, 10, line += lineHeight, 10);
    colorModeGui = new NumberBox("Color mode", fractPars.colorMode, NumberBox::INTEGER, 0, fractPars.numColorModes-1, 10, line += lineHeight, 10);
    mappingModeGui = new NumberBox("Mapping mode", fractPars.mappingMode, NumberBox::INTEGER, 0, fractPars.numMappingModes-1, 10, line += lineHeight, 10);
    spectrumResolutionGui = new NumberBox("Spectrum resolution", spectrumResolution, NumberBox::INTEGER, 1, 1000, 10, line += lineHeight, 10);
    postProcessingModeGui = new NumberBox("Effect", fractPars.postEffectMode, NumberBox::INTEGER, 0, fractPars.numPostEffectModes-1, 10, line += lineHeight, 10);
    rememberIterationsGui = new CheckBox("Remember iterations", rememberIterations, 10, line += lineHeight);
    useMultisamplingGui = new CheckBox("Multisampling", useMultisampling, 10, line += lineHeight);
    multisamplingLevelGui = new NumberBox("Multisampling level", multisamplingLevel, NumberBox::INTEGER, 0, 1<<30, 10, line += lineHeight, 8);
    multisamplingRangeGui = new NumberBox("Multisampling range", multisamplingRange, NumberBox::FLOATING_POINT, 0, 1e10, 10, line += lineHeight, 10);
    multisamplingRangePowerGui = new NumberBox("Range power", multisamplingRangePower, NumberBox::FLOATING_POINT, 0, 1e10, 10, line += lineHeight, 10);

    //usePreviewGui = new CheckBox("Preview", useMultisampling, 10, line += lineHeight);
    //previewIterationFactorGui = new NumberBox("", previewIterationFactor, NumberBox::FLOATING_POINT, 0, 1.0, 140, line, 8);

    panel->addChildElement(scaleGui);
    panel->addChildElement(locationXGui);
    panel->addChildElement(locationYGui);
    panel->addChildElement(juliaPointXGui);
    panel->addChildElement(juliaPointYGui);
    panel->addChildElement(maxIterationGui);
    panel->addChildElement(mandelbrotModeGui);
    panel->addChildElement(colorModeGui);
    panel->addChildElement(mappingModeGui);
    panel->addChildElement(spectrumResolutionGui);
    panel->addChildElement(postProcessingModeGui);
    panel->addChildElement(rememberIterationsGui);
    panel->addChildElement(useMultisamplingGui);
    panel->addChildElement(multisamplingLevelGui);
    panel->addChildElement(multisamplingRangeGui);
    panel->addChildElement(multisamplingRangePowerGui);
    //panel->addChildElement(usePreviewGui);
    //panel->addChildElement(previewIterationFactorGui);

    panel->addGuiEventListener(new MandelbrotVisualizerPanelListener(this));

    panel->size.set(320, line + lineHeight + 10);

    parentElement->addChildElement(panel);

    return panel;
  }
  void removePanel(GuiElement *parentElement) {
    if(!panel) return;
    parentElement->deleteChildElement(panel);
    panel = NULL;
    scaleGui = NULL;
    locationXGui = NULL;
    locationYGui = NULL;
    juliaPointXGui = NULL;
    juliaPointYGui = NULL;
    maxIterationGui = NULL;
    mandelbrotModeGui = NULL;
    colorModeGui = NULL;
    mappingModeGui = NULL;
    spectrumResolutionGui = NULL;
    postProcessingModeGui = NULL;
    rememberIterationsGui = NULL;
    useMultisamplingGui = NULL;
    multisamplingLevelGui = NULL;
    multisamplingRangeGui = NULL;
    multisamplingRangePowerGui = NULL;
    //usePreviewGui = NULL;
    //previewIterationFactorGui = NULL;
  }
  void updatePanel() {
    if(panel) {
      scaleGui->setValue(fractPars.scale);
      locationXGui->setValue(fractPars.location.x);
      locationYGui->setValue(fractPars.location.y);
      juliaPointXGui->setValue(fractPars.juliaPoint.x);
      juliaPointYGui->setValue(fractPars.juliaPoint.y);
      maxIterationGui->setValue(fractPars.maxIter);
      mandelbrotModeGui->setValue(fractPars.mandelbrotMode);
      colorModeGui->setValue(fractPars.colorMode);
      mappingModeGui->setValue(fractPars.mappingMode);
      spectrumResolutionGui->setValue(spectrumResolution);
      postProcessingModeGui->setValue(fractPars.postEffectMode);
      rememberIterationsGui->setValue(rememberIterations);
      useMultisamplingGui->setValue(useMultisampling);
      multisamplingLevelGui->setValue(multisamplingLevel);
      multisamplingRangeGui->setValue(multisamplingRange);
      multisamplingRangePowerGui->setValue(multisamplingRangePower);
      //usePreviewGui->setValue(usePreview);
      //previewIterationFactorGui->setValue(previewIterationFactor);
    }
  }
  Panel *getPanel() {
    return panel;
  }

  struct MandelbrotVisualizerPanelListener : public GuiEventListener {
    MandelbrotVisualizer *mandelbrotVisualizer;
    MandelbrotVisualizerPanelListener(MandelbrotVisualizer *mandelbrotVisualizer) {
      this->mandelbrotVisualizer = mandelbrotVisualizer;
    }
    void onValueChange(GuiElement *guiElement) {
      if(guiElement == mandelbrotVisualizer->scaleGui) {
        guiElement->getValue((void*)&mandelbrotVisualizer->fractPars.scale);
        mandelbrotVisualizer->fractPars.viewChanged = true;
      }
      if(guiElement == mandelbrotVisualizer->locationXGui) {
        guiElement->getValue((void*)&mandelbrotVisualizer->fractPars.location.x);
        mandelbrotVisualizer->fractPars.viewChanged = true;
      }
      if(guiElement == mandelbrotVisualizer->locationYGui) {
        guiElement->getValue((void*)&mandelbrotVisualizer->fractPars.location.y);
        mandelbrotVisualizer->fractPars.viewChanged = true;
      }
      if(guiElement == mandelbrotVisualizer->juliaPointXGui) {
        guiElement->getValue((void*)&mandelbrotVisualizer->fractPars.juliaPoint.x);
        mandelbrotVisualizer->fractPars.viewChanged = true;
      }
      if(guiElement == mandelbrotVisualizer->juliaPointYGui) {
        guiElement->getValue((void*)&mandelbrotVisualizer->fractPars.juliaPoint.y);
        mandelbrotVisualizer->fractPars.viewChanged = true;
      }
      if(guiElement == mandelbrotVisualizer->maxIterationGui) {
        guiElement->getValue((void*)&mandelbrotVisualizer->fractPars.maxIter);
        mandelbrotVisualizer->fractPars.viewChanged = true;
      }
      if(guiElement == mandelbrotVisualizer->mandelbrotModeGui) {
        guiElement->getValue((void*)&mandelbrotVisualizer->fractPars.mandelbrotMode);
        mandelbrotVisualizer->fractPars.viewChanged = true;
      }
      if(guiElement == mandelbrotVisualizer->colorModeGui) {
        guiElement->getValue((void*)&mandelbrotVisualizer->fractPars.colorMode);
      }
      if(guiElement == mandelbrotVisualizer->mappingModeGui) {
        guiElement->getValue((void*)&mandelbrotVisualizer->fractPars.mappingMode);
      }
      if(guiElement == mandelbrotVisualizer->spectrumResolutionGui) {
        guiElement->getValue((void*)&mandelbrotVisualizer->spectrumResolution);
      }
      if(guiElement == mandelbrotVisualizer->postProcessingModeGui) {
        guiElement->getValue((void*)&mandelbrotVisualizer->fractPars.postEffectMode);
      }
      if(guiElement == mandelbrotVisualizer->rememberIterationsGui) {
        guiElement->getValue((void*)&mandelbrotVisualizer->rememberIterations);
        mandelbrotVisualizer->fractPars.viewChanged = true;
      }
      if(guiElement == mandelbrotVisualizer->useMultisamplingGui) {
        guiElement->getValue((void*)&mandelbrotVisualizer->useMultisampling);
        if(mandelbrotVisualizer->useMultisampling && !mandelbrotVisualizer->rememberIterations) {
          mandelbrotVisualizer->rememberIterations = true;
          mandelbrotVisualizer->rememberIterationsGui->setValue(mandelbrotVisualizer->rememberIterations);
        }
        mandelbrotVisualizer->fractPars.viewChanged = true;
      }
      if(guiElement == mandelbrotVisualizer->multisamplingLevelGui) {
        guiElement->getValue((void*)&mandelbrotVisualizer->multisamplingLevel);
        if(mandelbrotVisualizer->useMultisampling) {
          mandelbrotVisualizer->fractPars.viewChanged = true;
        }
      }
      if(guiElement == mandelbrotVisualizer->multisamplingRangeGui) {
        guiElement->getValue((void*)&mandelbrotVisualizer->multisamplingRange);
        if(mandelbrotVisualizer->useMultisampling) {
          mandelbrotVisualizer->fractPars.viewChanged = true;
        }
      }
      if(guiElement == mandelbrotVisualizer->multisamplingRangePowerGui) {
        guiElement->getValue((void*)&mandelbrotVisualizer->multisamplingRangePower);
        if(mandelbrotVisualizer->useMultisampling) {
          mandelbrotVisualizer->fractPars.viewChanged = true;
        }
      }
      /*if(guiElement == mandelbrotVisualizer->usePreviewGui) {
        guiElement->getValue((void*)&mandelbrotVisualizer->usePreview);
      }
      if(guiElement == mandelbrotVisualizer->previewIterationFactorGui) {
        guiElement->getValue((void*)&mandelbrotVisualizer->previewIterationFactor);
      }*/

    }
  };

};
