#pragma once
//#include <SDL2/SDL.h>
#include <portaudio.h>
//#include "util.h"

/* Many things I have programmed have been familiar to me in most of the levels (such as graphics stuff),
 * but for this sound synthesis thing I have been the least prepared. I have learned stuff as I have kept
 * going, and fixing poor solutions as I have discovered better ways of doing things. This is still
 * largely uncharted territory for me and the most optimal solutions haven't arrived for me yet.
 *
 * I still don't know, for example, what's the best way to deal with floating point precision problem. I reach the limit
 * of float variables as soon as timer goes to something like 300-400 seconds. Perhaps I should be using integer
 * or long variables instead of float altogether with all the timing stuff. I think this port audio don't support
 * doubles. Actually, I am using that long (buffer count) in sound synthesis part now, but I still haven't fixed
 * my shaders. Maybe this part is actually quite fine already. Yes, this is not a problem anymore.
 *
 * But I still have very poor solution here. This class should be as general as possible for sound stuff, and I still
 * have that looping mechanism in here.
 *
 */

 struct PortAudioInterface
 {
   PaStream *stream = NULL;
   double outputLatency = -1;
   double sampleRate = 44100;
   int framesPerBuffer = 128;
   long bufferCount = 0, bufferCountAccumulated = 0;

   bool initialized = false;
   bool started = false;

   double paTime = 0;

   virtual ~PortAudioInterface();

   void setup(double sampleRate, int framesPerBuffer, double suggestedOutputLatency);
   void setup(double sampleRate, int framesPerBuffer, double suggestedOutputLatency, PaStreamCallback *streamCallback, int suggestedAudioDevice);
   void setup(PaStreamCallback *streamCallback);
   void setup();
   void quit();

   void start();
   void stop();

   double getPaTime();

   void setPaTime(double t);

   void update();

   virtual void onUpdate();
   virtual void onStart();

   double getCpuLoad();

   /*std::vector<float> readStream() {
     std::vector<float> samples(2 * framesPerBuffer);
     err = Pa_ReadStream(stream, samples.data(), framesPerBuffer );

     return samples;
   }*/

   static int streamCallback(const void *inputBuffer, void *outputBuffer,
                      unsigned long framesPerBuffer,
                      const PaStreamCallbackTimeInfo* timeInfo,
                      PaStreamCallbackFlags statusFlags, void *userData);

 };
