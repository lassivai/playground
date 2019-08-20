#pragma once

#include "synthtitlebar.h"
#include "synthtitlebar.h"
#include "../synth/preset.h"



struct SynthTitleBarExt : public SynthTitleBar
{
  FileSaver *fileSaver = NULL;
  HierarchicalTextFileParser *fileToBeSaved = NULL;

  Button *saveGui = NULL;
  Button *loadGui = NULL;
  
  Button *openMenuGui = NULL;

  SynthTitleMenu *menu = NULL;

  virtual ~SynthTitleBarExt() {}
  
  SynthTitleBarExt(const std::string &text, Panel *parentPanel, bool isActive, int layout, std::vector<Preset> *presets) : SynthTitleBar(text, parentPanel, isActive, layout, presets) {
    addGuiEventListener(new SynthTitleBarExtListener(this));
  }
  
  SynthTitleBarExt(const std::string &text, Panel *parentPanel, bool isActive, int layout) : SynthTitleBar(text, parentPanel, isActive, layout) {
    addGuiEventListener(new SynthTitleBarExtListener(this));
  }
  SynthTitleBarExt(const std::string &text, Panel *parentPanel, bool isActive) : SynthTitleBar(text, parentPanel, isActive) {
    addGuiEventListener(new SynthTitleBarExtListener(this));
  }
  SynthTitleBarExt(const std::string &text, Panel *parentPanel) : SynthTitleBar(text, parentPanel) {
    addGuiEventListener(new SynthTitleBarExtListener(this));
  }
  
  /*void init() {
    addGuiEventListener(new SynthTitleBarExtListener(this));
  }*/
  
  virtual void onUpdate(double time, double dt) {
    if(fileSaver && fileSaver->closeRequested) {
      deleteChildElement(fileSaver);
      fileSaver = NULL;
      if(saveGui) {
        saveGui->setValue(false);
      }
    }
    if(menu && menu->closeRequested) {
      /*deleteChildElement(menu);
      menu = NULL;*/
      menu->setVisible(false);
      if(openMenuGui) {
        openMenuGui->setValue(false);
      }
    }
  }

  void addSaveButton(HierarchicalTextFileParser *fileToBeSaved) {
    this->fileToBeSaved = fileToBeSaved;
    addChildElement(saveGui = new Button("Save", "data/synth/textures/save.png", leftKnobPlacer.alignLeftMiddle(), Button::ToggleButton, false));
    saveGui->pressedOverlayColor.set(0.6, 0.6, 0.6, 1.0);
  }
  
  void addLoadButton() {
    addChildElement(loadGui = new Button("Load", "data/synth/textures/load.png", leftKnobPlacer.alignLeftMiddle(), Button::ToggleButton, false));
    loadGui->pressedOverlayColor.set(0.6, 0.6, 0.6, 1.0);
  }
  
  void addMenu() {
    addChildElement(openMenuGui = new Button("Menu", "data/synth/textures/menu.png", leftKnobPlacer.alignLeftMiddle(), Button::ToggleButton, false));
    //openMenuGui->pressedOverlayColor.set(0.8, 0.8, 0.8, 1.0);
    addChildElement(menu = new SynthTitleMenu("Synth title menu"));
    // menu->addHelpButton();
    /*menu->addLayoutButton();
    menu->addMenuDivider();
    menu->addLoadButton();
    menu->addSaveButton();*/
  }

  struct SynthTitleBarExtListener : public GuiEventListener {
    SynthTitleBarExt *synthTitleBarExt = NULL;
    SynthTitleBarExtListener(SynthTitleBarExt *synthTitleBarExt) : synthTitleBarExt(synthTitleBarExt) {}
    virtual void onAction(GuiElement *guiElement) {
      if(guiElement == synthTitleBarExt->openMenuGui) {
        if(synthTitleBarExt->menu->isVisible) {
          synthTitleBarExt->menu->setVisible(false);
        }
        else {
          synthTitleBarExt->menu->setVisible(true);
        }
      }
      if(guiElement == synthTitleBarExt->saveGui) {
        if(!synthTitleBarExt->fileSaver) {
          synthTitleBarExt->addChildElement(synthTitleBarExt->fileSaver = new FileSaver(synthTitleBarExt->fileToBeSaved));
          synthTitleBarExt->fileSaver->setPosition(-synthTitleBarExt->fileSaver->size.x - 5, 0);
        }
        else {
          synthTitleBarExt->deleteChildElement(synthTitleBarExt->fileSaver);
          synthTitleBarExt->fileSaver = NULL;
        }
      }
    }
  };


};
