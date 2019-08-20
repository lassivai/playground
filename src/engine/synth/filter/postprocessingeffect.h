#pragma once

#include <string>
#include <ctgmath>

#include "../../gui/gui.h"
#include "../delayline.h"
#include "../../util.h"
#include "../preset.h"



struct PostProcessingEffect : public PanelInterface, public HierarchicalTextFileParser {
  Panel *panel = NULL;
  DelayLine *delayLine = NULL;
  bool isActive = true;
  std::string name;
  std::string presetName;
  
  std::vector<Preset> *presets = NULL;

  virtual ~PostProcessingEffect() {}

  PostProcessingEffect(DelayLine *delayLine, const std::string &name) {
    this->delayLine = delayLine;
    this->name = name;
  }
  
  PostProcessingEffect(const std::string &name) {
    this->name = name;
  }

  //virtual void update() {}
  virtual void apply(Vec2d &sample) {}

  std::string getName() {
    return name;
  }

  virtual Panel *addPanel(GuiElement *parentElement, const std::string &title = "") = 0;
  virtual void removePanel(GuiElement *parentElement) = 0;
  virtual void updatePanel() = 0;
  virtual Panel *getPanel() = 0;

  Panel *previewPanel = NULL;
  Panel *getPreviewPanel(const Vec2d &size) {
    return previewPanel ? previewPanel : previewPanel = createPreviewPanel(size);
  }
  virtual Panel *createPreviewPanel(const Vec2d &size) {
    return new Panel(size);
  }
  
  virtual void reset() {}

  virtual void prepare() {}
  
  virtual void applyPreset(int index) {}

  virtual void initializeEffectTrackPanel(Panel *effectTrackPanel) {}


  virtual std::string getBlockName() {
    return "effectBase";
  }
};


