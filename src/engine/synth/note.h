#pragma once
#include <vector>
#include "synthconstants.h"
#include "filter/biquad.h"
#include "filter/pitchchanger.h"

/* TODO
 * Instead of synthesizing the sound with the synth at each time point, record the note before, and play it back when note played.
 * This will save gigantic amount of processing time for the real-time audio thread, but this is only usable for notes
 * on the sequencer, not for those played with a real-time controller.
 *
 * - calculate note length = noteLength + envelope release time
 */
 
 /*  I am not sure if I should change the paradigm of these note variables to such that I would include oscillator objects per note.
  * And the oscillator objects would include the phases, and those filters. 
  */
 

struct OscillatorBiquadFilter  {
  //BiquadFilter *biquadFilter = NULL;
  double sampleRate = 1;
  Vec2d x0, x1, x2, y0, y1, y2;
  double a0 = 0, a1 = 0, a2 = 0, b0 = 0, b1 = 0, b2 = 0;
  double frequency = 0;
  double bandwidth = 0;
  double dbGain = 0.1;
  double wetLevel = 1;
  double gain = 1;
  BiquadFilter::Type type = BiquadFilter::Type::LowPass;
  
  void reset() {
    x0.set(0, 0);
    x1.set(0, 0);
    x2.set(0, 0);
    y0.set(0, 0);
    y1.set(0, 0);
    y2.set(0, 0);
  }

  void update(BiquadFilter *biquad) {
    frequency = biquad->frequency;
    bandwidth = biquad->bandwidth;    
    dbGain = biquad->dbGain;    
    wetLevel = biquad->wetLevel;    
    gain = biquad->gain;    
    type = biquad->type;
    prepare();
  }
  
  void set(const OscillatorBiquadFilter &o) {
    sampleRate = o.sampleRate;
    x0.set(o.x0);
    x1.set(o.x1);
    x2.set(o.x2);
    y0.set(o.y0);
    y1.set(o.y1);
    y2.set(o.y2);
    a0 = o.a0;
    a1 = o.a1;
    a2 = o.a2;
    b0 = o.b0;
    b1 = o.b1;
    b2 = o.b2;
    
    frequency = o.frequency;
    bandwidth = o.bandwidth;
    dbGain = o.dbGain;
    wetLevel = o.wetLevel;
    gain = o.gain;
    type = o.type;
  }
  
  /*void updateFactors(BiquadFilter *biquad) {
    a0 = biquad->a0;
    a1 = biquad->a1;
    a2 = biquad->a2;
    b0 = biquad->b0;
    b1 = biquad->b1;
    b2 = biquad->b2;
    frequency = biquad->frequency;
    bandwidth = biquad->bandwidth;    
    wetLevel = biquad->wetLevel;    
    gain = biquad->gain;    
    reset();
  }*/
  
  void init(BiquadFilter *biquadFilter, double noteFrequency, double noteVelocity) {
    if(noteFrequency  < 1) {
      printf("note initialized inproperly!\n");
    }
    sampleRate = biquadFilter->sampleRate;
    type = biquadFilter->type;
    wetLevel = biquadFilter->wetLevel;    
    gain = biquadFilter->gain;    
    dbGain = biquadFilter->dbGain;    
    frequency = biquadFilter->frequency;
    bandwidth = biquadFilter->bandwidth;    
    
    //printf("at OscillatorBiquadFilter::init() 1, %f - %f\n", biquadFilter->frequencyKeyTracking, biquadFilter->bandwidthKeyTracking);
    
    if(biquadFilter->keyTrackingMode == BiquadFilter::KeyTrackingMode::Pitch) {
      if(biquadFilter->frequencyKeyTracking > 0) {
        frequency = (1.0-biquadFilter->frequencyKeyTracking) * biquadFilter->frequency + biquadFilter->frequencyKeyTracking * (biquadFilter->frequency * noteFrequency / noteToFreq(60));
      }
      else {
        frequency = (1.0+biquadFilter->frequencyKeyTracking) * biquadFilter->frequency - biquadFilter->frequencyKeyTracking * (biquadFilter->frequency * noteToFreq(60) / noteFrequency);
      }
      
      if(biquadFilter->bandwidthKeyTracking > 0) {
        bandwidth = (1.0-biquadFilter->bandwidthKeyTracking) * biquadFilter->bandwidth + biquadFilter->bandwidthKeyTracking * (biquadFilter->bandwidth * noteFrequency / noteToFreq(60));
      }
      else {
        bandwidth = (1.0+biquadFilter->bandwidthKeyTracking) * biquadFilter->bandwidth - biquadFilter->bandwidthKeyTracking * (biquadFilter->bandwidth * noteToFreq(60) / noteFrequency);
      }
    }
    if(biquadFilter->keyTrackingMode == BiquadFilter::KeyTrackingMode::Velocity) {
      if(biquadFilter->frequencyKeyTracking > 0) {
        frequency = map(noteVelocity*noteVelocity, 0, 1, (1.0-biquadFilter->frequencyKeyTracking)*biquadFilter->frequency, biquadFilter->frequency);
      }
      else {
        frequency = map(noteVelocity*noteVelocity, 0, 1, biquadFilter->frequency, (1.0+biquadFilter->frequencyKeyTracking)*biquadFilter->frequency);
      }
      
      if(biquadFilter->bandwidthKeyTracking > 0) {
        bandwidth = map(noteVelocity, 0, 1, (1.0-biquadFilter->frequencyKeyTracking)*(10-biquadFilter->bandwidth)+biquadFilter->bandwidth, biquadFilter->bandwidth);
      }
      else {
        bandwidth = map(noteVelocity, 0, 1, biquadFilter->bandwidth, (1.0-biquadFilter->frequencyKeyTracking)*(10-biquadFilter->bandwidth)+biquadFilter->bandwidth);
      }
    }
    
    //printf("at OscillatorBiquadFilter::init() 2,  %f -> %f   |   %f -> %f\n", biquadFilter->frequency, frequency, biquadFilter->bandwidth, bandwidth);
      
    prepare();
    reset();
  }
  
  inline void prepare() {
    prepare(a0, a1, a2, b0, b1, b2, frequency, bandwidth);
  }
  
  void prepare(double &a0, double &a1, double &a2, double &b0, double &b1, double &b2, double &frequency, double &bandwidth) {
    double w0 = 2.0 * PI * frequency / sampleRate;
    double alpha = sin(w0) * sinh(ln2 * bandwidth * w0 / sin(w0));

    if(type == BiquadFilter::Type::None) {
      b0 = 1;
      b1 = 0;
      b2 = 0;
      a0 = 1;
      a1 = 0;
      a2 = 0;
    }
    else if(type == BiquadFilter::Type::Random) {
      b0 = Random::getDouble(-3, 3);
      b1 = Random::getDouble(-3, 3);
      b2 = Random::getDouble(-3, 3);
      a0 = Random::getDouble(-3, 3);
      a1 = Random::getDouble(-3, 3);
      a2 = Random::getDouble(-3, 3);
    }
    else if(type == BiquadFilter::Type::LowPass1) {
      double K = tan(w0*0.5);
      b0 = K/(1.0+K);
      b1 = K/(1.0+K);
      b2 = 0;
      a0 = 1;
      a1 = -(1.0-K)/(1.0+K);
      a2 = 0;
    }
    else if(type == BiquadFilter::Type::HighPass1) {
      double K = tan(w0*0.5);
      b0 = 1.0/(1.0+K);
      b1 = -1.0/(1.0+K);
      b2 = 0;
      a0 = 1;
      a1 = -(1.0-K)/(1.0+K);
      a2 = 0;
    }
    else if(type == BiquadFilter::Type::LowPass) {
      double cosw0 = cos(w0);
      b0 = (1.0 - cosw0) * 0.5;
      b1 = 1.0 - cosw0;
      b2 = (1.0 - cosw0) * 0.5;
      a0 = 1.0 + alpha;
      a1 = -2.0 * cosw0;
      a2 = 1.0 - alpha;
    }
    else if(type == BiquadFilter::Type::HighPass) {
      double cosw0 = cos(w0);
      b0 = (1.0 + cosw0) * 0.5;
      b1 = -1.0 - cosw0;
      b2 = (1.0 + cosw0) * 0.5;
      a0 = 1.0 + alpha;
      a1 = -2.0 * cosw0;
      a2 = 1.0 - alpha;
    }
    else if(type == BiquadFilter::Type::BandPass) {
      double sinw0 = sin(w0);
      b0 = sinw0 * 0.5;
      b1 = 0;
      b2 = -sinw0 * 0.5;
      a0 = 1.0 + alpha;
      a1 = -2.0 * cos(w0);
      a2 = 1.0 - alpha;
    }
    else if(type == BiquadFilter::Type::Notch) {
      double cosw0 = cos(w0);
      b0 = 1;
      b1 = -2.0 * cosw0;
      b2 = 1;
      a0 = 1.0 + alpha;
      a1 = -2.0 * cosw0;
      a2 = 1.0 - alpha;
    }
    else if(type == BiquadFilter::Type::AllPass) {
      double cosw0 = cos(w0);
      b0 = 1.0 - alpha;
      b1 = -2.0 * cosw0;
      b2 = 1.0 + alpha;
      a0 = 1.0 + alpha;
      a1 = -2.0 * cosw0;
      a2 = 1.0 - alpha;
    }
    else if(type == BiquadFilter::Type::PeakingEQ) {
      double A = sqrt(pow(10.0, dbGain / 20.0));
      double cosw0 = cos(w0);
      b0 = 1.0 + alpha * A;
      b1 = -2.0 * cosw0;
      b2 = 1.0 - alpha * A;
      a0 = 1.0 + alpha / A;
      a1 = -2.0 * cosw0;
      a2 = 1.0 - alpha / A;
    }
    else if(type == BiquadFilter::Type::LowShelving || type == BiquadFilter::Type::HighShelving) {
      double V = pow(10.0, fabs(dbGain) / 20.0);
      double K = tan(PI * frequency / sampleRate);
       
      if(type == BiquadFilter::Type::LowShelving) {
        if(dbGain >= 0.0f) {    // boost
          double norm = 1.0f / (1.0f + sqrt(2.0f) * K + K * K);
          b0 = (1.0f + sqrt(2.0f * V) * K + V * K * K) * norm;
          b1 = 2.0f * (V * K * K - 1.0f) * norm;
          b2 = (1.0f - sqrt(2.0f * V) * K + V * K * K) * norm;
          a1 = 2.0f * (K * K - 1.0f) * norm;
          a2 = (1.0f - sqrt(2.0f) * K + K * K) * norm;
        }
        else {    // cut
          double norm = 1.0f / (1.0f + sqrt(2 * V) * K + V * K * K);
          b0 = (1.0f + sqrt(2.0f) * K + K * K) * norm;
          b1 = 2.0f * (K * K - 1.0f) * norm;
          b2 = (1.0f - sqrt(2.0f) * K + K * K) * norm;
          a1 = 2.0f * (V * K * K - 1.0f) * norm;
          a2 = (1.0f - sqrt(2 * V) * K + V * K * K) * norm;
        }
      }
       if(type == BiquadFilter::Type::HighShelving) {
         if(dbGain >= 0.0f) {    // boost
           double norm = 1.0f / (1.0f + sqrt(2.0f) * K + K * K);
           b0 = (V + sqrt(2 * V) * K + K * K) * norm;
           b1 = 2.0f * (K * K - V) * norm;
           b2 = (V - sqrt(2 * V) * K + K * K) * norm;
           a1 = 2.0f * (K * K - 1.0f) * norm;
           a2 = (1.0f - sqrt(2.0f) * K + K * K) * norm;
         }
         else {    // cut
           double norm = 1.0f / (V + sqrt(2.0f * V) * K + K * K);
           b0 = (1.0f + sqrt(2.0f) * K + K * K) * norm;
           b1 = 2.0f * (K * K - 1) * norm;
           b2 = (1.0f - sqrt(2.0f) * K + K * K) * norm;
           a1 = 2.0f * (K * K - V) * norm;
           a2 = (V - sqrt(2.0f * V) * K + K * K) * norm;
         }
       }    
     }
     if(a0 == 0) {
       a0 = 1e-10;
     }
     a1 /= a0;
     a2 /= a0;
     b0 /= a0;
     b1 /= a0;
     b2 /= a0;
     
     //reset();
   }


  
  void apply(Vec2d &sample) {
    x0.set(sample);

    y0 = x0*b0 + x1*b1 + x2*b2 - y1*a1 - y2*a2;
    y2 = y1;
    y1 = y0;
    x2 = x1;
    x1 = x0;

    x0 = x0 * (1.0-wetLevel) + y0 * wetLevel;
    x0 *= gain;
    
    sample.set(x0);
  }

};




struct DeprecatedNote
{

  
  double pitch = 0, frequency = 0, volume = -1;
  double startTime = 0, keyHoldDuration = 0;
  double insertTime = 0;

  bool isHolding = false;
  int instrumentIndex = 0;
  std::string instrumentName = "";

  long initializeID = -1;
  bool isInitialized = false;

  double sampleRate = 1;

  //FIXME
  int noteIndex = -1;

  

  /**************** OSCILLATOR STUFF ****************/
  
  std::vector<Vec2d> phasesGM;
  std::vector<std::vector<Vec2d>> phasesVoiceNew;

  std::vector<Vec2d> amplitudeModulatorOutputs;
  std::vector<Vec2d> frequencyModulatorOutputs;
  std::vector<Vec2d> amplitudeModulatorOutputsPrevious;
  std::vector<Vec2d> frequencyModulatorOutputsPrevious;
  std::vector<double> envelopeOutputs;
  std::vector<Vec2d> voiceOutputs;
  
  
  /**************** NOTE RECORDING STUFF ****************/
  
  std::vector<Vec2d> *samplesReference;
  std::vector<Vec2d> samples;
  double noteFullLengthSecs = 0, noteActualLength = -1;
  int sampledNoteLengthSamples = 0;
  double sampledNoteStart = 0;
  int sampledNoteSampleRate = 0;
  bool isRecorded = false;
  bool isReadyToPlayRecorded = false;


  /**************** SEQUENCER STUFF ****************/
  
  int instrumentTrackIndex = 0;
  int padIndex = 0;
  
  double widthFraction = 1;
  Rect sequencerRect;
  double noteLength = 0;
  double noteValueInverse = 1;

  double startTimeInMeasures = 0;
  bool isSelected = false;
  //long startMeasure = 0;
  //double startMeasureFraction = 0;
  //double lenghtMeasureFraction = 0;

  /**************** PER NOTE FILTERS ****************/
  
  std::vector<OscillatorBiquadFilter> voiceBiquadFilters;
  OscillatorBiquadFilter biquadFilter;
  //std::vector<OscillatorBiquadFilter> modulatorBiquadFilters;
    
    
  /**************** VOCODER STUFF ****************/
  
  PitchChanger *pitchChanger = NULL;




  virtual void reset() {
    pitch = 0;
    frequency = 0;
    volume = -1;
    startTime = 0;
    keyHoldDuration = 0;
    insertTime = 0;
    isHolding = false;
    instrumentIndex = 0;
    initializeID = -1;
    isInitialized = false;
    noteLength = 0;
    noteValueInverse = 1;
    instrumentTrackIndex = 0;
    padIndex = 0;
    
    instrumentName = "";
    
    noteFullLengthSecs = 0;
    sampledNoteLengthSamples = 0;
    sampledNoteStart = 0;
    sampledNoteSampleRate = 0;
    isRecorded = false;
    isReadyToPlayRecorded = false;
    samples.clear();
    
    noteActualLength = -1;
    
    startTimeInMeasures = 0;
    
    phasesGM.assign(phasesGM.size(), Vec2d::Zero);;
    
    isSelected = false;
    
    for(int i=0; i<phasesVoiceNew.size(); i++) {
      phasesVoiceNew[i].assign(phasesVoiceNew[i].size(), Vec2d::Zero);;
    }
    if(pitchChanger) {
      pitchChanger->reset();
    }
  }

  virtual ~DeprecatedNote() {
    if(pitchChanger) {
      delete pitchChanger;
    }
  }
  
  DeprecatedNote() {
    pitch = 0;
    volume = -1;
    startTime = 0;
    instrumentIndex = 0;
  }
  
  DeprecatedNote(double sampleRate, double pitch, double startTime, double volume) {
    this->sampleRate = sampleRate;
    this->pitch = pitch;
    frequency = noteToFreq(pitch);
    this->startTime = startTime;
    this->volume = volume;
    instrumentIndex = 0;
    //pitchChanger.init(sampleRate, 0.1);
  }

  DeprecatedNote(double sampleRate, double pitch, double startTime, double volume, int instrumentIndex) {
    this->sampleRate = sampleRate;
    this->pitch = pitch;
    this->startTime = startTime;
    this->volume = volume;
    frequency = noteToFreq(pitch);
    //phase.set(0, 0);
    this->instrumentIndex = instrumentIndex;
    //pitchChanger.init(sampleRate, 0.1);
  }
  
  //TrackNote(double sampleRate, double pitch, double startTime, double volume) : Note(sampleRate, pitch, startTime, volume) {}
  //TrackNote(double sampleRate, double pitch, double startTime, double volume, int instrumentIndex) : Note(sampleRate, pitch, startTime, volume, instrumentIndex) {}
  DeprecatedNote(double sampleRate, double pitch, double startTime, double volume, int instrumentIndex, int instrumentTrackIndex) : DeprecatedNote(sampleRate, pitch, startTime, volume, instrumentIndex) {
    this->instrumentTrackIndex = instrumentTrackIndex;
  }

  // FIXME
  inline bool prepare(double sampleRate) {
    this->sampleRate = sampleRate;
    if(!pitchChanger) {
      pitchChanger = new PitchChanger();
    }
    if(pitchChanger->delayLine.buffer.size() == 0) {
      pitchChanger->init(sampleRate, 0.1, 0.1);
      return true;
    }
    return false;
  }

  void operator=(const DeprecatedNote &note) {
    reset();
    this->pitch = note.pitch;
    this->startTime = note.startTime;
    this->frequency = note.frequency;
    this->volume = note.volume;
    this->instrumentIndex = note.instrumentIndex;
    this->frequency = note.frequency;
    this->keyHoldDuration = note.keyHoldDuration;
    this->insertTime = note.insertTime;
    this->widthFraction = note.widthFraction;
    this->noteLength = note.noteLength;
    this->noteValueInverse = note.noteValueInverse;
    this->instrumentTrackIndex = note.instrumentTrackIndex;
    this->padIndex = note.padIndex;
    this->isHolding = note.isHolding;
    
    noteFullLengthSecs = note.noteFullLengthSecs;
    sampledNoteLengthSamples = note.sampledNoteLengthSamples;
    sampledNoteStart = note.sampledNoteStart;
    sampledNoteSampleRate = note.sampledNoteSampleRate;
    isRecorded = note.isRecorded;
    isReadyToPlayRecorded = note.isReadyToPlayRecorded;
    //samplesReference = note.samples.size();
    samples = note.samples;
    
    sampleRate = note.sampleRate;
    
    noteActualLength = note.noteActualLength;
    //pitchChanger.init(sampleRate, 0.1);

    //pitchChanger = note.pitchChanger;
    
    startTimeInMeasures = note.startTimeInMeasures;
    
    //phase.set(0, 0);
  }
  
  void set(const DeprecatedNote &note) {
    reset();
    this->pitch = note.pitch;
    this->startTime = note.startTime;
    this->frequency = note.frequency;
    this->volume = note.volume;
    this->instrumentIndex = note.instrumentIndex;
    this->frequency = note.frequency;
    this->keyHoldDuration = note.keyHoldDuration;
    this->insertTime = note.insertTime;
    this->widthFraction = note.widthFraction;
    this->noteLength = note.noteLength;
    this->noteValueInverse = note.noteValueInverse;
    this->isHolding = note.isHolding;
    
    noteFullLengthSecs = note.noteFullLengthSecs;
    sampledNoteLengthSamples = note.sampledNoteLengthSamples;
    sampledNoteStart = note.sampledNoteStart;
    sampledNoteSampleRate = note.sampledNoteSampleRate;
    isRecorded = note.isRecorded;
    isReadyToPlayRecorded = note.isReadyToPlayRecorded;
    //samplesReference = note.samples.size();
    samples = note.samples;
    
    sampleRate = note.sampleRate;
    
    noteActualLength = note.noteActualLength;
    //pitchChanger.init(sampleRate, 0.1);

  }


  void set(double sampleRate, double pitch, double startTime, double volume, int instrumentIndex) {
    reset();
    this->sampleRate = sampleRate;
    this->pitch = pitch;
    this->startTime = startTime;
    this->volume = volume;
    this->instrumentIndex = instrumentIndex;
    frequency = noteToFreq(pitch);

  }


  void print() {
    printf("pitch %.2f, volume %.2f, start %.2f, hold %.2f, holding %d, insert %.2f, freq. %.2f\n", pitch, volume, startTime, keyHoldDuration, isHolding, insertTime, frequency);
  }

  void transpose(double steps) {
    pitch += steps;
    frequency = noteToFreq(pitch);
  }

};












struct RecordedNote
{
  double pitch = 0;
  //double volume = -1;
  double fullLengthInSecs = 0;

  double sampleRate = 1;
    
  std::vector<Vec2d> samples;
  //double noteFullLengthSecs = 0, noteActualLength = -1;
  int lengthInSamples = 0;
  //double sampledNoteStart = 0;
  //int sampledNoteSampleRate = 0;
  bool isReadyToPlayRecorded = false;

  int numSequencerNotes = 0;

  /*RecordedNote() {}
  RecordedNote(double sampleRate, double pitch, double noteLength) {
    
  }*/

  void resetSamples() {
    pitch = 0;
    //volume = 0;
    fullLengthInSecs = 0;
    sampleRate = 1;
      
    samples.clear();
    //noteFullLengthSecs = 0;
    //noteActualLength = -1;
    int sampledNoteLengthSamples = 0;
    bool isReadyToPlayRecorded = false;

  }
};




struct SynthesisNote
{
  double pitch = 0, frequency = 0, volume = -1;
  double startTime = 0;
  double lengthInSecs = 0;
  //double keyHoldDuration = 0;
  double insertTime = 0;

  bool isHolding = false;
  int instrumentIndex = 0;

  long initializeID = -1;
  bool isInitialized = false;

  //FIXME
  int noteIndex = -1;

  double sampleRate = 1;

  double noteFullLengthSecs = 0;
  
  std::string instrumentName;

  /**************** OSCILLATOR STUFF ****************/
  
  std::vector<Vec2d> phasesGM;
  std::vector<std::vector<Vec2d>> phasesVoiceNew;

  std::vector<Vec2d> amplitudeModulatorOutputs;
  std::vector<Vec2d> frequencyModulatorOutputs;
  std::vector<Vec2d> amplitudeModulatorOutputsPrevious;
  std::vector<Vec2d> frequencyModulatorOutputsPrevious;
  std::vector<double> envelopeOutputs;
  std::vector<Vec2d> voiceOutputs;
  

  /**************** PER NOTE FILTERS ****************/
  
  std::vector<OscillatorBiquadFilter> voiceBiquadFilters;
  OscillatorBiquadFilter biquadFilter;
  //std::vector<OscillatorBiquadFilter> modulatorBiquadFilters;
    
    
  /**************** VOCODER STUFF ****************/
  
  PitchChanger *pitchChanger = NULL;




  virtual void reset() {
    instrumentName = "";
    pitch = 0;
    frequency = 0;
    volume = -1;
    startTime = 0;
    lengthInSecs = 0;
    
    insertTime = 0;

    isHolding = false;
    instrumentIndex = 0;

    initializeID = -1;
    isInitialized = false;

    noteIndex = -1;
    
    sampleRate = 1;
    
    noteFullLengthSecs = 0;
    
    phasesGM.assign(phasesGM.size(), Vec2d::Zero);;
    
    for(int i=0; i<phasesVoiceNew.size(); i++) {
      phasesVoiceNew[i].assign(phasesVoiceNew[i].size(), Vec2d::Zero);;
    }
    if(pitchChanger) {
      pitchChanger->reset();
    }
  }

  virtual ~SynthesisNote() {
    if(pitchChanger) {
      delete pitchChanger;
    }
  }
  
  SynthesisNote() {}


  SynthesisNote(double sampleRate, double pitch, double startTime, double volume, int instrumentIndex) {
    this->sampleRate = sampleRate;
    this->pitch = pitch;
    this->startTime = startTime;
    this->volume = volume;
    this->instrumentIndex = instrumentIndex;
  }
  

  // FIXME
  inline bool prepare(double sampleRate) {
    this->sampleRate = sampleRate;
    if(!pitchChanger) {
      pitchChanger = new PitchChanger();
    }
    if(pitchChanger->delayLine.buffer.size() == 0) {
      pitchChanger->init(sampleRate, 0.1, 0.1);
      return true;
    }
    return false;
  }

  void operator=(const SynthesisNote &note) {
    pitch = note.pitch;
    frequency = note.frequency;

    startTime = note.startTime;
    lengthInSecs = note.lengthInSecs;
    
    insertTime = note.insertTime;

    isHolding = note.isHolding;
    instrumentIndex = note.instrumentIndex;

    initializeID = -1;
    isInitialized = false;

    noteIndex = note.noteIndex;

    sampleRate = note.sampleRate;
    
    noteFullLengthSecs = note.noteFullLengthSecs;
    
    instrumentName = note.instrumentName;
  }
  

  void set(double sampleRate, double pitch, double startTime, double volume, int instrumentIndex) {
    reset();
    this->sampleRate = sampleRate;
    this->pitch = pitch;
    this->startTime = startTime;
    this->volume = volume;
    this->instrumentIndex = instrumentIndex;
  }


  void print() {
    printf("pitch %.2f, volume %.2f, start %.2f, length %.2f, holding %d, isInitialized %d\n", pitch, volume, startTime, lengthInSecs, isHolding, isInitialized);
  }

};






struct SequencerNote
{  
  double pitch = 0, volume = -1;
  double startTime = 0;
  double lengthInSecs = 0;

  int instrumentTrackIndex = 0;

  // FIXME set us to at initialization
  double widthFraction = 1;
  Rect sequencerRect;
  double noteValueInverse = 1;

  double startTimeInMeasures = 0;
  bool isSelected = false;

  SequencerNote() {}
  
  SequencerNote(double pitch, double lengthInSecs, double startTime, double volume, double instrumentTrackIndex) {
    this->pitch = pitch;
    this->lengthInSecs = lengthInSecs;
    this->startTime = startTime;
    this->volume = volume;
    this->instrumentTrackIndex = instrumentTrackIndex;
  }
  

  void reset() {
    pitch = 0;
    volume = -1;
    startTime = 0;

    
    instrumentTrackIndex = 0;
    
    widthFraction = 1;
    sequencerRect.set(0, 0, 0, 0);
    lengthInSecs = 0;
    noteValueInverse = 1;

    startTimeInMeasures = 0;
    isSelected = false;
  }



  /*SequencerNote(double pitch, double noteLength, double startTime, double volume, int instrumentTrackIndex) {
    this->pitch = pitch;
    this->noteLength = noteLength;
    this->startTime = startTime;
    this->volume = volume;
    frequency = noteToFreq(pitch);
    this->instrumentTrackIndex = instrumentTrackIndex;
  }*/
  

  void print() {
    //printf("pitch %.2f, volume %.2f, start %.2f, hold %.2f, holding %d, insert %.2f, freq. %.2f\n", pitch, volume, startTime, lengthInSecs, isHolding, insertTime, frequency);
  }

  void transpose(double steps) {
    pitch += steps;
    //frequency = noteToFreq(pitch);
  }

};






