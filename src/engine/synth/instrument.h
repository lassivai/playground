#pragma once

#include "oscillator.h"
#include "note.h"
#include "modularparameter.h"

#include "delayline.h"
#include "filter/postprocessingeffect.h"
#include "filter/multidelay.h"
#include "filter/biquad.h"
#include "filter/eq.h"
#include "filter/reverb.h"

#include "spectrummapfixed.h"

#include "gui/effecttrackpanel.h"
/* TODO 
 * Option to choose longer delay line
 *
 * crashed when activated eq from launchpad
 * note/voice filters...
 */





struct Instrument : public HierarchicalTextFileParser {
  struct ModularParameterPanel : public Panel {
    //std::vector<ModularParameter*> modularParameters;
    std::vector<Label*> labels;
    
    Instrument *instrument;
    
    /*void deleteChildElements() {
      GuiElement::deleteChildElements();
      
    }*/
    virtual ~ModularParameterPanel() {
      for(int k=0; k<instrument->modularParameters.size(); k++) {
        instrument->modularParameters[k]->modularTrackPanel = NULL;
      }
    }
    ModularParameterPanel() : Panel("Modular parameters panel") {}
    
    ModularParameterPanel(Instrument *instrument) : Panel("Modular parameters panel") {
      init(instrument);
      update();
    }
    
    void init(Instrument *instrument) {
      this->instrument = instrument;
      //addChildElement(new Label("Modular parameters", 10, 10));
    }
    void update() {

      clearChildElements();
      for(int i=0; i<labels.size(); i++) {
        delete labels[i];
      }

      labels.clear();

      double line = -13, lineHeight = 23;
      for(int k=0; k<instrument->modularParameters.size(); k++) {
        Label *label = new Label("Note: " + instrument->modularParameters[k]->name, 10, line+=lineHeight);
        labels.push_back(label);
        addChildElement(label);

        Panel *panel = instrument->modularParameters[k]->getModularTrackPanel();
        panel->setPosition(10+220, line);
        addChildElement(panel);
      }

      for(int i=0; i<instrument->numVoices; i++) {
        for(int k=0; k<instrument->voices[i].modularParameters.size(); k++) {
          Label *label = new Label("Voice "+std::to_string(i+1) +": " + instrument->voices[i].modularParameters[k]->name, 10, line+=lineHeight);
          labels.push_back(label);
          addChildElement(label);

          Panel *panel = instrument->voices[i].modularParameters[k]->getModularTrackPanel();
          panel->setPosition(10+220, line);
          addChildElement(panel);
        }
      }
      
      for(int i=0; i<instrument->numModulators; i++) {
        for(int k=0; k<instrument->modulators[i].modularParameters.size(); k++) {
          Label *label = new Label("Mod "+std::to_string(i+1) +": " + instrument->modulators[i].modularParameters[k]->name, 10, line+=lineHeight);
          labels.push_back(label);
          addChildElement(label);

          Panel *panel = instrument->modulators[i].modularParameters[k]->getModularTrackPanel();
          panel->setPosition(10+220, line);
          addChildElement(panel);
        }
      }
      
      setSize(715, line+lineHeight+10);
      
    }

    struct PanelUpdateListener : public GuiEventListener {
      ModularParameterPanel *modularParameterPanel;
      PanelUpdateListener(ModularParameterPanel *modularParameterPanel) {
        this->modularParameterPanel = modularParameterPanel;
      }
      virtual void onMessage(GuiElement *guiElement, const std::string &message) {
        if(message == "modparchange") {
          modularParameterPanel->update();
        }
      }
    };
  };
  
  
  struct NoteBiquadFilter : public BiquadFilter {
    virtual ~NoteBiquadFilter() {}
    NoteBiquadFilter(DelayLine *delayLine) : BiquadFilter(delayLine) {}
    static std::string getClassName() {
      return "noteFilter";
    }
    virtual std::string getBlockName() {
      return getClassName();
    }
  };  
  
  struct ModularParameterListener : public GuiEventListener {
    Instrument *instrument;
    ModularParameterListener(Instrument *instrument) {
      this->instrument = instrument;
    }
    virtual void onMessage(GuiElement *guiElement, const std::string &message) {
      instrument->checkModularConnections();
    }
  };

  void checkModularConnections() {
    areModularConnections = false;
    for(int i=0; i<modularParameters.size(); i++) {
      if(modularParameters[i]->checkConnected()) {
        areModularConnections = true;
      }
    }
  }

  VoiceCrossModulation voiceCrossModulation;

  enum InstrumentType { DefaultInstrument, CompositePads };
  InstrumentType instrumentType = DefaultInstrument;

  std::string name;

  static int instrumentCounter;

  int totalOscillatorCount = 0;

  bool stopAllNotesRequested = false;
  long currentInitializeID = 0;

  double volume = 1.0;

  std::vector<PostProcessingEffect*> postProcessingEffects;
  DelayLine delayLine;

  //int activePostProcessingEffect = 0;

  Vec2d sampleOut;

  bool isMuted = false;
  bool isActive = false;

  bool inactivationRequested = false;

  int numVoices = 0, numModulators = 0, numEnvelopes = 0;
  std::vector<Voice> voices;
  std::vector<GenericModulator> modulators;
  std::vector<GenericEnvelope> envelopes;

  bool isSustainActive = false;

  std::unordered_map<int, SynthesisNote*> sustainedNotes;

  std::vector<ModularParameter*> modularParameters;
  bool areModularConnections = false;
  ModularParameter filterFrequencyModular;
  ModularParameter filterBandwidthModular;
  ModularParameter filterWetLevelModular;
  double filterFrequency = 2000;
  double filterBandwidth = 2;
  double filterWetLevel = 1;
  
  NoteBiquadFilter *biquadFilter = NULL;

  bool isSynthesisActive = false;


  std::map<std::pair<double, double>, RecordedNote> recordedNotes;

  void clearRecordedNotes() {
    recordedNotes.clear();
  }

  struct RecordedNotesPanel : public Panel {
    Instrument *instrument = NULL;
    RecordedNotesPanel(Instrument *instrument) : Panel("Recorded notes panel") {
      this->instrument = instrument;
    }
    virtual void onRender(GeomRenderer &geomRenderer, TextGl &textRenderer) override {
      std::string str = "recorded notes";
      int i = 0;
      for(auto &pair : instrument->recordedNotes) {
        str += "\n";
        i++;
        RecordedNote &note = pair.second;
        double maxVal = 0;
        for(int i=0; i<note.samples.size(); i++) {
          maxVal = max(maxVal, note.samples[i].x);
        }
        str += format("notes %d, p %f, fl %f, tt %f, m %f, s %lu", note.numSequencerNotes, note.pitch, note.fullLengthInSecs, note.playTime, maxVal, note.samples.size());
      }
      Vec2d dim = textRenderer.getDimensions(str, 10);
      Vec2d size = dim + Vec2d(10, 10);
      setSize(size);
      textRenderer.setColor(1, 1, 1, 1);
      textRenderer.print(str, absolutePos.x+5, absolutePos.y+5, 10);
    }
  };


  void reinitialize() {
    voiceCrossModulation.reset();

    instrumentType = DefaultInstrument;

    name = "<new instrument>";

    totalOscillatorCount = 0;

    stopAllNotesRequested = true;
    currentInitializeID++;

    volume = 1.0;

    for(int i=0; i<postProcessingEffects.size(); i++) {
      delete postProcessingEffects[i];
    }
    postProcessingEffects.clear();
    
    delayLine.reset();

    //activePostProcessingEffect = 0;

    sampleOut.set(0, 0);

    //isMuted = false;
    //isActive = false;

    //inactivationRequested = false;

    numVoices = 0;
    numModulators = 0;
    numEnvelopes = 0;
    /*voices.clear();
    modulators.clear();
    envelopes.clear();
    voices.resize(maxNumVoices);
    modulators.resize(maxNumModulators);
    envelopes.resize(maxNumEnvelopes);*/
    
    voices.assign(maxNumVoices, Voice());
    modulators.assign(maxNumModulators, GenericModulator());
    envelopes.assign(maxNumEnvelopes, GenericEnvelope());
    
    for(int i=0; i<voices.size(); i++) {
      voices[i].init(&delayLine);
    }

    //isSustainActive = false;

    //std::unordered_map<int, Note*> sustainedNotes; // TODO

    //std::vector<ModularParameter*> modularParameters; 
    modularParameters.clear();
    areModularConnections = false;
    //ModularParameter filterFrequencyModular;  // TODO
    //ModularParameter filterBandwidthModular;  // TODO
    //ModularParameter filterWetLevelModular;  // TODO
    filterFrequency = 2000;
    filterBandwidth = 2;
    filterWetLevel = 1;
    
    
    
    
    
  
    //delayLine.init(sampleRate, 1);
    initBiquad(&delayLine);
    
  }


  

  virtual std::string getFileExtension() {
    return "syn";
  }
  virtual std::string getDefaultFileDirectory() {
    return "data/synth/instruments/";
  }
  
  void initBiquad(DelayLine *delayLine) {
    if(biquadFilter) delete biquadFilter;
    biquadFilter = new NoteBiquadFilter(delayLine);
    biquadFilter->isActive = false;
    biquadFilter->showTrackingPanel = true;
    
    modularParameters.push_back(&filterFrequencyModular.init("Filter frequency", &filterFrequency, 0, biquadFilter->sampleRate/2.0, 100, 1000, new ModularParameterListener(this)));
    modularParameters.push_back(&filterBandwidthModular.init("Filter bandwidth", &filterBandwidth, 0, 1e10, 0.5, 5, new ModularParameterListener(this)));
    modularParameters.push_back(&filterWetLevelModular.init("Filter wet", &filterWetLevel, 0, 1, 0, 1, new ModularParameterListener(this)));
    /*filterFrequency = biquadFilter->frequency;
    filterBandwidth = biquadFilter->bandwidth;
    filterWetLevel = biquadFilter->wetLevel;*/
    //initModularParameters();
  }

  virtual ~Instrument() {
    if(instrumentPanel) {
      GuiElement *parent = instrumentPanel->parentGuiElement;
      parent->deleteChildElement(instrumentPanel);
    }
  }

  Instrument() {
  }
  
  Instrument(double sampleRate, int framesPerBuffer) {
    instrumentCounter++;
    name = "<new instrument " + std::to_string(instrumentCounter) + ">";

    voices.resize(maxNumVoices);
    modulators.resize(maxNumModulators);
    envelopes.resize(maxNumEnvelopes);
    
    numVoices = 1;
    numEnvelopes = 1;
    

    initialize(sampleRate);
    
    for(int i=0; i<voices.size(); i++) {
      voices[i].init(&delayLine);
      voices[i].waveForm.initSampleEditor(sampleRate, framesPerBuffer);
    }
    for(int i=0; i<modulators.size(); i++) {
      modulators[i].waveForm.initSampleEditor(sampleRate, framesPerBuffer);
    }
    for(int i=0; i<envelopes.size(); i++) {
      envelopes[i].outputIndex = i;
    }
    
    voiceCrossModulation.setNumVoices(numVoices);
    
    initPreviewNote();
    
    checkModularConnections();
  }

  Instrument(const Instrument &instrument) {
    initialize(instrument.delayLine.sampleRate);

    instrumentCounter++;
    name = instrument.name;

    voices.resize(maxNumVoices);
    modulators.resize(maxNumModulators);
    envelopes.resize(maxNumEnvelopes);

    totalOscillatorCount = instrument.totalOscillatorCount;

    volume = instrument.volume;

    //activePostProcessingEffect = instrument.activePostProcessingEffect;

    isMuted = instrument.isMuted;
    isActive = instrument.isActive;

    numVoices = instrument.numVoices;
    numModulators = instrument.numModulators;
    numEnvelopes = instrument.numEnvelopes;

    voiceCrossModulation = instrument.voiceCrossModulation;

    this->biquadFilter->set(*instrument.biquadFilter);

    for(int i=0; i<maxNumVoices; i++) {
      voices[i] = instrument.voices[i];
    }
    for(int i=0; i<maxNumModulators; i++) {
      modulators[i] = instrument.modulators[i];
    }
    for(int i=0; i<maxNumEnvelopes; i++) {
      envelopes[i] = instrument.envelopes[i];
    }

    for(int i=0; i<instrument.postProcessingEffects.size(); i++) {
      PostProcessingEffect *postProcessingEffect = NULL;
      if(dynamic_cast<MultiDelay*>(instrument.postProcessingEffects[i])) {
        postProcessingEffect = new MultiDelay(&delayLine, *dynamic_cast<MultiDelay*>(instrument.postProcessingEffects[i]));
      }
      if(dynamic_cast<BiquadFilter*>(instrument.postProcessingEffects[i])) {
        postProcessingEffect = new BiquadFilter(&delayLine, *dynamic_cast<BiquadFilter*>(instrument.postProcessingEffects[i]));
      }
      if(dynamic_cast<Equalizer*>(instrument.postProcessingEffects[i])) {
        postProcessingEffect = new Equalizer(delayLine.sampleRate, *dynamic_cast<Equalizer*>(instrument.postProcessingEffects[i]));
      }
      if(dynamic_cast<Reverb*>(instrument.postProcessingEffects[i])) {
        postProcessingEffect = new Reverb(delayLine.sampleRate, *dynamic_cast<Reverb*>(instrument.postProcessingEffects[i]));
      }
      postProcessingEffects.push_back(postProcessingEffect);
    }
    
    initPreviewNote();
  }

private:
  SynthesisNote previewNote;

  void initPreviewNote(double noteFrequency = 261.63) {
    previewNote.set(delayLine.sampleRate, freqToNote(noteFrequency), 0, 1.0, -1/*instrumentIndex*/);
    previewNote.isHolding = true;
    previewNote.prepare(delayLine.sampleRate);
    initializeNote(previewNote);
  }
public:
  
  void preparePreviewNote(SynthesisNote &note) {
    note = previewNote;
    // fix this too:
    note.voiceOutputs = previewNote.voiceOutputs;
    note.amplitudeModulatorOutputs = previewNote.amplitudeModulatorOutputs;
    note.frequencyModulatorOutputs = previewNote.frequencyModulatorOutputs;
    note.amplitudeModulatorOutputsPrevious = previewNote.amplitudeModulatorOutputsPrevious;
    note.frequencyModulatorOutputsPrevious = previewNote.frequencyModulatorOutputsPrevious;
    note.envelopeOutputs = previewNote.envelopeOutputs;
    note.biquadFilter = previewNote.biquadFilter;
    note.phasesGM = previewNote.phasesGM;
    note.phasesVoiceNew = previewNote.phasesVoiceNew;
    note.voiceBiquadFilters = previewNote.voiceBiquadFilters;
    note.noteFullLengthSecs = previewNote.noteFullLengthSecs;
    note.isInitialized = true;
  }

  // testing...
  void threadSafePreparedPreviewNoteReset(SynthesisNote &note) {
    for(int i=0; i<previewNote.phasesGM.size(); i++) {
      note.phasesGM[i] = previewNote.phasesGM[i];
    }
    for(int i=0; i<previewNote.phasesVoiceNew.size(); i++) {
      note.phasesVoiceNew[i] = previewNote.phasesVoiceNew[i];
    }
    for(int i=0; i<previewNote.amplitudeModulatorOutputs.size(); i++) {
      note.amplitudeModulatorOutputs[i] = previewNote.amplitudeModulatorOutputs[i];
    }
    for(int i=0; i<previewNote.frequencyModulatorOutputs.size(); i++) {
      note.frequencyModulatorOutputs[i] = previewNote.frequencyModulatorOutputs[i];
    }
    for(int i=0; i<previewNote.amplitudeModulatorOutputsPrevious.size(); i++) {
      note.amplitudeModulatorOutputsPrevious[i] = previewNote.amplitudeModulatorOutputsPrevious[i];
    }
    for(int i=0; i<previewNote.frequencyModulatorOutputsPrevious.size(); i++) {
      note.frequencyModulatorOutputsPrevious[i] = previewNote.frequencyModulatorOutputsPrevious[i];
    }
    for(int i=0; i<previewNote.envelopeOutputs.size(); i++) {
      note.envelopeOutputs[i] = previewNote.envelopeOutputs[i];
    }
    for(int i=0; i<previewNote.voiceOutputs.size(); i++) {
      note.voiceOutputs[i] = previewNote.voiceOutputs[i];
    }
    
    for(int i=0; i<previewNote.voiceBiquadFilters.size(); i++) {
      note.voiceBiquadFilters[i].set(previewNote.voiceBiquadFilters[i]);
    }
    note.biquadFilter.set(previewNote.biquadFilter);
    
    note.isInitialized = true;
  }



  void initialize(double sampleRate) {
    delayLine.init(sampleRate, 1);
    initBiquad(&delayLine);
    //postProcessingEffects.push_back(new MultiDelay(&delayLine));
  }

  /*void reinitPanel() {
    if(instrumentPanel) {
      instrumentPanel->deleteChildElement(instrumentPanel->voicesPanel);
      instrumentPanel->deleteChildElement(instrumentPanel->modulatorsPanel);
      instrumentPanel->deleteChildElement(instrumentPanel->envelopesPanel);
      instrumentPanel->deleteChildElement(instrumentPanel->modularParameterPanel);
      instrumentPanel->deleteChildElement(instrumentPanel->voiceCrossModulationPanel);
      voiceCrossModulation.voiceCrossModulationPanel = NULL;
      //instrumentPanel->deleteChildElement(biquadFilter->getPanel());
      biquadFilter->removePanel(instrumentPanel);
      
      instrumentPanel->init();
    }
  }*/

  virtual void toggleGuiVisibility(GuiElement *parentGuiElement) {
    if(!instrumentPanel) {
      instrumentPanel = new InstrumentPanel(this, parentGuiElement);
      instrumentPanel->startUpdate();
    }
    else {
      instrumentPanel->toggleVisibility();
      if(instrumentPanel->isVisible) instrumentPanel->startUpdate();
      else  instrumentPanel->stopUpdate();
    }
  }
  
  bool isUpdating() {
    for(int i=0; i<numVoices; i++) {
      if(voices[i].waveForm.isUpdating) return true;
    }
    for(int i=0; i<numModulators; i++) {
      if(modulators[i].waveForm.isUpdating) return true;
    }
    return false;
  }

  bool isVocoder() {
    for(int i=0; i<numVoices; i++) {
      if(voices[i].audioSource == Voice::AudioSource::LineIn) {
        return true;
      }
    }
    return false;
  }

  void reset() {
    
    stopAllNotesRequested = true;
    currentInitializeID++;
    delayLine.reset();
    for(PostProcessingEffect *postProcessingEffects : postProcessingEffects) {
      postProcessingEffects->reset();
    }
  }


  inline void initializeSampling() {
    sampleOut.set(0, 0);
  }

  // TODO maybe preinitialization of notes. Not nesessary to init max amount of mods/voices
  void initializeNote(SynthesisNote &note) {
    
    note.frequency = noteToFreq(note.pitch);
    
    note.instrumentName = name;
    
    note.voiceOutputs.assign(maxNumModulators+1, Vec2d(0, 0));
    note.amplitudeModulatorOutputs.assign(maxNumModulators+1, Vec2d(1, 1));
    note.frequencyModulatorOutputs.assign(maxNumModulators+1, Vec2d(0, 0));
    note.amplitudeModulatorOutputsPrevious.assign(maxNumModulators+1, Vec2d(1, 1));
    note.frequencyModulatorOutputsPrevious.assign(maxNumModulators+1, Vec2d(0, 0));
    note.envelopeOutputs.assign(maxNumEnvelopes+1, 1);

    note.phasesGM.resize(maxNumModulators);
    for(int i=0; i<maxNumModulators; i++) {
      modulators[i].waveForm.initPhase(note.phasesGM[i], note.pitch);
    }

    note.biquadFilter.init(biquadFilter, note.frequency, note.volume);

    note.phasesVoiceNew.resize(maxNumVoices);
    note.voiceBiquadFilters.resize(maxNumVoices);
    
    for(int j=0; j<maxNumVoices; j++) {
      note.phasesVoiceNew[j].resize(maxUnison);
      note.voiceBiquadFilters[j].init(voices[j].biquadFilter, note.frequency, note.volume);
      for(int k=0; k<maxUnison; k++) {
        voices[j].waveForm.initPhase(note.phasesVoiceNew[j][k], note.pitch);
      }
    }
    
    //if(!note.isRecorded) {
      note.noteFullLengthSecs = getNoteMaxLength(note.lengthInSecs);
    //}
    
    note.isInitialized = true;
    
    //unsigned char pitch = (unsigned char)clamp(note.pitch, 0.0, 127.0);
  }

  virtual void update() {
  }

  inline void applyFilters() {
    delayLine.update(sampleOut);
    
    for(int k=0; k<postProcessingEffects.size(); k++) {
      postProcessingEffects[k]->apply(sampleOut);
    }
    
    delayLine.buffer[delayLine.bufferPos] = sampleOut;
  }

  int getTotalOscillatorCount() {
    totalOscillatorCount = 0;
    for(int i=0; i<numVoices; i++) {
      totalOscillatorCount += voices[i].getTotalOscillatorCount();
    }
    for(int i=0; i<numModulators; i++) {
      totalOscillatorCount += modulators[i].getTotalOscillatorCount();
    }
    return totalOscillatorCount;
  }

  // FIXME combined action of envelopes with bigger max value than 1.0 might make the note even longer
  // Currently above (and issue with multiple voices) fixed, but not optimal in many other cases.
  //double getNoteMaxLength(const Note &note) {
  double getNoteMaxLength(double lengthInSecs) {
    double maxLength = 0;

    for(int k=0; k<numVoices; k++) {
      if(voices[k].inputAmplitudeEnvelope >= 0) {
       for(int i=0; i<numEnvelopes; i++) {
         if(envelopes[i].outputIndex == voices[k].inputAmplitudeEnvelope) {
           double el = envelopes[i].getEnvelopeLength();
           maxLength = max(el, maxLength);
          }
        }
      }
    }
    return maxLength + lengthInSecs;
  }

  double getNoteActualLength(double lengthInSecs, bool isHolding = false) {
    double length = 0;

    for(int k=0; k<numVoices; k++) {
      if(voices[k].inputAmplitudeEnvelope >= 0) {
       for(int i=0; i<numEnvelopes; i++) {
         if(envelopes[i].outputIndex == voices[k].inputAmplitudeEnvelope) {
           double el = envelopes[i].getEnvelopeLength(isHolding ? -1 : lengthInSecs);
           length = max(el, length);
          }
        }
      }
    }
    return length;
  }
  /*double getNoteActualLength(const Note &note) {
    double length = 0;

    for(int k=0; k<numVoices; k++) {
      if(voices[k].inputAmplitudeEnvelope >= 0) {
       for(int i=0; i<numEnvelopes; i++) {
         if(envelopes[i].outputIndex == voices[k].inputAmplitudeEnvelope) {
           double el = envelopes[i].getEnvelopeLength(note.isHolding ? -1 : max(note.lengthInSecs, note.keyHoldDuration));
           length = max(el, length);
          }
        }
      }
    }
    return length;
  }*/

  
  inline bool isStopAllNotesRequested() {
    if(stopAllNotesRequested) return true;
    
    for(int i=0; i<numVoices; i++) {
      if(voices[i].waveForm.stopAllNotesRequested) {
        return true;
      }
    }
    for(int i=0; i<numModulators; i++) {
      if(modulators[i].waveForm.stopAllNotesRequested) {
        return true;
      }
    }
    return false;
  }
  
  void resetNoteStopRequest() {
    stopAllNotesRequested = false;
    
    for(int i=0; i<numVoices; i++) {
      voices[i].waveForm.stopAllNotesRequested = false;
    }
    for(int i=0; i<numModulators; i++) {
      modulators[i].waveForm.stopAllNotesRequested = false;
    }
  }

  virtual void getSample(Vec2d &sampleOut, const Vec2d &sampleIn, double dt, double t, SynthesisNote &note, bool applyVolume = true) {
    if(isStopAllNotesRequested() || isMuted || t < 0 || !note.isInitialized) return;
    
    /*if(midiLatencyTestActivated) {
      double latencyFromMidiReadToPlay = midiLatencyTestTicToc.toc();
      printf("latency from midi message read to note play: %f ms\n", latencyFromMidiReadToPlay * 1000.0);
      midiLatencyTestActivated = false;
    }*/
    
    //bool isRecordedNoteAvailable = recordedNotes.count({note.pitch, note.lengthInSecs});
    
    if(note.recordedNote && note.recordedNote->isReadyToPlayRecorded) {
      if(t * note.recordedNote->sampleRate < note.recordedNote->samples.size()) {
        sampleOut += note.recordedNote->samples[(int)(t *  note.recordedNote->sampleRate)] * volume * note.volume;
        note.recordedNote->playTime += dt;
      }
      else {
        note.volume = -1.5;
      }
      return;
    }
    
    if(note.noteFullLengthSecs >= 0 && t > note.noteFullLengthSecs) {
      note.volume = -2;
      return;
    }
      
    //double keyHoldTime = max(note.keyHoldDuration, note.lengthInSecs);
    //bool isKeyHolding = note.isHolding || t <= keyHoldTime;
    bool isKeyHolding = note.isHolding || t <= note.lengthInSecs;
    
    unsigned char pitch = (unsigned char)clamp(note.pitch, 0.0, 127.0);
    
    Vec2d noteOut;
    
    for(int k=0; k<maxNumEnvelopes; k++) {
      note.envelopeOutputs[k] = 1;
    }
    for(int k=0; k<numEnvelopes; k++) {
      envelopes[k].apply(t, note.lengthInSecs, isKeyHolding, note.envelopeOutputs);
    }
    for(int k=numModulators; k >= 0; k--) {
      note.amplitudeModulatorOutputsPrevious[k] = note.amplitudeModulatorOutputs[k];
      note.frequencyModulatorOutputsPrevious[k] = note.frequencyModulatorOutputs[k];
      note.amplitudeModulatorOutputs[k].set(1, 1);
      note.frequencyModulatorOutputs[k].set(0, 0);
    }
    for(int k=numModulators-1; k >= 0; k--) {
      modulators[k].apply(dt, note.frequency, note.phasesGM[k], pitch,
        k <= modulators[k].amplitudeEnvelope ? note.amplitudeModulatorOutputsPrevious : note.amplitudeModulatorOutputs,
        k <= modulators[k].frequencyEnvelope ? note.frequencyModulatorOutputsPrevious : note.frequencyModulatorOutputs,
        note.amplitudeModulatorOutputs, note.frequencyModulatorOutputs, note.envelopeOutputs);
    }
    for(int k=0; k<numVoices; k++) {
      double frequency = note.frequency;
      
      if(voices[k].keyMappingMode == Voice::KeyMappingMode::FixedFrequency) {
        frequency = voices[k].fixedFrequency;
      }
      
      Vec2d modulatedFrequency(frequency, frequency);
      if(voices[k].inputFM >= 0) {
        modulatedFrequency += note.frequencyModulatorOutputs[voices[k].inputFM] * note.frequency;
      }
      if(voices[k].inputFrequencyEnvelope >= 0) {
        modulatedFrequency *= note.envelopeOutputs[voices[k].inputFrequencyEnvelope];
      }
      
      Vec2d out;
      //voice[k].updateModularParameters(note.amplitudeModulatorOutputs, note.envelopeOutputs, note.voiceOutputs);
      
      // FIXME should happen probably in waveform class
      if(voices[k].audioSource == Voice::AudioSource::LineIn) { // I think this functionality go by the name Vocoder usually
        if(!note.pitchChanger) {
          printf("Warning at Instrument.getSample(), !note.pitchChanger, pitch %d\n", pitch);
          note.pitchChanger = new PitchChanger();
        }
        if(note.pitchChanger->delayLine.buffer.size() == 0) {
          printf("Warning at Instrument.getSample(), note.pitchChanger.delayLine.buffer.size() == 0, pitch %d\n", pitch);
          note.pitchChanger->init(delayLine.sampleRate, 0.1, 0.1);
        }
        else {
          note.pitchChanger->transpositionAmount = modulatedFrequency.x / C4_FREQUENCY;
          Vec2d in = sampleIn;
          note.pitchChanger->apply(in);
          out += in * voices[k].volume;
        }
      }
      /*else if(voices[k].waveSource == Voice::WaveSource::Sample) {
        
      }*/
      else {
        voices[k].getSample(out, note.phasesVoiceNew[k], dt, modulatedFrequency, pitch, note.amplitudeModulatorOutputs, note.envelopeOutputs, note.voiceOutputs, voiceCrossModulation, k);
        if(voices[k].waveForm.usePitchDependendGain) {
          out *= voices[k].waveForm.pitchDependendGains[pitch];
        }
      }
      
      if(voices[k].inputAM >= 0) {
        out *= note.amplitudeModulatorOutputs[voices[k].inputAM];
        if(voiceCrossModulation.amplitudeMode == VoiceCrossModulation::AmplitudeMode::Adjustable) {
          note.voiceOutputs[k] *= note.amplitudeModulatorOutputs[voices[k].inputAM];
        }
      }
      if(voices[k].inputAmplitudeEnvelope >= 0) {
        out *= note.envelopeOutputs[voices[k].inputAmplitudeEnvelope];
        if(voiceCrossModulation.amplitudeMode == VoiceCrossModulation::AmplitudeMode::Adjustable) {
          note.voiceOutputs[k] *= note.envelopeOutputs[voices[k].inputAmplitudeEnvelope];
        }
      }
      
      if(voices[k].biquadFilter->isActive) {
        //if(voices[k].biquadChanged) {
          //note.voiceBiquadFilters[k].update(voices[k].biquadFilter);
          voices[k].applyFilterModulars(note.amplitudeModulatorOutputs, note.envelopeOutputs, note.voiceBiquadFilters[k].frequency, note.voiceBiquadFilters[k].bandwidth, note.voiceBiquadFilters[k].wetLevel);
          note.voiceBiquadFilters[k].prepare();
//        }
        note.voiceBiquadFilters[k].apply(out);
      }

      if(applyVolume) {
        out = out * volume * note.volume;
      }
      
      noteOut += out;
      //note.voiceOutputs[k] = out;
    }

    if(biquadFilter->isActive) {
      
      if(filterFrequencyModular.isConnected || filterBandwidthModular.isConnected || filterWetLevelModular.isConnected) {

        if(filterFrequencyModular.isConnected) {
          double env = 1, mod = 1;
          if(filterFrequencyModular.modulator >= 0) {
            mod = note.amplitudeModulatorOutputs[filterFrequencyModular.modulator].x;
          }
          if(filterFrequencyModular.envelope >= 0) {
            env = note.envelopeOutputs[filterFrequencyModular.envelope];
          }
          note.biquadFilter.frequency = mod * env * (filterFrequencyModular.range.y - filterFrequencyModular.range.x) + filterFrequencyModular.range.x;
        }
        
        if(filterBandwidthModular.isConnected) {
          double env = 1, mod = 1;
          if(filterBandwidthModular.modulator >= 0) {
            mod = note.amplitudeModulatorOutputs[filterBandwidthModular.modulator].x;
          }
          if(filterBandwidthModular.envelope >= 0) {
            env = note.envelopeOutputs[filterBandwidthModular.envelope];
          }
          note.biquadFilter.bandwidth = mod * env * (filterBandwidthModular.range.y - filterBandwidthModular.range.x) + filterBandwidthModular.range.x;
        }

        if(filterWetLevelModular.isConnected) {
          double env = 1, mod = 1;
          if(filterWetLevelModular.modulator >= 0) {
            mod = note.amplitudeModulatorOutputs[filterWetLevelModular.modulator].x;
          }
          if(filterWetLevelModular.envelope >= 0) {
            env = note.envelopeOutputs[filterWetLevelModular.envelope];
          }
          note.biquadFilter.wetLevel = mod * env * (filterWetLevelModular.range.y - filterWetLevelModular.range.x) + filterWetLevelModular.range.x;
        }

        
        //note.biquadFilter.update(biquadFilter);
        note.biquadFilter.prepare();
      }
      //printf("earg\n");
      note.biquadFilter.apply(noteOut);
    }
    
    sampleOut += noteOut;
  }

  double getSample(double dt, double frequency, double keyHoldTime, bool isKeyHolding) {
    double totalAmplitude = 0;

    return totalAmplitude;
  }


  /*---------------- GUIs ----------------*/


  struct VoicesPanel : public Panel {
    struct VoicesPanelListener : public GuiEventListener {
      VoicesPanel *voicesPanel;
      VoicesPanelListener(VoicesPanel *voicesPanel) {
        this->voicesPanel = voicesPanel;
      }
      enum DraggingVoiceMode { NotDragging, SwapDragging, CopyDragging };
      DraggingVoiceMode draggingVoice = DraggingVoiceMode::NotDragging;
      int voiceOrderSwapIndexA = 0;
      int voiceOrderSwapIndexB = 0;
      
      void onMousePressed(GuiElement *guiElement, Events &events) override {
        for(int i=0; i<voicesPanel->instrument->numVoices; i++) {
          if(voicesPanel->voicePreviewPanels[i]->isPointWithin(events.m)) {
            if(events.mouseButton == 1) {
              draggingVoice = DraggingVoiceMode::SwapDragging;
              voiceOrderSwapIndexA = i;
              voiceOrderSwapIndexB = i;
            }
            if(events.mouseButton == 2) {
              draggingVoice = DraggingVoiceMode::CopyDragging;
              voiceOrderSwapIndexA = i;
              voiceOrderSwapIndexB = i;
            }
          }
        }
      }
      void onMouseReleased(GuiElement *guiElement, Events &events) override {
        if(draggingVoice == DraggingVoiceMode::SwapDragging) {
          voicesPanel->instrument->stopAllNotesRequested = true;
          /*for(int i=0; i<voicesPanel->instrument->numVoices; i++) {
            if(voicesPanel->voicePreviewPanels[i]->isPointWithin(events.m)) {
              voiceOrderSwapIndexB = i;
            }
          }*/
          if(voiceOrderSwapIndexA != voiceOrderSwapIndexB) {
            if(voicesPanel->instrument->voices[voiceOrderSwapIndexA].getPanel()) {
              voicesPanel->instrument->voices[voiceOrderSwapIndexA].removePanel(voicesPanel);
            }
            if(voicesPanel->instrument->voices[voiceOrderSwapIndexB].getPanel()) {
              voicesPanel->instrument->voices[voiceOrderSwapIndexB].removePanel(voicesPanel);
            }
            Voice tmpVoice;
            tmpVoice = voicesPanel->instrument->voices[voiceOrderSwapIndexA];
            voicesPanel->instrument->voices[voiceOrderSwapIndexA] = voicesPanel->instrument->voices[voiceOrderSwapIndexB];
            voicesPanel->instrument->voices[voiceOrderSwapIndexB] = tmpVoice;
            voicesPanel->update();
          }
        }
        if(draggingVoice == DraggingVoiceMode::CopyDragging) {
          voicesPanel->instrument->stopAllNotesRequested = true;
          
          if(voiceOrderSwapIndexA != voiceOrderSwapIndexB) {
            if(voicesPanel->instrument->voices[voiceOrderSwapIndexB].getPanel()) {
              voicesPanel->instrument->voices[voiceOrderSwapIndexB].removePanel(voicesPanel);
            }
            voicesPanel->instrument->voices[voiceOrderSwapIndexB] = voicesPanel->instrument->voices[voiceOrderSwapIndexA];
            voicesPanel->update();
          }
        }
        draggingVoice = DraggingVoiceMode::NotDragging;
      }
      void onMouseMotion(GuiElement *guiElement, Events &events) override {
        if(draggingVoice != DraggingVoiceMode::NotDragging) {
          for(int i=0; i<voicesPanel->instrument->numVoices; i++) {
            if(voicesPanel->voicePreviewPanels[i]->isPointWithin(events.m)) {
              voiceOrderSwapIndexB = i;
            }
          }
        }
      }
      void onValueChange(GuiElement *guiElement) {
        if(guiElement == voicesPanel->numVoicesGui) {
          //instrument->currentInitializeID++;
          //instrument->stopAllNotesRequested = true;

          guiElement->getValue((void*)&voicesPanel->instrument->numVoices);
          for(int i=voicesPanel->instrument->numVoices; i<maxNumVoices; i++) {
            if(voicesPanel->instrument->voices[i].getPanel()) {
              voicesPanel->instrument->voices[i].removePanel(voicesPanel);
            }
          }
          voicesPanel->update();
          
          voicesPanel->sendMessage("modparchange");
          voicesPanel->sendMessage({"voiceschange", std::to_string(voicesPanel->instrument->numVoices)});
          //voicesPanel->instrument->voiceCrossModulation.setNumVoices(voicesPanel->instrument->numVoices);
        }

        for(int i=0; i<voicesPanel->instrument->numVoices; i++) {
          if(guiElement == voicesPanel->voiceOpenGuiGuis[i]) {
            if(voicesPanel->instrument->voices[i].getPanel()) {
              voicesPanel->instrument->voices[i].removePanel(voicesPanel);
            }
            else {
              voicesPanel->instrument->voices[i].addPanel(voicesPanel, "Voice " + std::to_string(i+1));
              voicesPanel->instrument->voices[i].setPreviousPanelPosition();
            }
          }
          
          if(guiElement == voicesPanel->voiceAmplitudeModulatorInputIndexGuis[i]) {
            guiElement->getValue((void*)&voicesPanel->instrument->voices[i].inputAM);
          }
          if(guiElement == voicesPanel->voiceFrequencyModulatorInputIndexGuis[i]) {
            guiElement->getValue((void*)&voicesPanel->instrument->voices[i].inputFM);
          }
          if(guiElement == voicesPanel->voiceAmplitudeEnvelopeIndexGuis[i]) {
            guiElement->getValue((void*)&voicesPanel->instrument->voices[i].inputAmplitudeEnvelope);
          }
          if(guiElement == voicesPanel->voiceFrequencyEnvelopeIndexGuis[i]) {
            guiElement->getValue((void*)&voicesPanel->instrument->voices[i].inputFrequencyEnvelope);
          }
        }
      }

      void onKeyDown(GuiElement *guiElement, Events &events) {
        if(events.sdlKeyCode == SDLK_HOME) {
          for(int i=0; i<voicesPanel->instrument->numVoices; i++) {
            if(guiElement == voicesPanel->voiceAmplitudeModulatorInputIndexGuis[i] ||
               guiElement == voicesPanel->voiceFrequencyModulatorInputIndexGuis[i] ||
               guiElement == voicesPanel->voiceAmplitudeEnvelopeIndexGuis[i] ||
               guiElement == voicesPanel->voiceFrequencyEnvelopeIndexGuis[i]) {

              if(voicesPanel->instrument->voices[i].getPanel()) {
                voicesPanel->instrument->voices[i].removePanel(voicesPanel);
              }
              else {
                voicesPanel->instrument->voices[i].addPanel(voicesPanel, "Voice " + std::to_string(i+1));
                voicesPanel->instrument->voices[i].setPreviousPanelPosition();
              }
            }
          }
        }
      }
    };
    struct VoicePreviewPanel : public Panel {
      int voiceIndex = 0;
      Instrument *instrument = NULL;
      std::vector<double> waveForm;
      long currentWaveCycle = 0;
      
      VoicePreviewPanel(Instrument *instrument, int voiceIndex) : Panel("Voice preview panel") {
        drawBorder = false;
        drawBackground = false;
        draggable = false;
        this->instrument = instrument;
        this->voiceIndex = voiceIndex;
        setSize(70, 23);
        waveForm.resize(70, 0);
      }
            
      virtual void onRender(GeomRenderer &geomRenderer, TextGl &textRenderer) {
        if(!isVisible) return;
        
        Voice &voice = instrument->voices[voiceIndex];
        
        geomRenderer.texture = NULL;
        geomRenderer.strokeType = 1;
        geomRenderer.strokeWidth = 1;        
        //geomRenderer.fillColor.set(1, 1, 1, 0.5);
        if(waveForm.size() != (int)size.x) {
          waveForm.assign(size.x, 0);
        }
        
        double minValue = 0, maxValue = 0;
        double k = voice.waveForm.cyclesPerWaveTable > 1 ? ((currentWaveCycle++) % long(voice.waveForm.cyclesPerWaveTable)) : 0;
        for(int x=0; x<size.x; x++) {
          //int i = ((double)(x/(size.x-1)) * voice.waveForm.waveTableSizeM1);
          double t = k + (double)x/(size.x-1.0);
          //waveForm[x] = voice.waveForm.waveTable[i];
          waveForm[x] = voice.waveForm.getSample(t);
          minValue = min(minValue, waveForm[x]);
          maxValue = max(maxValue, waveForm[x]);
        }
        if(minValue < maxValue && minValue != -1 && maxValue != 1) {
          for(int x=0; x<size.x; x++) {
            waveForm[x] = map(waveForm[x], minValue, maxValue, -1, 1);
          }
        }
        geomRenderer.strokeColor.set(1, 1, 1, 0.5);
        geomRenderer.startRendering(GeomRenderer::RendererType::FastStrokeRenderer);
        for(int x=1; x<size.x; x++) {
          geomRenderer.drawLine(x-1, size.y*0.36 * (1.0 - waveForm[x-1]), x, size.y*0.36 * (1.0 - waveForm[x]), absolutePos);
        }
        geomRenderer.endRendering();
      }
    };
    
    
    
    NumberBox *numVoicesGui = NULL;
    std::vector<Button*> voiceOpenGuiGuis = std::vector<Button*>(maxNumVoices, NULL);
    std::vector<Label*> voiceLabels = std::vector<Label*>(maxNumVoices, NULL);
    std::vector<VoicePreviewPanel*> voicePreviewPanels = std::vector<VoicePreviewPanel*>(maxNumVoices, NULL);
    std::vector<NumberBox*> voiceAmplitudeModulatorInputIndexGuis = std::vector<NumberBox*>(maxNumVoices, NULL);
    std::vector<NumberBox*> voiceFrequencyModulatorInputIndexGuis = std::vector<NumberBox*>(maxNumVoices, NULL);
    std::vector<NumberBox*> voiceAmplitudeEnvelopeIndexGuis = std::vector<NumberBox*>(maxNumVoices, NULL);
    std::vector<NumberBox*> voiceFrequencyEnvelopeIndexGuis = std::vector<NumberBox*>(maxNumVoices, NULL);
    
    Instrument *instrument = NULL;
    GuiElement *parentGuiElement = NULL;

    double lineHeight = 23;
    int width = 40, widthA = 25, widthB = 100;
    int columnA = 10, columnB = 10+widthA, columnC = 10+widthA+widthB, columnD = 10+widthA+widthB+width, columnE = 10+widthA+widthB+width*2;
    int columnF = 10+widthA+widthB+width*3;

    VoicesPanelListener *voicesPanelListener = NULL;

    VoicesPanel(Instrument *instrument, GuiElement *parentGuiElement) : Panel("Voices panel") {
      init(instrument, parentGuiElement);
    }

    void init(Instrument *instrument, GuiElement *parentGuiElement) {
      this->instrument = instrument;
      this->parentGuiElement = parentGuiElement;
      
      double line = 10;
      this->addGuiEventListener(voicesPanelListener = new VoicesPanelListener(this));
      parentGuiElement->addChildElement(this);

      addChildElement(numVoicesGui = new NumberBox("Voices", instrument->numVoices, NumberBox::INTEGER, 0, maxNumVoices, 10, line, 2));
      numVoicesGui->incrementMode = NumberBox::IncrementMode::Linear;
      numVoicesGui->setTextSize(10);
      numVoicesGui->labelColor.set(1, 1, 1, 0.9);


      addChildElement(new Label("AM", columnC, line, 10, Vec4d(1, 1, 1, 0.9)));
      addChildElement(new Label("FM", columnD, line, 10, Vec4d(1, 1, 1, 0.9)));
      addChildElement(new Label("Amp", columnE, line, 10, Vec4d(1, 1, 1, 0.9)));
      addChildElement(new Label("Frq", columnF, line, 10, Vec4d(1, 1, 1, 0.9)));

      //this->setPosition(instrumentPanel->pos + Vec2d(0, 80));

      for(int i=0; i<maxNumVoices; i++) {
        line += lineHeight;
        addChildElement(voiceLabels[i] = new Label(std::to_string(i+1), columnA, line));
        
        addChildElement(voicePreviewPanels[i] = new VoicePreviewPanel(instrument, i));
        voicePreviewPanels[i]->setPosition(columnB, line+4);

        addChildElement(voiceOpenGuiGuis[i] = new Button("Open voice GUI", "data/synth/textures/gui.png", columnC-20, line+2, Button::ToggleButton));
        //voiceOpenGuiGuis[i]->pressedOverlayColor.set(1, 1, 1, 0.22);
        
        voiceAmplitudeModulatorInputIndexGuis[i] = new NumberBox("", instrument->voices[i].inputAM, NumberBox::INTEGER, -1, maxNumModulators, columnC, line, 2);
        voiceAmplitudeModulatorInputIndexGuis[i]->incrementMode = NumberBox::IncrementMode::Linear;
        voiceAmplitudeModulatorInputIndexGuis[i]->setValidValueRange(0, 16);
        this->addChildElement(voiceAmplitudeModulatorInputIndexGuis[i]);
        

        voiceFrequencyModulatorInputIndexGuis[i] = new NumberBox("", instrument->voices[i].inputFM, NumberBox::INTEGER, -1, maxNumModulators, columnD, line, 2);
        voiceFrequencyModulatorInputIndexGuis[i]->incrementMode = NumberBox::IncrementMode::Linear;
        voiceFrequencyModulatorInputIndexGuis[i]->setValidValueRange(0, 16);
        this->addChildElement(voiceFrequencyModulatorInputIndexGuis[i]);

        voiceAmplitudeEnvelopeIndexGuis[i] = new NumberBox("", instrument->voices[i].inputAmplitudeEnvelope, NumberBox::INTEGER, -1, maxNumEnvelopes, columnE, line, 2);
        voiceAmplitudeEnvelopeIndexGuis[i]->incrementMode = NumberBox::IncrementMode::Linear;
        voiceAmplitudeEnvelopeIndexGuis[i]->setValidValueRange(0, 16);
        this->addChildElement(voiceAmplitudeEnvelopeIndexGuis[i]);

        voiceFrequencyEnvelopeIndexGuis[i] = new NumberBox("", instrument->voices[i].inputFrequencyEnvelope, NumberBox::INTEGER, -1, maxNumEnvelopes, columnF, line, 2);
        voiceFrequencyEnvelopeIndexGuis[i]->incrementMode = NumberBox::IncrementMode::Linear;
        voiceFrequencyEnvelopeIndexGuis[i]->setValidValueRange(0, 16);
        this->addChildElement(voiceFrequencyEnvelopeIndexGuis[i]);
      }
      
      update();
    }

    void onRender(GeomRenderer &geomRenderer, TextGl &textRenderer) override {
      if(voicesPanelListener->draggingVoice != VoicesPanelListener::DraggingVoiceMode::NotDragging && voicesPanelListener->voiceOrderSwapIndexA != voicesPanelListener->voiceOrderSwapIndexB) {
        geomRenderer.strokeColor.set(1, 1, 1, 0.75);
        geomRenderer.fillColor.set(1, 1, 1, 0.75);
        geomRenderer.strokeType = 1;
        geomRenderer.strokeWidth = 1;
        double x = absolutePos.x + voicePreviewPanels[0]->pos.x - 2 - 5;
        double y1 = absolutePos.y + voicePreviewPanels[voicesPanelListener->voiceOrderSwapIndexA]->pos.y + voicePreviewPanels[0]->size.y * 0.33;
        double y2 = absolutePos.y + voicePreviewPanels[voicesPanelListener->voiceOrderSwapIndexB]->pos.y + voicePreviewPanels[0]->size.y * 0.33;
        
        geomRenderer.drawLine(x, y1, x, y2);
        double t = y2 < y1 ? 1 : -1;

        if(voicesPanelListener->draggingVoice == VoicesPanelListener::DraggingVoiceMode::SwapDragging) {
          geomRenderer.drawLine(x+5, y2+t*5, x, y2);
          geomRenderer.drawLine(x-5, y2+t*5, x, y2);
          
          geomRenderer.drawLine(x+5, y1-t*5, x, y1);
          geomRenderer.drawLine(x-5, y1-t*5, x, y1);
        }
        if(voicesPanelListener->draggingVoice == VoicesPanelListener::DraggingVoiceMode::CopyDragging) {
          geomRenderer.drawLine(x+5, y2+t*5, x, y2);
          geomRenderer.drawLine(x-5, y2+t*5, x, y2);
        }

      }
    }

    void update() {
      
      numVoicesGui->setValue(instrument->numVoices);
      
      for(int i=0; i<maxNumVoices; i++) {
        voiceAmplitudeModulatorInputIndexGuis[i]->setValue(instrument->voices[i].inputAM);
        voiceFrequencyModulatorInputIndexGuis[i]->setValue(instrument->voices[i].inputFM);
        voiceAmplitudeEnvelopeIndexGuis[i]->setValue(instrument->voices[i].inputAmplitudeEnvelope);
        voiceFrequencyEnvelopeIndexGuis[i]->setValue(instrument->voices[i].inputFrequencyEnvelope);
        
        voiceOpenGuiGuis[i]->setValue(false);
        //voiceOpenGuiGuis[i]->setValue(instrument->voices[i].getPanel() != NULL && instrument->voices[i].getPanel()->isVisible);
        
        voiceLabels[i]->setVisible(i < instrument->numVoices);
        voicePreviewPanels[i]->setVisible(i < instrument->numVoices);
        voiceOpenGuiGuis[i]->setVisible(i < instrument->numVoices);
        voiceAmplitudeModulatorInputIndexGuis[i]->setVisible(i < instrument->numVoices);
        voiceFrequencyModulatorInputIndexGuis[i]->setVisible(i < instrument->numVoices);
        voiceAmplitudeEnvelopeIndexGuis[i]->setVisible(i < instrument->numVoices);
        voiceFrequencyEnvelopeIndexGuis[i]->setVisible(i < instrument->numVoices);
        
        
      }
      setSize(columnF + width + 5, 10 + lineHeight * (instrument->numVoices+1) + 10);
    }



  };



  struct ModulatorsPanel : public Panel {
    struct ModulatorsPanelListener : public GuiEventListener {
      ModulatorsPanel *modulatorsPanel;
      ModulatorsPanelListener(ModulatorsPanel *modulatorsPanel) {
        this->modulatorsPanel = modulatorsPanel;
      }
      enum DraggingModulatorMode { NotDragging, SwapDragging, CopyDragging };
      DraggingModulatorMode draggingModulator = DraggingModulatorMode::NotDragging;
      int modulatorOrderSwapIndexA = 0;
      int modulatorOrderSwapIndexB = 0;
      
      void onMousePressed(GuiElement *guiElement, Events &events) override {
        for(int i=0; i<modulatorsPanel->instrument->numModulators; i++) {
          if(modulatorsPanel->modulatorPreviewPanels[i]->isPointWithin(events.m)) {
            if(events.mouseButton == 1) {
              draggingModulator = DraggingModulatorMode::SwapDragging;
              modulatorOrderSwapIndexA = i;
              modulatorOrderSwapIndexB = i;
            }
            if(events.mouseButton == 2) {
              draggingModulator = DraggingModulatorMode::CopyDragging;
              modulatorOrderSwapIndexA = i;
              modulatorOrderSwapIndexB = i;
            }
          }
        }
      }
      void onMouseReleased(GuiElement *guiElement, Events &events) override {
        if(draggingModulator == DraggingModulatorMode::SwapDragging) {
          modulatorsPanel->instrument->stopAllNotesRequested = true;
          
          if(modulatorOrderSwapIndexA != modulatorOrderSwapIndexB) {
            if(modulatorsPanel->instrument->modulators[modulatorOrderSwapIndexA].getPanel()) {
              modulatorsPanel->instrument->modulators[modulatorOrderSwapIndexA].removePanel(modulatorsPanel);
            }
            if(modulatorsPanel->instrument->modulators[modulatorOrderSwapIndexB].getPanel()) {
              modulatorsPanel->instrument->modulators[modulatorOrderSwapIndexB].removePanel(modulatorsPanel);
            }
            GenericModulator tmpModulator;
            tmpModulator = modulatorsPanel->instrument->modulators[modulatorOrderSwapIndexA];
            modulatorsPanel->instrument->modulators[modulatorOrderSwapIndexA] = modulatorsPanel->instrument->modulators[modulatorOrderSwapIndexB];
            modulatorsPanel->instrument->modulators[modulatorOrderSwapIndexB] = tmpModulator;
            modulatorsPanel->update();            
          }
        }
        if(draggingModulator == DraggingModulatorMode::CopyDragging) {
          modulatorsPanel->instrument->stopAllNotesRequested = true;
          
          if(modulatorOrderSwapIndexA != modulatorOrderSwapIndexB) {
            if(modulatorsPanel->instrument->modulators[modulatorOrderSwapIndexA].getPanel()) {
              modulatorsPanel->instrument->modulators[modulatorOrderSwapIndexA].removePanel(modulatorsPanel);
            }
            GenericModulator tmpModulator;
            modulatorsPanel->instrument->modulators[modulatorOrderSwapIndexB] = modulatorsPanel->instrument->modulators[modulatorOrderSwapIndexA];
            modulatorsPanel->update();            
          }
        }
        draggingModulator = DraggingModulatorMode::NotDragging;
      }
      void onMouseMotion(GuiElement *guiElement, Events &events) override {
        if(draggingModulator != DraggingModulatorMode::NotDragging) {
          for(int i=0; i<modulatorsPanel->instrument->numModulators; i++) {
            if(modulatorsPanel->modulatorPreviewPanels[i]->isPointWithin(events.m)) {
              modulatorOrderSwapIndexB = i;
            }
          }
        }
      }

      void onValueChange(GuiElement *guiElement) {
        if(guiElement == modulatorsPanel->numModulatorsGui) {
          //instrument->currentInitializeID++;
          //instrument->stopAllNotesRequested = true;

          guiElement->getValue((void*)&modulatorsPanel->instrument->numModulators);
          for(int i=modulatorsPanel->instrument->numModulators; i<maxNumModulators; i++) {
            if(modulatorsPanel->instrument->modulators[i].getPanel()) {
              modulatorsPanel->instrument->modulators[i].removePanel(modulatorsPanel);
            }
          }
          modulatorsPanel->update();
          
          modulatorsPanel->sendMessage("modparchange");
        }
        for(int i=0; i<modulatorsPanel->instrument->numModulators; i++) {
          if(guiElement == modulatorsPanel->modulatorOpenGuiGuis[i]) {
            if(modulatorsPanel->instrument->modulators[i].getPanel()) {
              modulatorsPanel->instrument->modulators[i].removePanel(modulatorsPanel);
            }
            else {
              modulatorsPanel->instrument->modulators[i].addPanel(modulatorsPanel, "Modulator " + std::to_string(i+1));
              modulatorsPanel->instrument->modulators[i].setPreviousPanelPosition();
            }
          }
          
          if(guiElement == modulatorsPanel->genericModulatorAmplitudeModulatorOutputIndexGuis[i]) {
            guiElement->getValue((void*)&modulatorsPanel->instrument->modulators[i].outputAM);
          }
          if(guiElement == modulatorsPanel->genericModulatorFrequencyModulatorOutputIndexGuis[i]) {
            guiElement->getValue((void*)&modulatorsPanel->instrument->modulators[i].outputFM);
          }
          if(guiElement == modulatorsPanel->genericModulatorAmplitudeModulatorInputIndexGuis[i]) {
            guiElement->getValue((void*)&modulatorsPanel->instrument->modulators[i].inputAM);
          }
          if(guiElement == modulatorsPanel->genericModulatorFrequencyModulatorInputIndexGuis[i]) {
            guiElement->getValue((void*)&modulatorsPanel->instrument->modulators[i].inputFM);
          }
          if(guiElement == modulatorsPanel->genericModulatorAmplitudeEnvelopeIndexGuis[i]) {
            guiElement->getValue((void*)&modulatorsPanel->instrument->modulators[i].amplitudeEnvelope);
          }
          if(guiElement == modulatorsPanel->genericModulatorFrequencyEnvelopeIndexGuis[i]) {
            guiElement->getValue((void*)&modulatorsPanel->instrument->modulators[i].frequencyEnvelope);
          }
        }
      }

      void onKeyDown(GuiElement *guiElement, Events &events) {
        if(events.sdlKeyCode == SDLK_HOME) {
          for(int i=0; i<modulatorsPanel->instrument->numModulators; i++) {
            if(guiElement == modulatorsPanel->genericModulatorAmplitudeModulatorOutputIndexGuis[i] ||
               guiElement == modulatorsPanel->genericModulatorFrequencyModulatorOutputIndexGuis[i] ||
               guiElement == modulatorsPanel->genericModulatorAmplitudeModulatorInputIndexGuis[i] ||
               guiElement == modulatorsPanel->genericModulatorFrequencyModulatorInputIndexGuis[i] ||
               guiElement == modulatorsPanel->genericModulatorAmplitudeEnvelopeIndexGuis[i] ||
               guiElement == modulatorsPanel->genericModulatorFrequencyEnvelopeIndexGuis[i]) {

              if(modulatorsPanel->instrument->modulators[i].getPanel()) {
                modulatorsPanel->instrument->modulators[i].removePanel(modulatorsPanel);
              }
              else {
                modulatorsPanel->instrument->modulators[i].addPanel(modulatorsPanel, "Modulator " + std::to_string(i+1));
                modulatorsPanel->instrument->modulators[i].setPreviousPanelPosition();
              }
            }
          }
        }
      }

    };
    struct ModulatorPreviewPanel : public Panel {
      int modulatorIndex = 0;
      Instrument *instrument = NULL;
      std::vector<double> waveForm;
      long currentWaveCycle = 0;
      
      ModulatorPreviewPanel(Instrument *instrument, int modulatorIndex) : Panel("Voice preview panel") {
        drawBorder = false;
        drawBackground = false;
        draggable = false;
        this->instrument = instrument;
        this->modulatorIndex = modulatorIndex;
        setSize(70, 23);
        waveForm.resize(70, 0);
      }
            
      virtual void onRender(GeomRenderer &geomRenderer, TextGl &textRenderer) {
        if(!isVisible) return;
        
        GenericModulator &modulator = instrument->modulators[modulatorIndex];
        
        geomRenderer.texture = NULL;
        geomRenderer.strokeType = 1;
        geomRenderer.strokeWidth = 1;        
        //geomRenderer.fillColor.set(1, 1, 1, 0.5);
        if(waveForm.size() != (int)size.x) {
          waveForm.assign(size.x, 0);
        }
        
        double minValue = 0, maxValue = 0;
        double k = modulator.waveForm.cyclesPerWaveTable > 1 ? ((currentWaveCycle++) % long(modulator.waveForm.cyclesPerWaveTable)) : 0;

        for(int x=0; x<size.x; x++) {
          //int i = ((double)(x/(size.x-1)) * voice.waveForm.waveTableSizeM1);
          double t = k + (double)x/(size.x-1.0);
          //waveForm[x] = voice.waveForm.waveTable[i];
          waveForm[x] = modulator.waveForm.getSample(t);
          minValue = min(minValue, waveForm[x]);
          maxValue = max(maxValue, waveForm[x]);
        }
        if(minValue < maxValue && minValue != -1 && maxValue != 1) {
          for(int x=0; x<size.x; x++) {
            waveForm[x] = map(waveForm[x], minValue, maxValue, -1, 1);
          }
        }
        geomRenderer.strokeColor.set(1, 1, 1, 0.5);
        geomRenderer.startRendering(GeomRenderer::RendererType::FastStrokeRenderer);
        for(int x=1; x<size.x; x++) {
          geomRenderer.drawLine(x-1, size.y*0.36 * (1.0 - waveForm[x-1]), x, size.y*0.36 * (1.0 - waveForm[x]), absolutePos);
        }
        geomRenderer.endRendering();
      }
    };
    
    
    NumberBox *numModulatorsGui = NULL;//, *numEnvelopesGui = NULL;
    std::vector<Button*> modulatorOpenGuiGuis = std::vector<Button*>(maxNumModulators, NULL);
    std::vector<Label*> modulatorLabels = std::vector<Label*>(maxNumModulators, NULL);
    std::vector<ModulatorPreviewPanel*> modulatorPreviewPanels = std::vector<ModulatorPreviewPanel*>(maxNumModulators, NULL);
    
    std::vector<NumberBox*> genericModulatorAmplitudeModulatorOutputIndexGuis = std::vector<NumberBox*>(maxNumModulators, NULL);
    std::vector<NumberBox*> genericModulatorFrequencyModulatorOutputIndexGuis = std::vector<NumberBox*>(maxNumModulators, NULL);
    std::vector<NumberBox*> genericModulatorAmplitudeModulatorInputIndexGuis = std::vector<NumberBox*>(maxNumModulators, NULL);
    std::vector<NumberBox*> genericModulatorFrequencyModulatorInputIndexGuis = std::vector<NumberBox*>(maxNumModulators, NULL);
    std::vector<NumberBox*> genericModulatorAmplitudeEnvelopeIndexGuis = std::vector<NumberBox*>(maxNumEnvelopes, NULL);
    std::vector<NumberBox*> genericModulatorFrequencyEnvelopeIndexGuis = std::vector<NumberBox*>(maxNumEnvelopes, NULL);

    ModulatorsPanelListener *modulatorsPanelListener = NULL;

    Instrument *instrument = NULL;
    GuiElement *parentGuiElement = NULL;

    double lineHeight = 23;
    int width = 40, widthA = 25, widthB = 100;
    int columnA = 10, columnB = 10+widthA, columnC = 10+widthA+widthB, columnD = 10+widthA+widthB+width, columnE = 10+widthA+widthB+width*2;
    int columnF = 10+widthA+widthB+width*3, columnG = 10+widthA+widthB+width*4, columnH = 10+widthA+widthB+width*5;

    ModulatorsPanel(Instrument *instrument, GuiElement *parentGuiElement) : Panel("Modulators panel") {
      init(instrument, parentGuiElement);
    }

    void init(Instrument *instrument, GuiElement *parentGuiElement) {
      this->instrument = instrument;
      this->parentGuiElement = parentGuiElement;
      double line = 10;

      this->addGuiEventListener(modulatorsPanelListener = new ModulatorsPanelListener(this));
      parentGuiElement->addChildElement(this);

      numModulatorsGui = new NumberBox("Modulators", instrument->numModulators, NumberBox::INTEGER, 0, maxNumModulators, 10, line, 2);
      numModulatorsGui->incrementMode = NumberBox::IncrementMode::Linear;
      numModulatorsGui->setTextSize(10);
      numModulatorsGui->labelColor.set(1, 1, 1, 0.9);
      this->addChildElement(numModulatorsGui);

      this->addChildElement(new Label("AM→", columnC, line, 10, Vec4d(1, 1, 1, 0.9)));
      this->addChildElement(new Label("FM→", columnD, line, 10, Vec4d(1, 1, 1, 0.9)));
      this->addChildElement(new Label("AM", columnE, line, 10, Vec4d(1, 1, 1, 0.9)));
      this->addChildElement(new Label("FM", columnF, line, 10, Vec4d(1, 1, 1, 0.9)));
      this->addChildElement(new Label("Amp", columnG, line, 10, Vec4d(1, 1, 1, 0.9)));
      this->addChildElement(new Label("Frq", columnH, line, 10, Vec4d(1, 1, 1, 0.9)));

      //modulatorsPanel->setPosition(instrumentPanel->pos + Vec2d(-columnF - width - 10, 0));

      for(int i=0; i<maxNumModulators; i++) {
        line += lineHeight;
        
        addChildElement(modulatorLabels[i] = new Label(std::to_string(i+1), columnA, line));
        
        addChildElement(modulatorPreviewPanels[i] = new ModulatorPreviewPanel(instrument, i));
        modulatorPreviewPanels[i]->setPosition(columnB, line+4);

        addChildElement(modulatorOpenGuiGuis[i] = new Button("Open modulator GUI", "data/synth/textures/gui.png", columnC-20, line+2, Button::ToggleButton));
        //modulatorOpenGuiGuis[i]->pressedOverlayColor.set(1, 1, 1, 0.22);

        genericModulatorAmplitudeModulatorOutputIndexGuis[i] = new NumberBox("", instrument->modulators[i].outputAM, NumberBox::INTEGER, -1, maxNumModulators, columnC, line, 2);
        genericModulatorAmplitudeModulatorOutputIndexGuis[i]->incrementMode = NumberBox::IncrementMode::Linear;
        genericModulatorAmplitudeModulatorOutputIndexGuis[i]->setValidValueRange(0, 16);
        this->addChildElement(genericModulatorAmplitudeModulatorOutputIndexGuis[i]);

        genericModulatorFrequencyModulatorOutputIndexGuis[i] = new NumberBox("", instrument->modulators[i].outputFM, NumberBox::INTEGER, -1, maxNumModulators, columnD, line, 2);
        genericModulatorFrequencyModulatorOutputIndexGuis[i]->incrementMode = NumberBox::IncrementMode::Linear;
        genericModulatorFrequencyModulatorOutputIndexGuis[i]->setValidValueRange(0, 16);
        this->addChildElement(genericModulatorFrequencyModulatorOutputIndexGuis[i]);

        genericModulatorAmplitudeModulatorInputIndexGuis[i] = new NumberBox("", instrument->modulators[i].inputAM, NumberBox::INTEGER, -1, maxNumModulators, columnE, line, 2);
        genericModulatorAmplitudeModulatorInputIndexGuis[i]->incrementMode = NumberBox::IncrementMode::Linear;
        genericModulatorAmplitudeModulatorInputIndexGuis[i]->setValidValueRange(0, 16);
        this->addChildElement(genericModulatorAmplitudeModulatorInputIndexGuis[i]);

        genericModulatorFrequencyModulatorInputIndexGuis[i] = new NumberBox("", instrument->modulators[i].inputFM, NumberBox::INTEGER, -1, maxNumModulators, columnF, line, 2);
        genericModulatorFrequencyModulatorInputIndexGuis[i]->setValidValueRange(0, 16);
        genericModulatorFrequencyModulatorInputIndexGuis[i]->incrementMode = NumberBox::IncrementMode::Linear;
        this->addChildElement(genericModulatorFrequencyModulatorInputIndexGuis[i]);

        genericModulatorAmplitudeEnvelopeIndexGuis[i] = new NumberBox("", instrument->modulators[i].amplitudeEnvelope, NumberBox::INTEGER, -1, maxNumEnvelopes, columnG, line, 2);
        genericModulatorAmplitudeEnvelopeIndexGuis[i]->setValidValueRange(0, 16);
        genericModulatorAmplitudeEnvelopeIndexGuis[i]->incrementMode = NumberBox::IncrementMode::Linear;
        this->addChildElement(genericModulatorAmplitudeEnvelopeIndexGuis[i]);

        genericModulatorFrequencyEnvelopeIndexGuis[i] = new NumberBox("", instrument->modulators[i].frequencyEnvelope, NumberBox::INTEGER, -1, maxNumEnvelopes, columnH, line, 2);
        genericModulatorFrequencyEnvelopeIndexGuis[i]->setValidValueRange(0, 16);
        genericModulatorFrequencyEnvelopeIndexGuis[i]->incrementMode = NumberBox::IncrementMode::Linear;
        this->addChildElement(genericModulatorFrequencyEnvelopeIndexGuis[i]);
      }

      update();
    }
    
    void onRender(GeomRenderer &geomRenderer, TextGl &textRenderer) override {
      if(modulatorsPanelListener->draggingModulator != ModulatorsPanelListener::DraggingModulatorMode::NotDragging && modulatorsPanelListener->modulatorOrderSwapIndexA != modulatorsPanelListener->modulatorOrderSwapIndexB) {
        geomRenderer.strokeColor.set(1, 1, 1, 0.75);
        geomRenderer.fillColor.set(1, 1, 1, 0.75);
        geomRenderer.strokeType = 1;
        geomRenderer.strokeWidth = 1;
        double x = absolutePos.x + modulatorPreviewPanels[0]->pos.x - 2 - 5;
        double y1 = absolutePos.y + modulatorPreviewPanels[modulatorsPanelListener->modulatorOrderSwapIndexA]->pos.y + modulatorPreviewPanels[0]->size.y * 0.33;
        double y2 = absolutePos.y + modulatorPreviewPanels[modulatorsPanelListener->modulatorOrderSwapIndexB]->pos.y + modulatorPreviewPanels[0]->size.y * 0.33;
        
        geomRenderer.drawLine(x, y1, x, y2);
        double t = y2 < y1 ? 1 : -1;

        if(modulatorsPanelListener->draggingModulator == ModulatorsPanelListener::DraggingModulatorMode::SwapDragging) {
          geomRenderer.drawLine(x+5, y2+t*5, x, y2);
          geomRenderer.drawLine(x-5, y2+t*5, x, y2);
          
          geomRenderer.drawLine(x+5, y1-t*5, x, y1);
          geomRenderer.drawLine(x-5, y1-t*5, x, y1);
        }
        if(modulatorsPanelListener->draggingModulator == ModulatorsPanelListener::DraggingModulatorMode::CopyDragging) {
          geomRenderer.drawLine(x+5, y2+t*5, x, y2);
          geomRenderer.drawLine(x-5, y2+t*5, x, y2);
        }
      }
    }

    void update() {
      
      numModulatorsGui->setValue(instrument->numModulators);
      
      for(int i=0; i<maxNumModulators; i++) {
        genericModulatorAmplitudeModulatorOutputIndexGuis[i]->setValue(instrument->modulators[i].outputAM);
        genericModulatorFrequencyModulatorOutputIndexGuis[i]->setValue(instrument->modulators[i].outputFM);
        genericModulatorAmplitudeModulatorInputIndexGuis[i]->setValue(instrument->modulators[i].inputAM);
        genericModulatorFrequencyModulatorInputIndexGuis[i]->setValue(instrument->modulators[i].inputFM);
        genericModulatorAmplitudeEnvelopeIndexGuis[i]->setValue(instrument->modulators[i].amplitudeEnvelope);
        genericModulatorFrequencyEnvelopeIndexGuis[i]->setValue(instrument->modulators[i].frequencyEnvelope);
        
        modulatorOpenGuiGuis[i]->setValue(false);
        
        modulatorLabels[i]->setVisible(i < instrument->numModulators);
        modulatorPreviewPanels[i]->setVisible(i < instrument->numModulators);
        modulatorOpenGuiGuis[i]->setVisible(i < instrument->numModulators);

        genericModulatorAmplitudeModulatorOutputIndexGuis[i]->setVisible(i < instrument->numModulators);
        genericModulatorFrequencyModulatorOutputIndexGuis[i]->setVisible(i < instrument->numModulators);
        genericModulatorAmplitudeModulatorInputIndexGuis[i]->setVisible(i < instrument->numModulators);
        genericModulatorFrequencyModulatorInputIndexGuis[i]->setVisible(i < instrument->numModulators);
        genericModulatorAmplitudeEnvelopeIndexGuis[i]->setVisible(i < instrument->numModulators);
        genericModulatorFrequencyEnvelopeIndexGuis[i]->setVisible(i < instrument->numModulators);
      }
      
      setSize(columnH + width + 5, 10 + lineHeight * (instrument->numModulators+1) + 10);
    }


  };


  struct EnvelopesPanel : public Panel {
    struct EnvelopesPanelListener : public GuiEventListener {
      EnvelopesPanel *envelopesPanel;
      EnvelopesPanelListener(EnvelopesPanel *envelopesPanel) {
        this->envelopesPanel = envelopesPanel;
      }
      
      enum DraggingEnvelopeMode { NotDragging, SwapDragging, CopyDragging };
      DraggingEnvelopeMode draggingEnvelope = DraggingEnvelopeMode::NotDragging;
      
      int envelopeOrderSwapIndexA = 0;
      int envelopeOrderSwapIndexB = 0;
      
      void onMousePressed(GuiElement *guiElement, Events &events) override {
        for(int i=0; i<envelopesPanel->instrument->numEnvelopes; i++) {
          if(envelopesPanel->envelopePreviewPanels[i]->isPointWithin(events.m)) {
            if(events.mouseButton == 1) {
              draggingEnvelope = DraggingEnvelopeMode::SwapDragging;
              envelopeOrderSwapIndexA = i;
              envelopeOrderSwapIndexB = i;
            }
            if(events.mouseButton == 2) {
              draggingEnvelope = DraggingEnvelopeMode::CopyDragging;
              envelopeOrderSwapIndexA = i;
              envelopeOrderSwapIndexB = i;
            }
          }
        }
      }
      void onMouseReleased(GuiElement *guiElement, Events &events) override {
        if(draggingEnvelope == DraggingEnvelopeMode::SwapDragging) {
          envelopesPanel->instrument->stopAllNotesRequested = true;
          
          if(envelopeOrderSwapIndexA != envelopeOrderSwapIndexB) {
            if(envelopesPanel->instrument->envelopes[envelopeOrderSwapIndexA].getPanel()) {
              envelopesPanel->instrument->envelopes[envelopeOrderSwapIndexA].removePanel(envelopesPanel);
            }
            if(envelopesPanel->instrument->envelopes[envelopeOrderSwapIndexB].getPanel()) {
              envelopesPanel->instrument->envelopes[envelopeOrderSwapIndexB].removePanel(envelopesPanel);
            }
            
            GenericEnvelope tmpEnvelope;
            tmpEnvelope = envelopesPanel->instrument->envelopes[envelopeOrderSwapIndexA];
            envelopesPanel->instrument->envelopes[envelopeOrderSwapIndexA] = envelopesPanel->instrument->envelopes[envelopeOrderSwapIndexB];
            envelopesPanel->instrument->envelopes[envelopeOrderSwapIndexB] = tmpEnvelope;
            envelopesPanel->update();
          }
        }
        if(draggingEnvelope == DraggingEnvelopeMode::CopyDragging) {
          envelopesPanel->instrument->stopAllNotesRequested = true;
          
          if(envelopeOrderSwapIndexA != envelopeOrderSwapIndexB) {
            if(envelopesPanel->instrument->envelopes[envelopeOrderSwapIndexB].getPanel()) {
              envelopesPanel->instrument->envelopes[envelopeOrderSwapIndexB].removePanel(envelopesPanel);
            }
            
            GenericEnvelope tmpEnvelope;
            envelopesPanel->instrument->envelopes[envelopeOrderSwapIndexB] = envelopesPanel->instrument->envelopes[envelopeOrderSwapIndexA];
            envelopesPanel->update();
          }
        }
        draggingEnvelope = DraggingEnvelopeMode::NotDragging;
      }
      void onMouseMotion(GuiElement *guiElement, Events &events) override {
        if(draggingEnvelope != DraggingEnvelopeMode::NotDragging) {
          for(int i=0; i<envelopesPanel->instrument->numEnvelopes; i++) {
            if(envelopesPanel->envelopePreviewPanels[i]->isPointWithin(events.m)) {
              envelopeOrderSwapIndexB = i;
            }
          }
        }
      }
      

      void onValueChange(GuiElement *guiElement) {
        if(guiElement == envelopesPanel->numEnvelopesGui) {
          //instrument->currentInitializeID++;
          //instrument->stopAllNotesRequested = true;
          guiElement->getValue((void*)&envelopesPanel->instrument->numEnvelopes);

          for(int i=envelopesPanel->instrument->numEnvelopes; i<maxNumEnvelopes; i++) {
            if(envelopesPanel->instrument->envelopes[i].getPanel()) {
              envelopesPanel->instrument->envelopes[i].removePanel(envelopesPanel);
            }
          }
          envelopesPanel->update();
        }

        for(int i=0; i<envelopesPanel->instrument->numEnvelopes; i++) {
          if(guiElement == envelopesPanel->envelopeOpenGuiGuis[i]) {
            if(envelopesPanel->instrument->envelopes[i].getPanel()) {
              envelopesPanel->instrument->envelopes[i].removePanel(envelopesPanel);
            }
            else {
              envelopesPanel->instrument->envelopes[i].addPanel(envelopesPanel, "Envelope " + std::to_string(i+1));
              envelopesPanel->instrument->envelopes[i].setPreviousPanelPosition();
            }
          }
          
          if(guiElement == envelopesPanel->genericEnvelopeOutputGuis[i]) {
            guiElement->getValue((void*)&envelopesPanel->instrument->envelopes[i].outputIndex);
          }
        }
      }

      void onKeyDown(GuiElement *guiElement, Events &events) {
        if(events.sdlKeyCode == SDLK_HOME) {
          for(int i=0; i<envelopesPanel->instrument->numEnvelopes; i++) {
            if(guiElement == envelopesPanel->genericEnvelopeOutputGuis[i]) {
              if(envelopesPanel->instrument->envelopes[i].getPanel()) {
                envelopesPanel->instrument->envelopes[i].removePanel(envelopesPanel);
              }
              else {
                envelopesPanel->instrument->envelopes[i].addPanel(envelopesPanel, "Envelope " + std::to_string(i+1));
                envelopesPanel->instrument->envelopes[i].setPreviousPanelPosition();
              }
            }
          }
        }
      }
    };
    struct EnvelopePreviewPanel : public Panel {
      int envelopeIndex = 0;
      Instrument *instrument = NULL;
      std::vector<double> envelopeTrail;
      
      double envelopeMaxLength = 3;
      double holdDuration = 1;
      
      EnvelopePreviewPanel(Instrument *instrument, int envelopeIndex) : Panel("Voice preview panel") {
        drawBorder = false;
        drawBackground = false;
        draggable = false;
        this->instrument = instrument;
        this->envelopeIndex = envelopeIndex;
        setSize(70, 23);
        envelopeTrail.resize(70, 0);
      }
            
      virtual void onRender(GeomRenderer &geomRenderer, TextGl &textRenderer) {
        if(!isVisible) return;
        
        GenericEnvelope &envelope = instrument->envelopes[envelopeIndex];
        
        geomRenderer.texture = NULL;
        geomRenderer.strokeType = 1;
        geomRenderer.strokeWidth = 1;        
        geomRenderer.fillColor.set(1, 1, 1, 0.5);
        if(envelopeTrail.size() != (int)size.x) {
          envelopeTrail.assign(size.x, 0);
        }
        
        //double minValue = 0, maxValue = 0;
        for(int x=0; x<size.x; x++) {
          double t = envelopeMaxLength * (double)x/(size.x-1.0);
          envelopeTrail[x] = envelope.getSample(t, holdDuration, t <= holdDuration);
          //minValue = min(minValue, envelopeTrail[x]);
          //maxValue = max(maxValue, envelopeTrail[x]);
        }
        /*if(minValue < maxValue && minValue != -1 && maxValue != 1) {
          for(int x=0; x<size.x; x++) {
            envelopeTrail[x] = map(envelopeTrail[x], minValue, maxValue, -1, 1);
          }
        }*/
        geomRenderer.strokeColor.set(1, 1, 1, 0.5);
        geomRenderer.startRendering(GeomRenderer::RendererType::FastStrokeRenderer);
        //geomRenderer.startRendering();
        for(int x=1; x<size.x; x++) {
          //geomRenderer.drawLine(x-1, size.y*0.36 * (1.0 - envelopeTrail[x-1]), x, size.y*0.36 * (1.0 - envelopeTrail[x]), absolutePos);
          geomRenderer.drawLine(x-1, size.y*0.72 * (1.0 - envelopeTrail[x-1]), x, size.y*0.72 * (1.0 - envelopeTrail[x]), absolutePos);
        }
        geomRenderer.endRendering();
      }
    };
    //Panel *envelopesPanel = NULL;
    NumberBox *numEnvelopesGui = NULL;
    
    std::vector<Button*> envelopeOpenGuiGuis = std::vector<Button*>(maxNumEnvelopes, NULL);
    std::vector<Label*> envelopeLabels = std::vector<Label*>(maxNumEnvelopes, NULL);
    std::vector<EnvelopePreviewPanel*> envelopePreviewPanels = std::vector<EnvelopePreviewPanel*>(maxNumEnvelopes, NULL);

    std::vector<NumberBox*> genericEnvelopeOutputGuis = std::vector<NumberBox*>(maxNumEnvelopes, NULL);

    EnvelopesPanelListener *envelopesPanelListener = NULL;

    Instrument *instrument = NULL;
    GuiElement *parentGuiElement = NULL;

    EnvelopesPanel(Instrument *instrument, GuiElement *parentGuiElement) : Panel("Envelopes panel") {
      init(instrument, parentGuiElement);
    }
    
    double lineHeight = 23;
    int width = 40, widthA = 25, widthB = 100;
    int columnA = 10, columnB = 10+widthA, columnC = 10+widthA+widthB, columnD = 10+widthA+widthB+width, columnE = 10+widthA+widthB+width*2;
    int columnF = 10+widthA+widthB+width*3;
    
    void init(Instrument *instrument, GuiElement *parentGuiElement) {
      this->instrument = instrument;
      this->parentGuiElement = parentGuiElement;

      double line = 10;

      this->addGuiEventListener(envelopesPanelListener = new EnvelopesPanelListener(this));
      parentGuiElement->addChildElement(this);

      numEnvelopesGui = new NumberBox("Envelopes", instrument->numEnvelopes, NumberBox::INTEGER, 0, maxNumEnvelopes, 10, line, 2);
      numEnvelopesGui->incrementMode = NumberBox::IncrementMode::Linear;
      this->addChildElement(numEnvelopesGui);
      numEnvelopesGui->setTextSize(10);
      numEnvelopesGui->labelColor.set(1, 1, 1, 0.9);

      this->addChildElement(new Label("Env→", columnC, line, 10, Vec4d(1, 1, 1, 0.9)));

      //this->setPosition(instrumentPanel->pos + Vec2d(-170 - 10, 0));

      for(int i=0; i<maxNumEnvelopes; i++) {
        line += lineHeight;
        addChildElement(envelopeLabels[i] = new Label(std::to_string(i+1), columnA, line));
        
        addChildElement(envelopePreviewPanels[i] = new EnvelopePreviewPanel(instrument, i));
        envelopePreviewPanels[i]->setPosition(columnB, line+4);

        addChildElement(envelopeOpenGuiGuis[i] = new Button("Open envelope GUI", "data/synth/textures/gui.png", columnC-20, line+2, Button::ToggleButton));
        //envelopeOpenGuiGuis[i]->pressedOverlayColor.set(1, 1, 1, 0.22);
        
        
        genericEnvelopeOutputGuis[i] = new NumberBox("", instrument->envelopes[i].outputIndex, NumberBox::INTEGER, -1, maxNumEnvelopes, columnC, line, 2);
        genericEnvelopeOutputGuis[i]->setValidValueRange(0, 16);
        genericEnvelopeOutputGuis[i]->incrementMode = NumberBox::IncrementMode::Linear;
        //genericEnvelopeOutputGuis[i]->setVisible(i < instrument->numEnvelopes);
        this->addChildElement(genericEnvelopeOutputGuis[i]);
      }

      //setSize(170, 10 + lineHeight * (instrument->numEnvelopes+2) + 10);
      update();
    }

    void onRender(GeomRenderer &geomRenderer, TextGl &textRenderer) override {
      if(envelopesPanelListener->draggingEnvelope != EnvelopesPanelListener::DraggingEnvelopeMode::NotDragging && envelopesPanelListener->envelopeOrderSwapIndexA != envelopesPanelListener->envelopeOrderSwapIndexB) {
        geomRenderer.strokeColor.set(1, 1, 1, 0.75);
        geomRenderer.fillColor.set(1, 1, 1, 0.75);
        geomRenderer.strokeType = 1;
        geomRenderer.strokeWidth = 1;
        double x = absolutePos.x + envelopePreviewPanels[0]->pos.x - 2 - 5;
        double y1 = absolutePos.y + envelopePreviewPanels[envelopesPanelListener->envelopeOrderSwapIndexA]->pos.y + envelopePreviewPanels[0]->size.y * 0.33;
        double y2 = absolutePos.y + envelopePreviewPanels[envelopesPanelListener->envelopeOrderSwapIndexB]->pos.y + envelopePreviewPanels[0]->size.y * 0.33;
        geomRenderer.drawLine(x, y1, x, y2);
        double t = y2 < y1 ? 1 : -1;

        if(envelopesPanelListener->draggingEnvelope == EnvelopesPanelListener::DraggingEnvelopeMode::SwapDragging) {
          geomRenderer.drawLine(x+5, y2+t*5, x, y2);
          geomRenderer.drawLine(x-5, y2+t*5, x, y2);
          
          geomRenderer.drawLine(x+5, y1-t*5, x, y1);
          geomRenderer.drawLine(x-5, y1-t*5, x, y1);
        }
        if(envelopesPanelListener->draggingEnvelope == EnvelopesPanelListener::DraggingEnvelopeMode::CopyDragging) {
          geomRenderer.drawLine(x+5, y2+t*5, x, y2);
          geomRenderer.drawLine(x-5, y2+t*5, x, y2);
        }
      }
    }

    void update() {
      numEnvelopesGui->setValue(instrument->numEnvelopes);
      
      for(int i=0; i<maxNumEnvelopes; i++) {
        envelopeLabels[i]->setVisible(i < instrument->numEnvelopes);
        envelopePreviewPanels[i]->setVisible(i < instrument->numEnvelopes);
        envelopeOpenGuiGuis[i]->setVisible(i < instrument->numEnvelopes);
        
        envelopeOpenGuiGuis[i]->setValue(false);
        
        genericEnvelopeOutputGuis[i]->setValue(instrument->envelopes[i].outputIndex);
        genericEnvelopeOutputGuis[i]->setVisible(i < instrument->numEnvelopes);
      }

      setSize(columnD + 5, 10 + lineHeight * (instrument->numEnvelopes+1) + 10);
    }


  };

  struct InstrumentPanel : public Panel {  
    //Panel *instrumentPanel = NULL;
    
    struct InstrumentTitleBar : public SynthTitleBarExt {
      //titleBar->addSaveButton(instrument);
      MenuButton *showVoicesMenuButton = NULL, *showModulatorsMenuButton = NULL, *showEnvelopesMenuButton = NULL;
      MenuButton *showModularsMenuButton = NULL, *showCrossModulationMenuButton = NULL, *showNoteFilterMenuButton = NULL;
      Instrument *instrument = NULL;
      InstrumentPanel *instrumentPanel = NULL;
      
      InstrumentTitleBar(InstrumentPanel *instrumentPanel, Instrument *instrument) : SynthTitleBarExt("Instrument", instrumentPanel) {
        init(instrumentPanel, instrument);
      }
      
      std::string getHide(Panel *panel) {
        return panel && panel->isVisible ? "Hide " : "Show ";
      }
      
      void init(InstrumentPanel *instrumentPanel, Instrument *instrument) {
        this->instrument = instrument;
        this->instrumentPanel = instrumentPanel;
        
        addMenu();
        //titleBar->menu->fileLoader->addGuiEventListener(new FileLoaderListener(instrument));
        
        //menu->addLayoutMenuItem();
        
        //menu->addMenuDivider();
        
        menu->addSaveMenuItem(instrument);
        menu->addLoadMenuItem(instrument);

        menu->fileLoaderListener = new FileLoaderListener(instrumentPanel); 
        
        menu->addMenuDivider();
        
        showVoicesMenuButton = menu->addMenuButton(getHide(instrumentPanel->voicesPanel) + "voices");
        showEnvelopesMenuButton = menu->addMenuButton(getHide(instrumentPanel->envelopesPanel) + "envelopes");
        showModulatorsMenuButton = menu->addMenuButton(getHide(instrumentPanel->modulatorsPanel) + "modulators");
        showCrossModulationMenuButton = menu->addMenuButton(getHide(instrumentPanel->voiceCrossModulationPanel) + "cross modulation");
        showModularsMenuButton = menu->addMenuButton(getHide(instrumentPanel->modularParameterPanel) + "modulars");
        showNoteFilterMenuButton = menu->addMenuButton(getHide(instrument->biquadFilter->getPanel()) + "note filter");
        
        addGuiEventListener(new InstrumentTitleBarListener(this));
      }
      
      void update() {
        showVoicesMenuButton->setText(getHide(instrumentPanel->voicesPanel) + "voices");
        showEnvelopesMenuButton->setText(getHide(instrumentPanel->envelopesPanel) + "envelopes");
        showModulatorsMenuButton->setText(getHide(instrumentPanel->modulatorsPanel) + "modulators");
        showCrossModulationMenuButton->setText(getHide(instrumentPanel->voiceCrossModulationPanel) + "cross modulation");
        showModularsMenuButton->setText(getHide(instrumentPanel->modularParameterPanel) + "modulars");
        showNoteFilterMenuButton->setText(getHide(instrument->biquadFilter->getPanel()) + "note filter");
      }
      
      struct InstrumentTitleBarListener : public GuiEventListener {
        InstrumentTitleBar *instrumentTitleBar;
        InstrumentTitleBarListener(InstrumentTitleBar *instrumentTitleBar) : instrumentTitleBar(instrumentTitleBar) {}
        
        virtual void onAction(GuiElement *guiElement) override {
          if(guiElement == instrumentTitleBar->showCrossModulationMenuButton) {
            if(instrumentTitleBar->instrumentPanel->voiceCrossModulationPanel) {
              instrumentTitleBar->instrumentPanel->voiceCrossModulationPanel->toggleVisibility();
              instrumentTitleBar->showCrossModulationMenuButton->setText(instrumentTitleBar->getHide(instrumentTitleBar->instrumentPanel->voiceCrossModulationPanel) + "cross modulation");
            }
          }
          if(guiElement == instrumentTitleBar->showModularsMenuButton) {
            if(instrumentTitleBar->instrumentPanel->modularParameterPanel) {
              instrumentTitleBar->instrumentPanel->modularParameterPanel->toggleVisibility();
              instrumentTitleBar->showModularsMenuButton->setText(instrumentTitleBar->getHide(instrumentTitleBar->instrumentPanel->modularParameterPanel) + "modulars");
            }
          }
          if(guiElement == instrumentTitleBar->showNoteFilterMenuButton) {
            if(instrumentTitleBar->instrument->biquadFilter->getPanel()) {
              instrumentTitleBar->instrument->biquadFilter->getPanel()->toggleVisibility();
              instrumentTitleBar->showNoteFilterMenuButton->setText(instrumentTitleBar->getHide(instrumentTitleBar->instrument->biquadFilter->getPanel()) + "note filter");
            }
          }
          if(guiElement == instrumentTitleBar->showVoicesMenuButton) {
            if(instrumentTitleBar->instrumentPanel->voicesPanel) {
              instrumentTitleBar->instrumentPanel->voicesPanel->toggleVisibility();
              instrumentTitleBar->showVoicesMenuButton->setText(instrumentTitleBar->getHide(instrumentTitleBar->instrumentPanel->voicesPanel) + "voices");
            }
          }
          if(guiElement == instrumentTitleBar->showEnvelopesMenuButton) {
            if(instrumentTitleBar->instrumentPanel->envelopesPanel) {
              instrumentTitleBar->instrumentPanel->envelopesPanel->toggleVisibility();
              instrumentTitleBar->showEnvelopesMenuButton->setText(instrumentTitleBar->getHide(instrumentTitleBar->instrumentPanel->envelopesPanel) + "envelopes");
            }
          }
          if(guiElement == instrumentTitleBar->showModulatorsMenuButton) {
            if(instrumentTitleBar->instrumentPanel->modulatorsPanel) {
              instrumentTitleBar->instrumentPanel->modulatorsPanel->toggleVisibility();
              instrumentTitleBar->showModulatorsMenuButton->setText(instrumentTitleBar->getHide(instrumentTitleBar->instrumentPanel->modulatorsPanel) + "modulators");
            }
          }
        }
      };
    };
    
    struct PreviewPanel : public Panel {
      struct PreviewPanelListener : public GuiEventListener {
        PreviewPanel *previewPanel = NULL;
        PreviewPanelListener(PreviewPanel *previewPanel) : previewPanel(previewPanel) {}
        //virtual onKeyDown(GuiElement *guiElement, Events &events) override {
        bool draggingIntensity = false;
        int draggingGradient = 0;
        
        virtual void onMousePressed(GuiElement *guiElement, Events &events) override {
          if(previewPanel->isPointWithin(events.m)) {
            //if(events.sdlKeyCode == SDLK_v) {
            if(events.mouseNowDownM) {
              previewPanel->spectrumMap.volumeUnit = (previewPanel->spectrumMap.volumeUnit+1) % 4;
            }
            else if(events.mouseNowDownR) {
              if(events.lControlDown && !events.lShiftDown) draggingGradient = 1;
              if(!events.lControlDown && events.lShiftDown) draggingGradient = 2;
              if(events.lControlDown && events.lShiftDown) draggingGradient = 3;
              if(draggingGradient > 0) previewPanel->captureAndHideMouse();
            }
            else if(events.mouseNowDownR && events.numModifiersDown == 0) {
              previewPanel->captureAndHideMouse();
              draggingIntensity = true;
            }
            previewPanel->isInputGrabbed = true;
          }
        }
        virtual void onMouseMotion(GuiElement *guiElement, Events &events) override {
          if(draggingGradient == 1) {
            previewPanel->spectrumMap.gradientParameters.x += 0.01*events.md.x;
            previewPanel->spectrumMap.gradientParameters.x = modff(previewPanel->spectrumMap.gradientParameters.x, -2*PI, 2*PI);
          }
          else if(draggingGradient == 2) {
            previewPanel->spectrumMap.gradientParameters.y += 0.01*events.md.x;
            previewPanel->spectrumMap.gradientParameters.y = modff(previewPanel->spectrumMap.gradientParameters.y, -2*PI, 2*PI);
          }
          else if(draggingGradient == 3) {
            previewPanel->spectrumMap.gradientParameters.z += 0.01*events.md.x;
            previewPanel->spectrumMap.gradientParameters.z = clamp(previewPanel->spectrumMap.gradientParameters.z, 0, 1);
          }

          if(draggingIntensity) {
            if(events.md.x > 0) {
              previewPanel->spectrumMap.spectrumMapScaling *= pow(1.01, events.md.x);
            }
            if(events.md.x < 0) {
              previewPanel->spectrumMap.spectrumMapScaling /= pow(1.01, -events.md.x);
            }
          }
        }
        virtual void onMouseReleased(GuiElement *guiElement, Events &events) override {
          draggingIntensity = false;
          previewPanel->isInputGrabbed = false;
          draggingGradient = 0;
          previewPanel->releaseMouse();
        }
      };
      
      SpectrumMapFixed spectrumMap;
      Instrument *instrument = NULL;
      std::thread updateThread;
      bool updating = false;
      int updateIntervalMillis = 160;
      double spectrumLenghtSecs = 2;
      std::vector<double> wave;//(previewPanel->spectrumLenghtSecs * previewPanel->instrument->delayLine.sampleRate);
      SynthesisNote previewNote;
      
      PreviewPanelListener *previewPanelListener = NULL;
      
      virtual ~PreviewPanel() {
        stopUpdate();
      }
      
      PreviewPanel(Instrument *instrument, int width, int height) : Panel("Instrument preview panel") {
        setSize(width, height);
        this->instrument = instrument;
        
        //wave.resize(spectrumLenghtSecs * instrument->delayLine.sampleRate);
        wave.resize(spectrumLenghtSecs * instrument->delayLine.sampleRate);
        instrument->preparePreviewNote(previewNote);
        
        spectrumMap.init(instrument->delayLine.sampleRate, max(1, width - 2), max(1, height - 2), false);
        //spectrumMap.init(instrument->delayLine.sampleRate, width, height, false);
        spectrumMap.spectrumLimits = Vec2d(1, 10000);
        spectrumMap.spectrumAsNotes = false;
        spectrumMap.update(wave);

        //startUpdate(250);
        
        //drawBorder = true;
        drawBackground = false;
        //draggable = false;
        
        //drawShadow = true;
        
        addGuiEventListener(previewPanelListener = new PreviewPanelListener(this));
      }

      
      static void spectrumUpdateThread(PreviewPanel *previewPanel) {
        printf("spectrumUpdateThread started!\n");
        long numSamples = previewPanel->wave.size();
        double dt = (double)previewPanel->spectrumLenghtSecs / previewPanel->wave.size();
        
        while(previewPanel->updating) {
          previewPanel->instrument->threadSafePreparedPreviewNoteReset(previewPanel->previewNote);
          
          double minValue = 0, maxValue = 0;
          for(long i=0; i<numSamples; i++) {
            Vec2d sampleOut;
            previewPanel->instrument->getSample(sampleOut, Vec2d(), dt, dt*i, previewPanel->previewNote);
            previewPanel->wave[i] = sampleOut.x; // or left + right ?
            //minValue = min(previewPanel->wave[i], minValue);
            //maxValue = max(previewPanel->wave[i], maxValue);
          }
          /*if(maxValue > minValue) {
            for(long i=0; i<numSamples; i++) {
              previewPanel->wave[i] = map(previewPanel->wave[i], minValue, maxValue, -1, 1);
            }
          }*/

          previewPanel->spectrumMap.update(previewPanel->wave);
          
          //std::this_thread::sleep_for(std::chrono::milliseconds(previewPanel->updateIntervalMillis));
        }
        printf("spectrumUpdateThread ended!\n");
      }
      
      void startUpdate() {
        if(updating) {
          updating = false;
          updateThread.join();
        }
        updating = true;
        updateThread = std::thread(spectrumUpdateThread, this);
      }
      void stopUpdate() {
        if(updating) {
          updating = false;
          updateThread.join();
        }
      }
      std::vector<Vec2d> envelopeBuffer;
      
      virtual void onRender(GeomRenderer &geomRenderer, TextGl &textRenderer) override {
        //spectrumUpdateThread(this);
        //spectrumMap.refreshTexture();
        
        /*if(envelopeBuffer.size() != size.x-2) {
          envelopeBuffer.resize(size.x-2);
        }
        //int t = (int)(dt * instrument->delayLine.sampleRate);
        double maxValTotal = -1e10, minValTotal = 1e10;
        long n = wave.size()/spectrumLenghtSecs * 0.1;
        for(int i=0; i<envelopeBuffer.size(); i++) {
          int a = map(i, 0, envelopeBuffer.size(), 0, n-1);
          int b = map(i+1, 0, envelopeBuffer.size(), 0, n-1);
          double maxVal = -1e10, minVal = 1e10;
          for(int k=a; k<b; k++) {
             double v = wave[k];
             if(v > maxVal) maxVal = v;
             if(v < minVal) minVal = v;
          }
          envelopeBuffer[i].set(minVal, maxVal);
          if(maxVal > maxValTotal) maxValTotal = maxVal;
          if(minVal < minValTotal) minValTotal = minVal;
        }
        for(int i=0; i<envelopeBuffer.size(); i++) {
          envelopeBuffer[i].x = map(envelopeBuffer[i].x, minValTotal, maxValTotal, -1, 1);
          envelopeBuffer[i].y = map(envelopeBuffer[i].y, minValTotal, maxValTotal, -1, 1);
        }
        geomRenderer.strokeColor.set(1, 1, 1, 0.6);
        
        for(int i=0; i<envelopeBuffer.size(); i++) {
          Vec2d p = envelopeBuffer[i];
          geomRenderer.drawLine(i+1, 10 - p.x*5, i, 10 - p.y*5, absolutePos);
        }*/
        /*geomRenderer.strokeColor.set(1, 1, 1, 0.6);
        
        for(int i=0; i<size.x; i++) {
          int ind = i*wave.size()/size.x;
          geomRenderer.drawLine(i+1, 10 - wave[ind]*5, i, 10 - wave[ind]*5, absolutePos);
        }*/
        
        geomRenderer.texture = NULL;
        geomRenderer.fillColor.set(0, 0, 0, 0);
        geomRenderer.strokeColor.set(1, 1, 1, 0.16);
        geomRenderer.strokeType = 1;
        geomRenderer.strokeWidth = 1;
        geomRenderer.drawRect(size, absolutePos + size*0.5);

        spectrumMap.render(absolutePos.x + size.x*0.5, absolutePos.y + size.y*0.5, Vec4d(1, 1, 1, 0.93));
        
        
        textRenderer.setColor(1, 1, 1, 1);
        if(previewPanelListener->draggingIntensity) {
          textRenderer.print(format("Intensity %.2f", spectrumMap.spectrumMapScaling), absolutePos.x + 16, absolutePos.y + 6, 10);
        }
        else if(previewPanelListener->draggingGradient == 1) {
          textRenderer.print(format("Gradient hue %.2f", spectrumMap.gradientParameters.x/(2.0*PI)*360.0), absolutePos.x + 16, absolutePos.y + 6, 10);
        }
        else if(previewPanelListener->draggingGradient == 2) {
          textRenderer.print(format("Gradient hue range  %.2f", spectrumMap.gradientParameters.y/(2.0*PI)*360.0), absolutePos.x + 16, absolutePos.y + 6, 10);
        }
        else if(previewPanelListener->draggingGradient == 3) {
          textRenderer.print(format("Gradient saturation %.2f", spectrumMap.gradientParameters.z*100.0), absolutePos.x + 16, absolutePos.y + 6, 10);
        }
      }
      
      
      virtual void onVisibilityChanged(bool toVisible) override {
        if(toVisible) {
          startUpdate();
        }
        else {
          stopUpdate();
        }
      }


    };
    
    InstrumentTitleBar *titleBar = NULL;
    
    TextBox *instrumentNameGui = NULL;
    //ListBox *instrumentEffectsGui = NULL;
    
    PreviewPanel *previewPanel = NULL;
    
    /*CheckBox *showModularMatrixGui = NULL;
    CheckBox *showVoiceCrossModulationMatrixGui = NULL;
    CheckBox *showBiquadFilterGui = NULL;*/
    
    EnvelopesPanel *envelopesPanel = NULL;
    ModulatorsPanel *modulatorsPanel = NULL;
    VoicesPanel *voicesPanel = NULL;
    
    ModularParameterPanel *modularParameterPanel = NULL;
    VoiceCrossModulation::VoiceCrossModulationPanel *voiceCrossModulationPanel = NULL;
    
    EffectTrackPanel *effectTrackPanel = NULL;
    
    Instrument *instrument = NULL;
    GuiElement *parentGuiElement = NULL;

    RecordedNotesPanel *recordedNotesPanel = NULL;
    
    InstrumentPanel(Instrument *instrument, GuiElement *parentGuiElement) : Panel("Instrument panel") {
      init(instrument, parentGuiElement);
    }

    void startUpdate() {
    	if(previewPanel) {
    	  previewPanel->startUpdate();
    	}
    }
    void stopUpdate() {
      if(previewPanel) {
        previewPanel->stopUpdate();
      }
    }
    
    // FIXME should call children automatically
    virtual void onVisibilityChanged(bool toVisible) override {
      if(toVisible && previewPanel) {
        previewPanel->onVisibilityChanged(toVisible);
      }
    }

    void init(Instrument *instrument = NULL, GuiElement *parentGuiElement = NULL) {

      this->instrument = instrument;
      this->parentGuiElement = parentGuiElement;
      setSize(300, 50);
      
      setPosition(10, 75);
      
      double line = 0, lineHeight = 23;

      titleBar = new InstrumentTitleBar(this, instrument);
      
      line += titleBar->size.y + 5;

      this->addGuiEventListener(new InstrumentPanelListener(this));
      parentGuiElement->addChildElement(this);

      instrumentNameGui = new TextBox("Name", instrument->name, 12, line);
      this->addChildElement(instrumentNameGui);
      line += lineHeight;
      /*instrumentEffectsGui = new ListBox("Effects/filters", 10, line+=lineHeight, 12);
      this->addChildElement(instrumentEffectsGui);
      line+=lineHeight;*/
      /*addChildElement(previewPanel = new PreviewPanel(instrument, size.x-20, 90));
      previewPanel->setPosition(10, line+=6);
      line += previewPanel->size.y;*/
      
      /*addChildElement(showModularMatrixGui = new CheckBox("Modulars", false, 10, line+=lineHeight));
      addChildElement(showVoiceCrossModulationMatrixGui = new CheckBox("Voice cross modulation", false, 10+107, line));
      addChildElement(showBiquadFilterGui = new CheckBox("Note filter", instrument->biquadFilter->isActive, 10, line+=lineHeight));*/
      setSize(300, line + 7);
      
      //addChildElement(previewPanel = new PreviewPanel(instrument, 300, 90));
      //previewPanel->setPosition(0, size.y + 5);

      voicesPanel = new VoicesPanel(this->instrument, this);
      voicesPanel->setPosition(0, this->size.y + 5);
      //voicesPanel->setPosition(0, previewPanel->pos.y + previewPanel->size.y + 5);
      
      envelopesPanel = new EnvelopesPanel(this->instrument, this);
      envelopesPanel->setPosition(this->size.x + 5, 0);
      
      modulatorsPanel = new ModulatorsPanel(this->instrument, this);
      modulatorsPanel->setPosition(0, voicesPanel->pos.y + voicesPanel->size.y + 23 + 5);
      //modulatorsPanel->setVisible(false);

      addChildElement(modularParameterPanel = new ModularParameterPanel(this->instrument));
      modularParameterPanel->setVisible(false);
      modularParameterPanel->setPosition(-modularParameterPanel->size.x - 5, 0);
      voicesPanel->addGuiEventListener(new ModularParameterPanel::PanelUpdateListener(modularParameterPanel));
      //modulatorsPanel->addGuiEventListener(new ModularParameterPanel::PanelUpdateListener(modularParameterPanel));

      addChildElement(voiceCrossModulationPanel = this->instrument->voiceCrossModulation.getPanel());
      voiceCrossModulationPanel->setVisible(false);
      voiceCrossModulationPanel->setPosition(0, -voiceCrossModulationPanel->size.y-5);
      voicesPanel->addGuiEventListener(new VoiceCrossModulation::VoiceCrossModulationPanel::PanelUpdateListener(voiceCrossModulationPanel));

      instrument->biquadFilter->addPanel(this);
      instrument->biquadFilter->getPanel()->setVisible(this->instrument->biquadFilter->isActive);
      instrument->biquadFilter->getPanel()->setPosition(voicesPanel->size.x + 5, envelopesPanel->size.y + 23 + 5);

      //addChildElement(recordedNotesPanel = new RecordedNotesPanel(instrument));
      //recordedNotesPanel->setPosition(0, 300);

      effectTrackPanel = new EffectTrackPanel(&this->instrument->postProcessingEffects, &instrument->delayLine, this);
      effectTrackPanel->setPosition(envelopesPanel->pos.x + envelopesPanel->size.x + 5, 0);
      effectTrackPanel->setVisible(true);

      update();

      //updatePostPocessingPanels();

    }

    /*void updatePostPocessingPanels() {
      if(instrument->postProcessingEffects.size() > 0) {
        for(int i=0; i<instrument->postProcessingEffects.size(); i++) {
          if(i != instrument->activePostProcessingEffect && instrument->postProcessingEffects[i]->getPanel()) {
            instrument->postProcessingEffects[i]->removePanel(this);
          }
        }
      }
    }*/

    void update(bool reinit = false) {

      instrumentNameGui->setValue(instrument->name);
      //showModularMatrixGui->setValue(modularParameterPanel && modularParameterPanel->isVisible);
      //showVoiceCrossModulationMatrixGui->setValue(voiceCrossModulationPanel && voiceCrossModulationPanel->isVisible);
      //showBiquadFilterGui->setValue(instrument->biquadFilter->getPanel() && instrument->biquadFilter->getPanel()->isVisible);
      /*instrumentEffectsGui->clearItems();
      if(instrument->postProcessingEffects.size() == 0) {
        instrumentEffectsGui->addItems("No effects");
      }
      else {
        for(int i=0; i<instrument->postProcessingEffects.size(); i++) {
          instrumentEffectsGui->addItems(instrument->postProcessingEffects[i]->name + " " +
                                         std::to_string(i+1) + "/" + std::to_string(instrument->postProcessingEffects.size()));
        }
        instrumentEffectsGui->setValue(instrument->activePostProcessingEffect);
      }*/
      voicesPanel->update();
      modulatorsPanel->update();
      envelopesPanel->update();
      modularParameterPanel->update();
      voiceCrossModulationPanel->update();
      
      if(!this->instrument->biquadFilter->getPanel()) {
        this->instrument->biquadFilter->addPanel(this);

        this->instrument->biquadFilter->getPanel()->setVisible(false);
      }
      
      if(reinit) {
        for(int i=0; i<instrument->voices.size(); i++) {
          if(instrument->voices[i].getPanel()) {
            instrument->voices[i].removePanel(voicesPanel);
          }
        }
        for(int i=0; i<instrument->modulators.size(); i++) {
          if(instrument->modulators[i].getPanel()) {
            instrument->modulators[i].removePanel(modulatorsPanel);
          }
        }
        for(int i=0; i<instrument->envelopes.size(); i++) {
          if(instrument->envelopes[i].getPanel()) {
            instrument->envelopes[i].removePanel(envelopesPanel);
          }
        }
        for(int i=0; i<instrument->postProcessingEffects.size(); i++) {
          if(instrument->postProcessingEffects[i]->getPanel()) {
            instrument->postProcessingEffects[i]->removePanel(this);
          }
        }
      }
      titleBar->update();
    }
    
    struct FileLoaderListener : public GuiEventListener {
      InstrumentPanel *instrumentPanel;
      FileLoaderListener(InstrumentPanel *instrumentPanel) {
        this->instrumentPanel = instrumentPanel;
      }
      virtual void onMessage(GuiElement *guiElement, const std::string &message) {
        if(message == "fileLoaded") {
          instrumentPanel->update(true);
        }
      }
    };


    struct InstrumentPanelListener : public GuiEventListener {
      InstrumentPanel *instrumentPanel;
      InstrumentPanelListener(InstrumentPanel *instrumentPanel) {
        this->instrumentPanel = instrumentPanel;
      }
      
      void onValueChange(GuiElement *guiElement) {
        if(guiElement == instrumentPanel->instrumentNameGui) {
          guiElement->getValue((void*)&instrumentPanel->instrument->name);
          instrumentPanel->getRoot()->sendMessage("instrumentNameChanged", instrumentPanel->instrument);
        }
        /*if(guiElement == instrumentPanel->instrumentEffectsGui) {
          guiElement->getValue((void*)&instrumentPanel->instrument->activePostProcessingEffect);
          //printf("1... %d, %lu\n", instrument->activePostProcessingEffect, instrument->postProcessingEffects.size());
          instrumentPanel->update();
          //printf("2... %d, %lu\n", instrument->activePostProcessingEffect, instrument->postProcessingEffects.size());
        }*/
        /*if(guiElement == instrumentPanel->showModularMatrixGui) {
          bool visible = false;
          guiElement->getValue((void*)&visible);
          instrumentPanel->modularParameterPanel->setVisible(visible);
        }
        if(guiElement == instrumentPanel->showVoiceCrossModulationMatrixGui) {
          bool visible = false;
          guiElement->getValue((void*)&visible);
          instrumentPanel->voiceCrossModulationPanel->setVisible(visible);
        }
        if(guiElement == instrumentPanel->showBiquadFilterGui) {
          bool value;
          guiElement->getValue((void*)&value);
          instrumentPanel->instrument->biquadFilter->updatePanel();
          instrumentPanel->instrument->biquadFilter->getPanel()->setVisible(value);
        }*/
      }


      /*void onKeyDown(GuiElement *guiElement, Events &events) {
        if(guiElement == instrumentPanel->instrumentEffectsGui) {
          bool changed = false;
          // FIXME clean
          if(events.sdlKeyCode == SDLK_1) {
            instrumentPanel->instrument->currentInitializeID++;
            instrumentPanel->instrument->stopAllNotesRequested = true;
            MultiDelay *multiDelay = new MultiDelay(&instrumentPanel->instrument->delayLine);
            //multiDelay->isActive = false;
            instrumentPanel->instrument->postProcessingEffects.push_back(multiDelay);
            instrumentPanel->instrument->activePostProcessingEffect = instrumentPanel->instrument->postProcessingEffects.size() - 1;
            changed = true;
          }
          if(events.sdlKeyCode == SDLK_2) {
            instrumentPanel->instrument->currentInitializeID++;
            instrumentPanel->instrument->stopAllNotesRequested = true;
            BiquadFilter *biquadFilter = new BiquadFilter(&instrumentPanel->instrument->delayLine);
            //biquadFilter->isActive = false;
            instrumentPanel->instrument->postProcessingEffects.push_back(biquadFilter);
            instrumentPanel->instrument->activePostProcessingEffect = instrumentPanel->instrument->postProcessingEffects.size() - 1;
            changed = true;
          }
          if(events.sdlKeyCode == SDLK_3) {
            instrumentPanel->instrument->currentInitializeID++;
            instrumentPanel->instrument->stopAllNotesRequested = true;
            Equalizer *equalizer = new Equalizer(instrumentPanel->instrument->delayLine.sampleRate);
            //equalizer->isActive = false;
            instrumentPanel->instrument->postProcessingEffects.push_back(equalizer);
            instrumentPanel->instrument->activePostProcessingEffect = instrumentPanel->instrument->postProcessingEffects.size() - 1;
            changed = true;
          }
          if(events.sdlKeyCode == SDLK_4) {
            instrumentPanel->instrument->currentInitializeID++;
            instrumentPanel->instrument->stopAllNotesRequested = true;
            Reverb *reverb = new Reverb(instrumentPanel->instrument->delayLine.sampleRate);
            //reverb->isActive = false;
            instrumentPanel->instrument->postProcessingEffects.push_back(reverb);
            instrumentPanel->instrument->activePostProcessingEffect = instrumentPanel->instrument->postProcessingEffects.size() - 1;
            changed = true;
          }
          if(events.sdlKeyCode == SDLK_HOME && instrumentPanel->instrument->activePostProcessingEffect < instrumentPanel->instrument->postProcessingEffects.size()) {
            if(instrumentPanel->instrument->postProcessingEffects[instrumentPanel->instrument->activePostProcessingEffect]->getPanel()) {
              instrumentPanel->instrument->postProcessingEffects[instrumentPanel->instrument->activePostProcessingEffect]->removePanel(instrumentPanel);
            }
            else {
              instrumentPanel->instrument->postProcessingEffects[instrumentPanel->instrument->activePostProcessingEffect]->addPanel(instrumentPanel);
              if(!instrumentPanel->instrument->postProcessingEffects[instrumentPanel->instrument->activePostProcessingEffect]->setPreviousPanelPosition()) {
                //panel->pos += Vec2d(60, 60);
              }
            }
          }

          if(events.sdlKeyCode == SDLK_DELETE) {
            printf("(debugging) at InstrumentPanelListener::onKeyDown(): 7...\n");
            instrumentPanel->instrument->currentInitializeID++;
            instrumentPanel->instrument->stopAllNotesRequested = true;
            //int index;
            //instrument->instrumentEffectsGui->getValue((void*)&index);
            if(instrumentPanel->instrument->activePostProcessingEffect < instrumentPanel->instrument->postProcessingEffects.size()) {

              if(instrumentPanel->instrument->postProcessingEffects[instrumentPanel->instrument->activePostProcessingEffect]->getPanel()) {
                instrumentPanel->instrument->postProcessingEffects[instrumentPanel->instrument->activePostProcessingEffect]->removePanel(instrumentPanel);
              }
              printf("(debugging) at InstrumentPanelListener::onKeyDown(): 8...\n");

              delete instrumentPanel->instrument->postProcessingEffects[instrumentPanel->instrument->activePostProcessingEffect];
              instrumentPanel->instrument->postProcessingEffects.erase(instrumentPanel->instrument->postProcessingEffects.begin() + instrumentPanel->instrument->activePostProcessingEffect);

              instrumentPanel->instrument->activePostProcessingEffect = max(0, instrumentPanel->instrument->activePostProcessingEffect-1);
              printf("(debugging) at InstrumentPanelListener::onKeyDown(): 9...\n");
            }

            changed = true;
          }
          if(changed) {
            printf("(debugging) at InstrumentPanelListener::onKeyDown(): 10...\n");
            instrumentPanel->update();
            instrumentPanel->instrumentEffectsGui->isInputGrabbed = true;
          }
          printf("(debugging) at InstrumentPanelListener::onKeyDown(): 11...\n");
        }
      }*/
    };
  };

  InstrumentPanel *instrumentPanel = NULL;
  
  Panel *getPanel() {
    return instrumentPanel;
  }
  
  static std::string getClassName() {
    return "instrument";
  }
  
  virtual std::string getBlockName() {
    return getClassName();
  }
  
  virtual void encodeParameters() {
    clearParameters();
    putNumericParameter("volume", volume);
    putStringParameter("name", name);
    
    for(int i=0; i<postProcessingEffects.size(); i++) {
      postProcessingEffects[i]->encodeParameters();
    }
    for(int i=0; i<numVoices; i++) {
      voices[i].encodeParameters();
    }
    for(int i=0; i<numModulators; i++) {
      modulators[i].encodeParameters();
    }
    for(int i=0; i<numEnvelopes; i++) {
      envelopes[i].encodeParameters(); 
    }
    if(voiceCrossModulation.mode != VoiceCrossModulationMode::CrossModulatationNone) {
      voiceCrossModulation.encodeParameters();
    }
    if(filterFrequencyModular.isConnected) {
      putNumericParameter("noteFilterFrequencyModular", {(double)filterFrequencyModular.modulator, (double)filterFrequencyModular.envelope, filterFrequencyModular.range.x, filterFrequencyModular.range.y});
    }
    if(filterBandwidthModular.isConnected) {
      putNumericParameter("noteFilterBandwidthModular", {(double)filterBandwidthModular.modulator, (double)filterBandwidthModular.envelope, filterBandwidthModular.range.x, filterBandwidthModular.range.y});
    }
    if(filterWetLevelModular.isConnected) {
      putNumericParameter("noteFilterWetLevelModular", {(double)filterWetLevelModular.modulator, (double)filterWetLevelModular.envelope, filterWetLevelModular.range.x, filterWetLevelModular.range.y});
    }
    
    if(biquadFilter->isActive) {
      biquadFilter->encodeParameters();
    }
  }

  
  virtual void decodeParameters() {
    getNumericParameter("volume", volume);
    getStringParameter("name", name);
    
    getNumericParameter("noteFilterFrequencyModular", filterFrequencyModular.modulator, 0);
    getNumericParameter("noteFilterFrequencyModular", filterFrequencyModular.envelope, 1);
    getNumericParameter("noteFilterFrequencyModular", filterFrequencyModular.range.x, 2);
    getNumericParameter("noteFilterFrequencyModular", filterFrequencyModular.range.y, 3);
    //filterFrequencyModular.checkConnected();
    
    getNumericParameter("noteFilterBandwidthModular", filterBandwidthModular.modulator, 0);
    getNumericParameter("noteFilterBandwidthModular", filterBandwidthModular.envelope, 1);
    getNumericParameter("noteFilterBandwidthModular", filterBandwidthModular.range.x, 2);
    getNumericParameter("noteFilterBandwidthModular", filterBandwidthModular.range.y, 3);
    //filterBandwidthModular.checkConnected();
    
    getNumericParameter("noteFilterWetLevelModular", filterWetLevelModular.modulator, 0);
    getNumericParameter("noteFilterWetLevelModular", filterWetLevelModular.envelope, 1);
    getNumericParameter("noteFilterWetLevelModular", filterWetLevelModular.range.x, 2);
    getNumericParameter("noteFilterWetLevelModular", filterWetLevelModular.range.y, 3);
    //filterWetLevelModular.checkConnected();
    
    checkModularConnections();
    /*for(int i=0; i<numVoices; i++) {
      voices[i].waveForm.readyToPrepareWaveTable = true;
      voices[i].waveForm.prepareWaveTable();
    }
    for(int i=0; i<numModulators; i++) {
      modulators[i].waveForm.readyToPrepareWaveTable = true;
      modulators[i].waveForm.prepareWaveTable();
    }*/
    if(instrumentPanel) {
      instrumentPanel->getRoot()->sendMessage("instrumentNameChanged", this);
    }
    isActive = previousActive;
    
    if(instrumentPanel && instrumentPanel->isVisible) {
      instrumentPanel->startUpdate();
    }
    if(instrumentPanel) {
      instrumentPanel->effectTrackPanel->onItemsChanged();
    }
  }

  virtual void onPrintParameters(std::string &content, int level = 0) {
    for(int i=0; i<postProcessingEffects.size(); i++) {
      postProcessingEffects[i]->printParameters(content, level);
    }
    for(int i=0; i<numVoices; i++) {
      voices[i].printParameters(content, level);
    }
    for(int i=0; i<numModulators; i++) {
      modulators[i].printParameters(content, level);
    }
    for(int i=0; i<numEnvelopes; i++) {
      envelopes[i].printParameters(content, level); 
    }
    if(biquadFilter->isActive) {
      biquadFilter->printParameters(content, level); 
    }
    if(voiceCrossModulation.mode != VoiceCrossModulationMode::CrossModulatationNone) {
      voiceCrossModulation.printParameters(content, level); 
    }
  }
  
  bool previousActive = false;
  virtual void onParse() {
    
    if(instrumentPanel) {
      instrumentPanel->stopUpdate();
    }
    while(isSynthesisActive) {}
    
    previousActive = isActive;
    isActive = false;
    reset();
    reinitialize();
    /*for(int i=0; i<maxNumVoices; i++) {
      voices[i].waveForm.readyToPrepareWaveTable = false;
      //voices[i].waveForm.prepareWaveTable();
    }
    for(int i=0; i<maxNumModulators; i++) {
      modulators[i].waveForm.readyToPrepareWaveTable = false;
      //modulators[i].waveForm.prepareWaveTable();
    }*/
  }

  virtual int onNewBlock(const std::string &blockName, int blockStartInd, int blockLevel) {
    if(blockName == Reverb::getClassName()) {
      Reverb *reverb = new Reverb(delayLine.sampleRate);
      postProcessingEffects.push_back(reverb);
      return reverb->parse(content, blockStartInd, blockLevel);
    }
    if(blockName == BiquadFilter::getClassName()) {
      BiquadFilter *biquad = new BiquadFilter(&delayLine);
      postProcessingEffects.push_back(biquad);
      return biquad->parse(content, blockStartInd, blockLevel);
    }
    if(blockName == MultiDelay::getClassName()) {
      MultiDelay *multidelay = new MultiDelay(&delayLine);
      postProcessingEffects.push_back(multidelay);
      return multidelay->parse(content, blockStartInd, blockLevel);
    }
    if(blockName == Equalizer::getClassName()) {
      Equalizer *equalizer = new Equalizer(delayLine.sampleRate);
      postProcessingEffects.push_back(equalizer);
      return equalizer->parse(content, blockStartInd, blockLevel);
    }
    if(blockName == Voice::getClassName()) {
      if(numVoices < maxNumVoices) {
        numVoices++;
        return voices[numVoices-1].parse(content, blockStartInd, blockLevel);
      }
    }
    if(blockName == GenericModulator::getClassName()) {
      if(numModulators < maxNumModulators) {
        numModulators++;
        return modulators[numModulators-1].parse(content, blockStartInd, blockLevel);
      }
    }
    if(blockName == Envelope::getClassName()) {
      if(numEnvelopes < maxNumEnvelopes) {
        numEnvelopes++;
        return envelopes[numEnvelopes-1].parse(content, blockStartInd, blockLevel);
      }
    }
    if(blockName == NoteBiquadFilter::getClassName()) {
      return biquadFilter->parse(content, blockStartInd, blockLevel);
    }
    if(blockName == VoiceCrossModulation::getClassName()) {
      return voiceCrossModulation.parse(content, blockStartInd, blockLevel);
    }
    return 0;
  }
  
  
  
  

};


static bool recordNoteSamples2(Instrument *instrument, RecordedNote &recordedNote, double maxLengthSecs) {
  
  double newLength = instrument->getNoteActualLength(recordedNote.lengthInSecs, false);
  if(recordedNote.fullLengthInSecs != newLength) {
    recordedNote.fullLengthInSecs = newLength;
    recordedNote.isReadyToPlayRecorded = false;
    recordedNote.fullLengthInSecs = min(recordedNote.fullLengthInSecs, maxLengthSecs);
    recordedNote.lengthInSamples = int(recordedNote.fullLengthInSecs*recordedNote.sampleRate);
    recordedNote.samples.assign(recordedNote.lengthInSamples, Vec2d::Zero);
  }

  SynthesisNote synthesisNote(recordedNote.sampleRate, recordedNote.pitch, 0, 1, -1);
  synthesisNote.lengthInSecs = recordedNote.lengthInSecs;
  instrument->initializeNote(synthesisNote);
  
  double dt = 1.0 / recordedNote.sampleRate;
  double t = 0;
  Vec2d tmp;
  for(int i=0; i<recordedNote.samples.size(); i++) {
    recordedNote.samples[i].set(0, 0);
    instrument->getSample(recordedNote.samples[i], tmp, dt, t, synthesisNote, false);
    t += dt;
  }

  recordedNote.isReadyToPlayRecorded = true;
  //printf("Note recorded.\n");

  return true;
}


//static bool recordNoteSamples(Instrument *instrument, SequencerNote *sequencerNote, int sampleRate, double maxLengthSecs) {
static bool recordNoteSamples(Instrument *instrument, double pitch, double lengthInSecs, int sampleRate, double maxLengthSecs) {

  //if(recordedNotes.count(pitchLenght) > 0) return true;
  bool alreadyExists = instrument->recordedNotes.count(std::pair<double, double>(pitch, lengthInSecs)) > 0;
  
  RecordedNote &recordedNote = alreadyExists ? instrument->recordedNotes[std::pair<double, double>(pitch, lengthInSecs)] : instrument->recordedNotes[std::pair<double, double>(pitch, lengthInSecs)] = RecordedNote();
  //RecordedNote &recordedNote = instrument->recordedNotes[std::pair<double, double>(pitch, lengthInSecs)] = RecordedNote();
  recordedNote.numSequencerNotes++;
  if(alreadyExists) {
    return true;
  }
  recordedNote.isReadyToPlayRecorded = false;
  //note->isRecorded = false;
  recordedNote.sampleRate = sampleRate;
  recordedNote.lengthInSecs = lengthInSecs;
  recordedNote.pitch = pitch;
  recordedNote.fullLengthInSecs = instrument->getNoteActualLength(lengthInSecs, false);
  
  recordedNote.fullLengthInSecs = min(recordedNote.fullLengthInSecs, maxLengthSecs);
  
  recordedNote.lengthInSamples = int(recordedNote.fullLengthInSecs*sampleRate);
  recordedNote.samples.assign(recordedNote.lengthInSamples, Vec2d::Zero);

  //recordNoteSamples2(instrument, recordedNote, maxLengthSecs);
  
  SynthesisNote synthesisNote(recordedNote.sampleRate, recordedNote.pitch, 0, 1, -1);
  synthesisNote.lengthInSecs = recordedNote.lengthInSecs;
  instrument->initializeNote(synthesisNote);
  
  double dt = 1.0 / recordedNote.sampleRate;
  double t = 0;
  Vec2d tmp;
  for(int i=0; i<recordedNote.samples.size(); i++) {
    instrument->getSample(recordedNote.samples[i], tmp, dt, t, synthesisNote, false);
    t += dt;
  }

  recordedNote.isReadyToPlayRecorded = true;

  //printf("Recording started %s, %d, %f\n", instrument->name.c_str(), note->sampledNoteSampleRate, note->noteFullLengthSecs);
  /*SynthesisNote synthesisNote(sampleRate, pitch, 0, 1, -1);
  synthesisNote.lengthInSecs = lengthInSecs;
  instrument->initializeNote(synthesisNote);
  
  double dt = 1.0 / sampleRate;
  double t = 0;
  Vec2d tmp;
  for(int i=0; i<recordedNote.samples.size(); i++) {
    instrument->getSample(recordedNote.samples[i], tmp, dt, t, synthesisNote, false);
    t += dt;
  }

  recordedNote.isReadyToPlayRecorded = true;*/
  //printf("Note recorded.\n");

  return true;
}




/*static bool recordNoteSamples(Instrument *instrument, Note *note, int sampleRate, double maxLengthSecs) {
  Note noteTmp = *note;
  instrument->initializeNote(noteTmp);
  note->isRecorded = false;
  note->sampledNoteSampleRate = sampleRate;
  note->noteFullLengthSecs = instrument->getNoteActualLength(*note);
  
  //printf("Recording started %s, %d, %f\n", instrument->name.c_str(), note->sampledNoteSampleRate, note->noteFullLengthSecs);
  note->noteFullLengthSecs = min(note->noteFullLengthSecs, maxLengthSecs);
  
  note->sampledNoteLengthSamples = int(note->noteFullLengthSecs*sampleRate);
  note->samples.assign(note->sampledNoteLengthSamples, Vec2d::Zero);
  
  double dt = 1.0 / sampleRate;
  double t = 0;
  Vec2d tmp;
  for(int i=0; i<note->samples.size(); i++) {
    instrument->getSample(note->samples[i], tmp, dt, t, noteTmp, false);
    t += dt;
  }
  
  note->isRecorded = true;
  note->isReadyToPlayRecorded = false;
  //printf("Note recorded.\n");

  return true;
}*/







