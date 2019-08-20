#pragma once
#include "postprocessingeffect.h"

/*
 * Notch: all-pass settings, local memory, bandwidth 0.1 - 6, wet 0.5
 * Notch: notch settings, local memory, bandwidth 0.1 - 6, wet 1.0
 * Band-pass: bandwidth 0.01-1, cascade 2-10, wet 0.1-0.4
 * High-pass: local memory, bandwidth 0-10, wet 1.0
 * Low-pass: local memory, bandwidth 0-10, wet 1.0
 *
 * Fix that static stuff mess
 */
 
static const double ln2 = log(2.0) * 0.5;
static const int lowestFilterResponseGraphFrequency = 1;
static const int highestFilterResponseGraphFrequency = 16000;
static const int numGraphRanges = 7;
static const std::vector<Vec2d> filterResponseGraphRanges = {Vec2d(0, 100), Vec2d(0, 500), Vec2d(0, 1200), Vec2d(0, 3000), Vec2d(0, 6000), Vec2d(0, 12000), Vec2d(0, 22100)};
static const int filterResponseGraphSize = 600;
//static std::vector<double> filterResponseGraphNeutral;
static std::vector<std::vector<double>> filterResponseGraphsNeutral;
static std::vector<double> filterResponseGraphNeutralPitches;
static bool filterResponseGraphNeutralInitialized;

static FFTW3Interface filterResponseGraphFFTW;

static int sampleClipSize;
static std::vector<double> sampleClip;
//static std::vector<std::vector<double>> sampleClips;

static void Apply(std::vector<Vec2d> &xs, std::vector<Vec2d> &ys, Vec3d a, Vec3d b, double wetLevel, double gain) {
  //for(int i=0; i<cascadeLevel; i++) {
    //ys[0] = (xs[0]*b0 + xs[1]*b1 + xs[2]*b2 - ys[1]*a1 - ys[2]*a2) / a0;
    ys[0] = xs[0]*b.x + xs[1]*b.y + xs[2]*b.z - ys[1]*a.y - ys[2]*a.z;
    ys[2] = ys[1];
    ys[1] = ys[0];
    xs[2] = xs[1];
    xs[1] = xs[0];

    xs[0] = xs[0] * (1.0-wetLevel) + ys[0] * wetLevel;
    xs[0] *= gain;
  //}
}

static void prepareFilterResponseGraphNeutral(double sampleRate) {

  Vec3d as(1, 0, 0), bs(1, 0, 0);

  std::vector<Vec2d> xs = {Vec2d(), Vec2d(), Vec2d()};
  std::vector<Vec2d> ys = {Vec2d(), Vec2d(), Vec2d()};

  //filterResponseGraphFFTW.initialize(sampleClipSize, true);
  for(int p=0; p<numGraphRanges; p++) {
    memset(filterResponseGraphFFTW.input.data(), 0, filterResponseGraphFFTW.input.size()*sizeof(filterResponseGraphFFTW.input[0]));
    for(int i=0; i<sampleClip.size(); i++) {
      xs[0].x = sampleClip[i];
      Apply(xs, ys, as, bs, 1, 1);
      filterResponseGraphFFTW.input[i] = xs[0].x;
    }

  filterResponseGraphFFTW.transform();
  
    for(int i=0; i<filterResponseGraphSize; i++) {
      int k = (int)map(i, 0, filterResponseGraphSize-1, filterResponseGraphRanges[p].x, filterResponseGraphRanges[p].y);
      k = clamp(k, 0, sampleClip.size()); // FIXME
      filterResponseGraphsNeutral[p][i] = filterResponseGraphFFTW.output2[k].x;
    }
  }
  
  double minPitch = freqToNote(lowestFilterResponseGraphFrequency);
  double maxPitch = freqToNote(highestFilterResponseGraphFrequency);
  for(int i=0; i<filterResponseGraphSize; i++) {        
    //double pitch = map(i, 0, filterResponseGraphSize-1, minPitch, maxPitch);
    filterResponseGraphNeutralPitches[i] = getPitchAmplitudeFromFrequencySpectrum(sampleRate, filterResponseGraphFFTW.output2, minPitch, maxPitch, i, filterResponseGraphSize).x;
  }

  /*for(int i=0; i<filterResponseGraphSize; i++) {
    int k = (int)map(i, 0, filterResponseGraphSize-1, lowestFilterResponseGraphFrequency, highestFilterResponseGraphFrequency);
    k = clamp(k, 0, sampleClipSize-1);
    filterResponseGraphNeutral[i] = filterResponseGraphFFTW.output2[k].x;
  }*/

  //this->type = typeTmp;
}


static void prepareBiquadFilterReferenceGraph(double sampleRate) {
  if(!filterResponseGraphNeutralInitialized) {
    sampleClipSize = 44100 / 4.0;
    //sampleClipSize = 10000;
    //sampleClip.resize(sampleClipSize);
    //filterResponseGraphNeutral.resize(filterResponseGraphSize);
    sampleClip.resize(sampleClipSize);

    filterResponseGraphsNeutral.resize(numGraphRanges);
    for(int p=0; p<numGraphRanges; p++) {
      filterResponseGraphsNeutral[p].resize(filterResponseGraphSize);

      for(int i=0; i<min(filterResponseGraphSize, filterResponseGraphRanges[p].y); i++) {
        double f = (int)map(i, 0.0, min(filterResponseGraphSize, filterResponseGraphRanges[p].y)-1, filterResponseGraphRanges[p].x, filterResponseGraphRanges[p].y);
        for(int k=0; k<sampleClip.size(); k++) {
          sampleClip[i] += sin(2.0*PI * f * k / sampleRate);
        }
      }
    }
    filterResponseGraphNeutralPitches.resize(filterResponseGraphSize);
    
    filterResponseGraphFFTW.initialize((int)sampleRate, true);
    prepareFilterResponseGraphNeutral(sampleRate);
    filterResponseGraphNeutralInitialized = true;
  }
}
  


struct BiquadFilter : public PostProcessingEffect {
  
  void print() {
    printf("%s, freq %f, bw %f, dbgain %f, q %f\n", typeNames[type].c_str(), frequency, bandwidth, dbGain, qualityFactor);
  }
  
  enum Type { None, LowPass1, HighPass1, LowPass, HighPass, BandPass, Notch, LowShelving, HighShelving, PeakingEQ, AllPass, Random};
  const std::vector<std::string> typeNames = { "None", "Low-pass (1. order)", "High-pass (1. order)", "Low-pass", "High-pass", "Band-pass", "Notch", "Low shelving", "High shelving", "Peaking EQ", "All-pass", "Random" };
  //static const int numTypes = 8;

  Type type = LowPass;

  double frequency = 2000, dbGain = 3, bandwidth = 2, qualityFactor = 1.0;

  std::vector<double> bandwidths = {2, 2, 2, 2};

  Vec2d x0, x1, x2, y0, y1, y2;
  double a0, a1, a2, b0, b1, b2;
  
  bool useLocalMemory = true;
  int cascadeLevel = 1;

  double wetLevel = 1.0;
  double gain = 1.0;

  std::vector<double> filterResponseGraph;
    
  std::vector<double> filterResponseGraphNormalized;

  bool showTrackingPanel = false;
  double frequencyKeyTracking = 0.0;
  double bandwidthKeyTracking = 0.0;

  bool graphAsPitches = false;
  
  double sampleRate = 0;
  
  int filterResponseGraphRange = 1;
  
  bool noGui = false;
  
  enum Cascade { NoCascade, Bessel4, Butterworth4, LinkwitzRiley4, Bessel6, Butterworth6, Bessel8, Butterworth8, LinkwitzRiley8, Custom};
  const std::vector<std::string> cascadeNames = { "None", "Bessel4", "Butterworth4", "LinkwitzRiley4", "Bessel6", "Butterworth6", "Bessel8", "Butterworth8", "LinkwitzRiley8", "Custom"};
  Cascade cascade = NoCascade;
  int numCascadedBiquads = 1;
  std::vector<BiquadFilter*> cascadedBiquadFilters;
  
  void set(double sampleRate, double wetLevel, double gain, Type type, double frequency, double bandwidth, double dbGain) {
    this->sampleRate = sampleRate;
    this->type = type;
    this->wetLevel = wetLevel;
    this->gain = gain;
    this->frequency = frequency;
    this->bandwidth = bandwidth;
    this->dbGain = dbGain;
    this->biquadEquation = WithBandwidth;
    prepare();
    reset();
  }
  void set(double sampleRate, Type type, double frequency, double bandwidth, double dbGain) {
    this->sampleRate = sampleRate;
    this->type = type;
    this->frequency = frequency;
    this->bandwidth = bandwidth;
    this->dbGain = dbGain;
    this->biquadEquation = WithBandwidth;
    prepare();
    reset();
  }
  void setQ(double sampleRate, double wetLevel, double gain, Type type, double frequency, double qualityFactor) {
    this->sampleRate = sampleRate;
    this->type = type;
    this->wetLevel = wetLevel;
    this->gain = gain;
    this->frequency = frequency;
    this->qualityFactor = qualityFactor;
    this->biquadEquation = WithQ;
    prepare();
    reset();
  }

  void prepareCascade() {
    if(cascadedBiquadFilters.size() != 4) {
      cascadedBiquadFilters.resize(4);
      for(int i=0; i<cascadedBiquadFilters.size(); i++) {
        cascadedBiquadFilters[i] = new BiquadFilter(sampleRate, true);
      }
    }
    if(cascade == Bessel4) {
      numCascadedBiquads = 2;
      cascadedBiquadFilters[0]->setQ(sampleRate, wetLevel, gain, type, frequency, 0.52);
      cascadedBiquadFilters[1]->setQ(sampleRate, wetLevel, gain, type, frequency, 0.81);
    }
    if(cascade == Butterworth4) {
      numCascadedBiquads = 2;
      cascadedBiquadFilters[0]->setQ(sampleRate, wetLevel, gain, type, frequency, 0.54);
      cascadedBiquadFilters[1]->setQ(sampleRate, wetLevel, gain, type, frequency, 1.31);
    }
    if(cascade == LinkwitzRiley4) {
      numCascadedBiquads = 2;
      cascadedBiquadFilters[0]->setQ(sampleRate, wetLevel, gain, type, frequency, 0.707);
      cascadedBiquadFilters[1]->setQ(sampleRate, wetLevel, gain, type, frequency, 0.707);
    }
    if(cascade == Bessel6) {
      numCascadedBiquads = 3;
      cascadedBiquadFilters[0]->setQ(sampleRate, wetLevel, gain, type, frequency, 0.51);
      cascadedBiquadFilters[1]->setQ(sampleRate, wetLevel, gain, type, frequency, 0.61);
      cascadedBiquadFilters[2]->setQ(sampleRate, wetLevel, gain, type, frequency, 1.02);
    }
    if(cascade == Butterworth6) {
      numCascadedBiquads = 3;
      cascadedBiquadFilters[0]->setQ(sampleRate, wetLevel, gain, type, frequency, 0.52);
      cascadedBiquadFilters[1]->setQ(sampleRate, wetLevel, gain, type, frequency, 0.71);
      cascadedBiquadFilters[2]->setQ(sampleRate, wetLevel, gain, type, frequency, 1.93);
    }
    
    if(cascade == Bessel8) {
      numCascadedBiquads = 4;
      cascadedBiquadFilters[0]->setQ(sampleRate, wetLevel, gain, type, frequency, 0.51);
      cascadedBiquadFilters[1]->setQ(sampleRate, wetLevel, gain, type, frequency, 0.56);
      cascadedBiquadFilters[2]->setQ(sampleRate, wetLevel, gain, type, frequency, 0.71);
      cascadedBiquadFilters[3]->setQ(sampleRate, wetLevel, gain, type, frequency, 1.21);
    }
    if(cascade == Butterworth8) {
      numCascadedBiquads = 4;
      cascadedBiquadFilters[0]->setQ(sampleRate, wetLevel, gain, type, frequency, 0.51);
      cascadedBiquadFilters[1]->setQ(sampleRate, wetLevel, gain, type, frequency, 0.6);
      cascadedBiquadFilters[2]->setQ(sampleRate, wetLevel, gain, type, frequency, 0.9);
      cascadedBiquadFilters[3]->setQ(sampleRate, wetLevel, gain, type, frequency, 2.56);
    }
    if(cascade == LinkwitzRiley8) {
      numCascadedBiquads = 4;
      cascadedBiquadFilters[0]->setQ(sampleRate, wetLevel, gain, type, frequency, 0.54);
      cascadedBiquadFilters[1]->setQ(sampleRate, wetLevel, gain, type, frequency, 0.54);
      cascadedBiquadFilters[2]->setQ(sampleRate, wetLevel, gain, type, frequency, 1.31);
      cascadedBiquadFilters[3]->setQ(sampleRate, wetLevel, gain, type, frequency, 1.31);
    }
    if(cascade == Custom) {
      numCascadedBiquads = 4;
      cascadedBiquadFilters[0]->setQ(sampleRate, wetLevel, gain, type, frequency, bandwidths[0]);
      cascadedBiquadFilters[1]->setQ(sampleRate, wetLevel, gain, type, frequency, bandwidths[1]);
      cascadedBiquadFilters[2]->setQ(sampleRate, wetLevel, gain, type, frequency, bandwidths[2]);
      cascadedBiquadFilters[3]->setQ(sampleRate, wetLevel, gain, type, frequency, bandwidths[3]);
    }
    
    prepareFilterResponseGraph();
  }

  void set(const BiquadFilter &biquadFilter) {
    this->type = biquadFilter.type;;
    this->frequency = biquadFilter.frequency;
    this->dbGain = biquadFilter.dbGain;
    this->bandwidth = biquadFilter.bandwidth;
    this->qualityFactor = biquadFilter.qualityFactor;
    this->useLocalMemory = biquadFilter.useLocalMemory;
    this->cascadeLevel = biquadFilter.cascadeLevel;
    this->sampleRate = biquadFilter.sampleRate;
    frequencyKeyTracking = biquadFilter.frequencyKeyTracking;
    bandwidthKeyTracking = biquadFilter.bandwidthKeyTracking;
    prepare();
  }
  
  virtual ~BiquadFilter() {
    if(panel) {
      GuiElement *parent = panel->parent;
      parent->deleteChildElement(panel);
    }
  }
  
  BiquadFilter(const BiquadFilter &biquadFilter) : PostProcessingEffect("Biquad filter") {
    //sampleClipSize = (int)(delayLine->sampleRate);
    filterResponseGraph.resize(filterResponseGraphSize);    
    filterResponseGraphNormalized.resize(filterResponseGraphSize);

    this->isActive = biquadFilter.isActive;

    this->type = biquadFilter.type;;

    this->frequency = biquadFilter.frequency;
    this->dbGain = biquadFilter.dbGain;
    this->bandwidth = biquadFilter.bandwidth;
    this->qualityFactor = biquadFilter.qualityFactor;

    this->useLocalMemory = biquadFilter.useLocalMemory;
    this->cascadeLevel = biquadFilter.cascadeLevel;

    this->sampleRate = biquadFilter.sampleRate;

    prepare();
  }
  
  BiquadFilter(DelayLine *delayLine, const BiquadFilter &biquadFilter) : PostProcessingEffect(delayLine, "Biquad filter") {
    //sampleClipSize = (int)(delayLine->sampleRate);
    filterResponseGraph.resize(filterResponseGraphSize);    
    filterResponseGraphNormalized.resize(filterResponseGraphSize);

    this->isActive = biquadFilter.isActive;

    this->type = biquadFilter.type;;

    this->frequency = biquadFilter.frequency;
    this->dbGain = biquadFilter.dbGain;
    this->bandwidth = biquadFilter.bandwidth;
    this->qualityFactor = biquadFilter.qualityFactor;

    this->useLocalMemory = biquadFilter.useLocalMemory;
    this->cascadeLevel = biquadFilter.cascadeLevel;

    this->sampleRate = delayLine->sampleRate;

    prepare();
  }
  
  //void initialize()


  BiquadFilter(DelayLine *delayLine) : PostProcessingEffect(delayLine, "Biquad filter") {
    if(!noGui) {
      filterResponseGraph.resize(filterResponseGraphSize);
      filterResponseGraphNormalized.resize(filterResponseGraphSize);
    }

    this->sampleRate = delayLine->sampleRate;

    prepare();
  }
  
  BiquadFilter(double sampleRate, bool noGui = false) : PostProcessingEffect("Biquad filter") {
    this->noGui = noGui;
    if(!noGui) {
      filterResponseGraph.resize(filterResponseGraphSize);
      filterResponseGraphNormalized.resize(filterResponseGraphSize);
    }
    
    this->sampleRate = sampleRate;

    prepare();
  }
  
  BiquadFilter() : PostProcessingEffect("Biquad filter") {
    if(!noGui) {
      filterResponseGraph.resize(filterResponseGraphSize);
      filterResponseGraphNormalized.resize(filterResponseGraphSize);
    }
  }
  
  BiquadFilter(double sampleRate, Type type, double frequency = 440, double bandwidth = 2, double dbGain = 0) : PostProcessingEffect("Biquad filter") {
    if(!noGui) {
      filterResponseGraph.resize(filterResponseGraphSize);
      filterResponseGraphNormalized.resize(filterResponseGraphSize);
    }
    
    this->sampleRate = sampleRate;
    this->type = type;
    this->frequency = frequency;
    this->bandwidth = bandwidth;
    this->dbGain = dbGain;
    prepare();
    reset();
  }


  void prepareFilterResponseGraph() {
    if(noGui) return;
    
    double bw = frequency;// / (min(1.0, bandwidth));
    if(!(bw > filterResponseGraphRanges[filterResponseGraphRange].y*0.2 && bw < filterResponseGraphRanges[filterResponseGraphRange].y*0.8)) {
      for(int i=0; i<numGraphRanges; i++) {
        if(i == numGraphRanges - 1 || (bw > filterResponseGraphRanges[i].x && bw < filterResponseGraphRanges[i].y*0.8)) {
          filterResponseGraphRange = i;
          break;
        }
      }
      graphPanel->horizontalAxisLimits.x = filterResponseGraphRanges[filterResponseGraphRange].x;
      graphPanel->horizontalAxisLimits.y = filterResponseGraphRanges[filterResponseGraphRange].y;
    }
    
    
    std::vector<Vec2d> xs = { Vec2d(), Vec2d(), Vec2d() };
    std::vector<Vec2d> ys = { Vec2d(), Vec2d(), Vec2d() };

    //filterResponseGraphFFTW.initialize(sampleClipSize, true);
    for(int i=0; i<sampleClip.size(); i++) {
      xs[0].x = sampleClip[i];
      apply(xs, ys);
      filterResponseGraphFFTW.input[i] = xs[0].x;
    }

    filterResponseGraphFFTW.transform();
    
    if(graphAsPitches) {
      double minPitch = freqToNote(lowestFilterResponseGraphFrequency);
      double maxPitch = freqToNote(highestFilterResponseGraphFrequency);
      
      for(int i=0; i<filterResponseGraphSize; i++) {        
        //double pitch = map(i, 0, filterResponseGraphSize-1, minPitch, maxPitch);
        filterResponseGraph[i] = getPitchAmplitudeFromFrequencySpectrum(sampleRate, filterResponseGraphFFTW.output2, minPitch, maxPitch, i, filterResponseGraphSize).x;

        filterResponseGraphNormalized[i] = filterResponseGraph[i] / filterResponseGraphNeutralPitches[i];
      }
    }
    else {
      //for(int p=0; p<numGraphRanges; p++) {
        for(int i=0; i<filterResponseGraphSize; i++) {
          int k = (int)map(i, 0, filterResponseGraphSize-1, filterResponseGraphRanges[filterResponseGraphRange].x, filterResponseGraphRanges[filterResponseGraphRange].y);
          k = clamp(k, 0, sampleClip.size()-1);
          filterResponseGraph[i] = filterResponseGraphFFTW.output2[k].x;
          filterResponseGraphNormalized[i] = filterResponseGraph[i] / filterResponseGraphsNeutral[filterResponseGraphRange][i];
        }
      //}
      /*for(int i=0; i<filterResponseGraphSize; i++) {
        int k = (int)map(i, 0, filterResponseGraphSize-1, lowestFilterResponseGraphFrequency, highestFilterResponseGraphFrequency);
        k = clamp(k, 0, sampleClipSize-1);
        filterResponseGraph[i] = filterResponseGraphFFTW.output2[k].x;
        filterResponseGraphNormalized[i] = filterResponseGraph[i] / filterResponseGraphNeutral[i];
      }*/
    }
    
    double maxVal = 0;
    for(int i=0; i<filterResponseGraphNormalized.size(); i++) {
      if(filterResponseGraphNormalized[i] > maxVal) {
        maxVal = filterResponseGraphNormalized[i];
      }
    }
    graphPanel->verticalAxisLimits.y = max(2, round(maxVal+0.5));
    graphPanel->numVerticalMarks = (int)graphPanel->verticalAxisLimits.y + 1;
    if(graphPanel) {
      graphPanel->prerenderingNeeded = true;
    }
  }

  void prepare(bool updateGraphPanel = true) {
    prepare(a0, a1, a2, b0, b1, b2, frequency, bandwidth, updateGraphPanel);
  }

  enum BiquadEquation { WithBandwidth, WithQ };
  const std::vector<std::string> biquadEquationNames = { "With bandwidth", "With quality factor" };

  BiquadEquation biquadEquation = WithBandwidth;

   void prepare(double &a0, double &a1, double &a2, double &b0, double &b1, double &b2, double &frequency, double &bandwidth, bool updateGraphPanel = true) {
     if(cascade != NoCascade) {
       prepareCascade();
     }
     else {
     double w0 = 2.0 * PI * frequency / sampleRate;
     double alpha;
     
     if(biquadEquation == WithBandwidth) {
       alpha = sin(w0) * sinh(ln2 * bandwidth * w0 / sin(w0));
     }
     else if(biquadEquation == WithQ) {
       double ql = pow(10.0, qualityFactor / 20.0);
       alpha = sin(w0) * 0.5 * sqrt(2.0 - 0.5*sqrt(16.0-16.0/(ql*ql)));
     }

     if(type == Type::None) {
       b0 = 1;
       b1 = 0;
       b2 = 0;
       a0 = 1;
       a1 = 0;
       a2 = 0;
     }
     if(type == Type::Random) {
       b0 = Random::getDouble(-3, 3);
       b1 = Random::getDouble(-3, 3);
       b2 = Random::getDouble(-3, 3);
       a0 = Random::getDouble(-3, 3);
       a1 = Random::getDouble(-3, 3);
       a2 = Random::getDouble(-3, 3);
     }
     if(type == Type::LowPass1) {
       double K = tan(w0*0.5);
       b0 = K/(1.0+K);
       b1 = K/(1.0+K);
       b2 = 0;
       a0 = 1;
       a1 = -(1.0-K)/(1.0+K);
       a2 = 0;
     }
     if(type == Type::HighPass1) {
       double K = tan(w0*0.5);
       b0 = 1.0/(1.0+K);
       b1 = -1.0/(1.0+K);
       b2 = 0;
       a0 = 1;
       a1 = -(1.0-K)/(1.0+K);
       a2 = 0;
     }
     if(type == Type::LowPass) {
       double cosw0 = cos(w0);
       b0 = (1.0 - cosw0) * 0.5;
       b1 = 1.0 - cosw0;
       b2 = (1.0 - cosw0) * 0.5;
       a0 = 1.0 + alpha;
       a1 = -2.0 * cosw0;
       a2 = 1.0 - alpha;
     }
     if(type == Type::HighPass) {
       double cosw0 = cos(w0);
       b0 = (1.0 + cosw0) * 0.5;
       b1 = -1.0 - cosw0;
       b2 = (1.0 + cosw0) * 0.5;
       a0 = 1.0 + alpha;
       a1 = -2.0 * cosw0;
       a2 = 1.0 - alpha;
     }
     if(type == Type::BandPass) {
       double sinw0 = sin(w0);
       b0 = sinw0 * 0.5;
       b1 = 0;
       b2 = -sinw0 * 0.5;
       a0 = 1.0 + alpha;
       a1 = -2.0 * cos(w0);
       a2 = 1.0 - alpha;
     }
     if(type == Type::Notch) {
       double cosw0 = cos(w0);
       b0 = 1;
       b1 = -2.0 * cosw0;
       b2 = 1;
       a0 = 1.0 + alpha;
       a1 = -2.0 * cosw0;
       a2 = 1.0 - alpha;
     }
     if(type == Type::AllPass) {
       double cosw0 = cos(w0);
       b0 = 1.0 - alpha;
       b1 = -2.0 * cosw0;
       b2 = 1.0 + alpha;
       a0 = 1.0 + alpha;
       a1 = -2.0 * cosw0;
       a2 = 1.0 - alpha;
     }
     if(type == Type::PeakingEQ) {
       double A = sqrt(pow(10.0, dbGain / 20.0));
       double cosw0 = cos(w0);
       b0 = 1.0 + alpha * A;
       b1 = -2.0 * cosw0;
       b2 = 1.0 - alpha * A;
       a0 = 1.0 + alpha / A;
       a1 = -2.0 * cosw0;
       a2 = 1.0 - alpha / A;
     }
     /*if(type == Type::LowShelving) {
       double A = sqrt(pow(10.0, dbGain / 20.0));
       double beta = sqrt(A) / qualityFactor;
       double cosw0 = cos(w0);
       double sinw0 = sin(w0);
       
       b0 = A * ((A+1) - (A-1)*cosw0 + beta*sinw0);
       b1 = 2*A * ((A-1) - (A+1)*cosw0);
       b2 = A * ((A+1) - (A-1)*cosw0 - beta*sinw0);
       a0 = ((A+1) + (A-1)*cosw0 + beta*sinw0);
       a1 = -2*A * ((A-1) + (A+1)*cosw0);
       a2 = ((A+1) + (A-1)*cosw0 - beta*sinw0);
     }
     if(type == Type::HighShelving) {
       double A = sqrt(pow(10.0, dbGain / 20.0));
       double beta = sqrt(A) / qualityFactor;
       double cosw0 = cos(w0);
       double sinw0 = sin(w0);
       b0 = A * ((A+1) + (A-1)*cosw0 + beta*sinw0);
       b1 = -2*A * ((A-1) + (A+1)*cosw0);
       b2 = A * ((A+1) + (A-1)*cosw0 - beta*sinw0);
       a0 = ((A+1) - (A-1)*cosw0 + beta*sinw0);
       a1 = 2*A * ((A-1) - (A+1)*cosw0);
       a2 = ((A+1) - (A-1)*cosw0 - beta*sinw0);
     }*/
     
     if(type == Type::LowShelving || type == Type::HighShelving) {
       double V = pow(10.0, fabs(dbGain) / 20.0);
       double K = tan(PI * frequency / sampleRate);
       
       if(type == Type::LowShelving) {
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
       if(type == Type::HighShelving) {
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
     else {
       if(a0 == 0) {
         a0 = 1e-10;
       }
       a1 /= a0;
       a2 /= a0;
       b0 /= a0;
       b1 /= a0;
       b2 /= a0;
     }
    }
     if(updateGraphPanel && graphPanel && graphPanel->isVisible) {
       prepareFilterResponseGraph();
     }
   }

   

   void reset() {
     x0.set(0, 0);
     x1.set(0, 0);
     x2.set(0, 0);
     y0.set(0, 0);
     y1.set(0, 0);
     y2.set(0, 0);
   }

   // FIXME cascade not working

  void apply(std::vector<Vec2d> &xs, std::vector<Vec2d> &ys) {
    if(cascade != NoCascade) {
      for(int i=0; i<numCascadedBiquads; i++) {
        cascadedBiquadFilters[i]->apply(xs, ys);
      }
    }
    else {
      ys[0] = xs[0]*b0 + xs[1]*b1 + xs[2]*b2 - ys[1]*a1 - ys[2]*a2;
      ys[2] = ys[1];
      ys[1] = ys[0];
      xs[2] = xs[1];
      xs[1] = xs[0];
      xs[0] = (xs[0] * (1.0-wetLevel) + ys[0] * wetLevel) * gain;
    }
  }
  
  void apply(Vec2d &x0, Vec2d &x1, Vec2d &x2, Vec2d &y0, Vec2d &y1, Vec2d &y2, double a1, double a2, double b0, double b1, double b2) {
    if(cascade != NoCascade) {
      for(int i=0; i<numCascadedBiquads; i++) {
        cascadedBiquadFilters[i]->apply(x0, x1, x2, y0, y1, y2, a1, a2, b0, b1, b2);
      }
    }
    else {
      y0.set(x0*b0 + x1*b1 + x2*b2 - y1*a1 - y2*a2);
      y2.set(y1);
      y1.set(y0);
      x2.set(x1);
      x1.set(x0);

      x0.set(x0 * (1.0-wetLevel) + y0 * wetLevel);
      x0 *= gain;
    }
  }
  
  
  void apply(Vec2d &sample) {
    if(!isActive) return;

    if(cascade != NoCascade) {
      for(int i=0; i<numCascadedBiquads; i++) {
        cascadedBiquadFilters[i]->apply(sample);
      }
    }
    else {
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
  }


  struct OscillatorBiquadFilterPanel : public Panel {
    BiquadFilter *biquadFilter;
    
    NumberBox *frequencyKeyTrackingGui = NULL;
    NumberBox *bandwidthKeyTrackingGui = NULL;
    RotaryKnob<double> *frequencyKeyTrackingKnob = NULL;
    RotaryKnob<double> *bandwidthKeyTrackingKnob = NULL;
    
    std::vector<SynthGuiLayout> layouts = std::vector<SynthGuiLayout>(BiquadFilter::numLayouts);
    
    OscillatorBiquadFilterPanel(GuiElement *parentGuiElement, BiquadFilter *biquadFilter) : Panel("OscillatorBiquadFilterPanel") {
      init(parentGuiElement, biquadFilter);
    }
    
    void update() {
      frequencyKeyTrackingGui->setValue(biquadFilter->frequencyKeyTracking);
      bandwidthKeyTrackingGui->setValue(biquadFilter->bandwidthKeyTracking);
      frequencyKeyTrackingKnob->setValue(biquadFilter->frequencyKeyTracking);
      bandwidthKeyTrackingKnob->setValue(biquadFilter->bandwidthKeyTracking);
      
      for(int i=0; i<layouts.size(); i++) {
        layouts[i].gui->setVisible(i == biquadFilter->layout);
      }
      setSize(layouts[biquadFilter->layout].size);
      setVisible(biquadFilter->layout != 2);
    }
    
    void init(GuiElement *parentGuiElement, BiquadFilter *biquadFilter) {
      this->biquadFilter = biquadFilter;
      
      parentGuiElement->addChildElement(this);
      this->setPosition(0, parentGuiElement->size.y + 5);
      this->addGuiEventListener(new OscillatorBiquadFilterPanelListener(this));
      
      double line = -13, lineHeight = 23;
      
      for(int i=0; i<layouts.size(); i++) {
        addChildElement(layouts[i].gui = new GuiElement("Key tracking layout "+std::to_string(i)));
        layouts[i].gui->setVisible(i == biquadFilter->layout);
      }
       // BOX LAYOUT
      layouts[0].gui->addChildElement(frequencyKeyTrackingGui = new NumberBox("Frequency key tracking", biquadFilter->frequencyKeyTracking, NumberBox::FLOATING_POINT, -1.0, 1.0, 10, line+=lineHeight, 7));
      layouts[0].gui->addChildElement(bandwidthKeyTrackingGui = new NumberBox("Bandwidth key tracking", biquadFilter->bandwidthKeyTracking, NumberBox::FLOATING_POINT, -1.0, 1.0, 10, line+=lineHeight, 7));
      layouts[0].gui->setSize(parentGuiElement->size.x, line + lineHeight + 10);
      layouts[0].size = layouts[0].gui->size;

      // KNOB LAYOUT
      RowColumnPlacer placer(parentGuiElement, 10, 5);

      layouts[1].gui->addChildElement(frequencyKeyTrackingKnob = new RotaryKnob<double>("Frequency key tracking", placer.progressX(), placer.getY(), RotaryKnob<double>::LimitedKnob, -1.0, 1.0, biquadFilter->frequencyKeyTracking));
      layouts[1].gui->addChildElement(bandwidthKeyTrackingKnob = new RotaryKnob<double>("Bandwidth key tracking", placer.progressX(), placer.getY(), RotaryKnob<double>::LimitedKnob, -1.0, 1.0, biquadFilter->bandwidthKeyTracking));
      layouts[1].gui->setSize(parentGuiElement->size.x, placer.getY() + placer.knobSize);
      layouts[1].size = layouts[1].gui->size;
      
      
      setSize(layouts[biquadFilter->layout].size);
      setVisible(biquadFilter->layout != 2);
    }
    
    struct OscillatorBiquadFilterPanelListener : public GuiEventListener {
      OscillatorBiquadFilterPanel *oscillatorBiquadFilterPanel;
      OscillatorBiquadFilterPanelListener(OscillatorBiquadFilterPanel *oscillatorBiquadFilterPanel) {
        this->oscillatorBiquadFilterPanel = oscillatorBiquadFilterPanel;
      }
      void onValueChange(GuiElement *guiElement) {
        if(guiElement == oscillatorBiquadFilterPanel->frequencyKeyTrackingGui || guiElement == oscillatorBiquadFilterPanel->frequencyKeyTrackingKnob) {
          guiElement->getValue((void*)&oscillatorBiquadFilterPanel->biquadFilter->frequencyKeyTracking);
        }
        if(guiElement == oscillatorBiquadFilterPanel->bandwidthKeyTrackingGui || guiElement == oscillatorBiquadFilterPanel->bandwidthKeyTrackingKnob) {
          guiElement->getValue((void*)&oscillatorBiquadFilterPanel->biquadFilter->bandwidthKeyTracking);
        }
      }
    };
  };
  
  void initGraphPanel(int w = 320, int h = 150, bool isVisible = true, bool prepareGraph = true) {
    graphPanel = new GraphPanel(&filterResponseGraphNormalized, true, 11, 0, filterResponseGraphRanges[filterResponseGraphRange].x, filterResponseGraphRanges[filterResponseGraphRange].y, 0, 2, 2, 0, "Hz", "");
    graphPanel->setSize(w, h);
    graphPanel->setVisible(isVisible);
    if(prepareGraph) {
      prepareFilterResponseGraph();
    }
  }
  
  NumberBox *frequencyGui = NULL;
  NumberBox *dbGainGui = NULL;
  NumberBox *bandwidthGui = NULL;
  NumberBox *wetLevelGui = NULL, *gainGui = NULL;
  ListBox *typeGui = NULL;

  RotaryKnob<double> *frequencyKnob = NULL;
  RotaryKnob<double> *dbGainKnob = NULL;
  RotaryKnob<double> *bandwidthKnob = NULL;
  RotaryKnob<double> *wetLevelKnob = NULL, *gainKnob = NULL;
  RotaryKnob<long> *typeKnob = NULL;

  GraphPanel *graphPanel = NULL;
  
  OscillatorBiquadFilterPanel *oscillatorBiquadFilterPanel = NULL;
  
  SynthTitleBar *titleBar = NULL;
  static const int numLayouts = 3;
  int layout = 1;
  
  std::vector<SynthGuiLayout> biquadLayouts = std::vector<SynthGuiLayout>(numLayouts);
  

  Panel *getPanel() {
    return panel;
  }

  Panel *addPanel(GuiElement *parentElement, const std::string &title = "") {
    double line = 0, lineHeight = 23;

    if(!panel) {
      panel = new Panel(300, 300, 60, 10);

      panel->addGuiEventListener(new BiquadFilterPanelListener(this));
      
      titleBar = new SynthTitleBar(getName(), panel, isActive, layout);
      line += titleBar->size.y + 5;
      
      for(int i=0; i<biquadLayouts.size(); i++) {
        panel->addChildElement(biquadLayouts[i].gui = new GuiElement("Biquad gui layout "+std::to_string(i)));
        biquadLayouts[i].gui->setPosition(0, line);
      }
      
      // BOX LAYOUT
      line = 0;
      biquadLayouts[0].gui->addChildElement(typeGui = new ListBox("Type", 10, line, 10));
      typeGui->setItems(typeNames);
      typeGui->setValue(type);
      
      biquadLayouts[0].gui->addChildElement(wetLevelGui = new NumberBox("Wet level", wetLevel, NumberBox::FLOATING_POINT, 0, 1, 10, line += lineHeight, 7));
      biquadLayouts[0].gui->addChildElement(gainGui = new NumberBox("Gain", gain, NumberBox::FLOATING_POINT, 0, 1e10, 10, line += lineHeight, 7));

      biquadLayouts[0].gui->addChildElement(frequencyGui = new NumberBox("Frequency", frequency, NumberBox::FLOATING_POINT, 0, delayLine->sampleRate*0.5, 10, line += lineHeight, 9));
      frequencyGui->defaultIncrementModePowerShift = -1;
      biquadLayouts[0].gui->addChildElement(bandwidthGui = new NumberBox("Bandwidth", bandwidth, NumberBox::FLOATING_POINT, -1e6, delayLine->sampleRate*0.25, 10, line += lineHeight, 9));
      biquadLayouts[0].gui->addChildElement(dbGainGui = new NumberBox("db gain", dbGain, NumberBox::FLOATING_POINT, -1e6, 1e10, 10, line += lineHeight, 9));

      biquadLayouts[0].gui->setSize(panel->size.x, line + lineHeight + 10);
      biquadLayouts[0].size = biquadLayouts[0].gui->pos + biquadLayouts[0].gui->size;

      // KNOB LAYOUT
      
      RowColumnPlacer placer(panel->size.x);
      
      biquadLayouts[1].gui->addChildElement(typeKnob = new RotaryKnob<long>("Type", placer.progressX(), placer.getY(), RotaryKnob<long>::ListKnob, 0, 1, (double)type/(typeNames.size()-1)));
      typeKnob->setItems(typeNames);
      typeKnob->setValue(type);
      
      biquadLayouts[1].gui->addChildElement(wetLevelKnob = new RotaryKnob<double>("Wet level", placer.progressX(), placer.getY(), RotaryKnob<double>::LimitedKnob, 0, 1, wetLevel));
      biquadLayouts[1].gui->addChildElement(gainKnob = new RotaryKnob<double>("Gain", placer.progressX(), placer.getY(), RotaryKnob<double>::LimitedKnob, 0, 1, gain));


      biquadLayouts[1].gui->addChildElement(frequencyKnob = new RotaryKnob<double>("Frequency", placer.progressX(), placer.getY(), RotaryKnob<double>::LimitedKnob, 1, delayLine->sampleRate*0.5-1, frequency));
      frequencyKnob->valueMappingFunction = frequencyExpMapper;
      biquadLayouts[1].gui->addChildElement(bandwidthKnob = new RotaryKnob<double>("Bandwidth", placer.progressX(), placer.getY(), RotaryKnob<double>::LimitedKnob, 0.01, 20, bandwidth));
      biquadLayouts[1].gui->addChildElement(dbGainKnob = new RotaryKnob<double>("dB Gain", placer.progressX(), placer.getY(), RotaryKnob<double>::LimitedKnob, -20, 20, dbGain));

      biquadLayouts[1].gui->setSize(panel->size.x, placer.getY() + placer.knobSize);
      biquadLayouts[1].size = biquadLayouts[1].gui->pos + biquadLayouts[1].gui->size;

      // TITLE BAR LAYOUT
      biquadLayouts[2].size = titleBar->size;
      
      
      panel->setSize(biquadLayouts[layout].size);

      parentElement->addChildElement(panel);

      if(showTrackingPanel) {
        oscillatorBiquadFilterPanel = new OscillatorBiquadFilterPanel(panel, this);
      }

      initGraphPanel(300, 70);
      panel->addChildElement(graphPanel);
      if(showTrackingPanel) graphPanel->setPosition(0, oscillatorBiquadFilterPanel->pos.y+oscillatorBiquadFilterPanel->size.y + 5);
      else graphPanel->setPosition(0, panel->size.y + 5);

      
      for(int i=0; i<numLayouts; i++) {
        biquadLayouts[i].gui->setVisible(i == layout);
      }
      
      panel->setSize(biquadLayouts[layout].size);
    }
    else {
      wetLevelGui->setValue(wetLevel);
      gainGui->setValue(gain);
      typeGui->setValue(type);
      frequencyGui->setValue(frequency);
      dbGainGui->setValue(dbGain);
      bandwidthGui->setValue(bandwidth);
      
      wetLevelKnob->setValue(wetLevel);
      gainKnob->setValue(gain);
      typeKnob->setValue(type);
      frequencyKnob->setValue(frequency);
      dbGainKnob->setValue(dbGain);
      bandwidthKnob->setValue(bandwidth);
      
      for(int i=0; i<numLayouts; i++) {
        biquadLayouts[i].gui->setVisible(i == layout);
      }
      
      panel->setSize(biquadLayouts[layout].size);
      
      if(oscillatorBiquadFilterPanel) {
        oscillatorBiquadFilterPanel->setPosition(0, panel->size.y + 5);
        oscillatorBiquadFilterPanel->update();
      }
      
      if(oscillatorBiquadFilterPanel && showTrackingPanel) graphPanel->setPosition(0, oscillatorBiquadFilterPanel->pos.y+oscillatorBiquadFilterPanel->size.y + 5);
      else graphPanel->setPosition(0, panel->size.y + 5);
      
      graphPanel->setVisible(layout != 2);
    }

    return panel;
  }

  void removePanel(GuiElement *parentElement) {
    PanelInterface::removePanel(parentElement);

    if(panel) {
      parentElement->deleteChildElement(panel);
      panel = NULL;
      frequencyGui = NULL;
      dbGainGui = NULL;
      bandwidthGui = NULL;
      wetLevelGui = NULL;
      gainGui = NULL;
      typeGui = NULL;

      frequencyKnob = NULL;
      dbGainKnob = NULL;
      bandwidthKnob = NULL;
      wetLevelKnob = NULL;
      gainKnob = NULL;
      typeKnob = NULL;
      
      graphPanel = NULL;
      oscillatorBiquadFilterPanel = NULL;
    }
  }

  void updatePanel() {
    if(panel) {
      addPanel(NULL);
    }
    if(oscillatorBiquadFilterPanel) {
      oscillatorBiquadFilterPanel->update();
    }
  }


  struct BiquadFilterPanelListener : public GuiEventListener {
    BiquadFilter *biquadFilter;
    BiquadFilterPanelListener(BiquadFilter *biquadFilter) {
      this->biquadFilter = biquadFilter;
    }
    void onValueChange(GuiElement *guiElement) {
      if(guiElement == biquadFilter->titleBar->layoutGui) {
        biquadFilter->layout = (biquadFilter->layout+1) % biquadFilter->numLayouts;
        biquadFilter->updatePanel();
      }
      if(guiElement == biquadFilter->titleBar->isActiveGui) {
        guiElement->getValue(&biquadFilter->isActive);
        biquadFilter->prepareFilterResponseGraph();
      }
      
      if(guiElement == biquadFilter->typeGui || guiElement == biquadFilter->typeKnob) {
        guiElement->getValue(&biquadFilter->type);
        biquadFilter->prepare();
      }

      if(guiElement == biquadFilter->wetLevelGui || guiElement == biquadFilter->wetLevelKnob) {
        guiElement->getValue(&biquadFilter->wetLevel);
        biquadFilter->prepareFilterResponseGraph();
      }
      if(guiElement == biquadFilter->gainGui || guiElement == biquadFilter->gainKnob) {
        guiElement->getValue(&biquadFilter->gain);
        biquadFilter->prepareFilterResponseGraph();
      }

      if(guiElement == biquadFilter->frequencyGui || guiElement == biquadFilter->frequencyKnob) {
        guiElement->getValue(&biquadFilter->frequency);
        biquadFilter->prepare();
      }
      if(guiElement == biquadFilter->dbGainGui || guiElement == biquadFilter->dbGainKnob) {
        guiElement->getValue(&biquadFilter->dbGain);
        biquadFilter->prepare();
      }
      if(guiElement == biquadFilter->bandwidthGui || guiElement == biquadFilter->bandwidthKnob) {
        guiElement->getValue(&biquadFilter->bandwidth);
        biquadFilter->prepare();
      }

      biquadFilter->reset();
    }
  };
  
  
  
  void applyPreset(int index) {
    if(!presets || index < 0 || index >= presets->size()) return;
    applyParameters(presets->at(index).doubleParameters, presets->at(index).stringParameters);
  }  
  
  void applyParameters(const std::vector<DoubleParameter> &doubleParameters, const std::vector<StringParameter> &stringParameters) {
    std::string typeName;
    getStringParameter("subtype", typeName);
    for(int i=0; i<typeNames.size(); i++) {
      if(typeName == typeNames[i]) {
        type = (BiquadFilter::Type)i;
        break;
      }
    }
    getNumericParameter("gain", gain);
    getNumericParameter("wet", wetLevel);
    getNumericParameter("frequency", frequency);
    getNumericParameter("dbGain", dbGain);
    getNumericParameter("bandwidth", bandwidth);
    getNumericParameter("qualityFactor", qualityFactor);
    
    getNumericParameter("frequencyKeyTracking", frequencyKeyTracking);
    getNumericParameter("bandwidthKeyTracking", bandwidthKeyTracking);

    isActive = true;
    
    prepare();
  }
  
  static std::string getClassName() {
    return "biquadFilter";
  }
  
  virtual std::string getBlockName() {
    return getClassName();
  }
  
  virtual void decodeParameters() {
    applyParameters(doubleParameters, stringParameters);
  }

  virtual void encodeParameters() {
    clearParameters();

    //stringParameters.push_back(StringParameter("type", "reverb"));
    putStringParameter("subtype", typeNames[type]);

    putNumericParameter("gain", gain);
    putNumericParameter("wet", wetLevel);
    putNumericParameter("frequency", frequency);
    putNumericParameter("dbGain", dbGain);
    putNumericParameter("bandwidth", bandwidth);
    putNumericParameter("qualityFactor", qualityFactor);

    if(showTrackingPanel) {
      putNumericParameter("frequencyKeyTracking", frequencyKeyTracking);
      putNumericParameter("bandwidthKeyTracking", bandwidthKeyTracking);
    }

    if(presetName.size() > 0) {
      putStringParameter("presetName", presetName);
    }
  }
  
  
  
  
  
  
  
  
  
  
  
  
};
