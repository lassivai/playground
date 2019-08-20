#pragma once
#include "postprocessingeffect.h"
#include "../delayline.h"
#include <freeverb/revmodel.hpp>
#include <freeverb/nrevb.hpp>
#include <freeverb/strev.hpp>
#include <freeverb/zrev2.hpp>
#include <freeverb/progenitor2.hpp>
#include "../../external/MVerb.h"

static std::vector<Preset> reverbPresets;


struct Reverb : public PostProcessingEffect {
  
  enum Type { FreeVerb, CCRMA, Lexicon224, Progenitor, Zita, Mverb, numTypes };
  const std::vector<std::string> typeNames = { "FreeVerb", "CCRMA", "Lexicon224", "Progenitor", "Zita", "MVerb" };
  
/*  segfault bug in Zita. Crashed when changed synth0028 (with Lexicon224 reverb) to synth0027b (with Zita reverb)

#0  fv3::noisegen_pink_frac_::process (this=<optimized out>) at ../freeverb/efilter_t.hpp:252
#1  fv3::noisegen_pink_frac_::operator() (this=<optimized out>) at ../freeverb/efilter_t.hpp:254
#2  fv3::strev_::processreplace (this=0x55b43d83c218, inputL=0x7fea14000b60, inputR=0x7fea14000ba0, outputL=0x7fea14000c00, 
    outputR=0x7fea14000c40, numsamples=1) at ../freeverb/strev.cpp:109
#3  0x000055b3aefd5c53 in Reverb::apply(Vec2d&) ()
#4  0x000055b3aeff089f in Instrument::applyFilters() ()
#5  0x000055b3af00e670 in Synth::synthStereo(Vec2d&, Vec2d const&, double, double, std::vector<Note, std::allocator<Note> >&, int, std::vector<Note, std::allocator<Note> >&, int, DelayLine&, std::vector<Instrument*, std::allocator<Instrument*> >&) ()
#6  0x000055b3af00eb0b in Synth::streamCallbackSynth(void const*, void*, unsigned long, PaStreamCallbackTimeInfo const*, unsigned long, void*) ()
*/
  
  double sampleRate;
  
  Type type = FreeVerb;
  
  fv3::revmodel_ freeverb;
  fv3::zrev2_ zita;
  fv3::nrevb_ ccrma;
  fv3::strev_ lexicon224;
  fv3::progenitor2_ progenitor;
  MVerb<double> mverb;
  double **arr;
  
  double gain = 1.0;
  

  Reverb(double sampleRate) : PostProcessingEffect("Reverb")  {
    this->sampleRate = sampleRate;
    freeverb.setSampleRate(sampleRate);
    zita.setSampleRate(sampleRate);
    ccrma.setSampleRate(sampleRate);
    lexicon224.setSampleRate(sampleRate);
    progenitor.setSampleRate(sampleRate);
    mverb.setSampleRate(sampleRate);
    arr = new double*[2] {new double[1], new double[1]};
    this->presets = &reverbPresets;
  }
  
  Reverb(Reverb &reverb) : PostProcessingEffect("Reverb")  {
    this->sampleRate = reverb.sampleRate;
    /*freeverb.setSampleRate(reverb.freeverb.getSampleRate());
    freeverb.setwet(reverb.freeverb.getwet());
    freeverb.setdry(reverb.freeverb.getdry());
    freeverb.setroomsize(reverb.freeverb.getroomsize());
    freeverb.setdamp(reverb.freeverb.getdamp());
    freeverb.setwidth(reverb.freeverb.getwidth());*/
    this->presets = &reverbPresets;
  }
  Reverb(double sampleRate, Reverb &reverb) : PostProcessingEffect("Reverb")  {
    this->sampleRate = sampleRate;
    this->presets = &reverbPresets;
    /*if(reverb->type == Reverb::Type::FreeVerb) {
      wetLevelFreeVerbGui->setValue(reverb->freeverb.getwet());
      dryLevelFreeVerbGui->setValue(reverb->freeverb.getdry());

      roomSizeFreeVerbGui->setValue(reverb->freeverb.getroomsize());
      dampFreeVerbGui->setValue(reverb->freeverb.getdamp());
      widthFreeVerbGui->setValue(reverb->freeverb.getwidth());
    }
    
    if(reverb->type == Reverb::Type::CCRMA) {
      feedbackCcrmaGui->setValue(reverb->ccrma.getfeedback());
      apFeedbackCcrmaGui->setValue(reverb->ccrma.getapfeedback());
      dampCcrmaGui->setValue(reverb->ccrma.getdamp());
    }
    
    if(reverb->type == Reverb::Type::Progenitor) {
      diffusion1ProgGui->setValue(reverb->progenitor.getdiffusion1());
      diffusion2ProgGui->setValue(reverb->progenitor.getdiffusion2());
      diffusion3ProgGui->setValue(reverb->progenitor.getdiffusion3());
      diffusion4ProgGui->setValue(reverb->progenitor.getdiffusion4());
      dampProgGui->setValue(reverb->progenitor.getdamp());
      //damp2ProgGui->setValue(reverb->progenitor.getdamp2());
      inputDampProgGui->setValue(reverb->progenitor.getinputdamp());
      ouputDampProgGui->setValue(reverb->progenitor.getoutputdamp());
      ouputDampBandwidthProgGui->setValue(reverb->progenitor.getoutputdampbw());
      spinProgGui->setValue(reverb->progenitor.getspin());
      spinLimitProgGui->setValue(reverb->progenitor.getspinlimit());
      //wanderProgGui->setValue(reverb->progenitor.getwander());
      //spin2ProgGui->setValue(reverb->progenitor.getspin2());
      //spin2LimitProgGui->setValue(reverb->progenitor.getspinlimit2());
      /*wander2ProgGui->setValue(reverb->progenitor.getwander2());
      spin2WanderProgGui->setValue(reverb->progenitor.getspin2wander());
      decay0ProgGui->setValue(reverb->progenitor.getdecay0());
      decay1ProgGui->setValue(reverb->progenitor.getdecay1());
      decay2ProgGui->setValue(reverb->progenitor.getdecay2());
      decay3ProgGui->setValue(reverb->progenitor.getdecay3());
      bassBandwidthProgGui->setValue(reverb->progenitor.getbassbw());
      bassBoostProgGui->setValue(reverb->progenitor.getbassboost());
      idiffusionProgGui->setValue(reverb->progenitor.getidiffusion1());
      odiffusionProgGui->setValue(reverb->progenitor.getodiffusion1());
      modulationNoise1ProgGui->setValue(reverb->progenitor.getmodulationnoise1());
      modulationNoise2ProgGui->setValue(reverb->progenitor.getmodulationnoise2());
      crossFeedProgGui->setValue(reverb->progenitor.getcrossfeed());
    }
    if(reverb->type == Reverb::Type::Lexicon224) {
      diffusion1LexiconGui->setValue(reverb->lexicon224.getdiffusion1());
      diffusion2LexiconGui->setValue(reverb->lexicon224.getdiffusion2());
      dampLexiconGui->setValue(reverb->lexicon224.getdamp());
      inputDampLexiconGui->setValue(reverb->lexicon224.getinputdamp());
      //ouputDampLexiconGui->setValue(reverb->lexicon224.getoutputdamp());
      rt60LexiconGui->setValue(reverb->lexicon224.getrt60());
      dcCutFrequencyLexiconGui->setValue(reverb->lexicon224.getdccutfreq());
      //spinLexiconGui->setValue(reverb->lexicon224.getspin());
      //spinLimitLexiconGui->setValue(reverb->lexicon224.getspinlimit());
      /*spinDifferenceLexiconGui->setValue(reverb->lexicon224.getspindiff());
      wanderLexiconGui->setValue(reverb->lexicon224.getwander());
      modulationNoise1LexiconGui->setValue(reverb->lexicon224.getmodulationnoise1());
      modulationNoise2LexiconGui->setValue(reverb->lexicon224.getmodulationnoise2());
      autoDifferenceLexiconGui->setValue(reverb->lexicon224.getAutoDiff());
    }
    if(reverb->type == Reverb::Type::Zita) {
      diffusionZitaGui->setValue(reverb->zita.getidiffusion1());
      rt60ZitaGui->setValue(reverb->zita.getrt60());
      rt60LowZitaGui->setValue(reverb->zita.getrt60_factor_low());
      rt60HighZitaGui->setValue(reverb->zita.getrt60_factor_high());
      dcCutFrequencyZitaGui->setValue(reverb->zita.getdccutfreq());
      lfo1FrequencyZitaGui->setValue(reverb->zita.getlfo1freq());
      lfo2FrequencyZitaGui->setValue(reverb->zita.getlfo2freq());
      lfoFactorZitaGui->setValue(reverb->zita.getlfofactor());
      spinZitaGui->setValue(reverb->zita.getspin());
      spinFactorZitaGui->setValue(reverb->zita.getspinfactor());
      //wanderZitaGui->setValue(reverb->zita.getwander());
      xoverLowZitaGui->setValue(reverb->zita.getxover_low());
      xoverHighZitaGui->setValue(reverb->zita.getxover_high());
      apFeedbackZitaGui->setValue(reverb->zita.getapfeedback());
      loopDampZitaGui->setValue(reverb->zita.getloopdamp());
      outputLpfZitaGui->setValue(reverb->zita.getoutputlpf());
      outputHpfZitaGui->setValue(reverb->zita.getoutputhpf());
    }
    if(reverb->type == Reverb::Type::Mverb) {
      dampingFreqMVerbGui->setValue(reverb->mverb.getParameter(MVerb<double>::DAMPINGFREQ));
      densityMVerbGui->setValue(reverb->mverb.getParameter(MVerb<double>::DENSITY));
      bandwidthFreqMVerbGui->setValue(reverb->mverb.getParameter(MVerb<double>::BANDWIDTHFREQ));
      preDelayMVerbGui->setValue(reverb->mverb.getParameter(MVerb<double>::PREDELAY));
      sizeMVerbGui->setValue(reverb->mverb.getParameter(MVerb<double>::SIZE));
      decayMVerbGui->setValue(reverb->mverb.getParameter(MVerb<double>::DECAY));
      gainMVerbGui->setValue(reverb->mverb.getParameter(MVerb<double>::GAIN));
      mixMVerbGui->setValue(reverb->mverb.getParameter(MVerb<double>::MIX));
      earlyMixMVerbGui->setValue(reverb->mverb.getParameter(MVerb<double>::EARLYMIX));
    }*/
  }
  virtual ~Reverb() {
    if(reverbPanel) {
      GuiElement *parent = reverbPanel->parent;
      parent->deleteChildElement(reverbPanel);
    }
    delete [] arr;
  }
  
  void apply(Vec2d &sample) {
    if(!isActive) return;
    try {
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
      else if(type == Mverb) {
        arr[0][0] = sample.x;
        arr[1][0] = sample.y;
        mverb.process(arr, arr, 1);
        sample.x = arr[0][0];
        sample.y = arr[1][0];
      }
    }
    catch(...) {
      printf("Exception caught from reverb %s!\n", typeNames[type].c_str());
    };
    
    sample *= gain;
  }
  

  void reset() {
    freeverb.mute();
    ccrma.mute();
    lexicon224.mute();
    progenitor.mute();
    zita.mute();
    mverb.reset();
  }

  struct ReverbPanel : public Panel {
    
    //CheckBox *isActiveGui = NULL;
    CheckBox *muteOnParameterChangeGui = NULL;
    
    NumberBox *initialDelayGui = NULL;
    NumberBox *preDelayGui = NULL;

    static const int numLayouts = 3;
    int layout = 1;
    
    struct ReverbGuiLayout : public SynthGuiLayout {
      struct Tab {
        GuiElement *gui = NULL;
        Vec2d size;
      };
      std::vector<Tab> tabs = std::vector<Tab>(Reverb::Type::numTypes);
    };

    std::vector<ReverbGuiLayout> reverbLayouts = std::vector<ReverbGuiLayout>(numLayouts);
    //std::vector<GuiElement *> reverbLayoutCommonGuis;
    //std::vector<std::vector<GuiElement *>> reverbLayoutTabGuis;
    
    //std::vector<double> guiHeights = std::vector<double>(Type::numTypes);
    // freeverb
    //GuiElement *freeverbGui = NULL;

    NumberBox *wetLevelFreeVerbGui = NULL;
    NumberBox *dryLevelFreeVerbGui = NULL;
    NumberBox *widthFreeVerbGui = NULL;
    NumberBox *roomSizeFreeVerbGui = NULL;
    NumberBox *dampFreeVerbGui = NULL;
    // CCRMA
    //GuiElement *tab = NULL;
    NumberBox *wetLevelCCRMAGui = NULL;
    NumberBox *dryLevelCCRMAGui = NULL;
    NumberBox *widthCCRMAGui = NULL;
    NumberBox *feedbackCcrmaGui = NULL;
    NumberBox *apFeedbackCcrmaGui = NULL;
    NumberBox *dampCcrmaGui = NULL;
    // progenitor
    //GuiElement *tab = NULL;
    NumberBox *wetLevelProgGui = NULL;
    NumberBox *dryLevelProgGui = NULL;
    NumberBox *widthProgGui = NULL;
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
    //GuiElement *tab = NULL;
    NumberBox *wetLevelLexiconGui = NULL;
    NumberBox *dryLevelLexiconGui = NULL;
    NumberBox *widthLexiconGui = NULL;
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
    //GuiElement *tab = NULL;
    NumberBox *wetLevelZitaGui = NULL;
    NumberBox *dryLevelZitaGui = NULL;
    NumberBox *widthZitaGui = NULL;
    NumberBox *rt60ZitaGui = NULL;
    NumberBox *apFeedbackZitaGui = NULL;
    //NumberBox *loopDampZitaGui = NULL;
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
    //mverb
    //GuiElement *tab = NULL;
    NumberBox *dampingFreqMVerbGui = NULL;
    NumberBox *densityMVerbGui = NULL;
    NumberBox *bandwidthFreqMVerbGui = NULL;
    NumberBox *preDelayMVerbGui = NULL;
    NumberBox *sizeMVerbGui = NULL;
    NumberBox *decayMVerbGui = NULL;
    //NumberBox *gainMVerbGui = NULL;
    NumberBox *mixMVerbGui = NULL;
    NumberBox *earlyMixMVerbGui = NULL;
    
    
    ListBox *presetsGui = NULL;    
    NumberBox *gainGui = NULL;
    ListBox *typeGui = NULL;

    
    
    // KNOB LAYOUT

    RotaryKnob<double> *wetLevelFreeVerbKnob = NULL;
    RotaryKnob<double> *dryLevelFreeVerbKnob = NULL;
    RotaryKnob<double> *widthFreeVerbKnob = NULL;
    RotaryKnob<double> *roomSizeFreeVerbKnob = NULL;
    RotaryKnob<double> *dampFreeVerbKnob = NULL;
    // CCRMA
    //GuiElement *tab = NULL;
    RotaryKnob<double> *wetLevelCCRMAKnob = NULL;
    RotaryKnob<double> *dryLevelCCRMAKnob = NULL;
    RotaryKnob<double> *widthCCRMAKnob = NULL;
    RotaryKnob<double> *feedbackCcrmaKnob = NULL;
    RotaryKnob<double> *apFeedbackCcrmaKnob = NULL;
    RotaryKnob<double> *dampCcrmaKnob = NULL;
    // progenitor
    //GuiElement *tab = NULL;
    RotaryKnob<double> *wetLevelProgKnob = NULL;
    RotaryKnob<double> *dryLevelProgKnob = NULL;
    RotaryKnob<double> *widthProgKnob = NULL;
    RotaryKnob<double> *diffusion1ProgKnob = NULL;
    RotaryKnob<double> *diffusion2ProgKnob = NULL;
    RotaryKnob<double> *diffusion3ProgKnob = NULL;
    RotaryKnob<double> *diffusion4ProgKnob = NULL;
    RotaryKnob<double> *dampProgKnob = NULL;
    RotaryKnob<double> *damp2ProgKnob = NULL;
    RotaryKnob<double> *inputDampProgKnob = NULL;
    RotaryKnob<double> *ouputDampProgKnob = NULL;
    RotaryKnob<double> *ouputDampBandwidthProgKnob = NULL;
    RotaryKnob<double> *spinProgKnob = NULL;
    RotaryKnob<double> *spinLimitProgKnob = NULL;
    RotaryKnob<double> *wanderProgKnob = NULL;
    RotaryKnob<double> *spin2ProgKnob = NULL;
    RotaryKnob<double> *spin2WanderProgKnob = NULL;
    RotaryKnob<double> *spin2LimitProgKnob = NULL;
    RotaryKnob<double> *wander2ProgKnob = NULL;
    RotaryKnob<double> *decay0ProgKnob = NULL;
    RotaryKnob<double> *decay1ProgKnob = NULL;
    RotaryKnob<double> *decay2ProgKnob = NULL;
    RotaryKnob<double> *decay3ProgKnob = NULL;
    RotaryKnob<double> *bassBandwidthProgKnob = NULL;
    RotaryKnob<double> *bassBoostProgKnob = NULL;
    RotaryKnob<double> *idiffusionProgKnob = NULL;
    RotaryKnob<double> *odiffusionProgKnob = NULL;
    RotaryKnob<double> *modulationNoise1ProgKnob = NULL;
    RotaryKnob<double> *modulationNoise2ProgKnob = NULL;
    RotaryKnob<double> *crossFeedProgKnob = NULL;
    // lexicon224
    //GuiElement *tab = NULL;
    RotaryKnob<double> *wetLevelLexiconKnob = NULL;
    RotaryKnob<double> *dryLevelLexiconKnob = NULL;
    RotaryKnob<double> *widthLexiconKnob = NULL;
    RotaryKnob<double> *diffusion1LexiconKnob = NULL;
    RotaryKnob<double> *diffusion2LexiconKnob = NULL;
    RotaryKnob<double> *dampLexiconKnob = NULL;
    RotaryKnob<double> *inputDampLexiconKnob = NULL;
    RotaryKnob<double> *ouputDampLexiconKnob = NULL;
    RotaryKnob<double> *rt60LexiconKnob = NULL;
    RotaryKnob<double> *dcCutFrequencyLexiconKnob = NULL;
    RotaryKnob<double> *spinLexiconKnob = NULL;
    RotaryKnob<double> *spinDifferenceLexiconKnob = NULL;
    RotaryKnob<double> *spinLimitLexiconKnob = NULL;
    RotaryKnob<double> *wanderLexiconKnob = NULL;
    RotaryKnob<double> *modulationNoise1LexiconKnob = NULL;
    RotaryKnob<double> *modulationNoise2LexiconKnob = NULL;
    RotaryKnob<double> *autoDifferenceLexiconKnob = NULL;
    // zita
    //GuiElement *tab = NULL;
    RotaryKnob<double> *wetLevelZitaKnob = NULL;
    RotaryKnob<double> *dryLevelZitaKnob = NULL;
    RotaryKnob<double> *widthZitaKnob = NULL;
    RotaryKnob<double> *rt60ZitaKnob = NULL;
    RotaryKnob<double> *apFeedbackZitaKnob = NULL;
    //RotaryKnob<double> *loopDampZitaKnob = NULL;
    RotaryKnob<double> *outputLpfZitaKnob = NULL;
    RotaryKnob<double> *outputHpfZitaKnob = NULL;
    RotaryKnob<double> *dcCutFrequencyZitaKnob = NULL;
    RotaryKnob<double> *lfo1FrequencyZitaKnob = NULL;
    RotaryKnob<double> *lfo2FrequencyZitaKnob = NULL;
    RotaryKnob<double> *lfoFactorZitaKnob = NULL;
    RotaryKnob<double> *rt60LowZitaKnob = NULL;
    RotaryKnob<double> *rt60HighZitaKnob = NULL;
    RotaryKnob<double> *diffusionZitaKnob = NULL;
    RotaryKnob<double> *spinZitaKnob = NULL;
    RotaryKnob<double> *spinFactorZitaKnob = NULL;
    RotaryKnob<double> *wanderZitaKnob = NULL;
    RotaryKnob<double> *xoverLowZitaKnob = NULL;
    RotaryKnob<double> *xoverHighZitaKnob = NULL;
    //mverb
    //GuiElement *tab = NULL;
    RotaryKnob<double> *dampingFreqMVerbKnob = NULL;
    RotaryKnob<double> *densityMVerbKnob = NULL;
    RotaryKnob<double> *bandwidthFreqMVerbKnob = NULL;
    RotaryKnob<double> *preDelayMVerbKnob = NULL;
    RotaryKnob<double> *sizeMVerbKnob = NULL;
    RotaryKnob<double> *decayMVerbKnob = NULL;
    //RotaryKnob<double> *gainMVerbKnob = NULL;
    RotaryKnob<double> *mixMVerbKnob = NULL;
    RotaryKnob<double> *earlyMixMVerbKnob = NULL;
    
    
    RotaryKnob<long> *typeKnob = NULL;
    RotaryKnob<double> *gainKnob = NULL;


    /*RotaryKnob<long> *presetsKnob = NULL;    
    Button *isActiveGui = NULL;
    Button *layoutGui = NULL;*/
    SynthTitleBar *titleBar = NULL;
    bool isPresetActive = false;
    //RotaryKnob<double> *gainGui = NULL;
    
    GuiElement *parentGuiElement = NULL;
    Reverb *reverb = NULL;
    
    ReverbPanel(Reverb *reverb, GuiElement *parentGuiElement) : Panel("Reverb panel") {
      init(reverb, parentGuiElement);
    }
      
    void init(Reverb *reverb, GuiElement *parentGuiElement) {
      
      

      this->reverb = reverb;
      this->parentGuiElement = parentGuiElement;
      //this->synth = synth;
      double line = 0, lineHeight = 23;
      double width = 290;
      setSize(width, 31);

      addGuiEventListener(new ReverbPanelListener(this));
      parentGuiElement->addChildElement(this);
      
      //addChildElement(new Label(reverb->getName(), 10, line));
      
      /*TitleBar *titleBar;
      addChildElement(titleBar = new TitleBar(reverb->getName(), 290, 31, 0, line));
      
      titleBar->addChildElement(isActiveGui = new Button("Power", "data/synth/textures/power.png", 290-30+2, 6, Button::ToggleButton, reverb->isActive));
      titleBar->getGlowLevel = [this]() { return isActiveGui->getActivationLevel(); };
      
      titleBar->addChildElement(layoutGui = new Button("Layout", "data/synth/textures/layout.png", 290-30-35, 4, Button::PressButton));
      layoutGui->pressedOverlayColor.set(0.6, 0.6, 0.6, 1.0);
      
      
      if(reverb->presets && reverb->presets->size() > 0) {
        titleBar->addChildElement(presetsKnob = new RotaryKnob<long>("Preset", 10, 3, RotaryKnob<long>::ListKnob, 0, 1));
        
        for(int i=0; i<reverb->presets->size(); i++) {
          presetsKnob->addItems(reverb->presets->at(i).name);
        }
        presetsKnob->setActiveItem(0);
      }*/
      
      titleBar = new SynthTitleBar(reverb->getName(), this, reverb->isActive, layout, reverb->presets);
      
      line += titleBar->size.y + 5;
      
      for(int i=0; i<2; i++) {
        addChildElement(reverbLayouts[i].gui = new GuiElement("Reverb common controls "+ std::to_string(i)));
        reverbLayouts[i].gui->setPosition(i, line);
      }
      
      line = 0;
      
      reverbLayouts[0].gui->addChildElement(typeGui = new ListBox("Type", 10, line));
      typeGui->setItems(reverb->typeNames);
      typeGui->setValue(reverb->type);
      
      reverbLayouts[0].gui->addChildElement(gainGui = new NumberBox("Gain", reverb->gain, NumberBox::FLOATING_POINT, 0, 1e10, 10, line+=lineHeight));

      
      double lineStart = reverbLayouts[0].gui->pos.y + line;
      line = 0;
          
      GuiElement *tab;
      // freeverb
      addChildElement(tab = new GuiElement("FreeVerb tab"));
      tab->setPosition(0, lineStart);
      tab->addChildElement(wetLevelFreeVerbGui = new NumberBox("Wet", reverb->freeverb.getwet(), NumberBox::FLOATING_POINT, 0, 1, 10, line+=lineHeight, 8));
      tab->addChildElement(dryLevelFreeVerbGui = new NumberBox("Dry", reverb->freeverb.getdry(), NumberBox::FLOATING_POINT, 0, 1, 10, line+=lineHeight, 8));
      tab->addChildElement(widthFreeVerbGui = new NumberBox("Width", reverb->freeverb.getwidth(), NumberBox::FLOATING_POINT, 0, 1, 10, line+=lineHeight, 8));
      
      tab->addChildElement(roomSizeFreeVerbGui = new NumberBox("Room size", reverb->freeverb.getroomsize(), NumberBox::FLOATING_POINT, 0, 1, 10, line+=lineHeight, 8));
      tab->addChildElement(dampFreeVerbGui = new NumberBox("Damp", reverb->freeverb.getdamp(), NumberBox::FLOATING_POINT, 0, 1, 10, line+=lineHeight, 8));
      tab->setSize(290, line + lineHeight + 7);
      reverbLayouts[0].tabs[Reverb::Type::FreeVerb].gui = tab;
      //guiHeights[Reverb::Type::FreeVerb] = line + lineHeight + 7;
      
      // CCRMA
      line = 0;
      addChildElement(tab = new GuiElement("CCRMA tab"));
      tab->setPosition(0, lineStart);
      tab->addChildElement(wetLevelCCRMAGui = new NumberBox("Wet", reverb->ccrma.getwetr(), NumberBox::FLOATING_POINT, 0, 1, 10, line+=lineHeight, 8));
      tab->addChildElement(dryLevelCCRMAGui = new NumberBox("Dry", reverb->ccrma.getdryr(), NumberBox::FLOATING_POINT, 0, 1, 10, line+=lineHeight, 8));
      tab->addChildElement(widthCCRMAGui = new NumberBox("Width", reverb->ccrma.getwidth(), NumberBox::FLOATING_POINT, 0, 1, 10, line+=lineHeight, 8));
      tab->addChildElement(feedbackCcrmaGui = new NumberBox("Feedback", reverb->ccrma.getfeedback(), NumberBox::FLOATING_POINT, 0, 1, 10, line+=lineHeight, 8));
      tab->addChildElement(apFeedbackCcrmaGui = new NumberBox("AP Feedback", reverb->ccrma.getapfeedback(), NumberBox::FLOATING_POINT, 0, 1, 10, line+=lineHeight, 8));
      tab->addChildElement(dampCcrmaGui = new NumberBox("Damp", reverb->ccrma.getdamp(), NumberBox::FLOATING_POINT, 0, 1, 10, line+=lineHeight, 8));
      tab->setSize(290, line + lineHeight + 7);
      reverbLayouts[0].tabs[Reverb::Type::CCRMA].gui = tab;
      
      // progenitor
      line = 0;
      addChildElement(tab = new GuiElement("Progenitor tab"));
      tab->setPosition(0, lineStart);
      tab->addChildElement(wetLevelProgGui = new NumberBox("Wet", reverb->progenitor.getwetr(), NumberBox::FLOATING_POINT, 0, 1, 10, line+=lineHeight, 8));
      tab->addChildElement(dryLevelProgGui = new NumberBox("Dry", reverb->progenitor.getdryr(), NumberBox::FLOATING_POINT, 0, 1, 10, line+=lineHeight, 8));
      tab->addChildElement(widthProgGui = new NumberBox("Width", reverb->progenitor.getwidth(), NumberBox::FLOATING_POINT, 0, 1, 10, line+=lineHeight, 8));
      tab->addChildElement(diffusion1ProgGui = new NumberBox("Diffusion 1", reverb->progenitor.getdiffusion1(), NumberBox::FLOATING_POINT, 0, 0.97, 10, line+=lineHeight, 8));
      tab->addChildElement(diffusion2ProgGui = new NumberBox("Diffusion 2", reverb->progenitor.getdiffusion2(), NumberBox::FLOATING_POINT, 0, 0.97, 10, line+=lineHeight, 8));
      tab->addChildElement(diffusion3ProgGui = new NumberBox("Diffusion 3", reverb->progenitor.getdiffusion3(), NumberBox::FLOATING_POINT, 0, 0.97, 10, line+=lineHeight, 8));
      tab->addChildElement(diffusion4ProgGui = new NumberBox("Diffusion 4", reverb->progenitor.getdiffusion4(), NumberBox::FLOATING_POINT, 0, 0.97, 10, line+=lineHeight, 8));
            
      tab->addChildElement(dampProgGui = new NumberBox("Effect lowpass", reverb->progenitor.getdamp(), NumberBox::FLOATING_POINT, 0, reverb->sampleRate/2, 10, line+=lineHeight, 8));
      //addChildElement(damp2ProgGui = new NumberBox("Damp 2", reverb->progenitor.getdamp2(), NumberBox::FLOATING_POINT, 0, 1e10, 10, line+=lineHeight, 8));
      tab->addChildElement(inputDampProgGui = new NumberBox("Input lowpass", reverb->progenitor.getinputdamp(), NumberBox::FLOATING_POINT, 0, reverb->sampleRate/2, 10, line+=lineHeight, 8));
      tab->addChildElement(ouputDampProgGui = new NumberBox("Output lowpass", reverb->progenitor.getoutputdamp(), NumberBox::FLOATING_POINT, 0, reverb->sampleRate/2, 10, line+=lineHeight, 8));
      tab->addChildElement(ouputDampBandwidthProgGui = new NumberBox("Output lowpass BW", reverb->progenitor.getoutputdampbw(), NumberBox::FLOATING_POINT, 0, 1e10, 10, line+=lineHeight, 8));

      tab->addChildElement(spinProgGui = new NumberBox("Spin", reverb->progenitor.getspin(), NumberBox::FLOATING_POINT, 0, 20, 10, line+=lineHeight, 8));
      //addChildElement(spinLimitProgGui = new NumberBox("Spin limit", reverb->progenitor.getspinlimit(), NumberBox::FLOATING_POINT, 0, 1e10, 10, line+=lineHeight, 8));
      //addChildElement(wanderProgGui = new NumberBox("Spin wander", reverb->progenitor.getwander(), NumberBox::FLOATING_POINT, 0, 1e10, 10, line+=lineHeight, 8));
      //addChildElement(spin2ProgGui = new NumberBox("Spin 2", reverb->progenitor.getspin2(), NumberBox::FLOATING_POINT, 0, 20, 10, line+=lineHeight, 8));
      //addChildElement(spin2LimitProgGui = new NumberBox("Spin 2 limit", reverb->progenitor.getspinlimit2(), NumberBox::FLOATING_POINT, 0, 1e10, 10, line+=lineHeight, 8));
      //addChildElement(wander2ProgGui = new NumberBox("Spin 2 wander", reverb->progenitor.getwander2(), NumberBox::FLOATING_POINT, 0, 1e10, 10, line+=lineHeight, 8));
      //addChildElement(spin2WanderProgGui = new NumberBox("Spin filter length", reverb->progenitor.getspin2wander(), NumberBox::FLOATING_POINT, 0, 1e10, 10, line+=lineHeight, 8));*/

      tab->addChildElement(decay0ProgGui = new NumberBox("Decay 1", reverb->progenitor.getdecay0(), NumberBox::FLOATING_POINT, 0, 0.99, 10, line+=lineHeight, 8));
      tab->addChildElement(decay1ProgGui = new NumberBox("Decay 2", reverb->progenitor.getdecay1(), NumberBox::FLOATING_POINT, 0, 0.99, 10, line+=lineHeight, 8));
      tab->addChildElement(decay2ProgGui = new NumberBox("Decay 3", reverb->progenitor.getdecay2(), NumberBox::FLOATING_POINT, 0, 0.99, 10, line+=lineHeight, 8));
      tab->addChildElement(decay3ProgGui = new NumberBox("Decay 4", reverb->progenitor.getdecay3(), NumberBox::FLOATING_POINT, 0, 0.99, 10, line+=lineHeight, 8));
      
      /*addChildElement(bassBandwidthProgGui = new NumberBox("Bass bandwidth", reverb->progenitor.getbassbw(), NumberBox::FLOATING_POINT, 0, 1e10, 10, line+=lineHeight, 8));
      addChildElement(bassBoostProgGui = new NumberBox("Bass boost", reverb->progenitor.getbassboost(), NumberBox::FLOATING_POINT, 0, 1e10, 10, line+=lineHeight, 8));*/
      
      /*addChildElement(idiffusionProgGui = new NumberBox("Input diffusion", reverb->progenitor.getidiffusion1(), NumberBox::FLOATING_POINT, 0, 1, 10, line+=lineHeight, 8));
      addChildElement(odiffusionProgGui = new NumberBox("Output diffusion", reverb->progenitor.getodiffusion1(), NumberBox::FLOATING_POINT, 0, 1, 10, line+=lineHeight, 8));*/
      /*addChildElement(modulationNoise1ProgGui = new NumberBox("Modulation noise 1", reverb->progenitor.getmodulationnoise1(), NumberBox::FLOATING_POINT, 0, 1e10, 10, line+=lineHeight, 8));
      addChildElement(modulationNoise2ProgGui = new NumberBox("Modulation noise 2", reverb->progenitor.getmodulationnoise2(), NumberBox::FLOATING_POINT, 0, 1e10, 10, line+=lineHeight, 8));
      addChildElement(crossFeedProgGui = new NumberBox("Crossfeed", reverb->progenitor.getcrossfeed(), NumberBox::FLOATING_POINT, 0, 1, 10, line+=lineHeight, 8));*/
      tab->setSize(290, line + lineHeight + 7);
      reverbLayouts[0].tabs[Reverb::Type::Progenitor].gui = tab;
      
      // lexicon224
      line = 0;
      addChildElement(tab = new GuiElement("Lexicon tab"));
      tab->setPosition(0, lineStart);
      tab->addChildElement(wetLevelLexiconGui = new NumberBox("Wet", reverb->lexicon224.getwetr(), NumberBox::FLOATING_POINT, 0, 1, 10, line+=lineHeight, 8));
      tab->addChildElement(dryLevelLexiconGui = new NumberBox("Dry", reverb->lexicon224.getdryr(), NumberBox::FLOATING_POINT, 0, 1, 10, line+=lineHeight, 8));
      tab->addChildElement(widthLexiconGui = new NumberBox("Width", reverb->lexicon224.getwidth(), NumberBox::FLOATING_POINT, 0, 1, 10, line+=lineHeight, 8));
      tab->addChildElement(diffusion1LexiconGui = new NumberBox("Diffusion", reverb->lexicon224.getdiffusion1(), NumberBox::FLOATING_POINT, 0, 1, 10, line+=lineHeight, 8));
      //addChildElement(diffusion2LexiconGui = new NumberBox("Diffusion 2", reverb->lexicon224.getdiffusion2(), NumberBox::FLOATING_POINT, 0, 1, 10, line+=lineHeight, 8));
      
      tab->addChildElement(dampLexiconGui = new NumberBox("Damp", reverb->lexicon224.getdamp(), NumberBox::FLOATING_POINT, 0, reverb->sampleRate/2, 10, line+=lineHeight, 8));
      reverb->lexicon224.setoutputdamp(reverb->lexicon224.getinputdamp());
      tab->addChildElement(inputDampLexiconGui = new NumberBox("Input/output damp", reverb->lexicon224.getinputdamp(), NumberBox::FLOATING_POINT, 0, reverb->sampleRate/2, 10, line+=lineHeight, 8));
      //addChildElement(ouputDampLexiconGui = new NumberBox("Output damp", reverb->lexicon224.getoutputdamp(), NumberBox::FLOATING_POINT, 0, reverb->sampleRate/2, 10, line+=lineHeight, 8));
      tab->addChildElement(rt60LexiconGui = new NumberBox("Rt 60", reverb->lexicon224.getrt60(), NumberBox::FLOATING_POINT, 0, 1e10, 10, line+=lineHeight, 8));
      tab->addChildElement(dcCutFrequencyLexiconGui = new NumberBox("DC cut frequency", reverb->lexicon224.getdccutfreq(), NumberBox::FLOATING_POINT, 0, reverb->sampleRate/2, 10, line+=lineHeight, 8));
      //addChildElement(spinLexiconGui = new NumberBox("Spin", reverb->lexicon224.getspin(), NumberBox::FLOATING_POINT, 0, 1e10, 10, line+=lineHeight, 8));
      //addChildElement(spinDifferenceLexiconGui = new NumberBox("Spin difference", reverb->lexicon224.getspindiff(), NumberBox::FLOATING_POINT, 0, 1e10, 10, line+=lineHeight, 8));
      //addChildElement(spinLimitLexiconGui = new NumberBox("Spin limit", reverb->lexicon224.getspinlimit(), NumberBox::FLOATING_POINT, 0, 1e10, 10, line+=lineHeight, 8));
      //addChildElement(wanderLexiconGui = new NumberBox("Spin strength", reverb->lexicon224.getwander(), NumberBox::FLOATING_POINT, 0, 1e10, 10, line+=lineHeight, 8));

      //addChildElement(modulationNoise1LexiconGui = new NumberBox("Modulation noise 1", reverb->lexicon224.getmodulationnoise1(), NumberBox::FLOATING_POINT, 0, 1e10, 10, line+=lineHeight, 8));
      //addChildElement(modulationNoise2LexiconGui = new NumberBox("Modulation noise 2", reverb->lexicon224.getmodulationnoise2(), NumberBox::FLOATING_POINT, 0, 1e10, 10, line+=lineHeight, 8));
      //addChildElement(autoDifferenceLexiconGui = new NumberBox("Auto difference", reverb->lexicon224.getAutoDiff(), NumberBox::FLOATING_POINT, 0, 1e10, 10, line+=lineHeight, 8));
      tab->setSize(290, line + lineHeight + 7);
      reverbLayouts[0].tabs[Reverb::Type::Lexicon224].gui = tab;
      
      // zita
      line = 0;
      addChildElement(tab = new GuiElement("Zita tab"));
      tab->setPosition(0, lineStart);
      tab->addChildElement(wetLevelZitaGui = new NumberBox("Wet", reverb->zita.getwetr(), NumberBox::FLOATING_POINT, 0, 1, 10, line+=lineHeight, 8));
      tab->addChildElement(dryLevelZitaGui = new NumberBox("Dry", reverb->zita.getdryr(), NumberBox::FLOATING_POINT, 0, 1, 10, line+=lineHeight, 8));
      tab->addChildElement(widthZitaGui = new NumberBox("Width", reverb->zita.getwidth(), NumberBox::FLOATING_POINT, 0, 1, 10, line+=lineHeight, 8));
      tab->addChildElement(diffusionZitaGui = new NumberBox("Diffusion", reverb->zita.getidiffusion1(), NumberBox::FLOATING_POINT, 0, 0.99, 10, line+=lineHeight, 8));
      tab->addChildElement(rt60ZitaGui = new NumberBox("Rt60", reverb->zita.getrt60(), NumberBox::FLOATING_POINT, 0, 1e10, 10, line+=lineHeight, 8));
      tab->addChildElement(rt60LowZitaGui = new NumberBox("Rt60 factor low", reverb->zita.getrt60_factor_low(), NumberBox::FLOATING_POINT, 0, 1, 10, line+=lineHeight, 8));
      tab->addChildElement(rt60HighZitaGui = new NumberBox("Rt60 factor high", reverb->zita.getrt60_factor_high(), NumberBox::FLOATING_POINT, 0, 1, 10, line+=lineHeight, 8));

      tab->addChildElement(dcCutFrequencyZitaGui = new NumberBox("DC cut frequency", reverb->zita.getdccutfreq(), NumberBox::FLOATING_POINT, 0, reverb->sampleRate/2, 10, line+=lineHeight, 8));
      tab->addChildElement(lfo1FrequencyZitaGui = new NumberBox("LFO 1 frequency", reverb->zita.getlfo1freq(), NumberBox::FLOATING_POINT, 0, 20, 10, line+=lineHeight, 8));
      tab->addChildElement(lfo2FrequencyZitaGui = new NumberBox("LFO 2 frequency", reverb->zita.getlfo2freq(), NumberBox::FLOATING_POINT, 0, 20, 10, line+=lineHeight, 8));
      tab->addChildElement(lfoFactorZitaGui = new NumberBox("LFO strength", reverb->zita.getlfofactor(), NumberBox::FLOATING_POINT, 0, 1, 10, line+=lineHeight, 8));

      tab->addChildElement(spinZitaGui = new NumberBox("Spin", reverb->zita.getspin(), NumberBox::FLOATING_POINT, 0, 20, 10, line+=lineHeight, 8));
      tab->addChildElement(spinFactorZitaGui = new NumberBox("Spin factor", reverb->zita.getspinfactor(), NumberBox::FLOATING_POINT, 0, 10, 10, line+=lineHeight, 8));
      //addChildElement(wanderZitaGui = new NumberBox("Spin strength", reverb->zita.getwander(), NumberBox::FLOATING_POINT, 0, 1e10, 10, line+=lineHeight, 8));
      
      tab->addChildElement(xoverLowZitaGui = new NumberBox("X-over low", reverb->zita.getxover_low(), NumberBox::FLOATING_POINT, 0, 10000, 10, line+=lineHeight, 8));
      tab->addChildElement(xoverHighZitaGui = new NumberBox("X-over high", reverb->zita.getxover_high(), NumberBox::FLOATING_POINT, 0, 10000, 10, line+=lineHeight, 8));

      tab->addChildElement(apFeedbackZitaGui = new NumberBox("AP feedback", reverb->zita.getapfeedback(), NumberBox::FLOATING_POINT, 0, 1, 10, line+=lineHeight, 8));
      //addChildElement(loopDampZitaGui = new NumberBox("Loop damp", reverb->zita.getloopdamp(), NumberBox::FLOATING_POINT, 0, reverb->sampleRate/2-1, 10, line+=lineHeight, 8));
      tab->addChildElement(outputLpfZitaGui = new NumberBox("Lowpass frequency", reverb->zita.getoutputlpf(), NumberBox::FLOATING_POINT, 0, reverb->sampleRate/2, 10, line+=lineHeight, 8));
      tab->addChildElement(outputHpfZitaGui = new NumberBox("Highpass frequency", reverb->zita.getoutputhpf(), NumberBox::FLOATING_POINT, 0, reverb->sampleRate/2, 10, line+=lineHeight, 8));
      tab->setSize(290, line + lineHeight + 7);
      reverbLayouts[0].tabs[Reverb::Type::Zita].gui = tab;
      
      
      
      //mverb
      line = 0;
      addChildElement(tab = new GuiElement("MVerb tab"));
      tab->setPosition(0, lineStart);
      tab->addChildElement(mixMVerbGui = new NumberBox("Mix", reverb->mverb.getParameter(MVerb<double>::MIX), NumberBox::FLOATING_POINT, 0, 1, 10, line+=lineHeight, 8));
      tab->addChildElement(earlyMixMVerbGui = new NumberBox("Early mix", reverb->mverb.getParameter(MVerb<double>::EARLYMIX), NumberBox::FLOATING_POINT, 0, 1, 10, line+=lineHeight, 8));
      //addChildElement(gainMVerbGui = new NumberBox("Gain", reverb->mverb.getParameter(MVerb<double>::GAIN), NumberBox::FLOATING_POINT, 0, 1, 10, line+=lineHeight, 8));
      tab->addChildElement(sizeMVerbGui = new NumberBox("Size", reverb->mverb.getParameter(MVerb<double>::SIZE), NumberBox::FLOATING_POINT, 0.05, 1, 10, line+=lineHeight, 8));
      tab->addChildElement(densityMVerbGui = new NumberBox("Density", reverb->mverb.getParameter(MVerb<double>::DENSITY), NumberBox::FLOATING_POINT, 0, 1, 10, line+=lineHeight, 8));
      tab->addChildElement(decayMVerbGui = new NumberBox("Decay", reverb->mverb.getParameter(MVerb<double>::DECAY), NumberBox::FLOATING_POINT, 0, 1, 10, line+=lineHeight, 8));
      tab->addChildElement(preDelayMVerbGui = new NumberBox("Pre-delay", reverb->mverb.getParameter(MVerb<double>::PREDELAY), NumberBox::FLOATING_POINT, 0, 1, 10, line+=lineHeight, 8));
      tab->addChildElement(dampingFreqMVerbGui = new NumberBox("Damping frequency", reverb->mverb.getParameter(MVerb<double>::DAMPINGFREQ), NumberBox::FLOATING_POINT, 0,  1, 10, line+=lineHeight, 8));
      tab->addChildElement(bandwidthFreqMVerbGui = new NumberBox("Bandwidth frequency", reverb->mverb.getParameter(MVerb<double>::BANDWIDTHFREQ), NumberBox::FLOATING_POINT, 0, 1, 10, line+=lineHeight, 8));
      tab->setSize(290, line + lineHeight + 7);
      reverbLayouts[0].tabs[Reverb::Type::Mverb].gui = tab;


      // KNOB LAYOUT
      RowColumnPlacer placer(width);

      reverbLayouts[1].gui->addChildElement(typeKnob = new RotaryKnob<long>("Type", placer.progressX(), placer.getY(), RotaryKnob<long>::ListKnob, 0, 1, 0));
      typeKnob->setItems(reverb->typeNames);
      typeKnob->setActiveItem(reverb->type);
      
      reverbLayouts[1].gui->addChildElement(gainKnob = new RotaryKnob<double>("Gain", placer.progressX(), placer.getY(), RotaryKnob<double>::LimitedKnob, 0, 2, reverb->gain));
      
      reverbLayouts[1].gui->setSize(width, placer.getY() + placer.knobSize);
      
      
      double columnStart = placer.column;
      
      addChildElement(tab = new GuiElement("FreeVerb tab"));
      tab->setPosition(reverbLayouts[1].gui->pos);
      
      placer.reset(columnStart);
      
      tab->addChildElement(wetLevelFreeVerbKnob = new RotaryKnob<double>("Wet", placer.progressX(), placer.getY(), RotaryKnob<double>::LimitedKnob, 0, 1, reverb->freeverb.getwet()));
      tab->addChildElement(dryLevelFreeVerbKnob = new RotaryKnob<double>("Dry", placer.progressX(), placer.getY(), RotaryKnob<double>::LimitedKnob, 0, 1, reverb->freeverb.getdry()));
      tab->addChildElement(widthFreeVerbKnob = new RotaryKnob<double>("Width", placer.progressX(), placer.getY(), RotaryKnob<double>::LimitedKnob, 0, 1, reverb->freeverb.getwidth()));
      tab->addChildElement(roomSizeFreeVerbKnob = new RotaryKnob<double>("Room size", placer.progressX(), placer.getY(), RotaryKnob<double>::LimitedKnob, 0, 1, reverb->freeverb.getroomsize()));
      tab->addChildElement(dampFreeVerbKnob = new RotaryKnob<double>("Damp", placer.progressX(), placer.getY(), RotaryKnob<double>::LimitedKnob, 0, 1, reverb->freeverb.getdamp()));
      
      tab->setSize(width, placer.getY() + placer.knobSize);
      reverbLayouts[1].tabs[Reverb::Type::FreeVerb].gui = tab;
      
      
      // CCRMA
      placer.reset(columnStart);
      
      addChildElement(tab = new GuiElement("CCRMA tab"));
      tab->setPosition(reverbLayouts[1].gui->pos);
      
      tab->addChildElement(wetLevelCCRMAKnob = new RotaryKnob<double>("Wet", placer.progressX(), placer.getY(), RotaryKnob<double>::LimitedKnob, 0, 1, reverb->ccrma.getwet()));
      tab->addChildElement(dryLevelCCRMAKnob = new RotaryKnob<double>("Dry", placer.progressX(), placer.getY(), RotaryKnob<double>::LimitedKnob, 0, 1, reverb->ccrma.getdry()));
      tab->addChildElement(widthCCRMAKnob = new RotaryKnob<double>("Width", placer.progressX(), placer.getY(), RotaryKnob<double>::LimitedKnob, 0, 1, reverb->ccrma.getwidth()));
      tab->addChildElement(feedbackCcrmaKnob = new RotaryKnob<double>("Feedback", placer.progressX(), placer.getY(), RotaryKnob<double>::LimitedKnob, 0, 1, reverb->ccrma.getfeedback()));
      tab->addChildElement(apFeedbackCcrmaKnob = new RotaryKnob<double>("AP Feedback", placer.progressX(), placer.getY(), RotaryKnob<double>::LimitedKnob, 0, 1, reverb->ccrma.getapfeedback()));
      tab->addChildElement(dampCcrmaKnob = new RotaryKnob<double>("Damp", placer.progressX(), placer.getY(), RotaryKnob<double>::LimitedKnob, 0, 1, reverb->ccrma.getdamp()));
      
      tab->setSize(width, placer.getY() + placer.knobSize);
      reverbLayouts[1].tabs[Reverb::Type::CCRMA].gui = tab;
      
      // progenitor
      placer.reset(columnStart);
      addChildElement(tab = new GuiElement("Progenitor tab"));
      tab->setPosition(reverbLayouts[1].gui->pos);
      
      tab->addChildElement(wetLevelProgKnob = new RotaryKnob<double>("Wet", placer.progressX(), placer.getY(), RotaryKnob<double>::LimitedKnob, 0, 1, reverb->progenitor.getwetr()));
      tab->addChildElement(dryLevelProgKnob = new RotaryKnob<double>("Dry", placer.progressX(), placer.getY(), RotaryKnob<double>::LimitedKnob, 0, 1, reverb->progenitor.getdryr()));
      tab->addChildElement(widthProgKnob = new RotaryKnob<double>("Width", placer.progressX(), placer.getY(), RotaryKnob<double>::LimitedKnob, 0, 1, reverb->progenitor.getwidth()));
      tab->addChildElement(diffusion1ProgKnob = new RotaryKnob<double>("Diffusion 1", placer.progressX(), placer.getY(), RotaryKnob<double>::LimitedKnob, 0, 0.97, reverb->progenitor.getdiffusion1()));
      tab->addChildElement(diffusion2ProgKnob = new RotaryKnob<double>("Diffusion 2", placer.progressX(), placer.getY(), RotaryKnob<double>::LimitedKnob, 0, 0.97, reverb->progenitor.getdiffusion2()));
      tab->addChildElement(diffusion3ProgKnob = new RotaryKnob<double>("Diffusion 3", placer.progressX(), placer.getY(), RotaryKnob<double>::LimitedKnob, 0, 0.97, reverb->progenitor.getdiffusion3()));
      tab->addChildElement(diffusion4ProgKnob = new RotaryKnob<double>("Diffusion 4", placer.progressX(), placer.getY(), RotaryKnob<double>::LimitedKnob, 0, 0.97, reverb->progenitor.getdiffusion4()));
      tab->addChildElement(dampProgKnob = new RotaryKnob<double>("Effect lowpass", placer.progressX(), placer.getY(), RotaryKnob<double>::LimitedKnob, 0, reverb->sampleRate/2-1, reverb->progenitor.getdamp()));
      dampProgKnob->valueMappingFunction = frequencyExpMapper;
      
      tab->addChildElement(inputDampProgKnob = new RotaryKnob<double>("Input lowpass", placer.progressX(), placer.getY(), RotaryKnob<double>::LimitedKnob, 0, reverb->sampleRate/2-1, reverb->progenitor.getinputdamp()));
      inputDampProgKnob->valueMappingFunction = frequencyExpMapper;
      
      tab->addChildElement(ouputDampProgKnob = new RotaryKnob<double>("Output lowpass", placer.progressX(), placer.getY(), RotaryKnob<double>::LimitedKnob, 0, reverb->sampleRate/2-1, reverb->progenitor.getoutputdamp()));
      ouputDampProgKnob->valueMappingFunction = frequencyExpMapper;
      
      tab->addChildElement(ouputDampBandwidthProgKnob = new RotaryKnob<double>("Output lowpass BW", placer.progressX(), placer.getY(), RotaryKnob<double>::LimitedKnob, 0, 100, reverb->progenitor.getoutputdampbw()));
      
      tab->addChildElement(spinProgKnob = new RotaryKnob<double>("Spin", placer.progressX(), placer.getY(), RotaryKnob<double>::LimitedKnob, 0, 20, reverb->progenitor.getspin()));
      tab->addChildElement(decay0ProgKnob = new RotaryKnob<double>("Decay 1", placer.progressX(), placer.getY(), RotaryKnob<double>::LimitedKnob, 0, 0.99, reverb->progenitor.getdecay0()));
      tab->addChildElement(decay1ProgKnob = new RotaryKnob<double>("Decay 2", placer.progressX(), placer.getY(), RotaryKnob<double>::LimitedKnob, 0, 0.99, reverb->progenitor.getdecay1()));
      tab->addChildElement(decay2ProgKnob = new RotaryKnob<double>("Decay 3", placer.progressX(), placer.getY(), RotaryKnob<double>::LimitedKnob, 0, 0.99, reverb->progenitor.getdecay2()));
      tab->addChildElement(decay3ProgKnob = new RotaryKnob<double>("Decay 4", placer.progressX(), placer.getY(), RotaryKnob<double>::LimitedKnob, 0, 0.99, reverb->progenitor.getdecay3()));

      tab->setSize(width, placer.getY() + placer.knobSize);
      reverbLayouts[1].tabs[Reverb::Type::Progenitor].gui = tab;
      
      // lexicon224
      placer.reset(columnStart);
      addChildElement(tab = new GuiElement("Lexicon tab"));
      tab->setPosition(reverbLayouts[1].gui->pos);
      
      tab->addChildElement(wetLevelLexiconKnob = new RotaryKnob<double>("Wet", placer.progressX(), placer.getY(), RotaryKnob<double>::LimitedKnob, 0, 1, reverb->lexicon224.getwetr()));
      tab->addChildElement(dryLevelLexiconKnob = new RotaryKnob<double>("Dry", placer.progressX(), placer.getY(), RotaryKnob<double>::LimitedKnob, 0, 1, reverb->lexicon224.getdryr()));
      tab->addChildElement(widthLexiconKnob = new RotaryKnob<double>("Width", placer.progressX(), placer.getY(), RotaryKnob<double>::LimitedKnob, 0, 1, reverb->lexicon224.getwidth()));
      tab->addChildElement(diffusion1LexiconKnob = new RotaryKnob<double>("Diffusion", placer.progressX(), placer.getY(), RotaryKnob<double>::LimitedKnob, 0, 1, reverb->lexicon224.getdiffusion1()));
      tab->addChildElement(dampLexiconKnob = new RotaryKnob<double>("Damp", placer.progressX(), placer.getY(), RotaryKnob<double>::LimitedKnob, 0, reverb->sampleRate/2-1, reverb->lexicon224.getdamp()));
      dampLexiconKnob->valueMappingFunction = frequencyExpMapper;
      tab->addChildElement(inputDampLexiconKnob = new RotaryKnob<double>("Input/output damp", placer.progressX(), placer.getY(), RotaryKnob<double>::LimitedKnob, 0, reverb->sampleRate/2-1, reverb->lexicon224.getinputdamp()));
      inputDampLexiconKnob->valueMappingFunction = frequencyExpMapper;
      tab->addChildElement(rt60LexiconKnob = new RotaryKnob<double>("Rt 60", placer.progressX(), placer.getY(), RotaryKnob<double>::LimitedKnob, 0, 63, reverb->lexicon224.getrt60()));      
      tab->addChildElement(dcCutFrequencyLexiconKnob = new RotaryKnob<double>("DC cut frequency", placer.progressX(), placer.getY(), RotaryKnob<double>::LimitedKnob, 0, reverb->sampleRate/2-1, reverb->lexicon224.getdccutfreq()));
      dcCutFrequencyLexiconKnob->valueMappingFunction = frequencyExpMapper;
      
      tab->setSize(width, placer.getY() + placer.knobSize);
      reverbLayouts[1].tabs[Reverb::Type::Lexicon224].gui = tab;
      
      // zita
      placer.reset(columnStart);
      addChildElement(tab = new GuiElement("Zita tab"));
      tab->setPosition(reverbLayouts[1].gui->pos);
      
      tab->addChildElement(wetLevelZitaKnob = new RotaryKnob<double>("Wet", placer.progressX(), placer.getY(), RotaryKnob<double>::LimitedKnob, 0, 1, reverb->zita.getwetr()));
      tab->addChildElement(dryLevelZitaKnob = new RotaryKnob<double>("Dry", placer.progressX(), placer.getY(), RotaryKnob<double>::LimitedKnob, 0, 1, reverb->zita.getdryr()));
      tab->addChildElement(widthZitaKnob = new RotaryKnob<double>("Width", placer.progressX(), placer.getY(), RotaryKnob<double>::LimitedKnob, 0, 1, reverb->zita.getwidth()));
      tab->addChildElement(diffusionZitaKnob = new RotaryKnob<double>("Diffusion", placer.progressX(), placer.getY(), RotaryKnob<double>::LimitedKnob, 0, 0.97, reverb->zita.getidiffusion1()));
      tab->addChildElement(rt60ZitaKnob = new RotaryKnob<double>("Rt60", placer.progressX(), placer.getY(), RotaryKnob<double>::LimitedKnob, 0, 63, reverb->zita.getrt60()));
      tab->addChildElement(rt60LowZitaKnob = new RotaryKnob<double>("Rt60 factor low", placer.progressX(), placer.getY(), RotaryKnob<double>::LimitedKnob, 0, 1, reverb->zita.getrt60_factor_low()));
      tab->addChildElement(rt60HighZitaKnob = new RotaryKnob<double>("Rt60 factor high", placer.progressX(), placer.getY(), RotaryKnob<double>::LimitedKnob, 0, 1, reverb->zita.getrt60_factor_high()));      
      tab->addChildElement(dcCutFrequencyZitaKnob = new RotaryKnob<double>("DC cut frequency", placer.progressX(), placer.getY(), RotaryKnob<double>::LimitedKnob, 0, reverb->sampleRate/2-1, reverb->zita.getdccutfreq()));
      dcCutFrequencyZitaKnob->valueMappingFunction = frequencyExpMapper;
      tab->addChildElement(lfo1FrequencyZitaKnob = new RotaryKnob<double>("LFO 1 frequency", placer.progressX(), placer.getY(), RotaryKnob<double>::LimitedKnob, 0, 20, reverb->zita.getlfo1freq()));
      tab->addChildElement(lfo2FrequencyZitaKnob = new RotaryKnob<double>("LFO 2 frequency", placer.progressX(), placer.getY(), RotaryKnob<double>::LimitedKnob, 0, 20, reverb->zita.getlfo2freq()));
      tab->addChildElement(lfoFactorZitaKnob = new RotaryKnob<double>("LFO strength", placer.progressX(), placer.getY(), RotaryKnob<double>::LimitedKnob, 0, 1, reverb->zita.getlfofactor()));
      tab->addChildElement(spinZitaKnob = new RotaryKnob<double>("Spin", placer.progressX(), placer.getY(), RotaryKnob<double>::LimitedKnob, 0, 20, reverb->zita.getspin()));
      tab->addChildElement(spinFactorZitaKnob = new RotaryKnob<double>("Spin factor", placer.progressX(), placer.getY(), RotaryKnob<double>::LimitedKnob, 0, 10, reverb->zita.getspinfactor()));
      tab->addChildElement(xoverLowZitaKnob = new RotaryKnob<double>("X-over low", placer.progressX(), placer.getY(), RotaryKnob<double>::LimitedKnob, 0, reverb->sampleRate/2-1, reverb->zita.getxover_low()));
      xoverLowZitaKnob->valueMappingFunction = frequencyExpMapper;
      tab->addChildElement(xoverHighZitaKnob = new RotaryKnob<double>("X-over high", placer.progressX(), placer.getY(), RotaryKnob<double>::LimitedKnob, 0, reverb->sampleRate/2-1, reverb->zita.getxover_high()));
      xoverHighZitaKnob->valueMappingFunction = frequencyExpMapper;
      tab->addChildElement(apFeedbackZitaKnob = new RotaryKnob<double>("AP feedback", placer.progressX(), placer.getY(), RotaryKnob<double>::LimitedKnob, 0, 1, reverb->zita.getapfeedback()));
      tab->addChildElement(outputLpfZitaKnob = new RotaryKnob<double>("Lowpass frequency", placer.progressX(), placer.getY(), RotaryKnob<double>::LimitedKnob, 0, reverb->sampleRate/2-1, reverb->zita.getoutputlpf()));
      outputLpfZitaKnob->valueMappingFunction = frequencyExpMapper;
      tab->addChildElement(outputHpfZitaKnob = new RotaryKnob<double>("Highpass frequency", placer.progressX(), placer.getY(), RotaryKnob<double>::LimitedKnob, 0, reverb->sampleRate/2-1, reverb->zita.getoutputhpf()));
      outputHpfZitaKnob->valueMappingFunction = frequencyExpMapper;
      
      tab->setSize(width, placer.getY() + placer.knobSize);
      reverbLayouts[1].tabs[Reverb::Type::Zita].gui = tab;
      
      
      
      //mverb
      placer.reset(columnStart);
      addChildElement(tab = new GuiElement("MVerb tab"));
      tab->setPosition(reverbLayouts[1].gui->pos);
      
      tab->addChildElement(mixMVerbKnob = new RotaryKnob<double>("Mix", placer.progressX(), placer.getY(), RotaryKnob<double>::LimitedKnob, 0, 1, reverb->mverb.getParameter(MVerb<double>::MIX)));
      tab->addChildElement(earlyMixMVerbKnob = new RotaryKnob<double>("Early mix", placer.progressX(), placer.getY(), RotaryKnob<double>::LimitedKnob, 0, 1, reverb->mverb.getParameter(MVerb<double>::EARLYMIX)));
      tab->addChildElement(sizeMVerbKnob = new RotaryKnob<double>("Size", placer.progressX(), placer.getY(), RotaryKnob<double>::LimitedKnob, 0.05, 1, reverb->mverb.getParameter(MVerb<double>::SIZE)));
      tab->addChildElement(densityMVerbKnob = new RotaryKnob<double>("Density", placer.progressX(), placer.getY(), RotaryKnob<double>::LimitedKnob, 0, 1, reverb->mverb.getParameter(MVerb<double>::DENSITY)));
      tab->addChildElement(decayMVerbKnob = new RotaryKnob<double>("Decay", placer.progressX(), placer.getY(), RotaryKnob<double>::LimitedKnob, 0, 1, reverb->mverb.getParameter(MVerb<double>::DECAY)));
      tab->addChildElement(preDelayMVerbKnob = new RotaryKnob<double>("Pre-delay", placer.progressX(), placer.getY(), RotaryKnob<double>::LimitedKnob, 0, 1, reverb->mverb.getParameter(MVerb<double>::PREDELAY)));
      tab->addChildElement(dampingFreqMVerbKnob = new RotaryKnob<double>("Damping frequency", placer.progressX(), placer.getY(), RotaryKnob<double>::LimitedKnob, 0, 1, reverb->mverb.getParameter(MVerb<double>::DAMPINGFREQ)));
      tab->addChildElement(bandwidthFreqMVerbKnob = new RotaryKnob<double>("Bandwidth frequency", placer.progressX(), placer.getY(), RotaryKnob<double>::LimitedKnob, 0, 1, reverb->mverb.getParameter(MVerb<double>::BANDWIDTHFREQ)));

      tab->setSize(width, placer.getY() + placer.knobSize);
      reverbLayouts[1].tabs[Reverb::Type::Mverb].gui = tab;


      for(int i=0; i<numLayouts; i++) {
        if(reverbLayouts[i].gui) {
          titleBar->addPresetControlledGuiElements(reverbLayouts[i].gui);
        }
        for(int k=0; k<Reverb::Type::numTypes; k++) {
          if(reverbLayouts[i].tabs[k].gui) {
            reverbLayouts[i].tabs[k].size.set(reverbLayouts[i].tabs[k].gui->pos + reverbLayouts[i].tabs[k].gui->size);
            titleBar->addPresetControlledGuiElements(reverbLayouts[i].tabs[k].gui);
          }
          else if(reverbLayouts[i].gui) {
            reverbLayouts[i].tabs[k].size.set(reverbLayouts[i].gui->pos + reverbLayouts[i].gui->size);
          }
          else {
            reverbLayouts[i].tabs[k].size.set(titleBar->pos + titleBar->size);
          }
          
        }
      }
      
      update();
    }

    void update() {
      /*if(isPresetActive) {
        titleBar->presetsKnob->defaultOverlayColor.set(1, 1, 1, 0.8);
      }
      else {
        titleBar->presetsKnob->defaultOverlayColor.set(1, 1, 1, 0.2);
      }*/
      //setSize(290, guiHeights[reverb->type]);
      
      //setSize(reverbLayoutTabGuis[layout][reverb->type]->pos.x + reverbLayoutTabGuis[layout][reverb->type]->size.x,
//              reverbLayoutTabGuis[layout][reverb->type]->pos.y + reverbLayoutTabGuis[layout][reverb->type]->size.y);
      setSize(reverbLayouts[layout].tabs[reverb->type].size);

      titleBar->isActiveGui->setValue(reverb->isActive);
      typeGui->setValue(reverb->type);
      gainGui->setValue(reverb->gain);

      //gainGui->setRotation(map(reverb->gain, gainGui->minValue, gainGui->maxValue, gainGui->knobMin, gainGui->knobMax));
      //typeKnob->setValue(reverb->type);
      gainKnob->setValue(reverb->gain);
      
      if(reverb->type == Reverb::Type::FreeVerb) {
        wetLevelFreeVerbGui->setValue(reverb->freeverb.getwet());
        dryLevelFreeVerbGui->setValue(reverb->freeverb.getdry());
        widthFreeVerbGui->setValue(reverb->freeverb.getwidth());
        roomSizeFreeVerbGui->setValue(reverb->freeverb.getroomsize());
        dampFreeVerbGui->setValue(reverb->freeverb.getdamp());
      }
      
      if(reverb->type == Reverb::Type::CCRMA) {
        wetLevelCCRMAGui->setValue(reverb->ccrma.getwetr());
        dryLevelCCRMAGui->setValue(reverb->ccrma.getdryr());
        widthCCRMAGui->setValue(reverb->ccrma.getwidth());
        feedbackCcrmaGui->setValue(reverb->ccrma.getfeedback());
        apFeedbackCcrmaGui->setValue(reverb->ccrma.getapfeedback());
        dampCcrmaGui->setValue(reverb->ccrma.getdamp());
      }
      
      if(reverb->type == Reverb::Type::Progenitor) {
        wetLevelProgGui->setValue(reverb->progenitor.getwetr());
        dryLevelProgGui->setValue(reverb->progenitor.getdryr());
        widthProgGui->setValue(reverb->progenitor.getwidth());
        diffusion1ProgGui->setValue(reverb->progenitor.getdiffusion1());
        diffusion2ProgGui->setValue(reverb->progenitor.getdiffusion2());
        diffusion3ProgGui->setValue(reverb->progenitor.getdiffusion3());
        diffusion4ProgGui->setValue(reverb->progenitor.getdiffusion4());
        dampProgGui->setValue(reverb->progenitor.getdamp());
        //damp2ProgGui->setValue(reverb->progenitor.getdamp2());
        inputDampProgGui->setValue(reverb->progenitor.getinputdamp());
        ouputDampProgGui->setValue(reverb->progenitor.getoutputdamp());
        ouputDampBandwidthProgGui->setValue(reverb->progenitor.getoutputdampbw());
        spinProgGui->setValue(reverb->progenitor.getspin());
        //spinLimitProgGui->setValue(reverb->progenitor.getspinlimit());
        //wanderProgGui->setValue(reverb->progenitor.getwander());
        //spin2ProgGui->setValue(reverb->progenitor.getspin2());
        //spin2LimitProgGui->setValue(reverb->progenitor.getspinlimit2());
        /*wander2ProgGui->setValue(reverb->progenitor.getwander2());
        spin2WanderProgGui->setValue(reverb->progenitor.getspin2wander());*/
        decay0ProgGui->setValue(reverb->progenitor.getdecay0());
        decay1ProgGui->setValue(reverb->progenitor.getdecay1());
        decay2ProgGui->setValue(reverb->progenitor.getdecay2());
        decay3ProgGui->setValue(reverb->progenitor.getdecay3());
        /*bassBandwidthProgGui->setValue(reverb->progenitor.getbassbw());
        bassBoostProgGui->setValue(reverb->progenitor.getbassboost());
        idiffusionProgGui->setValue(reverb->progenitor.getidiffusion1());
        odiffusionProgGui->setValue(reverb->progenitor.getodiffusion1());
        modulationNoise1ProgGui->setValue(reverb->progenitor.getmodulationnoise1());
        modulationNoise2ProgGui->setValue(reverb->progenitor.getmodulationnoise2());
        crossFeedProgGui->setValue(reverb->progenitor.getcrossfeed());*/
      }
      if(reverb->type == Reverb::Type::Lexicon224) {
        wetLevelLexiconGui->setValue(reverb->lexicon224.getwetr());
        dryLevelLexiconGui->setValue(reverb->lexicon224.getdryr());
        widthLexiconGui->setValue(reverb->lexicon224.getwidth());
        
        diffusion1LexiconGui->setValue(reverb->lexicon224.getdiffusion1());
        //diffusion2LexiconGui->setValue(reverb->lexicon224.getdiffusion2());
        dampLexiconGui->setValue(reverb->lexicon224.getdamp());
        inputDampLexiconGui->setValue(reverb->lexicon224.getinputdamp());
        //ouputDampLexiconGui->setValue(reverb->lexicon224.getoutputdamp());
        rt60LexiconGui->setValue(reverb->lexicon224.getrt60());
        dcCutFrequencyLexiconGui->setValue(reverb->lexicon224.getdccutfreq());
        //spinLexiconGui->setValue(reverb->lexicon224.getspin());
        //spinLimitLexiconGui->setValue(reverb->lexicon224.getspinlimit());
        /*spinDifferenceLexiconGui->setValue(reverb->lexicon224.getspindiff());
        wanderLexiconGui->setValue(reverb->lexicon224.getwander());
        modulationNoise1LexiconGui->setValue(reverb->lexicon224.getmodulationnoise1());
        modulationNoise2LexiconGui->setValue(reverb->lexicon224.getmodulationnoise2());
        autoDifferenceLexiconGui->setValue(reverb->lexicon224.getAutoDiff());*/
      }
      if(reverb->type == Reverb::Type::Zita) {
        wetLevelZitaGui->setValue(reverb->zita.getwetr());
        dryLevelZitaGui->setValue(reverb->zita.getdryr());
        widthZitaGui->setValue(reverb->zita.getwidth());
        
        diffusionZitaGui->setValue(reverb->zita.getidiffusion1());
        rt60ZitaGui->setValue(reverb->zita.getrt60());
        rt60LowZitaGui->setValue(reverb->zita.getrt60_factor_low());
        rt60HighZitaGui->setValue(reverb->zita.getrt60_factor_high());
        dcCutFrequencyZitaGui->setValue(reverb->zita.getdccutfreq());
        lfo1FrequencyZitaGui->setValue(reverb->zita.getlfo1freq());
        lfo2FrequencyZitaGui->setValue(reverb->zita.getlfo2freq());
        lfoFactorZitaGui->setValue(reverb->zita.getlfofactor());
        spinZitaGui->setValue(reverb->zita.getspin());
        spinFactorZitaGui->setValue(reverb->zita.getspinfactor());
        //wanderZitaGui->setValue(reverb->zita.getwander());
        xoverLowZitaGui->setValue(reverb->zita.getxover_low());
        xoverHighZitaGui->setValue(reverb->zita.getxover_high());
        apFeedbackZitaGui->setValue(reverb->zita.getapfeedback());
        //loopDampZitaGui->setValue(reverb->zita.getloopdamp());
        outputLpfZitaGui->setValue(reverb->zita.getoutputlpf());
        outputHpfZitaGui->setValue(reverb->zita.getoutputhpf());
      }
      if(reverb->type == Reverb::Type::Mverb) {
        dampingFreqMVerbGui->setValue(reverb->mverb.getParameter(MVerb<double>::DAMPINGFREQ));
        densityMVerbGui->setValue(reverb->mverb.getParameter(MVerb<double>::DENSITY));
        bandwidthFreqMVerbGui->setValue(reverb->mverb.getParameter(MVerb<double>::BANDWIDTHFREQ));
        preDelayMVerbGui->setValue(reverb->mverb.getParameter(MVerb<double>::PREDELAY));
        sizeMVerbGui->setValue(reverb->mverb.getParameter(MVerb<double>::SIZE));
        decayMVerbGui->setValue(reverb->mverb.getParameter(MVerb<double>::DECAY));
        //gainMVerbGui->setValue(reverb->mverb.getParameter(MVerb<double>::GAIN));
        mixMVerbGui->setValue(reverb->mverb.getParameter(MVerb<double>::MIX));
        earlyMixMVerbGui->setValue(reverb->mverb.getParameter(MVerb<double>::EARLYMIX));
      }

      if(reverb->type == Reverb::Type::FreeVerb) {
        wetLevelFreeVerbKnob->setValue(reverb->freeverb.getwet());
        dryLevelFreeVerbKnob->setValue(reverb->freeverb.getdry());
        widthFreeVerbKnob->setValue(reverb->freeverb.getwidth());
        roomSizeFreeVerbKnob->setValue(reverb->freeverb.getroomsize());
        dampFreeVerbKnob->setValue(reverb->freeverb.getdamp());
      }
      
      if(reverb->type == Reverb::Type::CCRMA) {
        wetLevelCCRMAKnob->setValue(reverb->ccrma.getwetr());
        dryLevelCCRMAKnob->setValue(reverb->ccrma.getdryr());
        widthCCRMAKnob->setValue(reverb->ccrma.getwidth());
        feedbackCcrmaKnob->setValue(reverb->ccrma.getfeedback());
        apFeedbackCcrmaKnob->setValue(reverb->ccrma.getapfeedback());
        dampCcrmaKnob->setValue(reverb->ccrma.getdamp());
      }
      
      if(reverb->type == Reverb::Type::Progenitor) {
        wetLevelProgKnob->setValue(reverb->progenitor.getwetr());
        dryLevelProgKnob->setValue(reverb->progenitor.getdryr());
        widthProgKnob->setValue(reverb->progenitor.getwidth());
        diffusion1ProgKnob->setValue(reverb->progenitor.getdiffusion1());
        diffusion2ProgKnob->setValue(reverb->progenitor.getdiffusion2());
        diffusion3ProgKnob->setValue(reverb->progenitor.getdiffusion3());
        diffusion4ProgKnob->setValue(reverb->progenitor.getdiffusion4());
        dampProgKnob->setValue(reverb->progenitor.getdamp());
        inputDampProgKnob->setValue(reverb->progenitor.getinputdamp());
        ouputDampProgKnob->setValue(reverb->progenitor.getoutputdamp());
        ouputDampBandwidthProgKnob->setValue(reverb->progenitor.getoutputdampbw());
        spinProgKnob->setValue(reverb->progenitor.getspin());
        decay0ProgKnob->setValue(reverb->progenitor.getdecay0());
        decay1ProgKnob->setValue(reverb->progenitor.getdecay1());
        decay2ProgKnob->setValue(reverb->progenitor.getdecay2());
        decay3ProgKnob->setValue(reverb->progenitor.getdecay3());
      }
      if(reverb->type == Reverb::Type::Lexicon224) {
        wetLevelLexiconKnob->setValue(reverb->lexicon224.getwetr());
        dryLevelLexiconKnob->setValue(reverb->lexicon224.getdryr());
        widthLexiconKnob->setValue(reverb->lexicon224.getwidth());
        diffusion1LexiconKnob->setValue(reverb->lexicon224.getdiffusion1());
        dampLexiconKnob->setValue(reverb->lexicon224.getdamp());
        inputDampLexiconKnob->setValue(reverb->lexicon224.getinputdamp());
        rt60LexiconKnob->setValue(reverb->lexicon224.getrt60());
        dcCutFrequencyLexiconKnob->setValue(reverb->lexicon224.getdccutfreq());
      }
      if(reverb->type == Reverb::Type::Zita) {
        wetLevelZitaKnob->setValue(reverb->zita.getwetr());
        dryLevelZitaKnob->setValue(reverb->zita.getdryr());
        widthZitaKnob->setValue(reverb->zita.getwidth());
        
        diffusionZitaKnob->setValue(reverb->zita.getidiffusion1());
        rt60ZitaKnob->setValue(reverb->zita.getrt60());
        rt60LowZitaKnob->setValue(reverb->zita.getrt60_factor_low());
        rt60HighZitaKnob->setValue(reverb->zita.getrt60_factor_high());
        dcCutFrequencyZitaKnob->setValue(reverb->zita.getdccutfreq());
        lfo1FrequencyZitaKnob->setValue(reverb->zita.getlfo1freq());
        lfo2FrequencyZitaKnob->setValue(reverb->zita.getlfo2freq());
        lfoFactorZitaKnob->setValue(reverb->zita.getlfofactor());
        spinZitaKnob->setValue(reverb->zita.getspin());
        spinFactorZitaKnob->setValue(reverb->zita.getspinfactor());
        xoverLowZitaKnob->setValue(reverb->zita.getxover_low());
        xoverHighZitaKnob->setValue(reverb->zita.getxover_high());
        apFeedbackZitaKnob->setValue(reverb->zita.getapfeedback());
        outputLpfZitaKnob->setValue(reverb->zita.getoutputlpf());
        outputHpfZitaKnob->setValue(reverb->zita.getoutputhpf());
      }
      if(reverb->type == Reverb::Type::Mverb) {
        dampingFreqMVerbKnob->setValue(reverb->mverb.getParameter(MVerb<double>::DAMPINGFREQ));
        densityMVerbKnob->setValue(reverb->mverb.getParameter(MVerb<double>::DENSITY));
        bandwidthFreqMVerbKnob->setValue(reverb->mverb.getParameter(MVerb<double>::BANDWIDTHFREQ));
        preDelayMVerbKnob->setValue(reverb->mverb.getParameter(MVerb<double>::PREDELAY));
        sizeMVerbKnob->setValue(reverb->mverb.getParameter(MVerb<double>::SIZE));
        decayMVerbKnob->setValue(reverb->mverb.getParameter(MVerb<double>::DECAY));
        mixMVerbKnob->setValue(reverb->mverb.getParameter(MVerb<double>::MIX));
        earlyMixMVerbKnob->setValue(reverb->mverb.getParameter(MVerb<double>::EARLYMIX));
      }

      for(int i=0; i<numLayouts; i++) {
        if(reverbLayouts[i].gui) {
          reverbLayouts[i].gui->setVisible(layout == i);
        }
        for(int k=0; k<Reverb::Type::numTypes; k++) {
          if(reverbLayouts[i].tabs[k].gui) {
            reverbLayouts[i].tabs[k].gui->setVisible(layout == i && reverb->type == k);
          }
        }
      }
    }
    
    /*double getWet() {
      if(reverb->type == Reverb::Type::Mverb) {
        //mixMVerbKnob->setValue(reverb->mverb.getParameter(MVerb<double>::MIX));
        return reverb->mverb.getParameter(MVerb<double>::MIX);
      }
      else if(reverb->type == Reverb::Type::Zita) {
        return reverb->zita.getwetr();
      }
    }
    double getDry() {
      if(reverb->type == Reverb::Type::Mverb) {
        return earlyMixMVerbKnob->setValue(reverb->mverb.getParameter(MVerb<double>::EARLYMIX));
      }
      else if(reverb->type == Reverb::Type::Zita) {
        return dryLevelZitaKnob->setValue(reverb->zita.getdryr());
      }
    }
    void setWet() {
      
    }
    
    double getGain() {
      return gain;
    }
    void setGain(double gain) {
      this->gain = gain;
      // update knob
    }*/
    

    struct ReverbPanelListener : public GuiEventListener {
      ReverbPanel *reverbPanel;
      ReverbPanelListener(ReverbPanel *reverbPanel) {
        this->reverbPanel = reverbPanel;
      }

      void onValueChange(GuiElement *guiElement) {
        
        
        if(guiElement == reverbPanel->titleBar->layoutGui) {
          reverbPanel->layout = (reverbPanel->layout+1) % reverbPanel->numLayouts;
          reverbPanel->update();
        }
        if(guiElement == reverbPanel->titleBar->isActiveGui) {
          guiElement->getValue((void*)&reverbPanel->reverb->isActive);
          reverbPanel->reverb->reset();
        }
        if(guiElement == reverbPanel->presetsGui || guiElement == reverbPanel->titleBar->presetsKnob) {
          int index;
          guiElement->getValue((void*)&index);
          reverbPanel->reverb->reset();
          reverbPanel->reverb->applyPreset(index);
          reverbPanel->update();
          reverbPanel->typeKnob->setValue(reverbPanel->reverb->type);
        }
        
        if(guiElement == reverbPanel->typeGui || guiElement == reverbPanel->typeKnob) {
          guiElement->getValue((void*)&reverbPanel->reverb->type);
          reverbPanel->reverb->reset();
          reverbPanel->update();
        }
        if(guiElement == reverbPanel->gainGui || guiElement == reverbPanel->gainKnob) {
          guiElement->getValue((void*)&reverbPanel->reverb->gain);
        }
        
        
        if(guiElement == reverbPanel->wetLevelFreeVerbGui || guiElement == reverbPanel->wetLevelFreeVerbKnob) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->freeverb.setwet(value);
        }
        if(guiElement == reverbPanel->dryLevelFreeVerbGui || guiElement == reverbPanel->dryLevelFreeVerbKnob) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->freeverb.setdry(value);
        }        
        
        if(guiElement == reverbPanel->roomSizeFreeVerbGui || guiElement == reverbPanel->roomSizeFreeVerbKnob) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->freeverb.setroomsize(value);
        }
        if(guiElement == reverbPanel->dampFreeVerbGui || guiElement == reverbPanel->dampFreeVerbKnob) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->freeverb.setdamp(value);
        }
        if(guiElement == reverbPanel->widthFreeVerbGui || guiElement == reverbPanel->widthFreeVerbKnob) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->freeverb.setwidth(value);
        }
        
        
        
        if(guiElement == reverbPanel->wetLevelCCRMAGui || guiElement == reverbPanel->wetLevelCCRMAKnob) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->ccrma.setwetr(value);
        }
        if(guiElement == reverbPanel->dryLevelCCRMAGui || guiElement == reverbPanel->dryLevelCCRMAKnob) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->ccrma.setdryr(value);
        }
        if(guiElement == reverbPanel->widthCCRMAGui || guiElement == reverbPanel->widthCCRMAKnob) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->ccrma.setwidth(value);
        }
        if(guiElement == reverbPanel->feedbackCcrmaGui || guiElement == reverbPanel->feedbackCcrmaKnob) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->ccrma.setfeedback(value);
          reverbPanel->reverb->ccrma.mute();
        }        
        if(guiElement == reverbPanel->apFeedbackCcrmaGui || guiElement == reverbPanel->apFeedbackCcrmaKnob) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->ccrma.setapfeedback(value);
        }
        if(guiElement == reverbPanel->dampCcrmaGui || guiElement == reverbPanel->dampCcrmaKnob) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->ccrma.setdamp(value);
        }
        
        if(guiElement == reverbPanel->wetLevelProgGui || guiElement == reverbPanel->wetLevelProgKnob) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->progenitor.setwetr(value);
        }
        if(guiElement == reverbPanel->dryLevelProgGui || guiElement == reverbPanel->dryLevelProgKnob) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->progenitor.setdryr(value);
        }
        if(guiElement == reverbPanel->widthProgGui || guiElement == reverbPanel->widthProgKnob) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->progenitor.setwidth(value);
        }
        
        if(guiElement == reverbPanel->diffusion1ProgGui || guiElement == reverbPanel->diffusion1ProgKnob) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->progenitor.setdiffusion1(value);
        }
        if(guiElement == reverbPanel->diffusion2ProgGui || guiElement == reverbPanel->diffusion2ProgKnob) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->progenitor.setdiffusion2(value);
        }
        if(guiElement == reverbPanel->diffusion3ProgGui || guiElement == reverbPanel->diffusion3ProgKnob) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->progenitor.setdiffusion3(value);
        }
        if(guiElement == reverbPanel->diffusion4ProgGui || guiElement == reverbPanel->diffusion4ProgKnob) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->progenitor.setdiffusion4(value);
        }
        
        if(guiElement == reverbPanel->dampProgGui || guiElement == reverbPanel->dampProgKnob) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->progenitor.setdamp(value);
        }
        if(guiElement == reverbPanel->damp2ProgGui || guiElement == reverbPanel->damp2ProgKnob) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->progenitor.setdamp2(value);
        }
        if(guiElement == reverbPanel->inputDampProgGui || guiElement == reverbPanel->inputDampProgKnob) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->progenitor.setinputdamp(value);
        }
        if(guiElement == reverbPanel->ouputDampProgGui || guiElement == reverbPanel->ouputDampProgKnob) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->progenitor.setoutputdamp(value);
        }
        if(guiElement == reverbPanel->ouputDampBandwidthProgGui || guiElement == reverbPanel->ouputDampBandwidthProgKnob) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->progenitor.setoutputdampbw(value);
        }
        
        if(guiElement == reverbPanel->spinProgGui || guiElement == reverbPanel->spinProgKnob) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->progenitor.setspin(value);
          /*if(reverbPanel->reverb->progenitor.getspinlimit() > value) {
            reverbPanel->reverb->progenitor.setspinlimit(value);
            reverbPanel->spinLimitProgGui->setValue(value);
          }*/
        }
        if(guiElement == reverbPanel->spinLimitProgGui || guiElement == reverbPanel->spinLimitProgKnob) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->progenitor.setspinlimit(value);
          /*if(reverbPanel->reverb->progenitor.getspin() < value) {
            reverbPanel->reverb->progenitor.setspin(value);
            reverbPanel->spinProgGui->setValue(value);
          }*/
        }
        if(guiElement == reverbPanel->wanderProgGui || guiElement == reverbPanel->wanderProgKnob) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->progenitor.setwander(value);
        }
        if(guiElement == reverbPanel->spin2ProgGui) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->progenitor.setspin2(value);
          /*if(reverbPanel->reverb->progenitor.getspinlimit2() > value) {
            reverbPanel->reverb->progenitor.setspinlimit2(value);
            reverbPanel->spin2LimitProgGui->setValue(value);
          }*/
        }
        if(guiElement == reverbPanel->spin2LimitProgGui || guiElement == reverbPanel->spin2LimitProgKnob) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->progenitor.setspinlimit2(value);
          /*if(reverbPanel->reverb->progenitor.getspin2() < value) {
            reverbPanel->reverb->progenitor.setspin2(value);
            reverbPanel->spin2ProgGui->setValue(value);
          }*/
        }
        if(guiElement == reverbPanel->wander2ProgGui || guiElement == reverbPanel->wander2ProgKnob) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->progenitor.setwander2(value);
        }
        if(guiElement == reverbPanel->spin2WanderProgGui || guiElement == reverbPanel->spin2WanderProgKnob) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->progenitor.setspin2wander(value);
        }
        
        if(guiElement == reverbPanel->decay0ProgGui || guiElement == reverbPanel->decay0ProgKnob) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->progenitor.setdecay0(value);
        }
        if(guiElement == reverbPanel->decay1ProgGui || guiElement == reverbPanel->decay1ProgKnob) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->progenitor.setdecay1(value);
        }
        if(guiElement == reverbPanel->decay2ProgGui || guiElement == reverbPanel->decay2ProgKnob) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->progenitor.setdecay2(value);
        }
        if(guiElement == reverbPanel->decay3ProgGui || guiElement == reverbPanel->decay3ProgKnob) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->progenitor.setdecay3(value);
        }
        
        if(guiElement == reverbPanel->bassBandwidthProgGui || guiElement == reverbPanel->bassBandwidthProgKnob) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->progenitor.setbassbw(value);
        }
        if(guiElement == reverbPanel->bassBoostProgGui || guiElement == reverbPanel->bassBoostProgKnob) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->progenitor.setbassboost(value);
        }
        if(guiElement == reverbPanel->idiffusionProgGui || guiElement == reverbPanel->idiffusionProgKnob) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->progenitor.setidiffusion1(value);
        }
        if(guiElement == reverbPanel->odiffusionProgGui || guiElement == reverbPanel->odiffusionProgKnob) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->progenitor.setodiffusion1(value);
        }
        if(guiElement == reverbPanel->modulationNoise1ProgGui || guiElement == reverbPanel->modulationNoise1ProgKnob) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->progenitor.setmodulationnoise1(value);
        }
        if(guiElement == reverbPanel->modulationNoise2ProgGui || guiElement == reverbPanel->modulationNoise2ProgKnob) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->progenitor.setmodulationnoise2(value);
        }
        if(guiElement == reverbPanel->crossFeedProgGui || guiElement == reverbPanel->crossFeedProgKnob) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->progenitor.setcrossfeed(value);
        }
        
        if(guiElement == reverbPanel->wetLevelLexiconGui || guiElement == reverbPanel->wetLevelLexiconKnob) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->lexicon224.setwetr(value);
        }
        if(guiElement == reverbPanel->dryLevelLexiconGui || guiElement == reverbPanel->dryLevelLexiconKnob) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->lexicon224.setdryr(value);
        }       
        if(guiElement == reverbPanel->widthLexiconGui || guiElement == reverbPanel->widthLexiconKnob) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->lexicon224.setwidth(value);
        }

        if(guiElement == reverbPanel->diffusion1LexiconGui || guiElement == reverbPanel->diffusion1LexiconKnob) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->lexicon224.setdiffusion1(value);
        }
        /*if(guiElement == reverbPanel->diffusion2LexiconGui) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->lexicon224.setdiffusion2(value);
        }*/
        if(guiElement == reverbPanel->dampLexiconGui || guiElement == reverbPanel->dampLexiconKnob) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->lexicon224.setdamp(value);
        }
        if(guiElement == reverbPanel->inputDampLexiconGui || guiElement == reverbPanel->inputDampLexiconKnob) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->lexicon224.setinputdamp(value);
          reverbPanel->reverb->lexicon224.setoutputdamp(value);
        }
        if(guiElement == reverbPanel->ouputDampLexiconGui || guiElement == reverbPanel->ouputDampLexiconKnob) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->lexicon224.setoutputdamp(value);
        }
        if(guiElement == reverbPanel->rt60LexiconGui || guiElement == reverbPanel->rt60LexiconKnob) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->lexicon224.setrt60(value);
        }
        if(guiElement == reverbPanel->dcCutFrequencyLexiconGui || guiElement == reverbPanel->dcCutFrequencyLexiconKnob) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->lexicon224.setdccutfreq(value);
        }
        
        if(guiElement == reverbPanel->spinLexiconGui || guiElement == reverbPanel->spinLexiconKnob) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->lexicon224.setspin(value);
        }
        if(guiElement == reverbPanel->spinDifferenceLexiconGui || guiElement == reverbPanel->spinDifferenceLexiconKnob) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->lexicon224.setspindiff(value);
        }
        if(guiElement == reverbPanel->spinLimitLexiconGui || guiElement == reverbPanel->spinLimitLexiconKnob) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->lexicon224.setspinlimit(value);
        }
        if(guiElement == reverbPanel->wanderLexiconGui || guiElement == reverbPanel->wanderLexiconKnob) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->lexicon224.setwander(value);
        }

        if(guiElement == reverbPanel->modulationNoise1LexiconGui || guiElement == reverbPanel->modulationNoise1LexiconKnob) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->lexicon224.setmodulationnoise1(value);
        }
        if(guiElement == reverbPanel->modulationNoise2LexiconGui || guiElement == reverbPanel->modulationNoise2LexiconKnob) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->lexicon224.setmodulationnoise2(value);
        }
        if(guiElement == reverbPanel->autoDifferenceLexiconGui || guiElement == reverbPanel->autoDifferenceLexiconKnob) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->lexicon224.setAutoDiff(value);
        }

        
        if(guiElement == reverbPanel->wetLevelZitaGui || guiElement == reverbPanel->wetLevelZitaKnob) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->zita.setwetr(value);
        }
        if(guiElement == reverbPanel->dryLevelZitaGui || guiElement == reverbPanel->dryLevelZitaKnob) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->zita.setdryr(value);
        }
        if(guiElement == reverbPanel->widthZitaGui || guiElement == reverbPanel->widthZitaKnob) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->zita.setwidth(value);
        }
        if(guiElement == reverbPanel->diffusionZitaGui || guiElement == reverbPanel->diffusionZitaKnob) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->zita.setidiffusion1(value);
          reverbPanel->reverb->reset();
        }
        if(guiElement == reverbPanel->rt60ZitaGui || guiElement == reverbPanel->rt60ZitaKnob) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->zita.setrt60(value);
        }
        if(guiElement == reverbPanel->rt60LowZitaGui || guiElement == reverbPanel->rt60LowZitaKnob) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->zita.setrt60_factor_low(value);
        }
        if(guiElement == reverbPanel->rt60HighZitaGui || guiElement == reverbPanel->rt60HighZitaKnob) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->zita.setrt60_factor_high(value);
        }
        
        if(guiElement == reverbPanel->dcCutFrequencyZitaGui || guiElement == reverbPanel->dcCutFrequencyZitaKnob) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->zita.setdccutfreq(value);
        }
        if(guiElement == reverbPanel->lfo1FrequencyZitaGui || guiElement == reverbPanel->lfo1FrequencyZitaKnob) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->zita.setlfo1freq(value);
        }
        if(guiElement == reverbPanel->lfo2FrequencyZitaGui || guiElement == reverbPanel->lfo2FrequencyZitaKnob) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->zita.setlfo2freq(value);
        }
        if(guiElement == reverbPanel->lfoFactorZitaGui || guiElement == reverbPanel->lfoFactorZitaKnob) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->zita.setlfofactor(value);
        }
        
        if(guiElement == reverbPanel->spinZitaGui || guiElement == reverbPanel->spinZitaKnob) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->zita.setspin(value);
        }
        if(guiElement == reverbPanel->spinFactorZitaGui || guiElement == reverbPanel->spinFactorZitaKnob) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->zita.setspinfactor(value);
        }
        if(guiElement == reverbPanel->wanderZitaGui || guiElement == reverbPanel->wanderZitaKnob) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->zita.setwander(value);
        }
        
        if(guiElement == reverbPanel->xoverLowZitaGui || guiElement == reverbPanel->xoverLowZitaKnob) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->zita.setxover_low(value);
          if(reverbPanel->reverb->zita.getxover_high() < value) {
            reverbPanel->xoverHighZitaGui->setValue(value);
            //reverbPanel->reverb->zita.setxover_high(value);
          }
        }
        if(guiElement == reverbPanel->xoverHighZitaGui || guiElement == reverbPanel->xoverHighZitaKnob) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->zita.setxover_high(value);
          if(reverbPanel->reverb->zita.getxover_low() > value) {
            reverbPanel->xoverLowZitaGui->setValue(value);
            //reverbPanel->reverb->zita.setxover_low(value);
          }
        }
        
        if(guiElement == reverbPanel->apFeedbackZitaGui || guiElement == reverbPanel->apFeedbackZitaKnob) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->zita.setapfeedback(value);
        }
        /*if(guiElement == reverbPanel->loopDampZitaGui) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->zita.setloopdamp(value);
        }*/
        if(guiElement == reverbPanel->outputLpfZitaGui || guiElement == reverbPanel->outputLpfZitaKnob) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->zita.setoutputlpf(value);
        }
        if(guiElement == reverbPanel->outputHpfZitaGui || guiElement == reverbPanel->outputHpfZitaKnob) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->zita.setoutputhpf(value);
        }
        
        if(guiElement == reverbPanel->dampingFreqMVerbGui || guiElement == reverbPanel->dampingFreqMVerbKnob) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->mverb.setParameter(MVerb<double>::DAMPINGFREQ, value);
        }
        if(guiElement == reverbPanel->densityMVerbGui || guiElement == reverbPanel->densityMVerbKnob) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->mverb.setParameter(MVerb<double>::DENSITY, value);
        }
        if(guiElement == reverbPanel->bandwidthFreqMVerbGui || guiElement == reverbPanel->bandwidthFreqMVerbKnob) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->mverb.setParameter(MVerb<double>::BANDWIDTHFREQ, value);
        }
        if(guiElement == reverbPanel->preDelayMVerbGui || guiElement == reverbPanel->preDelayMVerbKnob) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->mverb.setParameter(MVerb<double>::PREDELAY, value);
        }
        if(guiElement == reverbPanel->sizeMVerbGui || guiElement == reverbPanel->sizeMVerbKnob) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->mverb.setParameter(MVerb<double>::SIZE, value);
        }
        if(guiElement == reverbPanel->decayMVerbGui || guiElement == reverbPanel->decayMVerbKnob) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->mverb.setParameter(MVerb<double>::DECAY, value);
        }
        /*if(guiElement == reverbPanel->gainMVerbGui) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->mverb.setParameter(MVerb<double>::GAIN, value);
        }*/
        if(guiElement == reverbPanel->mixMVerbGui || guiElement == reverbPanel->mixMVerbKnob) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->mverb.setParameter(MVerb<double>::MIX, value);
        }
        if(guiElement == reverbPanel->earlyMixMVerbGui || guiElement == reverbPanel->earlyMixMVerbKnob) {
          double value;
          guiElement->getValue((void*)&value);
          reverbPanel->reverb->mverb.setParameter(MVerb<double>::EARLYMIX, value);
        }

        
      }
    };
  };
  ReverbPanel *reverbPanel = NULL;
  
  Panel *getPanel() {
    return reverbPanel;
  }

  Panel *addPanel(GuiElement *parentGuiElement, const std::string &title = "") {
    return reverbPanel = new ReverbPanel(this, parentGuiElement);
  }
  
  void removePanel(GuiElement *parentGuiElement) {
    PanelInterface::removePanel(parentGuiElement);

    if(reverbPanel) {
      parentGuiElement->deleteChildElement(reverbPanel);
      reverbPanel = NULL;
    }
  }

  void updatePanel() {
    if(reverbPanel) {
      reverbPanel->update();
    }
  }
  
  struct ReverbPreviewPanel : public Panel {
    Reverb *reverb = NULL;
    Label *reverbLabel = NULL;
    Button *isActiveButton = NULL;
    RotaryKnob<long> *reverbPresetKnob = NULL;
    RotaryKnob<double> *reverbWetKnob = NULL;
    RotaryKnob<double> *reverbDryKnob = NULL;
    RotaryKnob<double> *reverbGainKnob = NULL;
    
    
    ReverbPreviewPanel(const Vec2d &size, Reverb *reverb) {
      this->reverb = reverb;
      setSize(size);

      drawBorder = false;
      drawBackground = false;
      draggable = false;
      
      addChildElement(isActiveButton = new Button("Power", "data/synth/textures/power.png", 4, 4, Button::ToggleButton));
      addChildElement(reverbLabel = new Label("Reverb", 4+25, 5));
      
      
      /*addChildElement(reverbPresetKnob = new RotaryKnob<long>("Preset", 70 + 30, 3, RotaryKnob<long>::ListKnob, 0, 1, 0));
      for(int i=0; i<presets->size(); i++) {
        presetsKnob->addItems(presets->at(i).name);
      }
      presetsKnob->setActiveItem(0);
      presetsKnob->addGuiEventListener(new PresetActivationGuiEventListener(this));
      setPresetActive(false);
      
      addChildElement(reverbWetKnob = new RotaryKnob<double>("Wet", placer.progressX(), placer.getY(), RotaryKnob<double>::LimitedKnob, 0, 1, reverb-*/
    }
    
    virtual void onRender(GeomRenderer &geomRenderer, TextGl &textRenderer) {
      if(!isVisible) return;
      Vec2d p = absolutePos + Vec2d(90, 0);
      
      geomRenderer.texture = NULL;
      geomRenderer.fillColor.set(0, 0, 0, 0.6);
      geomRenderer.strokeColor.set(1, 1, 1, 0.6);
      geomRenderer.strokeType = 1;
      geomRenderer.strokeWidth = 1;
      geomRenderer.drawRect(size.x - p.x - 20, size.y - 10, p.x+10, p.y+5);
      // TODO visualization of the effect of the effect
    }

    struct ReverbPreviewPanelListener : public GuiEventListener {
      ReverbPreviewPanel *reverbPreviewPanel = NULL;
      ReverbPreviewPanelListener(ReverbPreviewPanel *reverbPreviewPanel) : reverbPreviewPanel(reverbPreviewPanel) {}
      
      virtual void onValueChange(GuiElement *guiElement) override {
        if(guiElement == reverbPreviewPanel->isActiveButton) {
          guiElement->getValue((void*)&reverbPreviewPanel->reverb->isActive);
          reverbPreviewPanel->reverb->reset();
          // update main panel active gui etc.
        }

      }
    };
    
  };
  
  virtual Panel *createPreviewPanel(const Vec2d &size) override {
    return new ReverbPreviewPanel(size, this);
  }
  
  
  
  
  
  
  
  
  
  
  
  
  static std::string getClassName() {
    return "reverb";
  }
  
  virtual std::string getBlockName() {
    return getClassName();
  }  
  virtual void decodeParameters() {
    applyParameters(doubleParameters, stringParameters);
  }
  
  void applyPreset(int index) {
    if(!presets || index < 0 || index >= presets->size()) return;
    
    applyParameters(presets->at(index).doubleParameters, presets->at(index).stringParameters);
  }  
  
  void applyParameters(const std::vector<DoubleParameter> &doubleParameters, const std::vector<StringParameter> &stringParameters) {
    for(int i=0; i<stringParameters.size(); i++) {
      if(stringParameters[i].values.size() > 0 && stringParameters[i].name.compare("subtype") == 0) {
        for(int k=0; k<this->typeNames.size(); k++) {
          if(stringParameters[i].values[0].compare(this->typeNames[k]) == 0) {
            this->type = (Reverb::Type)k;
          }
        }
      }
    }
    gain = 1.0;
    for(int i=0; i<doubleParameters.size(); i++) {
      if(doubleParameters[i].values.size() > 0 && doubleParameters[i].name.compare("gain") == 0) {
        gain = doubleParameters[i].values[0];
        break;
      }
    }

    if(this->type == Reverb::Type::FreeVerb) {
      for(int i=0; i<doubleParameters.size(); i++) {
        if(doubleParameters[i].values.size() > 0 && doubleParameters[i].name.compare("wet") == 0) {
          this->freeverb.setwet(doubleParameters[i].values[0]);
        }
        else if(doubleParameters[i].values.size() > 0 && doubleParameters[i].name.compare("dry") == 0) {
          this->freeverb.setdry(doubleParameters[i].values[0]);
        }
        else if(doubleParameters[i].values.size() > 0 && doubleParameters[i].name.compare("width") == 0) {
          this->freeverb.setwidth(doubleParameters[i].values[0]);
        }
        else if(doubleParameters[i].values.size() > 0 && doubleParameters[i].name.compare("roomSize") == 0) {
          this->freeverb.setroomsize(doubleParameters[i].values[0]);
        }
        else if(doubleParameters[i].values.size() > 0 && doubleParameters[i].name.compare("damp") == 0) {
          this->freeverb.setdamp(doubleParameters[i].values[0]);
        }
      }
    }
    else if(this->type == Reverb::Type::CCRMA) {
      for(int i=0; i<doubleParameters.size(); i++) {
        if(doubleParameters[i].values.size() > 0 && doubleParameters[i].name.compare("wet") == 0) {
          this->ccrma.setwetr(doubleParameters[i].values[0]);
        }
        else if(doubleParameters[i].values.size() > 0 && doubleParameters[i].name.compare("dry") == 0) {
          this->ccrma.setdryr(doubleParameters[i].values[0]);
        }
        else if(doubleParameters[i].values.size() > 0 && doubleParameters[i].name.compare("width") == 0) {
          this->ccrma.setwidth(doubleParameters[i].values[0]);
        }
        else if(doubleParameters[i].values.size() > 0 && doubleParameters[i].name.compare("feedback") == 0) {
          this->ccrma.setfeedback(doubleParameters[i].values[0]);
        }
        else if(doubleParameters[i].values.size() > 0 && doubleParameters[i].name.compare("apFeedback") == 0) {
          this->ccrma.setapfeedback(doubleParameters[i].values[0]);
        }
        else if(doubleParameters[i].values.size() > 0 && doubleParameters[i].name.compare("damp") == 0) {
          this->ccrma.setdamp(doubleParameters[i].values[0]);
        }
      }
    }
    else if(this->type == Reverb::Type::Lexicon224) {
      for(int i=0; i<doubleParameters.size(); i++) {
        if(doubleParameters[i].values.size() > 0 && doubleParameters[i].name.compare("wet") == 0) {
          this->lexicon224.setwetr(doubleParameters[i].values[0]);
        }
        else if(doubleParameters[i].values.size() > 0 && doubleParameters[i].name.compare("dry") == 0) {
          this->lexicon224.setdryr(doubleParameters[i].values[0]);
        }
        else if(doubleParameters[i].values.size() > 0 && doubleParameters[i].name.compare("width") == 0) {
          this->lexicon224.setwidth(doubleParameters[i].values[0]);
        }
        else if(doubleParameters[i].values.size() > 0 && doubleParameters[i].name.compare("diffusion") == 0) {
          this->lexicon224.setdiffusion1(doubleParameters[i].values[0]);
        }
        else if(doubleParameters[i].values.size() > 0 && doubleParameters[i].name.compare("damp") == 0) {
          this->lexicon224.setdamp(doubleParameters[i].values[0]);
        }
        else if(doubleParameters[i].values.size() > 0 && doubleParameters[i].name.compare("inputDamp") == 0) {
          this->lexicon224.setinputdamp(doubleParameters[i].values[0]);
        }
        else if(doubleParameters[i].values.size() > 0 && doubleParameters[i].name.compare("outputDamp") == 0) {
          this->lexicon224.setoutputdamp(doubleParameters[i].values[0]);
        }
        else if(doubleParameters[i].values.size() > 0 && doubleParameters[i].name.compare("rt60") == 0) {
          this->lexicon224.setrt60(doubleParameters[i].values[0]);
        }
        else if(doubleParameters[i].values.size() > 0 && doubleParameters[i].name.compare("dcCutFrequency") == 0) {
          this->lexicon224.setdccutfreq(doubleParameters[i].values[0]);
        }
      }
    }
    else if(this->type == Reverb::Type::Progenitor) {
      for(int i=0; i<doubleParameters.size(); i++) {
        if(doubleParameters[i].values.size() > 0 && doubleParameters[i].name.compare("wet") == 0) {
          this->progenitor.setwetr(doubleParameters[i].values[0]);
        }
        else if(doubleParameters[i].values.size() > 0 && doubleParameters[i].name.compare("dry") == 0) {
          this->progenitor.setdryr(doubleParameters[i].values[0]);
        }
        else if(doubleParameters[i].values.size() > 0 && doubleParameters[i].name.compare("width") == 0) {
          this->progenitor.setwidth(doubleParameters[i].values[0]);
        }
        else if(doubleParameters[i].values.size() > 0 && doubleParameters[i].name.compare("diffusion1") == 0) {
          this->progenitor.setdiffusion1(doubleParameters[i].values[0]);
        }
        else if(doubleParameters[i].values.size() > 0 && doubleParameters[i].name.compare("diffusion2") == 0) {
          this->progenitor.setdiffusion2(doubleParameters[i].values[0]);
        }
        else if(doubleParameters[i].values.size() > 0 && doubleParameters[i].name.compare("diffusion3") == 0) {
          this->progenitor.setdiffusion3(doubleParameters[i].values[0]);
        }
        else if(doubleParameters[i].values.size() > 0 && doubleParameters[i].name.compare("diffusion4") == 0) {
          this->progenitor.setdiffusion4(doubleParameters[i].values[0]);
        }
        else if(doubleParameters[i].values.size() > 0 && doubleParameters[i].name.compare("damp") == 0) {
          this->progenitor.setdamp(doubleParameters[i].values[0]);
        }
        else if(doubleParameters[i].values.size() > 0 && doubleParameters[i].name.compare("inputDamp") == 0) {
          this->progenitor.setinputdamp(doubleParameters[i].values[0]);
        }
        else if(doubleParameters[i].values.size() > 0 && doubleParameters[i].name.compare("outputDamp") == 0) {
          this->progenitor.setoutputdamp(doubleParameters[i].values[0]);
        }
        else if(doubleParameters[i].values.size() > 0 && doubleParameters[i].name.compare("outputBandwidth") == 0) {
          this->progenitor.setoutputdampbw(doubleParameters[i].values[0]);
        }
        else if(doubleParameters[i].values.size() > 0 && doubleParameters[i].name.compare("spin") == 0) {
          this->progenitor.setspin(doubleParameters[i].values[0]);
        }
        else if(doubleParameters[i].values.size() > 0 && doubleParameters[i].name.compare("decay1") == 0) {
          this->progenitor.setdecay0(doubleParameters[i].values[0]);
        }
        else if(doubleParameters[i].values.size() > 0 && doubleParameters[i].name.compare("decay2") == 0) {
          this->progenitor.setdecay1(doubleParameters[i].values[0]);
        }
        else if(doubleParameters[i].values.size() > 0 && doubleParameters[i].name.compare("decay3") == 0) {
          this->progenitor.setdecay2(doubleParameters[i].values[0]);
        }
        else if(doubleParameters[i].values.size() > 0 && doubleParameters[i].name.compare("decay4") == 0) {
          this->progenitor.setdecay3(doubleParameters[i].values[0]);
        }
      }
    }
    else if(this->type == Reverb::Type::Zita) {
      for(int i=0; i<doubleParameters.size(); i++) {
        if(doubleParameters[i].values.size() > 0 && doubleParameters[i].name.compare("wet") == 0) {
          this->zita.setwetr(doubleParameters[i].values[0]);
        }
        else if(doubleParameters[i].values.size() > 0 && doubleParameters[i].name.compare("dry") == 0) {
          this->zita.setdryr(doubleParameters[i].values[0]);
        }
        else if(doubleParameters[i].values.size() > 0 && doubleParameters[i].name.compare("width") == 0) {
          this->zita.setwidth(doubleParameters[i].values[0]);
        }
        else if(doubleParameters[i].values.size() > 0 && doubleParameters[i].name.compare("diffusion") == 0) {
          this->zita.setidiffusion1(doubleParameters[i].values[0]);
        }
        else if(doubleParameters[i].values.size() > 0 && doubleParameters[i].name.compare("rt60") == 0) {
          this->zita.setrt60(doubleParameters[i].values[0]);
        }
        else if(doubleParameters[i].values.size() > 0 && doubleParameters[i].name.compare("rt60factorLow") == 0) {
          this->zita.setrt60_factor_low(doubleParameters[i].values[0]);
        }
        else if(doubleParameters[i].values.size() > 0 && doubleParameters[i].name.compare("rt60factorHigh") == 0) {
          this->zita.setrt60_factor_high(doubleParameters[i].values[0]);
        }
        else if(doubleParameters[i].values.size() > 0 && doubleParameters[i].name.compare("dcCutFrequency") == 0) {
          this->zita.setdccutfreq(doubleParameters[i].values[0]);
        }
        else if(doubleParameters[i].values.size() > 0 && doubleParameters[i].name.compare("lfo1") == 0) {
          this->zita.setlfo1freq(doubleParameters[i].values[0]);
        }
        else if(doubleParameters[i].values.size() > 0 && doubleParameters[i].name.compare("lfo2") == 0) {
          this->zita.setlfo2freq(doubleParameters[i].values[0]);
        }
        else if(doubleParameters[i].values.size() > 0 && doubleParameters[i].name.compare("lfoFactor") == 0) {
          this->zita.setlfofactor(doubleParameters[i].values[0]);
        }
        else if(doubleParameters[i].values.size() > 0 && doubleParameters[i].name.compare("spin") == 0) {
          this->zita.setspin(doubleParameters[i].values[0]);
        }
        else if(doubleParameters[i].values.size() > 0 && doubleParameters[i].name.compare("spinFactor") == 0) {
          this->zita.setspinfactor(doubleParameters[i].values[0]);
        }
        else if(doubleParameters[i].values.size() > 0 && doubleParameters[i].name.compare("xOverLow") == 0) {
          this->zita.setxover_low(doubleParameters[i].values[0]);
        }
        else if(doubleParameters[i].values.size() > 0 && doubleParameters[i].name.compare("xOverHigh") == 0) {
          this->zita.setxover_high(doubleParameters[i].values[0]);
        }
        else if(doubleParameters[i].values.size() > 0 && doubleParameters[i].name.compare("apFeedback") == 0) {
          this->zita.setapfeedback(doubleParameters[i].values[0]);
        }
        else if(doubleParameters[i].values.size() > 0 && doubleParameters[i].name.compare("lowPass") == 0) {
          this->zita.setoutputlpf(doubleParameters[i].values[0]);
        }
        else if(doubleParameters[i].values.size() > 0 && doubleParameters[i].name.compare("highPass") == 0) {
          this->zita.setoutputhpf(doubleParameters[i].values[0]);
        }
      }
    }
    
    else if(this->type == Reverb::Type::Mverb) {
      for(int i=0; i<doubleParameters.size(); i++) {
        if(doubleParameters[i].values.size() > 0 && doubleParameters[i].name.compare("mix") == 0) {
          this->mverb.setParameter(MVerb<double>::MIX, doubleParameters[i].values[0]);
        }
        else if(doubleParameters[i].values.size() > 0 && doubleParameters[i].name.compare("earlyMix") == 0) {
          this->mverb.setParameter(MVerb<double>::EARLYMIX, doubleParameters[i].values[0]);
        }
        else if(doubleParameters[i].values.size() > 0 && doubleParameters[i].name.compare("gain") == 0) {
          this->mverb.setParameter(MVerb<double>::GAIN, doubleParameters[i].values[0]);
        }
        else if(doubleParameters[i].values.size() > 0 && doubleParameters[i].name.compare("size") == 0) {
          this->mverb.setParameter(MVerb<double>::SIZE, doubleParameters[i].values[0]);
        }
        else if(doubleParameters[i].values.size() > 0 && doubleParameters[i].name.compare("density") == 0) {
          this->mverb.setParameter(MVerb<double>::DENSITY, doubleParameters[i].values[0]);
        }
        else if(doubleParameters[i].values.size() > 0 && doubleParameters[i].name.compare("decay") == 0) {
          this->mverb.setParameter(MVerb<double>::DECAY, doubleParameters[i].values[0]);
        }
        else if(doubleParameters[i].values.size() > 0 && doubleParameters[i].name.compare("preDelay") == 0) {
          this->mverb.setParameter(MVerb<double>::PREDELAY, doubleParameters[i].values[0]);
        }
        else if(doubleParameters[i].values.size() > 0 && doubleParameters[i].name.compare("dampingFrequency") == 0) {
          this->mverb.setParameter(MVerb<double>::DAMPINGFREQ, doubleParameters[i].values[0]);
        }
        else if(doubleParameters[i].values.size() > 0 && doubleParameters[i].name.compare("bandwidthFrequency") == 0) {
          this->mverb.setParameter(MVerb<double>::BANDWIDTHFREQ, doubleParameters[i].values[0]);
        }
      }
    }
  }
  
  
  
  virtual void encodeParameters() {
    clearParameters();

    //stringParameters.push_back(StringParameter("type", "reverb"));
    putStringParameter("subtype", typeNames[type]);
    putNumericParameter("gain", gain);
    
    if(type == Reverb::Type::FreeVerb) {
      putNumericParameter("wet", freeverb.getwet());
      putNumericParameter("dry", freeverb.getdry());
      putNumericParameter("width", freeverb.getwidth());
      putNumericParameter("roomSize", freeverb.getroomsize());
      putNumericParameter("damp", freeverb.getdamp());
    }
    else if(type == Reverb::Type::CCRMA) {
      putNumericParameter("wet", ccrma.getwetr());
      putNumericParameter("dry", ccrma.getdryr());
      putNumericParameter("width", ccrma.getwidth());
      putNumericParameter("feedback", ccrma.getfeedback());
      putNumericParameter("apFeedback", ccrma.getapfeedback());
      putNumericParameter("damp", ccrma.getdamp());
    }
    else if(type == Reverb::Type::Progenitor) {
      putNumericParameter("wet", progenitor.getwetr());
      putNumericParameter("dry", progenitor.getdryr());
      putNumericParameter("width", progenitor.getwidth());
      putNumericParameter("diffusion1", progenitor.getdiffusion1());
      putNumericParameter("diffusion2", progenitor.getdiffusion2());
      putNumericParameter("diffusion3", progenitor.getdiffusion3());
      putNumericParameter("diffusion4", progenitor.getdiffusion4());
      putNumericParameter("damp", progenitor.getdamp());
      putNumericParameter("inputDamp", progenitor.getinputdamp());
      putNumericParameter("outputDamp", progenitor.getoutputdamp());
      putNumericParameter("outputBandwidth", progenitor.getoutputdampbw());
      putNumericParameter("spin", progenitor.getspin());
      putNumericParameter("decay1", progenitor.getdecay0());
      putNumericParameter("decay2", progenitor.getdecay1());
      putNumericParameter("decay3", progenitor.getdecay2());
      putNumericParameter("decay4", progenitor.getdecay3());
    }
    else if(type == Reverb::Type::Lexicon224) {
      putNumericParameter("wet", lexicon224.getwetr());
      putNumericParameter("dry", lexicon224.getdryr());
      putNumericParameter("width", lexicon224.getwidth());
      putNumericParameter("diffusion", lexicon224.getdiffusion1());
      putNumericParameter("damp", lexicon224.getdamp());
      putNumericParameter("inputDamp", lexicon224.getinputdamp());
      putNumericParameter("outputDamp", lexicon224.getoutputdamp());
      putNumericParameter("rt60", lexicon224.getrt60());
      putNumericParameter("dcCutFrequency", lexicon224.getdccutfreq());
    }
    else if(type == Reverb::Type::Zita) {
      putNumericParameter("wet", zita.getwetr());
      putNumericParameter("dry", zita.getdryr());
      putNumericParameter("width", zita.getwidth());
      putNumericParameter("diffusion", zita.getidiffusion1());
      putNumericParameter("rt60", zita.getrt60());
      putNumericParameter("rt60factorLow", zita.getrt60_factor_low());
      putNumericParameter("rt60factorHigh", zita.getrt60_factor_high());
      putNumericParameter("dcCutFrequency", zita.getdccutfreq());
      putNumericParameter("lfo1", zita.getlfo1freq());
      putNumericParameter("lfo2", zita.getlfo2freq());
      putNumericParameter("lfoFactor", zita.getlfofactor());
      putNumericParameter("spin", zita.getspin());
      putNumericParameter("spinFactor", zita.getspinfactor());
      putNumericParameter("xOverLow", zita.getxover_low());
      putNumericParameter("xOverHigh", zita.getxover_high());
      putNumericParameter("apFeedback", zita.getapfeedback());
      putNumericParameter("lowPass", zita.getoutputlpf());
      putNumericParameter("highPass", zita.getoutputhpf());
    }
    else if(type == Reverb::Type::Mverb) {
      putNumericParameter("mix", mverb.getParameter(MVerb<double>::MIX));
      putNumericParameter("earlyMix", mverb.getParameter(MVerb<double>::EARLYMIX));
      //putNumericParameter("gain", mverb.getParameter(MVerb<double>::GAIN)));
      putNumericParameter("size", mverb.getParameter(MVerb<double>::SIZE));
      putNumericParameter("density", mverb.getParameter(MVerb<double>::DENSITY));
      putNumericParameter("decay", mverb.getParameter(MVerb<double>::DECAY));
      putNumericParameter("preDelay", mverb.getParameter(MVerb<double>::PREDELAY));
      putNumericParameter("dampingFrequency", mverb.getParameter(MVerb<double>::DAMPINGFREQ));
      putNumericParameter("bandwidthFrequency", mverb.getParameter(MVerb<double>::BANDWIDTHFREQ));
    }

    if(presetName.size() > 0) {
      putStringParameter("presetName", presetName);
    }
  }
  
  
  
  
};
