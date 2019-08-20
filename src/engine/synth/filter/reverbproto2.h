#pragma once
#include "biquad.h"
#include "postprocessingeffect.h"
#include "multidelay.h"
#include "../delayline.h"
//#include "mathl.h"


static const int maxNumReverbChannels = 20;

struct ReverbProto2 : public PostProcessingEffect
{
  std::vector<BiquadFilter*> biquadFilters;
  std::vector<MultiDelay*> multiDelays;
  std::vector<DelayLine*> delayLines;
  
  std::vector<Vec2d> delayRanges = std::vector<Vec2d>(maxNumReverbChannels);
  std::vector<double> channelGains = std::vector<double>(maxNumReverbChannels);
  
  int numChannels = 8;
  
  double sampleRate = 0;
  double wetLevel = 1.0;
  double gain = 1.0;
  
  double delayLineLength = 5;
  
  const std::vector<std::string> algorithmNames = { "Delays parallel", "Delays parallel +ff1", "Delays parallel +ff2",
                                                    "Delays serial", "Delays serial +ff1", "Delays serial +ff2"};
  int algorithm = 0;
  
  MultiDelay::DelayDistribution delayDistribution;
  double feedForwardFactor = 0.5;
  
  ReverbProto2(double sampleRate) : PostProcessingEffect("ReverbProto2")  {
    this->sampleRate = sampleRate;
    biquadFilters.resize(maxNumReverbChannels);
    multiDelays.resize(maxNumReverbChannels);
    delayLines.resize(maxNumReverbChannels);
    
    std::vector<double> frequencies = {60, 200, 400, 900, 2000, 4500, 10000, 15000};
    std::vector<double> bandwidths = {2, 2, 2, 2, 2, 2, 2, 2};
    
    for(int i=0; i<maxNumReverbChannels; i++) {
      int k = min(i, 7);
      biquadFilters[i] = new BiquadFilter(sampleRate, BiquadFilter::Type::BandPass, frequencies[k], bandwidths[k], 0);
      delayLines[i] = new DelayLine(sampleRate, delayLineLength);
      multiDelays[i] = new MultiDelay(delayLines[i]);
      delayRanges[i].set(0.25, 1.0);
      channelGains[i] = 1.0;
    }
  }
  
  ReverbProto2(double sampleRate, ReverbProto2 &reverbProto2) : PostProcessingEffect("ReverbProto2")  {
    this->sampleRate = sampleRate;
    numChannels = reverbProto2.numChannels;
    wetLevel = reverbProto2.wetLevel;
    gain = reverbProto2.gain;
    delayLineLength = reverbProto2.delayLineLength;
    biquadFilters.resize(maxNumReverbChannels);
    multiDelays.resize(maxNumReverbChannels);
    delayLines.resize(maxNumReverbChannels);
    
    for(int i=0; i<maxNumReverbChannels; i++) {
      biquadFilters[i] = new BiquadFilter(*reverbProto2.biquadFilters[i]);
      delayLines[i] = new DelayLine(sampleRate, delayLineLength);
      multiDelays[i] = new MultiDelay(delayLines[i], *reverbProto2.multiDelays[i]);
      delayRanges[i] = reverbProto2.delayRanges[i];
      channelGains[i] = reverbProto2.channelGains[i];
    }
  }
  
  void apply(Vec2d &sample) {
    if(!isActive) return;
    Vec2d sampleOut;
    
    if(algorithm >= 0 && algorithm <= 2) {
      for(int i=0; i<numChannels; i++) {
        if(algorithm == 0) multiDelays[i]->feedForwardMode = 0;
        if(algorithm == 1) multiDelays[i]->feedForwardMode = 1;
        if(algorithm == 2) multiDelays[i]->feedForwardMode = 2;
        
        Vec2d sampleCopy(sample);
        biquadFilters[i]->apply(sampleCopy);
        
        delayLines[i]->update(sampleCopy);
        
        multiDelays[i]->apply(sampleCopy);
        delayLines[i]->buffer[delayLines[i]->bufferPos] = sampleCopy;
        
        sampleOut += sampleCopy * channelGains[i];
      }
    }
    else if(algorithm >= 3 && algorithm <= 5) {
      //Vec2d sampleCopy(sample);
      sampleOut.set(sample);
      for(int i=0; i<numChannels; i++) {
        if(algorithm == 3) multiDelays[i]->feedForwardMode = 0;
        if(algorithm == 4) multiDelays[i]->feedForwardMode = 1;
        if(algorithm == 5) multiDelays[i]->feedForwardMode = 2;

        biquadFilters[i]->wetLevel = clamp(channelGains[i], 0, 1);
        biquadFilters[i]->apply(sampleOut);
        
        delayLines[i]->update(sampleOut);
        
        multiDelays[i]->wetLevel = clamp(channelGains[i], 0, 1);
        multiDelays[i]->apply(sampleOut);
        delayLines[i]->buffer[delayLines[i]->bufferPos] = sampleOut;
        
        
        biquadFilters[i]->wetLevel = 1;
        multiDelays[i]->wetLevel = 1;
        //sampleOut += sampleCopy * channelGains[i];
      }
    }
    
    
    sample.set(sampleOut);
  }

  void reset() {
    for(int i=0; i<maxNumReverbChannels; i++) {
      biquadFilters[i]->reset();
      multiDelays[i]->reset();
      delayLines[i]->reset();
    }
  }

  struct ReverbProto2Panel : public Panel {
    CheckBox *isActiveGui = NULL;
    CheckBox *delaysAsSecondsGui = NULL;
    NumberBox *wetLevelGui = NULL, *gainGui = NULL;
    NumberBox *numChannelsGui = NULL;
    NumberBox *feedForwardFactorGui = NULL;
    ListBox *algorithmsGui = NULL;
    ListBox *delayDistributionGui = NULL;
    
    std::vector<NumberBox*> frequencyGuis = std::vector<NumberBox*>(maxNumReverbChannels, NULL);
    std::vector<NumberBox*> bandwidthGuis = std::vector<NumberBox*>(maxNumReverbChannels, NULL);
    std::vector<NumberBox*> gainGuis = std::vector<NumberBox*>(maxNumReverbChannels, NULL);
    std::vector<NumberBox*> numDelaysGuis = std::vector<NumberBox*>(maxNumReverbChannels, NULL);
    std::vector<NumberBox*> minDelayGuis = std::vector<NumberBox*>(maxNumReverbChannels, NULL);
    std::vector<NumberBox*> maxDelayGuis = std::vector<NumberBox*>(maxNumReverbChannels, NULL);

    std::vector<NumberBox*> minDelayIntegerGuis = std::vector<NumberBox*>(maxNumReverbChannels, NULL);
    std::vector<NumberBox*> maxDelayIntegerGuis = std::vector<NumberBox*>(maxNumReverbChannels, NULL);
    
    bool delaysAsSeconds = true;
    double line = 10, lineHeight = 23;
    int width = 65, widthE = 80, widthF = 80;
    int columnA = 10, columnB = 10 + width, columnC = 10 + width*2, columnD = 10 + width*3, columnE = 10 + width*4;
    int columnF = 10+width*4 + widthE;
    
    GuiElement *parentGuiElement = NULL;
    ReverbProto2 *reverbProto2 = NULL;
    
    ReverbProto2Panel(ReverbProto2 *reverbProto2, GuiElement *parentGuiElement) : Panel("ReverbProto2 panel") {
      init(reverbProto2, parentGuiElement);
    }
    
    void init(ReverbProto2 *reverbProto2, GuiElement *parentGuiElement) {
      this->reverbProto2 = reverbProto2;
      this->parentGuiElement = parentGuiElement;
      //this->synth = synth;
      

      addGuiEventListener(new EqualizerPanelListener(this));
      parentGuiElement->addChildElement(this);
      
      addChildElement(new Label(reverbProto2->getName(), 10, line));
      addChildElement(isActiveGui = new CheckBox("Active", reverbProto2->isActive, 10, line += lineHeight));
      addChildElement(wetLevelGui = new NumberBox("Wet level", reverbProto2->wetLevel, NumberBox::FLOATING_POINT, 0, 1, 10 + 80, line, 8));
      addChildElement(gainGui = new NumberBox("Gain", reverbProto2->gain, NumberBox::FLOATING_POINT, 0, 1e10, 10+270, line, 8));
      addChildElement(numChannelsGui = new NumberBox("Channels", reverbProto2->numChannels, NumberBox::INTEGER, 1, maxNumReverbChannels, 10, line += lineHeight, 5));
      numChannelsGui->incrementMode = NumberBox::IncrementMode::Linear;
      addChildElement(delaysAsSecondsGui = new CheckBox("Delays unit second", delaysAsSeconds, 10+160, line));
      
      addChildElement(algorithmsGui = new ListBox("Algorithm", 10, line+=lineHeight, 14));
      algorithmsGui->setItems(reverbProto2->algorithmNames);
      algorithmsGui->setValue(reverbProto2->algorithm);
      
      addChildElement(delayDistributionGui = new ListBox("Delay distribution", 10, line+=lineHeight, 14));
      delayDistributionGui->setItems(reverbProto2->multiDelays[0]->delayDistributionNames);
      delayDistributionGui->setValue(reverbProto2->delayDistribution);

      addChildElement(feedForwardFactorGui = new NumberBox("Feedforward factor", reverbProto2->feedForwardFactor, NumberBox::FLOATING_POINT, 0, 1, 10, line+=lineHeight, 8));

      addChildElement(new Label("Freq.", columnA, line+=lineHeight));
      addChildElement(new Label("Bandw.", columnB, line));
      addChildElement(new Label("Gain", columnC, line));
      addChildElement(new Label("Delays", columnD, line));
      addChildElement(new Label("Min del.", columnE, line));
      addChildElement(new Label("Max del.", columnF, line));

      for(int i=0; i<frequencyGuis.size(); i++) {
        line += lineHeight;
        frequencyGuis[i] = new NumberBox("", reverbProto2->biquadFilters[i]->frequency, NumberBox::FLOATING_POINT, -reverbProto2->sampleRate*0.5, reverbProto2->sampleRate*0.5, columnA, line, 5, 0);
        addChildElement(frequencyGuis[i]);
        
        addChildElement(bandwidthGuis[i] = new NumberBox("", reverbProto2->biquadFilters[i]->bandwidth, NumberBox::FLOATING_POINT, -1e6, 1e6, columnB, line, 5, 4, true));
        addChildElement(gainGuis[i] = new NumberBox("", reverbProto2->channelGains[i], NumberBox::FLOATING_POINT, 0, 1e6, columnC, line, 5, 4, true));
        
        addChildElement(numDelaysGuis[i] = new NumberBox("", reverbProto2->multiDelays[i]->numOutputDelays, NumberBox::INTEGER, 0, 100, columnD, line, 5));
        numDelaysGuis[i]->incrementMode = NumberBox::IncrementMode::Linear;

        addChildElement(minDelayGuis[i] = new NumberBox("", reverbProto2->delayRanges[i].x, NumberBox::FLOATING_POINT, 0, reverbProto2->delayLineLength, columnE, line, 6, 6, true));
        //minDelayGuis[i]->incrementMode = NumberBox::IncrementMode::Linear;
        addChildElement(maxDelayGuis[i] = new NumberBox("", reverbProto2->delayRanges[i].y, NumberBox::FLOATING_POINT, 0, reverbProto2->delayLineLength, columnF, line, 6, 6, true));
        //maxDelayGuis[i]->incrementMode = NumberBox::IncrementMode::Linear;

        addChildElement(minDelayIntegerGuis[i] = new NumberBox("", (int)(reverbProto2->delayRanges[i].x*reverbProto2->sampleRate), NumberBox::INTEGER, 0, reverbProto2->delayLines[0]->bufferSize, columnE, line, 6));
        minDelayIntegerGuis[i]->incrementMode = NumberBox::IncrementMode::Linear;
        minDelayIntegerGuis[i]->setVisible(false);
        addChildElement(maxDelayIntegerGuis[i] = new NumberBox("", (int)(reverbProto2->delayRanges[i].y*reverbProto2->sampleRate), NumberBox::INTEGER, 0, reverbProto2->delayLines[0]->bufferSize, columnF, line, 6));
        maxDelayIntegerGuis[i]->incrementMode = NumberBox::IncrementMode::Linear;
        maxDelayIntegerGuis[i]->setVisible(false);
      }

      update();
      setSize(columnF + widthF, 10 + lineHeight * (reverbProto2->numChannels+7) + 10);
      
      for(int i=0; i<maxNumReverbChannels; i++) {
        reverbProto2->biquadFilters[i]->initGraphPanel(150, 100, false, false);
        reverbProto2->biquadFilters[i]->graphPanel->setPosition(size.x + 5 + 155 * (i % 3), 105 * (i / 3));
        reverbProto2->biquadFilters[i]->graphPanel->showAxes = false;
        reverbProto2->biquadFilters[i]->graphPanel->numHorizontalMarks = 3;
        reverbProto2->biquadFilters[i]->graphPanel->numHorizontalAxisLabels = 1;
        //reverbProto2->biquadFilters[i]->graphPanel->horizontalAxisLimits.set(0, reverbProto2->biquadFilters[i]->frequency*2);
        //int k = (int)((reverbProto2->biquadFilters[i]->frequency*0.00125 + 1.0));
        //reverbProto2->biquadFilters[i]->graphPanel->horizontalAxisLimits.set(0, k*1000);
        reverbProto2->biquadFilters[i]->prepareFilterResponseGraph();
        addChildElement(reverbProto2->biquadFilters[i]->graphPanel);
      }
      
    }

    void update() {
      numChannelsGui->setValue(reverbProto2->numChannels);
      isActiveGui->setValue(reverbProto2->isActive);
      wetLevelGui->setValue(reverbProto2->wetLevel);
      gainGui->setValue(reverbProto2->gain);
      for(int i=0; i<maxNumReverbChannels; i++) {
        if(i < reverbProto2->numChannels) {
          frequencyGuis[i]->setValue(reverbProto2->biquadFilters[i]->frequency);
          bandwidthGuis[i]->setValue(reverbProto2->biquadFilters[i]->bandwidth);
          gainGuis[i]->setValue(reverbProto2->channelGains[i]);
          numDelaysGuis[i]->setValue(reverbProto2->multiDelays[i]->numOutputDelays);
          minDelayGuis[i]->setValue(reverbProto2->delayRanges[i].x);
          maxDelayGuis[i]->setValue(reverbProto2->delayRanges[i].y);
          minDelayIntegerGuis[i]->setValue((int)(reverbProto2->delayRanges[i].x*reverbProto2->sampleRate));
          maxDelayIntegerGuis[i]->setValue((int)(reverbProto2->delayRanges[i].y*reverbProto2->sampleRate));
        }
        frequencyGuis[i]->setVisible(i < reverbProto2->numChannels);
        bandwidthGuis[i]->setVisible(i < reverbProto2->numChannels);
        gainGuis[i]->setVisible(i < reverbProto2->numChannels);
        numDelaysGuis[i]->setVisible(i < reverbProto2->numChannels);
        minDelayGuis[i]->setVisible(i < reverbProto2->numChannels && delaysAsSeconds);
        maxDelayGuis[i]->setVisible(i < reverbProto2->numChannels && delaysAsSeconds);
        minDelayIntegerGuis[i]->setVisible(i < reverbProto2->numChannels && !delaysAsSeconds);
        maxDelayIntegerGuis[i]->setVisible(i < reverbProto2->numChannels && !delaysAsSeconds);
      }
      
      setSize(columnF + widthF, 10 + lineHeight * (reverbProto2->numChannels+7) + 10);
    }

    struct EqualizerPanelListener : public GuiEventListener {
      ReverbProto2Panel *reverbProto2Panel;
      EqualizerPanelListener(ReverbProto2Panel *reverbProto2Panel) {
        this->reverbProto2Panel = reverbProto2Panel;
      }

      void onValueChange(GuiElement *guiElement) {
        if(guiElement == reverbProto2Panel->numChannelsGui) {
          guiElement->getValue((void*)&reverbProto2Panel->reverbProto2->numChannels);
          reverbProto2Panel->update();
        }
        if(guiElement == reverbProto2Panel->isActiveGui) {
          guiElement->getValue((void*)&reverbProto2Panel->reverbProto2->isActive);
        }
        if(guiElement == reverbProto2Panel->delaysAsSecondsGui) {
          guiElement->getValue((void*)&reverbProto2Panel->delaysAsSeconds);
          reverbProto2Panel->update();
        }
        if(guiElement == reverbProto2Panel->wetLevelGui) {
          guiElement->getValue((void*)&reverbProto2Panel->reverbProto2->wetLevel);
          for(int i=0; i<maxNumReverbChannels; i++) {
            reverbProto2Panel->reverbProto2->biquadFilters[i]->wetLevel = reverbProto2Panel->reverbProto2->wetLevel;
            reverbProto2Panel->reverbProto2->multiDelays[i]->wetLevel = reverbProto2Panel->reverbProto2->wetLevel;
          }
        }
        if(guiElement == reverbProto2Panel->gainGui) {
          guiElement->getValue((void*)&reverbProto2Panel->reverbProto2->gain);
          for(int i=0; i<maxNumReverbChannels; i++) {
            reverbProto2Panel->reverbProto2->biquadFilters[i]->gain = reverbProto2Panel->reverbProto2->gain;
            reverbProto2Panel->reverbProto2->multiDelays[i]->gain = reverbProto2Panel->reverbProto2->gain;
          }
        }
        if(guiElement == reverbProto2Panel->algorithmsGui) {
          guiElement->getValue((void*)&reverbProto2Panel->reverbProto2->algorithm);
        }
        if(guiElement == reverbProto2Panel->feedForwardFactorGui) {
          guiElement->getValue((void*)&reverbProto2Panel->reverbProto2->feedForwardFactor);
          for(int i=0; i<reverbProto2Panel->reverbProto2->numChannels; i++) {
            reverbProto2Panel->reverbProto2->multiDelays[i]->feedForwardFactor = reverbProto2Panel->reverbProto2->feedForwardFactor;
          }
        }
        if(guiElement == reverbProto2Panel->delayDistributionGui) {
          guiElement->getValue((void*)&reverbProto2Panel->reverbProto2->delayDistribution);
          for(int i=0; i<reverbProto2Panel->reverbProto2->numChannels; i++) {
            reverbProto2Panel->reverbProto2->multiDelays[i]->distributeDelays(reverbProto2Panel->reverbProto2->delayRanges[i], reverbProto2Panel->reverbProto2->delayDistribution);
          }
        }
        
        for(int i=0; i<reverbProto2Panel->reverbProto2->numChannels; i++) {
          if(guiElement == reverbProto2Panel->frequencyGuis[i]) {
            guiElement->getValue((void*)&reverbProto2Panel->reverbProto2->biquadFilters[i]->frequency);
            reverbProto2Panel->reverbProto2->biquadFilters[i]->prepare();
          }
          if(guiElement == reverbProto2Panel->bandwidthGuis[i]) {
            guiElement->getValue((void*)&reverbProto2Panel->reverbProto2->biquadFilters[i]->bandwidth);
            reverbProto2Panel->reverbProto2->biquadFilters[i]->prepare();
          }
          if(guiElement == reverbProto2Panel->gainGuis[i]) {
            guiElement->getValue((void*)&reverbProto2Panel->reverbProto2->channelGains[i]);
            
          }
          if(guiElement == reverbProto2Panel->numDelaysGuis[i]) {
            guiElement->getValue((void*)&reverbProto2Panel->reverbProto2->multiDelays[i]->numOutputDelays);
            reverbProto2Panel->reverbProto2->multiDelays[i]->distributeDelays(reverbProto2Panel->reverbProto2->delayRanges[i], reverbProto2Panel->reverbProto2->delayDistribution);
          }
          if(guiElement == reverbProto2Panel->minDelayGuis[i]) {
            guiElement->getValue((void*)&reverbProto2Panel->reverbProto2->delayRanges[i].x);
            reverbProto2Panel->reverbProto2->multiDelays[i]->distributeDelays(reverbProto2Panel->reverbProto2->delayRanges[i], reverbProto2Panel->reverbProto2->delayDistribution);
          }
          if(guiElement == reverbProto2Panel->maxDelayGuis[i]) {
            guiElement->getValue((void*)&reverbProto2Panel->reverbProto2->delayRanges[i].y);
            reverbProto2Panel->reverbProto2->multiDelays[i]->distributeDelays(reverbProto2Panel->reverbProto2->delayRanges[i], reverbProto2Panel->reverbProto2->delayDistribution);
          }
          if(guiElement == reverbProto2Panel->minDelayIntegerGuis[i]) {
            int value;
            guiElement->getValue((void*)&value);
            reverbProto2Panel->reverbProto2->delayRanges[i].x = (double)value/reverbProto2Panel->reverbProto2->sampleRate;
            reverbProto2Panel->reverbProto2->multiDelays[i]->distributeDelays(reverbProto2Panel->reverbProto2->delayRanges[i], reverbProto2Panel->reverbProto2->delayDistribution);
          }
          if(guiElement == reverbProto2Panel->maxDelayIntegerGuis[i]) {
            int value;
            guiElement->getValue((void*)&value);
            reverbProto2Panel->reverbProto2->delayRanges[i].y = (double)value/reverbProto2Panel->reverbProto2->sampleRate;
            reverbProto2Panel->reverbProto2->multiDelays[i]->distributeDelays(reverbProto2Panel->reverbProto2->delayRanges[i], reverbProto2Panel->reverbProto2->delayDistribution);
          }

        }
      }

      void onKeyDown(GuiElement *guiElement, Events &events) {
        if(events.sdlKeyCode == SDLK_HOME) {
          for(int i=0; i<reverbProto2Panel->reverbProto2->numChannels; i++) {
            if(guiElement == reverbProto2Panel->frequencyGuis[i] ||
               guiElement == reverbProto2Panel->bandwidthGuis[i]) {
               for(int k=0; k<reverbProto2Panel->reverbProto2->numChannels; k++) {
                 reverbProto2Panel->reverbProto2->biquadFilters[k]->graphPanel->toggleVisibility();
               }
            }
          }
        }
      }
    };
  };
  ReverbProto2Panel *reverbProto2Panel = NULL;
  
  Panel *getPanel() {
    return reverbProto2Panel;
  }

  Panel *addPanel(GuiElement *parentGuiElement, const std::string &title = "") {
    return reverbProto2Panel = new ReverbProto2Panel(this, parentGuiElement);
  }
  
  void removePanel(GuiElement *parentGuiElement) {
    PanelInterface::removePanel(parentGuiElement);

    if(reverbProto2Panel) {
      parentGuiElement->deleteChildElement(reverbProto2Panel);
    }
  }

  void updatePanel() {
    if(reverbProto2Panel) {
      reverbProto2Panel->update();
    }
  }
  
  
  
  
};