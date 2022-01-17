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
    //SynthTitleMenu *menu = NULL;
    MenuButton *addBiquadMenuButton = NULL;
    MenuButton *addEqualizerMenuButton = NULL;
    MenuButton *addReverbMenuButton = NULL;
    MenuButton *addMultidelayMenuButton = NULL;

    EffectTrackTitleBar(const std::string &title, EffectTrackPanel *effectTrackPanel) : SynthTitleBarExt(title, effectTrackPanel) {
      init(effectTrackPanel);
    }

    void init(EffectTrackPanel *effectTrackPanel) {
      this->effectTrackPanel = effectTrackPanel;
      addMenu();

      addBiquadMenuButton = menu->addMenuButton("Add Biquad Filter");
      addEqualizerMenuButton = menu->addMenuButton("Add Equalizer");
      addReverbMenuButton = menu->addMenuButton("Add Reverb");
      addMultidelayMenuButton = menu->addMenuButton("Add Multidelay");

      menu->addGuiEventListener(new EffectTrackTitleBarListeneter(this));
    }
    struct EffectTrackTitleBarListeneter : public GuiEventListener {
      EffectTrackTitleBar *effectTrackTitleBar;
      EffectTrackTitleBarListeneter(EffectTrackTitleBar *effectTrackTitleBar) : effectTrackTitleBar(effectTrackTitleBar) {}
      
      virtual void onAction(GuiElement *guiElement) override {
        if(guiElement == effectTrackTitleBar->addBiquadMenuButton) {
          effectTrackTitleBar->effectTrackPanel->postProcessingEffects->push_back(new BiquadFilter(effectTrackTitleBar->effectTrackPanel->delayLine));
          effectTrackTitleBar->effectTrackPanel->onItemsChanged();
        }
        if(guiElement == effectTrackTitleBar->addEqualizerMenuButton) {
          effectTrackTitleBar->effectTrackPanel->postProcessingEffects->push_back((new Equalizer(effectTrackTitleBar->effectTrackPanel->delayLine->sampleRate)));
          effectTrackTitleBar->effectTrackPanel->onItemsChanged();
        }
        if(guiElement == effectTrackTitleBar->addReverbMenuButton) {
          effectTrackTitleBar->effectTrackPanel->postProcessingEffects->push_back(new Reverb(effectTrackTitleBar->effectTrackPanel->delayLine->sampleRate));;
          effectTrackTitleBar->effectTrackPanel->onItemsChanged();
        }
        if(guiElement == effectTrackTitleBar->addMultidelayMenuButton) {
          effectTrackTitleBar->effectTrackPanel->postProcessingEffects->push_back(new MultiDelay(effectTrackTitleBar->effectTrackPanel->delayLine));
          effectTrackTitleBar->effectTrackPanel->onItemsChanged();
        }
      }
    };
  };

  struct EffectPanel : public Panel {
    std::string effectName;
    PostProcessingEffect *postProcessingEffect = NULL;
    
    Label *labelEffectName = NULL;
    Button *showButton = NULL;
    Button *activateButton = NULL;
    Button *removeButton = NULL;
    
    EffectPanel(PostProcessingEffect *postProcessingEffect, std::string effectName) : Panel("Effect panel") {
      this->postProcessingEffect = postProcessingEffect;
      this->effectName = effectName;
      
      addChildElement(labelEffectName = new Label(effectName, 2, 2));
      
      addChildElement(showButton = new Button("Open GUI", "data/synth/textures/gui.png", 180-50, 2, Button::ToggleButton));
      
      //addChildElement(activateButton = new Button("Power", "data/synth/textures/power.png", 180-50, 2, Button::ToggleButton));
      
      addChildElement(removeButton = new Button("Remove", "data/synth/textures/close.png", 180-25, 2, Button::PressButton));
      
      setSize(180, 20);
    }
    
    /*struct EffectPanelListener {
      virtual void onAction(GuiElement *guiElement) override {
        if(guiElement == removeButton)
      }  
    }*/
    
  };
  
  
  
  std::vector<EffectPanel*> effectPanels;

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


    this->addGuiEventListener(new EffectTrackPanelListener(this));
    parentGuiElement->addChildElement(this);

    titleBar = new EffectTrackTitleBar("Effects", this);
    //line += titleBar->size.y;

    /*for(int i=0; i<postProcessingEffects->size(); i++) {
      Panel *panel = new Panel(trackItemSize.x, trackItemSize.y, 0, trackItemStartY+trackItemSize.y*i);
      postProcessingEffects->at(i)->initializeEffectTrackPanel(panel);
      postProcessingEffectPanels.push_back(panel);
    }*/

    onItemsChanged();
  }

  int moveableEffect = -1;
  std::vector<double> effectPanelLocations, effectPanelLocationsRel;
  int selectedEffectPanelIndex = -1;

  void onItemsChanged() {
    effectPanelLocations.clear();
    effectPanelLocationsRel.clear();
    
    for(int i=0; i<effectPanels.size(); i++) {
      this->deleteChildElement(effectPanels[i]);
    }
    effectPanels.clear();
    
    double line = titleBar->size.y+5;
    
    for(int i=0; i<postProcessingEffects->size(); i++) {
      EffectPanel *effectPanel = NULL;
      if(dynamic_cast<MultiDelay*>(postProcessingEffects->at(i))) {
        addChildElement(effectPanel = new EffectPanel(postProcessingEffects->at(i), "Multi delay"));
      }
      if(dynamic_cast<BiquadFilter*>(postProcessingEffects->at(i))) {
        addChildElement(effectPanel = new EffectPanel(postProcessingEffects->at(i), "Biquad filter"));
      }
      if(dynamic_cast<Equalizer*>(postProcessingEffects->at(i))) {
        addChildElement(effectPanel = new EffectPanel(postProcessingEffects->at(i), "Equalizer"));
      }
      if(dynamic_cast<Reverb*>(postProcessingEffects->at(i))) {
        addChildElement(effectPanel = new EffectPanel(postProcessingEffects->at(i), "Reverb"));
      }
      effectPanel->drawBackground = false;
      effectPanel->drawBorder = false;
      
      effectPanels.push_back(effectPanel);
      
      effectPanel->setPosition(0, line);
      effectPanelLocations.push_back(line + absolutePos.y);
      effectPanelLocationsRel.push_back(line);
      
      
      line += 21;
      
    }
    setSize(180, line + 5);

  }


  struct EffectTrackPanelListener : public GuiEventListener {
    EffectTrackPanel *effectTrackPanel;
    EffectTrackPanelListener(EffectTrackPanel *effectTrackPanel) {
      this->effectTrackPanel = effectTrackPanel;
    }
    bool removed = false;
    
    virtual void onAction(GuiElement *guiElement) override {
      
      for(int i=0; i<effectTrackPanel->effectPanels.size(); i++) {
        if(guiElement == effectTrackPanel->effectPanels[i]->removeButton) {
          if(effectTrackPanel->postProcessingEffects->at(i)->getPanel()) {
            effectTrackPanel->postProcessingEffects->at(i)->removePanel(effectTrackPanel);
          }          
          effectTrackPanel->postProcessingEffects->erase(effectTrackPanel->postProcessingEffects->begin() + i);
          removed = true;
        }
        if(guiElement == effectTrackPanel->effectPanels[i]->showButton) {
          if(effectTrackPanel->postProcessingEffects->at(i)->getPanel()) {
            effectTrackPanel->postProcessingEffects->at(i)->removePanel(effectTrackPanel);
          }
          else {
            effectTrackPanel->postProcessingEffects->at(i)->addPanel(effectTrackPanel);
            if(!effectTrackPanel->postProcessingEffects->at(i)->setPreviousPanelPosition()) {
              //panel->pos += Vec2d(60, 60);
            }
          }
        }
      }
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
    int minDistIndexPrev = -1;
    Vec2d pos;
    
    virtual void onMouseMotion(GuiElement *guiElement, Events &events) override {
      for(int i=0; i<effectTrackPanel->effectPanels.size(); i++) {
        effectTrackPanel->effectPanels[i]->pos.x = 0;
        
        if(effectTrackPanel->effectPanels[i]->pos.y < effectTrackPanel->titleBar->size.y+5) {
          effectTrackPanel->effectPanels[i]->pos.y = effectTrackPanel->titleBar->size.y+5;
        }
        if(effectTrackPanel->effectPanels[i]->pos.y > effectTrackPanel->size.y-5-21) {
          effectTrackPanel->effectPanels[i]->pos.y = effectTrackPanel->size.y-5-21;
        }
      }
      if(effectTrackPanel->moveableEffect >= 0) {
        double minDist = 9999999;
        int minDistIndex = -1;
        double y = effectTrackPanel->effectPanels[effectTrackPanel->moveableEffect]->absolutePos.y;
        for(int i=0; i<effectTrackPanel->effectPanelLocations.size(); i++) {
          double d = std::abs(effectTrackPanel->effectPanelLocations[i] - y);
          if(d < minDist) {
            minDistIndex = i;
            minDist = d;
          }
        }
        if(minDistIndex >= 0 && minDistIndex != minDistIndexPrev) {
          //printf("minDistIndex %d\n", minDistIndex);
          minDistIndexPrev = minDistIndex;
          
          if(minDistIndex != effectTrackPanel->moveableEffect) {
            effectTrackPanel->effectPanels[minDistIndex]->pos.y = effectTrackPanel->effectPanelLocationsRel[effectTrackPanel->moveableEffect];
            
            
            PostProcessingEffect *p = effectTrackPanel->postProcessingEffects->at(minDistIndex);
            effectTrackPanel->postProcessingEffects->at(minDistIndex) = effectTrackPanel->postProcessingEffects->at(effectTrackPanel->moveableEffect);
            effectTrackPanel->postProcessingEffects->at(effectTrackPanel->moveableEffect) = p;
            
            EffectPanel *tmp = effectTrackPanel->effectPanels[minDistIndex];
            effectTrackPanel->effectPanels[minDistIndex] = effectTrackPanel->effectPanels[effectTrackPanel->moveableEffect];
            effectTrackPanel->effectPanels[effectTrackPanel->moveableEffect] = tmp;
            effectTrackPanel->moveableEffect = minDistIndex;
          }
          
        }
      }
    }
    
    void onMousePressed(GuiElement *guiElement, Events &events) {
      effectTrackPanel->moveableEffect = -1;
      for(int i=0; i<effectTrackPanel->effectPanels.size(); i++) {
        if(effectTrackPanel->effectPanels[i]->isPointWithin(events.m)) {
          effectTrackPanel->moveableEffect = i;
          //effectTrackPanel->selectedEffectPanelIndex;
          //printf("effectTrackPanel->moveableEffect = %d\n", i);
        }
      }
    }
      
      virtual void onMouseReleased(GuiElement *guiElement, Events &events) {

        for(int i=0; i<effectTrackPanel->effectPanelLocationsRel.size(); i++) {
          effectTrackPanel->effectPanels[i]->pos.y = effectTrackPanel->effectPanelLocationsRel[i];
        }
        
        effectTrackPanel->moveableEffect = -1;
        minDistIndexPrev = -1;
        
        if(removed) {
          effectTrackPanel->onItemsChanged();
          removed = false;
        }

      }


  };

};
