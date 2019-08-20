#include <SDL2/SDL.h>
#include "portaudiointerface.h"
#include <string>

PortAudioInterface::~PortAudioInterface() {}

void PortAudioInterface::onUpdate() {}
void PortAudioInterface::onStart() {}


void PortAudioInterface::update() {
  onUpdate();
}


double PortAudioInterface::getPaTime() {
  return paTime;
  //return bufferCount * framesPerBuffer / sampleRate;
  // return Pa_GetStreamTime(stream);
}

double PortAudioInterface::getCpuLoad() {
  return Pa_GetStreamCpuLoad(this->stream);
}


void PortAudioInterface::setPaTime(double t) {
  bufferCount = (long)(t * sampleRate / framesPerBuffer);
  paTime = bufferCount * framesPerBuffer / sampleRate;
}


int PortAudioInterface::streamCallback(const void *inputBuffer, void *outputBuffer,
                   unsigned long framesPerBuffer,
                   const PaStreamCallbackTimeInfo* timeInfo,
                   PaStreamCallbackFlags statusFlags, void *userData)
{
  float *out = (float *)outputBuffer;
  PortAudioInterface *paInterface = (PortAudioInterface *)userData;
  paInterface->paTime = (double)(paInterface->bufferCount*framesPerBuffer)/paInterface->sampleRate;

  for(int i=0; i<framesPerBuffer; i++) {
    double d = (double)(paInterface->bufferCount*framesPerBuffer+i)/paInterface->sampleRate;
    out[i*2+1] = out[i*2] = 0;
  }

  paInterface->bufferCount++;
  return 0;
}



void PortAudioInterface::setup(double sampleRate, int framesPerBuffer, double suggestedOutputLatency) {
  this->setup(sampleRate, framesPerBuffer, suggestedOutputLatency, this->streamCallback, -1);
}

void PortAudioInterface::setup() {
  this->setup(this->sampleRate, this->framesPerBuffer, this->outputLatency, this->streamCallback, -1);
}

void PortAudioInterface::setup(PaStreamCallback *streamCallback) {
  this->setup(this->sampleRate, this->framesPerBuffer, this->outputLatency, streamCallback, -1);
}

void PortAudioInterface::setup(double sampleRate, int framesPerBuffer, double suggestedOutputLatency, PaStreamCallback *streamCallback, int suggestedAudioDevice = -1)
{
  this->sampleRate = sampleRate;
  int ret = Pa_Initialize();
  if(ret != paNoError) {
    printf("PortaAudio error: %s\n", Pa_GetErrorText(ret));
    return;
  }
  printf("PortAudio version %s\n", Pa_GetVersionText());

  int numDevices = Pa_GetDeviceCount();
  if(numDevices < 0) {
    printf("PortaAudio error: %s\n", Pa_GetErrorText(numDevices));
    return;
  }
  double minOutputLatency = 10000;
  int minOutputLatencyDevice = -1;
  int systemOutputDevice = -1, defaultOutputDevice = -1;
  std::string systemStr = "system";
  std::string defaultStr = "default";

  printf("PortAudio devices:\n");
  for(int i=0; i<numDevices; i++) {
    const PaDeviceInfo *info = Pa_GetDeviceInfo(i);
    printf("%d: %s, latency low %f/%f, latency high %f/%f, sample rate %f\n", i, info->name, info->defaultLowInputLatency, info->defaultLowOutputLatency, info->defaultHighInputLatency, info->defaultHighOutputLatency, info->defaultSampleRate);
    double latency = fabs(info->defaultLowOutputLatency);
    if(systemStr.compare(std::string(info->name)) == 0) {
        systemOutputDevice = i;
    }
    if(defaultStr.compare(std::string(info->name)) == 0) {
        defaultOutputDevice = i;
    }
    if(latency > 0 && latency < minOutputLatency) {
      minOutputLatency = info->defaultLowOutputLatency;
      minOutputLatencyDevice = i;
    }
  }
  int device = 0;

  PaSampleFormat sampleFormat = paFloat32;

  // FIXME

  if(suggestedAudioDevice >= 0 && suggestedAudioDevice < numDevices) {
    if(sampleRate < 0) {
      const PaDeviceInfo *info = Pa_GetDeviceInfo(suggestedAudioDevice);
      this->sampleRate = info->defaultSampleRate;
    } 
    PaStreamParameters outputStreamParameters = {suggestedAudioDevice, 2, sampleFormat, suggestedOutputLatency, NULL};
    PaStreamParameters inputStreamParameters = {suggestedAudioDevice, 2, sampleFormat, suggestedOutputLatency, NULL};
    ret = Pa_OpenStream(&this->stream, &inputStreamParameters, &outputStreamParameters, this->sampleRate, this->framesPerBuffer, 0, streamCallback, this);
    device = suggestedAudioDevice;
  }
  else if(systemOutputDevice != -1) {
      if(sampleRate < 0) {
        const PaDeviceInfo *info = Pa_GetDeviceInfo(systemOutputDevice);
        this->sampleRate = info->defaultSampleRate;
      } 
    PaStreamParameters outputStreamParameters = {systemOutputDevice, 2, sampleFormat, suggestedOutputLatency, NULL};
    PaStreamParameters inputStreamParameters = {systemOutputDevice, 2, sampleFormat, suggestedOutputLatency, NULL};
    ret = Pa_OpenStream(&this->stream, &inputStreamParameters, &outputStreamParameters, this->sampleRate, this->framesPerBuffer, 0, streamCallback, this);
    device = systemOutputDevice;
  }
  else if(suggestedOutputLatency > 0) {
      if(sampleRate < 0) {
        const PaDeviceInfo *info = Pa_GetDeviceInfo(minOutputLatencyDevice);
        this->sampleRate = info->defaultSampleRate;
      } 

    PaStreamParameters inputStreamParameters = {minOutputLatencyDevice, 2, sampleFormat, suggestedOutputLatency, NULL};
    PaStreamParameters outputStreamParameters = {minOutputLatencyDevice, 2, sampleFormat, suggestedOutputLatency, NULL};
    ret = Pa_OpenStream(&this->stream, &inputStreamParameters, &outputStreamParameters, this->sampleRate, this->framesPerBuffer, 0, streamCallback, this);
    device = minOutputLatencyDevice;
  }
  else {
      if(sampleRate < 0) {
        const PaDeviceInfo *info = Pa_GetDeviceInfo(defaultOutputDevice);
        this->sampleRate = info->defaultSampleRate;
      } 

    ret = Pa_OpenDefaultStream(&this->stream, 2, 2, sampleFormat, this->sampleRate,
    this->framesPerBuffer, streamCallback, this);
    device = defaultOutputDevice;
  }


  printf("Device: %d\n", device);

  if(ret != paNoError) {
    printf("PortaAudio error: %s\n", Pa_GetErrorText(ret));
    return;
  }
  const PaStreamInfo *info = Pa_GetStreamInfo(stream);
  if(info != NULL) {
    printf("Input latency %f, output latency %f, sample rate %f\n",
        info->inputLatency, info->outputLatency, info->sampleRate);
    this->outputLatency = info->outputLatency;
  }
  initialized = true;

  //start();
}

void PortAudioInterface::start() {
  int ret = Pa_StartStream(this->stream);
  if(ret != paNoError) {
    printf("PortaAudio error: %s\n", Pa_GetErrorText(ret));
    return;
  }
  started = true;
  onStart();
}

void PortAudioInterface::stop() {
  if(started) {
    int ret = Pa_StopStream(this->stream);
    if(ret != paNoError) {
      printf("PortaAudio error: %s\n", Pa_GetErrorText(ret));
      return;
    }
    started = false;
  }
  //bufferCount = 0;
}

void PortAudioInterface::quit() {
  stop();
  if(initialized) {
    int ret = Pa_CloseStream(this->stream);
    if(ret != paNoError) {
      printf("PortaAudio error: %s\n", Pa_GetErrorText(ret));
    }

    ret = Pa_Terminate();
    if(ret != paNoError) {
      printf("PortaAudio error: %s\n", Pa_GetErrorText(ret));
    }
  }
}
