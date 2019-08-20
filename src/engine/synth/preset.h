#pragma once
#include "../util.h"

struct Preset : public HierarchicalTextFileParser{
  std::string name;
  //std::vector<DoubleParameter> doubleParameters;
  //std::vector<StringParameter> stringParameters;
  std::vector<Preset*> subPresets;
  
  virtual ~Preset() {
    for(int i=0; i<subPresets.size(); i++) {
      delete subPresets[i];
    }
  }
  
  Preset() {}
  
  Preset(const std::string &name) {
    this->name = name;
  }
  
  virtual std::string getBlockName() {
    return name;
  }

  virtual void encodeParameters() {}
  virtual void decodeParameters() {}
  
  virtual void onPrintParameters(std::string &content, int level = 0) {
    for(int i=0; i<subPresets.size(); i++) {
      subPresets[i]->printParameters(content, level);
    }
  }

  virtual int onNewBlock(const std::string &blockName, int blockStartInd, int blockLevel) {
    Preset *subPreset = new Preset(blockName);
    subPresets.push_back(subPreset);    
    return subPreset->parse(content, blockStartInd, blockLevel);
  }
  
};