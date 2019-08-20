#pragma once
#include "engine/sketch.h"

struct VolraySketch : public Sketch
{
  Scene3D *scene3D = nullptr;


  FastNoise fastNoise, fastNoiseCellularLookup;
  std::vector<FastNoise> fastNoises = std::vector<FastNoise>(4);

  bool showBorders = true;

  bool updateGrid = true;

  const std::vector<std::string> voxelSetupNames = {"Empty", "FastNoise gray", "FastNoise RGB", "FastNoise RGBA"};
  int voxelSetup = 0;

  int requestedVoxelGridWidth, requestedVoxelGridHeight, requestedVoxelGridDepth;

  struct VoxelGridPanel : public Panel {
    NumberBox *voxelGridWidthGui = NULL;
    NumberBox *voxelGridHeightGui = NULL;
    NumberBox *voxelGridDepthGui = NULL;
    ListBox *voxelSetupGui = NULL;
    CheckBox *showBordersGui = NULL;
    CheckBox *updateGridGui = NULL;
    VolraySketch *volraySketch = NULL;
    FastNoisePanel *fastNoisePanel;
    std::vector<FastNoisePanel*> fastNoisePanels;

    VoxelGridPanel() : Panel("Voxel grid panel", 0, 0, 1, 1)  {}
    VoxelGridPanel(VolraySketch *volraySketch) : Panel("Voxel grid panel", 0, 0, 1, 1)   {
      init(volraySketch);
    }

    void init(VolraySketch *volraySketch) {
      this->volraySketch = volraySketch;

      double line = 10, lineHeight = 23;
      voxelSetupGui = new ListBox("Voxels", 10, line);
      voxelSetupGui->setItems(volraySketch->voxelSetupNames);
      voxelSetupGui->setValue(volraySketch->voxelSetup);
      this->addChildElement(voxelSetupGui);

      voxelGridWidthGui = new NumberBox("Width", (int)volraySketch->scene3D->voxelGrid.w, NumberBox::INTEGER, 1, 100000, 10, line+=lineHeight);
      this->addChildElement(voxelGridWidthGui);

      voxelGridHeightGui = new NumberBox("Height", (int)volraySketch->scene3D->voxelGrid.h, NumberBox::INTEGER, 1, 100000, 10, line+=lineHeight);
      this->addChildElement(voxelGridHeightGui);

      voxelGridDepthGui = new NumberBox("Depth", (int)volraySketch->scene3D->voxelGrid.d, NumberBox::INTEGER, 1, 100000, 10, line+=lineHeight);
      this->addChildElement(voxelGridDepthGui);

      showBordersGui = new CheckBox("Borders", volraySketch->showBorders, 10, line+=lineHeight);
      this->addChildElement(showBordersGui);

      updateGridGui = new CheckBox("Auto-update", volraySketch->updateGrid, 10, line+=lineHeight);
      this->addChildElement(updateGridGui);

      this->setSize(320, line + lineHeight + 10);

      this->addGuiEventListener(new VoxelGridPanelListener(this));

      fastNoisePanel = new FastNoisePanel(this, &volraySketch->fastNoise, "Monochrome");
      fastNoisePanel->setPosition(0, this->size.y + 5);
      fastNoisePanel->setVisible(volraySketch->voxelSetup == 1);
      fastNoisePanel->addGuiEventListener(new FastNoisePanelListener(this));

      fastNoisePanels.resize(4);
      fastNoisePanels[0] = new FastNoisePanel(this, &volraySketch->fastNoises[0], "Red channel");
      fastNoisePanels[1] = new FastNoisePanel(this, &volraySketch->fastNoises[1], "Green channel");
      fastNoisePanels[2] = new FastNoisePanel(this, &volraySketch->fastNoises[2], "Blue channel");
      fastNoisePanels[3] = new FastNoisePanel(this, &volraySketch->fastNoises[3], "Alpha channel");

      for(int i=0; i<4; i++) {
        fastNoisePanels[i]->setPosition(i * 30, this->size.y + 5 + i * 30);
        fastNoisePanels[i]->addGuiEventListener(new FastNoisePanelListener(this));
      }
      for(int i=0; i<3; i++) {
        fastNoisePanels[i]->setVisible(volraySketch->voxelSetup == 2 || volraySketch->voxelSetup == 3);
      }
      fastNoisePanels[3]->setVisible(volraySketch->voxelSetup == 3);

      volraySketch->guiRoot.addChildElement(this);
    }

    struct VoxelGridPanelListener : public GuiEventListener {
      VoxelGridPanel *voxelGridPanel;
      VoxelGridPanelListener(VoxelGridPanel *voxelGridPanel) {
        this->voxelGridPanel = voxelGridPanel;
      }
      void onValueChange(GuiElement *guiElement) {
        if(guiElement == voxelGridPanel->voxelSetupGui) {
          guiElement->getValue((void*)&voxelGridPanel->volraySketch->voxelSetup);
          voxelGridPanel->fastNoisePanel->setVisible(voxelGridPanel->volraySketch->voxelSetup == 1);
          for(int i=0; i<3; i++) {
            voxelGridPanel->fastNoisePanels[i]->setVisible(voxelGridPanel->volraySketch->voxelSetup == 2 || voxelGridPanel->volraySketch->voxelSetup == 3);
          }
          voxelGridPanel->fastNoisePanels[3]->setVisible(voxelGridPanel->volraySketch->voxelSetup == 3);

          voxelGridPanel->volraySketch->updateVoxelGrid();
        }
        if(guiElement == voxelGridPanel->showBordersGui) {
          guiElement->getValue((void*)&voxelGridPanel->volraySketch->showBorders);
          voxelGridPanel->volraySketch->updateVoxelGrid();
        }
        if(guiElement == voxelGridPanel->updateGridGui) {
          guiElement->getValue((void*)&voxelGridPanel->volraySketch->updateGrid);
          if(voxelGridPanel->volraySketch->updateGrid) {
            voxelGridPanel->volraySketch->updateVoxelGrid();
          }
        }
        if(guiElement == voxelGridPanel->voxelGridWidthGui) {
          guiElement->getValue((void*)&voxelGridPanel->volraySketch->requestedVoxelGridWidth);
          voxelGridPanel->volraySketch->updateVoxelGrid();
        }
        if(guiElement == voxelGridPanel->voxelGridHeightGui) {
          guiElement->getValue((void*)&voxelGridPanel->volraySketch->requestedVoxelGridHeight);
          voxelGridPanel->volraySketch->updateVoxelGrid();
        }
        if(guiElement == voxelGridPanel->voxelGridDepthGui) {
          guiElement->getValue((void*)&voxelGridPanel->volraySketch->requestedVoxelGridDepth);
          voxelGridPanel->volraySketch->updateVoxelGrid();
        }
      }
    };
    struct FastNoisePanelListener : public GuiEventListener {
      VoxelGridPanel *voxelGridPanel;
      FastNoisePanelListener(VoxelGridPanel *voxelGridPanel) {
        this->voxelGridPanel = voxelGridPanel;
      }
      void onValueChange(GuiElement *guiElement) {
        voxelGridPanel->volraySketch->updateVoxelGrid();
      }
    };
  };


  VoxelGridPanel *voxelGridPanel = NULL;


  void onInit() {
    scene3D = new Scene3D(screenW, screenH);

    fastNoiseCellularLookup.SetSeed(785745);
    fastNoiseCellularLookup.SetNoiseType(FastNoise::Simplex);
    fastNoiseCellularLookup.SetFrequency(1.0);
    fastNoise.SetCellularNoiseLookup(&fastNoiseCellularLookup);
    for(int i=0; i<fastNoises.size(); i++)
      fastNoises[i].SetCellularNoiseLookup(&fastNoiseCellularLookup);

    requestedVoxelGridWidth = 100;
    requestedVoxelGridHeight = 100;
    requestedVoxelGridDepth = 100;

    updateVoxelGrid();

    /*for(int i=0; i<scene3D->rayTracedSceneCurrent.w; i++) {
      for(int j=0; j<scene3D->rayTracedSceneCurrent.h; j++) {
        scene3D->rayTracedSceneCurrent.setPixel(i, j, Random::getDouble(), Random::getDouble(), Random::getDouble(), 1.0);
      }
    }
    scene3D->rayTracedSceneCurrent.applyPixels();*/

    voxelGridPanel = new VoxelGridPanel(this);
    voxelGridPanel->setPosition(10, 10);
    voxelGridPanel->setVisible(false);
  }


  void updateVoxelGrid() {
    if(!updateGrid) return;

    if((int)scene3D->voxelGrid.w != requestedVoxelGridWidth || (int)scene3D->voxelGrid.h != requestedVoxelGridHeight || (int)scene3D->voxelGrid.d != requestedVoxelGridDepth) {
      scene3D->voxelGrid.setSize(requestedVoxelGridWidth, requestedVoxelGridHeight, requestedVoxelGridDepth);
    }


    Vec4d color;
    for(int i=0; i<scene3D->voxelGrid.voxels.size(); i++) {
      if(showBorders && scene3D->voxelGrid.voxelCoordinates[i].w >= 2) {
        color.set(0.5, 0.5, 0.5, 0.5);
      }
      else if(voxelSetup == 1) {
        color.x = color.y = color.z = fastNoise.GetNoise(scene3D->voxelGrid.voxelCoordinates[i].x,
                                                         scene3D->voxelGrid.voxelCoordinates[i].y,
                                                         scene3D->voxelGrid.voxelCoordinates[i].z) * 0.5 + 0.5;
        color.w = rgbToAlpha(color.x, color.y, color.z);
      }
      else if(voxelSetup == 2) {
        color.x = fastNoises[0].GetNoise(scene3D->voxelGrid.voxelCoordinates[i].x+times.x,
                                         scene3D->voxelGrid.voxelCoordinates[i].y+times.y,
                                         scene3D->voxelGrid.voxelCoordinates[i].z+times.z) * 0.5 + 0.5;
        color.y = fastNoises[1].GetNoise(scene3D->voxelGrid.voxelCoordinates[i].x+times.x,
                                         scene3D->voxelGrid.voxelCoordinates[i].y+times.y,
                                         scene3D->voxelGrid.voxelCoordinates[i].z+times.z) * 0.5 + 0.5;
        color.z = fastNoises[2].GetNoise(scene3D->voxelGrid.voxelCoordinates[i].x+times.x,
                                         scene3D->voxelGrid.voxelCoordinates[i].y+times.y,
                                         scene3D->voxelGrid.voxelCoordinates[i].z+times.z) * 0.5 + 0.5;
        color.w = rgbToAlpha(color.x, color.y, color.z);
      }
      else if(voxelSetup == 3) {
        color.x = fastNoises[0].GetNoise(scene3D->voxelGrid.voxelCoordinates[i].x,
                                         scene3D->voxelGrid.voxelCoordinates[i].y,
                                         scene3D->voxelGrid.voxelCoordinates[i].z) * 0.5 + 0.5;
        color.y = fastNoises[1].GetNoise(scene3D->voxelGrid.voxelCoordinates[i].x,
                                         scene3D->voxelGrid.voxelCoordinates[i].y,
                                         scene3D->voxelGrid.voxelCoordinates[i].z) * 0.5 + 0.5;
        color.z = fastNoises[2].GetNoise(scene3D->voxelGrid.voxelCoordinates[i].x,
                                         scene3D->voxelGrid.voxelCoordinates[i].y,
                                         scene3D->voxelGrid.voxelCoordinates[i].z) * 0.5 + 0.5;
        color.w = fastNoises[3].GetNoise(scene3D->voxelGrid.voxelCoordinates[i].x,
                                         scene3D->voxelGrid.voxelCoordinates[i].y,
                                         scene3D->voxelGrid.voxelCoordinates[i].z) * 0.5 + 0.5;
      }
      scene3D->voxelGrid.voxels[i] = color;
    }
  }

    /*Vec3d pos;
    for(int i=0; i<scene3D->voxelGrid.w; i++) {
      pos.x = map(i, 0, scene3D->voxelGrid.w-1, xRange.x, xRange.y);
      int xBorder = i == 0 || i == scene3D->voxelGrid.w-1 ? 1 : 0;
      for(int j=0; j<scene3D->voxelGrid.h; j++) {
        pos.y = map(j, 0, scene3D->voxelGrid.h-1, yRange.x, yRange.y);
        int yBorder = j == 0 || j == scene3D->voxelGrid.h-1 ? 1 : 0;
        for(int k=0; k<scene3D->voxelGrid.d; k++) {
          pos.z = map(k, 0, scene3D->voxelGrid.d-1, zRange.x, zRange.y);
          int zBorder = k == 0 || k == scene3D->voxelGrid.d-1 ? 1 : 0;
          bool isBorder = xBorder + yBorder + zBorder >= 2;
          double d = pos.length();
          Vec4d color;
          if(showBorders && isBorder) {
            color.set(0.5, 0.5, 0.5, 0.5);
          }
          else if(voxelSetup == 1) {
            color.x = color.y = color.z = fastNoise.GetNoise(pos.x, pos.y, pos.z)*0.5 + 0.5;
          }
          else if(voxelSetup == 2) {
            color.x = fastNoises[0].GetNoise(pos.x, pos.y, pos.z)*0.5 + 0.5;
            color.y = fastNoises[1].GetNoise(pos.x, pos.y, pos.z)*0.5 + 0.5;
            color.z = fastNoises[2].GetNoise(pos.x, pos.y, pos.z)*0.5 + 0.5;
          }
          scene3D->voxelGrid.setVoxel(i, j, k, color);
        }
      }
    }
  }*/

  void onQuit() {
    delete scene3D;
  }

  void onKeyUp() {
    scene3D->onKeyUp(events);
  }

  void onKeyDown() {
    scene3D->onKeyDown(events);

    if(events.sdlKeyCode == SDLK_HOME) {
      voxelGridPanel->toggleVisibility();
    }
    if(events.sdlKeyCode == SDLK_l) {
      updateThreadActive = !updateThreadActive;
    }
  }

  void onMouseWheel() {

  }

  void onMousePressed() {
  }

  void onMouseReleased() {
  }

  void onMouseMotion() {
    scene3D->onMouseMotion(events);
  }

  void onReloadShaders() {

  }

  void onUpdate() {
    for(int i=0; i<commandQueue.commands.size(); i++) {
      Command *cmd = NULL;
    }

    if(updateThreads.size() == 0 && updateThreadActive) {
      updateTime = time;
      updateThreads.push_back(std::thread(updateThread, this));
    }
    if(!updateThreadActive) {
      for(int i=0; i<updateThreads.size(); i++) {
        updateThreads[i].join();
      }
      updateThreads.clear();
    }
  }

  std::vector<std::thread> updateThreads;
  Vec3d times;
  bool updateThreadActive = false;
  double updateTime;

  static void updateThread(VolraySketch *volraySketch) {

    while(volraySketch->updateThreadActive) {
      double dt = volraySketch->time - volraySketch->updateTime;
      volraySketch->updateTime = volraySketch->time;
      volraySketch->times.x += dt * 0.01;
      volraySketch->times.y += dt * 0.03086;
      volraySketch->times.z += dt * 0.08057;
      volraySketch->updateVoxelGrid();
    }
  }


  void onDraw() {
    clear(0, 0, 0, 1);

    if(updateThreadActive) {
      scene3D->camRotY += PI*2.0 * dt / 80.0;
    }

    scene3D->updateView();
    scene3D->update(time);
    scene3D->draw(0, 0, screenW, screenH);
    scene3D->printInfo(textRenderer, 0, 0, screenW, screenH, 1, true);

    if(console.inputGrabbed) {
      console.render(sdlInterface);
    }
  }

};
