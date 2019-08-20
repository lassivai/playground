#pragma once
#include "postprocessingeffect.h"



// Experimental reverb...
struct ReverbPrototype : public PostProcessingEffect {
  double reverbLength = 2;
  double exponentialDecay = -4.0;
  double linearDecay = 0.4;
  int numSamples = 20;

  ReverbPrototype(DelayLine *delayLine) : PostProcessingEffect(delayLine, "Proto-reverb") {
  }

  void apply(Vec2d &sample) {
    if(!isActive) return;

    int reverbSize = (int)(reverbLength*delayLine->sampleRate);
    Vec2d r;
    double total = 0;
    int k = reverbSize / (numSamples+1);
    for(int i=0; i<reverbSize; i+=k) {
      int ri = (delayLine->bufferPos - 2 - i + delayLine->bufferSize) % delayLine->bufferSize;
      double t = (double)i/delayLine->bufferSize;
      t = exp(exponentialDecay*t);
      total += t;
      r += delayLine->buffer[ri].x * t;
    }
    r /= total;
    r *= linearDecay;

    sample += r;
  }
  
  /*void update() {
    if(!isActive) return;

    int reverbSize = (int)(reverbLength*delayLine->sampleRate);
    double r = 0;
    double total = 0;
    int k = reverbSize / (numSamples+1);
    for(int i=0; i<reverbSize; i+=k) {
      int ri = (delayLine->bufferPos - 2 - i + delayLine->bufferSize) % delayLine->bufferSize;
      double t = (double)i/delayLine->bufferSize;
      t = exp(exponentialDecay*t);
      total += t;
      r += t * delayLine->buffer[ri].x;
    }
    r /= total;
    r *= linearDecay;

    delayLine->buffer[delayLine->bufferPos].x += r;
  }*/


  NumberBox *reverbLengthGui, *exponentialDecayGui, *linearDecayGui, *numSamplesGui;
  CheckBox *isActiveGui;

  Panel *getPanel() {
    return panel;
  }

  Panel *addPanel(GuiElement *parentElement, const std::string &title = "") {
    if(!panel) {
      panel = new Panel(320, 320, 60, 10);
      double line = 10, lineHeight = 23;

      Label *label = new Label(getName(), 10, line);
      isActiveGui = new CheckBox("Active", isActive, 10, line += lineHeight);
      reverbLengthGui = new NumberBox("Reverb length", reverbLength, NumberBox::FLOATING_POINT, 0, delayLine->bufferLength, 10, line += lineHeight);
      //exponentialDecayGui = new NumberBox("Decay (exp.)", exponentialDecay, NumberBox::FLOATING_POINT, -1000, -0.0001, 10, line += lineHeight);
      linearDecayGui = new NumberBox("Decay (linear)", linearDecay, NumberBox::FLOATING_POINT, 0, 1.0, 10, line += lineHeight);
      numSamplesGui = new NumberBox("Sample count", numSamples, NumberBox::INTEGER, 1, 100000, 10, line += lineHeight);
      panel->addChildElement(label);
      panel->addChildElement(isActiveGui);
      panel->addChildElement(reverbLengthGui);
      //panel->addChildElement(exponentialDecayGui);
      panel->addChildElement(linearDecayGui);
      panel->addChildElement(numSamplesGui);
      panel->addGuiEventListener(new ReverbPrototypePanelListener(this));

      panel->size.set(320, line + lineHeight + 10);

      parentElement->addChildElement(panel);
    }

    return panel;
  }

  void removePanel(GuiElement *parentElement) {
    PanelInterface::removePanel(parentElement);

    if(panel) {
      parentElement->deleteChildElement(panel);
      panel = NULL;
    }
  }

  void updatePanel() {

  }


  struct ReverbPrototypePanelListener : public GuiEventListener {
    ReverbPrototype *reverbPrototype;
    ReverbPrototypePanelListener(ReverbPrototype *reverbPrototype) {
      this->reverbPrototype = reverbPrototype;
    }
    void onValueChange(GuiElement *guiElement) {
      if(guiElement == reverbPrototype->isActiveGui) {
        guiElement->getValue(&reverbPrototype->isActive);
      }
      if(guiElement == reverbPrototype->reverbLengthGui) {
        guiElement->getValue(&reverbPrototype->reverbLength);
      }
      /*if(guiElement == reverbPrototype->exponentialDecayGui) {
        guiElement->getValue(&reverbPrototype->exponentialDecay);
      }*/
      if(guiElement == reverbPrototype->linearDecayGui) {
        guiElement->getValue(&reverbPrototype->linearDecay);
      }
      if(guiElement == reverbPrototype->numSamplesGui) {
        guiElement->getValue(&reverbPrototype->numSamples);
      }
    }
  };
};
