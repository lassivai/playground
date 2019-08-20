#pragma once

#include <vector>
#include <ctgmath>


struct DelayLine {
  double sampleRate;
  double bufferLength = 1;
  std::vector<Vec2d> buffer;
  std::vector<Vec2d> bufferRaw;
  std::vector<Vec2d> bufferFeedForward;
  
  int bufferSize = 0, bufferPos = 0;

  void reset() {
    buffer.assign(buffer.size(), Vec2d::Zero);
    bufferRaw.assign(bufferRaw.size(), Vec2d::Zero);
    bufferFeedForward.assign(bufferFeedForward.size(), Vec2d::Zero);
  }
  
  DelayLine() {}
  
  DelayLine(double sampleRate, double bufferLength) {
    this->sampleRate = sampleRate;
    this->bufferLength = bufferLength;
    bufferSize = (int)(bufferLength*sampleRate);
    bufferPos = 0;
    buffer.resize(bufferSize);
    bufferRaw.resize(bufferSize);
    bufferFeedForward.resize(bufferSize);
  }

  void init(double sampleRate, double bufferLength) {
    this->sampleRate = sampleRate;
    this->bufferLength = bufferLength;
    bufferSize = (int)(bufferLength*sampleRate);
    bufferPos = 0;
    buffer.resize(bufferSize);
    bufferRaw.resize(bufferSize);
    bufferFeedForward.resize(bufferSize);
  }

  /*inline void update(double sample) {
    bufferPos++;
    if(bufferPos >= bufferSize) {
      bufferPos = 0;
    }

    buffer[bufferPos] = sample;
    bufferRaw[bufferPos] = sample;
  }*/
  inline void update(Vec2d &sample) {
    bufferPos++;
    if(bufferPos >= bufferSize) {
      bufferPos = 0;
    }
    sample += bufferFeedForward[bufferPos];
    buffer[bufferPos] = sample;
    bufferRaw[bufferPos] = sample;
    bufferFeedForward[bufferPos].set(0, 0);
  }

  // FIXME
  inline void feedForward(const Vec2d &sample, long delay) {
    bufferFeedForward[(bufferPos + delay + bufferSize*100) % bufferSize] += sample;
  }
  inline void feedForward(double left, double right, long delay) {
    bufferFeedForward[(bufferPos + delay + bufferSize*100) % bufferSize].add(left, right);
  }
  inline void feedForwardInSeconds(const Vec2d &sample, double delayInSeconds) {
    int delay = (int)(delayInSeconds * sampleRate);
    bufferFeedForward[(bufferPos + delay + bufferSize*100) % bufferSize] += sample;
  }

  inline Vec2d getCurrentSample() {
    return buffer[bufferPos];
  }
  
 // FIXME
  inline Vec2d getOutputSampleInSeconds(double delayInSeconds) {
    int delay = (int)(delayInSeconds * sampleRate);
    return buffer[(bufferPos - delay + bufferSize*100) % bufferSize];
  }

  inline Vec2d getOutputSample(long delay) {
    return buffer[(bufferPos - delay + bufferSize*100) % bufferSize];
  }

  inline Vec2d getOutputSampleWithInterpolation(double delay) {
    delay *= sampleRate;
    long delay1 = long(delay);
    long delay2 = delay1 + 1;
    double f = delay - delay1;
    Vec2d a1 = buffer[(bufferPos - delay1 + bufferSize*100) % bufferSize];
    Vec2d a2 = buffer[(bufferPos - delay2 + bufferSize*100) % bufferSize];
    return a1 * (1.0-f) + a2 * f;
  }

  inline Vec2d getInputSample(long delay) {
    return bufferRaw[(bufferPos - delay + bufferSize*100) % bufferSize];
  }

  Vec2d getRMS(double lenghtInSeconds) {
    int length = (int)(lenghtInSeconds * sampleRate);
    Vec2d rms;
    if(length == 0) return rms;
    for(int i=0; i<length; i++) {
      Vec2d r = getOutputSample(i);
      r.x = r.x * r.x;
      r.y = r.y * r.y;
      rms += r;
    }
    rms.x = sqrt(rms.x / length);
    rms.y = sqrt(rms.y / length);
    return rms;
  }
  
  

  
  void getWaveForm(std::vector<Vec2d> &waveForm, double waveFormLowestFrequency = 80, double feedback = 0, double prevMaxReachSecs = -1, double scale = 1.0) {
    
    long pmr = 0;
    if(prevMaxReachSecs < 0) {
      prevMaxReachSecs = 1.0/50.0;
    }
    pmr = (long)(sampleRate * prevMaxReachSecs);
    long startDelta = 0;
    double maxValue = 0;
    for(long i=1; i<pmr; i++) {
      double v = getOutputSample(i).x;
      if(maxValue < v) {
        startDelta = i;
        maxValue = v;
      }
    }
      
    for(int i=0; i<waveForm.size(); i++) {
      Vec2d out = getOutputSampleInSeconds((double)startDelta/sampleRate + map(i, 0.0, waveForm.size(), 0, 1.0/waveFormLowestFrequency)) * scale;
      waveForm[i] = out * (1.0-feedback) + waveForm[i] * feedback;
    }
  }
  
  
};
