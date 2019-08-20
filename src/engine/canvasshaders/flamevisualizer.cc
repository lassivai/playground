#include "../sketch.h"


/* TODO
 * - inherit from canvas shader
 * - progress parameters
 *   - continuously
 *   - within a waveform such as sin (DONE)
 * - spectrum threshold level
 * - spectrum averaging only without progression
 *
 */


 CEREAL_CLASS_VERSION(FlameVisualizer, 1);

//struct FlameVisualizer
//{
  void FlameVisualizer::reset() {
    numHitCounterImages = 1;
    activeHitCounterImageIndex = 0;
    resetAllHitCounterImages = true;
    fractPars.reset();


    for(int i=0; i<spectrums.size(); i++) {
      spectrums[i].assign(spectrums[i].size(), 0);
    }
    spectrumSize = 7;
    spectrumAveraging = 5;
    spectrumScalings.assign(spectrumScalings.size(), 1.0);

    clearInterval = 0;
    spectrumMinimumThreshold = 0;
    currentClearStep = 0;

    spectrumScaling = 100.0;
    brightness = 1.0;

    for(int i=0; i<numFlameVariations; i++) {
      for(int k=0; k<maxNumFlameParameters; k++) {
        flameParameterSpectrumAssociation[i*maxNumFlameParameters + k] = k < 7 ? k : 0;
      }
    }

    fractPars.maxIter = 30;

    updatePanel();
    for(int i=0; i<flameParameterPanels.size(); i++) {
      flameParameterPanels[i]->update();
    }
  }


  FlameVisualizer::~FlameVisualizer() {}

  void FlameVisualizer::init(Events &events, int w, int h, GuiElement *parentGuiElement) {
    this->w = w;
    this->h = h;

    scene3D = new Scene3D(w, h);
    scene3D->voxelGrid.setSize(fractPars.voxelGridSize.x, fractPars.voxelGridSize.y, fractPars.voxelGridSize.z);
    scene3D->setUseGpu(true);
    //voxels.resize(scene3D->voxelGrid.voxels.size());
    fractPars.xRange = scene3D->voxelGrid.xRange;
    fractPars.yRange = scene3D->voxelGrid.yRange;
    fractPars.zRange = scene3D->voxelGrid.zRange;
    requestedVoxelGridSize = fractPars.voxelGridSize;


    flameParameterSpectrumAssociation.resize(totalNumFlameParameters);
    flameParameterSpectrumAssociationStrength.resize(totalNumFlameParameters);
    flameParameterProgressAmount.resize(totalNumFlameParameters);
    flameParameterProgression.resize(totalNumFlameParameters);


    spectrumScalings.resize(10, 1);

    for(int i=0; i<numFlameVariations; i++) {
      for(int k=0; k<maxNumFlameParameters; k++) {
        flameParameterSpectrumAssociation[i*maxNumFlameParameters + k] = k < 7 ? k : 0;
      }
    }

    hitCounterImageArray.createArray(w, h, maxNumHitCounterImages, Texture::PixelType::UINT);
    minMaxHitCounterImage.create(2, 1, Texture::PixelType::UINT);
    minMaxHitCounterVoxels.create(2, 1, Texture::PixelType::UINT);

    hitCounterVoxels.createArray(fractPars.voxelGridSize.x, fractPars.voxelGridSize.y, fractPars.voxelGridSize.z, Texture::PixelType::RGBA);

    fractPars.init(events, w, h);
    fractPars.setInputActive(false);
    fractPars.maxIter = 30;

    quad.create(1, 1, false);
    loadShader();

    addPanel(parentGuiElement);
    panel->setVisible(false);

  }


  void FlameVisualizer::loadShader() {
    flameShaderPass1.create("data/glsl/texture.vert", "data/glsl/flamePassX1.frag");
    flameShaderPass2.create("data/glsl/texture.vert", "data/glsl/flamePassX2.frag");
    flameShaderPass3.create("data/glsl/texture.vert", "data/glsl/flamePassX3.frag");
    flameShaderPass4.create("data/glsl/texture.vert", "data/glsl/flamePassX4.frag");
    flameShader3DPass1.create("data/glsl/texture.vert", "data/glsl/flamePass3D1.frag");
    flameShader3DPass2.create("data/glsl/texture.vert", "data/glsl/flamePass3D2.frag");
    flameShader3DPass3.create("data/glsl/texture.vert", "data/glsl/flamePass3D3.frag");
    flameShader3DPass4.create("data/glsl/texture.vert", "data/glsl/flamePass3D4.frag");
    scene3D->reloadShaders();
  }

  void FlameVisualizer::setFlame3D(bool value) {
      useVoxelFlame = value;
      fractPars.softwareRenderMode = useVoxelFlame ? 3 : 2;
  }

  void FlameVisualizer::onKeyUp(Events &events) {
    if(view3dActive) {
      scene3D->onKeyUp(events);
    }
  }

  void FlameVisualizer::onKeyDown(Events &events) {
    if(!isInputActive) return;

    if(view3dActive) {
      scene3D->onKeyDown(events);
    }

    if(events.sdlKeyCode == SDLK_LESS) {
        view3dActive = !view3dActive;
        fractPars.setInputActive(!view3dActive);
    }

    if(events.sdlKeyCode == SDLK_m) {
        printf("%d x %d x %d\n", scene3D->voxelGridTexture->w, scene3D->voxelGridTexture->h, scene3D->voxelGridTexture->textureArraySize);

        Vec3d pos;
        for(int i=0; i<scene3D->voxelGrid.w; i++) {
          pos.x = map(i, 0, scene3D->voxelGrid.w-1, fractPars.xRange.x, fractPars.xRange.y);
          int xBorder = i == 0 || i == scene3D->voxelGrid.w-1 ? 1 : 0;
          for(int j=0; j<scene3D->voxelGrid.h; j++) {
            pos.y = map(j, 0, scene3D->voxelGrid.h-1, fractPars.yRange.x, fractPars.yRange.y);
            int yBorder = j == 0 || j == scene3D->voxelGrid.h-1 ? 1 : 0;
            for(int k=0; k<scene3D->voxelGrid.d; k++) {
              pos.z = map(k, 0, scene3D->voxelGrid.d-1, fractPars.zRange.x, fractPars.zRange.y);
              int zBorder = k == 0 || k == scene3D->voxelGrid.d-1 ? 1 : 0;
              bool isBorder = xBorder + yBorder + zBorder >= 2;
              double d = pos.length();
              Vec4d color;
              if(isBorder) {
                //color.set(0.5, 0.5, 0.5, 0.5);
              }
              Vec3d a(0.5, 0.5, 0.3), b(-0.4, -0.2, -0.3);
              double r1 = Vec3d(pos-a).length();
              double r2 = Vec3d(pos-b).length();
              Vec4d aCol(0.7, 0.1, 0.2, 1), bCol(0.1, 0.2, 0.8, 1);
              color = aCol * (1.0/(r1*r1)) + bCol * (1.0/(r2*r2));
              color.y += (1.0+pos.y) * 0.04;
              color.w = rgbToAlpha(color.x, color.y, color.z);
              //scene3D->voxelGrid.setVoxel(i, j, k, color);
              scene3D->voxelGridTexture->setVoxel(i, j, k, color);
            }
          }
        }
        scene3D->voxelGridTexture->applyPixels();
      }




    if(events.rControlDown && panel) {
      if(events.sdlKeyCode == SDLK_1) flameParameterPanels[0]->toggleVisibility();
      if(events.sdlKeyCode == SDLK_2) flameParameterPanels[1]->toggleVisibility();
      if(events.sdlKeyCode == SDLK_3) flameParameterPanels[2]->toggleVisibility();
      if(events.sdlKeyCode == SDLK_4) flameParameterPanels[3]->toggleVisibility();
      if(events.sdlKeyCode == SDLK_5) flameParameterPanels[4]->toggleVisibility();
      if(events.sdlKeyCode == SDLK_6) flameParameterPanels[5]->toggleVisibility();
      if(events.sdlKeyCode == SDLK_7) flameParameterPanels[6]->toggleVisibility();
      if(events.sdlKeyCode == SDLK_8) flameParameterPanels[7]->toggleVisibility();
      if(events.sdlKeyCode == SDLK_9) flameParameterPanels[8]->toggleVisibility();
      if(events.sdlKeyCode == SDLK_0) flameParameterPanels[9]->toggleVisibility();
    }

    else {
        if(!useVoxelFlame) {
          int k = events.sdlKeyCode - SDLK_1;
          if(k >= 0 && k <= 10) {
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
      else {
        int k = events.sdlKeyCode - SDLK_1;
        if(k >= 0 && k <= 10) {
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

      if(events.sdlKeyCode == SDLK_HOME && panel) {
        panel->toggleVisibility();
      }
    }
  }


  void FlameVisualizer::onMouseMotion(Events &events) {
      if(view3dActive || (events.lAltDown && events.rAltDown)) {
        scene3D->onMouseMotion(events);
      }
    }


  void FlameVisualizer::onMouseWheel(Events &events) {
    if(!isInputActive) return;

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


  void FlameVisualizer::update(const std::vector<double> &spectrum, double volume) {
    if(spectrums.size() != spectrumAveraging) {
      spectrums.resize(spectrumAveraging);
    }
    for(int i=0; i<spectrums.size(); i++) {
      if(spectrums[i].size() != spectrumSize+1) {
        spectrums[i].resize(spectrumSize+1);
      }
    }

    if(this->spectrum.size() != spectrumSize+1) {
      this->spectrum.resize(spectrumSize+1);
    }
    resizeArray(spectrum, this->spectrum, -1, -1, 1, -1);
    this->spectrum[0] = volume;
    spectrums.put(this->spectrum);

    getAverageArray(spectrums, this->spectrum);

    if(spectrumMinimumThreshold > 0) {
      double minValue = getArrayMin(this->spectrum);
      double maxValue = getArrayMax(this->spectrum);
      for(int i=0; i<this->spectrum.size(); i++) {
        this->spectrum[i] = max(0.0, this->spectrum[i] - spectrumMinimumThreshold);
        if(this->spectrum[i] > 0 && maxValue - spectrumMinimumThreshold > 0) {
          this->spectrum[i] = this->spectrum[i] * maxValue / (maxValue - spectrumMinimumThreshold);
        }
      }
    }

    scaleArray(this->spectrum, spectrumScaling);
  }





  void FlameVisualizer::render(TextGl &textRenderer, double w, double h, double time, double dt, const Events &events, bool isDarkGuiColors) {
    quad.setSize(w, h);
   //printf("(debugging) at FlameVisualizer.render() -4\n" );

    if(spectrumPanel) {
      spectrumPanel->prerenderingNeeded = true;
      spectrumPanel->numHorizontalMarks = spectrum.size();
    }

    if(useVoxelFlame) {
      if(!view3dActive) {
          if(requestedVoxelGridSize != fractPars.voxelGridSize) {
            fractPars.voxelGridSize = requestedVoxelGridSize;
            scene3D->setVoxelGridSize(requestedVoxelGridSize.x, requestedVoxelGridSize.y, requestedVoxelGridSize.z);
            fractPars.xRange = scene3D->voxelGrid.xRange;
            fractPars.yRange = scene3D->voxelGrid.yRange;
            fractPars.zRange = scene3D->voxelGrid.zRange;
            //voxels.resize(requestedVoxelGridSize.x * requestedVoxelGridSize.y * requestedVoxelGridSize.z);
            hitCounterVoxels.createArray(fractPars.voxelGridSize.x, fractPars.voxelGridSize.y, fractPars.voxelGridSize.z, Texture::PixelType::RGBA);
            fractPars.viewChanged = true;
          }

          if(fractPars.viewChanged || (clearInterval != 0 && currentClearStep % clearInterval == 0)) {
            /*if(resetAllHitCounterImages) {
              for(int i=0; i<maxNumHitCounterImages; i++) {
                hitCounterImageMinMaxs[i].set(-1, 0);
                //hitCounterImages[i]->clearGl();
              }
              hitCounterImageArray.clearGl();
              resetAllHitCounterImages = false;
            }*/
            //else {
              //hitCounterImageMinMaxs[activeHitCounterImageIndex].set(-1, 0);
              flameShader3DPass4.activate();
              flameShader3DPass4.setUniform3i("gridSize", fractPars.voxelGridSize);
              hitCounterVoxels.activateImageStorage(flameShader3DPass4, "hitCounterVoxels", 0);
              quad.setSize(fractPars.voxelGridSize.x, fractPars.voxelGridSize.y);
              quad.render();
              hitCounterVoxels.inactivate(0);
              flameShader3DPass4.deActivate();
            //}
            //printf("resetting...\n");
          }
          minMaxHitCounterVoxels.uintPixels[0] = -1;
          minMaxHitCounterVoxels.uintPixels[1] = 0;
          minMaxHitCounterVoxels.applyPixels();
          /*else {
            minMaxHitCounterImage.uintPixels[0] = (unsigned int)hitCounterImageMinMaxs[activeHitCounterImageIndex].x;
            minMaxHitCounterImage.uintPixels[1] = (unsigned int)hitCounterImageMinMaxs[activeHitCounterImageIndex].y;
            minMaxHitCounterImage.applyPixels();
          }*/
          if(fractPars.viewChanged) {
            updatePanel();
            fractPars.viewChanged = false;
          }

          currentClearStep++;

          flameShader3DPass1.activate();
          flameShader3DPass1.setUniform2f("screenSize", w, h);
          flameShader3DPass1.setUniform2f("xRange", fractPars.xRange);
          flameShader3DPass1.setUniform2f("yRange", fractPars.yRange);
          flameShader3DPass1.setUniform2f("zRange", fractPars.zRange);
          flameShader3DPass1.setUniform1f("time", time);
          flameShader3DPass1.setUniform3f("location", fractPars.location3d);
          flameShader3DPass1.setUniform1f("scale", fractPars.scale);
          flameShader3DPass1.setUniform1f("maxIter", fractPars.maxIter);
          flameShader3DPass1.setUniform1f("minIter", fractPars.minIter);
          flameShader3DPass1.setUniform3i("gridSize", fractPars.voxelGridSize);
          std::vector<float> flameParameters(numFlameVariations*numFlameParameters3D, 0);
          std::vector<int> flameVariations(numFlameVariations, 0);
          for(int i=0; i<numFlameVariations; i++) {
            flameVariations[i] = fractPars.flamePars[i].variation;

            for(int k=0; k<numFlameParameters3D; k++) {
              if(flameParameterSpectrumAssociation[maxNumFlameParameters*i + k] >= 0 && flameParameterSpectrumAssociation[maxNumFlameParameters*i + k] < spectrum.size()-1) {
                if(flameParameterProgressAmount[maxNumFlameParameters*i + k] == 0) {
                  double d = flameParameterSpectrumAssociationStrength[maxNumFlameParameters*i + k] * spectrum[flameParameterSpectrumAssociation[maxNumFlameParameters*i + k]] * spectrumScalings[i];
                  flameParameters[i*maxNumFlameParameters + k] = fractPars.flamePars[i].p3d[k] + d;
                }
                else {
                  flameParameterProgression[i*maxNumFlameParameters + k] += spectrum[flameParameterSpectrumAssociation[maxNumFlameParameters*i + k]] * flameParameterProgressAmount[i*maxNumFlameParameters + k] * dt;
                  flameParameters[i*maxNumFlameParameters + k] = fractPars.flamePars[i].p3d[k] + (0.5 + 0.5*sin(flameParameterProgression[i*maxNumFlameParameters+k])) * flameParameterSpectrumAssociationStrength[numFlameParameters3D*i+k] ;
                }
              }
              else {
                flameParameters[i*maxNumFlameParameters + k] = fractPars.flamePars[i].p3d[k];
              }
            }
          }
          flameShader3DPass1.setUniform1iv("variations", flameVariations.size(), flameVariations.data());
          flameShader3DPass1.setUniform1fv("parameters3D", flameParameters.size(), flameParameters.data());
          //flameShaderPass1.setUniform1i("activeHitCounterImageIndex", activeHitCounterImageIndex);

          hitCounterVoxels.activateImageStorage(flameShader3DPass1, "hitCounterVoxels", 0);
          quad.setSize(w, h); // This is quite arbitrary size here, because it defines the amount of paths which
          quad.render();      // are iterated through the voxel grid
          hitCounterVoxels.inactivate(0);
          flameShader3DPass1.deActivate();



          flameShader3DPass2.activate();
          flameShader3DPass2.setUniform3i("gridSize", fractPars.voxelGridSize);
          //flameShader3DPass2.setUniform1i("activeHitCounterImageIndex", activeHitCounterImageIndex);
          hitCounterVoxels.activateImageStorage(flameShader3DPass2, "hitCounterVoxels", 1);
          minMaxHitCounterVoxels.activateImageStorage(flameShader3DPass2, "minMaxHitCounterImage", 0);
          quad.setSize(fractPars.voxelGridSize.x, fractPars.voxelGridSize.y);
          quad.render();
          minMaxHitCounterVoxels.inactivate(0);
          hitCounterVoxels.inactivate(1);
          flameShader3DPass2.deActivate();

          minMaxHitCounterVoxels.loadGlPixels();
          unsigned int minHits = minMaxHitCounterVoxels.uintPixels[0];
          unsigned int maxHits = minMaxHitCounterVoxels.uintPixels[1];

          flameShader3DPass3.activate();
          flameShader3DPass3.setUniform1fv("colorArgs", 3, fractPars.colorArg);
          flameShader3DPass3.setUniform1f("brightness", fabs(brightness));
          flameShader3DPass3.setUniform3i("gridSize", fractPars.voxelGridSize);
          flameShader3DPass3.setUniform1f("minValue", minHits);
          flameShader3DPass3.setUniform1f("maxValue", maxHits);
          //flameShader3DPass3.setUniform1f("inverseColors", brightness < 0 ? 1.0 : 0.0);
          //flameShader3DPass3.setUniform1i("numHitCounterImages", numHitCounterImages);
          //flameShader3DPass3.setUniform2fv("hitCounterImageMinMaxs", numHitCounterImages, hitCounterImageMinMaxs);
          scene3D->voxelGridTexture->activateImageStorage(flameShader3DPass3, "colorVoxels", 2);
          hitCounterVoxels.activateImageStorage(flameShader3DPass3, "hitCounterVoxels", 1);
          minMaxHitCounterVoxels.activateImageStorage(flameShader3DPass3, "minMaxHitCounterImage", 0);
          quad.setSize(fractPars.voxelGridSize.x, fractPars.voxelGridSize.y);
          quad.render();
          minMaxHitCounterVoxels.inactivate(0);
          hitCounterVoxels.inactivate(1);
          scene3D->voxelGridTexture->inactivate(2);
          flameShader3DPass3.deActivate();
      }
      scene3D->updateView();
      scene3D->update(time);
      scene3D->draw(0, 0, w, h);
      if(view3dActive) {
        scene3D->printInfo(textRenderer, 0, 0, w, h, 1, true);
      }
    }
    else {
      quad.setSize(w, h);
      activeHitCounterImageIndex = (activeHitCounterImageIndex + 1 ) % numHitCounterImages;

      if(fractPars.viewChanged || (clearInterval != 0 && currentClearStep % clearInterval == 0)) {
        //hitCounterImage.clearGl();
        if(resetAllHitCounterImages) {
          for(int i=0; i<maxNumHitCounterImages; i++) {
            hitCounterImageMinMaxs[i].set(-1, 0);
            //hitCounterImages[i]->clearGl();
          }
          hitCounterImageArray.clearGl();
          resetAllHitCounterImages = false;
        }
        else {
          hitCounterImageMinMaxs[activeHitCounterImageIndex].set(-1, 0);
          //hitCounterImages[activeHitCounterImageIndex]->clear();
          //hitCounterImageArray.clearArrayLayer(activeHitCounterImageIndex);
          flameShaderPass4.activate();
          flameShaderPass4.setUniform1i("activeHitCounterImageIndex", activeHitCounterImageIndex);
          hitCounterImageArray.activateImageStorage(flameShaderPass4, "hitCounterImageArray", 0);
          quad.render();
          hitCounterImageArray.inactivate(0);
          flameShaderPass4.deActivate();
        }
        minMaxHitCounterImage.uintPixels[0] = -1;
        minMaxHitCounterImage.uintPixels[1] = 0;
        minMaxHitCounterImage.applyPixels();
      }
      else {
        minMaxHitCounterImage.uintPixels[0] = (unsigned int)hitCounterImageMinMaxs[activeHitCounterImageIndex].x;
        minMaxHitCounterImage.uintPixels[1] = (unsigned int)hitCounterImageMinMaxs[activeHitCounterImageIndex].y;
        minMaxHitCounterImage.applyPixels();
      }
      if(fractPars.viewChanged) {
        updatePanel();
        fractPars.viewChanged = false;
      }

      currentClearStep++;

      flameShaderPass1.activate();
      flameShaderPass1.setUniform2f("screenSize", w, h);
      flameShaderPass1.setUniform1f("time", time);
      flameShaderPass1.setUniform2f("location", fractPars.location.x, -fractPars.location.y);
      flameShaderPass1.setUniform1f("scale", fractPars.scale);
      flameShaderPass1.setUniform1f("maxIter", fractPars.maxIter);
      flameShaderPass1.setUniform1f("minIter", fractPars.minIter);
      std::vector<float> flameParameters(numFlameVariations*numFlameParameters2D, 0);
      std::vector<int> flameVariations(numFlameVariations, 0);

      for(int i=0; i<numFlameVariations; i++) {
        flameVariations[i] = fractPars.flamePars[i].variation;

        for(int k=0; k<numFlameParameters2D; k++) {
          if(flameParameterSpectrumAssociation[maxNumFlameParameters*i + k] >= 0 && flameParameterSpectrumAssociation[maxNumFlameParameters*i + k] < spectrum.size()-1) {
            if(flameParameterProgressAmount[maxNumFlameParameters*i + k] == 0) {
              double d = flameParameterSpectrumAssociationStrength[maxNumFlameParameters*i + k] * spectrum[flameParameterSpectrumAssociation[maxNumFlameParameters*i + k]] * spectrumScalings[i];
              flameParameters[i*numFlameParameters2D + k] = fractPars.flamePars[i].p[k] + d;
            }
            else {
              flameParameterProgression[i*maxNumFlameParameters + k] += spectrum[flameParameterSpectrumAssociation[maxNumFlameParameters*i + k]] * flameParameterProgressAmount[i*maxNumFlameParameters + k] * dt;
              flameParameters[i*numFlameParameters2D + k] = fractPars.flamePars[i].p[k] + (0.5 + 0.5*sin(flameParameterProgression[i*maxNumFlameParameters+k])) * flameParameterSpectrumAssociationStrength[maxNumFlameParameters*i+k] ;
            }
          }
          else {
            flameParameters[i*numFlameParameters2D + k] = fractPars.flamePars[i].p[k];
          }
        }
      }
      flameShaderPass1.setUniform1iv("variations", flameVariations.size(), flameVariations.data());
      flameShaderPass1.setUniform1fv("parameters", flameParameters.size(), flameParameters.data());
      flameShaderPass1.setUniform1i("activeHitCounterImageIndex", activeHitCounterImageIndex);

      hitCounterImageArray.activateImageStorage(flameShaderPass1, "hitCounterImageArray", 0);
      quad.render();
      hitCounterImageArray.inactivate(0);
      flameShaderPass1.deActivate();

      flameShaderPass2.activate();
      flameShaderPass2.setUniform1i("activeHitCounterImageIndex", activeHitCounterImageIndex);
      hitCounterImageArray.activateImageStorage(flameShaderPass2, "hitCounterImageArray", 1);
      minMaxHitCounterImage.activateImageStorage(flameShaderPass2, "minMaxHitCounterImage", 0);
      quad.render();
      minMaxHitCounterImage.inactivate(0);
      hitCounterImageArray.inactivate(1);
      flameShaderPass2.deActivate();

      minMaxHitCounterImage.loadGlPixels();
      hitCounterImageMinMaxs[activeHitCounterImageIndex].x = minMaxHitCounterImage.uintPixels[0];
      hitCounterImageMinMaxs[activeHitCounterImageIndex].y = minMaxHitCounterImage.uintPixels[1];

      flameShaderPass3.activate();
      flameShaderPass3.setUniform1fv("colorArgs", 3, fractPars.colorArg);
      flameShaderPass3.setUniform1f("brightness", fabs(brightness));
      flameShaderPass3.setUniform1f("inverseColors", brightness < 0 ? 1.0 : 0.0);
      flameShaderPass3.setUniform1i("numHitCounterImages", numHitCounterImages);
      flameShaderPass3.setUniform2fv("hitCounterImageMinMaxs", numHitCounterImages, hitCounterImageMinMaxs);
      hitCounterImageArray.activateImageStorage(flameShaderPass3, "hitCounterImageArray", 0);
      quad.render();
      hitCounterImageArray.inactivate(0);
      flameShaderPass3.deActivate();
    }

    /*hitCounterImages[activeHitCounterImageIndex]->activateImageStorage(flameShaderPass1, "hitCounterImage", 0);
    quad.render();
    hitCounterImages[activeHitCounterImageIndex]->inactivate(0);
    flameShaderPass1.deActivate();

    flameShaderPass2.activate();
    hitCounterImages[activeHitCounterImageIndex]->activateImageStorage(flameShaderPass2, "hitCounterImage", 1);
    minMaxHitCounterImage.activateImageStorage(flameShaderPass2, "minMaxHitCounterImage", 0);
    quad.render();
    minMaxHitCounterImage.inactivate(0);
    hitCounterImage.inactivate(1);
    flameShaderPass2.deActivate();

    minMaxHitCounterImage.loadGlPixels();
    hitCounterImageMinMaxs[activeHitCounterImageIndex].x = minMaxHitCounterImage.uintPixels[0];
    hitCounterImageMinMaxs[activeHitCounterImageIndex].y = minMaxHitCounterImage.uintPixels[1];

    flameShaderPass3.activate();
    flameShaderPass3.setUniform1f("brightness", fabs(brightness));
    flameShaderPass3.setUniform1f("inverseColors", brightness < 0 ? 1.0 : 0.0);
    flameShaderPass3.setUniform1i("numHitCounterImages", numHitCounterImages);
    flameShaderPass3.setUniform2fv("hitCounterImageMinMaxs", numHitCounterImages, hitCounterImageMinMaxs);

    for(int i=0; i<numHitCounterImages; i++) {
      hitCounterImages[i]->activateImageStorage(flameShaderPass3, "hitCounterImages[" + std::to_string(i) + "]", i);
    }
    quad.render();
    for(int i=0; i<numHitCounterImages; i++) {
      hitCounterImages[i]->inactivate(i);
    }
    flameShaderPass3.deActivate();*/
    /*
    hitCounterImage.activateImageStorage(flameShaderPass1, "hitCounterImage", 0);
    quad.render();
    hitCounterImage.inactivate(0);
    flameShaderPass1.deActivate();

    flameShaderPass2.activate();
    hitCounterImage.activateImageStorage(flameShaderPass2, "hitCounterImage", 1);
    minMaxHitCounterImage.activateImageStorage(flameShaderPass2, "minMaxHitCounterImage", 0);
    quad.render();
    minMaxHitCounterImage.inactivate(0);
    hitCounterImage.inactivate(1);
    flameShaderPass2.deActivate();

    flameShaderPass3.activate();
    hitCounterImage.activateImageStorage(flameShaderPass3, "hitCounterImage", 1);
    minMaxHitCounterImage.activateImageStorage(flameShaderPass3, "minMaxHitCounterImage", 0);
    quad.render();
    minMaxHitCounterImage.inactivate(0);
    hitCounterImage.inactivate(1);
    flameShaderPass3.deActivate();
*/

    //bufferImage.unsetRenderTarget();
    //bufferImage.render();

    //printf("(debugging) at FlameVisualizer.render() 1\n" );
    if(showInfo && !view3dActive) {
      std::wstring str;
      wchar_t buf[256];
      std::swprintf(buf, 256, L"(%.4g, %.4g), %.4g, %d\n", fractPars.location.x, fractPars.location.y, 1.0/fractPars.scale, int(fractPars.maxIter));
      str += buf;

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
      //printf("(debugging) at FlameVisualizer.render() 2\n" );
      for(int i=0; i<fractPars.numActiveVariations; i++) {
        if(fractPars.flamePars[i].variation < 0) continue;
        str += L"\n";
        if(i == fractPars.activeFlamePars) str += L"*";

        if(!useVoxelFlame) {
          std::swprintf(buf, 256, L"%.2g, (%.2g %.2g %.2g), (%.2g %.2g %.2g)", fractPars.flamePars[i].p[0], fractPars.flamePars[i].p[1], fractPars.flamePars[i].p[2], fractPars.flamePars[i].p[3], fractPars.flamePars[i].p[4], fractPars.flamePars[i].p[5], fractPars.flamePars[i].p[6]);
          str += buf;
          for(int j=7; j<numFlamePars[min(fractPars.flamePars[i].variation, 49)]; j++) {
            std::swprintf(buf, 256, L", %.2g", fractPars.flamePars[i].p[j]);
            str += buf;
          }
        }
        else {
          std::swprintf(buf, 256, L"%.2g, (%.2g %.2g %.2g %.2g), (%.2g %.2g %.2g %.2g), (%.2g %.2g %.2g %.2g)", fractPars.flamePars[i].p3d[0], fractPars.flamePars[i].p3d[1], fractPars.flamePars[i].p3d[2], fractPars.flamePars[i].p3d[3], fractPars.flamePars[i].p3d[4], fractPars.flamePars[i].p3d[5], fractPars.flamePars[i].p3d[6], fractPars.flamePars[i].p3d[7], fractPars.flamePars[i].p3d[8], fractPars.flamePars[i].p3d[9], fractPars.flamePars[i].p3d[10], fractPars.flamePars[i].p3d[11], fractPars.flamePars[i].p3d[12]);
          str += buf;
          for(int j=7+6; j<6+numFlamePars[min(fractPars.flamePars[i].variation, 49)]; j++) {
            std::swprintf(buf, 256, L", %.2g", fractPars.flamePars[i].p3d[j]);
            str += buf;
          }
        }
        /*std::swprintf(buf, 256, L"%.2g, (%.2g %.2g %.2g), (%.2g %.2g %.2g)", fractPars.flamePars[i].p[0], fractPars.flamePars[i].p[1], fractPars.flamePars[i].p[2], fractPars.flamePars[i].p[3], fractPars.flamePars[i].p[4], fractPars.flamePars[i].p[5], fractPars.flamePars[i].p[6]);
        str += buf;
        for(int j=7; j<numFlamePars[min(fractPars.flamePars[i].variation, 49)]; j++) {
          std::swprintf(buf, 256, L", %.2g", fractPars.flamePars[i].p[j]);
          str += buf;
        }*/
      }
      //printf("(debugging) at FlameVisualizer.render() 3\n" );
      str += L"\n";
      std::swprintf(buf, 256, L"h %.2g, r %.2g, s %.2g", fractPars.colorArg[0], fractPars.colorArg[1], fractPars.colorArg[2]);
      str += buf;

      str += L"\n";

      if(isDarkGuiColors) {
        textRenderer.setColor(0, 0, 0, 1);
      }
      else {
        textRenderer.setColor(1, 1, 1, 1);
      }
      textRenderer.print(str, 10, 10, 10, 1);
      //printf("(debugging) at FlameVisualizer.render() 4\n" );
    }
  }

  void FlameVisualizer::setActive(bool isActive) {
    this->isInputActive = isActive;
    fractPars.setInputActive(isActive);
  }



    FlameVisualizer::FlameParameterPanel::~FlameParameterPanel() {
      deleteChildElements();
    }

    FlameVisualizer::FlameParameterPanel::FlameParameterPanel(FlameVisualizer *flameVisualizer, int variationIndex) : Panel("Flame parameter panel "+std::to_string(variationIndex+1), 0, 0, 1, 1) {
      init(flameVisualizer, variationIndex);
    }
    void FlameVisualizer::FlameParameterPanel::init(FlameVisualizer *flameVisualizer, int variationIndex) {
      this->flameVisualizer = flameVisualizer;
      this->variationIndex = variationIndex;

      double line = 10, lineHeight = 23;
      double columnA = 10, columnB = 120, columnC = 210, columnD = 300;

      std::string labelText = "Variation " + std::to_string(variationIndex + 1);

      if(flameVisualizer->fractPars.flamePars[variationIndex].variation >= 0) {
        labelText += " " + flameVisualizer->fractPars.flamePars[variationIndex].getVariationName();
      }
      variationLabelGui = new Label(labelText, columnA, line);
      this->addChildElement(variationLabelGui);

      variationGui = new NumberBox("#", flameVisualizer->fractPars.flamePars[variationIndex].variation, NumberBox::INTEGER, -1, FractalParameters::numFlameVariations-1, columnA, line +=  lineHeight, 4);
      variationGui->incrementMode = NumberBox::IncrementMode::Linear;
      this->addChildElement(variationGui);

      spectrumScalingGui = new NumberBox("Local spectrum scaling", flameVisualizer->spectrumScalings[variationIndex], NumberBox::FLOATING_POINT, -1e10, 1e10, 10, line += lineHeight, 8);
      this->addChildElement(spectrumScalingGui);


      this->addChildElement(new Label("parameter", columnA, line+=lineHeight));
      this->addChildElement(new Label("spec. pos.", columnB, line));
      this->addChildElement(new Label("spec. sensit.", columnC, line));
      this->addChildElement(new Label("progression", columnD, line));

      parametersGui.resize(flameVisualizer->maxNumFlameParameters);
      spectrumPositionsGui.resize(flameVisualizer->maxNumFlameParameters);
      spectrumStrengthsGui.resize(flameVisualizer->maxNumFlameParameters);
      progressAmountsGui.resize(flameVisualizer->maxNumFlameParameters);

      for(int i=0; i<flameVisualizer->maxNumFlameParameters; i++) {
        parametersGui[i] = new NumberBox(""+std::to_string(i+1), flameVisualizer->useVoxelFlame ? flameVisualizer->fractPars.flamePars[variationIndex].p3d[i] : flameVisualizer->fractPars.flamePars[variationIndex].p[i], NumberBox::FLOATING_POINT, -1e10, 1e10, columnA, line += lineHeight, 7);
        spectrumPositionsGui[i] = new NumberBox("", flameVisualizer->flameParameterSpectrumAssociation[variationIndex*FlameVisualizer::maxNumFlameParameters + i], NumberBox::INTEGER, -1, flameVisualizer->spectrumSize-1, columnB, line, 7);
        spectrumPositionsGui[i]->incrementMode = NumberBox::IncrementMode::Linear;
        spectrumStrengthsGui[i] = new NumberBox("", flameVisualizer->flameParameterSpectrumAssociationStrength[variationIndex*FlameVisualizer::maxNumFlameParameters + i], NumberBox::FLOATING_POINT, -1e10, 1e10, columnC, line, 7);
        progressAmountsGui[i] = new NumberBox("", flameVisualizer->flameParameterProgressAmount[variationIndex*FlameVisualizer::maxNumFlameParameters + i], NumberBox::FLOATING_POINT, -1e10, 1e10, columnD, line, 7);
        this->addChildElement(parametersGui[i]);
        this->addChildElement(spectrumPositionsGui[i]);
        this->addChildElement(spectrumStrengthsGui[i]);
        this->addChildElement(progressAmountsGui[i]);
        bool isVisible = i < flameVisualizer->fractPars.flamePars[variationIndex].getNumParameters() + (flameVisualizer->useVoxelFlame ? 6 : 0);
        parametersGui[i]->setVisible(isVisible);
        spectrumPositionsGui[i]->setVisible(isVisible);
        spectrumStrengthsGui[i]->setVisible(isVisible);
        progressAmountsGui[i]->setVisible(isVisible);
      }

      this->addGuiEventListener(new FlameParameterPanelListener(this));

      this->setSize(400, 20 + 23 * (4 + flameVisualizer->fractPars.flamePars[variationIndex].getNumParameters() + (flameVisualizer->useVoxelFlame ? 6 : 0)));
    }


    void FlameVisualizer::FlameParameterPanel::update() {
      std::string labelText = "Variation " + std::to_string(variationIndex + 1);

      if(flameVisualizer->fractPars.flamePars[variationIndex].variation >= 0) {
        labelText += " - #" + std::to_string(flameVisualizer->fractPars.flamePars[variationIndex].variation);
        labelText += " " + flameVisualizer->fractPars.flamePars[variationIndex].getVariationName();
      }
      variationLabelGui->setText(labelText);

      variationGui->setValue(flameVisualizer->fractPars.flamePars[variationIndex].variation);

      for(int i=0; i<flameVisualizer->maxNumFlameParameters; i++) {
        bool isVisible = i < flameVisualizer->fractPars.flamePars[variationIndex].getNumParameters() + (flameVisualizer->useVoxelFlame ? 6 : 0);
        parametersGui[i]->setVisible(isVisible);
        spectrumPositionsGui[i]->setVisible(isVisible);
        spectrumStrengthsGui[i]->setVisible(isVisible);
        progressAmountsGui[i]->setVisible(isVisible);
        if(isVisible) {
          spectrumPositionsGui[i]->setLimits(-1, flameVisualizer->spectrumSize);
          parametersGui[i]->setValue(flameVisualizer->useVoxelFlame ? flameVisualizer->fractPars.flamePars[variationIndex].p3d[i] : flameVisualizer->fractPars.flamePars[variationIndex].p[i]);
          spectrumPositionsGui[i]->setValue(flameVisualizer->flameParameterSpectrumAssociation[variationIndex*FlameVisualizer::maxNumFlameParameters + i]);
          spectrumStrengthsGui[i]->setValue(flameVisualizer->flameParameterSpectrumAssociationStrength[variationIndex*FlameVisualizer::maxNumFlameParameters + i]);
          progressAmountsGui[i]->setValue(flameVisualizer->flameParameterProgressAmount[variationIndex*FlameVisualizer::maxNumFlameParameters + i]);
        }
      }
      this->setSize(400, 20 + 23 * (4 + flameVisualizer->fractPars.flamePars[variationIndex].getNumParameters() + (flameVisualizer->useVoxelFlame ? 6 : 0)));
    }

      FlameVisualizer::FlameParameterPanel::FlameParameterPanelListener::FlameParameterPanelListener(FlameParameterPanel *flameParameterPanel) {
        this->flameParameterPanel = flameParameterPanel;
      }
      void FlameVisualizer::FlameParameterPanel::FlameParameterPanelListener::onValueChange(GuiElement *guiElement) {
        if(guiElement == flameParameterPanel->variationGui) {
          guiElement->getValue((void*)&flameParameterPanel->flameVisualizer->fractPars.flamePars[flameParameterPanel->variationIndex].variation);
          flameParameterPanel->flameVisualizer->fractPars.viewChanged = true;
          flameParameterPanel->update();
        }
        if(guiElement == flameParameterPanel->spectrumScalingGui) {
          guiElement->getValue((void*)&flameParameterPanel->flameVisualizer->spectrumScalings[flameParameterPanel->variationIndex]);
        }
        for(int i=0; i<flameParameterPanel->flameVisualizer->fractPars.flamePars[flameParameterPanel->variationIndex].getNumParameters() + (flameParameterPanel->flameVisualizer->useVoxelFlame ? 6 : 0); i++) {
          if(guiElement == flameParameterPanel->parametersGui[i]) {
            guiElement->getValue((void*)&(flameParameterPanel->flameVisualizer->useVoxelFlame ? flameParameterPanel->flameVisualizer->fractPars.flamePars[flameParameterPanel->variationIndex].p3d[i] : flameParameterPanel->flameVisualizer->fractPars.flamePars[flameParameterPanel->variationIndex].p[i]));
            flameParameterPanel->flameVisualizer->fractPars.viewChanged = true;
          }
          if(guiElement == flameParameterPanel->spectrumPositionsGui[i]) {
            guiElement->getValue((void*)&flameParameterPanel->flameVisualizer->flameParameterSpectrumAssociation[flameParameterPanel->variationIndex*FlameVisualizer::maxNumFlameParameters + i]);
          }
          if(guiElement == flameParameterPanel->spectrumStrengthsGui[i]) {
            guiElement->getValue((void*)&flameParameterPanel->flameVisualizer->flameParameterSpectrumAssociationStrength[flameParameterPanel->variationIndex*FlameVisualizer::maxNumFlameParameters + i]);
          }
          if(guiElement == flameParameterPanel->progressAmountsGui[i]) {
            guiElement->getValue((void*)&flameParameterPanel->flameVisualizer->flameParameterProgressAmount[flameParameterPanel->variationIndex*FlameVisualizer::maxNumFlameParameters + i]);
          }
        }
      }



  Panel *FlameVisualizer::addPanel(GuiElement *parentElement, const std::string &title) {
    if(panel) return panel;

    panel = new Panel(350, 590, 10, 10);
    double line = 10, lineHeight = 23;

    scaleGui = new NumberBox("Scale", fractPars.scale, NumberBox::FLOATING_POINT, -1e10, 1e10, 10, line, 10);
    locationXGui = new NumberBox("Location", fractPars.location.x, NumberBox::FLOATING_POINT, -1e10, 1e10, 10, line += lineHeight, 8);
    locationYGui = new NumberBox("", fractPars.location.y, NumberBox::FLOATING_POINT, -1e10, 1e10, 190, line, 8);
    minIterationGui = new NumberBox("Min iterations", fractPars.minIter, NumberBox::FLOATING_POINT, 0, 1e10, 10, line += lineHeight, 8);
    maxIterationGui = new NumberBox("Max iterations", fractPars.maxIter, NumberBox::FLOATING_POINT, 0, 1e10, 10, line += lineHeight, 8);
    spectrumScalingGui = new NumberBox("Spectrum scaling", spectrumScaling, NumberBox::FLOATING_POINT, -1e10, 1e10, 10, line += lineHeight, 8);
    spectrumMinimumThresholdGui = new NumberBox("Threshold level", spectrumMinimumThreshold, NumberBox::FLOATING_POINT, 0, 1e10, 10, line += lineHeight, 8);
    spectrumResolutionGui = new NumberBox("Spectrum resolution", spectrumSize, NumberBox::INTEGER, 2, 1<<18, 10, line += lineHeight, 8);
    spectrumResolutionGui->incrementMode = NumberBox::IncrementMode::Linear;
    spectrumAveragingGui = new NumberBox("Spectrum averaging", spectrumAveraging, NumberBox::INTEGER, 1, 1<<30, 10, line += lineHeight, 8);
    spectrumAveragingGui->incrementMode = NumberBox::IncrementMode::Linear;
    clearIntervalGui = new NumberBox("Clear interval", clearInterval, NumberBox::INTEGER, 0, 1<<30, 10, line += lineHeight, 8);
    clearIntervalGui->incrementMode = NumberBox::IncrementMode::Linear;
    hitCounterTrailLevelGui = new NumberBox("Trail level", numHitCounterImages, NumberBox::INTEGER, 1, maxNumHitCounterImages, 10, line += lineHeight, 8);
    hitCounterTrailLevelGui->incrementMode = NumberBox::IncrementMode::Linear;
    brightnessGui = new NumberBox("Brightness", brightness, NumberBox::FLOATING_POINT, -1e10, 1e10, 10, line += lineHeight, 8);
    showInfoGui = new CheckBox("Show info", showInfo, 10, line += lineHeight);

    useVoxelFlameGui = new CheckBox("3D flame", useVoxelFlame, 10, line += lineHeight);
    voxelGridWidthGui = new NumberBox("Width", (int)scene3D->voxelGrid.w, NumberBox::INTEGER, 1, 100000, 10, line+=lineHeight, 4);
    voxelGridHeightGui = new NumberBox("Height", (int)scene3D->voxelGrid.h, NumberBox::INTEGER, 1, 100000, 10, line+=lineHeight, 4);
    voxelGridDepthGui = new NumberBox("Depth", (int)scene3D->voxelGrid.d, NumberBox::INTEGER, 1, 100000, 10, line+=lineHeight, 4);

    panel->addChildElement(scaleGui);
    panel->addChildElement(locationXGui);
    panel->addChildElement(locationYGui);
    panel->addChildElement(minIterationGui);
    panel->addChildElement(maxIterationGui);
    panel->addChildElement(spectrumScalingGui);
    panel->addChildElement(spectrumMinimumThresholdGui);
    panel->addChildElement(spectrumResolutionGui);
    panel->addChildElement(spectrumAveragingGui);
    panel->addChildElement(clearIntervalGui);
    panel->addChildElement(hitCounterTrailLevelGui);
    panel->addChildElement(brightnessGui);
    panel->addChildElement(showInfoGui);

    panel->addChildElement(useVoxelFlameGui);
    panel->addChildElement(voxelGridWidthGui);
    panel->addChildElement(voxelGridHeightGui);
    panel->addChildElement(voxelGridDepthGui);

    panel->addGuiEventListener(new FlameVisualizerPanelListener(this));

    panel->setSize(320, line + lineHeight + 10);

    flameParameterPanels.resize(10);
    for(int i=0; i<flameParameterPanels.size(); i++) {
      flameParameterPanels[i] = new FlameParameterPanel(this, i);
      flameParameterPanels[i]->setPosition(10 + 30 * i, 10);
      flameParameterPanels[i]->setVisible(false);
      panel->addChildElement(flameParameterPanels[i]);
    }

    spectrumPanel = new GraphPanel(&spectrum, false, 7);
    spectrumPanel->setSize(320, 70);
    spectrumPanel->setPosition(0, panel->size.y + 5);
    spectrumPanel->verticalAxisLimits.set(0, 1);
    spectrumPanel->numHorizontalAxisLabels = 0;
    panel->addChildElement(spectrumPanel);

    parentElement->addChildElement(panel);

    return panel;
  }

  void FlameVisualizer::removePanel(GuiElement *parentElement) {
    if(!panel) return;
    parentElement->deleteChildElement(panel);
    panel = NULL;
    for(int i=0; i<flameParameterPanels.size(); i++) {
      flameParameterPanels[i] = NULL;
    }
    scaleGui = NULL;
    locationXGui = NULL;
    locationYGui = NULL;
    minIterationGui = NULL;
    maxIterationGui = NULL;
    spectrumScalingGui = NULL;
    spectrumMinimumThresholdGui = NULL;
    spectrumResolutionGui = NULL;
    spectrumAveragingGui = NULL;
    spectrumPanel = NULL;
    clearIntervalGui = NULL;
    hitCounterTrailLevelGui = NULL;
    brightnessGui = NULL;
    showInfoGui = NULL;

    useVoxelFlameGui = NULL;
    voxelGridWidthGui = NULL;
    voxelGridHeightGui = NULL;
    voxelGridDepthGui = NULL;
  }
  void FlameVisualizer::updatePanel() {
    if(panel) {
      scaleGui->setValue(fractPars.scale);
      locationXGui->setValue(fractPars.location.x);
      locationYGui->setValue(fractPars.location.y);
      minIterationGui->setValue(fractPars.minIter);
      maxIterationGui->setValue(fractPars.maxIter);
      clearIntervalGui->setValue(clearInterval);
      spectrumScalingGui->setValue(spectrumScaling);
      spectrumMinimumThresholdGui->setValue(spectrumMinimumThreshold);
      spectrumResolutionGui->setValue(spectrumSize);
      spectrumAveragingGui->setValue(spectrumAveraging);
      hitCounterTrailLevelGui->setValue(numHitCounterImages);
      brightnessGui->setValue(brightness);
      showInfoGui->setValue(showInfo);
      flameParameterPanels[fractPars.activeFlamePars]->update();

      useVoxelFlameGui->setValue(useVoxelFlame);
      voxelGridWidthGui->setValue((int)scene3D->voxelGrid.w);
      voxelGridHeightGui->setValue((int)scene3D->voxelGrid.h);
      voxelGridDepthGui->setValue((int)scene3D->voxelGrid.d);
    }
  }
  Panel *FlameVisualizer::getPanel() {
    return panel;
  }

    FlameVisualizer::FlameVisualizerPanelListener::FlameVisualizerPanelListener(FlameVisualizer *flameVisualizer) {
      this->flameVisualizer = flameVisualizer;
    }
    void FlameVisualizer::FlameVisualizerPanelListener::onValueChange(GuiElement *guiElement) {
      if(guiElement == flameVisualizer->scaleGui) {
        guiElement->getValue((void*)&flameVisualizer->fractPars.scale);
        flameVisualizer->fractPars.viewChanged = true;
      }
      if(guiElement == flameVisualizer->locationXGui) {
        guiElement->getValue((void*)&flameVisualizer->fractPars.location.x);
        flameVisualizer->fractPars.viewChanged = true;
      }
      if(guiElement == flameVisualizer->locationYGui) {
        guiElement->getValue((void*)&flameVisualizer->fractPars.location.y);
        flameVisualizer->fractPars.viewChanged = true;
      }
      if(guiElement == flameVisualizer->minIterationGui) {
        guiElement->getValue((void*)&flameVisualizer->fractPars.minIter);
        flameVisualizer->fractPars.viewChanged = true;
      }
      if(guiElement == flameVisualizer->maxIterationGui) {
        guiElement->getValue((void*)&flameVisualizer->fractPars.maxIter);
        flameVisualizer->fractPars.viewChanged = true;
      }
      if(guiElement == flameVisualizer->clearIntervalGui) {
        guiElement->getValue((void*)&flameVisualizer->clearInterval);
        if(flameVisualizer->clearInterval == 0) {
          flameVisualizer->fractPars.viewChanged = true;
          flameVisualizer->resetAllHitCounterImages = true;
        }
      }
      if(guiElement == flameVisualizer->spectrumScalingGui) {
        guiElement->getValue((void*)&flameVisualizer->spectrumScaling);
      }
      if(guiElement == flameVisualizer->spectrumMinimumThresholdGui) {12,

        guiElement->getValue((void*)&flameVisualizer->spectrumMinimumThreshold);
      }
      if(guiElement == flameVisualizer->spectrumResolutionGui) {
        guiElement->getValue((void*)&flameVisualizer->spectrumSize);
      }
      if(guiElement == flameVisualizer->spectrumAveragingGui) {
        guiElement->getValue((void*)&flameVisualizer->spectrumAveraging);
      }
      if(guiElement == flameVisualizer->hitCounterTrailLevelGui) {
        guiElement->getValue((void*)&flameVisualizer->numHitCounterImages);
      }
      if(guiElement == flameVisualizer->brightnessGui) {
        guiElement->getValue((void*)&flameVisualizer->brightness);
      }
      if(guiElement == flameVisualizer->showInfoGui) {
        guiElement->getValue((void*)&flameVisualizer->showInfo);
      }

      if(guiElement == flameVisualizer->useVoxelFlameGui) {
        bool value;
        guiElement->getValue((void*)&value);
        flameVisualizer->setFlame3D(value);
      }
      if(guiElement == flameVisualizer->voxelGridWidthGui) {
        guiElement->getValue((void*)&flameVisualizer->requestedVoxelGridSize.x);
      }
      if(guiElement == flameVisualizer->voxelGridHeightGui) {
        guiElement->getValue((void*)&flameVisualizer->requestedVoxelGridSize.y);
      }
      if(guiElement == flameVisualizer->voxelGridDepthGui) {
        guiElement->getValue((void*)&flameVisualizer->requestedVoxelGridSize.z);
      }

    }



  void FlameVisualizer::saveToFile(std::string filename) {
    if(filename.size() > 0) {
      filename = filePath + filename + fileExtension;
      createDirectories(filename);
      saveCerealBinary(*this, filename);
    }
  }

  void FlameVisualizer::loadFromFile(const std::string &filename) {
    if(filename.size() > 0) {
      loadCerealBinary(*this, filePath + filename + fileExtension);
    }
    if(panel) {
      updatePanel();
      for(int i=0; i<flameParameterPanels.size(); i++) {
        flameParameterPanels[i]->update();
      }
    }
  }



  template<class Archive>
  void FlameVisualizer::serialize(Archive &ar, const std::uint32_t version) {
    //printf("FlameVisualizer.serialize, version %d\n", version);
    if(version == 1) {
      ar(fractPars,
         numHitCounterImages,
         flameParameterSpectrumAssociation,
         flameParameterSpectrumAssociationStrength,
         flameParameterProgressAmount,
         flameParameterProgression,
         spectrumSize,
         spectrumAveraging,
         spectrumScalings,
         clearInterval,
         spectrumScaling,
         spectrumMinimumThreshold,
         brightness
      );
    }
  }
