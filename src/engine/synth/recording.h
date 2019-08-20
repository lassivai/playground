#pragma once

#include <vector>
#include "../mathl.h"
#include "delayline.h"
#include "filter/postprocessingeffect.h"
#include "filter/eq.h"
/* TODO
 * - combine these two classes, and also encapsulate file i/o
 */

struct AudioRecording {
  std::vector<Vec2d> samples;
  int sampleRate;
  long seeker = 0;
  long numVariableSamples = 0;
  bool tapeFull = false;
  long previousRecordingBufferPosition = 0;
  bool normalize = true;
  long numSamples = 0;
  
  
  bool allowTimeExtension = false;
  long timeExtensionSize = 1000;

  

  void initialize(const DelayLine &delayLine) {
    initialize(-1, delayLine);
  }

  void initialize(double maxLengthInSeconds, const DelayLine &delayLine) {
    this->sampleRate = (int)delayLine.sampleRate;
    
    if(maxLengthInSeconds >= 0) {
      allowTimeExtension = false;
      this->numSamples = (long)(maxLengthInSeconds*this->sampleRate);
    }
    else {
      allowTimeExtension = true;
      this->numSamples = 10000;
    }
    
    samples.assign(numSamples, Vec2d::Zero);
    seeker = 0;
    tapeFull = false;
    previousRecordingBufferPosition = delayLine.bufferPos;
    numVariableSamples = 0;
  }



  double getPositionInSeconds() {
    return (double)seeker / sampleRate;
  }
  
  double getLengthInSeconds() {
    return (double)numVariableSamples / sampleRate;
  }
  
  void setTimePostion(double time) {
    seeker = (long)(time * sampleRate);
  }    

  inline void putSample(const Vec2d &sample) {
    if(seeker >= samples.size()) {
      if(allowTimeExtension) {
        samples.resize(samples.size() + timeExtensionSize);
      }
      else {
        tapeFull = true;
        return;
      }
    }
    
    this->samples[seeker] = sample;
    numVariableSamples = max(numVariableSamples, seeker);
    
    seeker++;
  }

  inline void update(const DelayLine &delayLine) {
    long currentRecordingBufferPosition = delayLine.bufferPos;
    if(currentRecordingBufferPosition == previousRecordingBufferPosition || tapeFull) {
      return;
    }
    
    long n;
    if(currentRecordingBufferPosition > previousRecordingBufferPosition) {
      n = currentRecordingBufferPosition - previousRecordingBufferPosition;
    }
    else {
      n = delayLine.bufferSize - previousRecordingBufferPosition + currentRecordingBufferPosition;
    }
    
    //timeExtensionSize = max(1000, n * 10);
    
    for(long i=0; i<n; i++) {
      long pos = (i + previousRecordingBufferPosition) % delayLine.bufferSize;
      putSample(delayLine.buffer[pos]);
    }
    previousRecordingBufferPosition = currentRecordingBufferPosition;
  }
  

};


struct RecordingPlayback {
  
  enum Status { Uninitialized, Initialized, Playing, Finished };
  Status status = Uninitialized;
  
  std::vector<Vec2d> *recording;
  long previousPlaybackBufferPosition = 0;
  long previousPlaybackBufferIndex = 0;
  long recordingBufferPosition = 0;
  long numSamplesBefore;
  long numSamplesAfter;
  long start, end;
  int sampleRate;


  void toBeginning() {
    //previousPlaybackBufferPosition = 0;
    //previousPlaybackBufferIndex = 0;
    recordingBufferPosition = 0;
    status = Initialized;
  }
  
  void setTimePostion(double time) {
    //previousPlaybackBufferPosition = (long)(time * sampleRate);
    //previousPlaybackBufferIndex = 0;
    recordingBufferPosition = (long)(time * sampleRate);
    status = Initialized;
  }

  void initialize(std::vector<Vec2d> *recording, int frameSize, int sampleRate, double start = -1, double end = -1, long numSamplesBefore = -1, long numSamplesAfter = -1) {
    this->recording = recording;
    this->numSamplesBefore = numSamplesBefore >= 0 ? numSamplesBefore : frameSize*2;
    this->numSamplesAfter = numSamplesAfter >= 0 ? numSamplesAfter : frameSize*20;
    this->start = start >= 0 && start*sampleRate < recording->size() ? long(start*sampleRate) : 0;
    this->end = end >= 0 && end >= start && end*sampleRate <= recording->size() ? long(end*sampleRate) : recording->size();
    this->sampleRate = sampleRate;
    status = Initialized;
    recordingBufferPosition = this->start;
  }
  
  void reset() {
    this->start = 0;
    this->end = recording->size();
    status = Initialized;
    //recordingBufferPosition = this->start;
  }

  /*void reset() {
    this->start = 0;
    this->end = recording->size();
    status = Initialized;
    //recordingBufferPosition = this->start;
  }*/

  double getPositionInSeconds() {
    return (double)recordingBufferPosition / sampleRate;
  }

  void fillPlaybackBuffer(std::vector<Vec2d> &playbackBuffer, long playbackBufferPosition, double volume = 1.0, std::vector<PostProcessingEffect*> *postProcessingEffects = NULL) {
    if(status == Initialized) {
      status = Playing;
      previousPlaybackBufferPosition = playbackBufferPosition;
      previousPlaybackBufferIndex = playbackBufferPosition + numSamplesBefore;
    }
    if(status != Playing) {
      return;
    }
    
    long k = min(playbackBufferPosition - previousPlaybackBufferPosition, playbackBuffer.size()/10);

    //printf("%lu %lu %lu %lu %lu %lu %lu %lu\n", recording->size(), playbackBuffer.size(), playbackBufferPosition, previousPlaybackBufferIndex, recordingBufferPosition, n, start, end);
    
    while(true) {
      if(recordingBufferPosition >= end) {
        status = Finished;
        return;
      }
      Vec2d sample = recording->at(recordingBufferPosition);
      
      if(postProcessingEffects) {
        for(int i=0; i<postProcessingEffects->size(); i++) {
          postProcessingEffects->at(i)->apply(sample);
        }
      }
      
      playbackBuffer[(previousPlaybackBufferIndex++) % playbackBuffer.size()] += sample * volume;
      
      recordingBufferPosition++;
      if(previousPlaybackBufferIndex >= playbackBufferPosition + k + numSamplesAfter) {
        break;
      }
    }
    previousPlaybackBufferPosition = playbackBufferPosition;
  }
};