#pragma once
#include "postprocessingeffect.h"
#include "../delayline.h"
#include <freeverb/revmodel.hpp>
#include <freeverb/nrevb.hpp>
#include <freeverb/strev.hpp>
#include <freeverb/zrev2.hpp>
#include <freeverb/progenitor2.hpp>

struct MVerbWrap : public PostProcessingEffect
{
  enum Type { FreeVerb, CCRMA, Lexicon224, Progenitor, Zita };
  const std::vector<std::string> typeNames = { "FreeVerb", "CCRMA", "Lexicon224", "Progenitor", "Zita" };
  
  double sampleRate;
  
  Type type = FreeVerb;
  
  fv3::revmodel_ freeverb;
  fv3::zrev2_ zita;
  fv3::nrevb_ ccrma;
  fv3::strev_ lexicon224;
  fv3::progenitor2_ progenitor;
  
  double masterWetLevel = 1.0;
    
  Reverb(double sampleRate) : PostProcessingEffect("FreeVerb")  {
    this->sampleRate = sampleRate;
    freeverb.setSampleRate(sampleRate);
    zita.setSampleRate(sampleRate);
    ccrma.setSampleRate(sampleRate);
    lexicon224.setSampleRate(sampleRate);
    progenitor.setSampleRate(sampleRate);
  }
  
  Reverb(Reverb &freeVerbWrap) : PostProcessingEffect("FreeVerb")  {
    this->sampleRate = freeVerbWrap.sampleRate;
    /*freeverb.setSampleRate(freeVerbWrap.freeverb.getSampleRate());
    freeverb.setwet(freeVerbWrap.freeverb.getwet());
    freeverb.setdry(freeVerbWrap.freeverb.getdry());
    freeverb.setroomsize(freeVerbWrap.freeverb.getroomsize());
    freeverb.setdamp(freeVerbWrap.freeverb.getdamp());
    freeverb.setwidth(freeVerbWrap.freeverb.getwidth());*/
  }
  
  void apply(Vec2d &sample) {
    if(!isActive) return;
    Vec2d sampleIn(sample);
    
    if(type == FreeVerb) {
      freeverb.processreplace(&sample.x, &sample.y, &sample.x, &sample.y, 1);
    }
    else if(type == CCRMA) {
      ccrma.processreplace(&sample.x, &sample.y, &sample.x, &sample.y, 1);
    }
    else if(type == Lexicon224) {
      lexicon224.processreplace(&sample.x, &sample.y, &sample.x, &sample.y, 1);
    }
    else if(type == Progenitor) {
      progenitor.processreplace(&sample.x, &sample.y, &sample.x, &sample.y, 1);
    }
    else if(type == Zita) {
      zita.processreplace(&sample.x, &sample.y, &sample.x, &sample.y, 1);
    }
    
    sample = sample * masterWetLevel + sampleIn * (1.0 - masterWetLevel);    
  }
  

  void reset() {
    freeverb.mute();
    ccrma.mute();
    lexicon224.mute();
    progenitor.mute();
    zita.mute();
  }

  struct FreeVerbPanel : public Panel {
    CheckBox *isActiveGui = NULL;
    CheckBox *muteOnParameterChangeGui = NULL;
    ListBox *typeGui = NULL;
    NumberBox *initialDelayGui = NULL;
    NumberBox *preDelayGui = NULL;
    NumberBox *masterWetLevelGui = NULL;
    
    std::vector<double> guiHeights = std::vector<double>(5);
    // freeverb
    NumberBox *wetLevelFreeVerbGui = NULL;
    NumberBox *dryLevelFreeVerbGui = NULL;
    NumberBox *roomSizeFreeVerbGui = NULL;
    NumberBox *dampFreeVerbGui = NULL;
    NumberBox *widthFreeVerbGui = NULL;
    // CCRMA
    NumberBox *feedbackCcrmaGui = NULL;
    NumberBox *apFeedbackCcrmaGui = NULL;
    NumberBox *dampCcrmaGui = NULL;
    // progenitor
    NumberBox *diffusion1ProgGui = NULL;
    NumberBox *diffusion2ProgGui = NULL;
    NumberBox *diffusion3ProgGui = NULL;
    NumberBox *diffusion4ProgGui = NULL;
    NumberBox *dampProgGui = NULL;
    NumberBox *damp2ProgGui = NULL;
    NumberBox *inputDampProgGui = NULL;
    NumberBox *ouputDampProgGui = NULL;
    NumberBox *ouputDampBandwidthProgGui = NULL;
    NumberBox *spinProgGui = NULL;
    NumberBox *spinLimitProgGui = NULL;
    NumberBox *wanderProgGui = NULL;
    NumberBox *spin2ProgGui = NULL;
    NumberBox *spin2WanderProgGui = NULL;
    NumberBox *spin2LimitProgGui = NULL;
    NumberBox *wander2ProgGui = NULL;
    NumberBox *decay0ProgGui = NULL;
    NumberBox *decay1ProgGui = NULL;
    NumberBox *decay2ProgGui = NULL;
    NumberBox *decay3ProgGui = NULL;
    NumberBox *bassBandwidthProgGui = NULL;
    NumberBox *bassBoostProgGui = NULL;
    NumberBox *idiffusionProgGui = NULL;
    NumberBox *odiffusionProgGui = NULL;
    NumberBox *modulationNoise1ProgGui = NULL;
    NumberBox *modulationNoise2ProgGui = NULL;
    NumberBox *crossFeedProgGui = NULL;
    // lexicon224
    NumberBox *diffusion1LexiconGui = NULL;
    NumberBox *diffusion2LexiconGui = NULL;
    NumberBox *dampLexiconGui = NULL;
    NumberBox *inputDampLexiconGui = NULL;
    NumberBox *ouputDampLexiconGui = NULL;
    NumberBox *rt60LexiconGui = NULL;
    NumberBox *dcCutFrequencyLexiconGui = NULL;
    NumberBox *spinLexiconGui = NULL;
    NumberBox *spinDifferenceLexiconGui = NULL;
    NumberBox *spinLimitLexiconGui = NULL;
    NumberBox *wanderLexiconGui = NULL;
    NumberBox *modulationNoise1LexiconGui = NULL;
    NumberBox *modulationNoise2LexiconGui = NULL;
    NumberBox *autoDifferenceLexiconGui = NULL;
    // zita
    NumberBox *rt60ZitaGui = NULL;
    NumberBox *apFeedbackZitaGui = NULL;
    NumberBox *loopDampZitaGui = NULL;
    NumberBox *outputLpfZitaGui = NULL;
    NumberBox *outputHpfZitaGui = NULL;
    NumberBox *dcCutFrequencyZitaGui = NULL;
    NumberBox *lfo1FrequencyZitaGui = NULL;
    NumberBox *lfo2FrequencyZitaGui = NULL;
    NumberBox *lfoFactorZitaGui = NULL;
    NumberBox *rt60LowZitaGui = NULL;
    NumberBox *rt60HighZitaGui = NULL;
    NumberBox *diffusionZitaGui = NULL;
    NumberBox *spinZitaGui = NULL;
    NumberBox *spinFactorZitaGui = NULL;
    NumberBox *wanderZitaGui = NULL;
    NumberBox *xoverLowZitaGui = NULL;
    NumberBox *xoverHighZitaGui = NULL;
    

    GuiElement *parentGuiElement = NULL;
    Reverb *freeVerbWrap = NULL;
    
    FreeVerbPanel(Reverb *freeVerbWrap, GuiElement *parentGuiElement) : Panel("Reverb panel") {
      init(freeVerbWrap, parentGuiElement);
    }
    
    void init(Reverb *freeVerbWrap, GuiElement *parentGuiElement) {
      this->freeVerbWrap = freeVerbWrap;
      this->parentGuiElement = parentGuiElement;
      //this->synth = synth;
      double line = 10, lineHeight = 23;

      addGuiEventListener(new FreeVerbPanelListener(this));
      parentGuiElement->addChildElement(this);
      
      addChildElement(new Label(freeVerbWrap->getName(), 10, line));
      addChildElement(isActiveGui = new CheckBox("Active", freeVerbWrap->isActive, 10, line += lineHeight));
      
      addChildElement(masterWetLevelGui = new NumberBox("Master wet", freeVerbWrap->masterWetLevel, NumberBox::FLOATING_POINT, 0, 1, 10, line+=lineHeight, 8));
      
      addChildElement(typeGui = new ListBox("Type", 10, line+=lineHeight));
      typeGui->setItems(freeVerbWrap->typeNames);
      typeGui->setValue(freeVerbWrap->type);
      
      double lineStart = line;

      //addChildElement(initialDelayGui = new NumberBox("Initial delay", freeVerbWrap->freeverb.getInitialDelay(), NumberBox::FLOATING_POINT, 0, 1e10, 10, line+=lineHeight, 8));
      //addChildElement(preDelayGui = new NumberBox("Pre-delay", freeVerbWrap->freeverb.getPreDelay(), NumberBox::FLOATING_POINT, 0, 1e10, 10, line+=lineHeight, 8));
      
      
      // freeverb
      addChildElement(wetLevelFreeVerbGui = new NumberBox("Wet", freeVerbWrap->freeverb.getwet(), NumberBox::FLOATING_POINT, 0, 1, 10, line+=lineHeight, 8));
      addChildElement(dryLevelFreeVerbGui = new NumberBox("Dry", freeVerbWrap->freeverb.getdry(), NumberBox::FLOATING_POINT, 0, 1, 10, line+=lineHeight, 8));
      
      addChildElement(roomSizeFreeVerbGui = new NumberBox("Room size", freeVerbWrap->freeverb.getroomsize(), NumberBox::FLOATING_POINT, 0, 1, 10, line+=lineHeight, 8));
      addChildElement(dampFreeVerbGui = new NumberBox("Damp", freeVerbWrap->freeverb.getdamp(), NumberBox::FLOATING_POINT, 0, 1, 10, line+=lineHeight, 8));
      addChildElement(widthFreeVerbGui = new NumberBox("Width", freeVerbWrap->freeverb.getwidth(), NumberBox::FLOATING_POINT, 0, 1, 10, line+=lineHeight, 8));
      guiHeights[Reverb::Type::FreeVerb] = line + lineHeight + 10;
      // CCRMA
      line = lineStart;
      addChildElement(feedbackCcrmaGui = new NumberBox("Feedback", freeVerbWrap->ccrma.getfeedback(), NumberBox::FLOATING_POINT, 0, 1, 10, line+=lineHeight, 8));
      addChildElement(apFeedbackCcrmaGui = new NumberBox("AP Feedback", freeVerbWrap->ccrma.getapfeedback(), NumberBox::FLOATING_POINT, 0, 1, 10, line+=lineHeight, 8));
      addChildElement(dampCcrmaGui = new NumberBox("Damp", freeVerbWrap->ccrma.getdamp(), NumberBox::FLOATING_POINT, 0, 1, 10, line+=lineHeight, 8));
      guiHeights[Reverb::Type::CCRMA] = line + lineHeight + 10;
      
      // progenitor
      line = lineStart;
      addChildElement(diffusion1ProgGui = new NumberBox("Diffusion 1", freeVerbWrap->progenitor.getdiffusion1(), NumberBox::FLOATING_POINT, 0, 0.97, 10, line+=lineHeight, 8));
      addChildElement(diffusion2ProgGui = new NumberBox("Diffusion 2", freeVerbWrap->progenitor.getdiffusion2(), NumberBox::FLOATING_POINT, 0, 0.97, 10, line+=lineHeight, 8));
      addChildElement(diffusion3ProgGui = new NumberBox("Diffusion 3", freeVerbWrap->progenitor.getdiffusion3(), NumberBox::FLOATING_POINT, 0, 0.97, 10, line+=lineHeight, 8));
      addChildElement(diffusion4ProgGui = new NumberBox("Diffusion 4", freeVerbWrap->progenitor.getdiffusion4(), NumberBox::FLOATING_POINT, 0, 0.97, 10, line+=lineHeight, 8));
            
      addChildElement(dampProgGui = new NumberBox("Effect lowpass", freeVerbWrap->progenitor.getdamp(), NumberBox::FLOATING_POINT, 0, freeVerbWrap->sampleRate/2, 10, line+=lineHeight, 8));
      //addChildElement(damp2ProgGui = new NumberBox("Damp 2", freeVerbWrap->progenitor.getdamp2(), NumberBox::FLOATING_POINT, 0, 1e10, 10, line+=lineHeight, 8));
      addChildElement(inputDampProgGui = new NumberBox("Input lowpass", freeVerbWrap->progenitor.getinputdamp(), NumberBox::FLOATING_POINT, 0, freeVerbWrap->sampleRate/2, 10, line+=lineHeight, 8));
      addChildElement(ouputDampProgGui = new NumberBox("Output lowpass", freeVerbWrap->progenitor.getoutputdamp(), NumberBox::FLOATING_POINT, 0, freeVerbWrap->sampleRate/2, 10, line+=lineHeight, 8));
      addChildElement(ouputDampBandwidthProgGui = new NumberBox("Output lowpass BW", freeVerbWrap->progenitor.getoutputdampbw(), NumberBox::FLOATING_POINT, 0, 1e10, 10, line+=lineHeight, 8));

      addChildElement(spinProgGui = new NumberBox("Spin", freeVerbWrap->progenitor.getspin(), NumberBox::FLOATING_POINT, 0, 1e10, 10, line+=lineHeight, 8));
      addChildElement(spinLimitProgGui = new NumberBox("Spin limit", freeVerbWrap->progenitor.getspinlimit(), NumberBox::FLOATING_POINT, 0, 1e10, 10, line+=lineHeight, 8));
      //addChildElement(wanderProgGui = new NumberBox("Spin strength", freeVerbWrap->progenitor.getwander(), NumberBox::FLOATING_POINT, 0, 1e10, 10, line+=lineHeight, 8));
      //addChildElement(spin2ProgGui = new NumberBox("Spin 2", freeVerbWrap->progenitor.getspin2(), NumberBox::FLOATING_POINT, 0, 1e10, 10, line+=lineHeight, 8));
      //addChildElement(spin2LimitProgGui = new NumberBox("Spin 2 limit", freeVerbWrap->progenitor.getspinlimit2(), NumberBox::FLOATING_POINT, 0, 1e10, 10, line+=lineHeight, 8));
      /*addChildElement(wander2ProgGui = new NumberBox("Spin 2 strength", freeVerbWrap->progenitor.getwander2(), NumberBox::FLOATING_POINT, 0, 1e10, 10, line+=lineHeight, 8));
      addChildElement(spin2WanderProgGui = new NumberBox("Spin filter length", freeVerbWrap->progenitor.getspin2wander(), NumberBox::FLOATING_POINT, 0, 1e10, 10, line+=lineHeight, 8));*/

      addChildElement(decay0ProgGui = new NumberBox("Decay 1", freeVerbWrap->progenitor.getdecay0(), NumberBox::FLOATING_POINT, 0, 0.99, 10, line+=lineHeight, 8));
      addChildElement(decay1ProgGui = new NumberBox("Decay 2", freeVerbWrap->progenitor.getdecay1(), NumberBox::FLOATING_POINT, 0, 0.99, 10, line+=lineHeight, 8));
      addChildElement(decay2ProgGui = new NumberBox("Decay 3", freeVerbWrap->progenitor.getdecay2(), NumberBox::FLOATING_POINT, 0, 0.99, 10, line+=lineHeight, 8));
      addChildElement(decay3ProgGui = new NumberBox("Decay 4", freeVerbWrap->progenitor.getdecay3(), NumberBox::FLOATING_POINT, 0, 0.99, 10, line+=lineHeight, 8));
      
      /*addChildElement(bassBandwidthProgGui = new NumberBox("Bass bandwidth", freeVerbWrap->progenitor.getbassbw(), NumberBox::FLOATING_POINT, 0, 1e10, 10, line+=lineHeight, 8));
      addChildElement(bassBoostProgGui = new NumberBox("Bass boost", freeVerbWrap->progenitor.getbassboost(), NumberBox::FLOATING_POINT, 0, 1e10, 10, line+=lineHeight, 8));*/
      
      /*addChildElement(idiffusionProgGui = new NumberBox("Input diffusion", freeVerbWrap->progenitor.getidiffusion1(), NumberBox::FLOATING_POINT, 0, 1, 10, line+=lineHeight, 8));
      addChildElement(odiffusionProgGui = new NumberBox("Output diffusion", freeVerbWrap->progenitor.getodiffusion1(), NumberBox::FLOATING_POINT, 0, 1, 10, line+=lineHeight, 8));*/
      /*addChildElement(modulationNoise1ProgGui = new NumberBox("Modulation noise 1", freeVerbWrap->progenitor.getmodulationnoise1(), NumberBox::FLOATING_POINT, 0, 1e10, 10, line+=lineHeight, 8));
      addChildElement(modulationNoise2ProgGui = new NumberBox("Modulation noise 2", freeVerbWrap->progenitor.getmodulationnoise2(), NumberBox::FLOATING_POINT, 0, 1e10, 10, line+=lineHeight, 8));
      addChildElement(crossFeedProgGui = new NumberBox("Crossfeed", freeVerbWrap->progenitor.getcrossfeed(), NumberBox::FLOATING_POINT, 0, 1, 10, line+=lineHeight, 8));*/
      guiHeights[Reverb::Type::Progenitor] = line + lineHeight + 10;
      
      // lexicon224
      line = lineStart;
      addChildElement(diffusion1LexiconGui = new NumberBox("Diffusion 1", freeVerbWrap->lexicon224.getdiffusion1(), NumberBox::FLOATING_POINT, 0, 1, 10, line+=lineHeight, 8));
      addChildElement(diffusion2LexiconGui = new NumberBox("Diffusion 2", freeVerbWrap->lexicon224.getdiffusion2(), NumberBox::FLOATING_POINT, 0, 1, 10, line+=lineHeight, 8));
      
      addChildElement(dampLexiconGui = new NumberBox("Damp", freeVerbWrap->lexicon224.getdamp(), NumberBox::FLOATING_POINT, 0, freeVerbWrap->sampleRate/2, 10, line+=lineHeight, 8));
      freeVerbWrap->lexicon224.setoutputdamp(freeVerbWrap->lexicon224.getinputdamp());
      addChildElement(inputDampLexiconGui = new NumberBox("Input/output damp", freeVerbWrap->lexicon224.getinputdamp(), NumberBox::FLOATING_POINT, 0, freeVerbWrap->sampleRate/2, 10, line+=lineHeight, 8));
      //addChildElement(ouputDampLexiconGui = new NumberBox("Output damp", freeVerbWrap->lexicon224.getoutputdamp(), NumberBox::FLOATING_POINT, 0, freeVerbWrap->sampleRate/2, 10, line+=lineHeight, 8));
      addChildElement(rt60LexiconGui = new NumberBox("Rt 60", freeVerbWrap->lexicon224.getrt60(), NumberBox::FLOATING_POINT, 0, 1e10, 10, line+=lineHeight, 8));
      addChildElement(dcCutFrequencyLexiconGui = new NumberBox("DC cut frequency", freeVerbWrap->lexicon224.getdccutfreq(), NumberBox::FLOATING_POINT, 0, freeVerbWrap->sampleRate/2, 10, line+=lineHeight, 8));
      //addChildElement(spinLexiconGui = new NumberBox("Spin", freeVerbWrap->lexicon224.getspin(), NumberBox::FLOATING_POINT, 0, 1e10, 10, line+=lineHeight, 8));
      //addChildElement(spinDifferenceLexiconGui = new NumberBox("Spin difference", freeVerbWrap->lexicon224.getspindiff(), NumberBox::FLOATING_POINT, 0, 1e10, 10, line+=lineHeight, 8));
      //addChildElement(spinLimitLexiconGui = new NumberBox("Spin limit", freeVerbWrap->lexicon224.getspinlimit(), NumberBox::FLOATING_POINT, 0, 1e10, 10, line+=lineHeight, 8));
      //addChildElement(wanderLexiconGui = new NumberBox("Spin strength", freeVerbWrap->lexicon224.getwander(), NumberBox::FLOATING_POINT, 0, 1e10, 10, line+=lineHeight, 8));

      //addChildElement(modulationNoise1LexiconGui = new NumberBox("Modulation noise 1", freeVerbWrap->lexicon224.getmodulationnoise1(), NumberBox::FLOATING_POINT, 0, 1e10, 10, line+=lineHeight, 8));
      //addChildElement(modulationNoise2LexiconGui = new NumberBox("Modulation noise 2", freeVerbWrap->lexicon224.getmodulationnoise2(), NumberBox::FLOATING_POINT, 0, 1e10, 10, line+=lineHeight, 8));
      //addChildElement(autoDifferenceLexiconGui = new NumberBox("Auto difference", freeVerbWrap->lexicon224.getAutoDiff(), NumberBox::FLOATING_POINT, 0, 1e10, 10, line+=lineHeight, 8));
      guiHeights[Reverb::Type::Lexicon224] = line + lineHeight + 10;
      
      // zita
      line = lineStart;
      addChildElement(diffusionZitaGui = new NumberBox("Diffusion", freeVerbWrap->zita.getidiffusion1(), NumberBox::FLOATING_POINT, 0, 1, 10, line+=lineHeight, 8));
      addChildElement(rt60ZitaGui = new NumberBox("Rt60", freeVerbWrap->zita.getrt60(), NumberBox::FLOATING_POINT, 0, 1e10, 10, line+=lineHeight, 8));
      addChildElement(rt60LowZitaGui = new NumberBox("Rt60 factor low", freeVerbWrap->zita.getrt60_factor_low(), NumberBox::FLOATING_POINT, 0, 1, 10, line+=lineHeight, 8));
      addChildElement(rt60HighZitaGui = new NumberBox("Rt60 factor high", freeVerbWrap->zita.getrt60_factor_high(), NumberBox::FLOATING_POINT, 0, 1, 10, line+=lineHeight, 8));

      addChildElement(dcCutFrequencyZitaGui = new NumberBox("DC cut frequency", freeVerbWrap->zita.getdccutfreq(), NumberBox::FLOATING_POINT, 0, freeVerbWrap->sampleRate/2, 10, line+=lineHeight, 8));
      addChildElement(lfo1FrequencyZitaGui = new NumberBox("LFO 1 frequency", freeVerbWrap->zita.getlfo1freq(), NumberBox::FLOATING_POINT, 0, 20, 10, line+=lineHeight, 8));
      addChildElement(lfo2FrequencyZitaGui = new NumberBox("LFO 2 frequency", freeVerbWrap->zita.getlfo2freq(), NumberBox::FLOATING_POINT, 0, 20, 10, line+=lineHeight, 8));
      addChildElement(lfoFactorZitaGui = new NumberBox("LFO factor", freeVerbWrap->zita.getlfofactor(), NumberBox::FLOATING_POINT, 0, 1, 10, line+=lineHeight, 8));

      addChildElement(spinZitaGui = new NumberBox("Spin", freeVerbWrap->zita.getspin(), NumberBox::FLOATING_POINT, 0, 20, 10, line+=lineHeight, 8));
      addChildElement(spinFactorZitaGui = new NumberBox("Spin factor", freeVerbWrap->zita.getspinfactor(), NumberBox::FLOATING_POINT, 0, 10, 10, line+=lineHeight, 8));
      //addChildElement(wanderZitaGui = new NumberBox("Spin strength", freeVerbWrap->zita.getwander(), NumberBox::FLOATING_POINT, 0, 1e10, 10, line+=lineHeight, 8));
      
      addChildElement(xoverLowZitaGui = new NumberBox("X-over low", freeVerbWrap->zita.getxover_low(), NumberBox::FLOATING_POINT, 0, 10000, 10, line+=lineHeight, 8));
      addChildElement(xoverHighZitaGui = new NumberBox("X-over high", freeVerbWrap->zita.getxover_high(), NumberBox::FLOATING_POINT, 0, 10000, 10, line+=lineHeight, 8));

      addChildElement(apFeedbackZitaGui = new NumberBox("AP feedback", freeVerbWrap->zita.getapfeedback(), NumberBox::FLOATING_POINT, 0, 1, 10, line+=lineHeight, 8));
      addChildElement(loopDampZitaGui = new NumberBox("Loop damp", freeVerbWrap->zita.getloopdamp(), NumberBox::FLOATING_POINT, 0, freeVerbWrap->sampleRate/2-1, 10, line+=lineHeight, 8));
      addChildElement(outputLpfZitaGui = new NumberBox("Lowpass frequency", freeVerbWrap->zita.getoutputlpf(), NumberBox::FLOATING_POINT, 0, freeVerbWrap->sampleRate/2, 10, line+=lineHeight, 8));
      addChildElement(outputHpfZitaGui = new NumberBox("Highpass frequency", freeVerbWrap->zita.getoutputhpf(), NumberBox::FLOATING_POINT, 0, freeVerbWrap->sampleRate/2, 10, line+=lineHeight, 8));
      guiHeights[Reverb::Type::Zita] = line + lineHeight + 10;
      
      update();
    }

    void update() {
      
      setSize(300, guiHeights[freeVerbWrap->type]);
      
      isActiveGui->setValue(freeVerbWrap->isActive);
      typeGui->setValue(freeVerbWrap->type);
      
      if(freeVerbWrap->type == Reverb::Type::FreeVerb) {
        wetLevelFreeVerbGui->setValue(freeVerbWrap->freeverb.getwet());
        dryLevelFreeVerbGui->setValue(freeVerbWrap->freeverb.getdry());

        roomSizeFreeVerbGui->setValue(freeVerbWrap->freeverb.getroomsize());
        dampFreeVerbGui->setValue(freeVerbWrap->freeverb.getdamp());
        widthFreeVerbGui->setValue(freeVerbWrap->freeverb.getwidth());
      }
      
      if(freeVerbWrap->type == Reverb::Type::CCRMA) {
        feedbackCcrmaGui->setValue(freeVerbWrap->ccrma.getfeedback());
        apFeedbackCcrmaGui->setValue(freeVerbWrap->ccrma.getapfeedback());
        dampCcrmaGui->setValue(freeVerbWrap->ccrma.getdamp());
      }
      
      if(freeVerbWrap->type == Reverb::Type::Progenitor) {
        diffusion1ProgGui->setValue(freeVerbWrap->progenitor.getdiffusion1());
        diffusion2ProgGui->setValue(freeVerbWrap->progenitor.getdiffusion2());
        diffusion3ProgGui->setValue(freeVerbWrap->progenitor.getdiffusion3());
        diffusion4ProgGui->setValue(freeVerbWrap->progenitor.getdiffusion4());
        dampProgGui->setValue(freeVerbWrap->progenitor.getdamp());
        //damp2ProgGui->setValue(freeVerbWrap->progenitor.getdamp2());
        inputDampProgGui->setValue(freeVerbWrap->progenitor.getinputdamp());
        ouputDampProgGui->setValue(freeVerbWrap->progenitor.getoutputdamp());
        ouputDampBandwidthProgGui->setValue(freeVerbWrap->progenitor.getoutputdampbw());
        spinProgGui->setValue(freeVerbWrap->progenitor.getspin());
        spinLimitProgGui->setValue(freeVerbWrap->progenitor.getspinlimit());
        //wanderProgGui->setValue(freeVerbWrap->progenitor.getwander());
        //spin2ProgGui->setValue(freeVerbWrap->progenitor.getspin2());
        //spin2LimitProgGui->setValue(freeVerbWrap->progenitor.getspinlimit2());
        /*wander2ProgGui->setValue(freeVerbWrap->progenitor.getwander2());
        spin2WanderProgGui->setValue(freeVerbWrap->progenitor.getspin2wander());*/
        decay0ProgGui->setValue(freeVerbWrap->progenitor.getdecay0());
        decay1ProgGui->setValue(freeVerbWrap->progenitor.getdecay1());
        decay2ProgGui->setValue(freeVerbWrap->progenitor.getdecay2());
        decay3ProgGui->setValue(freeVerbWrap->progenitor.getdecay3());
        /*bassBandwidthProgGui->setValue(freeVerbWrap->progenitor.getbassbw());
        bassBoostProgGui->setValue(freeVerbWrap->progenitor.getbassboost());
        idiffusionProgGui->setValue(freeVerbWrap->progenitor.getidiffusion1());
        odiffusionProgGui->setValue(freeVerbWrap->progenitor.getodiffusion1());
        modulationNoise1ProgGui->setValue(freeVerbWrap->progenitor.getmodulationnoise1());
        modulationNoise2ProgGui->setValue(freeVerbWrap->progenitor.getmodulationnoise2());
        crossFeedProgGui->setValue(freeVerbWrap->progenitor.getcrossfeed());*/
      }
      if(freeVerbWrap->type == Reverb::Type::Lexicon224) {
        diffusion1LexiconGui->setValue(freeVerbWrap->lexicon224.getdiffusion1());
        diffusion2LexiconGui->setValue(freeVerbWrap->lexicon224.getdiffusion2());
        dampLexiconGui->setValue(freeVerbWrap->lexicon224.getdamp());
        inputDampLexiconGui->setValue(freeVerbWrap->lexicon224.getinputdamp());
        //ouputDampLexiconGui->setValue(freeVerbWrap->lexicon224.getoutputdamp());
        rt60LexiconGui->setValue(freeVerbWrap->lexicon224.getrt60());
        dcCutFrequencyLexiconGui->setValue(freeVerbWrap->lexicon224.getdccutfreq());
        //spinLexiconGui->setValue(freeVerbWrap->lexicon224.getspin());
        //spinLimitLexiconGui->setValue(freeVerbWrap->lexicon224.getspinlimit());
        /*spinDifferenceLexiconGui->setValue(freeVerbWrap->lexicon224.getspindiff());
        wanderLexiconGui->setValue(freeVerbWrap->lexicon224.getwander());
        modulationNoise1LexiconGui->setValue(freeVerbWrap->lexicon224.getmodulationnoise1());
        modulationNoise2LexiconGui->setValue(freeVerbWrap->lexicon224.getmodulationnoise2());
        autoDifferenceLexiconGui->setValue(freeVerbWrap->lexicon224.getAutoDiff());*/
      }
      if(freeVerbWrap->type == Reverb::Type::Zita) {
        diffusionZitaGui->setValue(freeVerbWrap->zita.getidiffusion1());
        rt60ZitaGui->setValue(freeVerbWrap->zita.getrt60());
        rt60LowZitaGui->setValue(freeVerbWrap->zita.getrt60_factor_low());
        rt60HighZitaGui->setValue(freeVerbWrap->zita.getrt60_factor_high());
        dcCutFrequencyZitaGui->setValue(freeVerbWrap->zita.getdccutfreq());
        lfo1FrequencyZitaGui->setValue(freeVerbWrap->zita.getlfo1freq());
        lfo2FrequencyZitaGui->setValue(freeVerbWrap->zita.getlfo2freq());
        lfoFactorZitaGui->setValue(freeVerbWrap->zita.getlfofactor());
        spinZitaGui->setValue(freeVerbWrap->zita.getspin());
        spinFactorZitaGui->setValue(freeVerbWrap->zita.getspinfactor());
        //wanderZitaGui->setValue(freeVerbWrap->zita.getwander());
        xoverLowZitaGui->setValue(freeVerbWrap->zita.getxover_low());
        xoverHighZitaGui->setValue(freeVerbWrap->zita.getxover_high());
        apFeedbackZitaGui->setValue(freeVerbWrap->zita.getapfeedback());
        loopDampZitaGui->setValue(freeVerbWrap->zita.getloopdamp());
        outputLpfZitaGui->setValue(freeVerbWrap->zita.getoutputlpf());
        outputHpfZitaGui->setValue(freeVerbWrap->zita.getoutputhpf());
      }

      wetLevelFreeVerbGui->setVisible(freeVerbWrap->type == Reverb::Type::FreeVerb);
      dryLevelFreeVerbGui->setVisible(freeVerbWrap->type == Reverb::Type::FreeVerb);
      roomSizeFreeVerbGui->setVisible(freeVerbWrap->type == Reverb::Type::FreeVerb);
      dampFreeVerbGui->setVisible(freeVerbWrap->type == Reverb::Type::FreeVerb);
      widthFreeVerbGui->setVisible(freeVerbWrap->type == Reverb::Type::FreeVerb);

      feedbackCcrmaGui->setVisible(freeVerbWrap->type == Reverb::Type::CCRMA);
      apFeedbackCcrmaGui->setVisible(freeVerbWrap->type == Reverb::Type::CCRMA);
      dampCcrmaGui->setVisible(freeVerbWrap->type == Reverb::Type::CCRMA);

      diffusion1ProgGui->setVisible(freeVerbWrap->type == Reverb::Type::Progenitor);
      diffusion2ProgGui->setVisible(freeVerbWrap->type == Reverb::Type::Progenitor);
      diffusion3ProgGui->setVisible(freeVerbWrap->type == Reverb::Type::Progenitor);
      diffusion4ProgGui->setVisible(freeVerbWrap->type == Reverb::Type::Progenitor);
      dampProgGui->setVisible(freeVerbWrap->type == Reverb::Type::Progenitor);
      //damp2ProgGui->setVisible(freeVerbWrap->type == Reverb::Type::Progenitor);
      inputDampProgGui->setVisible(freeVerbWrap->type == Reverb::Type::Progenitor);
      ouputDampProgGui->setVisible(freeVerbWrap->type == Reverb::Type::Progenitor);
      ouputDampBandwidthProgGui->setVisible(freeVerbWrap->type == Reverb::Type::Progenitor);
      spinProgGui->setVisible(freeVerbWrap->type == Reverb::Type::Progenitor);
      spinLimitProgGui->setVisible(freeVerbWrap->type == Reverb::Type::Progenitor);
      //wanderProgGui->setVisible(freeVerbWrap->type == Reverb::Type::Progenitor);
      //spin2ProgGui->setVisible(freeVerbWrap->type == Reverb::Type::Progenitor);
      //spin2LimitProgGui->setVisible(freeVerbWrap->type == Reverb::Type::Progenitor);
      /*wander2ProgGui->setVisible(freeVerbWrap->type == Reverb::Type::Progenitor);
      spin2WanderProgGui->setVisible(freeVerbWrap->type == Reverb::Type::Progenitor);*/
      decay0ProgGui->setVisible(freeVerbWrap->type == Reverb::Type::Progenitor);
      decay1ProgGui->setVisible(freeVerbWrap->type == Reverb::Type::Progenitor);
      decay2ProgGui->setVisible(freeVerbWrap->type == Reverb::Type::Progenitor);
      decay3ProgGui->setVisible(freeVerbWrap->type == Reverb::Type::Progenitor);
      /*bassBandwidthProgGui->setVisible(freeVerbWrap->type == Reverb::Type::Progenitor);
      bassBoostProgGui->setVisible(freeVerbWrap->type == Reverb::Type::Progenitor);
      idiffusionProgGui->setVisible(freeVerbWrap->type == Reverb::Type::Progenitor);
      odiffusionProgGui->setVisible(freeVerbWrap->type == Reverb::Type::Progenitor);
      modulationNoise1ProgGui->setVisible(freeVerbWrap->type == Reverb::Type::Progenitor);
      modulationNoise2ProgGui->setVisible(freeVerbWrap->type == Reverb::Type::Progenitor);
      crossFeedProgGui->setVisible(freeVerbWrap->type == Reverb::Type::Progenitor);*/
    
      diffusion1LexiconGui->setVisible(freeVerbWrap->type == Reverb::Type::Lexicon224);
      diffusion2LexiconGui->setVisible(freeVerbWrap->type == Reverb::Type::Lexicon224);
      dampLexiconGui->setVisible(freeVerbWrap->type == Reverb::Type::Lexicon224);
      inputDampLexiconGui->setVisible(freeVerbWrap->type == Reverb::Type::Lexicon224);
      //ouputDampLexiconGui->setVisible(freeVerbWrap->type == Reverb::Type::Lexicon224);
      rt60LexiconGui->setVisible(freeVerbWrap->type == Reverb::Type::Lexicon224);
      dcCutFrequencyLexiconGui->setVisible(freeVerbWrap->type == Reverb::Type::Lexicon224);
      /*spinLexiconGui->setVisible(freeVerbWrap->type == Reverb::Type::Lexicon224);
      spinLimitLexiconGui->setVisible(freeVerbWrap->type == Reverb::Type::Lexicon224);
      spinDifferenceLexiconGui->setVisible(freeVerbWrap->type == Reverb::Type::Lexicon224);
      wanderLexiconGui->setVisible(freeVerbWrap->type == Reverb::Type::Lexicon224);
      modulationNoise1LexiconGui->setVisible(freeVerbWrap->type == Reverb::Type::Lexicon224);
      modulationNoise2LexiconGui->setVisible(freeVerbWrap->type == Reverb::Type::Lexicon224);
      autoDifferenceLexiconGui->setVisible(freeVerbWrap->type == Reverb::Type::Lexicon224);*/

      diffusionZitaGui->setVisible(freeVerbWrap->type == Reverb::Type::Zita);
      rt60ZitaGui->setVisible(freeVerbWrap->type == Reverb::Type::Zita);
      rt60LowZitaGui->setVisible(freeVerbWrap->type == Reverb::Type::Zita);
      rt60HighZitaGui->setVisible(freeVerbWrap->type == Reverb::Type::Zita);
      dcCutFrequencyZitaGui->setVisible(freeVerbWrap->type == Reverb::Type::Zita);
      lfo1FrequencyZitaGui->setVisible(freeVerbWrap->type == Reverb::Type::Zita);
      lfo2FrequencyZitaGui->setVisible(freeVerbWrap->type == Reverb::Type::Zita);
      lfoFactorZitaGui->setVisible(freeVerbWrap->type == Reverb::Type::Zita);
      spinZitaGui->setVisible(freeVerbWrap->type == Reverb::Type::Zita);
      spinFactorZitaGui->setVisible(freeVerbWrap->type == Reverb::Type::Zita);
      //wanderZitaGui->setVisible(freeVerbWrap->type == Reverb::Type::Zita);
      xoverLowZitaGui->setVisible(freeVerbWrap->type == Reverb::Type::Zita);
      xoverHighZitaGui->setVisible(freeVerbWrap->type == Reverb::Type::Zita);
      apFeedbackZitaGui->setVisible(freeVerbWrap->type == Reverb::Type::Zita);
      loopDampZitaGui->setVisible(freeVerbWrap->type == Reverb::Type::Zita);
      outputLpfZitaGui->setVisible(freeVerbWrap->type == Reverb::Type::Zita);
      outputHpfZitaGui->setVisible(freeVerbWrap->type == Reverb::Type::Zita);
      
    }

    struct FreeVerbPanelListener : public GuiEventListener {
      FreeVerbPanel *freeVerbPanel;
      FreeVerbPanelListener(FreeVerbPanel *freeVerbPanel) {
        this->freeVerbPanel = freeVerbPanel;
      }

      void onValueChange(GuiElement *guiElement) {
        if(guiElement == freeVerbPanel->isActiveGui) {
          guiElement->getValue((void*)&freeVerbPanel->freeVerbWrap->isActive);
          freeVerbPanel->freeVerbWrap->reset();
        }
        if(guiElement == freeVerbPanel->typeGui) {
          guiElement->getValue((void*)&freeVerbPanel->freeVerbWrap->type);
          freeVerbPanel->freeVerbWrap->reset();
          freeVerbPanel->update();
        }
        if(guiElement == freeVerbPanel->masterWetLevelGui) {
          guiElement->getValue((void*)&freeVerbPanel->freeVerbWrap->masterWetLevel);
        }
        if(guiElement == freeVerbPanel->wetLevelFreeVerbGui) {
          double value;
          guiElement->getValue((void*)&value);
          freeVerbPanel->freeVerbWrap->freeverb.setwet(value);
        }
        if(guiElement == freeVerbPanel->dryLevelFreeVerbGui) {
          double value;
          guiElement->getValue((void*)&value);
          freeVerbPanel->freeVerbWrap->freeverb.setdry(value);
        }        
        
        if(guiElement == freeVerbPanel->roomSizeFreeVerbGui) {
          double value;
          guiElement->getValue((void*)&value);
          freeVerbPanel->freeVerbWrap->freeverb.setroomsize(value);
        }
        if(guiElement == freeVerbPanel->dampFreeVerbGui) {
          double value;
          guiElement->getValue((void*)&value);
          freeVerbPanel->freeVerbWrap->freeverb.setdamp(value);
        }
        if(guiElement == freeVerbPanel->widthFreeVerbGui) {
          double value;
          guiElement->getValue((void*)&value);
          freeVerbPanel->freeVerbWrap->freeverb.setwidth(value);
        }
        
        if(guiElement == freeVerbPanel->feedbackCcrmaGui) {
          double value;
          guiElement->getValue((void*)&value);
          freeVerbPanel->freeVerbWrap->ccrma.setfeedback(value);
          freeVerbPanel->freeVerbWrap->ccrma.mute();
        }        
        if(guiElement == freeVerbPanel->apFeedbackCcrmaGui) {
          double value;
          guiElement->getValue((void*)&value);
          freeVerbPanel->freeVerbWrap->ccrma.setapfeedback(value);
        }
        if(guiElement == freeVerbPanel->dampCcrmaGui) {
          double value;
          guiElement->getValue((void*)&value);
          freeVerbPanel->freeVerbWrap->ccrma.setdamp(value);
        }
        
        if(guiElement == freeVerbPanel->diffusion1ProgGui) {
          double value;
          guiElement->getValue((void*)&value);
          freeVerbPanel->freeVerbWrap->progenitor.setdiffusion1(value);
        }
        if(guiElement == freeVerbPanel->diffusion2ProgGui) {
          double value;
          guiElement->getValue((void*)&value);
          freeVerbPanel->freeVerbWrap->progenitor.setdiffusion2(value);
        }
        if(guiElement == freeVerbPanel->diffusion3ProgGui) {
          double value;
          guiElement->getValue((void*)&value);
          freeVerbPanel->freeVerbWrap->progenitor.setdiffusion3(value);
        }
        if(guiElement == freeVerbPanel->diffusion4ProgGui) {
          double value;
          guiElement->getValue((void*)&value);
          freeVerbPanel->freeVerbWrap->progenitor.setdiffusion4(value);
        }
        
        if(guiElement == freeVerbPanel->dampProgGui) {
          double value;
          guiElement->getValue((void*)&value);
          freeVerbPanel->freeVerbWrap->progenitor.setdamp(value);
        }
        if(guiElement == freeVerbPanel->damp2ProgGui) {
          double value;
          guiElement->getValue((void*)&value);
          freeVerbPanel->freeVerbWrap->progenitor.setdamp2(value);
        }
        if(guiElement == freeVerbPanel->inputDampProgGui) {
          double value;
          guiElement->getValue((void*)&value);
          freeVerbPanel->freeVerbWrap->progenitor.setinputdamp(value);
        }
        if(guiElement == freeVerbPanel->ouputDampProgGui) {
          double value;
          guiElement->getValue((void*)&value);
          freeVerbPanel->freeVerbWrap->progenitor.setoutputdamp(value);
        }
        if(guiElement == freeVerbPanel->ouputDampBandwidthProgGui) {
          double value;
          guiElement->getValue((void*)&value);
          freeVerbPanel->freeVerbWrap->progenitor.setoutputdampbw(value);
        }
        
        if(guiElement == freeVerbPanel->spinProgGui) {
          double value;
          guiElement->getValue((void*)&value);
          freeVerbPanel->freeVerbWrap->progenitor.setspin(value);
          if(freeVerbPanel->freeVerbWrap->progenitor.getspinlimit() > value) {
            freeVerbPanel->freeVerbWrap->progenitor.setspinlimit(value);
            freeVerbPanel->spinLimitProgGui->setValue(value);
          }
        }
        if(guiElement == freeVerbPanel->spinLimitProgGui) {
          double value;
          guiElement->getValue((void*)&value);
          freeVerbPanel->freeVerbWrap->progenitor.setspinlimit(value);
          if(freeVerbPanel->freeVerbWrap->progenitor.getspin() < value) {
            freeVerbPanel->freeVerbWrap->progenitor.setspin(value);
            freeVerbPanel->spinProgGui->setValue(value);
          }
        }
        if(guiElement == freeVerbPanel->wanderProgGui) {
          double value;
          guiElement->getValue((void*)&value);
          freeVerbPanel->freeVerbWrap->progenitor.setwander(value);
        }
        if(guiElement == freeVerbPanel->spin2ProgGui) {
          double value;
          guiElement->getValue((void*)&value);
          freeVerbPanel->freeVerbWrap->progenitor.setspin2(value);
          if(freeVerbPanel->freeVerbWrap->progenitor.getspinlimit2() > value) {
            freeVerbPanel->freeVerbWrap->progenitor.setspinlimit2(value);
            freeVerbPanel->spin2LimitProgGui->setValue(value);
          }
        }
        if(guiElement == freeVerbPanel->spin2LimitProgGui) {
          double value;
          guiElement->getValue((void*)&value);
          freeVerbPanel->freeVerbWrap->progenitor.setspinlimit2(value);
          if(freeVerbPanel->freeVerbWrap->progenitor.getspin2() < value) {
            freeVerbPanel->freeVerbWrap->progenitor.setspin2(value);
            freeVerbPanel->spin2ProgGui->setValue(value);
          }
        }
        if(guiElement == freeVerbPanel->wander2ProgGui) {
          double value;
          guiElement->getValue((void*)&value);
          freeVerbPanel->freeVerbWrap->progenitor.setwander2(value);
        }
        if(guiElement == freeVerbPanel->spin2WanderProgGui) {
          double value;
          guiElement->getValue((void*)&value);
          freeVerbPanel->freeVerbWrap->progenitor.setspin2wander(value);
        }
        
        if(guiElement == freeVerbPanel->decay0ProgGui) {
          double value;
          guiElement->getValue((void*)&value);
          freeVerbPanel->freeVerbWrap->progenitor.setdecay0(value);
        }
        if(guiElement == freeVerbPanel->decay1ProgGui) {
          double value;
          guiElement->getValue((void*)&value);
          freeVerbPanel->freeVerbWrap->progenitor.setdecay1(value);
        }
        if(guiElement == freeVerbPanel->decay2ProgGui) {
          double value;
          guiElement->getValue((void*)&value);
          freeVerbPanel->freeVerbWrap->progenitor.setdecay2(value);
        }
        if(guiElement == freeVerbPanel->decay3ProgGui) {
          double value;
          guiElement->getValue((void*)&value);
          freeVerbPanel->freeVerbWrap->progenitor.setdecay3(value);
        }
        
        if(guiElement == freeVerbPanel->bassBandwidthProgGui) {
          double value;
          guiElement->getValue((void*)&value);
          freeVerbPanel->freeVerbWrap->progenitor.setbassbw(value);
        }
        if(guiElement == freeVerbPanel->bassBoostProgGui) {
          double value;
          guiElement->getValue((void*)&value);
          freeVerbPanel->freeVerbWrap->progenitor.setbassboost(value);
        }
        if(guiElement == freeVerbPanel->idiffusionProgGui) {
          double value;
          guiElement->getValue((void*)&value);
          freeVerbPanel->freeVerbWrap->progenitor.setidiffusion1(value);
        }
        if(guiElement == freeVerbPanel->odiffusionProgGui) {
          double value;
          guiElement->getValue((void*)&value);
          freeVerbPanel->freeVerbWrap->progenitor.setodiffusion1(value);
        }
        if(guiElement == freeVerbPanel->modulationNoise1ProgGui) {
          double value;
          guiElement->getValue((void*)&value);
          freeVerbPanel->freeVerbWrap->progenitor.setmodulationnoise1(value);
        }
        if(guiElement == freeVerbPanel->modulationNoise2ProgGui) {
          double value;
          guiElement->getValue((void*)&value);
          freeVerbPanel->freeVerbWrap->progenitor.setmodulationnoise2(value);
        }
        if(guiElement == freeVerbPanel->crossFeedProgGui) {
          double value;
          guiElement->getValue((void*)&value);
          freeVerbPanel->freeVerbWrap->progenitor.setcrossfeed(value);
        }
        
        if(guiElement == freeVerbPanel->diffusion1LexiconGui) {
          double value;
          guiElement->getValue((void*)&value);
          freeVerbPanel->freeVerbWrap->lexicon224.setdiffusion1(value);
        }
        if(guiElement == freeVerbPanel->diffusion2LexiconGui) {
          double value;
          guiElement->getValue((void*)&value);
          freeVerbPanel->freeVerbWrap->lexicon224.setdiffusion2(value);
        }
        if(guiElement == freeVerbPanel->dampLexiconGui) {
          double value;
          guiElement->getValue((void*)&value);
          freeVerbPanel->freeVerbWrap->lexicon224.setdamp(value);
        }
        if(guiElement == freeVerbPanel->inputDampLexiconGui) {
          double value;
          guiElement->getValue((void*)&value);
          freeVerbPanel->freeVerbWrap->lexicon224.setinputdamp(value);
          freeVerbPanel->freeVerbWrap->lexicon224.setoutputdamp(value);
        }
        if(guiElement == freeVerbPanel->ouputDampLexiconGui) {
          double value;
          guiElement->getValue((void*)&value);
          freeVerbPanel->freeVerbWrap->lexicon224.setoutputdamp(value);
        }
        if(guiElement == freeVerbPanel->rt60LexiconGui) {
          double value;
          guiElement->getValue((void*)&value);
          freeVerbPanel->freeVerbWrap->lexicon224.setrt60(value);
        }
        if(guiElement == freeVerbPanel->dcCutFrequencyLexiconGui) {
          double value;
          guiElement->getValue((void*)&value);
          freeVerbPanel->freeVerbWrap->lexicon224.setdccutfreq(value);
        }
        
        if(guiElement == freeVerbPanel->spinLexiconGui) {
          double value;
          guiElement->getValue((void*)&value);
          freeVerbPanel->freeVerbWrap->lexicon224.setspin(value);
        }
        if(guiElement == freeVerbPanel->spinDifferenceLexiconGui) {
          double value;
          guiElement->getValue((void*)&value);
          freeVerbPanel->freeVerbWrap->lexicon224.setspindiff(value);
        }
        if(guiElement == freeVerbPanel->spinLimitLexiconGui) {
          double value;
          guiElement->getValue((void*)&value);
          freeVerbPanel->freeVerbWrap->lexicon224.setspinlimit(value);
        }
        if(guiElement == freeVerbPanel->wanderLexiconGui) {
          double value;
          guiElement->getValue((void*)&value);
          freeVerbPanel->freeVerbWrap->lexicon224.setwander(value);
        }

        if(guiElement == freeVerbPanel->modulationNoise1LexiconGui) {
          double value;
          guiElement->getValue((void*)&value);
          freeVerbPanel->freeVerbWrap->lexicon224.setmodulationnoise1(value);
        }
        if(guiElement == freeVerbPanel->modulationNoise2LexiconGui) {
          double value;
          guiElement->getValue((void*)&value);
          freeVerbPanel->freeVerbWrap->lexicon224.setmodulationnoise2(value);
        }
        if(guiElement == freeVerbPanel->autoDifferenceLexiconGui) {
          double value;
          guiElement->getValue((void*)&value);
          freeVerbPanel->freeVerbWrap->lexicon224.setAutoDiff(value);
        }

        if(guiElement == freeVerbPanel->diffusionZitaGui) {
          double value;
          guiElement->getValue((void*)&value);
          freeVerbPanel->freeVerbWrap->zita.setidiffusion1(value);
        }
        if(guiElement == freeVerbPanel->rt60ZitaGui) {
          double value;
          guiElement->getValue((void*)&value);
          freeVerbPanel->freeVerbWrap->zita.setrt60(value);
        }
        if(guiElement == freeVerbPanel->rt60LowZitaGui) {
          double value;
          guiElement->getValue((void*)&value);
          freeVerbPanel->freeVerbWrap->zita.setrt60_factor_low(value);
        }
        if(guiElement == freeVerbPanel->rt60HighZitaGui) {
          double value;
          guiElement->getValue((void*)&value);
          freeVerbPanel->freeVerbWrap->zita.setrt60_factor_high(value);
        }
        
        if(guiElement == freeVerbPanel->dcCutFrequencyZitaGui) {
          double value;
          guiElement->getValue((void*)&value);
          freeVerbPanel->freeVerbWrap->zita.setdccutfreq(value);
        }
        if(guiElement == freeVerbPanel->lfo1FrequencyZitaGui) {
          double value;
          guiElement->getValue((void*)&value);
          freeVerbPanel->freeVerbWrap->zita.setlfo1freq(value);
        }
        if(guiElement == freeVerbPanel->lfo2FrequencyZitaGui) {
          double value;
          guiElement->getValue((void*)&value);
          freeVerbPanel->freeVerbWrap->zita.setlfo2freq(value);
        }
        if(guiElement == freeVerbPanel->lfoFactorZitaGui) {
          double value;
          guiElement->getValue((void*)&value);
          freeVerbPanel->freeVerbWrap->zita.setlfofactor(value);
        }
        
        if(guiElement == freeVerbPanel->spinZitaGui) {
          double value;
          guiElement->getValue((void*)&value);
          freeVerbPanel->freeVerbWrap->zita.setspin(value);
        }
        if(guiElement == freeVerbPanel->spinFactorZitaGui) {
          double value;
          guiElement->getValue((void*)&value);
          freeVerbPanel->freeVerbWrap->zita.setspinfactor(value);
        }
        if(guiElement == freeVerbPanel->wanderZitaGui) {
          double value;
          guiElement->getValue((void*)&value);
          freeVerbPanel->freeVerbWrap->zita.setwander(value);
        }
        
        if(guiElement == freeVerbPanel->xoverLowZitaGui) {
          double value;
          guiElement->getValue((void*)&value);
          freeVerbPanel->freeVerbWrap->zita.setxover_low(value);
          if(freeVerbPanel->freeVerbWrap->zita.getxover_high() < value) {
            freeVerbPanel->xoverHighZitaGui->setValue(value);
            //freeVerbPanel->freeVerbWrap->zita.setxover_high(value);
          }
        }
        if(guiElement == freeVerbPanel->xoverHighZitaGui) {
          double value;
          guiElement->getValue((void*)&value);
          freeVerbPanel->freeVerbWrap->zita.setxover_high(value);
          if(freeVerbPanel->freeVerbWrap->zita.getxover_low() > value) {
            freeVerbPanel->xoverLowZitaGui->setValue(value);
            //freeVerbPanel->freeVerbWrap->zita.setxover_low(value);
          }
        }
        
        if(guiElement == freeVerbPanel->apFeedbackZitaGui) {
          double value;
          guiElement->getValue((void*)&value);
          freeVerbPanel->freeVerbWrap->zita.setapfeedback(value);
        }
        if(guiElement == freeVerbPanel->loopDampZitaGui) {
          double value;
          guiElement->getValue((void*)&value);
          freeVerbPanel->freeVerbWrap->zita.setloopdamp(value);
        }
        if(guiElement == freeVerbPanel->outputLpfZitaGui) {
          double value;
          guiElement->getValue((void*)&value);
          freeVerbPanel->freeVerbWrap->zita.setoutputlpf(value);
        }
        if(guiElement == freeVerbPanel->outputHpfZitaGui) {
          double value;
          guiElement->getValue((void*)&value);
          freeVerbPanel->freeVerbWrap->zita.setoutputhpf(value);
        }
      }
    };
  };
  FreeVerbPanel *freeVerbPanel = NULL;
  
  Panel *getPanel() {
    return freeVerbPanel;
  }

  Panel *addPanel(GuiElement *parentGuiElement, const std::string &title = "") {
    return freeVerbPanel = new FreeVerbPanel(this, parentGuiElement);
  }
  
  void removePanel(GuiElement *parentGuiElement) {
    PanelInterface::removePanel(parentGuiElement);

    if(freeVerbPanel) {
      parentGuiElement->deleteChildElement(freeVerbPanel);
    }
  }

  void updatePanel() {
    if(freeVerbPanel) {
      freeVerbPanel->update();
    }
  }
  
  
  
  
};