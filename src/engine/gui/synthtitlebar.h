#pragma once

#include "titlebar.h"
#include "../synth/preset.h"



struct SynthTitleBar : public TitleBar
{
  struct PresetInactivationGuiEventListener : public GuiEventListener {
    SynthTitleBar *synthTitleBar = NULL;
    PresetInactivationGuiEventListener(SynthTitleBar *synthTitleBar) : synthTitleBar(synthTitleBar) {}
    virtual void onValueChange(GuiElement *guiElement) {
      synthTitleBar->setPresetActive(false);
    }
  };
  struct PresetActivationGuiEventListener : public GuiEventListener {
    SynthTitleBar *synthTitleBar = NULL;
    PresetActivationGuiEventListener(SynthTitleBar *synthTitleBar) : synthTitleBar(synthTitleBar) {}
    virtual void onValueChange(GuiElement *guiElement) {
      synthTitleBar->setPresetActive(true);
    }
  };
  
  //PresetInactivationGuiEventListener *presetInactivationGuiEventListener = NULL;
  //PresetActivationGuiEventListener *presetActivationGuiEventListener = NULL;
  
  Panel *parentPanel = NULL;
    
  RotaryKnob<long> *presetsKnob = NULL;    
  Button *isActiveGui = NULL;
  Button *layoutGui = NULL;
  Button *closeGui = NULL;
  
  RowColumnPlacer leftKnobPlacer;
  RowColumnPlacer rightKnobPlacer;

  bool hideOnClose = false;


  virtual ~SynthTitleBar() {} 
  
  SynthTitleBar(const std::string &text, Panel *parentPanel, bool isActive, int layout, std::vector<Preset> *presets) : TitleBar(text, parentPanel->size.x, 31, 0, 0) {
    init(text, parentPanel);
    addPowerButton(isActive);
    addLayoutButton(layout);
    addPresetsKnob(presets);
  }
  
  SynthTitleBar(const std::string &text, Panel *parentPanel, bool isActive, int layout) : TitleBar(text, parentPanel->size.x, 31, 0, 0) {
    init(text, parentPanel);
    addPowerButton(isActive);
    addLayoutButton(layout);
  }
  SynthTitleBar(const std::string &text, Panel *parentPanel, bool isActive) : TitleBar(text, parentPanel->size.x, 31, 0, 0) {
    init(text, parentPanel);
    addPowerButton(isActive);
  }
  SynthTitleBar(const std::string &text, Panel *parentPanel) : TitleBar(text, parentPanel->size.x, 31, 0, 0) {
    init(text, parentPanel);
  }
  
  void init(const std::string &text, Panel *parentPanel) {
    this->parentPanel = parentPanel;
    parentPanel->addChildElement(this);
    leftKnobPlacer.init(size.x, size.y, 5, 5, 0);
    //leftKnobPlacer.knobSize = 27;
    
    rightKnobPlacer.init(size.x, size.y, 5, 5, 0, true);
    //rightKnobPlacer.knobSize = 27;
    //presetInactivationGuiEventListener = new PresetInactivationGuiEventListener(this);
    //presetActivationGuiEventListener = new PresetActivationGuiEventListener(this);
    addGuiEventListener(new SynthTitleBarListener(this));
  }
  


  
  void addPowerButton(bool isActive) {
    addChildElement(isActiveGui = new Button("Power", "data/synth/textures/power.png", rightKnobPlacer.alignRightMiddle(), Button::ToggleButton, isActive));
    //isActiveGui->setPosition(isActiveGui->pos.x - isActiveGui->size.x, isActiveGui->pos.y);
    //rightKnobPlacer.progessX(isActiveGui->size.x + 3);
    getGlowLevel = [this]() { return isActiveGui->getActivationLevel(); };
  }
  
  void addLayoutButton(int layout) {
    addChildElement(layoutGui = new Button("Layout", "data/synth/textures/layout.png", rightKnobPlacer.alignRightMiddle(), Button::PressButton));
    //layoutGui->setPosition(layoutGui->pos.x - layoutGui->size.x, layoutGui->pos.y);
    //rightKnobPlacer.progessX(layoutGui->size.x + 3);
    layoutGui->pressedOverlayColor.set(0.6, 0.6, 0.6, 1.0);
  }
  
  void addCloseButton(bool hideOnClose = false) {
    this->hideOnClose = hideOnClose;
    
    addChildElement(closeGui = new Button("Layout", "data/synth/textures/close.png", rightKnobPlacer.alignRightMiddle(), Button::PressButton));
    //closeGui->setPosition(closeGui->pos.x - closeGui->size.x, closeGui->pos.y);
    //rightKnobPlacer.progessX(closeGui->size.x + 3);
    closeGui->pressedOverlayColor.set(0.6, 0.6, 0.6, 1.0);
  }
  

  
  void addPresetsKnob(std::vector<Preset> *presets) {
    if(presets && presets->size() > 0) {
      addChildElement(presetsKnob = new RotaryKnob<long>("Preset", leftKnobPlacer.alignLeftMiddle(), RotaryKnob<long>::ListKnob, 0, 1, 0));
      
      for(int i=0; i<presets->size(); i++) {
        presetsKnob->addItems(presets->at(i).name);
      }
      presetsKnob->setActiveItem(0);
      presetsKnob->addGuiEventListener(new PresetActivationGuiEventListener(this));
      setPresetActive(false);
    }
  }
  
  virtual void onSetSize() {
    rightKnobPlacer.reset();
    if(isActiveGui) {
      rightKnobPlacer.alignRightMiddle(isActiveGui);
    }
    if(layoutGui) {
      rightKnobPlacer.alignRightMiddle(layoutGui);
    }
  }
  
  void addPresetControlledGuiElements(GuiElement *guiElement) {
    guiElement->addGuiEventListener(new PresetInactivationGuiEventListener(this));
  }
  void addPresetControlledGuiElements(const std::vector<GuiElement *> guiElements) {
    for(int i=0; i<guiElements.size(); i++) {
      guiElements[i]->addGuiEventListener(new PresetInactivationGuiEventListener(this));
    }
  }
  
  virtual void onPrepare(GeomRenderer &geomRenderer, TextGl &textRenderer) {
    TitleBar::onPrepare(geomRenderer, textRenderer);
    if(size.x != parentPanel->size.x) {
      setSize(parentPanel->size.x, size.y);
    }
  }
  
  void setPresetActive(bool value) {
    if(!presetsKnob) return;
    if(value) {
      presetsKnob->defaultOverlayColor.set(1, 1, 1, 0.8);
    }
    else {
      presetsKnob->defaultOverlayColor.set(1, 1, 1, 0.2);
    }
  }
  
  struct SynthTitleBarListener : public GuiEventListener {
    SynthTitleBar *synthTitleBar;
    SynthTitleBarListener(SynthTitleBar *synthTitleBar) {
      this->synthTitleBar = synthTitleBar;
    }
    virtual void onAction(GuiElement *guiElement) {
      if(guiElement == synthTitleBar->closeGui) {
        if(synthTitleBar->hideOnClose) {
          synthTitleBar->parentPanel->setVisible(false);
        }
      }
    }
  };
    

};
