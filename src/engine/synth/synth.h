#pragma once
#include <ctgmath>
#include <portaudio.h>
#include <list>
#include <SDL2/SDL.h>
#include <unordered_map>

#include "../gui/gui.h"
#include "../portaudiointerface.h"
#include "../rtmidi_init.h"

#include "synthconstants.h"
#include "waveform.h"
#include "envelope.h"
#include "oscillator.h"
#include "note.h"
#include "instrument.h"
#include "drumpad.h"
#include "delayline.h"
#include "instrumenttrack.h"
#include "loopersequencetrack.h"

#include "stereooscilloscope.h"

#include "filter/postprocessingeffect.h"
//#include "filter/reverbproto.h"
#include "filter/multidelay.h"
#include "filter/biquad.h"
#include "filter/eq.h"
//#include "filter/reverbproto2.h"
#include "filter/reverb.h"
#include "filter/pitchchanger.h"
//#include "filter/pitchexp.h"
//#include "synthfileio.h"
#include "recording.h"
#include "recordingtrack.h"
#include "sndfileio.h"

#include "preset.h"

#include "spectrummap.h"



struct SequenceLooper {
  const int maxNumTracks = 1;
  int numTracks = 1;
  int currentTrack = 0;
  int numRepeats = 0;
  int currentRepeat = 0;

  
  // FIXME get rid of the pointer
  std::vector<LooperSequenceTrack*> tracks;
  LooperSequenceTrack *copyTrack = NULL;
  
  virtual ~SequenceLooper() {
    for(int i=0; i<tracks.size(); i++) delete tracks[i];
  }
  
  void init(int sampleRate) {
    tracks.resize(maxNumTracks);
    
    for(int i=0; i<maxNumTracks; i++) {
      tracks[i] = new LooperSequenceTrack(i, sampleRate);
    }
  }
  
  /*void setMaxNumTrackNotes(int maxNumTrackNotes) {
    this->maxNumTrackNotes = maxNumTrackNotes;
    for(int i=0; i<tracks.size(); i++) {
      tracks[i]->setNumNotes(maxNumTrackNotes);
    }
  }*/
};


struct Synth : public PortAudioInterface, public HierarchicalTextFileParser
{
  enum Mode { Nothing, Looper, Song };

  const std::string defaultRecordingsPath = "output/synth/recordings/";
  const std::string defaultInstrumentPath = "data/synth/instruments/";
  const std::string defaultEffectPath = "data/synth/effects/";
  
  SequenceLooper sequenceLooper;

  //bool loopTracksActive = true;
  Mode mode = Mode::Nothing;
  bool isRecordingMode = false;

  const int maxCurrentlyPlayingNotes = 1024;
  int numCurrentlyPlayingNotes = 10;
  std::vector<Note> currentlyPlayingNotes;

  const int maxCurrentlyPlayingNotesPresampled = 1024;
  int numCurrentlyPlayingNotesPresampled = 50;
  std::vector<Note> currentlyPlayingNotesPresampled;

  double masterVolume = 1.0;
  double inputVolume = 1.0;

  bool isStereo = true;
  int keyBaseNote = 0;

  double defaultNoteVolume = 0.75;
  bool applyDefaultVolumeTokeyboards = false;

  double defaultNoteKeyHoldTime = 0.0;

  double noteHoldTime = 8;

  std::vector<Instrument*> instruments;
  int activeInstrumentIndex = 0;

  int activeInstrumentTrackIndex = 0;

  static const int maxNumInstrumentTracks = 16;
  int numInstrumentTracks = 0;
  std::vector<InstrumentTrack> instrumentTracks = std::vector<InstrumentTrack>(maxNumInstrumentTracks);

  std::vector<PostProcessingEffect*> postProcessingEffects;
  std::vector<PostProcessingEffect*> inputPostProcessingEffects;
  Equalizer *equalizer = NULL;
  Reverb *reverb = NULL;
  DelayLine delayLine;

  DelayLine inputDelayLine;
  FFTW3Interface delayLineFFTW, delayLineFFTWRight;

  std::unordered_map<int, Note*> holdingNotesMidi;
  std::unordered_map<int, Note*> holdingNotesKeyboard;
  std::vector<Note*> holdingNotesScreenKeys = std::vector<Note*>(3, NULL); 

  int screenKeyboardMinNote = 60 - 36;
  int screenKeyboardMaxNote = 60 + 36;

  bool isKeyboardPianoActive = false;

  bool screenKeysActive = false;
  bool isBackgroundGraphVisible = false;

  int screenW = 1, screenH = 1;

  StereoOscilloscope stereoOscilloscope;
  
  int measuresPerLooperTrack = 4;
  int beatsPerMeasure = 4;
  int subdivisionsPerMeasure = 8;
  double noteValueInverse = 8;

  bool progressOnNoteStartWhenPaused = true;

  double tempo = 1;
  double looperTrackDuration = 300;
  int lastUpdateFrame = -1;
  int beatsPerLoop = 128;
  int lastUpdateBeat = -1;
  double looperTime = 0;
  bool isLooperPlaying = false;

  std::vector<Vec2d> recordingPlaybackBuffer;
  long recordingPlaybackBufferPosition = 0;

  RecordingPlayback recordingPlayback;
  bool isPlayingRecordedAudio = false;
  
  AudioRecording audioRecording;
  double audioRecordingMaxLengthInSeconds = 60;
  bool recordingAudio = false;

  bool playAudioInput = false;


  int acticeInstrumentTrackIndex = 0;  


  double parseTimeTotal = 0;
  double applyTimeTotal = 0;
  double parseTimeLatest = 0;
  double applyTimeLatest = 0;
  int numInstrumentsLoaded = 0;
  std::string latestLoadedFilename = "";


  bool recordTrackNotes = true;
  std::vector<std::thread> recordingThreads;
  int recordNoteTrackResetInstrumentTrackIndex = -1;
  bool recordNoteTrackRestartRequested = false;
  bool recordNoteTrackActive = false;
  std::vector<std::thread> trackRecordingThreads;

  const int maxAudioRecordingTracks = 32;
  int numAudioRecordingTracks = 0;
  std::vector<AudioRecordingTrack> audioRecordingTracks = std::vector<AudioRecordingTrack>(maxAudioRecordingTracks);
  
  //std::vector<Preset> postProcessingEffectPresets;
  //std::vector<Preset> reverbPresets;
  //std::vector<std::string> reverbPresetNames;
  //std::vector<EffectString> equalizerPresets;
  //std::vector<EffectString> multiDelayPresets;
  
  int numDrumPads = 0;
  
  int loopFrame = 0;


  
  // TODO Trying to prevent concurrent access related bugs, find out other ways...
  bool synthThreadExecuting = false;
  bool synthThreadDisabled = false;
  bool synthThreadDisableRequested = false;
  
  int requestedNumberOfNotes = 0, requestedNumberOfNotesPresampled = 0;
  
  
  Vec2d sequencerTimeWindow;
  double sequencerZoom = 1.0;


  Synth(double sampleRate = -1, int framesPerBuffer = 128, double suggestedOutputLatency = 0, int screenW = 1, int screenH = 1, int suggestedAudioDevice = -1) {
    setup(sampleRate, framesPerBuffer, suggestedOutputLatency, screenW, screenH, suggestedAudioDevice);
    
    prepareBiquadFilterReferenceGraph(this->sampleRate);
    //activeInstruments.resize(maxNumActiveInstruments);

    requestedNumberOfNotes = numCurrentlyPlayingNotes;
    requestedNumberOfNotesPresampled = numCurrentlyPlayingNotesPresampled;
    currentlyPlayingNotes.resize(maxCurrentlyPlayingNotes);
    currentlyPlayingNotesPresampled.resize(maxCurrentlyPlayingNotesPresampled);

    for(int i=0; i<maxCurrentlyPlayingNotes; i++) {
      currentlyPlayingNotes[i].prepare(this->sampleRate);
    }
    for(int i=0; i<maxCurrentlyPlayingNotesPresampled; i++) {
      currentlyPlayingNotesPresampled[i].prepare(this->sampleRate);
    }
    
    
    recordingPlaybackBuffer.resize(this->sampleRate*5);

    sequenceLooper.init(sampleRate);
    
    tempo = 60;
    looperTrackDuration = 60 * measuresPerLooperTrack * beatsPerMeasure / tempo;
    sequencerTimeWindow.y = looperTrackDuration;
    looperTime = looperTrackDuration;

    addInstrument();
    numInstrumentTracks = 1;

    delayLine.init(this->sampleRate, 10.0);
    inputDelayLine.init(this->sampleRate, 10.0);
    
    postProcessingEffects.push_back(equalizer = new Equalizer(this->sampleRate));
    postProcessingEffects.push_back(new PitchChanger(this->sampleRate));
    postProcessingEffects.push_back(reverb = new Reverb(this->sampleRate));
    for(int i=0; i<postProcessingEffects.size(); i++) {
      postProcessingEffects[i]->isActive = false;
    }

    inputPostProcessingEffects.push_back(new Equalizer(this->sampleRate));
    inputPostProcessingEffects.push_back(new PitchChanger(this->sampleRate));
    inputPostProcessingEffects.push_back(new Reverb(this->sampleRate));
    for(int i=0; i<inputPostProcessingEffects.size(); i++) {
      inputPostProcessingEffects[i]->isActive = false;
    }
    

    loadEffectsPresets();

    delayLineFFTW.initialize(1024*64, false);
    delayLineFFTWRight.initialize(1024*64, false);



    audioRecording.initialize(delayLine);

    for(int i=0; i<audioRecordingTracks.size(); i++) {
      audioRecordingTracks[i].init(&inputDelayLine, framesPerBuffer);
    }

    updateActiveInstruments();
    
    this->start();
  }

  ~Synth() {
    for(int i=0; i<postProcessingEffects.size(); i++) delete postProcessingEffects[i];
    for(int i=0; i<inputPostProcessingEffects.size(); i++) delete inputPostProcessingEffects[i];
    for(int i=0; i<instruments.size(); i++) delete instruments[i];
  }

  void onQuit() {
    for(int i=0; i<recordingThreads.size(); i++) {
      recordingThreads[i].join();
    }
    for(int i=0; i<trackRecordingThreads.size(); i++) {
      trackRecordingThreads[i].join();
    }
  }


  void reset() {
    inputDelayLine.reset();
    delayLine.reset();
    for(Instrument *instrument : instruments) {
      instrument->reset();
    }
    for(int i=0; i<postProcessingEffects.size(); i++) {
      postProcessingEffects[i]->reset();
    }
    for(int i=0; i<inputPostProcessingEffects.size(); i++) {
      inputPostProcessingEffects[i]->reset();
    }

    for(int i=0; i<numCurrentlyPlayingNotes; i++) {
      //currentlyPlayingNotes[i].pitchChanger->reset();
    }
    for(int i=0; i<numCurrentlyPlayingNotesPresampled; i++) {
      //currentlyPlayingNotesPresampled[i].pitchChanger.reset();
    }
  }

  void updateActiveInstruments() {
    for(int i=0; i<instruments.size(); i++) {
      //if(instruments[i]->instrumentType != Instrument::InstrumentType::DefaultInstrument) continue;
      
      bool active = instruments[i]->isActive;
      bool activationNeeded = false, inactivationNeeded = instruments[i]->isActive;

      for(int k=0; k<numInstrumentTracks; k++) {
        //if(instruments[instrumentTracks[k].instrumentIndex]->instrumentType == Instrument::InstrumentType::DefaultInstrument) {
          if(i == instrumentTracks[k].instrumentIndex && !instruments[i]->isActive) {
            activationNeeded = true;
          }
          if(i == instrumentTracks[k].instrumentIndex) {
            inactivationNeeded = false;
          }
        //}
        if(instruments[instrumentTracks[k].instrumentIndex]->instrumentType == Instrument::InstrumentType::CompositePads) {
          DrumPad *drumPad = dynamic_cast<DrumPad*>(instruments[instrumentTracks[k].instrumentIndex]);
          if(drumPad) {
            for(int q=0; q<drumPad->numPads; q++) {
              if(i == drumPad->pads[q].instrumentIndex && !instruments[drumPad->pads[q].instrumentIndex]->isActive) {
                activationNeeded = true;
              }
              if(i == drumPad->pads[q].instrumentIndex) {
                inactivationNeeded = false;
              }
            }
          }
        }
      }


      if(activationNeeded) {
        //printf("activation: %s\n", instruments[i]->name.c_str());
        instruments[i]->update();
        if(instruments[i]->instrumentType == Instrument::InstrumentType::DefaultInstrument) {
          instruments[i]->isActive = true;
          instruments[i]->inactivationRequested = false;
        }
      }

      if(inactivationNeeded) {
        for(int k=0; k<numCurrentlyPlayingNotes; k++) {
          if(currentlyPlayingNotes[k].instrumentIndex == i) {
            currentlyPlayingNotes[k].reset();
          }
        }
        for(int k=0; k<numCurrentlyPlayingNotesPresampled; k++) {
          if(currentlyPlayingNotesPresampled[k].instrumentIndex == i) {
            currentlyPlayingNotesPresampled[k].reset();
          }
        }
        instruments[i]->inactivationRequested = true;
        instruments[i]->reset();
        setSustain(false, i);
      }
    }
  }


  void startAudioRecording() {
    audioRecording.initialize(delayLine);
    recordingAudio = true;
  }
  void updateAudioRecording() {
    if(recordingAudio) {
      audioRecording.update(delayLine);
      if(audioRecording.tapeFull) {
        endAudioRecording();
      }
    }
  }
  void endAudioRecording() {
    audioRecording.update(delayLine);
    recordingAudio = false;
  }
  

  
  void playRecordedAudio() {
    //if(recordingPlayback.status == RecordingPlayback::Status::Uninitialized) {
    /*isPlayingRecordedAudio = !isPlayingRecordedAudio;
    if(isPlayingRecordedAudio) {
      recordingPlayback.initialize(&audioRecording.samples, framesPerBuffer, sampleRate);
    }*/
    isPlayingRecordedAudio = true;
    recordingPlayback.initialize(&audioRecording.samples, framesPerBuffer, sampleRate);
  }
  
  void stopRecordedAudio() {
    recordingPlayback.status = RecordingPlayback::Status::Uninitialized;
    isPlayingRecordedAudio = false;
  }
  
  
  
  void saveAudioRecordingToFile(const std::string &filename) {
    createDirectories(defaultRecordingsPath);
    std::string fileExtension = extractFileExtension(filename);
    if(fileExtension.size() == 0) {
      bool ret = AudioFileIo::save(defaultRecordingsPath + filename + ".ogg", audioRecording);
      ret = ret && AudioFileIo::save(defaultRecordingsPath + filename + ".flac", audioRecording);
      if(ret) {
        printf("Audio recording saved to files %s.ogg and %s.flac\n", filename.c_str(), filename.c_str());
      }
    }
    else {
      if(AudioFileIo::save(filename, audioRecording)) {
        printf("Audio recording saved to file \"%s\"\n", filename.c_str());
      }
    }
  }


  
  void printLoadingTimes() {
    printf("Load times. %s %.4f s (%.0f), avg. %.4f s (%.0f), total %.4f s\n", latestLoadedFilename.c_str(),
              applyTimeLatest+parseTimeLatest, applyTimeLatest/parseTimeLatest,
              (applyTimeTotal+parseTimeTotal)/numInstrumentsLoaded, applyTimeTotal/parseTimeTotal,
              applyTimeTotal+parseTimeTotal);
  }



  void addInstrument() {
    instruments.push_back(new Instrument(sampleRate, framesPerBuffer));
  }
  

  
  void addDrumPad() {
    instruments.push_back(new DrumPad(&instruments));
    instruments[instruments.size()-1]->name = "Drum pad " + std::to_string(++numDrumPads);
  }

  void duplicateInstrument(std::string duplicateName = "") {
    //printf("(debugging) at Synth::duplicateInstrument()\n");
    //if(activeInstrumentIndex < 0 || activeInstrumentIndex >= instruments.size()) return;
    if(numInstrumentTracks >= 0 && activeInstrumentTrackIndex < numInstrumentTracks) {
      if(duplicateName.size() == 0) {
        duplicateName = instruments[instrumentTracks[activeInstrumentTrackIndex].instrumentIndex]->name + " X";
      }
      Instrument *duplicateInstrument = new Instrument(*instruments[instrumentTracks[activeInstrumentTrackIndex].instrumentIndex]);
      duplicateInstrument->name = duplicateName;
      instruments.insert(instruments.begin()+instrumentTracks[activeInstrumentTrackIndex].instrumentIndex+1, duplicateInstrument);
    }
  }




  void removeInstrument(int index = -1) {
    if(instruments.size() <= 1) return;
    //Instrument *instrument = instruments[activeInstrumentIndex];
    if(index == -1) index = activeInstrumentIndex;

    for(int i=0; i<numCurrentlyPlayingNotes; i++) {
      if(currentlyPlayingNotes[i].instrumentIndex == index) {
        currentlyPlayingNotes[i].reset();
      }
    }
    for(int i=0; i<numCurrentlyPlayingNotesPresampled; i++) {
      if(currentlyPlayingNotesPresampled[i].instrumentIndex == index) {
        currentlyPlayingNotesPresampled[i].reset();
      }
    }
    for(int i=0; i<sequenceLooper.tracks.size(); i++) {
      for(int k=0; k<sequenceLooper.tracks[i]->notes.size(); k++) {
        if(sequenceLooper.tracks[i]->notes[k].instrumentIndex == index) {
          sequenceLooper.tracks[i]->notes[k].reset();
        }
      }
    }
    delete instruments[index];
    instruments.erase(instruments.begin() + index);

    activeInstrumentIndex = min(activeInstrumentIndex, (int)instruments.size()-1);

    /*for(int i=0; i<maxNumActiveInstruments; i++) {
      activeInstruments[i] = min(activeInstruments[i], (int)instruments.size()-1);
    }*/
    for(int i=0; i<maxNumInstrumentTracks; i++) {
      if(instrumentTracks[i].instrumentIndex > index) {
        instrumentTracks[i].instrumentIndex--;
      }
    }
  }

  Instrument *getInstrument() {
    if(activeInstrumentIndex < instruments.size()) {
      return instruments[activeInstrumentIndex];
    }
    return NULL;
  }

  InstrumentTrack *getInstrumentTrack() {
    return &instrumentTracks[activeInstrumentTrackIndex];
  }

  int setNumCurrentlyPlayingNotes(int num) {
    num = min(num, maxCurrentlyPlayingNotes);

    if(num != numCurrentlyPlayingNotes) {
      int a = min(num, numCurrentlyPlayingNotes);
      int b = max(num, numCurrentlyPlayingNotes);

      for(int i=a; i<b; i++) {
        currentlyPlayingNotes[i].reset();
      }
      numCurrentlyPlayingNotes = num;
    }
    return numCurrentlyPlayingNotes;
  }

  int setNumCurrentlyPlayingNotesPresampled(int num) {
    num = min(num, maxCurrentlyPlayingNotesPresampled);

    if(num != numCurrentlyPlayingNotesPresampled) {
      int a = min(num, numCurrentlyPlayingNotesPresampled);
      int b = max(num, numCurrentlyPlayingNotesPresampled);

      for(int i=a; i<b; i++) {
        currentlyPlayingNotesPresampled[i].reset();
      }
      numCurrentlyPlayingNotesPresampled = num;
    }
    return numCurrentlyPlayingNotesPresampled;
  }



  // FIXME fix us
  void roundLoopTimeMeasureSubDivisions() {
    looperTime = (round(looperTime/looperTrackDuration * subdivisionsPerMeasure * measuresPerLooperTrack)) / (subdivisionsPerMeasure * measuresPerLooperTrack) * looperTrackDuration;
  }
  
  double roundLoopTimeNoteValueInverse(double time) {
    return (round(time/looperTrackDuration * noteValueInverse * measuresPerLooperTrack)) / (noteValueInverse * measuresPerLooperTrack) * looperTrackDuration;
  }


  void roundLoopTimeNoteValueInverse() {
    looperTime = (round(looperTime/looperTrackDuration * noteValueInverse * measuresPerLooperTrack)) / (noteValueInverse * measuresPerLooperTrack) * looperTrackDuration;
  }

  double floorLoopTime(double t) {
    return (floor(t / looperTrackDuration * subdivisionsPerMeasure * measuresPerLooperTrack)) / (subdivisionsPerMeasure * measuresPerLooperTrack) * looperTrackDuration;
  }

  void progressLoopTimeByMeasureSubdivisions(double amount = 1.0) {
    roundLoopTimeMeasureSubDivisions();
    double t = subdivisionsPerMeasure > 0 ? 1.0 / subdivisionsPerMeasure : 1.0;
    looperTime += looperTrackDuration * t * amount / measuresPerLooperTrack;
    looperTime = max(looperTrackDuration, looperTime);
    roundLoopTimeMeasureSubDivisions();
    
    setTimeWindowWithinLoopProgress();
  }

  void progressLoopTimeByNoteValue(double amount = 1.0) {
    roundLoopTimeNoteValueInverse();
    double t = noteValueInverse > 0 ? 1.0 / noteValueInverse : 1.0;
    looperTime += looperTrackDuration * t * amount / measuresPerLooperTrack;
    looperTime = max(looperTrackDuration, looperTime);
    roundLoopTimeNoteValueInverse();
    
    setTimeWindowWithinLoopProgress();
  }

  void progressLoopTimeByMeasures(int amount = 1) {
    int currentMeasure = int(looperTime / looperTrackDuration * measuresPerLooperTrack);
    if(amount < 0 && fract(looperTime / looperTrackDuration * measuresPerLooperTrack) > 0.00000001) amount += 1;
    int nextMeasure = (measuresPerLooperTrack+currentMeasure+amount) % measuresPerLooperTrack;
    looperTime = looperTrackDuration / measuresPerLooperTrack * nextMeasure;
    looperTime = max(looperTrackDuration, looperTime);
    
    setTimeWindowWithinLoopProgress();
  }


  void setTimeWindowWithinLoopProgress() {
    if(sequencerZoom > 1 && !draggingSequencerZoom && !draggingSequencerTimeWindow) {
      double time = modfff(looperTime, looperTrackDuration);
      if((time < sequencerTimeWindow.x || time >= sequencerTimeWindow.x + looperTrackDuration/measuresPerLooperTrack * 4)) {
        sequencerTimeWindow.x = (int(time/looperTrackDuration*measuresPerLooperTrack) / 4 * 4) * (double)looperTrackDuration / measuresPerLooperTrack;
        sequencerTimeWindow.y = sequencerTimeWindow.x + looperTrackDuration/sequencerZoom;
        //sequencerTimeWindow.y = sequencerTimeWindow.x + looperTrackDuration/measuresPerLooperTrack * 4;
        clampSequencerTimeWindow();
        updateActiveNoteSequencerRects();
      }
    }
  }



  void setSustain(bool isSustainActive, int instrumentIndex) {
    if(instrumentIndex >= 0 && instrumentIndex < instruments.size()) {
      instruments[instrumentIndex]->isSustainActive = isSustainActive;
      if(!isSustainActive) {
        std::vector<int> notesToBeErased;
        for(const auto &intNotePair : instruments[instrumentIndex]->sustainedNotes) {
          Note *note = intNotePair.second;
          if(holdingNotesMidi.count(note->noteIndex) == 0) {
            note->isHolding = false;
            note->keyHoldDuration = getPaTime() - note->startTime;
            note->noteActualLength = instruments[instrumentIndex]->getNoteActualLength(*note);
            notesToBeErased.push_back(note->noteIndex);
          }
        }
        for(int i=0; i<notesToBeErased.size(); i++) {
          instruments[instrumentIndex]->sustainedNotes.erase(notesToBeErased[i]);
        }
      }
    }
  }

  void midiTrackNoteOn(unsigned char pitch, unsigned char velocity, int channel, int instrumentTrackIndex) {
    if(!instrumentTracks[instrumentTrackIndex].isMuted) {
      midiNoteOn(pitch, velocity, instrumentTrackIndex, instrumentTracks[instrumentTrackIndex].instrumentIndex, instrumentTrackIndex);
    }
  }
  void midiTrackNoteOff(unsigned char pitch, unsigned char velocity, int channel, int instrumentTrackIndex) {
    if(!instrumentTracks[instrumentTrackIndex].isMuted) {
      midiNoteOff(pitch, velocity, instrumentTrackIndex, instrumentTracks[instrumentTrackIndex].instrumentIndex, instrumentTrackIndex);
    }
  }


  void midiNoteOn(unsigned char pitch, unsigned char velocity, int channel, int instrumentIndex, int instrumentTrackIndex = -1) {
    if(instrumentIndex >= 0 && instrumentIndex < instruments.size()) {
      int noteIndex = (channel << 8) | pitch;
      
      if(holdingNotesMidi.count(noteIndex) > 0) {
        midiNoteOff(pitch, 0, channel, instrumentIndex, instrumentTrackIndex);
      }
      
      double volume = applyDefaultVolumeTokeyboards ? defaultNoteVolume : (double)velocity/127.0;
      Note *note = NULL;
      if(instrumentTrackIndex == -1) {
        note = startInstrumentNote(pitch, volume, instrumentIndex);
      }
      else {
        note = startInstrumentTrackNote(pitch, volume, instrumentTrackIndex);
      }

      if(note) {
        if(instruments[instrumentIndex]->sustainedNotes.count(noteIndex) > 0) {
          // FIXME wrap to functions
          instruments[instrumentIndex]->sustainedNotes[noteIndex]->isHolding = false;
          instruments[instrumentIndex]->sustainedNotes[noteIndex]->keyHoldDuration = getPaTime() - instruments[instrumentIndex]->sustainedNotes[noteIndex]->startTime;
          instruments[instrumentIndex]->sustainedNotes.erase(noteIndex);
        }
        note->noteIndex = noteIndex;
        note->isHolding = true;
        holdingNotesMidi[noteIndex] = note;
      }
    }
  }

  void midiNoteOff(unsigned char pitch, unsigned char velocity, int channel, int instrumentIndex, int instrumentTrackIndex = -1) {
    if(instrumentIndex >= 0 && instrumentIndex < instruments.size()) {
      int noteIndex = (channel << 8) | pitch;
      if(holdingNotesMidi.count(noteIndex) > 0) {
        Note *note = holdingNotesMidi[noteIndex];
        if(!instruments[instrumentIndex]->isSustainActive) {
          note->isHolding = false;
          note->keyHoldDuration = getPaTime() - note->startTime;
          note->noteActualLength = instruments[instrumentIndex]->getNoteActualLength(*note);
          
          printf("Key released, note full lenght = %f\n", note->noteActualLength);
        }
        else {
          if(instruments[instrumentIndex]->sustainedNotes.count(noteIndex) > 0) {
            instruments[instrumentIndex]->sustainedNotes[noteIndex]->isHolding = false;
            instruments[instrumentIndex]->sustainedNotes[noteIndex]->keyHoldDuration = getPaTime() - instruments[instrumentIndex]->sustainedNotes[noteIndex]->startTime;
            instruments[instrumentIndex]->sustainedNotes.erase(noteIndex);
          }
          instruments[instrumentIndex]->sustainedNotes[noteIndex] = note;
        }
        holdingNotesMidi.erase(noteIndex);
      }
    }
  }




  bool playNote(double pitch, double volume, double duration, double startTime) {
    if(numInstrumentTracks < 1) return false;
    Note note(sampleRate, pitch, getPaTime() + startTime, volume, instrumentTracks[0].instrumentIndex);
    Note *notePlaying = addCurrentlyPlayingNote(note);

    if(notePlaying) {
      notePlaying->noteLength = duration;
      notePlaying->keyHoldDuration = duration;
      notePlaying->noteActualLength = instruments[notePlaying->instrumentIndex]->getNoteActualLength(*notePlaying);
      notePlaying->isHolding = false;
      return true;
    }
    return false;
  }



  Note *startInstrumentNote(double pitch, double volume, int instrumentIndex) {
    if(mode == Mode::Looper && isRecordingMode && !isLooperPlaying) {
      roundLoopTimeNoteValueInverse();
    }
    Note *note = startInstrumentNote(pitch, volume, getLooperTime(), instrumentIndex);

    if(mode == Mode::Looper && isRecordingMode && !isLooperPlaying) {
      if(progressOnNoteStartWhenPaused) progressLoopTimeByNoteValue();
    }
    return note;
  }

  Note *startInstrumentNote(double pitch, double volume, double startTime, int instrumentIndex) {
    Note note(sampleRate, pitch, getPaTime(), volume, instrumentIndex);
    
    return addCurrentlyPlayingNote(note);
  }

  

  Note *startInstrumentTrackNote(double pitch, double volume, int instrumentTrackIndex, double time = -1) {
    if(instruments[instrumentTracks[instrumentTrackIndex].instrumentIndex]->instrumentType == Instrument::InstrumentType::CompositePads) {
      return startDrumPadNote(pitch, volume, instrumentTrackIndex, time);
    }
    
    Note note(sampleRate, pitch, getPaTime(), volume, instrumentTracks[instrumentTrackIndex].instrumentIndex, instrumentTrackIndex);
    //printf("(debugging) at Synth::startInstrumentTrackNote()\n");

    if(mode == Mode::Looper && isRecordingMode) {
      double startTime = (time < 0 ? getLooperTime() : time) + 0.0001;
      note.widthFraction = measuresPerLooperTrack * noteValueInverse;
      note.noteValueInverse = noteValueInverse;
      note.noteLength = (noteValueInverse * measuresPerLooperTrack == 0) ? 0 : looperTrackDuration / (noteValueInverse * measuresPerLooperTrack);
      note.startTimeInMeasures = measuresPerLooperTrack * startTime / looperTrackDuration;
      
      Note *ln = getActiveLooperSequenceTrack()->startNote(note, startTime);
      setNoteSequencerRect(*ln);
      if(recordTrackNotes) {
        recordNote(ln, instrumentTracks[instrumentTrackIndex].instrumentIndex);
      }
      if(!isLooperPlaying && time < 0) {
        if(progressOnNoteStartWhenPaused) progressLoopTimeByNoteValue();
      }
    }
    

    note.volume *= instrumentTracks[instrumentTrackIndex].volume;
    
    return addCurrentlyPlayingNote(note);
  }



  Note *startDrumPadNote(double pitch, double volume, int instrumentTrackIndex, double time = -1) {
    DrumPad *drumPad = dynamic_cast<DrumPad*>(instruments[instrumentTracks[instrumentTrackIndex].instrumentIndex]);
    if(!drumPad) return NULL;
    int pad = (int)pitch - drumPad->pitchOffset;
    
    //printf("1. %f, %f, %d, %f\n", pitch, volume, instrumentTrackIndex, time);
    
    if(!(pad >= 0 && pad < drumPad->numPads)) return NULL;
    
    Note note(sampleRate, drumPad->pads[pad].pitch, getPaTime(), drumPad->pads[pad].volume, drumPad->pads[pad].instrumentIndex);
    //printf("2. %f, %f, %d, %f\n", drumPad->pads[pad].pitch, drumPad->pads[pad].volume, drumPad->pads[pad].instrumentIndex, getPaTime());
    //printf("(debugging) at Synth::startInstrumentTrackNote()\n");

    if(mode == Mode::Looper && isRecordingMode) {
      double startTime = (time < 0 ? getLooperTime() : time) + 0.0001;
      note.padIndex = pad;
      note.widthFraction = measuresPerLooperTrack * noteValueInverse;
      note.noteValueInverse = noteValueInverse;
      note.noteLength = (noteValueInverse * measuresPerLooperTrack == 0) ? 0 : looperTrackDuration / (noteValueInverse * measuresPerLooperTrack);
      note.startTimeInMeasures = measuresPerLooperTrack * startTime / looperTrackDuration;
      //note.startMeasure = floor(measuresPerLooperTrack * startTime / looperTrackDuration);
      //note.startMeasureFraction = fract(measuresPerLooperTrack * startTime / looperTrackDuration);
      
      Note *ln = getActiveLooperSequenceTrack()->startNote(note, startTime + 0.0001);
      setNoteSequencerRect(*ln);
      if(recordTrackNotes) {
        recordNote(ln, drumPad->pads[pad].instrumentIndex);
      }
    }
    
    if(mode == Mode::Looper && isRecordingMode && !isLooperPlaying && time < 0) {
      if(progressOnNoteStartWhenPaused) progressLoopTimeByNoteValue();
    }

    note.volume *= instrumentTracks[instrumentTrackIndex].volume;
    return addCurrentlyPlayingNote(note);
  }
  
  
  
  
  
  Note *addCurrentlyPlayingNote(const Note &note, int deltaFrame = 0, bool isLoopTrackNote = false) {
    if(isLoopTrackNote && note.isRecorded) {
      return addCurrentlyPlayingNotePresampled(note, deltaFrame);
    }
    
    int i = 0;
    double earliestTime = 1e10;
    double earliestTimeHolding = 1e10;
    int numHolding = 0;
    int earliestTimeIndex = 0;
    int earliestTimeHoldingIndex = 0;
    
    for(; i<numCurrentlyPlayingNotes; i++) {
      if(currentlyPlayingNotes[i].volume <= 0) {
        earliestTimeIndex = i;
        break;
      }
      if(currentlyPlayingNotes[i].isHolding) {
        double t = currentlyPlayingNotes[i].startTime;
        if(earliestTimeHolding > t) {
          earliestTimeHolding = t;
          earliestTimeHoldingIndex = i;
          numHolding++;
        }
      }
      else {
        double t = currentlyPlayingNotes[i].startTime + max(currentlyPlayingNotes[i].noteLength, currentlyPlayingNotes[i].keyHoldDuration);
        if(earliestTime > t) {
          earliestTime = t;
          earliestTimeIndex = i;
        }
      }
    }
    i = numHolding == numCurrentlyPlayingNotes ? earliestTimeHoldingIndex : earliestTimeIndex;
    
    currentlyPlayingNotes[i].isInitialized = false;
    currentlyPlayingNotes[i] = note;

    if(isLoopTrackNote) {
      int frame = looperTrackDuration == 0 ? 0 : (int)(getPaTime()/looperTrackDuration);
      currentlyPlayingNotes[i].startTime += (frame+deltaFrame)*looperTrackDuration;
    }

    currentlyPlayingNotes[i].insertTime = 0.001*SDL_GetTicks();
    
    currentlyPlayingNotes[i].prepare(sampleRate);
    
    // FIXME could be done partially beforehand
    instruments[note.instrumentIndex]->initializeNote(currentlyPlayingNotes[i]);
    
    return &currentlyPlayingNotes[i];
  }


  Note *addCurrentlyPlayingNotePresampled(const Note &note, int deltaFrame = 0) {
    int i = 0;
    double earliestTime = 1e10;
    int earliestTimeIndex = 0;
    for(; i<numCurrentlyPlayingNotesPresampled; i++) { 
      if(currentlyPlayingNotesPresampled[i].volume <= 0) {
        earliestTimeIndex = i;
        break;
      }
      double t = (currentlyPlayingNotesPresampled[i].startTime + currentlyPlayingNotesPresampled[i].noteFullLengthSecs);
      
      if(earliestTime > t) {
        earliestTime = t;
        earliestTimeIndex = i;
      }
    }
    i = earliestTimeIndex;

    currentlyPlayingNotesPresampled[i].set(note);
    currentlyPlayingNotesPresampled[i].volume = -1;

    int frame = looperTrackDuration == 0 ? 0 : (int)(getPaTime()/looperTrackDuration);
    currentlyPlayingNotesPresampled[i].startTime += (frame+deltaFrame)*looperTrackDuration;

    currentlyPlayingNotesPresampled[i].insertTime = 0.001 * SDL_GetTicks();
    currentlyPlayingNotesPresampled[i].isInitialized = false;
    currentlyPlayingNotesPresampled[i].prepare(sampleRate);
    
    // FIXME could be done partially beforehand
    instruments[note.instrumentIndex]->initializeNote(currentlyPlayingNotesPresampled[i]);
    
    
    //printf("%d, %d, %f, %f, v %f, t %f, %d\n", frame, deltaFrame, currentlyPlayingNotesPresampled[i].startTime, getPaTime(), currentlyPlayingNotesPresampled[i].volume, currentlyPlayingNotesPresampled[i].noteFullLengthSecs, currentlyPlayingNotesPresampled[i].isRecorded);
    currentlyPlayingNotesPresampled[i].volume = note.volume;
    currentlyPlayingNotesPresampled[i].isReadyToPlayRecorded = true;
    return &currentlyPlayingNotesPresampled[i];
  }







  void recordNote(Note *note, int instrumentIndex) {
    note->isRecorded = false;
    recordingThreads.push_back(std::thread(recordNoteSamples, instruments[instrumentIndex], note, (int)sampleRate, 10));
    //recordNoteSamples(instruments[instrumentTracks[instrumentTrackIndex].instrumentIndex], (Note*)ln, (int)sampleRate, (int)sampleRate * 10);
  }
  
  double trackPreSamplingProgress = 0;
  
  static void recordTrackNotesThread(Synth *synth) {
    //printf("...\n");
    synth->recordNoteTrackActive = true;
    synth->toBeginning();
    synth->isLooperPlaying = false;
    synth->recordNoteTrackRestartRequested = false;
    //int maxSamples = 96000 * 6;
    
    int numNotesRecorded = 0;
    long noteTotalSamples = 0;
    double noteTotalTime = 0;
    
    TicToc totalTimeElapsed;
    int numNotesToBeRecorded = 0;
    
    for(int i=0; i<synth->sequenceLooper.numTracks; i++) {
      for(int k=0; k<synth->sequenceLooper.tracks[i]->notes.size(); k++) {
        if(synth->recordNoteTrackResetInstrumentTrackIndex == -1 || synth->recordNoteTrackResetInstrumentTrackIndex == synth->sequenceLooper.tracks[i]->notes[k].instrumentTrackIndex) {
          if(synth->sequenceLooper.tracks[i]->notes[k].instrumentTrackIndex >= 0 && synth->sequenceLooper.tracks[i]->notes[k].instrumentTrackIndex < synth->numInstrumentTracks && synth->sequenceLooper.tracks[i]->notes[k].volume > 0) {
            synth->sequenceLooper.tracks[i]->notes[k].isRecorded = false;
            numNotesToBeRecorded++;
          }
        }
      }
    }
    
    for(int i=0; i<synth->sequenceLooper.numTracks; i++) {
      for(int k=0; k<synth->sequenceLooper.tracks[i]->notes.size(); k++) {
        if(synth->recordNoteTrackRestartRequested) {
          i = 0;
          k = 0;
          synth->recordNoteTrackRestartRequested = false;
          synth->trackPreSamplingProgress = 0;
          numNotesToBeRecorded = 0;
          numNotesRecorded = 0;
          noteTotalSamples = 0;
          noteTotalTime = 0;
          //for(int j=0; j<synth->sequenceLooper.maxNumTracks; j++) {
          int j = 0;
            for(int p=0; p<synth->sequenceLooper.tracks[j]->notes.size(); p++) {
              if(synth->recordNoteTrackResetInstrumentTrackIndex == -1 || synth->recordNoteTrackResetInstrumentTrackIndex == synth->sequenceLooper.tracks[j]->notes[p].instrumentTrackIndex) {
                if(synth->sequenceLooper.tracks[j]->notes[p].instrumentTrackIndex >= 0 && synth->sequenceLooper.tracks[j]->notes[p].instrumentTrackIndex < synth->numInstrumentTracks && synth->sequenceLooper.tracks[j]->notes[p].volume > 0) {
                  synth->sequenceLooper.tracks[j]->notes[p].isRecorded = false;
                  numNotesToBeRecorded++;
                }
              }
            }
          //}
          totalTimeElapsed.tic();
        }
        
        if(synth->sequenceLooper.tracks[i]->notes[k].instrumentTrackIndex >= 0 && synth->sequenceLooper.tracks[i]->notes[k].instrumentTrackIndex < synth->numInstrumentTracks) {
          if(synth->recordTrackNotes && !synth->sequenceLooper.tracks[i]->notes[k].isRecorded && synth->sequenceLooper.tracks[i]->notes[k].volume > 0) {
            bool recorded = false;
            Instrument *instrument = synth->instruments[synth->instrumentTracks[synth->sequenceLooper.tracks[i]->notes[k].instrumentTrackIndex].instrumentIndex];
            
            // FIXME
            if(instrument->instrumentType == Instrument::InstrumentType::CompositePads) {
              DrumPad *drumPad = dynamic_cast<DrumPad*>(instrument);
              if(drumPad) {
                instrument = synth->instruments[drumPad->pads[synth->sequenceLooper.tracks[i]->notes[k].padIndex].instrumentIndex];
              }
            }
            recorded = recordNoteSamples(instrument, &synth->sequenceLooper.tracks[i]->notes[k], (int)synth->sampleRate, 10);
            
            if(recorded) {
              numNotesRecorded++;
              noteTotalSamples += synth->sequenceLooper.tracks[i]->notes[k].samples.size();
              noteTotalTime += synth->sequenceLooper.tracks[i]->notes[k].noteFullLengthSecs;
              synth->trackPreSamplingProgress = (double)numNotesRecorded/numNotesToBeRecorded;
            }
          }
        }
      }
    }
    
    totalTimeElapsed.toc();
    if(numNotesRecorded > 0) {
      printf("Track notes recorded, n %d, length total/mean %.4f/%.4f, samples total/mean %lu/%lu, percentage recorded %.2f %%, time elapsed total/mean %.4f/%.4f\n", numNotesRecorded, (double)noteTotalSamples/synth->sampleRate, (double)noteTotalSamples/synth->sampleRate/numNotesRecorded, noteTotalSamples, noteTotalSamples/numNotesRecorded, (double)noteTotalSamples/synth->sampleRate/noteTotalTime*100.0, totalTimeElapsed.duration, totalTimeElapsed.duration/numNotesRecorded);
    }
    
    synth->recordNoteTrackActive = false;

  }
  
  
  // when changing instrument, when changin any instrument settings, when changing loop track length/tempo
  void resetRecordedTrackNotes(int instrumentTrackIndex = -1) {
    
    bool wasAny = false;
    for(int i=0; i<sequenceLooper.maxNumTracks; i++) {
      for(int k=0; k<sequenceLooper.tracks[i]->notes.size(); k++) {
        if(instrumentTrackIndex == -1 || instrumentTrackIndex == sequenceLooper.tracks[i]->notes[k].instrumentTrackIndex) {
          if(sequenceLooper.tracks[i]->notes[k].instrumentTrackIndex >= 0 && sequenceLooper.tracks[i]->notes[k].instrumentTrackIndex < numInstrumentTracks) {
            sequenceLooper.tracks[i]->notes[k].isRecorded = false;
            wasAny = true;
          }
        }
      }
    }
    if(!wasAny) {
      return;
    }
    
    recordNoteTrackResetInstrumentTrackIndex = instrumentTrackIndex;
    
    for(int i=0; i<numCurrentlyPlayingNotesPresampled; i++) {
      currentlyPlayingNotesPresampled[i].reset();
    }
    
    if(!recordNoteTrackActive) {
      for(int i=0; i<trackRecordingThreads.size(); i++) {
        trackRecordingThreads[i].join();
      }
      trackRecordingThreads.clear();
      trackRecordingThreads.push_back(std::thread(Synth::recordTrackNotesThread, this));
    }
    else {
      recordNoteTrackRestartRequested = true;
    }
  }



  LooperSequenceTrack *getActiveLooperSequenceTrack() {
    return sequenceLooper.tracks[sequenceLooper.currentTrack];
  }

  LooperSequenceTrack *getLoopTrack(int delta) {
    return sequenceLooper.tracks[(sequenceLooper.currentTrack+delta+sequenceLooper.numTracks)%sequenceLooper.numTracks];
  }



  bool cancelNote() {
    if(mode == Mode::Looper && isRecordingMode) {
      return getActiveLooperSequenceTrack()->cancelNote();
    }
    return 0;
  }


  void stopAllNotes(bool stopStream = true) {
    for(int i=0; i<numCurrentlyPlayingNotes; i++) {
      currentlyPlayingNotes[i].reset();
    }
    for(int i=0; i<numCurrentlyPlayingNotesPresampled; i++) {
      currentlyPlayingNotesPresampled[i].reset();
    }

    for(int i=0; i<instruments.size(); i++) {
      instruments[i]->resetNoteStopRequest();
    }
    if(stopStream) {
      if(started) stop();
      else start();
    }
  }

  // FIXME
  void updateNoteLengths() {
    for(int i=0; i<sequenceLooper.tracks.size(); i++) {
      for(int k=0; k<sequenceLooper.tracks[i]->notes.size(); k++) {
        if(sequenceLooper.tracks[i]->notes[k].volume > 0) {
          sequenceLooper.tracks[i]->notes[k].noteLength = looperTrackDuration / (sequenceLooper.tracks[i]->notes[k].noteValueInverse * measuresPerLooperTrack);
          sequenceLooper.tracks[i]->notes[k].startTime = looperTrackDuration * sequenceLooper.tracks[i]->notes[k].startTimeInMeasures / measuresPerLooperTrack;
        }
      }
    }
  }


  // FIXME
  bool isEditing() {
    return mode == Mode::Looper && isRecordingMode;
  }

  void toggleRecordSequence() {
    isRecordingMode = !isRecordingMode;
  }
  void toggleLoopTracksActive() {
    mode = mode == Mode::Looper ? Mode::Nothing : Mode::Looper;
  }

  void increaseLoops() {
    if(sequenceLooper.numTracks < sequenceLooper.maxNumTracks) {
      sequenceLooper.numTracks++;
    }
  }
  void decreaseLoops() {
    if(sequenceLooper.numTracks > 1) {
      sequenceLooper.numTracks--;
      if(sequenceLooper.currentTrack >= sequenceLooper.numTracks) {
        sequenceLooper.currentTrack = sequenceLooper.numTracks-1;
        sequenceLooper.currentRepeat = sequenceLooper.numRepeats == 0 ? -1 : 0;
        //resetPaTime();
      }
    }
  }
  void increaseLoopRepeats() {
    if(sequenceLooper.numRepeats < 64) {
      sequenceLooper.numRepeats++;
      sequenceLooper.currentRepeat = 0;
      //resetPaTime();
    }
  }
  void decreaseLoopRepeats() {
    if(sequenceLooper.numRepeats > 0) {
      sequenceLooper.numRepeats--;
      sequenceLooper.currentRepeat = sequenceLooper.numRepeats == 0 ? -1 : 0;

      //resetPaTime();
    }
  }
  void toBeginning() {
    sequenceLooper.currentTrack = 0;
    sequenceLooper.currentRepeat = 0;
    looperTime = looperTrackDuration;    
  }
  void previousLoop() {
    sequenceLooper.currentTrack = (sequenceLooper.numTracks+sequenceLooper.currentTrack-1) % sequenceLooper.numTracks;
    sequenceLooper.currentRepeat = sequenceLooper.numRepeats == 0 ? -1 : 0;
    looperTime = looperTrackDuration;
  }
  void nextLoop() {
    sequenceLooper.currentTrack = (sequenceLooper.currentTrack+1) % sequenceLooper.numTracks;
    sequenceLooper.currentRepeat = sequenceLooper.numRepeats == 0 ? -1 : 0;
    looperTime = looperTrackDuration;
  }

  void resetAll() {
    for(int i=0; i<sequenceLooper.maxNumTracks; i++) {
      sequenceLooper.tracks[i]->reset();
    }
  }
  void resetCurrentLoop() {
    getActiveLooperSequenceTrack()->reset();
  }
  void resetCurrentLoop(int instrumentIndex) {
    getActiveLooperSequenceTrack()->reset(instrumentIndex);
  }



  void setLooperTracksPlaying(bool isLooperPlaying) {
    this->isLooperPlaying = isLooperPlaying;
    
    if(isLooperPlaying) {
      setPaTime(looperTime-0.05);
      onStart();
    }
    else {
      //stopAllNotes(false);
    }
    //lastUpdateFrame = 0;
  }

  void onStart() {
    //lastUpdateBeat = (lastUpdateBeat-2+beatsPerLoop)%beatsPerLoop;
    //int beat = (lastUpdateBeat-1+beatsPerLoop)%beatsPerLoop;
    int beat = looperTrackDuration == 0 ? 0 : int((looperTime-loopFrame*looperTrackDuration) / looperTrackDuration * beatsPerLoop);
    onBeat(beat-1);
    onBeat(beat);
    //lastUpdateFrame = -1;
    //lastUpdateBeat = -1;
    //sequenceLooper.currentTrack = 0;
    //sequenceLooper.currentRepeat = 0;
    
    // FIXME
    for(int i=0; i<numAudioRecordingTracks; i++) {
      audioRecordingTracks[i].setTimePostion(looperTime);
      audioRecordingTracks[i].onStart();
    }
  }

  double getLooperTime() {
    int frame = looperTrackDuration == 0 ? 0 : (int)(looperTime/looperTrackDuration);
    return looperTime - frame*looperTrackDuration;
  }


  void onUpdate() {
    for(int i=0; i<instruments.size(); i++) {
      if(instruments[i]->isActive) {
        // FIXME
        for(int k=0; k<instruments[i]->numVoices; k++) {
          if(instruments[i]->voices[k].waveForm.sampleEditor) {
            instruments[i]->voices[k].waveForm.sampleEditor->update(recordingPlaybackBuffer, recordingPlaybackBufferPosition);
          }
        }
      }
      if(instruments[i]->isStopAllNotesRequested()) {
        stopAllNotes(false);
      }
      if(instruments[i]->inactivationRequested && instruments[i]->isActive) {
        bool isInstrumentNotesStillPlaying = false;
        /*for(int k=0; k<currentlyPlayingNotes.size(); k++) {
          if(currentlyPlayingNotes[k].instrumentIndex == i && currentlyPlayingNotes[k].volume > 0.0) {
            isInstrumentNotesStillPlaying = true;
            break;
          }
        }*/
        if(!isInstrumentNotesStillPlaying) {
          instruments[i]->inactivationRequested = false;
          instruments[i]->isActive = false;
        }
      }
    }

    if(mode == Mode::Looper && !recordNoteTrackActive) {
      if(isLooperPlaying) {
        looperTime = getPaTime();
        setTimeWindowWithinLoopProgress();
        for(int i=0; i<numAudioRecordingTracks; i++) {
          audioRecordingTracks[i].onUpdate(recordingPlaybackBuffer, recordingPlaybackBufferPosition);
        }
      }
      loopFrame = looperTrackDuration == 0 ? 0 : (int)(looperTime/looperTrackDuration);

      if(loopFrame != lastUpdateFrame) {
        if(loopFrame > lastUpdateFrame) onLoopChange(1);
        if(loopFrame < lastUpdateFrame) onLoopChange(-1);
        lastUpdateFrame = loopFrame;
      }

      int beat = looperTrackDuration == 0 ? 0 : int((looperTime-loopFrame*looperTrackDuration) / looperTrackDuration * beatsPerLoop);
      if(beat != lastUpdateBeat) {
        lastUpdateBeat = beat;
        onBeat(beat);
      }
    }
    
    if(isPlayingRecordedAudio) {
      recordingPlayback.fillPlaybackBuffer(recordingPlaybackBuffer, recordingPlaybackBufferPosition);
      if(recordingPlayback.status == RecordingPlayback::Finished) {
        isPlayingRecordedAudio = false;
      }
    }
    /*unsigned long a = 0, b = 0;
    for(int i=0; i<numInstrumentTracks; i++) {
      if(instrumentTracks[i].instrumentIndex >= 0) {
        a += instruments[instrumentTracks[i].instrumentIndex]->numA;
        b += instruments[instrumentTracks[i].instrumentIndex]->numB;
      }
    }*/
    /*if(frameCount++ % 200 == 0) {
      printf("%lu, %lu, %f\n", a, b, (double)a/b);
    }*/
    
    updateActiveInstruments();
    
  }
  //long frameCount = 0;

  void onBeat(int beat) {

    if(mode == Mode::Looper) {
      //printf("onBeat %d, loop %d, repeat %d\n", beat, sequenceLooper.currentTrack, sequenceLooper.currentRepeat);
      double minTime = beatsPerLoop == 0 ? 0 : looperTrackDuration/beatsPerLoop*(beat+1);
      double maxTime = beatsPerLoop == 0 ? 0 : looperTrackDuration/beatsPerLoop*(beat+2);
      int deltaFrame = beat == beatsPerLoop-1 ? 1 : 0;
      if(maxTime > looperTrackDuration) {
        minTime -= looperTrackDuration;
        maxTime -= looperTrackDuration;
      }
      int deltaLoopTrack = 0;
      if(sequenceLooper.numTracks > 1 && sequenceLooper.numRepeats > 0 && sequenceLooper.currentRepeat+1 == sequenceLooper.numRepeats && beat+1 == beatsPerLoop) {
        deltaLoopTrack = 1;
      }
      if(isLooperPlaying) {
        LooperSequenceTrack *looperSequenceTrack = getLoopTrack(deltaLoopTrack);
        int n = 0;
        //for(int i=0; i<sequenceLooper.maxNumTrackNotes; i++) {
        for(int i=0; i<looperSequenceTrack->notes.size(); i++) {
          if(looperSequenceTrack->notes[i].volume > 0 && !instrumentTracks[looperSequenceTrack->notes[i].instrumentTrackIndex].isMuted) {
            if(looperSequenceTrack->notes[i].startTime >= minTime && looperSequenceTrack->notes[i].startTime < maxTime) {
              /*if(recordTrackNotes && !looperSequenceTrack->notes[i].isRecorded) {
                recordNote(&looperSequenceTrack->notes[i], instrumentTracks[looperSequenceTrack->notes[i].instrumentTrackIndex].instrumentIndex);
              }*/

              n++;
              //Note &note = looperSequenceTrack->notes[i];
              Note note = looperSequenceTrack->notes[i];
              
              if(looperSequenceTrack->notes[i].instrumentTrackIndex < numInstrumentTracks) {
                if(instruments[instrumentTracks[looperSequenceTrack->notes[i].instrumentTrackIndex].instrumentIndex]->instrumentType == Instrument::InstrumentType::DefaultInstrument) {
                  note.instrumentIndex = instrumentTracks[looperSequenceTrack->notes[i].instrumentTrackIndex].instrumentIndex;
                }
                else if(instruments[instrumentTracks[looperSequenceTrack->notes[i].instrumentTrackIndex].instrumentIndex]->instrumentType == Instrument::InstrumentType::CompositePads) {
                  DrumPad *drumPad = dynamic_cast<DrumPad*>(instruments[instrumentTracks[looperSequenceTrack->notes[i].instrumentTrackIndex].instrumentIndex]);
                  
                  note.instrumentIndex = drumPad->pads[looperSequenceTrack->notes[i].padIndex].instrumentIndex;
                  note.pitch = drumPad->pads[looperSequenceTrack->notes[i].padIndex].pitch;
                  note.volume = note.volume * drumPad->pads[looperSequenceTrack->notes[i].padIndex].volume * instrumentTracks[looperSequenceTrack->notes[i].instrumentTrackIndex].volume;
                }
              }
              Note *notec = addCurrentlyPlayingNote(note, deltaFrame, true);
              notec->volume *= instrumentTracks[looperSequenceTrack->notes[i].instrumentTrackIndex].volume;
              //note.print();
            }
          }
        }
        //printf("notes added %d/%lu\n", n, looperSequenceTrack->notes.size());
      }
    }
  }

  void onLoopChange(int loopChangeDirection = 1) {
    if(sequenceLooper.numRepeats > 0) {
      sequenceLooper.currentRepeat = (sequenceLooper.currentRepeat+sequenceLooper.numRepeats+loopChangeDirection) % sequenceLooper.numRepeats;

      if(sequenceLooper.currentRepeat == sequenceLooper.numRepeats-1) {
        sequenceLooper.currentTrack = (sequenceLooper.currentTrack+loopChangeDirection+sequenceLooper.numTracks) % sequenceLooper.numTracks;
      }
    }
  }

  void setLoopDuration(double time) {
    /*double f = looperTrackDuration == 0 ? 0 : time / looperTrackDuration;
    for(int i=0; i<sequenceLooper.maxNumTracks; i++) {
      for(int k=0; k<sequenceLooper.maxNumTrackNotes; k++) {
        sequenceLooper.tracks[i]->notes[k].startTime *= f;
      }
    }*/
    setPaTime(paTime / looperTrackDuration * time);
    looperTrackDuration = time;
    tempo = 60 * measuresPerLooperTrack * beatsPerMeasure / looperTrackDuration;
    looperTime = looperTrackDuration;
    updateNoteLengths();
    resetRecordedTrackNotes();
    
    fitSequencerView(4);
  }

  void setTempo(double tempo) {
    this->tempo = tempo;
    setLoopDuration(60 * measuresPerLooperTrack * beatsPerMeasure / tempo);
  }


  void setup(double sampleRate, int framesPerBuffer, double suggestedOutputLatency, int screenW = 1, int screenH = 1, int suggestedAudioDevice = -1) {
    PortAudioInterface::setup(sampleRate, framesPerBuffer, suggestedOutputLatency, streamCallbackSynth, suggestedAudioDevice);
    this->screenW = screenW;
    this->screenH = screenH;
  }
  void setup() {
    PortAudioInterface::setup(streamCallbackSynth);
  }

  // FIXME
  void print() {
    if(mode == Mode::Looper) {
      printf("Loop %d.%d/%d.%d, %.02f/%.02f (s), ", sequenceLooper.currentTrack+1, sequenceLooper.currentRepeat+1, sequenceLooper.numTracks, sequenceLooper.numRepeats, getPaTime(),
          looperTrackDuration);
      if(!started) {
        printf("<stopped> ");
      }
      else if(mode == Mode::Looper || !isRecordingMode) {
        printf("<playing> ");
      }
      else {
        printf("<editing> ");
      }
      printf("\n");
    }
    else {
      printf("<playing>\n");
    }
  }

  
  
    Note *getMostRecentNote() {
      float latestNoteTime = -100;
      int latestNoteIndex = 0;
      for(int i=0; i<numCurrentlyPlayingNotes; i++) {
        if(currentlyPlayingNotes[i].startTime > latestNoteTime) {
          latestNoteTime = currentlyPlayingNotes[i].startTime;
          latestNoteIndex = i;
        }
      }
      return &currentlyPlayingNotes[latestNoteIndex];
    }


  void onUpdateGetMidiMessages(MidiInterface &midiInterface) {

  }

  int getMouseHoverScreenKeysPitch(Events &events, int screenW, int screenH) {
    //return (int)mapRoundClamp(screenH-events.mouseY, 0, screenH-1, screenKeyboardMinNote, screenKeyboardMaxNote);
    int p = (int)map(screenH-events.mouseY, 0, screenH-1, screenKeyboardMinNote, screenKeyboardMaxNote);
    p = clamp(p, screenKeyboardMinNote, screenKeyboardMaxNote);
    return p;
  }

  void onMousePressedScreenKeys(Events &events, int screenW, int screenH) {
    //double n = 60 + mapRoundClamp(screenH-events.mouseY, 0, screenH-1, -36, 36);

    if(!screenKeysActive && !isRecordingMode) return;

    //double pitch = mapRoundClamp(screenH-events.mouseY, 0, screenH-1, screenKeyboardMinNote, screenKeyboardMaxNote);
    double pitch = getMouseHoverScreenKeysPitch(events, screenW, screenH);
    if(!isEditing()) {
      if(events.mouseButton >= 0 && events.mouseButton < 3 && holdingNotesScreenKeys[events.mouseButton] == NULL && activeInstrumentTrackIndex >= 0 && activeInstrumentTrackIndex < numInstrumentTracks) {
        holdingNotesScreenKeys[events.mouseButton] = startInstrumentTrackNote(pitch, defaultNoteVolume, activeInstrumentTrackIndex);
        if(holdingNotesScreenKeys[events.mouseButton]) {
          holdingNotesScreenKeys[events.mouseButton]->isHolding = true;
        }
      }
    }
    else {
      if(events.mouseNowDownL && holdingNotesScreenKeys[0] == NULL && events.numModifiersDown == 0) {
        double startTime = floorLoopTime(map(events.mouseX, 0, screenW-1, sequencerTimeWindow.x, sequencerTimeWindow.y));
        holdingNotesScreenKeys[0] = startInstrumentTrackNote(pitch, defaultNoteVolume, activeInstrumentTrackIndex, startTime);
        if(holdingNotesScreenKeys[0]) {
          holdingNotesScreenKeys[0]->isHolding = true;
        }
      }
      if(events.mouseNowDownM) {
        //cancelNote();
      }
      if(events.mouseNowDownR) {
        bool deleteSelectedNotes = false;
        //std::vector<int> notesToBeErased;
        LooperSequenceTrack *looperSequenceTrack = getActiveLooperSequenceTrack();
        for(int i=0; i<looperSequenceTrack->notes.size(); i++) {
          if(looperSequenceTrack->notes[i].sequencerRect.isPointWithin(events.m)) {
            //notesToBeErased.push_back(i);
            if(getActiveLooperSequenceTrack()->notes[i].isSelected) {
              deleteSelectedNotes = true;
            }
            else {
              looperSequenceTrack->resetNote(i);
            }
          }
        }
        /*for(int i=notesToBeErased.size()-1; i>=0; i--) {
          getActiveLooperSequenceTrack()->notes.erase(getActiveLooperSequenceTrack()->notes.begin() + i);
        }*/
        if(deleteSelectedNotes) {
          for(int i=0; i<selectedNotes.size(); i++) {
            selectedNotes[i]->volume = -1;
          }
          selectedNotes.clear();

          for(int i=getActiveLooperSequenceTrack()->notes.size()-1; i>=0; i--) {
            if(getActiveLooperSequenceTrack()->notes[i].volume <= 0) {
              getActiveLooperSequenceTrack()->notes.erase(getActiveLooperSequenceTrack()->notes.begin()+i);
            }
          }
        }
        updateNoteSequencerRects();
      }
    }
  }

  void onMouseReleasedScreenKeys(Events &events, int screenW, int screenH) {
    if(events.mouseButton >= 0 && events.mouseButton < 3 && holdingNotesScreenKeys[events.mouseButton]) {
      holdingNotesScreenKeys[events.mouseButton]->isHolding = false;
      holdingNotesScreenKeys[events.mouseButton]->noteActualLength = instruments[holdingNotesScreenKeys[events.mouseButton]->instrumentIndex]->getNoteActualLength(*holdingNotesScreenKeys[events.mouseButton]);
      //holdingNoteScreenKeys->startTime = getPaTime() - holdingNoteScreenKeys->attackDuration;
      holdingNotesScreenKeys[events.mouseButton]->keyHoldDuration = getPaTime() - holdingNotesScreenKeys[events.mouseButton]->startTime;
      holdingNotesScreenKeys[events.mouseButton] = NULL;
    }
  }
  bool draggingSequencerTimeWindow = false;
  bool draggingSequencerZoom = false;
  bool draggingNoteSelection = false;
  bool draggingNoteSelectionMove = false;
  bool isFittedSequencerView = false;
  
  std::vector<Note*> selectedNotes;
  
  Vec2d noteSelectionCorner1, noteSelectionCorner2;
  Rect noteSelectionRect;

  
  void fitSequencerView(int numMeasures = -1) {

    if(numMeasures > 0) {
      //sequencerTimeWindow.x = (int(sequencerTimeWindow.x / looperTrackDuration * measuresPerLooperTrack) / numMeasures * numMeasures) * (double)looperTrackDuration / measuresPerLooperTrack;
      sequencerTimeWindow.x = 0;
      sequencerZoom = (double)measuresPerLooperTrack / numMeasures;
    }
    else {
      sequencerTimeWindow.x = 0;
      //sequencerTimeWindow.x = round(sequencerTimeWindow.x / looperTrackDuration * measuresPerLooperTrack);
      sequencerZoom = measuresPerLooperTrack / round(measuresPerLooperTrack / sequencerZoom);
    }
    
    sequencerTimeWindow.y = sequencerTimeWindow.x + looperTrackDuration / sequencerZoom;
    isFittedSequencerView = true;
    clampSequencerTimeWindow();
    updateActiveNoteSequencerRects();
  }
  
  void clampSequencerTimeWindow() {
    if(sequencerTimeWindow.x < 0) {
      sequencerTimeWindow.x = 0;
      sequencerTimeWindow.y = looperTrackDuration/sequencerZoom;
    }
    if(sequencerTimeWindow.y > looperTrackDuration) {
      sequencerTimeWindow.x = looperTrackDuration - looperTrackDuration/sequencerZoom;
      sequencerTimeWindow.y = looperTrackDuration;
    }
  }
  
  void onMousePressed(Events &e) {
    if(e.mouseButton == 1 && e.numModifiersDown == 0) {
      draggingSequencerTimeWindow = true;
    }
    if(e.mouseButton == 1 && (e.lControlDown || e.rControlDown)) {
      draggingSequencerZoom = true;
    }
    if(e.mouseButton == 2) {
      //selectedNotes.clear();
      draggingNoteSelection = true;
      noteSelectionCorner1 = e.m;
      noteSelectionRect.set(1, 1, noteSelectionCorner1.x, noteSelectionCorner1.y);
    }
  }
  void onMouseReleased(Events &e) {
    if(draggingNoteSelection) {
      //selectedNotes.clear();
      LooperSequenceTrack *looperSequenceTrack = getActiveLooperSequenceTrack();
      bool clearSelection = noteSelectionRect.w < 2 && noteSelectionRect.h < 2;
      int numNotesAffected = 0;
      
      for(int i=0; i<looperSequenceTrack->notes.size(); i++) {
        if(noteSelectionRect.overlaps(looperSequenceTrack->notes[i].sequencerRect)) {
          if(!looperSequenceTrack->notes[i].isSelected) {
            selectedNotes.push_back(&looperSequenceTrack->notes[i]);
            looperSequenceTrack->notes[i].isSelected = true;
            numNotesAffected++;
          }
          else {
            for(int k=0; k<selectedNotes.size(); k++) {
              if(selectedNotes[k] == &looperSequenceTrack->notes[i]) {
                looperSequenceTrack->notes[i].isSelected = false;
                selectedNotes.erase(selectedNotes.begin()+k);
                numNotesAffected++;
              }
            }
          }
        }
      }
      if(clearSelection && numNotesAffected == 0) {
        for(int i=0; i<looperSequenceTrack->notes.size(); i++) {
          looperSequenceTrack->notes[i].isSelected = false;
        }
        selectedNotes.clear();
      }
    }
    draggingSequencerTimeWindow = false;
    draggingSequencerZoom = false;
    draggingNoteSelection = false;
    draggingNoteSelectionMove = false;
  }
  void onMouseMotion(Events &e) {

    if(draggingNoteSelection) {
      noteSelectionCorner2 = e.m;
      
      double x1 = min(noteSelectionCorner1.x, noteSelectionCorner2.x);
      double x2 = max(noteSelectionCorner1.x, noteSelectionCorner2.x);
      double y1 = min(noteSelectionCorner1.y, noteSelectionCorner2.y);
      double y2 = max(noteSelectionCorner1.y, noteSelectionCorner2.y);
      noteSelectionRect.set(x2-x1, y2-y1, (x1+x2)*0.5, (y1+y2)*0.5);
    }
    if(draggingSequencerTimeWindow) {
      double dt = -(sequencerTimeWindow.y - sequencerTimeWindow.x) * e.md.x / GuiElement::screenW;
      sequencerTimeWindow.x = clamp(sequencerTimeWindow.x + dt, 0, looperTrackDuration-looperTrackDuration/sequencerZoom);
      sequencerTimeWindow.y = clamp(sequencerTimeWindow.y + dt, looperTrackDuration/sequencerZoom, looperTrackDuration);
      updateActiveNoteSequencerRects();
      //printf("time window: %f - %f\n", sequencerTimeWindow.x, sequencerTimeWindow.y);
    }
    if(draggingSequencerZoom) {
      if(e.md.x > 0) sequencerZoom = clamp(sequencerZoom * (1.0 + e.md.x*0.001), 1, 1e10);
      else sequencerZoom = clamp(sequencerZoom / (1.0 - e.md.x*0.001), 1, 1e10);
      sequencerTimeWindow.y = sequencerTimeWindow.x + looperTrackDuration/sequencerZoom;
      isFittedSequencerView = false;
      clampSequencerTimeWindow();
      updateActiveNoteSequencerRects();
    }
  }
  
  void onKeyDownKeyboardPiano(Events &events) {
    if(!isKeyboardPianoActive) return;
    if(events.noModifiers && activeInstrumentTrackIndex >= 0 && activeInstrumentTrackIndex < numInstrumentTracks) {
      int pitch = keyboardPianoInput(events.sdlKeyCode);
      if(pitch > 0 && holdingNotesKeyboard.count(pitch) == 0) {
        Note *note = NULL;
        note = startInstrumentTrackNote(pitch, defaultNoteVolume, activeInstrumentTrackIndex);
        if(note) {
          note->isHolding = true;
          holdingNotesKeyboard[pitch] = note;
        }
        //midiInterface.sendMessage(MidiMessage::NoteOn, n-12, 255);
      }
    }
  }

  void onKeyUpKeyboardPiano(Events &events) {
    int n = keyboardPianoInput(events.sdlKeyCode);

    if(holdingNotesKeyboard.count(n) > 0) {
      Note *note = holdingNotesKeyboard[n];
      note->isHolding = false;
      note->noteActualLength = instruments[note->instrumentIndex]->getNoteActualLength(*note);
      note->keyHoldDuration = getPaTime() - note->startTime;
      holdingNotesKeyboard.erase(n);
    }
  }

  void togglePlay() {
    if(mode == Mode::Looper) {
      setLooperTracksPlaying(!isLooperPlaying);
    }
  }
  void stop() {
    if(mode == Mode::Looper) {
      looperTime = looperTrackDuration;
      sequenceLooper.currentTrack = 0;
      sequenceLooper.currentRepeat = 0;
      setLooperTracksPlaying(false);
    }
  }
  
  void progressSequencerTimeWindowByMeasures(int numMeasures) {
    if(sequencerZoom > 1) {
      sequencerTimeWindow.x += looperTrackDuration / measuresPerLooperTrack * numMeasures;
      sequencerTimeWindow.y = sequencerTimeWindow.x + looperTrackDuration / sequencerZoom;
      clampSequencerTimeWindow();
    }
  }

  void onKeyDownLooperControls(Events &events) {
    if(mode == Mode::Looper) {
      if(!isLooperPlaying) {
        if(events.sdlKeyCode == SDLK_LEFT) {
          if(events.lControlDown || events.rControlDown) progressLoopTimeByMeasures(-1);
          else if(events.lShiftDown || events.rShiftDown) progressLoopTimeByMeasures(-4);
          else progressLoopTimeByMeasureSubdivisions(-1);
        }
        if(events.sdlKeyCode == SDLK_RIGHT) {
          if(events.lControlDown || events.rControlDown) progressLoopTimeByMeasures(1);
          else if(events.lShiftDown || events.rShiftDown) progressLoopTimeByMeasures(4);
          else progressLoopTimeByMeasureSubdivisions(1);
        }
      }
      else {
        if(events.sdlKeyCode == SDLK_LEFT && (events.lShiftDown || events.rShiftDown)) {
          progressSequencerTimeWindowByMeasures(-4);
          fitSequencerView();
        }
        if(events.sdlKeyCode == SDLK_RIGHT && (events.lShiftDown || events.rShiftDown)) {
          progressSequencerTimeWindowByMeasures(4);
          fitSequencerView();
        }
      }
    }
    if(events.sdlKeyCode == SDLK_t) {
      fitSequencerView();
    }
    
    if(events.sdlKeyCode == SDLK_F12) {
      stopAllNotes();
    }
    /*if(events.sdlKeyCode == SDLK_KP_MINUS) {
      decreaseLoops();
    }
    if(events.sdlKeyCode == SDLK_KP_PLUS) {
      increaseLoops();
    }
    if(events.sdlKeyCode == SDLK_KP_DIVIDE) {
      decreaseLoopRepeats();
    }
    if(events.sdlKeyCode == SDLK_KP_MULTIPLY) {
      increaseLoopRepeats();
    }*/
    /*if(events.sdlKeyCode == SDLK_LEFT && (events.lShiftDown || events.rShiftDown)) {
      previousLoop();
    }
    if(events.sdlKeyCode == SDLK_RIGHT && (events.lShiftDown || events.rShiftDown)) {
      nextLoop();
    }*/

    if(events.sdlKeyCode == SDLK_SPACE) {
      if(mode == Mode::Looper) {
        if(!isLooperPlaying) {
          if(events.lControlDown || events.rControlDown) {
            looperTime = looperTrackDuration;
          }
          if(events.lShiftDown || events.rShiftDown) {
            looperTime = looperTrackDuration;
            sequenceLooper.currentTrack = 0;
            sequenceLooper.currentRepeat = 0;
          }
        }
        setLooperTracksPlaying(!isLooperPlaying);
      }
    }
    if(events.sdlKeyCode == SDLK_KP_DECIMAL && events.lControlDown && events.lShiftDown) {
      resetAll();
    }
    if(events.sdlKeyCode == SDLK_KP_0 && ((events.lControlDown && events.lShiftDown) || (events.rControlDown && events.rShiftDown))) {
      resetCurrentLoop();
    }
    if(events.sdlKeyCode == SDLK_KP_0 && ((events.lControlDown && !events.lShiftDown) || (events.rControlDown && !events.rShiftDown))) {
      resetCurrentLoop(activeInstrumentIndex);
    }
    if(events.sdlKeyCode == SDLK_RETURN) {
      toggleRecordSequence();
    }
    if(events.sdlKeyCode == SDLK_BACKSPACE) {
      double r = cancelNote();
      if(mode == Mode::Looper && isRecordingMode && !isLooperPlaying) {
        //progressLoopTime(-1);
        looperTime = floorLoopTime(r);
      }
    }

    if(events.sdlKeyCode == SDLK_DELETE) {
      for(int i=0; i<selectedNotes.size(); i++) {
        selectedNotes[i]->volume = -1;
      }
      selectedNotes.clear();
      
      for(int i=getActiveLooperSequenceTrack()->notes.size()-1; i>=0; i--) {
        if(getActiveLooperSequenceTrack()->notes[i].volume <= 0) {
          getActiveLooperSequenceTrack()->notes.erase(getActiveLooperSequenceTrack()->notes.begin()+i);
          /*int k = i;
          for(; k>=0 && getActiveLooperSequenceTrack()->notes[k].volume <= 0; k--) {}
          getActiveLooperSequenceTrack()->notes.erase(getActiveLooperSequenceTrack()->notes.begin()+k, getActiveLooperSequenceTrack()->notes.begin()+i);
          printf("removed notes from %d to %d\n", i, k);
          i = k;*/
        }
      }
      updateActiveNoteSequencerRects();
    }

    if(events.sdlKeyCode == SDLK_d && events.lControlDown) {
      LooperSequenceTrack *looperSequenceTrack = getActiveLooperSequenceTrack();
      double ta = 1e10, tb = 0;
      for(int i=0; i<selectedNotes.size(); i++) {
        ta = min(ta, selectedNotes[i]->startTime);
        tb = max(tb, selectedNotes[i]->startTime + selectedNotes[i]->noteLength);
        printf("selectedNotes[i]->startTime %f, selectedNotes[i]->noteLength %f\n", selectedNotes[i]->startTime, selectedNotes[i]->noteLength);
      }
      std::vector<Note*> newSelectedNotes;
      for(int i=0; i<selectedNotes.size(); i++) {
        // FIXME Wrap within function
        //printf("Starting note at synth... 1\n");
        //Note trackNote = *selectedNotes[i];
        Note newNote(sampleRate, selectedNotes[i]->pitch, selectedNotes[i]->startTime, selectedNotes[i]->volume, selectedNotes[i]->instrumentIndex, selectedNotes[i]->instrumentTrackIndex);
        //printf("Starting note at synth... 2\n");
        Note *note = looperSequenceTrack->startNote(newNote, selectedNotes[i]->startTime + tb - ta);
        
        //double startTime = (time < 0 ? getLooperTime() : time) + 0.0001;
        note->widthFraction = measuresPerLooperTrack * noteValueInverse;
        note->noteValueInverse = noteValueInverse;
        note->noteLength = (noteValueInverse * measuresPerLooperTrack == 0) ? 0 : looperTrackDuration / (noteValueInverse * measuresPerLooperTrack);
        note->startTimeInMeasures = measuresPerLooperTrack * note->startTime / looperTrackDuration;
        
        //Note *ln = getActiveLooperSequenceTrack()->startNote(note, startTime);
        setNoteSequencerRect(*note);
        if(recordTrackNotes) {
          recordNote(note, note->instrumentIndex);
        }
    
        /*Note *newNote = looperSequenceTrack->startNote(*selectedNotes[i], selectedNotes[i]->startTime + tb - ta);
        newSelectedNotes.push_back(newNote);*/
        newSelectedNotes.push_back(note);
      }
      for(int i=0; i<selectedNotes.size(); i++) {
        selectedNotes[i]->isSelected = false;
      }
      selectedNotes.clear();
      selectedNotes = newSelectedNotes;
      for(int i=0; i<selectedNotes.size(); i++) {
        selectedNotes[i]->isSelected = true;
      }
      updateNoteSequencerRects();
    }
    if(events.sdlKeyCode == SDLK_c && events.lControlDown) {
    }
    if(events.sdlKeyCode == SDLK_x && events.lControlDown) {
    }
    if(events.sdlKeyCode == SDLK_v && events.lControlDown) {
    }
    /*if(events.sdlKeyCode == SDLK_c && events.lControlDown) {
      //if(events.lShiftDown)
      if(sequenceLooper.copyTrack) delete sequenceLooper.copyTrack;
      sequenceLooper.copyTrack = new LooperSequenceTrack(getActiveLooperSequenceTrack());
    }
    if(events.sdlKeyCode == SDLK_x && events.lControlDown) {
    }
    if(events.sdlKeyCode == SDLK_v && events.lControlDown) {
      if(sequenceLooper.copyTrack) {
        LooperSequenceTrack *looperSequenceTrack = getActiveLooperSequenceTrack();
        for(int i=0; i<sequenceLooper.copyTrack->notes.size(); i++) {
          if(sequenceLooper.copyTrack->notes[i].volume > 0) {
            looperSequenceTrack->startNote(sequenceLooper.copyTrack->notes[i]);
          }
        }
      }
    }*/

    if(events.sdlKeyCode == SDLK_KP_8 || events.sdlKeyCode == SDLK_KP_2) {
      double steps = events.sdlKeyCode == SDLK_KP_8 ? 1 : -1;
      int instrumentIndex = -1;
      if(events.lControlDown || events.rControlDown) {
        steps *= 12;
      }
      if(events.lShiftDown || events.rShiftDown) {
        instrumentIndex = activeInstrumentIndex;
      }
      getActiveLooperSequenceTrack()->transposeTrack(steps, instrumentIndex);
      updateNoteSequencerRects();
    }
    if(events.sdlKeyCode == SDLK_KP_4 || events.sdlKeyCode == SDLK_KP_6) {
      double steps = events.sdlKeyCode == SDLK_KP_6 ? 1 : -1;
      steps *= looperTrackDuration / measuresPerLooperTrack / subdivisionsPerMeasure;
      int instrumentIndex = -1;
      /*if(events.lControlDown || events.rControlDown) {
        steps = 12;
      }*/
      if(events.lShiftDown || events.rShiftDown) {
        instrumentIndex = activeInstrumentIndex;
      }
      for(int i=0; i<getActiveLooperSequenceTrack()->notes.size(); i++) {
        if(instrumentIndex == -1 || getActiveLooperSequenceTrack()->notes[i].instrumentIndex == instrumentIndex) {
          if(getActiveLooperSequenceTrack()->notes[i].volume > 0) {
            getActiveLooperSequenceTrack()->notes[i].startTime += steps;
          }
        }
      }
      //getActiveLooperSequenceTrack()->transposeTrack(steps, instrumentIndex);
      updateNoteSequencerRects();
    }

    if(!isKeyboardPianoActive) {
      bool changeNoteValue = !(events.lShiftDown || events.rShiftDown);
      bool changeSubdivisions = !(events.lControlDown || events.rControlDown);
      bool dotted = events.lAltDown || events.rAltDown;
      
      int k = events.sdlKeyCode - SDLK_0;
      if(k >= 0 && k < 11 ) {;
        k--;
        double t = k == -1 ? 1.0 / measuresPerLooperTrack : pow(2.0, k);
        double q = 1.0;
        if(dotted) q = 1.5;
        if(changeNoteValue) noteValueInverse = 1.0/((1.0/t)*q);
        if(changeSubdivisions) subdivisionsPerMeasure = t;
      }
    }



    if(events.sdlKeyCode == SDLK_PAGEUP && events.noModifiers) {
      keyBaseNote = (keyBaseNote+1) % 12;
    }
    if(events.sdlKeyCode == SDLK_PAGEDOWN && events.noModifiers) {
      keyBaseNote = (12+keyBaseNote-1) % 12;
    }
  }





  Vec2d synthStereoPast(double t) {
    return delayLine.getOutputSampleInSeconds(t);
  }
  Vec2d synthStereoPastByIndex(int i) {
    return delayLine.getOutputSample(i);
  }



  static void synthStereo(Vec2d &sampleOut, const Vec2d &sampleIn, double t, double dt, std::vector<Note> &notes, int numNotes, std::vector<Note> &notesPresampled, int numNotesPresampled, DelayLine &delayLine, std::vector<Instrument*> &instruments) {

    for(int i=0; i<instruments.size(); i++) {
      if(instruments[i]->isActive && instruments[i]->instrumentType == Instrument::InstrumentType::DefaultInstrument) {
        instruments[i]->isSynthesisActive = true;
        instruments[i]->initializeSampling();
      }
    }

    for(int i=0; i<numNotes; i++) {
      if(notes[i].volume > 0 && notes[i].startTime <= t && notes[i].instrumentIndex >= 0 && notes[i].instrumentIndex <= instruments.size()) {
        if(instruments[notes[i].instrumentIndex]->isActive && instruments[notes[i].instrumentIndex]->instrumentType == Instrument::InstrumentType::DefaultInstrument) {
          if(notes[i].prepare(delayLine.sampleRate)) {
            printf("4r5tyuj4\n");
          }
          double noteTime = t - notes[i].startTime;
          instruments[notes[i].instrumentIndex]->getSample(instruments[notes[i].instrumentIndex]->sampleOut, sampleIn, dt, noteTime, notes[i]);
        }
      }
    }
    for(int i=0; i<numNotesPresampled; i++) {
      if(notesPresampled[i].volume > 0 && notesPresampled[i].startTime <= t && notesPresampled[i].instrumentIndex >= 0 && notesPresampled[i].instrumentIndex <= instruments.size()) {
        if(instruments[notesPresampled[i].instrumentIndex]->isActive && instruments[notesPresampled[i].instrumentIndex]->instrumentType == Instrument::InstrumentType::DefaultInstrument) {
          if(notesPresampled[i].prepare(delayLine.sampleRate)) {
            printf("4r5tyuj4\n");
          }
          double noteTime = t - notesPresampled[i].startTime;
          instruments[notesPresampled[i].instrumentIndex]->getSample(instruments[notesPresampled[i].instrumentIndex]->sampleOut, sampleIn, dt, noteTime, notesPresampled[i]);
        }
      }
    }

    sampleOut.set(0, 0);

    for(int i=0; i<instruments.size(); i++) {
      instruments[i]->isSynthesisActive = false;
      if(instruments[i]->isActive && !instruments[i]->isStopAllNotesRequested()) {
        if(instruments[i]->instrumentType == Instrument::InstrumentType::DefaultInstrument) {
          instruments[i]->applyFilters();
          sampleOut += instruments[i]->sampleOut;
        }
        else if(instruments[i]->instrumentType == Instrument::InstrumentType::CompositePads) {
          DrumPad *drumPad = dynamic_cast<DrumPad*>(instruments[i]);
          // TODO drumPad->applyFilters();
          
          Vec2d drumPadTotalSample;
          for(int k=0; k<drumPad->numPads; k++) {
            drumPadTotalSample += drumPad->instruments->at(drumPad->pads[k].instrumentIndex)->sampleOut;
          }
          drumPad->delayLine.update(drumPadTotalSample);
        }
      }
    }

  }


  static int streamCallbackSynth(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void *userData)
  {
    float *in = (float *)inputBuffer;
    float *out = (float *)outputBuffer;
    Synth *synth = (Synth*)userData;
    synth->paTime = (double)(synth->bufferCount*framesPerBuffer)/synth->sampleRate;


    Vec2d sampleOut, sampleIn;
    double dt = 1.0/synth->sampleRate;

    for(int i=0; i<framesPerBuffer; i++) {
      double t = (double)(synth->bufferCount*framesPerBuffer+i)/synth->sampleRate;
      sampleOut.set(0, 0);
      sampleIn.set(in[i*2] * synth->inputVolume, in[i*2+1] * synth->inputVolume);

      synth->inputDelayLine.update(sampleIn);
      for(int i=0; i<synth->inputPostProcessingEffects.size(); i++) {
        synth->inputPostProcessingEffects[i]->apply(sampleIn);
      }
      // FIXME this shouldn't be here
      synth->inputDelayLine.buffer[synth->inputDelayLine.bufferPos] = sampleIn;


      while(synth->synthThreadDisabled) {}

      synth->synthThreadExecuting = true;
      synthStereo(sampleOut, sampleIn, t, dt, synth->currentlyPlayingNotes, synth->numCurrentlyPlayingNotes, synth->currentlyPlayingNotesPresampled, synth->numCurrentlyPlayingNotesPresampled, synth->delayLine, synth->instruments);
      synth->synthThreadExecuting = false;
      
      sampleOut += synth->recordingPlaybackBuffer[synth->recordingPlaybackBufferPosition % synth->recordingPlaybackBuffer.size()];
      synth->recordingPlaybackBuffer[synth->recordingPlaybackBufferPosition % synth->recordingPlaybackBuffer.size()].set(0, 0);
      synth->recordingPlaybackBufferPosition++;

      if(synth->playAudioInput) {
        sampleOut += sampleIn;
      }
      
      sampleOut *= synth->masterVolume;

      synth->delayLine.update(sampleOut);
      
      for(int i=0; i<synth->postProcessingEffects.size(); i++) {
        synth->postProcessingEffects[i]->apply(sampleOut);
      }

      if(sampleOut.x > 1 || sampleOut.x < -1) {
        sampleOut.x = Random::getDouble(-0.1, 0.1);
      }
      if(sampleOut.y > 1 || sampleOut.y < -1) {
        sampleOut.y = Random::getDouble(-0.1, 0.1);
      }

      // FIXME this shouldn't be here
      synth->delayLine.buffer[synth->delayLine.bufferPos] = sampleOut;
              
      synth->stereoOscilloscope.update(sampleOut); // could be updated from delayline outside synthesizing thread

      out[i*2] = (float)sampleOut.x;
      out[i*2+1] = (float)sampleOut.y;
    }

    synth->bufferCount++;
    return 0;
  }

  inline bool isScreenKeysVisible() {
    return screenKeysActive || isRecordingMode;
  }

  /*void drawSynthNoteLayer(int screenW, int screenH, GeomRenderer &geomRenderer) {
    if(mode == Mode::Looper) {
      double f = map(getLooperTime(), 0, looperTrackDuration, 0, screenW);
      geomRenderer.texture = NULL;

      if(isScreenKeysVisible()) {
        geomRenderer.strokeColor.set(0.3, 0, 0, 1.0);
        geomRenderer.fillColor.set(0, 0, 0, 0.3);
      }
      else {
        geomRenderer.strokeColor.set(1, 1, 1, 0.8);
        geomRenderer.fillColor.set(1, 1, 1, 0.3);
      }
      geomRenderer.strokeType = 1;
      geomRenderer.strokeWidth = 1.5;
      geomRenderer.drawLine(f, 0, f, screenH);

      if(isScreenKeysVisible()) {
        geomRenderer.strokeColor.set(0, 0, 0, 0.7);
      }

      LooperSequenceTrack *looperSequenceTrack = getActiveLooperSequenceTrack();

      for(int i=0; i<looperSequenceTrack->numNotes; i++) {
        if(looperSequenceTrack->notes[i].volume > 0) {
          double t = looperSequenceTrack->notes[i].noteLength == 0 ? 0 : (getLooperTime() - looperSequenceTrack->notes[i].startTime) / looperSequenceTrack->notes[i].noteLength;
          //printf("%f, %f, %f, %f\n", t, getLooperTime(), looperSequenceTrack->notes[i].startTime, looperSequenceTrack->notes[i].noteLength);

          if(isScreenKeysVisible()) {
            if(t >= 0 && t < 1) {
              geomRenderer.strokeColor.set(0, 0, 0, 0.7 + (1.0-t)*0.3);
              geomRenderer.fillColor.set((1.0-t)*0.8, (1.0-t)*0.8, (1.0-t)*0.8, 0.3 + (1.0-t)*0.55);
            }
            else {
              geomRenderer.strokeColor.set(0, 0, 0, 0.7);
              geomRenderer.fillColor.set(0, 0, 0, 0.3);
            }
          }
          else {
            if(t >= 0 && t < 1) {
              geomRenderer.strokeColor.set(1, 1, 1, 0.6 + (1.0-t)*0.3);
              geomRenderer.fillColor.set(1, 1, 1, 0.15 + (1.0-t)*0.75);
            }
            else {
              geomRenderer.strokeColor.set(1, 1, 1, 0.6);
              geomRenderer.fillColor.set(1, 1, 1, 0.15);
            }
          }

          geomRenderer.drawRect(looperSequenceTrack->notes[i].sequencerRect);
        }
      }
    }
  }

  void updateNoteSequencerRects() {
    for(int i=0; i<sequenceLooper.tracks.size(); i++) {
      for(int k=0; k<sequenceLooper.tracks[i]->notes.size(); k++) {
        if(sequenceLooper.tracks[i]->notes[k].volume > 0) {
          setNoteSequencerRect(sequenceLooper.tracks[i]->notes[k]);
        }
      }
    }
  }

  void setNoteSequencerRect(Note &note) {
    double pitch = note.pitch;
    if(instruments[instrumentTracks[note.instrumentTrackIndex].instrumentIndex]->instrumentType == Instrument::InstrumentType::CompositePads) {
      DrumPad *drumPad = dynamic_cast<DrumPad*>(instruments[instrumentTracks[note.instrumentTrackIndex].instrumentIndex]);
      pitch = drumPad->pitchOffset + note.padIndex;
    }
    double h = (screenKeyboardMaxNote - screenKeyboardMinNote == 0) ? 0 : (double)screenH / (screenKeyboardMaxNote - screenKeyboardMinNote);
    double wg = (double)screenW / (measuresPerLooperTrack * subdivisionsPerMeasure);
    double w;
    double x = map(note.startTime, 0, looperTrackDuration, 0, screenW);
    double y = screenH - map(pitch, screenKeyboardMinNote, screenKeyboardMaxNote, 0, screenH);
    if(note.noteLength > 0) {
      w = looperTrackDuration == 0 ? 0 : note.noteLength / looperTrackDuration * screenW;
    }
    else {
      w = note.widthFraction == 0 ? 0 : (double)screenW / note.widthFraction;
      w = min(w, wg);
    }
    note.sequencerRect.set(w, h, x+w*0.5, y);
  }*/

  void drawSynthNoteLayer(int screenW, int screenH, GeomRenderer &geomRenderer, TextGl &textRenderer) {
    if(mode == Mode::Looper) {
      double f = map(getLooperTime(), sequencerTimeWindow.x, sequencerTimeWindow.y, 0, screenW);
      geomRenderer.texture = NULL;

      geomRenderer.strokeType = 1;
      geomRenderer.strokeWidth = 1.5;

      /*if(isScreenKeysVisible()) {
        geomRenderer.strokeColor.set(0, 0, 0, 0.7);
      }*/

      LooperSequenceTrack *looperSequenceTrack = getActiveLooperSequenceTrack();

      for(int i=0; i<looperSequenceTrack->notes.size(); i++) {
        if(looperSequenceTrack->notes[i].volume > 0) {
          double t = looperSequenceTrack->notes[i].noteLength == 0 ? 0 : (getLooperTime() - looperSequenceTrack->notes[i].startTime) / looperSequenceTrack->notes[i].noteLength;
          //printf("%f, %f, %f, %f\n", t, getLooperTime(), looperSequenceTrack->notes[i].startTime, looperSequenceTrack->notes[i].noteLength);

          /*if(isScreenKeysVisible()) {
            if(t >= 0 && t < 1) {
              geomRenderer.strokeColor.set(0, 0, 0, 0.7 + (1.0-t)*0.3);
              geomRenderer.fillColor.set((1.0-t)*0.8, (1.0-t)*0.8, (1.0-t)*0.8, 0.3 + (1.0-t)*0.55);
            }
            else {
              geomRenderer.strokeColor.set(0, 0, 0, 0.7);
              geomRenderer.fillColor.set(0, 0, 0, 0.3);
            }
          }
          else {*/
            if(looperSequenceTrack->notes[i].isSelected) {
              if(t >= 0 && t < 1) {
                geomRenderer.strokeColor.set(0.5, 0, 0, 0.8 + (1.0-t)*0.2);
                geomRenderer.fillColor.set(0.5, 0, 0, 0.35 + (1.0-t)*0.65);
              }
              else {
                geomRenderer.strokeColor.set(0.5, 0, 0, 0.8);
                geomRenderer.fillColor.set(0.5, 0, 0, 0.35);
              }
            }
            else {
              if(t >= 0 && t < 1) {
                geomRenderer.strokeColor.set(1, 1, 1, 0.6 + (1.0-t)*0.3);
                geomRenderer.fillColor.set(1, 1, 1, 0.15 + (1.0-t)*0.75);
              }
              else {
                geomRenderer.strokeColor.set(1, 1, 1, 0.6);
                geomRenderer.fillColor.set(1, 1, 1, 0.15);
              }
            }
          //}

          geomRenderer.drawRect(looperSequenceTrack->notes[i].sequencerRect);
        }
      }
      
      if(isScreenKeysVisible()) {
        geomRenderer.strokeWidth = 1.5;
        geomRenderer.strokeColor.set(1, 1, 1, 0.35);
        textRenderer.setColor(1, 1, 1, 0.85);
        //geomRenderer.fillColor.set(0, 0, 0, 0.2);
        int k = (int)(sequencerTimeWindow.x/looperTrackDuration*measuresPerLooperTrack) / 4 * 4;
        for(int i=k; ; i+=4) {
          double t = map(i, 0, measuresPerLooperTrack, 0, looperTrackDuration);
          if(t < sequencerTimeWindow.x || i == 0) continue;
          if(t >= sequencerTimeWindow.y) break;
          double x = map(t, sequencerTimeWindow.x, sequencerTimeWindow.y, 0, GuiElement::screenW-1);
          geomRenderer.drawLine(x, 0, x, GuiElement::screenH-1);
          textRenderer.print(std::to_string(i), x + 10, GuiElement::screenH - 19, 10);
        }
        geomRenderer.strokeColor.set(1, 1, 1, 0.55);
        double y = screenH-map(60.5, screenKeyboardMinNote+0.5, screenKeyboardMaxNote+0.5, 0, screenH);
        geomRenderer.drawLine(0, y, GuiElement::screenW-1, y);
      }
      if(sequencerZoom > 1) {
        double w1 = 90;
        double w2 = max(w1 / sequencerZoom, 2);
        double h = 40;
        double x1 = GuiElement::screenW - w1 - 30;
        double x2 = x1 + map(sequencerTimeWindow.x, 0, looperTrackDuration, 0, w1);
        double y = GuiElement::screenH - h - 5;

        geomRenderer.strokeWidth = 1.5;
        /*if(isScreenKeysVisible()) {
          geomRenderer.strokeColor.set(0, 0, 0, 0.5);
          geomRenderer.fillColor.set(0, 0, 0, 0.2);
        }
        else {*/
          geomRenderer.strokeColor.set(0.65, 0.65, 0.65, 0.5);
          geomRenderer.fillColor.set(0.65, 0.65, 0.65, 0.2);
        //}
        geomRenderer.drawRectCorner(w1, h, x1, y);
        
        geomRenderer.strokeColor.set(1, 1, 1, 0.6);
        geomRenderer.fillColor.set(1, 1, 1, 0.3);
        geomRenderer.drawRectCorner(w2, h, x2, y);
      }
      if(draggingNoteSelection) {
        geomRenderer.strokeWidth = 1.5;
        geomRenderer.strokeColor.set(1, 1, 1, 0.6);
        geomRenderer.fillColor.set(1, 1, 1, 0.1);
        geomRenderer.drawRect(noteSelectionRect);
      }
      if(isScreenKeysVisible()) {
        geomRenderer.strokeColor.set(0.3, 0, 0, 1.0);
        geomRenderer.fillColor.set(0, 0, 0, 0.3);
      }
      else {
        geomRenderer.strokeColor.set(1, 1, 1, 0.8);
        geomRenderer.fillColor.set(1, 1, 1, 0.3);
      }
      geomRenderer.strokeType = 1;
      geomRenderer.strokeWidth = 1.5;
      geomRenderer.drawLine(f, 0, f, screenH);
    }
    

  }

  void updateNoteSequencerRects() {
    for(int i=0; i<sequenceLooper.tracks.size(); i++) {
      for(int k=0; k<sequenceLooper.tracks[i]->notes.size(); k++) {
        if(sequenceLooper.tracks[i]->notes[k].volume > 0) {
          setNoteSequencerRect(sequenceLooper.tracks[i]->notes[k]);
        }
      }
    }
  }
  void updateActiveNoteSequencerRects() {
    for(int k=0; k<sequenceLooper.tracks[sequenceLooper.currentTrack]->notes.size(); k++) {
      if(sequenceLooper.tracks[sequenceLooper.currentTrack]->notes[k].volume > 0) {
        setNoteSequencerRect(sequenceLooper.tracks[sequenceLooper.currentTrack]->notes[k]);
      }
    }
  }

  void setNoteSequencerRect(Note &note) {
    double pitch = note.pitch;
    if(instruments[instrumentTracks[note.instrumentTrackIndex].instrumentIndex]->instrumentType == Instrument::InstrumentType::CompositePads) {
      DrumPad *drumPad = dynamic_cast<DrumPad*>(instruments[instrumentTracks[note.instrumentTrackIndex].instrumentIndex]);
      pitch = drumPad->pitchOffset + note.padIndex;
    }
    double h = (screenKeyboardMaxNote - screenKeyboardMinNote == 0) ? 0 : (double)screenH / (screenKeyboardMaxNote - screenKeyboardMinNote);
    double wg = (double)screenW / (measuresPerLooperTrack * subdivisionsPerMeasure);
    double w;
    double x = map(note.startTime, sequencerTimeWindow.x, sequencerTimeWindow.y, 0, screenW);
    double y = screenH - (map(pitch, screenKeyboardMinNote, screenKeyboardMaxNote, 0, screenH) + h * 0.5);
    if(note.noteLength > 0) {
      w = looperTrackDuration == 0 ? 0 : note.noteLength / (sequencerTimeWindow.y-sequencerTimeWindow.x) * screenW;
    }
    else {
      w = note.widthFraction == 0 ? 0 : (double)screenW / note.widthFraction;
      w = min(w, wg);
    }
    note.sequencerRect.set(w, h, x+w*0.5, y);
  }







  void setShaderUniforms(GlShader &shader) {
    /*std::vector<float> pitches = std::vector<float>(96, 0);
    std::vector<float> volumes = std::vector<float>(96, 0);
    std::vector<float> startTimes = std::vector<float>(96, 0);
    std::vector<int> isKeyHoldings = std::vector<int>(96, 0);
    int latestNoteIndex = 0;
    float latestNoteTime = -100;
    int numNotes = 0;
    
    for(int i=0; i<numCurrentlyPlayingNotes && numNotes < 96; i++) {
      Note *note = &currentlyPlayingNotes[i];
      bool started = getPaTime() >= note->startTime;
      bool playing = getPaTime() < note->startTime + note->noteActualLength;
      if(note->volume > 0 && (note->isHolding || (started && playing))) {          
        pitches[numNotes] = note->pitch;
        volumes[numNotes] = note->volume;
        startTimes[numNotes] = note->startTime;
        isKeyHoldings[numNotes] = note->isHolding;
        numNotes++;
      }
    }
    for(int i=0; i<numCurrentlyPlayingNotesPresampled && numNotes < 96; i++) {
      Note *note = &currentlyPlayingNotesPresampled[i];
      bool started = getPaTime() >= note->startTime;
      bool playing = getPaTime() < note->startTime + note->noteActualLength;
      
      if(note->volume > 0 && (note->isHolding || (started && playing))) {          
        pitches[numNotes] = note->pitch;
        volumes[numNotes] = note->volume;
        startTimes[numNotes] = note->startTime;
        isKeyHoldings[numNotes] = note->isHolding;
        numNotes++;
      }
    }
    printf("num notes %d\n", numNotes);
    shader.setUniform1i("numNotes", numNotes);
    shader.setUniform1fv("notes", 96, pitches.data());
    shader.setUniform1fv("noteVolumes", 96, volumes.data());
    shader.setUniform1fv("noteStartTimes", 96, startTimes.data());
    shader.setUniform1iv("isKeyHoldings", 96, isKeyHoldings.data());
    shader.setUniform1i("latestNoteIndex", latestNoteIndex);
*/
    shader.setUniform1i("measuresPerLooperTrack", measuresPerLooperTrack);
    shader.setUniform1i("subdivisionsPerMeasure", (int)subdivisionsPerMeasure);
    shader.setUniform1i("screenKeyboardMinNote", screenKeyboardMinNote);
    shader.setUniform1i("screenKeyboardMaxNote", screenKeyboardMaxNote);
    shader.setUniform1i("keyBaseNote", keyBaseNote);
    shader.setUniform1i("showMeasures", isEditing());
    shader.setUniform1i("isBackgroundGraphVisible", isBackgroundGraphVisible);

    shader.setUniform2f("timeWindow", sequencerTimeWindow);
    shader.setUniform1f("trackLenght", looperTrackDuration);

  }


  // FIXME reverb saving elsewhere
  enum FileIOMode { ActiveInstrumentFIO, ReverbFIO, AllInstrumentsFIO,  EverythingFIO };
  FileIOMode fileIOMode = ActiveInstrumentFIO;
  
  /*bool saveEffectToFile(PostProcessingEffect *postProcessingEffect, const std::string &filename) {
    createDirectories(defaultEffectPath);
    EffectFileIO efio;
    postProcessingEffect->presetName = filename;
    efio.write(postProcessingEffect);
    //efio.print();
    return efio.save(defaultEffectPath + filename);
    return false;
  }*/
  
  bool saveReverbToFile(const std::string &filename) {
    createDirectories(defaultEffectPath);
    fileIOMode = ReverbFIO;
    reverb->presetName = filename;
    
    return save(defaultEffectPath + filename);
  }
  
  bool saveInstrumentToFile(const std::string &filename) {
    if(numInstrumentTracks >= 0 && activeInstrumentTrackIndex < numInstrumentTracks) {
      fileIOMode = ActiveInstrumentFIO;
      createDirectories(defaultInstrumentPath);
      //return instruments[instrumentTracks[activeInstrumentTrackIndex].instrumentIndex]->save(defaultInstrumentPath + filename);
      return save(defaultInstrumentPath + filename);
    }
    return false;
  }
  
  bool loadInstrumentFromFile(const std::string &filename, bool isAbsolutePath = false) {
    std::string path = isAbsolutePath ? filename : defaultInstrumentPath + filename;
    TicToc ticToc;
    //Instrument *instrument = new Instrument(sampleRate);
    fileIOMode = ActiveInstrumentFIO;
    
    if(open(path)) {
      parseTimeTotal += parseTimeLatest = ticToc.toc();
      //instruments.push_back(instrument);
      latestLoadedFilename = extractFilename(filename);
      //numInstrumentsLoaded++;
      
      printLoadingTimes();
      return true;
    }
    return false;
  }
  
  virtual std::string getBlockName() {
    return "synth";
  }

  virtual void encodeParameters() {
    clearParameters();
    
    if(fileIOMode == ActiveInstrumentFIO) {
      instruments[instrumentTracks[activeInstrumentTrackIndex].instrumentIndex]->encodeParameters();
    }
    if(fileIOMode == ReverbFIO) {
      reverb->encodeParameters();
    }
  }
  
  virtual void decodeParameters() {
  }

  virtual void onPrintParameters(std::string &content, int level = 0) {
    if(fileIOMode == ActiveInstrumentFIO) {
      instruments[instrumentTracks[activeInstrumentTrackIndex].instrumentIndex]->printParameters(content, level);
    }
    if(fileIOMode == ReverbFIO) {
      reverb->printParameters(content, level);
    }
  }

  virtual int onNewBlock(const std::string &blockName, int blockStartInd, int blockLevel) {
    if(blockName == Instrument::getClassName()) {
      Instrument *instrument = new Instrument(sampleRate, framesPerBuffer);
      instruments.push_back(instrument);
      return instrument->parse(content, blockStartInd, blockLevel);
    }
    if(blockName == DrumPad::getClassName()) {
      DrumPad *drumPad = new DrumPad(&instruments);
      instruments.push_back(drumPad);
      return drumPad->parse(content, blockStartInd, blockLevel);
    }
    if(blockName == Reverb::getClassName()) {
      Preset reverbPreset;
      int blockSize = reverbPreset.parse(content, blockStartInd, blockLevel);
      
      std::string presetName;
      reverbPreset.getStringParameter("presetName", presetName);
      if(presetName.size() == 0) {
        std::string typeName;
        reverbPreset.getStringParameter("type", typeName);
        presetName = typeName;
        int n = 1;
        for(int i=0; i<reverbPresets.size(); i++) {
          if(presetName.compare(0, presetName.size(), reverbPresets[i].name, 0, presetName.size()) == 0) {
            n++;
          }
        }
        presetName += "(" + std::to_string(n) + ")";
      }
      reverbPreset.name = presetName;
      
      reverbPresets.push_back(reverbPreset);
      return blockSize;
    }
    return 0;
  }



  void loadEffectsPresets() {

    Directory dir(defaultEffectPath);
    std::vector<std::string> files;
    dir.getFilesRecursively(files);

    for(int i=0; i<files.size(); i++) {
      open(files[i]);
    }

  }

  /*void loadEffectsPresets() {
    //if(path.compare("*") == 0) {
      std::string path = "";
    //}

    if(isDirectory(defaultEffectPath + path)) {
      Directory dir(defaultEffectPath + path);
      std::vector<std::string> files;
      dir.getFilesRecursively(files);

      for(int i=0; i<files.size(); i++) {
        loadEffectPresetsFromFile(files[i], true);
      }
    }
    else {
      loadEffectPresetsFromFile(path);
    }

  }
  
  bool loadEffectPresetsFromFile(const std::string &filename, bool isAbsolutePath = false) {
    EffectFileIO efio;
    std::string path = isAbsolutePath ? filename : defaultEffectPath + filename;
    bool opened = efio.open(path);
    if(opened) {
      for(int i=0; i<efio.effectStrings.size(); i++) {
        //postProcessingEffectPresets.push_back(efio.effectStrings[i]);
        if(efio.effectStrings[i]->typeName.compare("reverb") == 0) {
          //reverbPresets.push_back(efio.effectStrings[i]);
          std::string presetName = "";
          if(efio.effectStrings[i]->presetName.size() > 0) {
            presetName = efio.effectStrings[i]->presetName;
          }
          else if(efio.effectStrings[i]->subtypeName.size() > 0) {
            presetName = efio.effectStrings[i]->subtypeName;
            int n = 1;
            for(int i=0; i<reverbPresets.size(); i++) {
              if(presetName.compare(0, presetName.size(), reverbPresets[i].name, 0, presetName.size()) == 0) {
                n++;
              }
              presetName += "(" + std::to_string(n) + ")";
            }
          }
          else {
            presetName = "<SOMETHING_WENT_WRONG>";
          }
            
          //reverbPresetNames.push_back(presetName);
          
          Preset reverbPreset;
          reverbPreset.name = presetName;
          reverbPreset.doubleParameters = efio.effectStrings[i]->doubleParameters;
          reverbPreset.stringParameters = efio.effectStrings[i]->stringParameters;
          reverbPresets.push_back(reverbPreset);
        }
      }
      return true;
    }
    return false;
  }*/
  

  
  struct SynthMidiListener : public MidiMessageListener {
    Synth *synth;
    SynthMidiListener(Synth *synth) {
      this->synth = synth;
    }
    virtual void onMessage(const MidiMessage &midiMessage, MidiPortType midiPortType) {
      printf("SynthMidiListener::onMessage(), beginning\n");
    
      midiMessage.print();
      
      if(synth->numInstrumentTracks < 1) return;
      
      /*HuiMessage huiMessage;
      int size = huiMessage.decode(midiMessages, i);
      if(huiMessage.type != HuiMessage::Type::Unrecognised) {
        if(huiMessage.type == HuiMessage::Type::PlayPressed) {
          synth->togglePlay();
        }
        if(huiMessage.type == HuiMessage::Type::StopPressed) {
          synth->stop();
        }
        if(huiMessage.type == HuiMessage::Type::RecordPressed) {
          synth->toggleRecordSequence();
        }
        if(huiMessage.type == HuiMessage::Type::LeftPressed) {
          synth->progressLoopTimeByMeasureSubdivisions(-1);
        }
        if(huiMessage.type == HuiMessage::Type::RightPressed) {
          synth->progressLoopTimeByMeasureSubdivisions(1);
        }
        if(huiMessage.type == HuiMessage::Type::DownPressed) {
          synth->progressLoopTimeByMeasures(-1);
        }
        if(huiMessage.type == HuiMessage::Type::UpPressed) {
          synth->progressLoopTimeByMeasures(1);
        }
        i += size - 1;
        continue;
      }*/

      int midiPortIndex = midiMessage.midiPortIndex;
      int channel = midiMessage.getMessageChannel() + 1;

      std::vector<int> instrumentTrackIndices;
      for(int k=0; k<synth->numInstrumentTracks; k++) {
        if(channel != 0 && (channel == synth->instrumentTracks[k].midiInChannel || synth->instrumentTracks[k].midiInChannel == 0) && midiPortIndex == synth->instrumentTracks[k].midiPortIndex) {
          instrumentTrackIndices.push_back(k);
        }
      }

      if(instrumentTrackIndices.size() != 0) {
        std::set<int> instrumentIndices;
        for(int k=0; k<instrumentTrackIndices.size(); k++) {
          instrumentIndices.insert(synth->instrumentTracks[instrumentTrackIndices[k]].instrumentIndex);
        }

        if(midiMessage.getMessageType() == MidiMessage::ControlChange) {
          if(midiMessage.dataByte1 == MidiMessage::SustainPedal) {
            for(int inin : instrumentIndices) {
              synth->setSustain(midiMessage.dataByte2 >= 64, inin);
            }
          }
        }

        bool noteOffWithNoteOn = false;

        if(midiMessage.getMessageType() == MidiMessage::NoteOn) {
          if(midiMessage.dataByte2 > 0) {
            int pitch = midiMessage.dataByte1;
            /*if(earTrainer.isActive) {
              earTrainer.guessPitch(pitch);
            }
            else {*/
              for(int k=0; k<instrumentTrackIndices.size(); k++) {
                synth->midiTrackNoteOn(midiMessage.dataByte1, midiMessage.dataByte2, midiPortIndex, instrumentTrackIndices[k]);
              }
            //}
          }
          else {
            noteOffWithNoteOn = true;
          }
        }
        if(noteOffWithNoteOn || midiMessage.getMessageType() == MidiMessage::NoteOff) {
          for(int k=0; k<instrumentTrackIndices.size(); k++) {
            synth->midiTrackNoteOff(midiMessage.dataByte1, midiMessage.dataByte2, midiPortIndex, instrumentTrackIndices[k]);
          }
        }
      }
      printf("SynthMidiListener::onMessage(), end\n");
    }
    
  };
  SynthMidiListener *synthMidiListener = NULL;
  
  MidiMessageListener *getMidiMessageListener() {
    return synthMidiListener ? synthMidiListener : synthMidiListener = new SynthMidiListener(this);
  }
};
