#pragma once
#include <fftw3.h>
#include "mathl.h"
#include <vector>
#include <cstring>
#include <cstdio>

/* TODO
 * reverse transform
 * input data copying probably not necessary, just point the pointer to the vector data beginning (?)
 *
 * get rid of two buffers, use methods to read/get values
 */

struct FFTW3Interface
{
  enum TransformMode { Forward, Reverse, ForwardAndReverse };
  
private:
  long n = 0;
  bool calculatePhase = 0;
  
  double mean = 0;
  double maxValue = 0, maxValuePhase = 0;
  int maxValueIndex = 0;

  double *in = NULL;
  fftw_complex *out = NULL;
  fftw_plan plan;

  fftw_complex *reverseIn = NULL;
  double *reverseOut = NULL;
  fftw_plan reversePlan;
  
  TransformMode transformMode = Forward;

  bool initialized = false;
  
  std::vector<double> hammingWindow, hanningWindow;
  
public:
  std::vector<double> input;
  std::vector<double> output;
  std::vector<Vec2d> output2;

  std::vector<Vec2d> reverseInput;
  std::vector<double> reverseOutput;
  
  enum TimeWindow { NoWindow, Hamming, Hanning };
  TimeWindow timeWindow = NoWindow;

  ~FFTW3Interface() {
    if(transformMode != Reverse) {
      //fftw_destroy_plan(plan);
      fftw_free(in);
      fftw_free(out);
    }
    
    if(transformMode != Forward) {
      //fftw_destroy_plan(reversePlan);
      fftw_free(reverseIn);
      fftw_free(reverseOut);
    }
  }

  bool isInitialized() const {
    return initialized;
  }
  inline TransformMode getTransformMode() const {
    return transformMode;
  }

  inline long getSize() {
    return n;
  }
  inline double getMean() {
    return mean;
  }
  inline double getMaxValue() {
    return maxValue;
  }
  inline double getMaxValuePhase() {
    return maxValuePhase;
  }
  inline int getMaxValueIndex() {
    return maxValueIndex;
  }

  void initialize(int n, TransformMode transformMode) {
    initialize(n, false, transformMode);
  }

  void initialize(int n, bool calculatePhase = false, TransformMode transformMode = Forward) {
    if(n == 0) {
      printf("Error at FFTW3Interface::initialize(), n == 0\n");
    }
    this->n = n;
    this->calculatePhase = calculatePhase;
    
    if(in) {
      fftw_free(in);
      //fftw_destroy_plan(plan);
    }
    if(out) {
      fftw_free(out);
    }
    if(reverseIn) {
      fftw_free(reverseIn);
      //fftw_destroy_plan(reversePlan);
    }
    if(reverseOut) {
      fftw_free(reverseOut);
    }
    this->transformMode = transformMode;
    
    if(transformMode != Reverse) {
      in = (double*)fftw_malloc(sizeof(double)*n);
      out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)*n);
      memset(in, 0, sizeof(double)*n);
      memset(out, 0, sizeof(fftw_complex)*n);
      plan = fftw_plan_dft_r2c_1d(n, in, out, FFTW_MEASURE);
      input.resize(n);
      if(calculatePhase) {
        output.resize(0);
        output2.resize(n);
      }
      else {
        output.resize(n);
        output2.resize(0);
      }
    }
    
    if(transformMode != Forward) {
      reverseIn = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)*n);
      reverseOut = (double*)fftw_malloc(sizeof(double)*n);
      memset(reverseIn, 0, sizeof(fftw_complex)*n);
      memset(reverseOut, 0, sizeof(double)*n);
      reversePlan = fftw_plan_dft_c2r_1d(n, reverseIn, reverseOut, FFTW_MEASURE);
      reverseInput.resize(n);
      reverseOutput.resize(n);
    }
    
    initialized = true;
  }

  inline void setReverseInput(int index, double v, double a) {
    reverseInput[index].x = sin(a) * v;
    reverseInput[index].y = cos(a) * v;
  }

  void transformReverse() {
    if(transformMode == Forward) {
      printf("Error at FFTW3Interface.transformReverse(), transformMode == Forward\n");
    }

    
    //reversePlan = fftw_plan_dft_c2r_1d(n, reverseIn, reverseOut, FFTW_MEASURE);
    if(sizeof(reverseIn[0]) != sizeof(reverseInput[0])) {
      printf("FIXME at FFTW3Interface.transformReverse(), sizeof(reverseIn[0]) != sizeof(input[0]), %d != %d\n", (int)sizeof(reverseIn[0]), (int)sizeof(reverseInput[0]));
    }
    memcpy(reverseIn, reverseInput.data(), n*sizeof(reverseInput[0]));
    //fftw_print_plan(plan);
    fftw_execute(reversePlan);
    //memcpy(output.data(), out, n*2);
    mean = 0;
    maxValue = 0;
    for(int i=0; i<n; i++) {
      reverseOutput[i] = reverseOut[i];
      maxValue = max(fabs(reverseOutput[i]), maxValue);
      mean += reverseOutput[i];
    }
    //memcpy(reverseOutput.data(), reverseOut, n);
    /*if(calculatePhase) {
      for(int k=0; k<n; k++) {
        double a = out[k][0] / (0.5*n);
        double b = out[k][1] / (0.5*n);
        double gain = sqrt(a*a + b*b);
        double phase = 0.5 - atan2(a, b) / (2.0 * 3.1415926535);
        output2[k].x = gain;
        output2[k].y = phase;
        mean += gain;
        if(gain > maxValue) {
          maxValue = gain;
          maxValuePhase = phase;
          maxValueIndex = k;
        }
      }
    }
    else {
      for(int k=0; k<n; k++) {
        double a = out[k][0] / (0.5*n);
        double b = out[k][1] / (0.5*n);
        output[k] = sqrt(a*a + b*b);
        mean += output[k];
        if(output[k] > maxValue) {
          maxValue = output[k];
          maxValueIndex = k;
        }
      }
    }*/
    mean /= n;
  }
  
  void transform() {
    if(transformMode == Reverse) {
      printf("Error at FFTW3Interface.transform(), transformMode == Reverse\n");
    }
    //plan = fftw_plan_dft_r2c_1d(n, in, out, FFTW_MEASURE);
    memcpy(in, input.data(), n);
    
    if(timeWindow == TimeWindow::Hanning) {
      if(hanningWindow.size() != n) {
        hanningWindow.resize(n);
        for(int i=0; i<n; i++) {
          hanningWindow[i] = 0.5 - 0.5 * cos(2.0*PI * i/(n-1.0));
        }
      }
      for(int i=0; i<n; i++) {
        in[i] *= hanningWindow[i];
      }
    }
    if(timeWindow == TimeWindow::Hamming) {
      if(hammingWindow.size() != n) {
        hammingWindow.resize(n);
        for(int i=0; i<n; i++) {
          hammingWindow[i] = 0.54 - 0.46 * cos(2.0*PI * i/(n-1.0));
        }
      }
      for(int i=0; i<n; i++) {
        in[i] *= hammingWindow[i];
      }
    }

    //std::copy(input.begin(), input.end, in);
    //fftw_print_plan(plan);
    fftw_execute(plan);
    //memcpy(output.data(), out, n*2);
    mean = 0;
    maxValue = 0;
    if(calculatePhase) {
      for(int k=0; k<n; k++) {
        double a = out[k][0] / (0.5*n);
        double b = out[k][1] / (0.5*n);
        double gain = sqrt(a*a + b*b);
        double phase = 0.5 - atan2(a, b) / (2.0 * 3.1415926535);
        output2[k].x = gain;
        output2[k].y = phase;
        mean += gain;
        if(gain > maxValue) {
          maxValue = gain;
          maxValuePhase = phase;
          maxValueIndex = k;
        }
      }
    }
    else {
      for(int k=0; k<n; k++) {
        double a = out[k][0] / (0.5*n);
        double b = out[k][1] / (0.5*n);
        output[k] = sqrt(a*a + b*b);
        mean += output[k];
        if(output[k] > maxValue) {
          maxValue = output[k];
          maxValueIndex = k;
        }
      }
    }
    mean /= n;
  }

};
