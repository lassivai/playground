#pragma once
#include "../synth.h"
#include "../../gui/gui.h"

struct EffectTrackPanel : public Panel {
  /*struct EffectPreviewPanel : public Panel {
    PostProcessingEffect *postProcessingEffect;
    
    EffectPreviewPanel(PostProcessingEffect *postProcessingEffect) : Panel("Voice preview panel") {
      drawBorder = false;
      drawBackground = false;
      draggable = false;
      this->postProcessingEffect = postProcessingEffect;
    }
          
    virtual void onRender(GeomRenderer &geomRenderer, TextGl &textRenderer) {
      if(!isVisible) return;
      
    }
  };*/
  struct EffectTrackTitleBar : public SynthTitleBarExt {
    EffectTrackPanel *effectTrackPanel = NULL;
    SynthTitleMenu *menu = NULL;
    MenuButton *addBiquadMenuButton = NULL;
    MenuButton *addEqualizerMenuButton = NULL;
    MenuButton *addReverbMenuButton = NULL;
    MenuButton *addMultidelayMenuButton = NULL;
    
    EffectTrackTitleBar(const std::string &title, EffectTrackPanel *effectTrackPanel) : SynthTitleBarExt(title, effectTrackPanel) {
      init(effectTrackPanel);
    }
    
    void init(EffectTrackPanel *effectTrackPanel) {
      this->effectTrackPanel = effectTrackPanel;
      menu = addMenu();
      
      addBiquadMenuButton = menu->addMenuButton("Add Biquad Filter");
      addEqualizerMenuButton = menu->addMenuButton("Add Equalizer");
      addReverbMenuButton = menu->addMenuButton("Add Reverb");
      addMultidelayMenuButton = menu->addMenuButton("Add Multidelay");
      
      addGuiEventListener(new EffectTrackTitleBarListeneter(this));
    }
    struct EffectTrackTitleBarListeneter : public GuiEventListener {
      EffectTrackTitleBar *effectTrackTitleBar;
      EffectTrackTitleBarListeneter(EffectTrackTitleBar *effectTrackTitleBar) : effectTrackTitleBar(effectTrackTitleBar) {}
      virtual void onValueChange(GuiElement *guiElement) override {
        if(guiElement == effectTrackTitleBar->addBiquadMenuButton) {
          effectTrackTitleBar->effectTrackPanel->postProcessingEffects->push_back(new Biquad(effectTrackTitleBar->effectTrackPanel->delayLine.sampleRate));
        }
        if(guiElement == effectTrackTitleBar->addBiquadMenuButton) {
          effectTrackTitleBar->effectTrackPanel->postProcessingEffects->push_back((new Equalizer(effectTrackTitleBar->effectTrackPanel->delayLine.sampleRate)));
        }
        if(guiElement == effectTrackTitleBar->addBiquadMenuButton) {
          effectTrackTitleBar->effectTrackPanel->postProcessingEffects->push_back(new Reverb(effectTrackTitleBar->effectTrackPanel->delayLine.sampleRate));
        }
        if(guiElement == effectTrackTitleBar->addBiquadMenuButton) {
          effectTrackTitleBar->effectTrackPanel->postProcessingEffects->push_back(new Multidelay(effectTrackTitleBar->effectTrackPanel->delayLine));
        }
      }
    };
  };


  EffectTrackTitleBar *titleBar = NULL;
  
  std::vector<PostProcessingEffect*> *postProcessingEffects = NULL;
  
  GuiElement *parentGuiElement = NULL;
  
  DelayLine *delayLine;
  
  EffectTrackPanel(std::vector<PostProcessingEffect*> *postProcessingEffects, DelayLine *delayLine, GuiElement *parentGuiElement) : Panel("Effect track panel") {
    init(postProcessingEffects, delayLine, parentGuiElement);
  }
  
  void init(std::vector<PostProcessingEffect*> *postProcessingEffects, DelayLine *delayLine, GuiElement *parentGuiElement) {
    this->parentGuiElement = parentGuiElement;
    this->delayLine = delayLine;
    this->postProcessingEffects = postProcessingEffects;

    double line = 0, lineHeight = 23;

    setSize(270, 50);

    this->addGuiEventListener(new EffectTrackPanelListener(this));
    parentGuiElement->addChildElement(this);
    
    titleBar = new EffectTrackTitleBar("Effects", this);
    //line += titleBar->size.y;
    
    /*for(int i=0; i<postProcessingEffects->size(); i++) {
      Panel *panel = new Panel(trackItemSize.x, trackItemSize.y, 0, trackItemStartY+trackItemSize.y*i);
      postProcessingEffects->at(i)->initializeEffectTrackPanel(panel);
      postProcessingEffectPanels.push_back(panel);
    }*/
    //update();
    update();
  }
  
  void onItemsChanged() {
    //update
  }

  void update() {
    double line = titleBar->size.y+5;
    
    for(int i=0; i<postProcessingEffects.size(); i++) {
      if(!hasChildElement(postProcessingEffects[i]->getPreviewPanel())) {
        addChildElement(postProcessingEffects[i]->getPreviewPanel());
      }
      postProcessingEffects[i]->getPreviewPanel()->setPosition(0, line);
      line += postProcessingEffects[i]->getPreviewPanel()->size.y;
    }
    setSize(270, line);
  }

  struct EffectTrackPanelListener : public GuiEventListener {
    EffectTrackPanel *effectTrackPanel;
    EffectTrackPanelListener(EffectTrackPanel *effectTrackPanel) {
      this->effectTrackPanel = effectTrackPanel;
    }
    /*virtual void onAction(GuiElement *guiElement) {
      if(guiElement == effectTrackPanel->openMenuGui) {
        if(effectTrackPanel->menu->isVisible) {
          effectTrackPanel->menu->setVisible(false);
        }
        else {
          effectTrackPanel->menu->setVisible(true);
        }
      }
    }*/
    void onMousePressed(GuiElement *guiElement, Events &events) {
      /*if(guiElement == effectTrackPanel->effectsGui && guiElement->isPointWithin(events.m)) {
        int index = effectTrackPanel->effects->activeItemIndex;
        if(index == 0) {
          effectTrackPanel->postProcessingEffects->push_back(new Biquad(effectTrackPanel->delayLine.sampleRate));
        }
        else if(index == 1) {
          effectTrackPanel->postProcessingEffects->push_back(new Equalizer(effectTrackPanel->sampleRate));
        }
        else if(index == 2) {
          effectTrackPanel->postProcessingEffects->push_back(new Multidelay(effectTrackPanel->delayLine));
        }
        else if(index == 3) {
          effectTrackPanel->postProcessingEffects->push_back(new Reverb(effectTrackPanel->delayLine.sampleRate));
        }
        Panel *panel = new Panel(effectTrackPanel->trackItemSize.x, effectTrackPanel->trackItemSize.y, 0, effectTrackPanel->trackItemStartY+effectTrackPanel->trackItemSize.y*i);
        postProcessingEffects->at(i)->initializeEffectTrackPanel(panel);
        postProcessingEffectPanels.push_back(panel);

      }*/
    }
    
  };
  
};