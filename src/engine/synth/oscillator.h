#pragma once

//#include "../FastNoise/FastNoise.h"
#include "waveform.h"
#include "envelope.h"
#include "modularparameter.h"
#include "filter/biquad.h"
#include "voicecross.h"

struct VoicePreset {
  std::string name;
  EnvelopePreset amplitudeEnvelopePreset;
  EnvelopePreset frequencyEnvelopePreset;
  PartialPreset partialPreset;
  double volume = 0.25, tuning = 1, stereoPhaseOffset = 0.5;
  Vec2d startPhaseOffsetLimits;
  bool startPhaseOffsetsLocked = true;
  int unison = 1;
  double unisonDetuning = 0.01;
  double gaussianSmoothing = 0.0;
  int detuningRandomLeft = 4, detuningRandomRight = 14;
  WaveForm::Type waveFormType = WaveForm::Type::Sin;
  std::vector<double> waveFormArgs;
  WaveForm::WaveTableMode waveTableMode;
  int numSteps = 1, numPartials = 1;

  VoicePreset() {}
  VoicePreset(const std::string name, double volume, double tuning, const EnvelopePreset &amplitudeEnvelopePreset, const EnvelopePreset &frequencyEnvelopePreset, const PartialPreset &partialPreset, WaveForm::Type waveFormType, const std::vector<double> &waveFormArgs, WaveForm::WaveTableMode waveTableMode, int numSteps = 1, int numPartials = 1, int unison = 1, double unisonDetuning = 0.01, double gaussianSmoothing = 0.0, double stereoPhaseOffset = 0.6, double startPhaseOffsetMin = 0, double startPhaseOffsetMax = 0, bool startPhaseOffsetsLocked = true, int detuningRandomLeft = 4, int detuningRandomRight = 14) {
    this->name = name;
    this->volume = volume;
    this->tuning = tuning;
    this->amplitudeEnvelopePreset = amplitudeEnvelopePreset;
    this->frequencyEnvelopePreset = frequencyEnvelopePreset;
    this->partialPreset = partialPreset;
    this->waveFormType = waveFormType;
    this->waveFormArgs = std::vector<double>(waveFormArgs);
    this->waveTableMode = waveTableMode;
    this->numSteps = numSteps;
    this->numPartials = numPartials;
    this->stereoPhaseOffset = stereoPhaseOffset;
    this->startPhaseOffsetLimits.set(startPhaseOffsetMin, startPhaseOffsetMax);
    this->startPhaseOffsetsLocked = startPhaseOffsetsLocked;
    this->unison = unison;
    this->unisonDetuning = unisonDetuning;
    this->detuningRandomLeft = detuningRandomLeft;
    this->detuningRandomRight = detuningRandomRight;
    this->gaussianSmoothing = gaussianSmoothing;
  }
  VoicePreset(const std::string name, double volume, double tuning, const EnvelopePreset &amplitudeEnvelopePreset, const EnvelopePreset &frequencyEnvelopePreset, const PartialPreset &partialPreset, WaveForm::Type waveFormType, WaveForm::WaveTableMode waveTableMode, int numSteps = 1, int numPartials = 1, int unison = 1, double unisonDetuning = 0.01, double gaussianSmoothing = 0.0, double stereoPhaseOffset = 0.6, double startPhaseOffsetMin = 0, double startPhaseOffsetMax = 0, bool startPhaseOffsetsLocked = true, int detuningRandomLeft = 4, int detuningRandomRight = 14) {
    this->name = name;
    this->volume = volume;
    this->tuning = tuning;
    this->amplitudeEnvelopePreset = amplitudeEnvelopePreset;
    this->frequencyEnvelopePreset = frequencyEnvelopePreset;
    this->partialPreset = partialPreset;
    this->waveFormType = waveFormType;
    this->waveTableMode = waveTableMode;
    this->numSteps = numSteps;
    this->numPartials = numPartials;
    this->stereoPhaseOffset = stereoPhaseOffset;
    this->startPhaseOffsetLimits.set(startPhaseOffsetMin, startPhaseOffsetMax);
    this->startPhaseOffsetsLocked = startPhaseOffsetsLocked;
    this->unison = unison;
    this->unisonDetuning = unisonDetuning;
    this->detuningRandomLeft = detuningRandomLeft;
    this->detuningRandomRight = detuningRandomRight;
    this->gaussianSmoothing = gaussianSmoothing;
  }
};

/*
VoicePreset(
  const std::string name,
  double volume,
  double tuning,
  const EnvelopePreset &amplitudeEnvelopePreset,
  const EnvelopePreset &frequencyEnvelopePreset,
  const PartialPreset &partialPreset,
  WaveForm::Type waveFormType,
  const std::vector<double> &waveFormArgs,
  WaveForm::WaveTableMode waveTableMode,
  int numSteps = 1,
  int numPartials = 1,
  int unison = 1,
  double unisonDetuning = 0.01,
  double gaussianSmoothing = 0.0,
  double stereoPhaseOffset = 0.6,
  double startPhaseOffsetMin = 0,
  double startPhaseOffsetMax = 0,
  bool startPhaseOffsetsLocked = true,
  int detuningRandomLeft = 4,
  int detuningRandomRight = 14)
*/

static VoicePreset vpDefault("Default", 0.25, 1, epAHE01, epFourpoint03, ppDefault, WaveForm::Type::Sin, {1}, WaveForm::WaveTableMode::Single, 1, 1, 5, 0.01);

// FIXME
static VoicePreset vpVocal01("Vocal 01", 0.25, 1, epAHE01, epFourpoint03, ppVocal03, WaveForm::Type::Wave08, {10}, WaveForm::WaveTableMode::Single, 1, 20, 5, 0.003);

static VoicePreset vpVocal02("Vocal 02 (aah, 36-60)", 0.25, 1, epAHE09, epFourpoint03, ppBuzzy01, WaveForm::Type::Sin, {1}, WaveForm::WaveTableMode::SingleFaster, 42, 7, 12, 0.005, 0.05);
static VoicePreset vpVocal03("Vocal 03 (aah, 48-77)", 0.25, 1, epAHE09, epFourpoint03, ppBuzzy01, WaveForm::Type::Sin, {1}, WaveForm::WaveTableMode::SingleFaster, 35, 3, 12, 0.005, 0.05);
static VoicePreset vpVocal04("Vocal 04 (aah, 55-84)", 0.25, 1, epAHE09, epFourpoint03, ppBuzzy01, WaveForm::Type::Sin, {1}, WaveForm::WaveTableMode::SingleFaster, 15, 11, 12, 0.005, 0.05);
static VoicePreset vpVocal05("Vocal 05 (huu, 48-84)", 0.25, 1, epAHE09, epFourpoint03, ppBuzzy01, WaveForm::Type::Sin, {1}, WaveForm::WaveTableMode::SingleFaster, 25, 22, 12, 0.005, 0.05);


static VoicePreset vpStrings01("Strings 01", 0.25, 1, epAHE12, epFourpoint03, ppDefault, WaveForm::Type::Saw, WaveForm::WaveTableMode::SingleFaster, 1, 1, 4, 0.008, 0.06, 0, 0, 1, false, 6, 50);
static VoicePreset vpStrings02("Strings 01 (pizz.)", 0.25, 1, epPAD06, epFourpoint03, ppDefault, WaveForm::Type::Saw, WaveForm::WaveTableMode::SingleFaster, 1, 1, 4, 0.008, 0.06, 0, 0, 1, false, 6, 50);


static VoicePreset vpOrgan01("Organ 01", 0.25, 1, epFourpoint04, epFourpoint03, ppOrgan01, WaveForm::Type::Wave02, {1}, WaveForm::WaveTableMode::SingleFaster, 1, 8);
// + multidelay preset 2, output 100, 1..1, 0..40000, more volume

// FIXME
static VoicePreset vpGuitar01("Guitar 01", 0.25, 1, epFourpoint02, epFourpoint03, ppNameless05, WaveForm::Type::Wave09, WaveForm::WaveTableMode::SingleFaster, 1, 20, 4, 0.001);
//+ AM, relative mode, depth 1, frequency x, with different waveforms and partials surprsingly usable for different kind of guitar sounds, maybe harpsichord too

static VoicePreset vpBass01("Bass 01", 0.25, 1, epAHE11, epFourpoint03, ppPiano02, WaveForm::Type::Sin, WaveForm::WaveTableMode::SingleFaster, 1, 10, 2, 0.001, 0.1, 0, 0.021, 0.021);



static VoicePreset vpPiano01("Piano 01", 0.25, 1, epAHE02, epFourpoint03, ppPiano01, WaveForm::Type::Wave13, {2, 2, 10}, WaveForm::WaveTableMode::ForEachPartialFaster, 1, 8, 2, 0.001);
// + Multidelay preset 3, wet 0.9, gain 1.1

static VoicePreset vpPiano02("Piano 02", 0.25, 1, epAHE05, epFourpoint03, ppPiano02, WaveForm::Type::Wave06, WaveForm::WaveTableMode::SingleFaster, 1, 16, 2, 0.004);

static VoicePreset vpFlute01("Flute 01", 0.25, 1, epADSR01, epFourpoint03, ppFlute02, WaveForm::Type::Sin, {1}, WaveForm::WaveTableMode::SingleFaster, 1, 14, 3, 0.0006);
// + FM, depth 0.0005, freq 5, sin

static VoicePreset vpThunder01("Thunder 01", 1.0, 1, epPAD02, epPAD01, ppDefault, WaveForm::Type::GradientNoise, {FastNoise::NoiseType::SimplexFractal, 1.0, -1, -1, -1, 5}, WaveForm::WaveTableMode::None, 1, 1, 1, 0, 0, 0.6, 0, 4000, false);


static VoicePreset vpPercussive01("Percussive 01", 0.25, 1, epAHE03, epAHE04, ppMetallic04, WaveForm::Type::Wave06, {1}, WaveForm::WaveTableMode::SingleFaster, 1, 20, 6, 0.5);

static VoicePreset vpPercussive02("Percussive 02", 1, 1, epAHE05, epAHE05, ppDefault, WaveForm::Type::GradientNoise, {FastNoise::NoiseType::Simplex, 1.0}, WaveForm::WaveTableMode::None, 1, 1, 3, 2.0);
static VoicePreset vpPercussive03a("Percussive 03a", 1, 1, epAHE05, epAHE06, ppDefault, WaveForm::Type::Sin, {1.0}, WaveForm::WaveTableMode::Single, 2, 1, 2, 1.0, 0.08);
static VoicePreset vpPercussive03b("Percussive 03b", 2, 1, epAHE05, epAHE06, ppNameless06, WaveForm::Type::Wave06, WaveForm::WaveTableMode::ForEachPartial, 1, 6, 3, 0.5, 0.4);
static VoicePreset vpPercussive03c("Percussive 03c", 2, 1, epAHE05, epAHE06, ppNameless06, WaveForm::Type::Wave06, WaveForm::WaveTableMode::ForEachPartial, 35, 6, 3, 0.5);

static VoicePreset vpPercussive04("Percussive 04", 1.0, 1, epPAD03, epPAD04, ppDefault, WaveForm::Type::GradientNoise, {FastNoise::NoiseType::Simplex, 40.0, -1, -1, -1, 5}, WaveForm::WaveTableMode::None, 1, 1, 1, 0, 0, 0.6, 0, 4000, false);

static VoicePreset vpPercussive05("Percussive 05", 1.0, 1, epPAD05, epPAD06, ppBuzzy02, WaveForm::Type::Wave08, {2}, WaveForm::WaveTableMode::SingleFaster, 1, 25, 1, 0.5, 0.2, 0);

static VoicePreset vpPercussive06("Percussive 06", 1.0, 1, epPAD05, epPAD07, ppBuzzy02, WaveForm::Type::Sin, {1}, WaveForm::WaveTableMode::SingleFaster, 1, 1, 1, 0.5, 0, 0);

static VoicePreset vpPercussive07("Percussive 07", 2.0, 2, epPAD05, epPAD08, ppBuzzy02, WaveForm::Type::Wave04, {2}, WaveForm::WaveTableMode::SingleFaster, 1, 1, 2, 0.5, 0, 0);

static VoicePreset vpPercussive08("Percussive 08", 2.0, 2, epPAD05, epPAD08, ppBuzzy02, WaveForm::Type::Wave04, {2}, WaveForm::WaveTableMode::SingleFaster, 1, 8, 2, 0.5, 0, 0);

static VoicePreset vpPercussive09("Percussive 09 (bell)", 1.0, 1, epPAD05, epFourpoint03, ppBuzzy02, WaveForm::Type::Wave03, WaveForm::WaveTableMode::Single, 1, 14, 1, 0, 0, 0);

static VoicePreset vpPercussive10("Percussive 10 (kick)", 1.0, 1, epPAD11, epPAD10, ppDefault, WaveForm::Type::GradientNoise, {FastNoise::NoiseType::Simplex, 1.0}, WaveForm::WaveTableMode::None, 1, 1, 1, 3);

static VoicePreset vpMallet01("Mallet 01 (metal)", 0.25, 1, epFourpoint01, epFourpoint03, ppMetallic01, WaveForm::Type::Sin, {1}, WaveForm::WaveTableMode::ForEachPartialFaster, 1, 8, 3, 0.0001);

static VoicePreset vpMallet02("Mallet 02 (wooden)", 1.0, 1, epPAD09, epFourpoint03, ppBuzzy02, WaveForm::Type::Wave04, {4}, WaveForm::WaveTableMode::SingleFaster, 1, 9, 4, 0.006, 0.6, 0, 0.087, 0.087);

static VoicePreset vpAccordion01("Accordion 01", 1.0, 1, epAHE10, epFourpoint03, ppNameless07, WaveForm::Type::Wave06, {4}, WaveForm::WaveTableMode::SingleFaster, 1, 21);


// FIXME worked only with these parameters when created, because of a bug
static VoicePreset vpCrystal01("Crystal 01", 0.25, 1, epAHE01, epFourpoint03, ppCrystal01, WaveForm::Type::Sin, {1}, WaveForm::WaveTableMode::SingleFaster, 1, 3, 3, 0.01, 0.05);

static VoicePreset vpFuzzy01("Soft 01", 0.25, 1, epFivepoint03, epFourpoint03, ppBuzzy01, WaveForm::Type::Sin, {1}, WaveForm::WaveTableMode::SingleFaster, 1, 50, 1, 0.001, 0.5);

static VoicePreset vpPad01("Pad 01 (piano)", 0.25, 1, epAHE07, epFourpoint03, ppBuzzy02, WaveForm::Type::Sin, {1}, WaveForm::WaveTableMode::SingleFaster, 1, 40, 2, 0.003, 0.08);

static VoicePreset vpPad02("Pad 02 (fuzzy)", 0.25, 1, epAHE08, epFourpoint03, ppBuzzy02, WaveForm::Type::Sinc, {30}, WaveForm::WaveTableMode::SingleFaster, 1, 50, 5, 0.006);

static VoicePreset vpPad03("Pad 03 (saw)", 0.25, 1, epAHE08, epFourpoint03, ppBuzzy02, WaveForm::Type::Saw, WaveForm::WaveTableMode::SingleFaster, 1, 1, 6, 0.009);

static std::vector<VoicePreset> voicePresets = { vpDefault, vpFuzzy01, vpCrystal01, vpPad01, vpPad02, vpPad03, vpVocal01, vpVocal02, vpVocal03, vpVocal04, vpVocal05, vpStrings01, vpStrings02, vpOrgan01, vpGuitar01, vpBass01, vpPiano01, vpPiano02, vpFlute01, vpAccordion01, vpMallet01, vpMallet02, vpThunder01, vpPercussive01, vpPercussive02, vpPercussive03a, vpPercussive03b, vpPercussive03c, vpPercussive04, vpPercussive05, vpPercussive06, vpPercussive07, vpPercussive08, vpPercussive09, vpPercussive10  };


static FastNoise fastNoiseUnisonDetuning;

static void initOscillatorGlobals() {
  fastNoiseUnisonDetuning.SetNoiseType(FastNoise::NoiseType::Value);
  fastNoiseUnisonDetuning.SetFrequency(1000.0);
  fastNoiseUnisonDetuning.SetInterp(FastNoise::Interp::Linear);
}

struct Voice : public PanelInterface, public HierarchicalTextFileParser {
  
  //enum PhaseMode { LeftAndRightPhase, RandomFromRangeSameLR, RandomFromRangeDifferentLR, UnisonsEvenlyDistributed };
  //const std::vector<std::string> phaseModeNames = { "Left/right", "Random, same L/R", "Random, different L/R", "Unisons evenly distributed" };
  
  //PhaseMode phaseMode = LeftAndRightPhase;
  
  /*enum FrequencyModulationAlgorithm { FmAlg1, FmAlg2, FmAlg3 };
  const std::vector<std::string> frequencyModulationAlgorithmNames = { "FmAlg1", "FmAlg2", "FmAlg3" };
  
  FrequencyModulationAlgorithm frequencyModulationAlgorithm = FmAlg1;*/
  
  bool isActive = true;
  
  double volume = 0.25;
  double tuning = 1.0;
  WaveForm waveForm;

  int unison = 1;
  double unisonDetuning = 0.001;
  std::vector<Vec2d> unisonTunings, unisonTuningsModular;

  int unisonDetuningRandomSeedLeft = 0;
  int unisonDetuningRandomSeedRight = 0;

  double stereoPhaseOffset = 0;
  double pan = 0;

  ModularParameter unisonDetuningModular;
  ModularParameter stereoPhaseOffsetModular;
  ModularParameter panModular;
  ModularParameter frequencyModular;
  ModularParameter amplitudeModular;
  // phases
  
  ModularParameter filterFrequencyModular;
  ModularParameter filterBandwidthModular;
  ModularParameter filterWetLevelModular;

  double filterFrequency;
  double filterBandwidth;
  double filterWetLevel;
  
  std::vector<ModularParameter*> modularParameters;
  
  bool areModularConnections = false;
  
  bool biquadChanged = false;
  
  enum KeyMappingMode { Normal, FixedFrequency };
  const std::vector<std::string> keyMappingModeNames { "Normal", "Fixed" };
  KeyMappingMode keyMappingMode = KeyMappingMode::Normal;
  double fixedFrequency = 440;

  const std::vector<std::string> audioSourceNames = { "Synthesis", "Line in" };
  enum AudioSource { Synthesis, LineIn };
  AudioSource audioSource = Synthesis;


  void checkModularConnections() {
    areModularConnections = false;
    for(int i=0; i<modularParameters.size(); i++) {
      if(modularParameters[i]->checkConnected()) {
        areModularConnections = true;
      }
    }
  }

  struct ModularParameterListener : public GuiEventListener {
    Voice *voice;
    ModularParameterListener(Voice *voice) {
      this->voice = voice;
    }
    virtual void onMessage(GuiElement *guiElement, const std::string &message) {
      voice->checkModularConnections();
    }
  };

  int totalOscillatorCount = 1;

  std::vector<double> partialSamplesOutLeft, partialSamplesOutRight;

  int inputAM = 0, inputFM = 0, inputAmplitudeEnvelope = 0, inputFrequencyEnvelope = -1;

  //bool stopAllNotesRequested = false;
  //long currentInitializeID = 0;
  
  BiquadFilter *biquadFilter = NULL;
  
  void init(DelayLine *delayLine) {
    if(biquadFilter) delete biquadFilter;
    biquadFilter = new BiquadFilter(delayLine);
    biquadFilter->isActive = false;
    biquadFilter->showTrackingPanel = true;
    
    modularParameters.clear();
    
    filterFrequency = biquadFilter->frequency;
    filterBandwidth = biquadFilter->bandwidth;
    filterWetLevel = biquadFilter->wetLevel;
    initModularParameters();
  }
  
  void init(DelayLine *delayLine, BiquadFilter *biquadFilter) {
    if(biquadFilter) delete biquadFilter;
    biquadFilter = new BiquadFilter(delayLine, *biquadFilter);
    filterFrequency = biquadFilter->frequency;
    filterBandwidth = biquadFilter->bandwidth;
    filterWetLevel = biquadFilter->wetLevel;
    
    modularParameters.clear();
    
    initModularParameters();
  }


  void initModularParameters() {
    //modularParameters.push_back(&frequencyModular.init("Frequency", NULL, -1e10, 1e10, -0.01, 0.01, new ModularParameterListener(this)));
    //modularParameters.push_back(&amplitudeModular.init("Amplitude", NULL, 0, 1, 0, 1, new ModularParameterListener(this)));
    modularParameters.push_back(&unisonDetuningModular.init("Unison detuning", &unisonDetuning, 0, 1e10, 0, 0.1, new ModularParameterListener(this)));
    modularParameters.push_back(&stereoPhaseOffsetModular.init("Stereo phase offset", &stereoPhaseOffset, -1e10, 1e10, 0, 1, new ModularParameterListener(this)));
    modularParameters.push_back(&panModular.init("Pan", &pan, -1, 1, -1, 1, new ModularParameterListener(this)));
    
    modularParameters.push_back(&filterFrequencyModular.init("Filter frequency", &filterFrequency, 0, biquadFilter->sampleRate/2.0, 100, 1000, new ModularParameterListener(this)));
    modularParameters.push_back(&filterBandwidthModular.init("Filter bandwidth", &filterBandwidth, 0, 1e10, 0.5, 5, new ModularParameterListener(this)));
    modularParameters.push_back(&filterWetLevelModular.init("Filter wet", &filterWetLevel, 0, 1, 0, 1, new ModularParameterListener(this)));
  }

  void update() {
    //printf("(debugging) at Voice.update()...\n");
    prepare();
    waveForm.update();
  }

  Voice() {
    partialSamplesOutLeft.resize(PartialSet::maxNumPartials);
    partialSamplesOutRight.resize(PartialSet::maxNumPartials);

    prepare();
  }

  inline Voice &operator=(const Voice &o) {
    this->volume = o.volume;
    this->tuning = o.tuning;
    this->unison = o.unison;
    this->waveForm = o.waveForm;
    this->unisonDetuning = o.unisonDetuning;
    this->unisonDetuningRandomSeedLeft = o.unisonDetuningRandomSeedLeft;
    this->unisonDetuningRandomSeedRight = o.unisonDetuningRandomSeedRight;
    this->unisonTunings = o.unisonTunings;
    this->stereoPhaseOffset = o.stereoPhaseOffset;
    //this->fastNoiseUnisonDetuning = o.fastNoiseUnisonDetuning;
    this->inputAM = o.inputAM;
    this->inputFM = o.inputFM;
    this->inputAmplitudeEnvelope = o.inputAmplitudeEnvelope;
    this->inputFrequencyEnvelope = o.inputFrequencyEnvelope;
    this->pan = o.pan;
    
    this->keyMappingMode = o.keyMappingMode;
    this->audioSource = o.audioSource;
    // FIXME
    //this->biquadFilter->set(*o.biquadFilter);
    
    return *this;
  }

  void setVoicePreset(int index) {
    if(index >= 0 && index < voicePresets.size()) {
      setVoicePreset(voicePresets[index]);
    }
  }
  void setVoicePreset(const VoicePreset &voicePreset) {
    waveForm.type = voicePreset.waveFormType;
    if(voicePreset.waveFormType == WaveForm::Type::GradientNoise) {
      if(voicePreset.waveFormArgs.size() > 0 && voicePreset.waveFormArgs[0] >= 0) waveForm.fastNoise.SetNoiseType((FastNoise::NoiseType)voicePreset.waveFormArgs[0]);
      if(voicePreset.waveFormArgs.size() > 1 && voicePreset.waveFormArgs[1] >= 0) waveForm.fastNoise.SetFrequency(voicePreset.waveFormArgs[1]);
      if(voicePreset.waveFormArgs.size() > 2 && voicePreset.waveFormArgs[2] >= 0) waveForm.fastNoise.SetInterp((FastNoise::Interp)voicePreset.waveFormArgs[2]);
      if(voicePreset.waveFormArgs.size() > 3 && voicePreset.waveFormArgs[3] >= 0) waveForm.fastNoise.SetSeed((int)voicePreset.waveFormArgs[3]);
      if(voicePreset.waveFormArgs.size() > 4 && voicePreset.waveFormArgs[4] >= 0) waveForm.fastNoise.SetFractalType((FastNoise::FractalType)voicePreset.waveFormArgs[4]);
      if(voicePreset.waveFormArgs.size() > 5 && voicePreset.waveFormArgs[5] >= 0) waveForm.fastNoise.SetFractalOctaves((int)voicePreset.waveFormArgs[5]);
      if(voicePreset.waveFormArgs.size() > 6 && voicePreset.waveFormArgs[6] >= 0) waveForm.fastNoise.SetFractalLacunarity(voicePreset.waveFormArgs[6]);
      if(voicePreset.waveFormArgs.size() > 7 && voicePreset.waveFormArgs[7] >= 0) waveForm.fastNoise.SetFractalGain(voicePreset.waveFormArgs[7]);
      if(voicePreset.waveFormArgs.size() > 8 && voicePreset.waveFormArgs[8] >= 0) waveForm.fastNoise.SetCellularDistanceFunction((FastNoise::CellularDistanceFunction)voicePreset.waveFormArgs[8]);
      if(voicePreset.waveFormArgs.size() > 9 && voicePreset.waveFormArgs[9] >= 0) waveForm.fastNoise.SetCellularReturnType((FastNoise::CellularReturnType)voicePreset.waveFormArgs[9]);
    }
    else {
      int n = min(waveForm.waveFormArgs[waveForm.type].size(), voicePreset.waveFormArgs.size());
      for(int i=0; i<n; i++) {
        waveForm.waveFormArgs[waveForm.type][i] = voicePreset.waveFormArgs[i];
      }
    }
    volume = voicePreset.volume;
    tuning = voicePreset.tuning;
    waveForm.waveTableMode = voicePreset.waveTableMode;
    waveForm.numSteps = voicePreset.numSteps;
    waveForm.partialSet.numPartials = voicePreset.numPartials;
    waveForm.smoothingWindowLengthInCycles = voicePreset.gaussianSmoothing;
    waveForm.sameLeftAndRightPhase = voicePreset.startPhaseOffsetsLocked;
    waveForm.phaseStartLimits = voicePreset.startPhaseOffsetLimits;
    stereoPhaseOffset = voicePreset.stereoPhaseOffset;
    unison = voicePreset.unison;
    unisonDetuning = voicePreset.unisonDetuning;
    unisonDetuningRandomSeedLeft = voicePreset.detuningRandomLeft;
    unisonDetuningRandomSeedRight = voicePreset.detuningRandomRight;
    //amplitudeEnvelope.setPreset(voicePreset.amplitudeEnvelopePreset); 
    //frequencyEnvelope.setPreset(voicePreset.frequencyEnvelopePreset);
    waveForm.setPartialPreset(voicePreset.partialPreset);

    prepare();
  }

  void prepare() {
    if(unisonTunings.size() != unison) {
      unisonTunings.resize(unison);
      unisonTuningsModular.resize(unison);
    }

    for(int i=0; i<unison; i++) {
      int u = i - unison/2;
      if(unisonDetuningRandomSeedLeft == 0) {
        unisonTunings[i].x = tuning + unisonDetuning * u * 2.0 / unison;
      }
      else {
        unisonTunings[i].x = tuning + fastNoiseUnisonDetuning.GetValue(i, unisonDetuningRandomSeedLeft)*unisonDetuning;
      }
      if(unisonDetuningRandomSeedRight == 0) {
        unisonTunings[i].y = tuning + unisonDetuning * u * 2.0 / unison;
      }
      else {
        unisonTunings[i].y = tuning + fastNoiseUnisonDetuning.GetValue(i, unisonDetuningRandomSeedRight)*unisonDetuning;
      }
    }
    
    memcpy(unisonTuningsModular.data(), unisonTunings.data(), sizeof(unisonTunings[0]) * unison);
  }


  int getTotalOscillatorCount() {
    if(waveForm.partialSet.numPartials > 1 && (waveForm.waveTableMode == WaveForm::WaveTableMode::None || waveForm.waveTableMode == WaveForm::WaveTableMode::ForEachPartial || waveForm.waveTableMode == WaveForm::WaveTableMode::ForEachPartialFaster)) {
      return waveForm.partialSet.numPartials * unison;
    }
    else {
      return unison;
    }
  }
  
  
  // FIXME
  void applyFilterModulars(const std::vector<Vec2d> &modulatorOutputs, const std::vector<double> &envelopeOutputs, double &noteBiquadFilterFrequency, double &noteBiquadFilterBandwidth, double &noteBiquadFilterWetLevel) {
    if(filterFrequencyModular.isConnected || filterBandwidthModular.isConnected || filterWetLevelModular.isConnected) {
      if(filterFrequencyModular.isConnected) {
        double env = 1, mod = 1;
        if(filterFrequencyModular.modulator >= 0) {
          mod = modulatorOutputs[filterFrequencyModular.modulator].x;
        }
        if(filterFrequencyModular.envelope >= 0) {
          env = envelopeOutputs[filterFrequencyModular.envelope];
        }
        noteBiquadFilterFrequency = mod * env * (filterFrequencyModular.range.y - filterFrequencyModular.range.x) + filterFrequencyModular.range.x;
      }
      
      if(filterBandwidthModular.isConnected) {
        double env = 1, mod = 1;
        if(filterBandwidthModular.modulator >= 0) {
          mod = modulatorOutputs[filterBandwidthModular.modulator].x;
        }
        if(filterBandwidthModular.envelope >= 0) {
          env = envelopeOutputs[filterBandwidthModular.envelope];
        }
        noteBiquadFilterBandwidth = mod * env * (filterBandwidthModular.range.y - filterBandwidthModular.range.x) + filterBandwidthModular.range.x;
      }

      if(filterWetLevelModular.isConnected) {
        double env = 1, mod = 1;
        if(filterWetLevelModular.modulator >= 0) {
          mod = modulatorOutputs[filterWetLevelModular.modulator].x;
        }
        if(filterWetLevelModular.envelope >= 0) {
          env = envelopeOutputs[filterWetLevelModular.envelope];
        }
        noteBiquadFilterWetLevel = mod * env * (filterWetLevelModular.range.y - filterWetLevelModular.range.x) + filterWetLevelModular.range.x;
      }
    }
  }
  

  inline void getSample(Vec2d &sampleOut, std::vector<Vec2d> &phases, double dt, const Vec2d &frequency, unsigned char pitch, const std::vector<Vec2d> &modulatorOutputs, const std::vector<double> &envelopeOutputs, std::vector<Vec2d> &voiceOutputs, const VoiceCrossModulation &voiceCrossModulation, int voiceIndex) {
    if(!isActive) return;
    
    double frequencyLeft = frequency.x;
    double frequencyRight = frequency.y;
    
    double left = 0, right = 0;
    
    for(int i=0; i<unison; i++) {
      left += waveForm.getSample(phases[i].x, pitch);
      right += waveForm.getSample(phases[i].y + stereoPhaseOffsetModular.value, pitch);
    }
    
    left /= unison;
    right /= unison;
    
    Vec2d sampleOutFull(left, right);
    
    if(voiceCrossModulation.isVoiceConnected[voiceIndex]) {
      if(voiceCrossModulation.mode == VoiceCrossModulationMode::CrossModulationAmplitude) {
        for(int x=0; x<voiceCrossModulation.numVoices; x++) {
          int i = x + voiceIndex*maxNumVoices;
          if(voiceCrossModulation.amplitudeFactors[i] > 0) {
            left *= (voiceOutputs[x].x*0.5 + 0.5) * voiceCrossModulation.amplitudeFactors[i] + (1.0-voiceCrossModulation.amplitudeFactors[i]);
            right *= (voiceOutputs[x].y*0.5 + 0.5) * voiceCrossModulation.amplitudeFactors[i] + (1.0-voiceCrossModulation.amplitudeFactors[i]);
          }
        }
      }
      else if(voiceCrossModulation.mode == VoiceCrossModulationMode::CrossModulationRing) {
        for(int x=0; x<voiceCrossModulation.numVoices; x++) {
          int i = x + voiceIndex*maxNumVoices;
          if(voiceCrossModulation.amplitudeFactors[i] > 0) {
            left *= voiceOutputs[x].x * voiceCrossModulation.amplitudeFactors[i] + (1.0-voiceCrossModulation.amplitudeFactors[i]);
            right *= voiceOutputs[x].y * voiceCrossModulation.amplitudeFactors[i] + (1.0-voiceCrossModulation.amplitudeFactors[i]);
          }
        }
      }
      else if(voiceCrossModulation.mode == VoiceCrossModulationMode::CrossModulationFrequency) {
        for(int x=0; x<voiceCrossModulation.numVoices; x++) {
          int i = x + voiceIndex*maxNumVoices;
          if(voiceCrossModulation.frequencyFactors[i] > 0) {
            frequencyLeft += voiceOutputs[x].x * frequency.x * voiceCrossModulation.frequencyFactors[i];
            frequencyRight += voiceOutputs[x].y * frequency.y * voiceCrossModulation.frequencyFactors[i];
          }
        }
      }
    }


    if(areModularConnections) {

      if(panModular.isConnected) {
        double env = 1, mod = 1;
        if(panModular.modulator >= 0) {
          mod = modulatorOutputs[panModular.modulator].x;
        }
        if(panModular.envelope >= 0) {
          env = envelopeOutputs[panModular.envelope];
        }
        panModular.value = mod * env * (panModular.range.y - panModular.range.x) + panModular.range.x;
      }

      if(unisonDetuningModular.isConnected) {
        double env = 1, mod = 1;
        if(unisonDetuningModular.modulator >= 0) {
          mod = modulatorOutputs[unisonDetuningModular.modulator].x;
        }
        if(stereoPhaseOffsetModular.envelope >= 0) {
          env = envelopeOutputs[unisonDetuningModular.envelope];
        }
        unisonDetuningModular.value = mod * env * (unisonDetuningModular.range.y - unisonDetuningModular.range.x) + unisonDetuningModular.range.x;

        for(int i=0; i<unison; i++) {
          unisonTuningsModular[i].x = unisonTunings[i].x * (1.0 + unisonDetuningModular.value);
          unisonTuningsModular[i].y = unisonTunings[i].y * (1.0 + unisonDetuningModular.value);
        }
      }

      if(stereoPhaseOffsetModular.isConnected) {
        double env = 1, mod = 1;
        if(stereoPhaseOffsetModular.modulator >= 0) {
          mod = modulatorOutputs[stereoPhaseOffsetModular.modulator].x;
        }
        if(stereoPhaseOffsetModular.envelope >= 0) {
          env = envelopeOutputs[stereoPhaseOffsetModular.envelope];
        }
        stereoPhaseOffsetModular.value = mod * env * (stereoPhaseOffsetModular.range.y - stereoPhaseOffsetModular.range.x) + stereoPhaseOffsetModular.range.x;
      }
      
      /*if(filterFrequencyModular.isConnected || filterBandwidthModular.isConnected || filterWetLevelModular.isConnected) {
        if(filterFrequencyModular.isConnected) {
          double env = 1, mod = 1;
          if(filterFrequencyModular.modulator >= 0) {
            mod = modulatorOutputs[filterFrequencyModular.modulator].x;
          }
          if(filterFrequencyModular.envelope >= 0) {
            env = envelopeOutputs[filterFrequencyModular.envelope];
          }
          noteBiquadFilter.frequency = mod * env * (filterFrequencyModular.range.y - filterFrequencyModular.range.x) + filterFrequencyModular.range.x;
        }
        
        if(filterBandwidthModular.isConnected) {
          double env = 1, mod = 1;
          if(filterBandwidthModular.modulator >= 0) {
            mod = modulatorOutputs[filterBandwidthModular.modulator].x;
          }
          if(filterBandwidthModular.envelope >= 0) {
            env = envelopeOutputs[filterBandwidthModular.envelope];
          }
          noteBiquadFilter.bandwidth = mod * env * (filterBandwidthModular.range.y - filterBandwidthModular.range.x) + filterBandwidthModular.range.x;
        }

        if(filterWetLevelModular.isConnected) {
          double env = 1, mod = 1;
          if(filterWetLevelModular.modulator >= 0) {
            mod = modulatorOutputs[filterWetLevelModular.modulator].x;
          }
          if(filterWetLevelModular.envelope >= 0) {
            env = envelopeOutputs[filterWetLevelModular.envelope];
          }
          noteBiquadFilter.wetLevel = mod * env * (filterWetLevelModular.range.y - filterWetLevelModular.range.x) + filterWetLevelModular.range.x;
        }
      }*/
    }

    if(panModular.value < 0) {
      sampleOut.x += volume * left;
      sampleOut.y += volume * right * (1.0+panModular.value);
    }
    else if(panModular.value > 0) {
      sampleOut.x += volume * left * (1.0-panModular.value);
      sampleOut.y += volume * right;
    }
    else {
      sampleOut.x += volume * left;
      sampleOut.y += volume * right;
    }

    frequencyLeft *= dt;
    frequencyRight *= dt;

    if(unison > 1) {
      for(int k=0; k<unison; k++) {
        phases[k].x += frequencyLeft * unisonTunings[k].x;
        phases[k].y += frequencyRight * unisonTunings[k].y;
      }
    }
    else {
      phases[0].x += frequencyLeft * tuning;
      phases[0].y += frequencyRight * tuning;
    }
    voiceOutputs[voiceIndex] = sampleOutFull;
  }


  inline double getSample(double dt, double frequency, double keyHoldTime, bool isKeyHolding) {
    return 0;
  }

  SynthTitleBar *titleBar = NULL;

  Panel *panel = NULL;
  //ListBox *voicePresetsGui = NULL;
  ListBox *waveSourceGui = NULL;
  NumberBox *volumeGui = NULL, *tuningGui = NULL, *unisonGui = NULL, *unisonDetuningGui = NULL;
  NumberBox *unisonDetuningRandomSeedLeftGui = NULL, *unisonDetuningRandomSeedRightGui = NULL;
  NumberBox *stereoPhaseOffsetGui = NULL, *stereoFrequencySeparationGui = NULL;
  NumberBox *panGui = NULL;
  ListBox *fmAlgorithmGui = NULL;

  ListBox *keyMappingModeGui = NULL;
  NumberBox *fixedFrequencyGui = NULL;

  CheckBox *showBiquadFilterGui = NULL;

  Panel *addPanel(GuiElement *parentElement, const std::string &title = "") {
    if(panel) return panel;

    panel = new Panel(250, 590, 10, 10);
    panel->zLayer = 1;
    
    panel->addGuiEventListener(new VoicePanelListener(this));
    parentElement->addChildElement(panel);

    titleBar = new SynthTitleBar(title, panel, isActive);
    //titleBar->addPresetsKnob(presets);

    ConstantWidthColumnPlacer layoutPlacer(250-20, 0, 10, titleBar->size.y+5);

    panel->addChildElement(waveSourceGui = new ListBox("Audio source", layoutPlacer, 7));
    waveSourceGui->setItems(audioSourceNames);
    waveSourceGui->setValue(audioSource);

    volumeGui = new NumberBox("Volume", volume, NumberBox::FLOATING_POINT, 0.0, 1e12, layoutPlacer);
    tuningGui = new NumberBox("Tuning", tuning, NumberBox::FLOATING_POINT, 0.0, 1e12, layoutPlacer);

    stereoPhaseOffsetGui = new NumberBox("Stereo phase offset", stereoPhaseOffset, NumberBox::FLOATING_POINT, 0, 1e12, layoutPlacer, 9);
    panel->addChildElement(panGui = new NumberBox("Pan", pan, NumberBox::FLOATING_POINT, -1, 1, layoutPlacer, 9));
    panGui->incrementMode = NumberBox::IncrementMode::Linear;
    panGui->linearIncrementAmount = 0.1;
    unisonGui = new NumberBox("Unison", unison, NumberBox::INTEGER, 1, maxUnison, layoutPlacer);

    unisonDetuningGui = new NumberBox("Unison detuning", unisonDetuning, NumberBox::FLOATING_POINT, 0.0, 1e12, layoutPlacer, 10);
    unisonDetuningRandomSeedLeftGui = new NumberBox("Detuning random left", unisonDetuningRandomSeedLeft, NumberBox::INTEGER, 0, 1<<30, layoutPlacer, 7);
    unisonDetuningRandomSeedRightGui = new NumberBox("Detuning random right", unisonDetuningRandomSeedRight, NumberBox::INTEGER, 0, 1<<30, layoutPlacer, 7);

    panel->addChildElement(keyMappingModeGui = new ListBox("Key mapping", layoutPlacer, 10));
    keyMappingModeGui->setItems(keyMappingModeNames);
    keyMappingModeGui->setValue(keyMappingMode);

    panel->addChildElement(fixedFrequencyGui = new NumberBox("Fixed frequency", fixedFrequency, NumberBox::FLOATING_POINT, 1.0, 1e12, layoutPlacer));

    showBiquadFilterGui = new CheckBox("Show filter", biquadFilter->isActive, layoutPlacer);
    

    panel->addChildElement(volumeGui);
    panel->addChildElement(tuningGui);
    panel->addChildElement(stereoPhaseOffsetGui);
    panel->addChildElement(unisonGui);
    panel->addChildElement(unisonDetuningGui);
    panel->addChildElement(unisonDetuningRandomSeedLeftGui);
    panel->addChildElement(unisonDetuningRandomSeedRightGui);
    panel->addChildElement(showBiquadFilterGui);

    panel->setSize(250, layoutPlacer.getY() + 10);

    waveForm.addPanel(panel);
    waveForm.getPanel()->setPosition(0, panel->size.y + 5);
    
    biquadFilter->addPanel(panel);
    biquadFilter->getPanel()->setVisible(biquadFilter->isActive);

    return panel;
    //setSubPanelPositions();
  }

  Panel *getPanel() {
    return panel;
  }

  void updatePanel() {
    if(panel) {
      //addPanel(NULL);
      volumeGui->setValue(volume);
      tuningGui->setValue(tuning);
      panGui->setValue(pan);
      stereoPhaseOffsetGui->setValue(stereoPhaseOffset);
      unisonGui->setValue(unison);
      unisonDetuningGui->setValue(unisonDetuning);
      unisonDetuningRandomSeedLeftGui->setValue(unisonDetuningRandomSeedLeft);
      unisonDetuningRandomSeedRightGui->setValue(unisonDetuningRandomSeedRight);

      waveSourceGui->setValue(audioSource);
      keyMappingModeGui->setValue(keyMappingMode);
      fixedFrequencyGui->setValue(fixedFrequency);

      showBiquadFilterGui->setValue(biquadFilter->isActive);
      waveForm.updatePanel();
    }
    if(biquadFilter->getPanel()) {
      biquadFilter->updatePanel();
    }
  }

  void removePanel(GuiElement *parentElement) {
    if(!panel) return;
    waveForm.removePanel(panel);
    biquadFilter->removePanel(panel);
    parentElement->deleteChildElement(panel);
    
    panel = NULL;
    //voicePresetsGui = NULL;
    waveSourceGui = NULL;
    volumeGui = NULL;
    tuningGui = NULL;
    unisonGui = NULL;
    unisonDetuningGui = NULL;
    unisonDetuningRandomSeedLeftGui = NULL;
    unisonDetuningRandomSeedRightGui = NULL;
    stereoPhaseOffsetGui = NULL;
    panGui = NULL;
    stereoFrequencySeparationGui = NULL;

    keyMappingModeGui = NULL;
    fixedFrequencyGui = NULL;

    showBiquadFilterGui = NULL;
    //fmAlgorithmGui = NULL;
  }

  struct VoicePanelListener : public GuiEventListener {
    Voice *voice;
    VoicePanelListener(Voice *voice) {
      this->voice = voice;
    }

    void onAction(GuiElement *guiElement) {
      /*if(guiElement == voice->voicePresetsGui) {
        //voice->currentInitializeID++;
        //voice->stopAllNotesRequested = true;
        int value;
        guiElement->getValue((void*)&value);
        voice->setVoicePreset(value);
        voice->voicePresetsGui->setValue(value);
      }*/
    }

    void onValueChange(GuiElement *guiElement) {

      if(guiElement == voice->titleBar->isActiveGui) {
        guiElement->getValue((void*)&voice->isActive);
      }
      if(guiElement == voice->waveSourceGui) {
        guiElement->getValue((void*)&voice->audioSource);
        // TODO show/hide waveform/vocoder? panel
      }
      if(guiElement == voice->keyMappingModeGui) {
        guiElement->getValue((void*)&voice->keyMappingMode);
      }
      if(guiElement == voice->fixedFrequencyGui) {
        guiElement->getValue((void*)&voice->fixedFrequency);
      }

      if(guiElement == voice->volumeGui) {
        guiElement->getValue((void*)&voice->volume);
      }
      if(guiElement == voice->tuningGui) {
        guiElement->getValue((void*)&voice->tuning);
        voice->prepare();
        voice->waveForm.oscillatorFrequencyFactor = voice->tuning;
        if(voice->waveForm.type == WaveForm::Type::SinBands) {
          voice->waveForm.prepareWaveTable();
        }
      }
      if(guiElement == voice->stereoPhaseOffsetGui) {
        guiElement->getValue((void*)&voice->stereoPhaseOffset);
        voice->stereoPhaseOffsetModular.value = voice->stereoPhaseOffset;
      }
      if(guiElement == voice->panGui) {
        guiElement->getValue((void*)&voice->pan);
        voice->panModular.value = voice->pan;
      }
      if(guiElement == voice->unisonGui) {
        //voice->currentInitializeID++;
        //voice->stopAllNotesRequested = true;
        guiElement->getValue((void*)&voice->unison);
        voice->prepare();
      }
      if(guiElement == voice->unisonDetuningGui) {
        guiElement->getValue((void*)&voice->unisonDetuning);
        voice->prepare();
      }
      if(guiElement == voice->unisonDetuningRandomSeedLeftGui) {
        guiElement->getValue((void*)&voice->unisonDetuningRandomSeedLeft);
        voice->prepare();
      }
      if(guiElement == voice->unisonDetuningRandomSeedRightGui) {
        guiElement->getValue((void*)&voice->unisonDetuningRandomSeedRight);
        voice->prepare();
      }
      if(guiElement == voice->showBiquadFilterGui) {
        bool value;
        guiElement->getValue((void*)&value);
        voice->biquadFilter->updatePanel();
        voice->biquadFilter->getPanel()->setVisible(value);
      }
      /*if(guiElement == voice->fmAlgorithmGui) {
        guiElement->getValue((void*)&voice->frequencyModulationAlgorithm);
      }*/

    }


  };


  static std::string getClassName() {
    return "voice";
  }
  
  virtual std::string getBlockName() {
    return getClassName();
  }

  virtual void decodeParameters() {
    printf("DEBUGGING Oscillator::decodeParameters()...\n");
    getNumericParameter("volume", volume);
    getNumericParameter("tuning", tuning);
    waveForm.oscillatorFrequencyFactor = tuning;
    getNumericParameter("stereoPhaseOffset", stereoPhaseOffset);
    getNumericParameter("pan", pan);
    getNumericParameter("unison", unison);
    getNumericParameter("unisonDetuning", unisonDetuning);
    getNumericParameter("unisonDetuningRandomSeeds", unisonDetuningRandomSeedLeft, 0);
    getNumericParameter("unisonDetuningRandomSeeds", unisonDetuningRandomSeedRight, 1);
    getNumericParameter("amplitudeModulator", inputAM);
    getNumericParameter("frequencyModulator", inputFM);
    getNumericParameter("amplitudeEnvelope", inputAmplitudeEnvelope);
    getNumericParameter("frequencyEnvelope", inputFrequencyEnvelope);

    int tmp = 0;
    getNumericParameter("keyMappingMode", tmp);
    keyMappingMode = (KeyMappingMode)tmp;
    getNumericParameter("fixedFrequency", fixedFrequency);

    
    getNumericParameter("panModular", panModular.modulator, 0);
    getNumericParameter("panModular", panModular.envelope, 1);
    getNumericParameter("panModular", panModular.range.x, 2);
    getNumericParameter("panModular", panModular.range.y, 3);
    //panModular.checkConnected();

    getNumericParameter("unisonDetuningModular", unisonDetuningModular.modulator, 0);
    getNumericParameter("unisonDetuningModular", unisonDetuningModular.envelope, 1);
    getNumericParameter("unisonDetuningModular", unisonDetuningModular.range.x, 2);
    getNumericParameter("unisonDetuningModular", unisonDetuningModular.range.y, 3);
    //unisonDetuningModular.checkConnected();
    
    getNumericParameter("stereoPhaseOffsetModular", stereoPhaseOffsetModular.modulator, 0);
    getNumericParameter("stereoPhaseOffsetModular", stereoPhaseOffsetModular.envelope, 1);
    getNumericParameter("stereoPhaseOffsetModular", stereoPhaseOffsetModular.range.x, 2);
    getNumericParameter("stereoPhaseOffsetModular", stereoPhaseOffsetModular.range.y, 3);
    //stereoPhaseOffsetModular.checkConnected();
    
    /*getNumericParameter("stereoPhaseDifferenceModular", stereoPhaseDifferenceModular.modulator, 0);
    getNumericParameter("stereoPhaseDifferenceModular", stereoPhaseDifferenceModular.envelope, 1);
    getNumericParameter("stereoPhaseDifferenceModular", stereoPhaseDifferenceModular.range.x, 2);
    getNumericParameter("stereoPhaseDifferenceModular", stereoPhaseDifferenceModular.range.y, 3);
    stereoPhaseDifferenceModular.checkConnected();*/
    
    getNumericParameter("filterFrequencyModular", filterFrequencyModular.modulator, 0);
    getNumericParameter("filterFrequencyModular", filterFrequencyModular.envelope, 1);
    getNumericParameter("filterFrequencyModular", filterFrequencyModular.range.x, 2);
    getNumericParameter("filterFrequencyModular", filterFrequencyModular.range.y, 3);
    //filterFrequencyModular.checkConnected();
    
    getNumericParameter("filterBandwidthModular", filterBandwidthModular.modulator, 0);
    getNumericParameter("filterBandwidthModular", filterBandwidthModular.envelope, 1);
    getNumericParameter("filterBandwidthModular", filterBandwidthModular.range.x, 2);
    getNumericParameter("filterBandwidthModular", filterBandwidthModular.range.y, 3);
    //filterBandwidthModular.checkConnected();
    
    getNumericParameter("filterWetLevelModular", filterWetLevelModular.modulator, 0);
    getNumericParameter("filterWetLevelModular", filterWetLevelModular.envelope, 1);
    getNumericParameter("filterWetLevelModular", filterWetLevelModular.range.x, 2);
    getNumericParameter("filterWetLevelModular", filterWetLevelModular.range.y, 3);
    //filterWetLevelModular.checkConnected();
    
    checkModularConnections();
    
    //prepare();
    update();
  }

  virtual void encodeParameters() {
    clearParameters();
    putNumericParameter("volume", volume);
    putNumericParameter("tuning", tuning);
    putNumericParameter("stereoPhaseOffset", stereoPhaseOffset);
    putNumericParameter("pan", pan);
    putNumericParameter("unison", unison);
    putNumericParameter("unisonDetuning", unisonDetuning);
    putNumericParameter("unisonDetuningRandomSeeds", { (double)unisonDetuningRandomSeedLeft, (double)unisonDetuningRandomSeedRight} );
    putNumericParameter("amplitudeModulator", (int)inputAM);
    putNumericParameter("frequencyModulator", (int)inputFM);
    putNumericParameter("amplitudeEnvelope", (int)inputAmplitudeEnvelope);
    putNumericParameter("frequencyEnvelope", (int)inputFrequencyEnvelope);
    
    if(keyMappingMode != KeyMappingMode::Normal) {
      putNumericParameter("keyMappingMode", keyMappingMode);
    }
    if(keyMappingMode == KeyMappingMode::FixedFrequency) {
      putNumericParameter("fixedFrequency", fixedFrequency);
    }

    if(panModular.isConnected) {
      putNumericParameter("panModular", {(double)panModular.modulator, (double)panModular.envelope, panModular.range.x, panModular.range.y});
    }
    if(unisonDetuningModular.isConnected) {
      putNumericParameter("unisonDetuningModular", {(double)unisonDetuningModular.modulator, (double)unisonDetuningModular.envelope, unisonDetuningModular.range.x, unisonDetuningModular.range.y});
    }
    if(stereoPhaseOffsetModular.isConnected) {
      putNumericParameter("stereoPhaseOffsetModular", {(double)stereoPhaseOffsetModular.modulator, (double)stereoPhaseOffsetModular.envelope, stereoPhaseOffsetModular.range.x, stereoPhaseOffsetModular.range.y});
    }
    /*if(stereoPhaseDifferenceModular.isConnected) {
      putNumericParameter("stereoPhaseDifferenceModular", {stereoPhaseDifferenceModular.modulator, stereoPhaseDifferenceModular.envelope, stereoPhaseDifferenceModular.range.x, stereoPhaseDifferenceModular.range.y});
    }*/

    if(filterFrequencyModular.isConnected) {
      putNumericParameter("filterFrequencyModular", {(double)filterFrequencyModular.modulator, (double)filterFrequencyModular.envelope, filterFrequencyModular.range.x, filterFrequencyModular.range.y});
    }
    if(filterBandwidthModular.isConnected) {
      putNumericParameter("filterBandwidthModular", {(double)filterBandwidthModular.modulator, (double)filterBandwidthModular.envelope, filterBandwidthModular.range.x, filterBandwidthModular.range.y});
    }
    if(filterWetLevelModular.isConnected) {
      putNumericParameter("filterWetLevelModular", {(double)filterWetLevelModular.modulator, (double)filterWetLevelModular.envelope, filterWetLevelModular.range.x, filterWetLevelModular.range.y});
    }

    
    waveForm.encodeParameters();
    
    
    if(biquadFilter->isActive) {
      biquadFilter->encodeParameters();
    }
  }

  virtual void onPrintParameters(std::string &content, int level = 0) {
    waveForm.printParameters(content, level);
    if(biquadFilter->isActive) {
      biquadFilter->printParameters(content, level);
    }

  }

  virtual int onNewBlock(const std::string &blockName, int blockStartInd, int blockLevel) {
    if(blockName == WaveForm::getClassName()) {
      return waveForm.parse(content, blockStartInd, blockLevel);
    }
    if(blockName == BiquadFilter::getClassName()) {
      return biquadFilter->parse(content, blockStartInd, blockLevel);
    }

    return 0;
  }


};




// TODO modulator class based on voice class
struct GenericModulator : public PanelInterface, public HierarchicalTextFileParser {
  /*struct ModularParameterListener : public GuiEventListener {
    GenericModulator *modulator;
    ModularParameterListener(GenericModulator *modulator) {
      this->modulator = modulator;
    }
    virtual void onMessage(GuiElement *guiElement, const std::string &message) {
      modulator->areModularConnections = false;
      for(int i=0; i<modulator->modularParameters.size(); i++) {
        modulator->areModularConnections = modulator->areModularConnections || modulator->modularParameters[i]->isConnected;
      }
    }
  };*/
  
  struct ModularParameterListener : public GuiEventListener {
    GenericModulator *modulator;
    ModularParameterListener(GenericModulator *modulator) {
      this->modulator = modulator;
    }
    virtual void onMessage(GuiElement *guiElement, const std::string &message) {
      modulator->checkModularConnections();
    }
  };
  
  WaveForm waveForm;
  bool isActive = true;
  double frequency = 5;
  double stereoFrequencySeparation = 0.0;
  double stereoPhaseOffset = 0.0;
  double pan = 0;
  double amplitudeModulationDepthMin = 1, amplitudeModulationDepthMax = 1;
  
  double frequencyModulationDepthMin = 0.001, frequencyModulationDepthMax = 0.001;
  bool isFrequencyRelativeToCarrierFrequency = false;

  //ModularParameter frequencyModular;
  ModularParameter stereoFrequencySeparationModular;
  ModularParameter stereoPhaseOffsetModular;
  ModularParameter panModular;
  
  std::vector<ModularParameter*> modularParameters;
  
  bool areModularConnections = false;

  int amplitudeEnvelope = -1, inputAM = -1;
  int frequencyEnvelope = -1, inputFM = -1;
  int outputAM = -1;
  int outputFM = -1;
  

  void checkModularConnections() {
    areModularConnections = false;
    for(int i=0; i<modularParameters.size(); i++) {
      if(modularParameters[i]->checkConnected()) {
        areModularConnections = true;
      }
    }
  }

  GenericModulator() {
    //modularParameters.push_back(&frequencyModular.init("Frequency", &frequency, 0, 1e10, 1, 2, new ModularParameterListener(this)));
    modularParameters.push_back(&stereoPhaseOffsetModular.init("Stereo phase offset", &stereoPhaseOffset, -1e10, 1e10, 0, 1, new ModularParameterListener(this)));
    //modularParameters.push_back(&stereoFrequencySeparationModular.init("Stereo seperation", &stereoFrequencySeparation, -1e10, 1e10, 0, 0.1, new ModularParameterListener(this)));
    modularParameters.push_back(&panModular.init("Pan", &pan, -1, 1, -1, 1, new ModularParameterListener(this)));
  }

  void update() {
    waveForm.update();
  }

  inline void operator=(const GenericModulator &gm) {
    this->amplitudeModulationDepthMin = gm.amplitudeModulationDepthMin;
    this->amplitudeModulationDepthMax = gm.amplitudeModulationDepthMax;
    this->frequencyModulationDepthMin = gm.frequencyModulationDepthMin;
    this->frequencyModulationDepthMax = gm.frequencyModulationDepthMax;
    this->frequency = gm.frequency;
    this->waveForm = gm.waveForm;
    this->isFrequencyRelativeToCarrierFrequency = gm.isFrequencyRelativeToCarrierFrequency;
    this->stereoFrequencySeparation = gm.stereoFrequencySeparation;
    this->stereoPhaseOffset = gm.stereoPhaseOffset;
    this->pan = gm.pan;
    this->amplitudeEnvelope = gm.amplitudeEnvelope;
    this->inputAM = gm.inputAM;
    this->frequencyEnvelope = gm.frequencyEnvelope;
    this->inputFM = gm.inputFM;
    this->outputAM = gm.outputAM;
    this->outputFM = gm.outputFM;
  }


  int getTotalOscillatorCount() {
    if(waveForm.partialSet.numPartials > 1 && (waveForm.waveTableMode == WaveForm::WaveTableMode::None || waveForm.waveTableMode == WaveForm::WaveTableMode::ForEachPartial || waveForm.waveTableMode == WaveForm::WaveTableMode::ForEachPartialFaster)) {
      return waveForm.partialSet.numPartials;
    }
    return 1;
  }


  inline void apply(double dt, double carrierFrequency, Vec2d &phase, unsigned char pitch, const std::vector<Vec2d> &amplitudeModulators, const std::vector<Vec2d> &frequencyModulators, std::vector<Vec2d> &amplitudeModulatorOutputs, std::vector<Vec2d> &frequencyModulatorOutputs, const std::vector<double> &envelopes) {
    if(!isActive) return;
    
    double left = waveForm.getSample(phase.x, pitch);
    double right = waveForm.getSample(phase.y + stereoPhaseOffset, pitch);

    if(pan < 0) {
      right *= (1.0+pan);
    }
    else if(pan > 0) {
      left *= (1.0-pan);
    }

    if(outputAM >= 0) {
      double dx = amplitudeModulationDepthMin;
      double dy = amplitudeModulationDepthMin;

      if(amplitudeEnvelope >= 0) {
        dx *= envelopes[amplitudeEnvelope];
        dy *= envelopes[amplitudeEnvelope];
      }
      if(inputAM >= 0) {
        dx *= amplitudeModulators[inputAM].x;
        dy *= amplitudeModulators[inputAM].y;
      }
      amplitudeModulatorOutputs[outputAM].x *= dx * (left*0.5 + 0.5) + 1.0 - dx;
      amplitudeModulatorOutputs[outputAM].y *= dy * (right*0.5 + 0.5) + 1.0 - dy;
    }

    if(outputFM >= 0) {
      double dx = frequencyModulationDepthMin;
      double dy = frequencyModulationDepthMin;

      if(amplitudeEnvelope >= 0) {
        dx *= envelopes[amplitudeEnvelope];
        dy *= envelopes[amplitudeEnvelope];
      }
      if(inputAM >= 0) {
        dx *= amplitudeModulators[inputAM].x;
        dy *= amplitudeModulators[inputAM].y;
      }
      frequencyModulatorOutputs[outputFM].x += dx * left;
      frequencyModulatorOutputs[outputFM].y += dy * right;
    }

    double f = frequency, fmEnv = 1.0;

    if(isFrequencyRelativeToCarrierFrequency) {
      f *= carrierFrequency;
    }

    if(frequencyEnvelope >= 0) {
      fmEnv = envelopes[frequencyEnvelope];
    }

    double s = stereoFrequencySeparation * frequency * 0.5;

    if(inputFM >= 0) {
      phase.x += (f + f * frequencyModulators[inputFM].x - s) * fmEnv * dt;
      phase.y += (f + f * frequencyModulators[inputFM].y + s) * fmEnv * dt;
    }
    else {
      phase.x += (f - s) * fmEnv * dt;
      phase.y += (f + s) * fmEnv * dt;
    }
  }




  Panel *panel = NULL;
  
  SynthTitleBar *titleBar = NULL;
  NumberBox *frequencyGui = NULL, *stereoFrequencySeparationGui = NULL, *stereoPhaseOffsetGui = NULL;
  NumberBox *panGui = NULL;
  NumberBox *amplitudeModulationDepthMinGui = NULL, *amplitudeModulationDepthMaxGui = NULL;
  NumberBox *frequencyModulationDepthMinGui = NULL, *frequencyModulationDepthMaxGui = NULL;
  CheckBox *isFrequencyRelativeToCarrierFrequencyGui = NULL;

  Panel *getPanel() {
    return panel;
  }

  Panel *addPanel(GuiElement *parentElement, const std::string &title = "") {
    if(panel) return panel;

    panel = new Panel(title, 250, 590, 10, 10);
    panel->zLayer = 1;
    
    panel->addGuiEventListener(new GenericModulatorPanelListener(this));
    parentElement->addChildElement(panel);

    double line = 0, lineHeight = 23;

    titleBar = new SynthTitleBar(title, panel, isActive);
    //titleBar->addPresetsKnob(presets);
    //line += titleBar->size.y + 5;
    
    ConstantWidthColumnPlacer layoutPlacer(250-20, 0, 10, titleBar->size.y+5);

    frequencyGui = new NumberBox("Frequency", frequency, NumberBox::FLOATING_POINT, 0.0, 1e12, layoutPlacer, 10);
    panel->addChildElement(frequencyGui);

    stereoFrequencySeparationGui = new NumberBox("Stereo separation", stereoFrequencySeparation, NumberBox::FLOATING_POINT, 0.0, 1e12, layoutPlacer, 10);
    panel->addChildElement(stereoFrequencySeparationGui);

    stereoPhaseOffsetGui = new NumberBox("Stereo phase offset", stereoPhaseOffset, NumberBox::FLOATING_POINT, 0.0, 1e12, layoutPlacer, 10);
    panel->addChildElement(stereoPhaseOffsetGui);
    
    panel->addChildElement(panGui = new NumberBox("Pan", pan, NumberBox::FLOATING_POINT, -1, 1, layoutPlacer, 9));
    panGui->incrementMode = NumberBox::IncrementMode::Linear;
    panGui->linearIncrementAmount = 0.1;

    amplitudeModulationDepthMinGui = new NumberBox("AM depth", amplitudeModulationDepthMin, NumberBox::FLOATING_POINT, 0.0, 1e12, layoutPlacer, 10);
    panel->addChildElement(amplitudeModulationDepthMinGui);

    frequencyModulationDepthMinGui = new NumberBox("FM depth", frequencyModulationDepthMin, NumberBox::FLOATING_POINT, 0.0, 1e12, layoutPlacer, 10);
    panel->addChildElement(frequencyModulationDepthMinGui);

    isFrequencyRelativeToCarrierFrequencyGui = new CheckBox("Relative mode", isFrequencyRelativeToCarrierFrequency, layoutPlacer);
    panel->addChildElement(isFrequencyRelativeToCarrierFrequencyGui);

    panel->setSize(250, layoutPlacer.getY() + 10);

    waveForm.addPanel(panel);
    waveForm.getPanel()->setPosition(0, panel->size.y + 5);

    return panel;
  }

  void updatePanel() {
    if(panel) {
      frequencyGui->setValue(frequency);
      stereoFrequencySeparationGui->setValue(stereoFrequencySeparation);
      panGui->setValue(pan);
      stereoPhaseOffsetGui->setValue(stereoPhaseOffset);
      amplitudeModulationDepthMinGui->setValue(amplitudeModulationDepthMin);
      frequencyModulationDepthMinGui->setValue(frequencyModulationDepthMin);
      isFrequencyRelativeToCarrierFrequencyGui->setValue(isFrequencyRelativeToCarrierFrequency);
      waveForm.updatePanel();
    }
  }

  void removePanel(GuiElement *parentElement) {
    if(!panel) return;
    waveForm.removePanel(panel);
    parentElement->deleteChildElement(panel);
    panel = NULL;
    frequencyGui = NULL;
    stereoFrequencySeparationGui = NULL;
    stereoPhaseOffsetGui = NULL;
    panGui = NULL;
    amplitudeModulationDepthMinGui = NULL;
    amplitudeModulationDepthMaxGui = NULL;
    frequencyModulationDepthMinGui = NULL;
    frequencyModulationDepthMaxGui = NULL;
    isFrequencyRelativeToCarrierFrequencyGui = NULL;
  }

  struct GenericModulatorPanelListener : public GuiEventListener {
    GenericModulator *genericModulator;
    GenericModulatorPanelListener(GenericModulator *genericModulator) {
      this->genericModulator = genericModulator;
    }
    void onValueChange(GuiElement *guiElement) {
      if(guiElement == genericModulator->titleBar->isActiveGui) {
        guiElement->getValue((void*)&genericModulator->isActive);
      }
      if(guiElement == genericModulator->frequencyGui) {
        guiElement->getValue((void*)&genericModulator->frequency);
      }
      if(guiElement == genericModulator->stereoFrequencySeparationGui) {
        guiElement->getValue((void*)&genericModulator->stereoFrequencySeparation);
      }
      if(guiElement == genericModulator->stereoPhaseOffsetGui) {
        guiElement->getValue((void*)&genericModulator->stereoPhaseOffset);
      }
      if(guiElement == genericModulator->panGui) {
        guiElement->getValue((void*)&genericModulator->pan);
      }
      if(guiElement == genericModulator->amplitudeModulationDepthMinGui) {
        guiElement->getValue((void*)&genericModulator->amplitudeModulationDepthMin);
      }
      if(guiElement == genericModulator->frequencyModulationDepthMinGui) {
        guiElement->getValue((void*)&genericModulator->frequencyModulationDepthMin);
      }
      if(guiElement == genericModulator->isFrequencyRelativeToCarrierFrequencyGui) {
        guiElement->getValue((void*)&genericModulator->isFrequencyRelativeToCarrierFrequency);
      }
    }
  };


  static std::string getClassName() {
    return "modulator";
  }

  virtual std::string getBlockName() {
    return getClassName();
  }


  virtual void decodeParameters() {
    getNumericParameter("frequency", frequency);
    getNumericParameter("stereoPhaseOffset", stereoPhaseOffset);
    getNumericParameter("pan", pan);
    getNumericParameter("stereoPhaseDifference", stereoFrequencySeparation);
    getNumericParameter("relativeMode", isFrequencyRelativeToCarrierFrequency);
    getNumericParameter("inputAM", inputAM);
    getNumericParameter("inputFM", inputFM);
    getNumericParameter("outputAM", outputAM);
    getNumericParameter("outputFM", outputFM);
    getNumericParameter("amplitudeEnvelope", amplitudeEnvelope);
    getNumericParameter("frequencyEnvelope", frequencyEnvelope);
    getNumericParameter("depthAM", amplitudeModulationDepthMin);
    getNumericParameter("depthFM", frequencyModulationDepthMin);
    
    getNumericParameter("stereoPhaseOffsetModular", stereoPhaseOffsetModular.modulator, 0);
    getNumericParameter("stereoPhaseOffsetModular", stereoPhaseOffsetModular.envelope, 1);
    getNumericParameter("stereoPhaseOffsetModular", stereoPhaseOffsetModular.range.x, 2);
    getNumericParameter("stereoPhaseOffsetModular", stereoPhaseOffsetModular.range.y, 3);
    //stereoPhaseOffsetModular.checkConnected();
      
    getNumericParameter("panModular", panModular.modulator, 0);
    getNumericParameter("panModular", panModular.envelope, 1);
    getNumericParameter("panModular", panModular.range.x, 2);
    getNumericParameter("panModular", panModular.range.y, 3);
    //stereoPhaseOffsetModular.checkConnected();
    
    checkModularConnections();
    
    /*getNumericParameter("stereoPhaseDifferenceModular", stereoPhaseDifferenceModular.modulator, 0);
    getNumericParameter("stereoPhaseDifferenceModular", stereoPhaseDifferenceModular.envelope, 1);
    getNumericParameter("stereoPhaseDifferenceModular", stereoPhaseDifferenceModular.range.x, 2);
    getNumericParameter("stereoPhaseDifferenceModular", stereoPhaseDifferenceModular.range.y, 3);
    stereoPhaseOffsetModular.checkConnected();*/
    
    update();

  }

  virtual void encodeParameters() {
    clearParameters();
    putNumericParameter("frequency", frequency);
    putNumericParameter("stereoPhaseOffset", stereoPhaseOffset);
    putNumericParameter("pan", pan);
    putNumericParameter("stereoPhaseDifference", stereoFrequencySeparation);
    putNumericParameter("relativeMode", isFrequencyRelativeToCarrierFrequency);
    putNumericParameter("inputAM", inputAM);
    putNumericParameter("inputFM", inputFM);
    putNumericParameter("outputAM", outputAM);
    putNumericParameter("outputFM", outputFM);
    putNumericParameter("amplitudeEnvelope", amplitudeEnvelope);
    putNumericParameter("frequencyEnvelope", frequencyEnvelope);
    putNumericParameter("depthAM", amplitudeModulationDepthMin);
    putNumericParameter("depthFM", frequencyModulationDepthMin);
    
    if(stereoPhaseOffsetModular.isConnected) {
      putNumericParameter("stereoPhaseOffsetModular", {(double)stereoPhaseOffsetModular.modulator, (double)stereoPhaseOffsetModular.envelope, stereoPhaseOffsetModular.range.x, stereoPhaseOffsetModular.range.y});
    }
    if(panModular.isConnected) {
      putNumericParameter("panModular", {(double)panModular.modulator, (double)panModular.envelope, panModular.range.x, panModular.range.y});
    }
    /*if(stereoPhaseDifferenceModular.isConnected) {
      putNumericParameter("stereoPhaseDifferenceModular", {(double)stereoPhaseDifferenceModular.modulator, (double)stereoPhaseDifferenceModular.envelope, stereoPhaseDifferenceModular.range.x, stereoPhaseDifferenceModular.range.y});
    }*/
        
    waveForm.encodeParameters();


  }
  
  virtual void onPrintParameters(std::string &content, int level = 0) {
    waveForm.printParameters(content, level);
  }

  virtual int onNewBlock(const std::string &blockName, int blockStartInd, int blockLevel) {
    if(blockName == WaveForm::getClassName()) {
      return waveForm.parse(content, blockStartInd, blockLevel);
    }

    return 0;
  }


};






struct GenericEnvelope : public Envelope {
  int outputIndex = 0;

  inline void operator=(const GenericEnvelope &a) {
    Envelope::operator=(a);
    this->outputIndex = a.outputIndex;
  }

  inline void apply(double t, double keyHoldTime, bool isKeyHolding, std::vector<double> &genericEnvelopeOutputs) {
    if(outputIndex >= 0 && outputIndex < genericEnvelopeOutputs.size()) {
      genericEnvelopeOutputs[outputIndex] *= getSample(t, keyHoldTime, isKeyHolding);
    }
  }
  
  virtual void decodeParameters() {
    Envelope::decodeParameters();
    getNumericParameter("output", outputIndex);
  }
  
  virtual void encodeParameters() {
    Envelope::encodeParameters();
    putNumericParameter("output", outputIndex);
  }

};
