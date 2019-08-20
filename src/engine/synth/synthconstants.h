#pragma once

#include <ctgmath>

struct ExpMapper {
  double exponent = std::exp(1.0);
  ExpMapper() {}
  ExpMapper(double exponent) : exponent(exponent) {}
  double operator()(double minValue, double maxValue, double x, double min, double max) {
    double t = map(x, min, max, 0, 1);
    return map(pow(exponent, t), 1, exponent, minValue, maxValue);
  }
};

static ExpMapper frequencyExpMapper = ExpMapper(160);

static const double SIGMA = pow(2.0, 1.0/12.0);
static const double LOG_SIGMA = log(pow(2.0, 1.0/12.0));
static const double A4_FREQUENCY = 440.0;
static const double C4_FREQUENCY = 261.6255653;
static const double A4_NUMBER = 69.0;

static const std::vector<std::string> noteNames = {"C", "C#/Db", "D", "D#/Eb", "E", "F", "F#/Gb", "G", "G#/Ab", "A", "A#/Bb", "B"};

static inline double noteToFreq(double note) {
  return pow(SIGMA, note-A4_NUMBER) * A4_FREQUENCY;
}

static inline double freqToNote(double freq) {
  /*freq = pow(SIGMA, note-69.0) * 440.0;
  freq / 440.0 = pow(SIGMA, note-69.0)
  SIGMA^(note-69.0) = freq / 440.0
  log SIGMA^(note-69.0) = log freq / 440.0
  (note-69.0) * log SIGMA = log freq / 440.0
  (note-69.0) = log(freq / 440.0) / log SIGMA*/

  return log(freq / A4_FREQUENCY) / LOG_SIGMA + A4_NUMBER;
}

static inline Vec2d freqToNoteAndCents(double freq) {
  double v = log(freq / A4_FREQUENCY) / LOG_SIGMA + A4_NUMBER;
  double p = round(v);
  double c = v - p;
  return Vec2d(p, c * 100.0);
}

static inline void noteInfoFromFrequency(double freq, char *buf) {
  double v = log(freq / A4_FREQUENCY) / LOG_SIGMA + A4_NUMBER;
  int p = round(v);
  int octave = p / 12 - 1;
  int cents = (int)((v-p)*100.0);

  std::sprintf(buf, "%s%d %s %d c, %.2f Hz", noteNames[(12+p%12)%12].c_str(), octave, cents >= 0 ? "+" : "-", abs(cents), freq);
}

static double getPitchAmplitudeFromFrequencySpectrum(double sampleRate, const std::vector<double> &frequencySpectrum, double minPitch, double maxPitch, double index, int destinationArraySize) {
  double noteBegin = map(index, 0.0, destinationArraySize-1, minPitch, maxPitch);
  double noteEnd = map(index+1, 0.0, destinationArraySize-1, minPitch, maxPitch);
  double frequencyMin = noteToFreq(noteBegin);
  double frequencyMax = noteToFreq(noteEnd);
  int f0 = (int)map(frequencyMin, 0.0, sampleRate/2.0, 0, frequencySpectrum.size()/2);
  int f1 = (int)map(frequencyMax, 0.0, sampleRate/2.0, 0, frequencySpectrum.size()/2);
  double t = 0;
  for(int k=f0; k<=f1; k++) {
    if(k < 0 || k >= frequencySpectrum.size()) {
      printf("Error at getPitchAmplitudeFromFrequencySpectrum, invalid frequency at spectrum as notes conversion, %d\n", k);
    }
    else {
      t += frequencySpectrum[k];
    }
  }
  t = t / (1.0 + f1 - f0);
  
  return t;
}



/*double noteBegin = map(i, 0.0, spectrumGraph.size()-1, minNote, maxNote);
double noteEnd = map(i+1, 0.0, spectrumGraph.size()-1, minNote, maxNote);
double frequencyMin = noteToFreq(noteBegin);
double frequencyMax = noteToFreq(noteEnd);
int f0 = (int)map(frequencyMin, 0.0, synth->sampleRate/2.0, 0, synth->delayLineFFTW.output.size()/2);
int f1 = (int)map(frequencyMax, 0.0, synth->sampleRate/2.0, 0, synth->delayLineFFTW.output.size()/2);
double t = 0;
for(int k=f0; k<=f1; k++) {
  if(k < 0 || k >= synth->delayLineFFTW.output.size()) {
    printf("Error at SynthSketch::onDraw(), invalid frequency at spectrum as notes conversion, %d\n", k);
  }
  else {
    t += synth->delayLineFFTW.output[k];
  }
}
t = t / (1.0 + f1 - f0);
spectrumGraph[i] = t;*/

static Vec2d getPitchAmplitudeFromFrequencySpectrum(double sampleRate, const std::vector<Vec2d> &frequencySpectrum, double minPitch, double maxPitch, double index, int destinationArraySize) {
  double noteBegin = map(index, 0.0, destinationArraySize-1, minPitch, maxPitch);
  double noteEnd = map(index+1, 0.0, destinationArraySize-1, minPitch, maxPitch);
  double frequencyMin = noteToFreq(noteBegin);
  double frequencyMax = noteToFreq(noteEnd);
  int f0 = (int)map(frequencyMin, 0.0, sampleRate/2.0, 0, frequencySpectrum.size()/2);
  int f1 = (int)map(frequencyMax, 0.0, sampleRate/2.0, 0, frequencySpectrum.size()/2);
  Vec2d t;
  for(int k=f0; k<=f1; k++) {
    if(k < 0 || k >= frequencySpectrum.size()) {
      printf("Error at getPitchAmplitudeFromFrequencySpectrum, invalid frequency at spectrum as notes conversion, %d\n", k);
    }
    else {
      t += frequencySpectrum[k];
    }
  }
  t /= 1.0 + f1 - f0;
  
  return t;
}

struct NoteInfo {
  double volume = 0;
  double time = 0;
  double frequency = 0;
  double absolutePitch = 0;
  int roundedPitch = 0;
  int centsDeviation = 0;
  int octave = 0;
  std::string noteName;

  void setFromPitch(double pitch) {
    this->absolutePitch = pitch;
    this->frequency = pow(SIGMA, absolutePitch-A4_NUMBER) * A4_FREQUENCY;;
    this->roundedPitch = round(absolutePitch);
    this->octave = roundedPitch / 12 - 1;
    this->centsDeviation = (int)((absolutePitch-roundedPitch)*100.0);
    this->noteName = noteNames[(12+roundedPitch%12)%12];
  }

  void setFromFrequency(double frequency) {
    this->frequency = frequency;
    this->absolutePitch = log(frequency / A4_FREQUENCY) / LOG_SIGMA + A4_NUMBER;
    this->roundedPitch = round(absolutePitch);
    this->octave = roundedPitch / 12 - 1;
    this->centsDeviation = (int)((absolutePitch-roundedPitch)*100.0);
    this->noteName = noteNames[(12+roundedPitch%12)%12];
  }

  std::string toString() {
    char buf[128];
    std::sprintf(buf, "%s%d %s %d c, %.2f Hz", noteName.c_str(), octave, centsDeviation >= 0 ? "+" : "-", abs(centsDeviation), frequency);
    return std::string(buf);
  }
};



/*const std::vector<double> noteFreqs;
noteFreqs.resize(1000);
void prepareNoteFreqs() {
  for(int i=0; i<1000; i++) {
    noteFreqs[i] = noteToFreq(i);
  }
}
static inline double noteToFreqLookup(int note) {
  return noteFreqs[note];
}
static inline double noteToFreqLookup(double note) {
  int i = floor(note);
  double f = note - i;
  return (noteFreqs[i+1] - noteFreqs[note]) * f + noteFreqs[note];
}*/

// own experimentally measured volume scale
inline double amplitudeToVol(double amplitude){
 return pow(amplitude, 0.4);
}

inline double amplitudeTodB(double amplitude){
 return 20.0 * log10(amplitude);
}

inline double dbToAmplitude(double db) {
  return pow(10.0, db/20.0);
}

/*static const int numFastNoiseTypes = 10;
static const char *fastNoiseTypeNames[numFastNoiseTypes] = {"Value", "Value fractal", "Perlin", "Perlin fractal", "Simplex", "Simplex fractal", "Cellular", "White noise", "Cubic", "Cubic fractal" };

static const int numFastNoiseFractalTypes = 3;
static const char *fastNoiseFractalTypeNames[numFastNoiseFractalTypes] = {"FBM", "Billow", "Rigid Multi"};

static const int numFastNoiseInterpolationTypes = 3;
static const char *fastNoiseInterpolationNames[numFastNoiseInterpolationTypes] = {"Linear", "Hermite", "Quintic"};

static const int numFastNoiseCellularDistanceFunctions = 3;
static const char *fastNoiseCellularDistanceFunctionNames[numFastNoiseCellularDistanceFunctions] = {"Euclidean", "Manhattan", "Natural"};

static const int numFastNoiseCellularReturnTypes = 8;
static const char *fastNoiseCellularReturnTypeNames[numFastNoiseCellularReturnTypes] = {"Cell value", "Noise lookup", "Distance", "Distance squared", "Distance squared (add)", "Dist squared (sub)", "Dist squared (mul)", "Dist squared (div)"};*/



enum OscillatorType { VoiceOscillator, AM, FM };

static const int maxNumModulators = 16, maxNumEnvelopes = 16, maxNumVoices = 16;

static const int maxUnison = 50;

//static const maxPostProcessingEffects = 64;

static const std::vector<std::string> voiceCrossModulationModeNames = { "None", "Frequency modulation", "Amplitude modulation", "Ring modulation" };
enum VoiceCrossModulationMode { CrossModulatationNone, CrossModulationFrequency, CrossModulationAmplitude, CrossModulationRing };





