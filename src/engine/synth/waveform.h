 #pragma once

#include "../geom_gfx.h"
#include "../gui/gui.h"
#include "../draggablegraph.h"
#include "audioplayer.h"
#include "../convolution.h"

#include <thread>



//#include "../pa_init.h"

/* TODO
 * seperate graph panel for partial gains and factors, probably same class for both
 * with draggable points
 *
 * This will allow somewhat intuitive approach to modifying the partials of the sound wave, but more importantly rapidly testing
 * probably huge/almost infinte universe of different sounding sounds
 *
 * Done. Seems valid.
 *
 * bugs:
 *  - troubles in parameter panel input (FIXED)
 *  - reorder points if dragged past each other (DONE)
 *  - prevent excessive volume at certain cases...
 *
 * - partial phases (custom, random, pattern)
 * - really long waveTables to capture partials with non mutually divisible frequencies
 *   - proably impossible to capture integer amount of cycles for arbitrary many of such partials, 
 *     so a way needed to parse together the two ends of the tabl
 *   - this could be accomplished with the pad synth algorithm from Zynsubaddfx
 *   - 3D graph (extra dimension for pitch dependence)
 *   - grid for graph, snap to grid (DONE)
 *   - linear interpolation option for graph, curvature option for cubic interpolation (DONE)
 *   - graph for pitch dependend gaussian smoothing
 *   - sepearate waveTables for each pitch
 *   - custom amount of cycles for waveTables (to little bit better capture non integer multiple partials)
 *
 *  Many of the things I am learning the "hard way". If things are going not as planned, then I have to zoom out and find other
 *  ways. There must be countless of ways, for example how different synths deal with that pitch dependance of the waveform.
 *  That's my next big challenge. So many good sounding waves, but can't be used in my current system but in only a limited range
 *  of piano keyes. That's annoying.
 *
 *  Another idea how to deal with that: partial attenuation matrix, with that graph interface. For each partial arbitrary scaling
 *  of less than one per octave.
 *
 *  Different scenarios can be handled in different ways.
 *    - Lowpass filters (I need to make the response curve more sharp to be useful here) are the easiest option. Those can be
 *      applied to every sound wave in after processing.
 *    - Other two options would be the custom partial attenuation factors (dependend on partial and pitch) and
 *      smoothing of the waveTable (dependend only on pitch). The former option couldn't help with other base waveforms than sin
 *      on it's own, even though it seems more elegant.
 *
 *  I think all of those can be generalized and abstracted in a way, that the user of the synth don't have to mess too much
 *  with adjusting the correct parameters. I think I try those all. 
 *
 *  I start with the "easiest option". I just need to make those filters more responsive.
 *
 *  Didn't have luck with the easiest option, but that waveTable smoothing based on pitch worked like charm. Need to make the update faster
 *
 *  Without this kind of pitch dependend processing, even the simple square wave is unusable with higher piano keys. No problem anymore!
 *
 *  Looking (or sounding) good. I think this was one of the last missing pieces that made my synth feel like little bit immature.
 *
 *  [Lot of work with pitch dependend partial atteanuator]
 *
 *  I think I accomplished what I tried, but I am not certain if it was worth all the toil. I am happy with this (the attenuation of higher partials) thing now.
 *
 *  Because, I am in more peace if I can make the higher notes sound boringly plain, than if they sound kind of broken.
 *
 *  That's the primary function of those pitch dependend attenuators: heal. Not create.
 *
 *  There was one crash bug somewhere, when activated pitchPartialAttenuations. Save/load needed also. Wavetable sizes...
 *
 *  ----
 *  
 *  I would like to be able to change synth parameters realtime, without noticeable clicks or other annoying sounds when updating the currently playing instrument.
 *  Now that there are possibly 128 waveTables (for each piano key, or pitch), that need to be updated every time a parameter is changed, it might take even a big part of
 *  a second to update 
 *
 *
 * Thread TODO
 * - find out about exceptions
 * - find out about thread pools
 * - fix file loading
 *
 */


static int numWaveTablePreparingThreads = 1;



struct PartialSet : public HierarchicalTextFileParser {
  static const int maxNumPartials = 100;
  static const int numPartialFactorPatternArgs = 6;
  static const int numPartialGainPatternArgs = 6;

  enum Pattern { None, Equation1, DraggingGraphs };
  const std::vector<std::string> patternNames = { "None", "Equation", "Graphs" };
  
  int numPartials = 1;
  std::vector<double> factors, gains;
  std::vector<double> factorParameters, gainParameters;
  
  std::vector<std::vector<double>> pitchPartialAttenuations;
  
  double totalGain = 0;
  std::vector<double> pitchTotalGains;

  
  Pattern pattern = None;
  

  virtual ~PartialSet() {}
  
  PartialSet() {
    reset();
  }
  
  void setNumPartials(int numPartials) {
    this->numPartials = numPartials;
  }
  
  void reset() {
    factors.resize(PartialSet::maxNumPartials);
    gains.resize(PartialSet::maxNumPartials);
    
    pitchPartialAttenuations.resize(128);
    pitchTotalGains.assign(128, 0);
    
    for(int i=0; i<128; i++) {
      pitchPartialAttenuations[i].assign(PartialSet::maxNumPartials, 1);
    }
    
    factorParameters = {1, 1, 0, 0, 1, 0};
    gainParameters = {0, 0, 0, 0, 0, 0};
    
    numPartials = 1;
  }

  PartialSet &operator=(const PartialSet &partialSet) {
    numPartials = partialSet.numPartials;
    factors = partialSet.factors;
    gains = partialSet.gains;

    totalGain = partialSet.totalGain;

    factorParameters = partialSet.factorParameters;
    gainParameters = partialSet.gainParameters;
    
    pitchPartialAttenuations = partialSet.pitchPartialAttenuations;
    
    pitchTotalGains = partialSet.pitchTotalGains;

    pattern = partialSet.pattern;
    
    return *this;
  }

  void applyPartialFactorPattern() {
    if(factorParameters.size() < 6) return;

    factors[0] = 1.0;
    for(int i=1; i<numPartials; i++) {
      factors[i] = factorParameters[0] * pow(factorParameters[1], i);
      factors[i] += factorParameters[2] * pow(i, factorParameters[3]);
      factors[i] += i * factorParameters[4] + factorParameters[5];
    }
  }
  void applyPartialGainPattern() {
    if(gainParameters.size() < 6) return;

    for(int i=0; i<numPartials; i++) {
      double x = (double)i/numPartials;
      double a = gainParameters[0];
      double b = gainParameters[1];
      double c = gainParameters[2];
      double d = gainParameters[3];
      double e = gainParameters[4];
      double t = gainParameters[5];
      double f = (1.0-t) * pow(1.0-x, a) + t;
      double g = c * pow(1.0-x, b) + t;
      double h = sin(d * 2.0*PI * x + e*2.0*PI + 0.5*PI) * 0.5 + 0.5;
      gains[i] = h * (f - g) + g;
    }
  }
  
  static std::string getClassName() {
    return "partials";
  }
  
  virtual std::string getBlockName() {
    return getClassName();
  }
  
  virtual void decodeParameters() {
    reset();
    
    std::string patternName;
    getStringParameter("pattern", patternName);
    for(int i=0; i<patternNames.size(); i++) {
      if(patternName == patternNames[i]) {
        pattern = (PartialSet::Pattern)i;
        break;
      }
    }
    getNumericParameter("count", numPartials);
    // FIXME get rid of me
    bool value = false;
    getNumericParameter("usePattern", value);
    if(value) pattern = Pattern::Equation1;
    
    if(pattern == Pattern::None) {
      getNumericParameter("frequencies", factors, false);
      getNumericParameter("gains", gains, false);
    }
    else if(pattern == Pattern::Equation1) {
      getNumericParameter("frequencyParameters", factorParameters, false);
      getNumericParameter("gainParameters", gainParameters, false);
      applyPartialGainPattern();
      applyPartialFactorPattern();
    }

  }

  virtual void encodeParameters() {
    clearParameters();
    putStringParameter("pattern", patternNames[pattern]);
    putNumericParameter("count", numPartials);
    //getNumericParameter("factorDraggingGraphRange", factorDraggingGraphRange);
    //getNumericParameter("gainDraggingGraphRange", gainDraggingGraphRange);
    if(pattern == Pattern::None) {
      putNumericParameter("frequencies", factors, numPartials);
      putNumericParameter("gains", gains, numPartials);
    }
    else if(pattern == Pattern::Equation1) {
      putNumericParameter("frequencyParameters", factorParameters);
      putNumericParameter("gainParameters", gainParameters);
    }
  }
  
};



struct PartialPreset {
  enum Type { Partials, Func };
  std::vector<double> factorArgs, gainArgs;
  std::string name;
  Type type;

  PartialPreset() {}

  PartialPreset(const std::string &name, const std::vector<double> &factorArgs, const std::vector<double> &gainArgs, Type type = Func) {
    this->name = name;
    this->type = type;
    this->factorArgs = std::vector<double>(factorArgs);
    this->gainArgs = std::vector<double>(gainArgs);
  }
};

static const PartialPreset ppDefault("Default", {1, 1, 0, 0, 1, 0}, {1, 0, 0, 0, 0});
static const PartialPreset ppVocal01("Vocal 01", {0, 0, 5, 0.09, 0, 0}, {1, 0, 0, 1, 0.5});
static const PartialPreset ppVocal02("Vocal 02", {0, 0, 2.3, 0.3, 0, 0}, {0.007, 0, 0, 0, 0.});
static const PartialPreset ppVocal03("Vocal 03", {0, 0, 2, 0.3, 0, 0}, {0., 0, 0, 1, 0.5});
static const PartialPreset ppVocal04("Vocal 04", {0, 0, 6.4, 0.15, 0, 0}, {0., 0, 0, 1, 0.5});
static const PartialPreset ppMetallic01("Metallic 01", {0.963, 2, 0, 1, 1, 0}, {0.3, 0, 0, 1.5, 0.32});
static const PartialPreset ppMetallic02("Metallic 02", {0.586, 1.9, 0, 0.9, 1, 0}, {2, 0, 0, 8.56, 0});
static const PartialPreset ppMetallic03("Metallic 03", {0.5, 2, 0, 1, 1, 0}, {2, 0, 0, 3.7, 0});
static const PartialPreset ppMetallic04("Metallic 04", {0., 0, 0.5, 1.8, 1, 0}, {1, 0, 0, 2, 0.5});
static const PartialPreset ppNameless01("Nameless 01", {0, 0, 0,1, 2.8, 1, 0}, {10, 0, 0, 0, 0});
static const PartialPreset ppNameless02("Nameless 02", {0, 0, 0,1, 2.8, 1, 0}, {10, 0, 0, 2.8, 0});
static const PartialPreset ppNameless03("Nameless 03", {0, 0, 0,1, 2.8, 1, 0}, {10, 0, 0, 5.33, 0});
static const PartialPreset ppNameless04("Nameless 04", {0, 0, 0,1, 2.8, 1, 0}, {10, 0, 0, 12.3, 0});
static const PartialPreset ppNameless05("Nameless 05", {1, 1, 0, 0, 1, 0}, {7, 0, 0, 1, 0.2});
static const PartialPreset ppNameless06("Nameless 06", {0, 0, 0, 0, 0.2, 1}, {1, 0, 0, 0, 0});
static const PartialPreset ppNameless07("Nameless 07", {1, 1, 0, 0, 1, 0}, {5, 0, 0, 0, 0});
static const PartialPreset ppCrystal01("Crystal 01", {1, 3, 0, 1, 1, 0}, {10, 0, 0, 15, 0, 0.05});
static const PartialPreset ppBuzzy01("Buzzy 01", {1, 1, 0, 1, 1, 0}, {1, 0, 0, 0, 0, 1});
static const PartialPreset ppBuzzy02("Buzzy 02", {1, 1, 0, 1, 1, 0}, {50, 0, 0, 0, 0, 0.03});

static const PartialPreset ppOrgan01("Organ 01", {1, 2, 0, 0, 0, 0}, {1, 0, 0, 0, 0, 0});

static const PartialPreset ppOboe01("Oboe 01", {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16}, {0.6, 0.5, 0.78, 1.0, 0.7, 0.55, 0.67, 0.54, 0.26, 0.2, 0.18, 0.45, 0.18, 0.22, 1.3, 0.25}, PartialPreset::Type::Partials);

static const PartialPreset ppClarinet01("Clarinet 01", {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16}, {1, 0.23, 0.9, 0.4, 0.75, 0.1, 0.53, 0.1, 0.4, 0.16, 0.28, 0.05, 0.18, 0.02, 0.16, 0.01}, PartialPreset::Type::Partials);

static const PartialPreset ppFlute01("Flute 01", {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16}, {1, 1, 0.9, 0.5, 0.4, 0.35, 0.4, 0.3,   0.18, 0.13, 0.09, 0.05, 0.05, 0.035, 0.02, 0.01}, PartialPreset::Type::Partials);

static const PartialPreset ppFlute02("Flute 02", {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14}, {1.0000, 1.0000, 0.1995, 0.1259, 0.0794, 0.0200, 0.0355, 0.0100, 0.0126, 0.0126, 0.0141, 0.0063, 0.0045, 0.0040}, PartialPreset::Type::Partials); // C# 5


static const PartialPreset ppTrumpet01("Trumpet 01", {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16}, {0.65, 1, 0.57, 0.56, 0.4, 0.2, 0.42, 0.28, 0.22, 0.14, 0.23, 0.16, 0.19, 0.16, 0.15, 0.18}, PartialPreset::Type::Partials);


//static const PartialPreset ppPiano01("Piano 01", { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18 }, { 0.1778, 0.3162, 1.0000, 0.8913, 0.3162, 0.1413, 0.0708, 0.0891, 0.1585, 0.0708, 0.1000, 0.0708, 0.0794, 0.0708, 0.0794, 0.0708, 0.0708, 0.0794 }, PartialPreset::Type::Partials);

static const PartialPreset ppPiano01("Piano 01", { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 }, { 3, 2, 0.4, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1 }, PartialPreset::Type::Partials);

static const PartialPreset ppPiano02("Piano 02", { 0, 0, 0, 0, 0.5, 1 }, { 1, 0, 0, 0, 0});

static std::vector<PartialPreset> partialPresets = {
      ppDefault,
      ppCrystal01,
      ppBuzzy01, ppBuzzy02,
      ppMetallic01, ppMetallic02, ppMetallic03, ppMetallic04,
      ppVocal01, ppVocal02, ppVocal03, ppVocal04,
      ppOrgan01,
      ppPiano01, ppPiano02,
      ppOboe01,
      ppClarinet01,
      ppTrumpet01,
      ppFlute01, ppFlute02
    };




struct WaveForm : public PanelInterface, public HierarchicalTextFileParser
{

  
  // :D have accidentally made functionality which supports my goal. I'd like to refine that audioplayer class further so that one could
  // zoom into the audio wave and pick a specific part of the audio for the sample/wavetable synthesis

  struct SampleEditor : public AudioPlayer {
    WaveForm *waveForm = NULL;
    SampleEditor(WaveForm *waveForm, int sampleRate, int framesPerBuffer) : AudioPlayer(sampleRate, framesPerBuffer) {
      this->waveForm = waveForm;
      loopingMode = NoLooping;
      fileBrowser->setPath("data/synth/samples/");
      progressTrackListWaitingTime = 0.5;
    }
    
    virtual void onTrackLoaded(bool trackLoaded) override {
      printf("areogkẗe\n");
      
      if(trackLoaded && audioRecordingTrack.audioRecording.numVariableSamples > 0 && audioRecordingTrack.audioRecording.numVariableSamples <= audioRecordingTrack.audioRecording.samples.size()) {
        waveForm->sampleBuffer.resize(audioRecordingTrack.audioRecording.numVariableSamples);
        waveForm->sampleBufferLenghtInSeconds = audioRecordingTrack.audioRecording.getLengthInSeconds();
        
        for(long i=0; i<audioRecordingTrack.audioRecording.numVariableSamples; i++) {
          waveForm->sampleBuffer[i] = audioRecordingTrack.audioRecording.samples[i].x;
        }
        printf("Sample loaded, number of samples %lu, length %f seconds\n", audioRecordingTrack.audioRecording.numVariableSamples, audioRecordingTrack.audioRecording.getLengthInSeconds());
        waveForm->waveTableSize = waveForm->sampleBuffer.size() + 1; // +1 because wavetable contains first element twice, in the start and end.
                                                                     // There must have been good reason for such duplication, need to refresh
                                                                     // my memory some time!
        waveForm->prepareWaveTable();
        if(waveForm->waveTableSizeGui) {
          waveForm->waveTableSizeGui->setValue(waveForm->waveTableSize);
        }
      }
    }
  };
  
  
  // TODO apply arbitrary equation to the linear curve values
  struct FactorDraggingGraph : public DraggableGraph {
    struct FactorDraggingGraphParameterPanel : public Panel {
      FactorDraggingGraph *factorDraggingGraph = NULL;
      NumberBox *clampMinGui = NULL;
      NumberBox *factorMinGui = NULL;
      NumberBox *factorMaxGui = NULL;
      CheckBox *roundFactorsGui = NULL;
      NumberBox *roundToNearestFractionGui = NULL;
      //ListBox *equationGui = NULL;
      
      virtual ~FactorDraggingGraphParameterPanel() {}
      
      FactorDraggingGraphParameterPanel(FactorDraggingGraph *factorDraggingGraph) : Panel("Factor Dragging Graph") {
        init(factorDraggingGraph);
      }
      
      void init(FactorDraggingGraph *factorDraggingGraph) {
        this->factorDraggingGraph = factorDraggingGraph;
        if(!factorDraggingGraph->draggableGraphPanel) return;
        
        factorDraggingGraph->draggableGraphPanel->addChildElement(this);
        
        addGuiEventListener(new ParameterPanelListener(this));
        double line = -13, lineHeight = 23;
        //addChildElement(new Label(name, 10, line+=lineHeight));
        addChildElement(factorMinGui = new NumberBox("Min factor", factorDraggingGraph->verticalLimits.x, NumberBox::FLOATING_POINT, -1e10, 1e10, 10, line+=lineHeight, 8));
        addChildElement(factorMaxGui = new NumberBox("Max factor", factorDraggingGraph->verticalLimits.y, NumberBox::FLOATING_POINT, -1e10, 1e10, 10, line+=lineHeight, 8));
        addChildElement(roundFactorsGui = new CheckBox("Round", factorDraggingGraph->roundDraggingFactors, 10, line+=lineHeight));
        addChildElement(roundToNearestFractionGui = new NumberBox("Rounding fraction, 1/", factorDraggingGraph->roundToNearestDraggingFraction, NumberBox::INTEGER, 1, 1<<29, 10, line+=lineHeight, 8));
        addChildElement(clampMinGui = new NumberBox("Clamp min", factorDraggingGraph->clampMinDraggingFactor, NumberBox::FLOATING_POINT, 0, 1e10, 10, line+=lineHeight, 8));
        
        setSize(320, line+lineHeight+10);
        setPosition(0, -size.y-5);
      }
      
      struct ParameterPanelListener : public GuiEventListener {
        FactorDraggingGraphParameterPanel *parameterPanel;
        ParameterPanelListener(FactorDraggingGraphParameterPanel *parameterPanel) {
          this->parameterPanel = parameterPanel;
        }
        void onValueChange(GuiElement *guiElement) {
          if(guiElement == parameterPanel->factorMinGui) {
            guiElement->getValue((void*)&parameterPanel->factorDraggingGraph->verticalLimits.x);
            parameterPanel->factorDraggingGraph->update();
          }
          if(guiElement == parameterPanel->factorMaxGui) {
            guiElement->getValue((void*)&parameterPanel->factorDraggingGraph->verticalLimits.y);
            parameterPanel->factorDraggingGraph->update();
          }
          if(guiElement == parameterPanel->roundFactorsGui) {
            guiElement->getValue((void*)&parameterPanel->factorDraggingGraph->roundDraggingFactors);
            //parameterPanel->factorDraggingGraph->update();
            parameterPanel->factorDraggingGraph->onUpdate();
          }
          if(guiElement == parameterPanel->roundToNearestFractionGui) {
            guiElement->getValue((void*)&parameterPanel->factorDraggingGraph->roundToNearestDraggingFraction);
            parameterPanel->factorDraggingGraph->gridDensity = parameterPanel->factorDraggingGraph->roundToNearestDraggingFraction;
            parameterPanel->factorDraggingGraph->update();
            //parameterPanel->factorDraggingGraph->onUpdate();
          }
          if(guiElement == parameterPanel->clampMinGui) {
            guiElement->getValue((void*)&parameterPanel->factorDraggingGraph->clampMinDraggingFactor);
            parameterPanel->factorDraggingGraph->onUpdate();
          }
        }
      };
    };

    WaveForm *waveForm = NULL;
    
    bool roundDraggingFactors = false;
    int roundToNearestDraggingFraction = 1;
    double clampMinDraggingFactor = 1.0;
    bool isCurveSet = false;

    FactorDraggingGraphParameterPanel *parameterPanel = NULL;
    
    
    FactorDraggingGraph &operator=(const FactorDraggingGraph &p) {
      DraggableGraph::operator=(p);
      
      roundDraggingFactors = p.roundDraggingFactors;
      roundToNearestDraggingFraction = p.roundToNearestDraggingFraction;
      clampMinDraggingFactor = p.clampMinDraggingFactor;
      isCurveSet = p.isCurveSet;
      
      return *this;
    }

    FactorDraggingGraph() : DraggableGraph() {
      w = 400; h = 400;
      gridDensity = 1;
      title = "Partial factors";
      setCurvePoint(0, 0, 0);
      setCurvePoint(1, 1, 1);
      verticalLimits.set(1, 2);
    }

    void init(GuiElement *parentGuiElement, WaveForm *waveForm) {
      DraggableGraph::init(parentGuiElement);
      this->waveForm = waveForm;
      numDataPoints = waveForm->partialSet.numPartials;
      parameterPanel = new FactorDraggingGraphParameterPanel(this);
    }
    
    virtual void update() {
      if(waveForm) numDataPoints = waveForm->partialSet.numPartials;
      DraggableGraph::update();
    }
    
    virtual void onUpdate() {
      // FIXME
      if(waveForm) {
        printf("factor drag graph update\n");
        for(int i=0; i<min(numDataPoints, waveForm->partialSet.numPartials); i++) {
          waveForm->partialSet.factors[i] = getDataPointValue(i);
          if(roundDraggingFactors) {
            waveForm->partialSet.factors[i] = round(waveForm->partialSet.factors[i] * roundToNearestDraggingFraction) / (double)roundToNearestDraggingFraction;
          }
          waveForm->partialSet.factors[i] = max(clampMinDraggingFactor, waveForm->partialSet.factors[i]);
        }
        waveForm->prepareWaveTable();
        for(int i=0; i<waveForm->partialSet.numPartials; i++) {
          if(waveForm->partialFactorsGui[i]) {
            waveForm->partialFactorsGui[i]->setValue(waveForm->partialSet.factors[i]);
          }
        }
      }
    }
    
    static std::string getClassName() {
      return "factorDraggingGraph";
    }
    
    virtual std::string getBlockName() {
      return getClassName();
    }  
    
    virtual void decodeParameters() {
      DraggableGraph::decodeParameters();
      getNumericParameter("roundDraggingFactors", roundDraggingFactors);
      getNumericParameter("roundToNearestDraggingFraction", roundToNearestDraggingFraction);
      getNumericParameter("clampMinDraggingFactor", clampMinDraggingFactor);
    }

    virtual void encodeParameters() {
      clearParameters();
      putVectorParameter("yRange", verticalLimits);
      putVectorParameter("curve", curve);
      
      putNumericParameter("roundDraggingFactors", roundDraggingFactors);
      putNumericParameter("roundToNearestDraggingFraction", roundToNearestDraggingFraction);
      putNumericParameter("clampMinDraggingFactor", clampMinDraggingFactor);
    }

    
  };
  
  
  
  struct GainDraggingGraph : public DraggableGraph {
    struct GainDraggingGraphParameterPanel : public Panel {
      GainDraggingGraph *gainDraggingGraph = NULL;
      NumberBox *gainMinGui = NULL;
      NumberBox *gainMaxGui = NULL;
      
      virtual ~GainDraggingGraphParameterPanel() {}
      
      GainDraggingGraphParameterPanel(GainDraggingGraph *gainDraggingGraph) : Panel("Gain Dragging Graph")  {
        init(gainDraggingGraph);
      }
      
      void init(GainDraggingGraph *gainDraggingGraph) {
        this->gainDraggingGraph = gainDraggingGraph;
        if(!gainDraggingGraph->draggableGraphPanel) return;
        
        gainDraggingGraph->draggableGraphPanel->addChildElement(this);
        addGuiEventListener(new ParameterPanelListener(this));
        double line = -13, lineHeight = 23;

        addChildElement(gainMinGui = new NumberBox("Min gain", gainDraggingGraph->verticalLimits.x, NumberBox::FLOATING_POINT, -1e6, 1e6, 10, line+=lineHeight, 8));
        addChildElement(gainMaxGui = new NumberBox("Max gain", gainDraggingGraph->verticalLimits.y, NumberBox::FLOATING_POINT, -1e10, 1e10, 10, line+=lineHeight, 8));
        
        setSize(320, line+lineHeight+10);
        setPosition(0, -size.y-5);
      }
      struct ParameterPanelListener : public GuiEventListener {
        GainDraggingGraphParameterPanel *parameterPanel;
        ParameterPanelListener(GainDraggingGraphParameterPanel *parameterPanel) {
          this->parameterPanel = parameterPanel;
        }
        void onValueChange(GuiElement *guiElement) {
          if(guiElement == parameterPanel->gainMinGui) {
            guiElement->getValue((void*)&parameterPanel->gainDraggingGraph->verticalLimits.x);
            parameterPanel->gainDraggingGraph->onUpdate();
          }
          if(guiElement == parameterPanel->gainMaxGui) {
            guiElement->getValue((void*)&parameterPanel->gainDraggingGraph->verticalLimits.y);
            parameterPanel->gainDraggingGraph->onUpdate();
          }
        }
      };
    };
    
    WaveForm *waveForm = NULL;
    
    GainDraggingGraphParameterPanel *parameterPanel = NULL;

    GainDraggingGraph &operator=(const GainDraggingGraph &p) {
      DraggableGraph::operator=(p);
      return *this;
    }

    GainDraggingGraph() : DraggableGraph() {
      w = 400; h = 400;
      title = "Partial gains";
      setCurvePoint(0, 0, 1);
      setCurvePoint(1, 1, 1);
      verticalLimits.set(0, 1);
    }
    
    void init(GuiElement *parentGuiElement, WaveForm *waveForm) {

      DraggableGraph::init(parentGuiElement);
      this->waveForm = waveForm;
      numDataPoints = waveForm->partialSet.numPartials;
      parameterPanel = new GainDraggingGraphParameterPanel(this);
    }
    
    virtual void update() {
      if(waveForm) numDataPoints = waveForm->partialSet.numPartials;
      DraggableGraph::update();
    }
      

    virtual void onUpdate() {
      if(waveForm) {
        for(int i=0; i<min(numDataPoints, waveForm->partialSet.numPartials); i++) {
          waveForm->partialSet.gains[i] = getDataPointValue(i);
        }
        waveForm->prepareWaveTable();
        for(int i=0; i<waveForm->partialSet.numPartials; i++) {
          if(waveForm->partialGainsGui[i]) {
            waveForm->partialGainsGui[i]->setValue(waveForm->partialSet.gains[i]);
          }
        }
      }
    }
    
    static std::string getClassName() {
      return "gainDraggingGraph";
    }
    
    virtual std::string getBlockName() {
      return getClassName();
    }  
  };
  
  
  
  struct PitchSmoothingnDraggingGraph : public DraggableGraph {
    WaveForm *waveForm = NULL;
    
    PitchSmoothingnDraggingGraph() : DraggableGraph() {
      w = 600; h = 200;
      title = "Pitch smoothing";
      numDataPoints = 128;
      
      setCurvePoint(0, 0, 0);
      setCurvePoint(1, 1, 0);

      setVerticalLimits(0, 1);
    }
    
    void init(GuiElement *parentGuiElement, WaveForm *waveForm) {
      DraggableGraph::init(parentGuiElement);
      this->waveForm = waveForm;
    }
    
    
      
    virtual void onUpdate() {
      //printf("(debugging) at PitchSmoothingnDraggingGraph::onUpdate() 1...\n");
      //printf("on PitchSmoothingnDraggingGraph::onUpdate() %d - usePitchDependendGaussianSmoothing %d - %d\n", waveForm != NULL, waveForm != NULL && waveForm->usePitchDependendGaussianSmoothing, numDataPoints);
      if(waveForm && waveForm->usePitchDependendGaussianSmoothing) {
        //printf("pitch smoothing graph update\n");
        /*if(waveForm->pitchSmoothings.size() != 128) {
          printf("Warning at PitchSmoothingnDraggingGraph::onUpdate(), waveForm->pitchSmoothings.size() == %lu != 128\n", waveForm->pitchSmoothings.size());
          waveForm->pitchSmoothings.resize(128); // Crashed several times at this point!!!(???)
        }*/

        for(int i=0; i<min(numDataPoints, waveForm->pitchSmoothings.size()); i++) {
          waveForm->pitchSmoothings[i] = getDataPointValue(i);
          //printf("|| %d  %f\n", i, waveForm->pitchSmoothings[i]);
        }
        //printf("(debugging) at PitchSmoothingnDraggingGraph::onUpdate() 2...\n");
        waveForm->prepareWaveTable();
      }
      //printf("(debugging) at PitchSmoothingnDraggingGraph::onUpdate() 3...\n");
    }
    
    static std::string getClassName() {
      return "pitchSmoothingsDraggingGraph";
    }
    
    virtual std::string getBlockName() {
      return getClassName();
    }
  };
  
  
  
  
  struct PartialAttenuationDraggingGraph : public DraggableGraph {
    
    struct PartialAttenuationDraggingGraphParameterPanel : public Panel {
      PartialAttenuationDraggingGraph *partialAttenuationDraggingGraph = NULL;
      NumberBox *exponentialAttenuationFactor1Gui = NULL;
      NumberBox *exponentialAttenuationFactor2Gui = NULL;
      NumberBox *linearAttenuationFactorGui = NULL;
      CheckBox *instantUpdateGui = NULL;
      //ListBox *equationGui = NULL;
      
      virtual ~PartialAttenuationDraggingGraphParameterPanel() {}
      
      PartialAttenuationDraggingGraphParameterPanel(PartialAttenuationDraggingGraph *partialAttenuationDraggingGraph) : Panel("Partial Attenuation Dragging Graph")  {
        init(partialAttenuationDraggingGraph);
      }
      
      void init(PartialAttenuationDraggingGraph *partialAttenuationDraggingGraph) {
        this->partialAttenuationDraggingGraph = partialAttenuationDraggingGraph;
        if(!partialAttenuationDraggingGraph->draggableGraphPanel) return;
        
        partialAttenuationDraggingGraph->draggableGraphPanel->addChildElement(this);
        addGuiEventListener(new ParameterPanelListener(this));
        double line = -13, lineHeight = 23;
        //addChildElement(new Label(name, 10, line+=lineHeight));
        addChildElement(exponentialAttenuationFactor1Gui = new NumberBox("Exponential attenuation 1", partialAttenuationDraggingGraph->exponentialAttenuationFactor1, NumberBox::FLOATING_POINT, 0, 1e6, 10, line+=lineHeight, 8));
        addChildElement(exponentialAttenuationFactor2Gui = new NumberBox("Exponential attenuation 2", partialAttenuationDraggingGraph->exponentialAttenuationFactor2, NumberBox::FLOATING_POINT, 0, 1e6, 10, line+=lineHeight, 8));
        addChildElement(linearAttenuationFactorGui = new NumberBox("Linear attenuation", partialAttenuationDraggingGraph->linearAttenuationFactor, NumberBox::FLOATING_POINT, 0, 1e10, 10, line+=lineHeight, 8));
        
        //addChildElement(instantUpdateGui = new CheckBox("Update while dragging", partialAttenuationDraggingGraph->instantUpdate, 10, line+=lineHeight));
        
        setSize(320, line+lineHeight+10);
        setPosition(0, -size.y-5);
      }
      struct ParameterPanelListener : public GuiEventListener {
        PartialAttenuationDraggingGraphParameterPanel *parameterPanel;
        ParameterPanelListener(PartialAttenuationDraggingGraphParameterPanel *parameterPanel) {
          this->parameterPanel = parameterPanel;
        }
        void onValueChange(GuiElement *guiElement) {
          if(guiElement == parameterPanel->exponentialAttenuationFactor1Gui) {
            guiElement->getValue((void*)&parameterPanel->partialAttenuationDraggingGraph->exponentialAttenuationFactor1);
            parameterPanel->partialAttenuationDraggingGraph->onUpdate();
          }
          if(guiElement == parameterPanel->exponentialAttenuationFactor2Gui) {
            guiElement->getValue((void*)&parameterPanel->partialAttenuationDraggingGraph->exponentialAttenuationFactor2);
            parameterPanel->partialAttenuationDraggingGraph->onUpdate();
          }
          if(guiElement == parameterPanel->linearAttenuationFactorGui) {
            guiElement->getValue((void*)&parameterPanel->partialAttenuationDraggingGraph->linearAttenuationFactor);
            parameterPanel->partialAttenuationDraggingGraph->onUpdate();
          }
          if(guiElement == parameterPanel->instantUpdateGui) {
            guiElement->getValue((void*)&parameterPanel->partialAttenuationDraggingGraph->instantUpdate);
          }
        }
      };
    };
    
    virtual void onDraw(GeomRenderer &geomRenderer, TextGl &textRenderer, const Vec2d &displacement) {
      geomRenderer.strokeType = 1;
      geomRenderer.strokeWidth = 2;
      geomRenderer.strokeColor.set(0.5, 0.5, 0.5, 1.0);
      double x1, y1, x2, y2;
      int prevPartial = -1;
      for(int partial=0; partial<waveForm->partialSet.numPartials; partial++) {
        //int partial = (int)map(i, 0.0, min(50, waveForm->partialSet.numPartials)-1, 0.0, waveForm->partialSet.numPartials-1);
        if(partial != waveForm->partialSet.numPartials-1 && prevPartial >= 0 && waveForm->partialSet.factors[partial] / waveForm->partialSet.factors[prevPartial] < 2.0) continue;
        prevPartial = partial;
        for(int pitch=0; pitch<128; pitch++) {
          x1 = map(pitch, 0.0, 127.0, 0, w-1.0);
          y1 = h-1 - waveForm->partialSet.pitchPartialAttenuations[pitch][partial] * (h-1);
          if(pitch > 0) {
            geomRenderer.drawLine(x1, y1, x2, y2, displacement);
          }
          x2 = x1;
          y2 = y1;
        }
      }
    }
    
    double exponentialAttenuationFactor1 = 1.0;
    double exponentialAttenuationFactor2 = 1.0;
    double linearAttenuationFactor = 0.0;
    
    bool instantUpdate = true;
    bool updateNeeded = false;
    
    WaveForm *waveForm = NULL;
    
    PartialAttenuationDraggingGraph &operator=(const PartialAttenuationDraggingGraph &p) {
      DraggableGraph::operator=(p);
      
      exponentialAttenuationFactor1 = p.exponentialAttenuationFactor1;
      exponentialAttenuationFactor2 = p.exponentialAttenuationFactor2;
      linearAttenuationFactor = p.linearAttenuationFactor;
      
      instantUpdate = p.instantUpdate;
      updateNeeded = p.updateNeeded;
      return *this;
    }
    
    PartialAttenuationDraggingGraphParameterPanel *parameterPanel = NULL;
    
    void onMouseReleased(const Events &events) {
      DraggableGraph::onMouseReleased(events);
      
      if(updateNeeded) {
        waveForm->prepareWaveTable();
        updateNeeded = false;
      }
    }
    
    
    PartialAttenuationDraggingGraph() : DraggableGraph() {
      title = "Pitch partial attenuation";
      w = 600; h = 200;
      drawDataPointLines = false;
      //dataPointMarkSize = 3;
      
      numDataPoints = 128;
      
      setCurvePoint(0, 0, 1);
      setCurvePoint(1, 1, 1);

      setVerticalLimits(0, 1);
    }
    
    void init(GuiElement *parentGuiElement, WaveForm *waveForm) {
      DraggableGraph::init(parentGuiElement);
      this->waveForm = waveForm;
      
      parameterPanel = new PartialAttenuationDraggingGraphParameterPanel(this);
    }
    
    virtual void onUpdate() {
      if(waveForm && waveForm->usePitchDependendPartialAttenuation) {
        printf("partial attenuation graph update\n");
        int maxPartial = 1;
        for(int i=0; i<waveForm->partialSet.numPartials; i++) {
          maxPartial = max(maxPartial, ceil(waveForm->partialSet.factors[i]));
        }
        if(waveForm->partialAttenuationValues.size() != 128) {
          waveForm->partialAttenuationValues.resize(128);
        }
        if(waveForm->partialAttenuationValues[0].size() != maxPartial) {
          for(int i=0; i<128; i++) {
            waveForm->partialAttenuationValues[i].resize(maxPartial);
          }
        }
        for(int i=0; i<min(numDataPoints, 128); i++) {
          double value = getDataPointValue(i);
          for(int k=0; k<maxPartial; k++) {
            if(k == 0) {
              waveForm->partialAttenuationValues[i][k] = 1.0;
            }
            else {
              waveForm->partialAttenuationValues[i][k] = pow(value/(1.0+linearAttenuationFactor), 1.0+(pow(k-1.0, 1.0+exponentialAttenuationFactor2)*exponentialAttenuationFactor1));
             }
          }
        }
        
        for(int i=0; i<128; i++) {
          for(int k=0; k<waveForm->partialSet.numPartials; k++) {
            int partial = clamp((int)waveForm->partialSet.factors[k] - 1, 0, maxPartial-1);
            double f = waveForm->partialSet.factors[k] - partial;
            double a = waveForm->partialAttenuationValues[i][clamp(partial, 0, maxPartial-1)];
            double b = waveForm->partialAttenuationValues[i][clamp(partial+1, 0, maxPartial-1)];
            waveForm->partialSet.pitchPartialAttenuations[i][k] = clamp(a * (1.0-f) + b*f, 0.0, 1.0);
          }
        }
        
        if(instantUpdate) {
          waveForm->prepareWaveTable();
        }
        else {
          updateNeeded = true;
        }
      }
    }
    
    static std::string getClassName() {
      return "pitchPartialAttenuationDraggingGraph";
    }
    
    virtual std::string getBlockName() {
      return getClassName();
    }  

  };

  struct PitchGainDraggingGraph : public DraggableGraph {
    WaveForm *waveForm = NULL;
    
    PitchGainDraggingGraph() : DraggableGraph() {
      w = 600; h = 200;
      title = "Pitch gain";
      numDataPoints = 128;
      
      setCurvePoint(0, 0, 1);
      setCurvePoint(1, 1, 1);

      setVerticalLimits(0, 1);
    }
    
    void init(GuiElement *parentGuiElement, WaveForm *waveForm) {
      DraggableGraph::init(parentGuiElement);
      this->waveForm = waveForm;
    }
    
    virtual void onUpdate() {

      if(waveForm && waveForm->usePitchDependendGain) {
        if(waveForm->pitchDependendGains.size() != 128) {
          waveForm->pitchDependendGains.resize(128);
          printf("Warning at PitchGainDraggingGraph::onUpdate(), waveForm->pitchDependendGains.size() != 128\n");
        }
        for(int i=0; i<min(numDataPoints, waveForm->pitchDependendGains.size()); i++) {
          waveForm->pitchDependendGains[i] = pow(getDataPointValue(i), 2.0);
          //printf("%d -> %f\n", i, waveForm->pitchDependendGains[i]);
        }
      }
    }
    
    static std::string getClassName() {
      return "pitchGainDraggingGraph";
    }
    
    virtual std::string getBlockName() {
      return getClassName();
    }
  };


  /*struct HarmonicBandwidthDraggingGraph : public DraggableGraph {
    WaveForm *waveForm = NULL;
    
    HarmonicBandwidthDraggingGraph() : DraggableGraph() {
      w = 600; h = 200;
      title = "Harmonic bandwidth";
      numDataPoints = 2048;
      
      setCurvePoint(0, 0, 1);
      setCurvePoint(1, 1, 1);

      setVerticalLimits(0, 1);
    }
    
    void init(GuiElement *parentGuiElement, WaveForm *waveForm) {
      DraggableGraph::init(parentGuiElement);
      this->waveForm = waveForm;
    }
    
    static std::string getClassName() {
      return "harmonicBandwidthDraggingGraph";
    }
    
    virtual std::string getBlockName() {
      return getClassName();
    }
  };*/

  
  void setNumPartials(int numPartials) {
    partialSet.setNumPartials(numPartials);
    
    if(partialSet.pattern == PartialSet::Pattern::DraggingGraphs) {
      factorDraggingGraph.setNumDataPoints(numPartials);
      gainDraggingGraph.setNumDataPoints(numPartials);
    }
    
    if(usePitchDependendPartialAttenuation) {
      partialAttenuationDraggingGraph.update();
    }
    if(usePitchDependendGaussianSmoothing) {
      pitchSmoothingnDraggingGraph.update();
    }
    if(usePitchDependendGain) {
      pitchGainDraggingGraph.update();
    }
  }
  
  void setPartialDraggingGraphsVisible(bool visible) {
    factorDraggingGraph.setVisible(visible);
    gainDraggingGraph.setVisible(visible);
    if(gainDraggingGraph.draggableGraphPanel->pos == factorDraggingGraph.draggableGraphPanel->pos) {
      gainDraggingGraph.draggableGraphPanel->pos += Vec2d(60, 60);
    }
  }
  
  void initDraggableGraphs(GuiElement *parentGuiElement) {
    factorDraggingGraph.init(parentGuiElement, this);
    gainDraggingGraph.init(parentGuiElement, this);
    pitchSmoothingnDraggingGraph.init(parentGuiElement, this);
    pitchSmoothingnDraggingGraph.setVisible(false);
    partialAttenuationDraggingGraph.init(parentGuiElement, this);
    partialAttenuationDraggingGraph.setVisible(false);
    pitchGainDraggingGraph.init(parentGuiElement, this);
    pitchGainDraggingGraph.setVisible(false);
    setPartialDraggingGraphsVisible(false);
  }

  void updatePartialDraggingGraphs() {
    if(partialSet.pattern == PartialSet::Pattern::DraggingGraphs) {
      factorDraggingGraph.update();
      gainDraggingGraph.update();
    }
  }



  struct WaveformGraphPanelListener : public GuiEventListener {
    GraphPanel *graphPanel = NULL;
    WaveForm *waveForm = NULL;
    bool draggingPitch = false;
    //double draggingTmp = 0;
    WaveformGraphPanelListener(WaveForm *waveForm, GraphPanel *graphPanel) {
      this->waveForm = waveForm;
      this->graphPanel = graphPanel;
    }
    virtual void onMousePressed(GuiElement *guiElement, Events &events) {
      //printf("%d\n", events.mouseButton);
      if(guiElement == graphPanel && graphPanel->isPointWithin(events.m)) {
        if(events.mouseButton == 1) {
          waveForm->graphPitchhWaveTableIndex = (int)clamp(map(events.m.x, graphPanel->absolutePos.x, graphPanel->absolutePos.x + graphPanel->size.x, 0, 128), 0, 127);
          waveForm->updateWaveTableGraphs();
          draggingPitch = true;
          //draggingTmp = waveForm->graphPitchhWaveTableIndex;
        }
        /*if(events.mouseButton == 2) {
          waveForm->showPitchWaveTable = !waveForm->showPitchWaveTable;
          waveForm->updateWaveTableGraphs();
        }*/
      }
    }
    virtual void onMouseReleased(GuiElement *guiElement, Events &events) {
      draggingPitch = false;
    }
    virtual void onMouseMotion(GuiElement *guiElement, Events &events) {
      if(draggingPitch) {
        int prevIndex = waveForm->graphPitchhWaveTableIndex;
        waveForm->graphPitchhWaveTableIndex = (int)clamp(map(events.m.x, graphPanel->absolutePos.x, graphPanel->absolutePos.x + graphPanel->size.x-1, 0, 128), 0, 127);
        if(waveForm->graphPitchhWaveTableIndex != prevIndex) {
          waveForm->updateWaveTableGraphs();
        }
      }
    }
    virtual void onMouseWheel(GuiElement *guiElement, Events &events) {
      if(guiElement == graphPanel && graphPanel->isPointWithin(events.m)) {
        int k = events.lControlDown ? 16 : 1;
        waveForm->graphPitchhWaveTableIndex = (waveForm->graphPitchhWaveTableIndex+128 + events.mouseWheel*k) % 128;
        waveForm->showPitchWaveTable = true;
        waveForm->updateWaveTableGraphs();
      }
    }
  };
  
  
  /* Few options how to deal with the requirement I would like to fulfill, that instrument parameters could be changed realtime, without any annoying clicks in the sound
   * that is playing.
   * The new waveTable should be updated in the background, and perhaps smoothly transitioned with interpolation from the previous one.
   * One option would be to update first smaller waveTables, which could be done in a fraction of the time, that what would take for the primary one, and the transition
   * would be more responsive. I am taking into concideration the worst case scenario, that I am having waveTables for every 128 possible pitches with different partial
   * attenuations and different gaussian smoothings. It might take even a second to update. There might be lot of optimizations to make it faster as such also.
   *
   * Too tired...
   *
   */
  
  const std::vector<std::string> typeNames = { "Sin", "Triangle", "Square", "Saw", "Wave01", "Wave02", "Wave03", "Wave04", "Wave05", "Wave06", "Wave07", "Wave08", "Wave09", "Wave10", "Wave11", "Wave12", "Wave13", "Wave14", "White noise", "Gradient noise", "Power", "Sinc", "Sample", "Sin bands" };
  enum Type { Sin, Triangle, Square, Saw, Wave01, Wave02, Wave03, Wave04, Wave05, Wave06, Wave07, Wave08, Wave09, Wave10, Wave11, Wave12, Wave13, Wave14, WhiteNoise, GradientNoise, Power, Sinc, Sample, SinBands, numTypes };

  // not applicable for types: sample, sin bands
  const std::vector<std::string> waveTableModeNames = { "Single", "Single faster", "Each partial", "Each partial faster", "None" };
  enum WaveTableMode { Single, SingleFaster, ForEachPartial, ForEachPartialFaster, None };

  const std::vector<std::string> phaseModeNames = { "Random in range", "Zeros in range", "First zero", "Left and right"};
  enum PhaseMode { AnyWithinRange, ZerosWithinRange, FirstZero, LeftAndRight };
  
  

  struct FastNoiseWaveform : public FastNoise {
    static std::string getClassName() {
      return "fastNoiseWaveform";
    }
  };
  
  const int maxWaveTableSize = 1<<20;
  const int minWaveTableSize = 4;
  const int maxPitchWaveTableSize = 1<<17;

  static const int maxNumArgs = 20;

  const std::vector<int> antiAliasingPitches = {0, 60, 72, 84, 96, 108, 128};
  const int antiAliasingCount = 6;


  
  Type type = Type::Sin;

  double waveTableFrequency = 1.0;
  
  
  int waveTableSize = 2048, waveTableSizeM1 = 2048 - 1;
  long waveTableSizeDefault = 2048;
  
  long sampleWaveTableSize = 2048;
  
    
  WaveTableMode waveTableMode = Single;

  
  std::vector<double> waveTable;
  std::vector<std::vector<double>> pitchWaveTables;

  std::vector<double> waveTableNew;
  std::vector<std::vector<double>> pitchWaveTablesNew;
  
  bool updateWaveTablesOnBackground = false;

  TicToc preparationTimer;
  
  std::vector<std::vector<double>> waveFormArgs;
  std::vector<int> numWaveFormArgs;
  
  std::vector<double> waveTableGraphLeft, waveTableGraphRight;

  PhaseMode phaseMode = FirstZero;
  std::vector<double> phaseZeroPoints;
  std::vector<std::vector<double>> phaseZeroPointsHB = std::vector<std::vector<double>>(antiAliasingCount);

  
  Vec2d phaseStartLimits;
  bool sameLeftAndRightPhase = true;

  PartialSet partialSet;
  


  FastNoiseWaveform fastNoise;
  FastNoise fastNoiseCellularLookup;

  int numSteps = 1;
  
  bool useGaussianSmoothing = false;
  double smoothingWindowLengthInCycles = 0;

  std::vector<double> pitchSmoothings = std::vector<double>(128, 0);
  bool usePitchDependendGaussianSmoothing = false;
  bool usePitchDependendPartialAttenuation = false;
  
  std::vector<std::vector<double>> partialAttenuationValues;
  
  
  std::vector<double> pitchDependendGains = std::vector<double>(128, 1.0);
  bool usePitchDependendGain = false;
  
  
  bool stopAllNotesRequested = false;
  
  double waveValueOffset = 0;
  
  std::vector<std::thread> *waveTablePreparingThreads = NULL;
  bool isUpdating = false;
  
  bool waveTablePreparationStopRequested = false;

  std::vector<double> waveformGraphLeft, waveformGraphRight;

  
  int graphPitchhWaveTableIndex = 60;
  bool showPitchWaveTable = true;
  
  bool readyToPrepareWaveTable = true;

  
  enum WaveTableSeamMode { NoSeam, LinearSeam /* etc. */};
  const std::vector<std::string> waveTableSeamModeNames = { "No seam", "Linear seam"};
  WaveTableSeamMode waveTableSeamMode = NoSeam;

  double cyclesPerWaveTable = 1;
  
  
  int minHarmonicSmoothing = 1, maxHarmonicSmoothing = 100;
  double harmonicSmoothingSlope = 1;
  double harmonicSmoothingPower = 1;
  
  long harmonicBandWaveTableSize = 1 << 20;
  FFTW3Interface harmonicBandForwardFFTW, harmonicBandFReverseFFTW;
  Convolution *harmonicBandConvolution = NULL;
  std::vector<double> tempTableIn, tempTableOut;
  bool harmonicBandConvolutionChanged = true;
  double harmonicBandCyclesPerWaveTable = 1000;

  bool useHarmonicBandAntiAliasing = true;
  std::vector<std::vector<double>> harmonicBandWaveTables = std::vector<std::vector<double>>(6);
  //std::vector<int> antiAliasingPitches = {108, 96, 84, 72, 60, 0};
  
  std::vector<double> harmonicBandPhases;
  
  
  
  
  // TODO stereo sample wavetables
  double sampleBufferLenghtInSeconds = 0;
  std::vector<double> sampleBuffer = std::vector<double>(1, 0);
  SampleEditor *sampleEditor = NULL;
  int sampleRate = 0, framesPerBuffer = 0;
  

  FactorDraggingGraph factorDraggingGraph;
  GainDraggingGraph gainDraggingGraph;
  PitchSmoothingnDraggingGraph pitchSmoothingnDraggingGraph;
  PartialAttenuationDraggingGraph partialAttenuationDraggingGraph;
  PitchGainDraggingGraph pitchGainDraggingGraph;
  //HarmonicBandwidthDraggingGraph harmonicBandwidthDraggingGraph;
  

  double oscillatorFrequencyFactor = 1.0;


  void prepareHarmonicBandConvolution() {
    if(harmonicBandConvolutionChanged) {
      if(harmonicBandConvolution) delete harmonicBandConvolution;
      
      harmonicBandConvolution = new VariableGaussianConvolution(harmonicBandCyclesPerWaveTable/1000.0*minHarmonicSmoothing,
                                                                harmonicBandCyclesPerWaveTable/1000.0*maxHarmonicSmoothing, [this](double t) -> double {
        //return harmonicBandwidthDraggingGraph.getDataPointValue(clamp(t*numDataPoints, 0, numDataPoints-1);
        if(partialSet.numPartials <= 1) return 0;
        
        double a = partialSet.factors[0] * harmonicBandCyclesPerWaveTable / (harmonicBandWaveTableSize-1.0);
        double b = partialSet.factors[partialSet.numPartials-1] * harmonicBandCyclesPerWaveTable / (harmonicBandWaveTableSize-1.0);
        
        if(a == b) return 0;
        
        double x = (t-a) / (b-a);
        
        return pow(x * harmonicSmoothingSlope, harmonicSmoothingPower);
      });
      
      //harmonicBandConvolution = new GaussianConvolution(10);
      printf("prepared harmonic band convolution %d %d %f\n", minHarmonicSmoothing, maxHarmonicSmoothing, harmonicSmoothingSlope);
      harmonicBandConvolutionChanged = false;
    }
  }
  
  // I try with forward + reverse method, so that harmonics are acquired from any wavetable sample even if it is
  // not constructed from pure sin waves by partials. It probably needs quite a lot of adjustment to get working, but let's see...
  // ...Because not even pure sin wave produces one clear peak but already a spread out spectrum. Which would be difficult to 
  // control in clear ways. Anyhow the goal would be to apply certain kind of smoothing filter for the transformed spectrum.
  // And transform it back to wave.
  // Will optimize the buffer copies away when I have first tested that it works.

  static void prepareHarmonicBandWaveTable(WaveForm *waveForm) {
    //waveForm->stopAllNotesRequested = true;
    //long sampleRate = 96000;
    if(waveForm->waveTablePreparationStopRequested) return;
    
    waveForm->prepareHarmonicBandConvolution();
    
    if(waveForm->waveTablePreparationStopRequested) return;
    //if(harmonicBandWaveTable.size() != harmonicBandWaveTableSize) {
    //  harmonicBandWaveTable.resize(harmonicBandWaveTableSize);
    //}
    //if(!waveForm->harmonicBandFReverseFFTW.isInitialized() || waveForm->harmonicBandFReverseFFTW.getSize() != waveForm->harmonicBandWaveTableSize) {
      //waveForm->harmonicBandFReverseFFTW.initialize(waveForm->harmonicBandWaveTableSize, false, FFTW3Interface::Reverse);
      //waveForm->tempTableIn.resize(waveForm->harmonicBandWaveTableSize);
      //waveForm->tempTableOut.resize(waveForm->harmonicBandWaveTableSize);
    //}
    if(waveForm->tempTableIn.size() != waveForm->harmonicBandWaveTableSize) {
      waveForm->tempTableIn.resize(waveForm->harmonicBandWaveTableSize);
      waveForm->tempTableOut.resize(waveForm->harmonicBandWaveTableSize);
    }
    
    if(waveForm->harmonicBandPhases.size() != waveForm->harmonicBandWaveTableSize) {
      waveForm->harmonicBandPhases.resize(waveForm->harmonicBandWaveTableSize);
      for(int i=0; i<waveForm->harmonicBandWaveTableSize; i++) {
        waveForm->harmonicBandPhases[i] = Random::getDouble(0, 2*PI);
      }
    }
    
    if(waveForm->useHarmonicBandAntiAliasing) {
      if(waveForm->harmonicBandWaveTables[0].size() != waveForm->harmonicBandWaveTableSize) {
        for(int i=0; i<waveForm->harmonicBandWaveTables.size(); i++) {
          waveForm->harmonicBandWaveTables[i].resize(waveForm->harmonicBandWaveTableSize);
        }
      }
    }
    
    if(waveForm->waveTablePreparationStopRequested) return;
    
    long maxIndex = 0;
    
    memset(waveForm->tempTableIn.data(), 0, sizeof(waveForm->tempTableIn[0]) * waveForm->tempTableIn.size());
    memset(waveForm->tempTableOut.data(), 0, sizeof(waveForm->tempTableOut[0]) * waveForm->tempTableOut.size());
    
    
    for(int i=0; i<waveForm->partialSet.numPartials; i++) {
      long freq = waveForm->harmonicBandCyclesPerWaveTable * waveForm->partialSet.factors[i];
      if(freq >= 0 && freq < waveForm->waveTableSize*0.5) {
        waveForm->tempTableIn[freq] = waveForm->partialSet.gains[i];
        maxIndex = max(maxIndex, freq + waveForm->harmonicBandConvolution->getFilterMaxSize());
      }
    }
    if(waveForm->waveTablePreparationStopRequested) return;
    
    // TODO apply only in the areas of partials
    long antialiasingCount = waveForm->harmonicBandWaveTables.size();
    
    
    long frequencyLimit = waveForm->sampleRate / 2;
    long prevMaxInd = 0;
    
    for(int k=0; k<waveForm->antiAliasingCount; k++) {
      if(prevMaxInd > maxIndex) {
        waveForm->harmonicBandWaveTables[k] = waveForm->harmonicBandWaveTables[k-1];
        continue;
      }
      
      int p = waveForm->antiAliasingCount - k - 1;
      
      long maxInd = (frequencyLimit/(noteToFreq(waveForm->antiAliasingPitches[p+1]) * waveForm->oscillatorFrequencyFactor)) * waveForm->harmonicBandCyclesPerWaveTable;
      
      // TODO instead of convolving the whole array, add impulse responses to the points of partials
      waveForm->harmonicBandConvolution->apply(waveForm->tempTableIn, waveForm->tempTableOut, prevMaxInd, maxInd);
      printf("%d: maxPitch %d, prevMaxInd %lu, maxInd %lu, maxIndex %lu\n", k, waveForm->antiAliasingPitches[p+1], prevMaxInd, maxInd, maxIndex);
      
      prevMaxInd = maxInd;
      
      
      //waveForm->harmonicBandConvolution->apply(waveForm->tempTableIn, waveForm->tempTableOut, maxIndex);
      
      if(waveForm->waveTablePreparationStopRequested) return;
      
      for(int i=0; i<waveForm->waveTableSize; i++) {
        waveForm->harmonicBandFReverseFFTW.setReverseInput(i, waveForm->tempTableOut[i], waveForm->harmonicBandPhases[i]);
      }
      if(waveForm->waveTablePreparationStopRequested) return;
      
      waveForm->harmonicBandFReverseFFTW.transformReverse();
      
      if(waveForm->waveTablePreparationStopRequested) return;
      
      
      for(long i=0; i<waveForm->waveTableSize; i++) {
        waveForm->harmonicBandWaveTables[k][i] = waveForm->harmonicBandFReverseFFTW.reverseOutput[i];
      }
      waveForm->harmonicBandWaveTables[k][waveForm->waveTableSizeM1] = waveForm->harmonicBandWaveTables[k][0];
      
      Vec2d limits = waveForm->normalizeWaveTable(waveForm->harmonicBandWaveTables[k]);
      printf("waveform min %f, max %f\n", limits.x, limits.y);
      
      if(waveForm->numSteps > 1) {
        long q = waveForm->numSteps * waveForm->harmonicBandCyclesPerWaveTable;
        long r = waveForm->waveTableSize / q;
        for(long i=waveForm->waveTableSize-1; i>0; i--) {
          long j = long((double)i/waveForm->waveTableSize * q);
          waveForm->harmonicBandWaveTables[k][i] = waveForm->harmonicBandWaveTables[k][j*r];
        }
      }
      /*if(waveForm->waveTable.size() != waveForm->harmonicBandWaveTableSize) {
        waveForm->setWaveTableSize(waveForm->harmonicBandWaveTableSize);
      }
      
      if(waveForm->waveTablePreparationStopRequested) return;
      
      for(long i=0; i<waveForm->waveTableSize; i++) {
        waveForm->waveTable[i] = waveForm->harmonicBandFReverseFFTW.reverseOutput[i];
      }
      waveForm->waveTable[waveForm->waveTableSizeM1] = waveForm->waveTable[0];
      
      waveForm->normalizeWaveTable(waveForm->waveTable);*/
    }
    

    
    
    waveForm->updatePhase();
    
    printf("sin band wavetable prepared!\n");
    waveForm->preparationFinished();
  }

  
  void setWaveTableSize(int size) {
    stopAllNotesRequested = true;
    waveTableSize = size;
    waveTableSizeM1 = waveTableSize - 1;
    waveTable.resize(waveTableSize);
    if(waveTableSizeGui) {
      waveTableSizeGui->setValue(waveTableSize);
    }
    // FIXME only when type == SinBands
    if(harmonicBandWaveTables[0].size() != size) {
      for(int i=0; i<harmonicBandWaveTables.size(); i++) {
        harmonicBandWaveTables[i].resize(waveTableSize);
      }
    }
    //harmonicBandwidthDraggingGraph.setNumDataPoints(size);
  }
  
  

  void initSampleEditor(int sampleRate, int framesPerBuffer) {
    this->sampleRate = sampleRate;
    this->framesPerBuffer = framesPerBuffer;
  }
  
  void setSampleEditorVisible(bool isVisible)  {
    if(panel) {
      if(!isVisible) {
        panel->deleteChildElement(sampleEditor);
        sampleEditor = NULL;
      }
      else {
        if(sampleEditor) {
          panel->deleteChildElement(sampleEditor);
        }
        panel->addChildElement(sampleEditor = new SampleEditor(this, sampleRate, framesPerBuffer));
        sampleEditor->setPosition(-sampleEditor->size.x - 5, 0);
      }
    }
  }
  

  
  //AudioRecordingTrack sampleRecording;

  /*enum SampleWaveTableMode { OneCycle, SeveralCycles };
  SampleWaveTableMode sampleWaveTableMode = OneCycle;
  double sampleWaveTableSpeed = 1.0;*/
  
  /*void setGraphPitchhWaveTableIndex(int graphPitchhWaveTableIndex) {
    this->graphPitchhWaveTableIndex = graphPitchhWaveTableIndex;
    updateG
  }*/
  

  
  // it might be simple as that
  void seamWaveTableEnds(std::vector<double> &table) {
    int n = table.size()-1;
    if(n > 1) {
      double startValue = table[0];
      double endValue = table[n-1];
      double diff = startValue - endValue;
      if(diff == 0) return;
      
      for(int i=0; i<n; i++) {
        double t = (double)i/(n-1);
        table[i] = table[i] + t * diff;
      }
    }
  }
  
  virtual ~WaveForm() {
    waveTablePreparationStopRequested = true;
    for(int i=0; i<waveTablePreparingThreads->size(); i++) {
      waveTablePreparingThreads->at(i).join();
    }
    delete waveTablePreparingThreads;
  }
  
  WaveForm() {
    waveTable.resize(waveTableSize);

    /*pitchWaveTables.resize(128);
    for(int i=0; i<128; i++) {
      pitchWaveTables[i].resize(waveTableSize);
    }*/
    
    waveformGraphLeft.resize(2048);
    waveformGraphRight.resize(2048);
    
    waveTablePreparingThreads = new std::vector<std::thread>();
    //mainTableThreads = new std::vector<std::thread>();
    
    resetDefaults();

    fastNoise.SetSeed(56789);
    fastNoise.SetNoiseType(FastNoise::Value);
    fastNoise.SetFrequency(100.0);
    fastNoise.SetInterp(FastNoise::Linear);
    fastNoise.SetFractalType(FastNoise::FBM);

    fastNoiseCellularLookup.SetSeed(785745);
    fastNoiseCellularLookup.SetNoiseType(FastNoise::Simplex);
    fastNoiseCellularLookup.SetFrequency(1.0);
    fastNoise.SetCellularNoiseLookup(&fastNoiseCellularLookup);

    waveFormArgs.resize(Type::numTypes);
    numWaveFormArgs.resize(Type::numTypes, 0);
    
    for(int i=0; i<waveFormArgs.size(); i++) {
      waveFormArgs[i].resize(3, 0);
    }
    
    waveFormArgs[Type::Sin] = {1.0, 0, 0};
    numWaveFormArgs[Type::Sin] = 1;
    waveFormArgs[Type::Triangle] = {2.0, 0, 0};
    numWaveFormArgs[Type::Triangle] = 1;
    waveFormArgs[Type::Square] = {0.5, 0, 0};
    numWaveFormArgs[Type::Square] = 1;
    //waveFormArgs[Type::Saw] = {0, 0, 0};
    //numWaveFormArgs[Type::Saw] = 0;
    waveFormArgs[Type::Wave01] = {2.0, 0, 0};
    numWaveFormArgs[Type::Wave01] = 1;
    waveFormArgs[Type::Wave02] = {1.0, 0, 0};
    numWaveFormArgs[Type::Wave02] = 1;
    waveFormArgs[Type::Wave03] = {0, 0, 0};
    numWaveFormArgs[Type::Wave03] = 0;
    waveFormArgs[Type::Wave04] = {4.0, 0, 0};
    numWaveFormArgs[Type::Wave04] = 1;
    waveFormArgs[Type::Wave05] = {20.0, 0, 0};
    numWaveFormArgs[Type::Wave05] = 1;
    waveFormArgs[Type::Wave06] = {0, 0, 0};
    numWaveFormArgs[Type::Wave06] = 1;
    waveFormArgs[Type::Wave07] = {5.0, 5.0, 0};
    numWaveFormArgs[Type::Wave07] = 2;
    waveFormArgs[Type::Wave08] = {5.0, 0, 0};
    numWaveFormArgs[Type::Wave08] = 1;
    /*waveFormArgs[Type::Wave09] = {0, 0, 0};
    numWaveFormArgs[Type::Wave09] = 0;
    waveFormArgs[Type::Wave10] = {0, 0, 0};
    numWaveFormArgs[Type::Wave10] = 0;
    waveFormArgs[Type::Wave11] = {0, 0, 0};
    numWaveFormArgs[Type::Wave11] = 0;
    waveFormArgs[Type::Wave12] = {0, 0, 0};
    numWaveFormArgs[Type::Wave12] = 0;*/
    waveFormArgs[Type::Wave13] = {1.0, 3.0, 4.0};
    numWaveFormArgs[Type::Wave13] = 3;
    /*waveFormArgs[Type::Wave14] = {0, 0, 0};
    numWaveFormArgs[Type::Wave14] = 0;
    waveFormArgs[Type::WhiteNoise] = {0, 0, 0};
    numWaveFormArgs[Type::WhiteNoise] = 0;
    waveFormArgs[Type::GradientNoise] = {0, 0, 0};
    numWaveFormArgs[Type::GradientNoise] = 0;*/
    waveFormArgs[Type::Power] = {1, 0, 0};
    numWaveFormArgs[Type::Power] = 1;
    waveFormArgs[Type::Sinc] = {1, 0, 0};
    numWaveFormArgs[Type::Sinc] = 1;
    /*waveFormArgs[Type::Sin] = {1.0};
    waveFormArgs[Type::Triangle] = {2.0};
    waveFormArgs[Type::Square] = {0.5};
    waveFormArgs[Type::Saw] = {};
    waveFormArgs[Type::Wave01] = {2.0};
    waveFormArgs[Type::Wave02] = {1.0};
    waveFormArgs[Type::Wave03] = {};
    waveFormArgs[Type::Wave04] = {4.0};
    waveFormArgs[Type::Wave05] = {20.0};
    waveFormArgs[Type::Wave06] = {};
    waveFormArgs[Type::Wave07] = {5.0, 5.0};
    waveFormArgs[Type::Wave08] = {5.0};
    waveFormArgs[Type::Wave09] = {};
    waveFormArgs[Type::Wave10] = {};
    waveFormArgs[Type::Wave11] = {};
    waveFormArgs[Type::Wave12] = {};
    waveFormArgs[Type::Wave13] = {1.0, 3.0, 4.0};
    waveFormArgs[Type::Wave14] = {};
    waveFormArgs[Type::WhiteNoise] = {};
    waveFormArgs[Type::GradientNoise] = {};
    waveFormArgs[Type::Power] = {1};
    waveFormArgs[Type::Sinc] = {1};*/

    // FIXME
    factorDraggingGraph.waveForm = this;
    gainDraggingGraph.waveForm = this;
    partialAttenuationDraggingGraph.waveForm = this;
    pitchSmoothingnDraggingGraph.waveForm = this;
    pitchGainDraggingGraph.waveForm = this;
    
    prepareWaveTable();
  }

  WaveForm &operator=(const WaveForm &w) {
    if(!waveTablePreparingThreads) {
      waveTablePreparingThreads = new std::vector<std::thread>();
    }
    waveTablePreparationStopRequested = true;
    for(int i=0; i<waveTablePreparingThreads->size(); i++) {
      waveTablePreparingThreads->at(i).join();
    }
    waveTablePreparingThreads->clear();
    
    type = w.type;
    waveTableFrequency = w.waveTableFrequency;
    waveTableSize = w.waveTableSize;
    waveTableSizeM1 = w.waveTableSizeM1;
    waveTableMode = w.waveTableMode;
    waveTable = w.waveTable;
    pitchWaveTables = w.pitchWaveTables;
    waveTableNew = w.waveTableNew;
    pitchWaveTablesNew = w.pitchWaveTablesNew;
    updateWaveTablesOnBackground = w.updateWaveTablesOnBackground;
    preparationTimer = w.preparationTimer;
    waveFormArgs = w.waveFormArgs;
    numWaveFormArgs =w.numWaveFormArgs;
    waveTableGraphLeft = w.waveTableGraphLeft;
    waveTableGraphRight = w.waveTableGraphRight;
    phaseMode = w.phaseMode;
    phaseZeroPoints = w.phaseZeroPoints;
    phaseZeroPointsHB = w.phaseZeroPointsHB;
    phaseStartLimits = w.phaseStartLimits;
    sameLeftAndRightPhase = w.sameLeftAndRightPhase;
    partialSet = w.partialSet;
    fastNoise = w.fastNoise;
    fastNoiseCellularLookup = w.fastNoiseCellularLookup;
    numSteps = w.numSteps;
    useGaussianSmoothing = w.useGaussianSmoothing;
    smoothingWindowLengthInCycles = w.smoothingWindowLengthInCycles;
    pitchSmoothings = w.pitchSmoothings;
    usePitchDependendGaussianSmoothing = w.usePitchDependendGaussianSmoothing;
    usePitchDependendPartialAttenuation = w.usePitchDependendPartialAttenuation;
    partialAttenuationValues = w.partialAttenuationValues;
    pitchDependendGains = w.pitchDependendGains;
    usePitchDependendGain = w.usePitchDependendGain;
    stopAllNotesRequested = w.stopAllNotesRequested;
    waveValueOffset = w.waveValueOffset;
    waveTablePreparationStopRequested = w.waveTablePreparationStopRequested;
    waveformGraphLeft = w.waveformGraphLeft;
    waveformGraphRight = w.waveformGraphRight;
    graphPitchhWaveTableIndex = w.graphPitchhWaveTableIndex;
    showPitchWaveTable = w.showPitchWaveTable;
    readyToPrepareWaveTable = w.readyToPrepareWaveTable;
    
    cyclesPerWaveTable = w.cyclesPerWaveTable;
    
    waveTableSeamMode = w.waveTableSeamMode;
    
    minHarmonicSmoothing = w.minHarmonicSmoothing;
    maxHarmonicSmoothing = w.maxHarmonicSmoothing;
    harmonicSmoothingSlope = w.harmonicSmoothingSlope;
    harmonicSmoothingPower = w.harmonicSmoothingPower;
    harmonicBandCyclesPerWaveTable = w.harmonicBandCyclesPerWaveTable;
    harmonicBandPhases = w.harmonicBandPhases;
    harmonicBandWaveTableSize = w.harmonicBandWaveTableSize;
    harmonicBandConvolutionChanged = true;
    harmonicBandWaveTables = w.harmonicBandWaveTables;
    
    
    factorDraggingGraph = w.factorDraggingGraph;
    gainDraggingGraph = w.gainDraggingGraph;
    pitchSmoothingnDraggingGraph = w.pitchSmoothingnDraggingGraph;
    partialAttenuationDraggingGraph = w.partialAttenuationDraggingGraph;
    pitchGainDraggingGraph = w.pitchGainDraggingGraph;
    
    return *this;
  }
  
/*  void toZeroPhase() {
    double previousValue = waveTable[0];
    for(int i=0; i<waveTableSize; i++) {
      if(waveTable[i] == 0 || sign(previousValue) != sign(waveTable[i])) {
        phaseStartLimits.x = (double)i/waveTableSize*cyclesPerWaveTable;
        phaseStartLimits.y = (double)i/waveTableSize*cyclesPerWaveTable;
        break;
      }
      previousValue = waveTable[i];
    }
    if(phaseStartMinGui && phaseStartMaxGui) {
      phaseStartMinGui->setValue(phaseStartLimits.x);
      phaseStartMaxGui->setValue(phaseStartLimits.y);
    }
    updateWaveTableGraphs();
  }*/
  
  void updatePhase() {
    if(type == Type::SinBands) {
      updateHarmonicBandPhases();
    }
    else {
      phaseZeroPoints.clear();
      double previousValue = waveTable[waveTableSizeM1-1];
      
      for(int i=0; i<waveTableSizeM1; i++) {
        if(waveTable[i] == 0 || sign(previousValue) != sign(waveTable[i])) {
          double x = (double)i/waveTableSizeM1*cyclesPerWaveTable;
          if(phaseMode != PhaseMode::ZerosWithinRange || (x >= phaseStartLimits.x*cyclesPerWaveTable && x <= phaseStartLimits.y*cyclesPerWaveTable)) {
            phaseZeroPoints.push_back(x);
          }
        }
        previousValue = waveTable[i];
      }
      if(phaseZeroPoints.size() == 0) {
        phaseZeroPoints.push_back(0);
        printf("########## TESTING PHASES ##########\n");
      }
      if(phaseMode == PhaseMode::FirstZero) {
        phaseStartLimits.x = phaseStartLimits.y = phaseZeroPoints[0];
        if(phaseStartMinGui && phaseStartMaxGui) {
          phaseStartMinGui->setValue(phaseZeroPoints[0]);
          phaseStartMaxGui->setValue(phaseZeroPoints[0]);
        }
      }
    }
  }
  
  void updateHarmonicBandPhases() {
    for(int k=0; k<antiAliasingCount; k++) {
      phaseZeroPointsHB[k].clear();
      
      // FIXME This might cause unexpected behavior
      //int n = waveTableSizeM1;
      int n = harmonicBandWaveTables[k].size()-1;
      if(n < 1) return;      
       
      double previousValue = harmonicBandWaveTables[k][n-1];
      
      for(int i=0; i<n; i++) {
        if(harmonicBandWaveTables[k][i] == 0 || sign(previousValue) != sign(harmonicBandWaveTables[k][i])) {
          double x = (double)i/n*cyclesPerWaveTable;
          if(phaseMode != PhaseMode::ZerosWithinRange || (x >= phaseStartLimits.x*cyclesPerWaveTable && x <= phaseStartLimits.y*cyclesPerWaveTable)) {
            phaseZeroPointsHB[k].push_back(x);
          }
        }
        previousValue = harmonicBandWaveTables[k][i];
      }
      if(phaseZeroPointsHB[k].size() == 0) {
        phaseZeroPointsHB[k].push_back(0);
      }
      /*if(phaseMode == PhaseMode::FirstZero) {
        phaseStartLimitsHB[k].x = phaseStartLimitsHB[k].y = phaseZeroPointsHB[k][0];
        if(phaseStartMinGui && phaseStartMaxGui) {
          phaseStartMinGui->setValue(phaseZeroPoints[0]);
          phaseStartMaxGui->setValue(phaseZeroPoints[0]);
        }
      }*/
    }
    if(phaseMode == PhaseMode::FirstZero) {
      phaseStartLimits.x = phaseStartLimits.y = phaseZeroPointsHB[0][0];
      if(phaseStartMinGui && phaseStartMaxGui) {
        phaseStartMinGui->setValue(phaseZeroPoints[0]);
        phaseStartMaxGui->setValue(phaseZeroPoints[0]);
      }
    }
  }
  
  
  void initPhase(Vec2d &phase, unsigned char pitch) {
    if(type == Type::SinBands) {
      int k = 0; // FIXME
      if(pitch < antiAliasingPitches[1]) k = 5;
      else if(pitch < antiAliasingPitches[2]) k = 4;
      else if(pitch < antiAliasingPitches[3]) k = 3;
      else if(pitch < antiAliasingPitches[4]) k = 2;
      else if(pitch < antiAliasingPitches[5]) k = 1;
      else if(pitch < antiAliasingPitches[6]) k = 0;
      
      if(phaseMode == WaveForm::PhaseMode::AnyWithinRange) {
        double p = phaseStartLimits.getRandomDoubleFromTheRange();
        phase.set(p, p);
        if(!sameLeftAndRightPhase) {
          phase.y = phaseStartLimits.getRandomDoubleFromTheRange();
        }
      }
      else if(phaseMode == WaveForm::PhaseMode::ZerosWithinRange && phaseZeroPointsHB[k].size() > 0) {
        double p = phaseZeroPointsHB[k][Random::getInt(0, phaseZeroPointsHB[k].size()-1)];
        phase.set(p, p);
        if(!sameLeftAndRightPhase) {
          phase.y = phaseZeroPointsHB[k][Random::getInt(0, phaseZeroPointsHB[k].size()-1)];
        }
      }
      else if(phaseMode == WaveForm::PhaseMode::FirstZero && phaseZeroPointsHB[k].size() > 0) {
        double p = phaseZeroPointsHB[k][0];
        phase.set(p, p);
      }
      else /*if(phaseMode == WaveForm::PhaseMode::LeftAndRight)*/ {
        phase.set(phaseStartLimits.x, phaseStartLimits.y);
      }
    }
    else {
      if(phaseMode == WaveForm::PhaseMode::AnyWithinRange) {
        double p = phaseStartLimits.getRandomDoubleFromTheRange();
        phase.set(p, p);
        if(!sameLeftAndRightPhase) {
          phase.y = phaseStartLimits.getRandomDoubleFromTheRange();
        }
      }
      else if(phaseMode == WaveForm::PhaseMode::ZerosWithinRange && phaseZeroPoints.size() > 0) {
        double p = phaseZeroPoints[Random::getInt(0, phaseZeroPoints.size()-1)];
        phase.set(p, p);
        if(!sameLeftAndRightPhase) {
          phase.y = phaseZeroPoints[Random::getInt(0, phaseZeroPoints.size()-1)];
        }
      }
      else if(phaseMode == WaveForm::PhaseMode::FirstZero && phaseZeroPoints.size() > 0) {
        double p = phaseZeroPoints[0];
        phase.set(p, p);
      }
      else /*if(phaseMode == WaveForm::PhaseMode::LeftAndRight)*/ {
        phase.set(phaseStartLimits.x, phaseStartLimits.y);
      }
    }
  }
  
  

  // TODO integrate properly
  void setCenterOffset() {
    double total = 0;
    for(int i=0; i<waveTable.size(); i++) {
      total += waveTable[i] - waveValueOffset;
    }
    waveValueOffset = -total / waveTable.size();
    if(waveValueOffsetGui) {
      waveValueOffsetGui->setValue(waveValueOffset);
      //waveValueOffsetGui->prerenderingNeeded = true;
    }
    //printf("waveValueOffset %f\n", waveValueOffset);
    prepareWaveTable();
  }
  
  

  void update() {
    prepareWaveTable();
  }


  void updateWaveTableGraphs() {
    if(!waveformGraphPanel || !readyToPrepareWaveTable) return;
    int pitch =  0;
    bool showPitch = false;
    if(showPitchWaveTable && graphPitchhWaveTableIndex >= 0 && graphPitchhWaveTableIndex < 128 && (usePitchDependendGaussianSmoothing || usePitchDependendPartialAttenuation)) {
      pitch = graphPitchhWaveTableIndex;
      showPitch = true;
    }
    
    //if(waveTableGraphLeft.size() != waveTableSizeM1) {
    if(waveTableGraphLeft.size() != min(waveTableSize, 2048)) {
      waveTableGraphLeft.resize(min(waveTableSize, 2048));
    }
    //if(waveTableGraphRight.size() != waveTableSizeM1) {
    if(waveTableGraphRight.size() != min(waveTableSize, 2048)) {
      waveTableGraphRight.resize(min(waveTableSize, 2048));
    }
    for(int i=0; i<waveTableGraphLeft.size(); i++) {
      if(waveTablePreparationStopRequested) {
        return;
      }
      double x = (double)i / (waveTableGraphLeft.size()-1);
      waveTableGraphLeft[i] = getSample(x + phaseStartLimits.x, pitch) - waveValueOffset;
      waveTableGraphRight[i] = getSample(x + phaseStartLimits.y, pitch) - waveValueOffset;
    }

    /*if(waveTableMode == WaveTableMode::None) {
      for(int i=0; i<waveTableSize; i++) {
        if(waveTablePreparationStopRequested) {
          return;
        }
        double x = (double)i / waveTableSize;
        waveTableGraphLeft[i] = getSample(x + phaseStartLimits.x, pitch) - waveValueOffset;
        waveTableGraphRight[i] = getSample(x + phaseStartLimits.y, pitch) - waveValueOffset;
      }
    }
    else {
      int n = waveTableGraphLeft.size();
      for(int i=0; i<n; i++) {
        if(waveTablePreparationStopRequested) {
          return;
        }
        int k = map(i, 0, n-1, 0, waveTableSizeM1-1);
        if(pitch >= 0)  {
          waveTableGraphLeft[i] = pitchWaveTables[pitch][clamp((int(k + phaseStartLimits.x * waveTableSizeM1)) % waveTableSizeM1, 0, waveTableSizeM1)] - waveValueOffset;
          waveTableGraphRight[i] = pitchWaveTables[pitch][clamp((int(k + phaseStartLimits.y * waveTableSizeM1)) % waveTableSizeM1, 0, waveTableSizeM1)] - waveValueOffset;
        }
        else {
          waveTableGraphLeft[i] = waveTable[clamp((int(k + phaseStartLimits.x * waveTableSizeM1)) % waveTableSizeM1, 0, waveTableSizeM1)] - waveValueOffset;
          waveTableGraphRight[i] = waveTable[clamp((int(k + phaseStartLimits.y * waveTableSizeM1)) % waveTableSizeM1, 0, waveTableSizeM1)] - waveValueOffset;
        }
      }
    }*/
    if(waveformGraphPanel) {
      if((waveTableMode != WaveTableMode::SingleFaster && waveTableMode != WaveTableMode::ForEachPartialFaster) || waveTableSize > waveformGraphPanel->size.x) {
        waveformGraphPanel->interpolation = GraphPanel::Interpolation::Linear;
      }
      else {
        waveformGraphPanel->interpolation = GraphPanel::Interpolation::NearestNeigbour;
      }
      
      if(showPitch) waveformGraphPanel->title = "pitch = " + std::to_string(pitch);
      else waveformGraphPanel->title = "";
      
      waveformGraphPanel->title2 = getWaveTablePreparationProgressMessage();
      
      waveformGraphPanel->prerenderingNeeded = true;
      //waveFormPreparationProgressLabel->prerenderingNeeded = true;
    }
    
    
  }

  /*void applyGaussianSmoothing() {
    applyGaussianSmoothing(waveTable, smoothingWindowLengthInCycles);
  }*/
  
  void prepareSmoothingFilter(std::vector<double> &smoothingFilter, int n) {
    smoothingFilter.resize(n);
    double total = 0;
    
    for(int i=0; i<n && !waveTablePreparationStopRequested; i++) {
      double x = map(i, 0, n-1, -2.6, 2.6);
      smoothingFilter[i] = exp(-x*x / 2.0);
      total += smoothingFilter[i];
    }
    for(int i=0; i<n && !waveTablePreparationStopRequested; i++) {
      smoothingFilter[i] /= total;
    }
  }
  void applyGaussianSmoothing(std::vector<double> &waveTable, const std::vector<double> originalWaveTable, const std::vector<double> &smoothingFilter, int n, double smoothingWindowLengthInCycles) {
    for(int i=0; i<waveTableSizeM1; i++) {
      double t = 0;
      for(int k=0; k<n && !waveTablePreparationStopRequested; k++) {
        int p = k - n/2;
        t += smoothingFilter[k] * originalWaveTable[(i + p + waveTableSizeM1) % waveTableSizeM1];
      }
      waveTable[i] = t;
    }
    waveTable[waveTableSizeM1] = waveTable[0];
  }
  
  void applyGaussianSmoothing(std::vector<double> &waveTable, double smoothingWindowLengthInCycles) {
    double length = min(1.0, smoothingWindowLengthInCycles);
    int n = int(length * waveTableSizeM1);

    if(n <= 1) return;

    std::vector<double> smoothingFilter;
    prepareSmoothingFilter(smoothingFilter, n);
    
    std::vector<double> originalWaveTable = std::vector<double>(waveTable);
    for(int i=0; i<waveTableSizeM1 && !waveTablePreparationStopRequested; i++) {
      double t = 0;
      for(int k=0; k<n; k++) {
        int p = k - n/2;
        t += smoothingFilter[k] * originalWaveTable[(i + p + waveTableSizeM1) % waveTableSizeM1];
      }
      waveTable[i] = t;
    }
    waveTable[waveTableSizeM1] = waveTable[0];
  }


  void setPartialPreset(int index) {
    if(index >= 0 && index < partialPresets.size()) {
      setPartialPreset(partialPresets[index]);
    }
  }

  void setPartialPreset(const PartialPreset &partialPreset) {
    if(partialPreset.type == PartialPreset::Type::Func) {
      partialSet.pattern = PartialSet::Pattern::Equation1;

      for(int i=0; i<PartialSet::numPartialFactorPatternArgs; i++) {
        partialSet.factorParameters[i] = i < partialPreset.factorArgs.size() ? partialPreset.factorArgs[i] : 0;
        if(partialPatternPanel) partialFactorPatternArgsGui[i]->setValue(partialSet.factorParameters[i]);
      }
      for(int i=0; i<PartialSet::numPartialGainPatternArgs; i++) {
        partialSet.gainParameters[i] = i < partialPreset.gainArgs.size() ? partialPreset.gainArgs[i] : 0;
        if(partialPatternPanel) partialGainPatternArgsGui[i]->setValue(partialSet.gainParameters[i]);
      }
      applyPartialFactorPattern();
      applyPartialGainPattern();
    }
    else {
      partialSet.pattern = PartialSet::Pattern::None;

      for(int i=0; i<PartialSet::maxNumPartials; i++) {
        partialSet.factors[i] = i < partialPreset.factorArgs.size() ? partialPreset.factorArgs[i] : 0;
        if(partialFactorsGui[i]) partialFactorsGui[i]->setValue(partialSet.factors[i]);
      }
      for(int i=0; i<PartialSet::maxNumPartials; i++) {
        partialSet.gains[i] = i < partialPreset.gainArgs.size() ? partialPreset.gainArgs[i] : 0;
        if(partialGainsGui[i]) partialGainsGui[i]->setValue(partialSet.gains[i]);
      }
    }
    /*if(usePartialPatternGui) {
      usePartialPatternGui->setValue(partialSet.usePattern);
    }*/
    if(partialPatternGui) {
      partialPatternGui->setValue(PartialSet::Pattern::Equation1);
    }
    prepareWaveTable();
  }

  void setWaveTableMode(WaveTableMode waveTableMode) {
    this->waveTableMode = waveTableMode;
    if(waveTableMode != WaveTableMode::None) {
      prepareWaveTable();
    }
  }

  void resetDefaults() {
    for(int i=0; i<PartialSet::maxNumPartials; i++) {
      partialSet.factors[i] = i+1;
      partialSet.gains[i] = pow(0.5, i+1);
    }
  }
  
  int numMainTableTreadshExecuting = 0;
  int numPitchTableTreadshExecuting = 0;
  int numProgressTreadshExecuting = 0;
  bool finishingMainTable = false;
  bool mainTablePrepared = false;
  bool stoppingThreads = false;
  bool restartPreparationRequested = false;
  //bool showUpdateTime = true;
  //std::mutex *mutex1 = NULL;
  
  
  
  void prepareWaveTable() {
    if(!readyToPrepareWaveTable) return;

    if(stoppingThreads) return;
    
    isUpdating = true;
    
    preparationTimer.tic();
    
    stoppingThreads = true;
    
    if(waveTablePreparingThreads->size() > 0) {
      waveTablePreparationStopRequested = true;
      for(int i=0; i<waveTablePreparingThreads->size(); i++) {
        waveTablePreparingThreads->at(i).join();
      }
      waveTablePreparingThreads->clear();
    }
    

    
    if(type == WaveForm::Type::Sample) {
      if(waveTableSize != sampleBuffer.size() + 1) {
        setWaveTableSize(sampleBuffer.size() + 1);
      }
    }
    else if(type == WaveForm::Type::SinBands) {
      /*if(harmonicBandWaveTables[0].size() != waveTableSize) {
        for(int i=0; i<harmonicBandWaveTables.size(); i++) {
          harmonicBandWaveTables[i].resize(waveTableSize);
        }
      }
      if(waveTableSize != harmonicBandWaveTableSize) {*/
        setWaveTableSize(harmonicBandWaveTableSize);
      //}
    }
    else {
      if(waveTableSize != waveTableSizeDefault) {
        setWaveTableSize(waveTableSizeDefault);
      }
    }
    
    if(!harmonicBandFReverseFFTW.isInitialized() || harmonicBandFReverseFFTW.getSize() != harmonicBandWaveTableSize) {
      harmonicBandFReverseFFTW.initialize(harmonicBandWaveTableSize, false, FFTW3Interface::Reverse);
    }
    
    /*if(waveTable.size() != waveTableSize) {
      waveTable.resize(waveTableSize);
    }    
    waveTableSizeM1 = waveTableSize -1;*/
    
    bool pitchTables = usePitchDependendPartialAttenuation || usePitchDependendGaussianSmoothing;
    
    if(pitchWaveTables.size() != 128) {
      pitchWaveTables.resize(128);
    }
    if(pitchWaveTables[0].size() != waveTableSize && pitchTables && type != Type::SinBands) {
      for(int i=0; i<pitchWaveTables.size(); i++) {
        pitchWaveTables[i].resize(waveTableSize);
      }
    }
    
    preparationTime = 1e10;
    
    if(waveformGraphPanel) {
      waveformGraphPanel->title2 = "Updating...";
      waveformGraphPanel->prerenderingNeeded = true;
    }
    
    numMainTableTreadshExecuting = 0;
    numPitchTableTreadshExecuting = 0;
    numProgressTreadshExecuting = 0;
    waveTablePreparationStopRequested = false;
    mainTablePrepared = false;

    printf("Time to stop previous wavetable preparation: %f µs\n", preparationTimer.toc() * 10e6);

    if(type == SinBands) {
      waveTablePreparingThreads->push_back(std::thread(WaveForm::prepareHarmonicBandWaveTable, this));
    }
    else {
      
      if(pitchTables) {
        if(pitchWaveTables.size() != 128) {
          pitchWaveTables.resize(128);
          for(int i=0; i<128; i++) {
            pitchWaveTables[i].resize(waveTableSize);
          }
        }
        for(int i=0; i<numWaveTablePreparingThreads; i++) {
          numPitchTableTreadshExecuting++;
          waveTablePreparingThreads->push_back(std::thread(preparePitchWaveTables, this, i, numWaveTablePreparingThreads));
        }
        numMainTableTreadshExecuting = 1;
        waveTablePreparingThreads->push_back(std::thread(WaveForm::prepareWaveTableX, this, &waveTable, type, -1, 0, 1, true));
      }
      else {
        for(int i=0; i<numWaveTablePreparingThreads; i++) {
          numMainTableTreadshExecuting++;
          waveTablePreparingThreads->push_back(std::thread(WaveForm::prepareWaveTableX, this, &waveTable, type, -1, i, numWaveTablePreparingThreads, true));
        }
      }
    }
    
    printf("Time to start new wavetable preparation: %f µs\n", preparationTimer.toc() * 10e6);
    
    if(type == WaveForm::Type::SinBands) {
      cyclesPerWaveTable = harmonicBandCyclesPerWaveTable;
    }
    else if(type == WaveForm::Type::GradientNoise) {
      cyclesPerWaveTable = fastNoise.GetFrequency();
    }
    else {
      cyclesPerWaveTable = 1;
    }
    
    stoppingThreads = false;
  }
  
  static void updatePreparationProgress(WaveForm *waveForm) {
    while((waveForm->numMainTableTreadshExecuting > 0 || waveForm->numPitchTableTreadshExecuting > 0) && !waveForm->waveTablePreparationStopRequested) {
      std::this_thread::sleep_for(std::chrono::milliseconds(60));
      waveForm->waveFormPreparationProgressLabel->setText(waveForm->getWaveTablePreparationProgressMessage());
    }
    TicToc ticToc;
    while(ticToc.toc() < 3.0 && !waveForm->waveTablePreparationStopRequested) {
      std::this_thread::sleep_for(std::chrono::milliseconds(60));
    }
    
    waveForm->numProgressTreadshExecuting--;
    if(waveForm->numProgressTreadshExecuting == 0) {
      waveForm->waveFormPreparationProgressLabel->setText(" ");
    }
  }
  
  inline Vec2d normalizeWaveTable(std::vector<double> &waveTable) {
    double maxAmp = -1e10, minAmp = 1e10;
    for(int i=0; i<waveTableSizeM1 && !waveTablePreparationStopRequested; i++) {
      maxAmp = max(maxAmp, waveTable.at(i));
      minAmp = min(minAmp, waveTable.at(i));
    }
    if(maxAmp > minAmp) {
      for(int i=0; i<waveTableSizeM1; i++) {
        waveTable.at(i) = map(waveTable.at(i), minAmp, maxAmp, -1.0, 1.0) + waveValueOffset;
      }
    }
    return Vec2d(minAmp, maxAmp);
  }
  static void prepareWaveTableX(WaveForm *waveForm, std::vector<double> *waveTable, Type type, int pitch, int threadNumber, int numThreads, bool isMainTable = false) {
    if(waveForm->waveTablePreparationStopRequested) {
      if(isMainTable) waveForm->numMainTableTreadshExecuting--;
      return;
    }

    int startIndex = waveForm->waveTableSize*threadNumber/numThreads;
    int endIndex =  waveForm->waveTableSize*(threadNumber+1)/numThreads;
    
    
    if(waveForm->partialSet.numPartials > 1 && (waveForm->waveTableMode == WaveTableMode::Single || waveForm->waveTableMode == WaveTableMode::SingleFaster)) {
      if(pitch >= 0 && waveForm->usePitchDependendPartialAttenuation) {
        for(int i=startIndex; i<endIndex && !waveForm->waveTablePreparationStopRequested; i++) {
          double t = (double)i/waveForm->waveTableSizeM1;
          double a = 0;
          a = getWaveFormStaticWithPartials(t, 1, waveForm, pitch);
          /*if(isnan(a)) {
            a = 0;
            printf("(debugging) at WaveForm::prepareWaveTable(), %s\n", waveForm->typeNames[type].c_str());
          }*/
          waveTable->at(i) = a;
        }
      }
      else {
        for(int i=startIndex; i<endIndex && !waveForm->waveTablePreparationStopRequested; i++) {
          double t = (double)i/waveForm->waveTableSizeM1;
          double a = 0;
          a = getWaveFormStaticWithPartials(t, 1, waveForm);
          waveTable->at(i) = a;
        }
      }
    }
    else {
      double p = waveForm->partialSet.gains[0] != 0 ? sign(waveForm->partialSet.gains[0]) : 1;
      for(int i=startIndex; i<endIndex && !waveForm->waveTablePreparationStopRequested; i++) {
        double t = (double)i/waveForm->waveTableSizeM1;
        double a = p * getWaveFormStatic(t, 1, waveForm);
        waveTable->at(i) = a;
      }
    }
  
    if(waveForm->waveTablePreparationStopRequested) {
      if(isMainTable) waveForm->numMainTableTreadshExecuting--;
      return;
    }

    if(!isMainTable) {
      if(waveForm->smoothingWindowLengthInCycles > 0) {
        waveForm->applyGaussianSmoothing(*waveTable, waveForm->smoothingWindowLengthInCycles);
      }


      //waveTable[waveTableSize-1] = waveTable[0];

      //updateWaveTableGraphs();

      /*if(waveformGraphPanel) {
        waveformGraphPanel->prerenderingNeeded = true;
      }*/
    }
    if(isMainTable) {
      waveForm->numMainTableTreadshExecuting--;
      if(waveForm->numMainTableTreadshExecuting == 0 && !waveForm->waveTablePreparationStopRequested) {
        prepareMainWaveTable(waveForm);
      }
    }
    
    
  }
  
  double preparationTime = 0;
  
  // FIXME
  std::string getWaveTablePreparationProgressMessage() {
    double t = preparationTimer.toc();
    if(preparationTime <= t) {
      if(preparationTime >= 1.0) return format("Updated in %.2f s", preparationTime);
      if(preparationTime >= 1e-3) return format("Updated in %.2f ms", preparationTime*1e3);
      return format("Updated in %.6f ms", preparationTime*1e3);
    }
    else return format("Updating %.6f s", t);
  }
  
  
  static void prepareMainWaveTable(WaveForm *waveForm) {    
    if(waveForm->waveTablePreparationStopRequested) return;

    waveForm->finishingMainTable = true;
    if(waveForm->smoothingWindowLengthInCycles > 0) {
      waveForm->applyGaussianSmoothing(waveForm->waveTable, waveForm->smoothingWindowLengthInCycles);
    }
    
    if(waveForm->waveTablePreparationStopRequested) {
      waveForm->finishingMainTable = false;
      return;
    }
    if(waveForm->waveTableSeamMode == WaveForm::WaveTableSeamMode::LinearSeam) {
      waveForm->seamWaveTableEnds(waveForm->waveTable);
    }
    
    waveForm->normalizeWaveTable(waveForm->waveTable);
    waveForm->updatePhase();
    
    if(waveForm->waveTablePreparationStopRequested) {
      waveForm->finishingMainTable = false;
      return;
    }

    waveForm->partialSet.totalGain = 0;
    for(int i=0; i<waveForm->partialSet.numPartials; i++) {
      waveForm->partialSet.totalGain += waveForm->partialSet.gains[i];
    }
    
    if(waveForm->partialSet.totalGain == 0) waveForm->partialSet.totalGain = 0.00001;
    
    waveForm->mainTablePrepared = true;
    if(waveForm->numPitchTableTreadshExecuting == 0 && waveForm->mainTablePrepared && !waveForm->waveTablePreparationStopRequested) {
      waveForm->preparationFinished();
    }

    waveForm->finishingMainTable = false;
  }
  
  
  void preparationFinished() {
    preparationTime = preparationTimer.toc();
    
    if(usePitchDependendPartialAttenuation || usePitchDependendGaussianSmoothing) {
      printf("Wavetable preparation finished, (1+128) x %d samples in %f seconds (%f s per wavetable)\n", waveTableSize, preparationTimer.duration, preparationTimer.duration/129.0);
    }
    else {
      printf("Wavetable preparation finished, %d samples in %f seconds\n", waveTableSize, preparationTimer.duration);
    }
    
    isUpdating = false;
    
    updateWaveTableGraphs();
  }
  
  
  static void preparePitchWaveTables(WaveForm *waveForm, int threadNumber, int numThreads) {
    if(waveForm->waveTablePreparationStopRequested) return;
    
    int startIndex = 128*threadNumber/numThreads;
    int endIndex =  128*(threadNumber+1)/numThreads;
    
    if(waveForm->usePitchDependendPartialAttenuation) {
      for(int i=startIndex; i<endIndex; i++) {
        if(waveForm->waveTablePreparationStopRequested) {
          waveForm->numPitchTableTreadshExecuting--;
          return;
        }
        prepareWaveTableX(waveForm, &waveForm->pitchWaveTables[i], waveForm->type, i, 0, 1);
        
      }
      if(waveForm->usePitchDependendGaussianSmoothing) {
        for(int i=startIndex; i<endIndex; i++) {
          if(waveForm->waveTablePreparationStopRequested) {
            waveForm->numPitchTableTreadshExecuting--;
            return;
          }
          waveForm->applyGaussianSmoothing(waveForm->pitchWaveTables[i], waveForm->pitchSmoothings[i]);
        }
      }
    }
    else if(waveForm->usePitchDependendGaussianSmoothing) {
      prepareWaveTableX(waveForm, &waveForm->pitchWaveTables[startIndex], waveForm->type, startIndex, 0, 1);
      waveForm->normalizeWaveTable(waveForm->pitchWaveTables[startIndex]);
      for(int i=startIndex+1; i<endIndex; i++) {
        if(waveForm->waveTablePreparationStopRequested) {
          waveForm->numPitchTableTreadshExecuting--;
          return;
        }
        memcpy(waveForm->pitchWaveTables[i].data(), waveForm->pitchWaveTables[startIndex].data(), sizeof(waveForm->pitchWaveTables[0][0])*waveForm->pitchWaveTables[0].size());
        waveForm->applyGaussianSmoothing(waveForm->pitchWaveTables[i], waveForm->pitchSmoothings[i]);
      }
      waveForm->applyGaussianSmoothing(waveForm->pitchWaveTables[startIndex], waveForm->pitchSmoothings[startIndex]);
    }
    
    if(waveForm->waveTableSeamMode == WaveForm::WaveTableSeamMode::LinearSeam) {
      for(int i=startIndex; i<endIndex; i++) {
        waveForm->seamWaveTableEnds(waveForm->pitchWaveTables[i]);
      }
    }
    for(int i=startIndex; i<endIndex; i++) {
      waveForm->normalizeWaveTable(waveForm->pitchWaveTables[i]);
    }

    waveForm->numPitchTableTreadshExecuting--;
    
    if(waveForm->numPitchTableTreadshExecuting == 0 && waveForm->mainTablePrepared && !waveForm->waveTablePreparationStopRequested) {
      waveForm->preparationFinished();
    }
    
    for(int i=0; i<128; i++) {
      waveForm->partialSet.pitchTotalGains[i] = 0;
      for(int p=0; p<waveForm->partialSet.numPartials; p++) {
        waveForm->partialSet.pitchTotalGains[i] += waveForm->partialSet.gains[p] * waveForm->partialSet.pitchPartialAttenuations[i][p];
      }
      if(waveForm->partialSet.pitchTotalGains[i] == 0) waveForm->partialSet.pitchTotalGains[i] = 0.00001;
    }
  }
  
  
  
  
  

  // FIXME
  double getSample(double x, unsigned char pitch = 0) {
    //double gain = usePitchDependendGain ? pitchDependendGains[pitch] : 1.0;
    double gain = 1;
        
    x /= cyclesPerWaveTable;
    
    if(type == Type::SinBands) {
      //printf("debugging at Waveform::getSample, sinbands, x %f, numSteps * cyclesPerWaveTable == %f\n", x, numSteps * cyclesPerWaveTable);
      
      if(pitch < antiAliasingPitches[1]) {
        return gain * harmonicBandWaveTables[5][int(x * waveTableSizeM1) % waveTableSizeM1];
      }
      else if(pitch < antiAliasingPitches[2]) {
        return gain * harmonicBandWaveTables[4][int(x * waveTableSizeM1) % waveTableSizeM1];
      }
      else if(pitch < antiAliasingPitches[3]) {
        return gain * harmonicBandWaveTables[3][int(x * waveTableSizeM1) % waveTableSizeM1];
      }
      else if(pitch < antiAliasingPitches[4]) {
        return gain * harmonicBandWaveTables[2][int(x * waveTableSizeM1) % waveTableSizeM1];
      }
      else if(pitch < antiAliasingPitches[5]) {
        return gain * harmonicBandWaveTables[1][int(x * waveTableSizeM1) % waveTableSizeM1];
      }
      else if(pitch < antiAliasingPitches[6]) {
        return gain * harmonicBandWaveTables[0][int(x * waveTableSizeM1) % waveTableSizeM1];
      }
    }


    if(usePitchDependendGaussianSmoothing && usePitchDependendPartialAttenuation) {
      if(waveTableMode == WaveTableMode::Single) {
        double f = x * waveTableSizeM1;
        double q = f - floor(f);
        int i0 = int(f) % waveTableSizeM1;
        int i1 = i0 + 1;
        return gain * (1.0-q) * pitchWaveTables[pitch][i0] + q * pitchWaveTables[pitch][i1];
      }
      else if(waveTableMode == WaveTableMode::SingleFaster) {
        return gain * pitchWaveTables[pitch][int(x * waveTableSizeM1) % waveTableSizeM1];
      }
      else if(waveTableMode == WaveTableMode::ForEachPartial) {
        double a = 0;
        double s = x * waveTableSizeM1;
        for(int p=0; p<partialSet.numPartials; p++) {
          double f = partialSet.factors[p] * s;
          double q = f - floor(f);
          int i0 = int(f) % waveTableSizeM1;
          int i1 = i0 + 1;
          a += partialSet.pitchPartialAttenuations[pitch][p] * partialSet.gains[p] * ((1.0-q) * pitchWaveTables[pitch][i0] + q * pitchWaveTables[pitch][i1]);
        }
        return gain * a / partialSet.pitchTotalGains[pitch];
      }
      else if(waveTableMode == WaveTableMode::ForEachPartialFaster) {
        double a = 0;
        double s = x * waveTableSizeM1;
        for(int p=0; p<partialSet.numPartials; p++) {
          int i = int(partialSet.factors[p] * s) % waveTableSizeM1;
          a += partialSet.pitchPartialAttenuations[pitch][p] * partialSet.gains[p] * pitchWaveTables[pitch][i];
        }
        return gain * a / partialSet.pitchTotalGains[pitch];
      }
    }
    else if(usePitchDependendGaussianSmoothing) {
      if(waveTableMode == WaveTableMode::Single) {
        double f = x * waveTableSizeM1;
        double q = f - floor(f);
        int i0 = int(f) % waveTableSizeM1;
        int i1 = i0 + 1;
        return gain * (1.0-q) * pitchWaveTables[pitch][i0] + q * pitchWaveTables[pitch][i1];
      }
      else if(waveTableMode == WaveTableMode::SingleFaster) {
        return gain * pitchWaveTables[pitch][int(x * waveTableSizeM1) % waveTableSizeM1];
      }
      else if(waveTableMode == WaveTableMode::ForEachPartial) {
        double a = 0;
        double s = x * waveTableSizeM1;
        for(int p=0; p<partialSet.numPartials; p++) {
          double f = partialSet.factors[p] * s;
          double q = f - floor(f);
          int i0 = int(f) % waveTableSizeM1;
          int i1 = i0 + 1;
          a += partialSet.gains[p] * ((1.0-q) * pitchWaveTables[pitch][i0] + q * pitchWaveTables[pitch][i1]);
        }
        return gain * a / partialSet.totalGain;
      }
      else if(waveTableMode == WaveTableMode::ForEachPartialFaster) {
        double a = 0;
        double s = x * waveTableSizeM1;
        for(int p=0; p<partialSet.numPartials; p++) {
          int i = int(partialSet.factors[p] * s) % waveTableSizeM1;
          a += partialSet.gains[p] * pitchWaveTables[pitch][i];
        }
        return gain * a / partialSet.totalGain;
      }
    }
    else if(usePitchDependendPartialAttenuation) {
      if(waveTableMode == WaveTableMode::Single) {
        double f = x * waveTableSizeM1;
        double q = f - floor(f);
        int i0 = int(f) % waveTableSizeM1;
        int i1 = i0 + 1;
        return gain * (1.0-q) * waveTable[i0] + q * waveTable[i1];
      }
      else if(waveTableMode == WaveTableMode::SingleFaster) {
        return gain * waveTable[int(x * waveTableSizeM1) % waveTableSizeM1];
      }
      else if(waveTableMode == WaveTableMode::ForEachPartial) {
        double a = 0;
        double s = x * waveTableSizeM1;
        for(int p=0; p<partialSet.numPartials; p++) {
          double f = partialSet.factors[p] * s;
          double q = f - floor(f);
          int i0 = int(f) % waveTableSizeM1;
          int i1 = i0 + 1;
          a += partialSet.pitchPartialAttenuations[pitch][p] * partialSet.gains[p] * ((1.0-q) * waveTable[i0] + q * waveTable[i1]);
        }
        return gain * a / partialSet.pitchTotalGains[pitch];
      }
      else if(waveTableMode == WaveTableMode::ForEachPartialFaster) {
        double a = 0;
        double s = x * waveTableSizeM1;
        for(int p=0; p<partialSet.numPartials; p++) {
          int i = int(partialSet.factors[p] * s) % waveTableSizeM1;
          a += partialSet.pitchPartialAttenuations[pitch][p] * partialSet.gains[p] * waveTable[i];
        }
        return gain * a / partialSet.pitchTotalGains[pitch];
      }
    }
    else {
      if(waveTableMode == WaveTableMode::Single) {
        double f = x * waveTableSizeM1;
        double q = f - floor(f);
        int i0 = int(f) % waveTableSizeM1;
        int i1 = i0 + 1;
        return gain * (1.0-q) * waveTable[i0] + q * waveTable[i1];
      }
      else if(waveTableMode == WaveTableMode::SingleFaster) {
        return gain * waveTable[int(x * waveTableSizeM1) % waveTableSizeM1];
      }
      else if(waveTableMode == WaveTableMode::ForEachPartial) {
        double a = 0;
        double s = x * waveTableSizeM1;
        for(int p=0; p<partialSet.numPartials; p++) {
          double f = partialSet.factors[p] * s;
          double q = f - floor(f);
          int i0 = int(f) % waveTableSizeM1;
          int i1 = i0 + 1;
          a += partialSet.gains[p] * ((1.0-q) * waveTable[i0] + q * waveTable[i1]);
        }
        return gain * a / partialSet.totalGain;
      }
      else if(waveTableMode == WaveTableMode::ForEachPartialFaster) {
        double a = 0;
        double s = x * waveTableSizeM1;
        for(int p=0; p<partialSet.numPartials; p++) {
          int i = int(partialSet.factors[p] * s) % waveTableSizeM1;
          a += partialSet.gains[p] * waveTable[i];
        }
        return gain * a / partialSet.totalGain;
      }
    }
    
    if(partialSet.numPartials > 1) {
      return gain * getWaveFormStaticWithPartials(x, 1, this) + waveValueOffset;
    }
    else {
      return gain * sign(partialSet.gains[0]) * getWaveFormStatic(x, 1, this) + waveValueOffset;
    }

  }



  static inline double getWaveFormStaticWithPartials(double t, double frequency, WaveForm *waveForm) {
    double totalGain = 0;
    double a = 0;
    for(int i=0; i<waveForm->partialSet.numPartials; i++) {
      totalGain += fabs(waveForm->partialSet.gains[i]);
    }
    for(int i=0; i<waveForm->partialSet.numPartials; i++) {
      a += waveForm->partialSet.gains[i] * getWaveFormStatic(t, frequency * waveForm->partialSet.factors[i], waveForm);
    }
    if(totalGain != 0) {
      return a / totalGain;
    }
    return 0;
  }

  static inline double getWaveFormStaticWithPartials(double t, double frequency, WaveForm *waveForm, unsigned char pitch) {
    double totalGain = 0;
    double a = 0;
    for(int i=0; i<waveForm->partialSet.numPartials; i++) {
      totalGain += waveForm->partialSet.pitchPartialAttenuations[pitch][i] * fabs(waveForm->partialSet.gains[i]);
    }
    for(int i=0; i<waveForm->partialSet.numPartials; i++) {
      a += waveForm->partialSet.pitchPartialAttenuations[pitch][i] * waveForm->partialSet.gains[i] * getWaveFormStatic(t, frequency * waveForm->partialSet.factors[i], waveForm);
    }
    if(totalGain != 0) {
      return a / totalGain;
    }
    return 0;
  }

  static inline double getWaveFormStatic(double t, double frequency, WaveForm *waveForm) {

    if(waveForm->numSteps > 1) {
      if(waveForm->type == WaveForm::Type::GradientNoise) {
        t = floor(t * waveForm->numSteps * waveForm->cyclesPerWaveTable) / (waveForm->numSteps * waveForm->cyclesPerWaveTable);
      }
      else {
        t = floor(t * waveForm->numSteps) / waveForm->numSteps;
      }
    }
    
    const std::vector<double> &args = waveForm->waveFormArgs[waveForm->type];

    double x = frequency * t;

    switch(waveForm->type) {
      case WaveForm::Type::Sin:
        return pow(sin(x*TAU), args[0]);

      case WaveForm::Type::Triangle: {
        double d = max(args[0], 1.00000001);
        double half = step(1.0/d, fract(x));
        double slope1 = d*fract(x);
        double slope2 = 1.0/d == 1.0 ? 0 : 1.0/(1.0-1.0/d)*fract(x-1.0/d);
        return (slope2*half + (1.0-slope1)*(1.0-half))*2.0 - 1.0;
      }

      case WaveForm::Type::Square: {
        double d = clamp(args[0], 1e-6, 1-1e-6);
        return 2.0*step(d, fract(x)) - 1.0;
      }

      case WaveForm::Type::Saw:
        return 2.0*fract(x) - 1.0;

      case WaveForm::Type::Wave01:
        return 0.5*args[0] == 0 ? 0 : pow(args[0], sin(x*TAU)) / (0.5*args[0]) - 1.0;

      case WaveForm::Type::Wave02: {
        double k = exp(args[0]) * 2.0;
        return k != 0 ? exp(args[0]*sin(x*TAU)) / k - 1.0 : 0;
      }

      case WaveForm::Type::Wave03:
        // FIXME
        return sin(sin(10.0*x*TAU)*sin(x*TAU));

      case WaveForm::Type::Wave04:
        return sin(x*TAU) * sin(args[0]*x*TAU);

      case WaveForm::Type::Wave05:
        return sin(x*TAU) * (sin(args[0]*x*TAU)*0.5 + 0.5);

      case WaveForm::Type::Wave06:
        return sinh(6.0*sin(x*TAU)) / 210.0;

      case WaveForm::Type::Wave07:
        return tanh(args[0]*pow(sin(x*TAU), args[1]));

      case WaveForm::Type::Wave08:
        return atan(args[0]*sin(x*TAU));

      case WaveForm::Type::Wave09:
        return tan(1.4*sin(x*TAU)) / 6.0;

      case WaveForm::Type::Wave10:
        return fabs(sin(x*TAU)) * 2.0 - 1.0;

      case WaveForm::Type::Wave11:
        return (pow(fabs(sin(x*TAU)), sin(x*TAU)) - 1.07) * 2.5;

      case WaveForm::Type::Wave12:
        return (pow(fabs(sin(x*TAU)), fabs(sin(x*TAU))) - 0.85) * 7.0;

      case WaveForm::Type::Wave13:
        //return sin(args[0]*pow(fabs(sin(args[1]*x*TAU), args[2]))*sin(x*TAU);
        return sin(args[0]*pow(fabs(sin(args[1]*x*TAU)), args[2])) * sin(x*TAU);

      case WaveForm::Type::Wave14: {
        double fx = fract(x);
        double f2x = fract(fx * 2.0) / 2.0;
        double s = fx <= 0.5 ? 1.0 : -1.0;
        return s * sqrt(0.25*0.25 - (f2x-0.25)*(f2x-0.25));
      }
      
      case WaveForm::Type::WhiteNoise:
        return Random::getDouble(-1, 1);

      case WaveForm::Type::GradientNoise:
        return waveForm->fastNoise.GetNoise(x, 0);

      case WaveForm::Type::Power:
        return pow(fract(x), args[0])*2.0 - 1.0;

      case WaveForm::Type::Sinc: {
        double t = fract(x) - 0.5;
        return args[0]*t == 0 ? 1 : sin(args[0]*t)/(args[0]*t);
      }
        // FIXME at first only one cycle length is allowed from the source audio, need to prepare with other software
      case WaveForm::Type::Sample: {
        double t = fract(x);
        long k = (long)(t * (waveForm->sampleBuffer.size()-1));
        return k >= 0 && k < waveForm->sampleBuffer.size()-1 ? waveForm->sampleBuffer[k] : 0;
      }

    }

    return 0;
  }




  void renderWaveForm(WaveForm::Type type, double x, double y, double w, double h, double width, const Vec4d &color, const Vec4d &axisColor, GeomRenderer &geomRenderer) {
    int sx = int(w);
    geomRenderer.texture = NULL;
    geomRenderer.strokeWidth = 1.5;
    geomRenderer.strokeType = 1;
    geomRenderer.strokeColor = axisColor;
    geomRenderer.drawLine(x, y, x+w, y);
    geomRenderer.drawLine(x, y, x, y+h);
    geomRenderer.drawLine(x+w, y+h, x+w, y);
    geomRenderer.drawLine(x+w, y+h, x, y+h);
    geomRenderer.drawLine(x, y+0.5*h, x+w, y+0.5*h);

    geomRenderer.strokeWidth = width;
    geomRenderer.strokeColor = color;


    double phaseLeft = (phaseStartLimits.x + phaseStartLimits.y) * 0.5;
    //double phaseRight = phaseLeft + stereoPhaseOffset;


    double ap = getSample(phaseLeft) * h * 0.5;
    
    if((waveTableMode != WaveTableMode::SingleFaster && waveTableMode != WaveTableMode::ForEachPartialFaster) || waveTableSize > sx) {
      for(int i=1; i<sx; i++) {
        double t = (double)i/sx;
        if(i == sx-1) t = 0;
        double a = getSample(t+phaseLeft) * h * 0.5;
        geomRenderer.drawLine(x+i-1, y+h*0.5-ap, x+i, y+h*0.5-a);
        ap = a;
      }
    }
    else {
      for(int i=1; i<sx; i++) {
        double t = (double)i/sx;
        if(i == sx-1) t = 0;
        double a = getSample(t+phaseLeft) * h * 0.5;
        geomRenderer.drawLine(x+i-1, y+h*0.5-a, x+i, y+h*0.5-a);
//        ap = a;
      }
    }
    /*if(phaseLeft != phaseRight) {
      for(int i=1; i<sx; i++) {
        double t = (double)i/sx;
        if(i == sx-1) t = 0;
        double a = getSample(t+phaseRight) * h * 0.5;
        geomRenderer.drawLine(x+i-1, y+h*0.5-ap, x+i, y+h*0.5-a);
        ap = a;
      }
    }*/
  }





  struct HarmonicSmoothingPanel : public Panel {
    struct BandwidthCurvePreview : public GuiElement {
      WaveForm *waveForm = NULL;
      BandwidthCurvePreview(double width, double height, double x, double y, WaveForm *waveForm) {
        setSize(width, height);
        setPosition(x, y);
        this->waveForm = waveForm;
      }
      virtual void onPrerender(GeomRenderer &geomRenderer, TextGl &textRenderer, const Vec2d &displacement) {
        geomRenderer.strokeType = 1;
        geomRenderer.strokeWidth = 1;
        geomRenderer.strokeColor.set(1, 1, 1, 0.25);
        for(int i=0; i<size.x; i++) {
          double x = (double)i / (size.x-1);
          double y = clamp(pow(waveForm->harmonicSmoothingSlope * x, waveForm->harmonicSmoothingPower), 0, 1);
          double m = max(waveForm->minHarmonicSmoothing, waveForm->maxHarmonicSmoothing);
          y = map(y, 0, 1, waveForm->minHarmonicSmoothing / m, waveForm->maxHarmonicSmoothing / m);
          geomRenderer.drawLine(i, size.y, i, size.y-max(1, y*size.y), displacement);
        }
      }
    };
    NumberBox* cyclesPerWaveTableGui = NULL;
    NumberBox* minHarmonicSmoothingGui = NULL;
    NumberBox* maxHarmonicSmoothingGui = NULL;
    NumberBox* harmonicSmoothingSlopeGui = NULL;
    NumberBox* harmonicSmoothingPowerGui = NULL;
    BandwidthCurvePreview *bandwidthCurvePreview = NULL;
    WaveForm *waveForm = NULL;
    
    HarmonicSmoothingPanel(WaveForm *waveForm) : Panel("Harmonic smoothing panel") {
      init(waveForm);
    }
    void init(WaveForm *waveForm) {
      this->waveForm = waveForm;

      //waveFormArgsPanel = new 
      /*double line = 10, lineHeight = 23;

      line -= lineHeight;*/
      ConstantWidthColumnPlacer layoutPlacer(250-20, 0, 10, 10);

      addChildElement(cyclesPerWaveTableGui = new NumberBox("Wavetable cycles", waveForm->harmonicBandCyclesPerWaveTable, NumberBox::FLOATING_POINT, 1, 1e10, layoutPlacer, 7));

      addChildElement(minHarmonicSmoothingGui = new NumberBox("First bandwidth", waveForm->minHarmonicSmoothing, NumberBox::INTEGER, 1, 10000, layoutPlacer, 7));
      
      addChildElement(maxHarmonicSmoothingGui = new NumberBox("Last bandwidth", waveForm->maxHarmonicSmoothing, NumberBox::INTEGER, 1, 10000, layoutPlacer, 7));
      
      addChildElement(harmonicSmoothingSlopeGui = new NumberBox("Increase slope", waveForm->harmonicSmoothingSlope, NumberBox::FLOATING_POINT, 1, 1e10, layoutPlacer, 6));
      harmonicSmoothingSlopeGui->defaultIncrementModePowerShift = -1;
      
      addChildElement(harmonicSmoothingPowerGui = new NumberBox("Increase power", waveForm->harmonicSmoothingPower, NumberBox::FLOATING_POINT, 1e-6, 1e10, layoutPlacer, 6));
      //harmonicSmoothingPowerGui->defaultIncrementModePowerShift = -1;
      
      addChildElement(bandwidthCurvePreview = new BandwidthCurvePreview(230, 40, 10, layoutPlacer.getY()+3, waveForm));
      
      addGuiEventListener(new HarmonicSmoothingPanelListener(this));

      setSize(250, layoutPlacer.getY() + bandwidthCurvePreview->size.y + 10);


      //panel->addChildElement(waveFormArgsPanel);
      //guiRoot.addChildElement(modulatorPanel);
      //setOscillatorSubPanelPositions();
    }

    void update() {
      cyclesPerWaveTableGui->setValue(waveForm->harmonicBandCyclesPerWaveTable);
      minHarmonicSmoothingGui->setValue(waveForm->minHarmonicSmoothing);
      maxHarmonicSmoothingGui->setValue(waveForm->maxHarmonicSmoothing);
      harmonicSmoothingSlopeGui->setValue(waveForm->harmonicSmoothingSlope);
      harmonicSmoothingPowerGui->setValue(waveForm->harmonicSmoothingPower);
      bandwidthCurvePreview->prerenderingNeeded = true;
    }

    struct HarmonicSmoothingPanelListener : public GuiEventListener {
      HarmonicSmoothingPanel *harmonicSmoothingPanel;
      HarmonicSmoothingPanelListener(HarmonicSmoothingPanel *harmonicSmoothingPanel) {
        this->harmonicSmoothingPanel = harmonicSmoothingPanel;
      }
      void onValueChange(GuiElement *guiElement) {
        if(guiElement == harmonicSmoothingPanel->cyclesPerWaveTableGui) {
          guiElement->getValue((void*)&harmonicSmoothingPanel->waveForm->harmonicBandCyclesPerWaveTable);
          harmonicSmoothingPanel->waveForm->harmonicBandConvolutionChanged = true;
          printf("changed cycles %f\n", harmonicSmoothingPanel->waveForm->harmonicBandCyclesPerWaveTable);
          harmonicSmoothingPanel->waveForm->prepareWaveTable();
        }
        if(guiElement == harmonicSmoothingPanel->minHarmonicSmoothingGui) {
          guiElement->getValue((void*)&harmonicSmoothingPanel->waveForm->minHarmonicSmoothing);
          harmonicSmoothingPanel->bandwidthCurvePreview->prerenderingNeeded = true;
          harmonicSmoothingPanel->waveForm->harmonicBandConvolutionChanged = true;
          harmonicSmoothingPanel->waveForm->prepareWaveTable();
        }
        if(guiElement == harmonicSmoothingPanel->maxHarmonicSmoothingGui) {
          guiElement->getValue((void*)&harmonicSmoothingPanel->waveForm->maxHarmonicSmoothing);
          harmonicSmoothingPanel->bandwidthCurvePreview->prerenderingNeeded = true;
          harmonicSmoothingPanel->waveForm->harmonicBandConvolutionChanged = true;
          harmonicSmoothingPanel->waveForm->prepareWaveTable();
        }
        if(guiElement == harmonicSmoothingPanel->harmonicSmoothingSlopeGui) {
          guiElement->getValue((void*)&harmonicSmoothingPanel->waveForm->harmonicSmoothingSlope);
          harmonicSmoothingPanel->bandwidthCurvePreview->prerenderingNeeded = true;
          harmonicSmoothingPanel->waveForm->harmonicBandConvolutionChanged = true;
          harmonicSmoothingPanel->waveForm->prepareWaveTable();
        }
        if(guiElement == harmonicSmoothingPanel->harmonicSmoothingPowerGui) {
          guiElement->getValue((void*)&harmonicSmoothingPanel->waveForm->harmonicSmoothingPower);
          harmonicSmoothingPanel->bandwidthCurvePreview->prerenderingNeeded = true;
          harmonicSmoothingPanel->waveForm->harmonicBandConvolutionChanged = true;
          harmonicSmoothingPanel->waveForm->prepareWaveTable();
        }
      }
    };
  };    

  Panel *panel = NULL;
  ListBox *waveFormTypeGui = NULL;
  ListBox *waveTableModeGui = NULL;
  NumberBox *waveTableSizeGui = NULL;
  NumberBox *numPartialsGui = NULL;
  ListBox *partialPatternGui = NULL;
  NumberBox *numStepsGui = NULL;
  NumberBox *gaussianSmoothingGui = NULL;
  NumberBox *phaseStartMinGui = NULL, *phaseStartMaxGui = NULL;
  CheckBox *sameLeftAndRightPhaseGui = NULL;
  ListBox *phaseModeGui = NULL;

  ListBox *waveTableSeamModeGui = NULL;

  CheckBox *pitchDependendSmoothingGui = NULL;
  CheckBox *pitchDependendPartialAttenuationGui = NULL;
  CheckBox *pitchDependendGainGui = NULL;

  GraphPanel *waveformGraphPanel = NULL;

  HarmonicSmoothingPanel *harmonicBandPanel = NULL;
  
  Label *waveFormPreparationProgressLabel = NULL;
  
  NumberBox * waveValueOffsetGui = NULL;

  Panel *addPanel(GuiElement *parentElement, const std::string &title = "") {
    if(panel) return panel;
    //readyToPrepareWaveTable = false;
    panel = new Panel("Waveform panel", 250, 590, 10, 10);
    double line = 10, lineHeight = 23;

    ConstantWidthColumnPlacer layoutPlacer(250-20, 0, 10, 10);

    waveFormTypeGui = new ListBox("Waveform", layoutPlacer, 12);
    waveFormTypeGui->setItems(typeNames);
    waveFormTypeGui->setValue(type);
    
    waveTableModeGui = new ListBox("Wavetable", layoutPlacer, 12);
    waveTableModeGui->setItems(waveTableModeNames);
    waveTableModeGui->setValue(waveTableMode);
    
    panel->addChildElement(waveTableSizeGui = new NumberBox("Wavetable size", waveTableSize, NumberBox::INTEGER, 4, 1<<22, layoutPlacer, 10));
    //waveTableSizeGui->incrementMode = NumberBox::IncrementMode::Linear;
    waveTableSizeGui->incrementMode = NumberBox::IncrementMode::Power;
    waveTableSizeGui->keyboardInputEnabled = false;

    numStepsGui = new NumberBox("Step count", numSteps, NumberBox::INTEGER, 1, 1<<30, layoutPlacer, 12);
    numStepsGui->incrementMode = NumberBox::IncrementMode::Linear;

    numPartialsGui = new NumberBox("Partials", partialSet.numPartials, NumberBox::INTEGER, 1, PartialSet::maxNumPartials, layoutPlacer, 12);
    numPartialsGui->incrementMode = NumberBox::IncrementMode::Linear;
    
    partialPatternGui = new ListBox("Partial pattern", layoutPlacer, 12);
    partialPatternGui->setItems(partialSet.patternNames);
    partialPatternGui->setValue(partialSet.pattern);

    panel->addChildElement(pitchDependendPartialAttenuationGui = new CheckBox("Pitch partial attenuation", usePitchDependendPartialAttenuation, layoutPlacer));
    panel->addChildElement(pitchDependendSmoothingGui = new CheckBox("Pitch smoothing", usePitchDependendGaussianSmoothing, layoutPlacer));
    panel->addChildElement(pitchDependendGainGui = new CheckBox("Pitch gain", usePitchDependendGain, layoutPlacer));

    gaussianSmoothingGui = new NumberBox("Smoothing", smoothingWindowLengthInCycles, NumberBox::FLOATING_POINT, 0.0, 1.0, layoutPlacer, 8);

    phaseStartMinGui = new NumberBox("Phase x", phaseStartLimits.x, NumberBox::FLOATING_POINT, 0, 1, layoutPlacer, 8);
    phaseStartMaxGui = new NumberBox("Phase y", phaseStartLimits.y, NumberBox::FLOATING_POINT, 0, 1, layoutPlacer, 8);
    panel->addChildElement(phaseStartMinGui);
    panel->addChildElement(phaseStartMaxGui);

    panel->addChildElement(phaseModeGui = new ListBox("Phase start", layoutPlacer, 12));
    phaseModeGui->setItems(phaseModeNames);
    phaseModeGui->setValue(phaseMode);
    
    sameLeftAndRightPhaseGui = new CheckBox("Locked stereo phase", sameLeftAndRightPhase, layoutPlacer);
    panel->addChildElement(sameLeftAndRightPhaseGui);

    waveTableSeamModeGui = new ListBox("Seam wavetable", layoutPlacer, 8);
    waveTableSeamModeGui->setItems(waveTableSeamModeNames);
    waveTableSeamModeGui->setValue(waveTableSeamMode);
    panel->addChildElement(waveTableSeamModeGui);

    panel->addChildElement(waveValueOffsetGui = new NumberBox("Value offset", waveValueOffset, NumberBox::FLOATING_POINT, -1e10, 1e10, layoutPlacer, 10));

    panel->addChildElement(waveFormTypeGui);
    panel->addChildElement(numStepsGui);
    panel->addChildElement(waveTableModeGui);
    panel->addChildElement(numPartialsGui);
    //panel->addChildElement(usePartialPatternGui);
    //panel->addChildElement(usePartialDraggingGraphsGui);
    panel->addChildElement(partialPatternGui);
    
    panel->addChildElement(gaussianSmoothingGui);

    panel->addGuiEventListener(new WaveformPanelListener(this));

    panel->size.set(250, layoutPlacer.getY() + 10);

    parentElement->addChildElement(panel);

    initDraggableGraphs(panel);
    
    if(numWaveFormArgs[type] > 0) {
      addWaveFormArgsPanel();
    }
    if(type == WaveForm::Type::GradientNoise) {
      addFastNoiseWaveFormPanel();
    }
    if(type == WaveForm::Type::SinBands) {
      panel->addChildElement(harmonicBandPanel = new HarmonicSmoothingPanel(this));
      printf("sin band panel added...\n");
    }
    
    //if(partialSet.numPartials > 1) {
      addPartialsPanel();
    //}

    waveformGraphPanel = new GraphPanel(&waveTableGraphLeft, false, 0, 0, 0, 1, -1, 1, 0, 0, "", "");
    waveformGraphPanel->data2 = &waveTableGraphRight;
    waveformGraphPanel->setSize(250, 85);
    waveformGraphPanel->addChildElement(waveFormPreparationProgressLabel = new Label("", 6, 70));
    waveformGraphPanel->addGuiEventListener(new WaveformGraphPanelListener(this, waveformGraphPanel));
    panel->addChildElement(waveformGraphPanel);
    updateWaveTableGraphs();

    setSubPanelPositions();

    //readyToPrepareWaveTable = true;
    return panel;
  }

  void updatePanel() {
    if(panel) {
      waveFormTypeGui->setValue(type);
      waveTableModeGui->setValue(waveTableMode);
      waveTableSizeGui->setValue(waveTableSize);
      numStepsGui->setValue(numSteps);
      numPartialsGui->setValue(partialSet.numPartials);
      partialPatternGui->setValue(partialSet.pattern);
      pitchDependendPartialAttenuationGui->setValue(usePitchDependendPartialAttenuation);
      pitchDependendSmoothingGui->setValue(usePitchDependendGaussianSmoothing);
      pitchDependendGainGui->setValue(usePitchDependendGain);
      gaussianSmoothingGui->setValue(smoothingWindowLengthInCycles);
      phaseStartMinGui->setValue(phaseStartLimits.x);
      phaseStartMaxGui->setValue(phaseStartLimits.y);
      phaseModeGui->setValue(phaseMode);    
      sameLeftAndRightPhaseGui->setValue(sameLeftAndRightPhase);
      panel->addChildElement(sameLeftAndRightPhaseGui);
      waveTableSeamModeGui->setValue(waveTableSeamMode);
      
      //updateWaveTableGraphs();
      
      if(harmonicBandPanel) {
        harmonicBandPanel->update();
      }
      
      updateWaveFormArgsPanel();
      updatePartialsPanel();
      updatePartialPatternPanel();
      updateFastNoiseWaveFormPanel();
    }
  }

  Panel *getPanel() {
    return panel;
  }

  void removePanel(GuiElement *parentElement) {
    if(!panel) return;

    if(waveformGraphPanel) {
      panel->deleteChildElement(waveformGraphPanel);
      waveformGraphPanel = NULL;
    }
    if(partialsPanel) {
      removePartialsPanel();
    }
    if(waveFormArgsPanel) {
      removeWaveFormArgsPanel();
    }
    if(fastNoiseWaveFormPanel) {
      removeFastNoiseWaveFormPanel();
    }

    parentElement->deleteChildElement(panel);
    panel = NULL;
    waveFormTypeGui = NULL;
    waveTableModeGui = NULL;
    numPartialsGui = NULL;
    partialPatternGui = NULL;
    numStepsGui = NULL;
    gaussianSmoothingGui = NULL;
    phaseStartMinGui = NULL;
    phaseStartMaxGui = NULL;
    phaseModeGui = NULL;
    sameLeftAndRightPhaseGui = NULL;
    waveTableSeamModeGui = NULL;
    pitchDependendSmoothingGui = NULL;
    waveValueOffsetGui = NULL;
    waveTableSizeGui = NULL;
    harmonicBandPanel = NULL;
  }

  struct WaveformPanelListener : public GuiEventListener {
    WaveForm *waveForm;
    WaveformPanelListener(WaveForm *waveForm) {
      this->waveForm = waveForm;
    }
    void onValueChange(GuiElement *guiElement) {
      if(guiElement == waveForm->waveFormTypeGui) {
        //waveForm->stopAllNotesRequested = true;
        
        guiElement->getValue((void*)&waveForm->type);
        //if(waveForm->waveTableMode != WaveForm::WaveTableMode::None) {
          //printf("(debugging) at WaveformPanelListener.onValueChange: 3...\n");
          /*if(waveForm->type == WaveForm::Type::Sample) {
            if(waveForm->waveTableSize != waveForm->sampleBuffer.size() + 1) {
              waveForm->setWaveTableSize(waveForm->sampleBuffer.size() + 1);
            }
          }
          else if(waveForm->type == WaveForm::Type::SinBands) {
            if(waveForm->waveTableSize != waveForm->harmonicBandWaveTableSize) {
              waveForm->setWaveTableSize(waveForm->harmonicBandWaveTableSize);
            }
          }
          else {
            if(waveForm->waveTableSize != waveForm->waveTableSizeDefault) {
              waveForm->setWaveTableSize(waveForm->waveTableSizeDefault);
            }
          }*/
          
          waveForm->prepareWaveTable();
        //}
        waveForm->removeWaveFormArgsPanel();
        if(waveForm->numWaveFormArgs[waveForm->type]) {
          waveForm->addWaveFormArgsPanel();
        }
        waveForm->removeFastNoiseWaveFormPanel();
        if(waveForm->type == WaveForm::Type::GradientNoise) {
          waveForm->addFastNoiseWaveFormPanel();
        }
        
        if(waveForm->type == WaveForm::Type::SinBands && !waveForm->harmonicBandPanel) {
          waveForm->panel->addChildElement(waveForm->harmonicBandPanel = new HarmonicSmoothingPanel(waveForm));
        }
        if(waveForm->type != WaveForm::Type::SinBands && waveForm->harmonicBandPanel) {
          waveForm->panel->deleteChildElement(waveForm->harmonicBandPanel);
          waveForm->harmonicBandPanel = NULL;
        }




        //if(waveForm->partialSet.numPartials > 1) {
        waveForm->addPartialsPanel();
        //}

        waveForm->setSubPanelPositions();
        
        if(waveForm->type == WaveForm::Type::Sample) {
          waveForm->setSampleEditorVisible(true);
        }
        else if(waveForm->sampleEditor) {
          waveForm->setSampleEditorVisible(false);
        }
      }
      if(guiElement == waveForm->numStepsGui) {
        guiElement->getValue((void*)&waveForm->numSteps);
        //if(waveForm->waveTableMode != WaveForm::WaveTableMode::None) {
          waveForm->prepareWaveTable();
        //}
      }
      if(guiElement == waveForm->waveTableSizeGui) {
        long size = 1;
        guiElement->getValue((void*)&size);
        if(waveForm->type != WaveForm::Type::Sample && waveForm->type != WaveForm::Type::SinBands) {
          waveForm->waveTableSizeDefault = size;
          //waveForm->setWaveTableSize(size);
        }
        if(waveForm->type == WaveForm::Type::SinBands) {
          waveForm->harmonicBandWaveTableSize = size;
        }
        waveForm->prepareWaveTable();
      }
      if(guiElement == waveForm->gaussianSmoothingGui) {
        guiElement->getValue((void*)&waveForm->smoothingWindowLengthInCycles);
        //if(waveForm->waveTableMode != WaveForm::WaveTableMode::None) {
          waveForm->prepareWaveTable();
        //}
      }
      if(guiElement == waveForm->phaseStartMinGui) {
        guiElement->getValue((void*)&waveForm->phaseStartLimits.x);
        if(waveForm->phaseStartLimits.x > waveForm->phaseStartLimits.y) {
          waveForm->phaseStartLimits.y = waveForm->phaseStartLimits.x;
          waveForm->phaseStartMaxGui->setValue(waveForm->phaseStartLimits.y);
        }
        waveForm->updatePhase();
        waveForm->updateWaveTableGraphs();
      }
      if(guiElement == waveForm->phaseStartMaxGui) {
        guiElement->getValue((void*)&waveForm->phaseStartLimits.y);
        if(waveForm->phaseStartLimits.x > waveForm->phaseStartLimits.y) {
          waveForm->phaseStartLimits.x = waveForm->phaseStartLimits.y;
          waveForm->phaseStartMinGui->setValue(waveForm->phaseStartLimits.x);
        }
        waveForm->updatePhase();
        waveForm->updateWaveTableGraphs();
      }
      if(guiElement == waveForm->sameLeftAndRightPhaseGui) {
        guiElement->getValue((void*)&waveForm->sameLeftAndRightPhase);
        waveForm->updateWaveTableGraphs();
      }
      if(guiElement == waveForm->waveTableSeamModeGui) {
        guiElement->getValue((void*)&waveForm->waveTableSeamMode);
        waveForm->prepareWaveTable();
      }
      if(guiElement == waveForm->waveValueOffsetGui) {
        guiElement->getValue((void*)&waveForm->waveValueOffset);
        //if(waveForm->waveTableMode != WaveForm::WaveTableMode::None) {
          waveForm->prepareWaveTable();
        //}
      }

      if(guiElement == waveForm->partialPatternGui) {
        guiElement->getValue((void*)&waveForm->partialSet.pattern);
        if(waveForm->partialSet.pattern == PartialSet::Pattern::Equation1) {
          waveForm->applyPartialFactorPattern();
          waveForm->applyPartialGainPattern();
          waveForm->prepareWaveTable();
        }
        else if(waveForm->partialSet.pattern == PartialSet::Pattern::DraggingGraphs) {
          waveForm->updatePartialDraggingGraphs();
        }
      }
      if(guiElement == waveForm->pitchDependendSmoothingGui) {
        guiElement->getValue((void*)&waveForm->usePitchDependendGaussianSmoothing);
        waveForm->stopAllNotesRequested = true;
        waveForm->prepareWaveTable();
      }

      if(guiElement == waveForm->pitchDependendPartialAttenuationGui) {
        guiElement->getValue((void*)&waveForm->usePitchDependendPartialAttenuation);
        waveForm->stopAllNotesRequested = true;
        waveForm->prepareWaveTable();
      }

      if(guiElement == waveForm->pitchDependendGainGui) {
        guiElement->getValue((void*)&waveForm->usePitchDependendGain);
      }

      if(guiElement == waveForm->phaseModeGui) {
        int value;
        guiElement->getValue((void*)&value);
        waveForm->phaseMode = (WaveForm::PhaseMode)value;
        if(waveForm->phaseMode == PhaseMode::AnyWithinRange || waveForm->phaseMode == PhaseMode::ZerosWithinRange) {
          waveForm->phaseStartLimits.set(0, 1);
          waveForm->phaseStartMinGui->setValue(waveForm->phaseStartLimits.x);
          waveForm->phaseStartMaxGui->setValue(waveForm->phaseStartLimits.y);
        }
        if(waveForm->phaseMode == PhaseMode::LeftAndRight) {
          waveForm->phaseStartLimits.set(0, 0.5);
          waveForm->phaseStartMinGui->setValue(waveForm->phaseStartLimits.x);
          waveForm->phaseStartMaxGui->setValue(waveForm->phaseStartLimits.y);
        }
        waveForm->updatePhase();
        waveForm->updateWaveTableGraphs();
      }

      if(guiElement == waveForm->waveTableModeGui) {
        int value;
        guiElement->getValue((void*)&value);
        waveForm->setWaveTableMode((WaveForm::WaveTableMode)value);
      }
      if(guiElement == waveForm->numPartialsGui) {
        int value;
        guiElement->getValue((void*)&value);
        if(value != waveForm->partialSet.numPartials) {
          //waveForm->partialSet.numPartials = value;
          waveForm->setNumPartials(value);
          //if(waveForm->waveTableMode != WaveForm::WaveTableMode::None) {
            waveForm->applyPartialFactorPattern();
            waveForm->applyPartialGainPattern();
            waveForm->prepareWaveTable();
          //}
        }
        waveForm->addPartialsPanel(true);
        /*if(waveForm->partialSet.numPartials > 1) {
        }
        if(waveForm->partialsPanel && waveForm->partialSet.numPartials == 1) {
          waveForm->removePartialsPanel();
        }*/
      }
    }
    void onMousePressed(GuiElement *guiElement, Events &events) {
      if(guiElement == waveForm->pitchDependendSmoothingGui && guiElement->isPointWithin(events.m) && events.mouseButton == 1) {
        waveForm->pitchSmoothingnDraggingGraph.toggleVisibility();
      }
      if(guiElement == waveForm->pitchDependendGainGui && guiElement->isPointWithin(events.m) && events.mouseButton == 1) {
        waveForm->pitchGainDraggingGraph.toggleVisibility();
      }

      if(guiElement == waveForm->pitchDependendPartialAttenuationGui && guiElement->isPointWithin(events.m) && events.mouseButton == 1) {
        waveForm->partialAttenuationDraggingGraph.toggleVisibility();
      }
      if(guiElement == waveForm->waveValueOffsetGui && waveForm->waveValueOffsetGui->isPointWithin(events.m) && events.mouseButton == 1) {
        waveForm->setCenterOffset();
      }
    }
    
    void onKeyDown(GuiElement *guiElement, Events &events) {
      /*if(events.sdlKeyCode == SDLK_h) {
        printf("Trying to prepare harmonic band wavetable...\n");
        waveForm->prepareHarmonicBandWaveTable();
        printf("DONE\n");
      }*/

      if(guiElement == waveForm->numPartialsGui && events.sdlKeyCode == SDLK_1) {
        for(int i=0; i<waveForm->partialSet.gains.size(); i++) {
          waveForm->partialSet.gains[i] = 1.0 / pow(1.5, i);
        }
      }
      if(guiElement == waveForm->numPartialsGui && events.sdlKeyCode == SDLK_2) {
        for(int i=0; i<waveForm->partialSet.gains.size(); i++) {
          waveForm->partialSet.gains[i] = 1.0 / pow(2.0, i);
        }
      }
      if(guiElement == waveForm->numPartialsGui && events.sdlKeyCode == SDLK_3) {
        for(int i=0; i<waveForm->partialSet.gains.size(); i++) {
          waveForm->partialSet.gains[i] = 1.0 / pow(3.0, i);
        }
      }
      if(guiElement == waveForm->partialPatternGui && events.sdlKeyCode == SDLK_HOME) {
        if(waveForm->partialSet.pattern == PartialSet::Pattern::DraggingGraphs) {
          waveForm->setPartialDraggingGraphsVisible(!waveForm->factorDraggingGraph.draggableGraphPanel->isVisible);
        }
        if(waveForm->partialSet.pattern == PartialSet::Pattern::Equation1) {
          if(!waveForm->partialPatternPanel) {
            waveForm->addPartialPatternPanel();
          }
          else {
            waveForm->removePartialPatternPanel();
          }
        }

      }
    }
  };



  Panel *waveFormArgsPanel = NULL;
  std::vector<NumberBox*> argsGui = std::vector<NumberBox*>(WaveForm::maxNumArgs, NULL);

  void addWaveFormArgsPanel() {
    if(waveFormArgsPanel) return;

    waveFormArgsPanel = new Panel("Waveform args panel", 350, 590, 10, 10);
    //double line = 10, lineHeight = 23;

    //line -= lineHeight;
    ConstantWidthColumnPlacer layoutPlacer(250-20, 0, 10, 10);

    for(int i=0; i<numWaveFormArgs[type]; i++) {
      argsGui[i] = new NumberBox("Arg "+std::to_string(i+1), waveFormArgs[type][i], NumberBox::FLOATING_POINT, -1e10, 1e10, layoutPlacer, 8);
      waveFormArgsPanel->addChildElement(argsGui[i]);
    }
    waveFormArgsPanel->addGuiEventListener(new WaveFormArgsPanelListener(this));

    waveFormArgsPanel->size.set(250, layoutPlacer.getY() + 10);


    panel->addChildElement(waveFormArgsPanel);
    //guiRoot.addChildElement(modulatorPanel);
    //setOscillatorSubPanelPositions();
  }

  void updateWaveFormArgsPanel() {
    if(waveFormArgsPanel) {
      for(int i=0; i<numWaveFormArgs[type]; i++) {
        argsGui[i]->setValue(waveFormArgs[type][i]);
      }
    }
  }

  void removeWaveFormArgsPanel() {
    if(!waveFormArgsPanel) return;
    panel->deleteChildElement(waveFormArgsPanel);
    waveFormArgsPanel = NULL;
    for(int i=0; i<WaveForm::maxNumArgs; i++) {
      argsGui[i] = NULL;
    }
  }

  struct WaveFormArgsPanelListener : public GuiEventListener {
    WaveForm *waveForm;
    WaveFormArgsPanelListener(WaveForm *waveForm) {
      this->waveForm = waveForm;
    }
    void onValueChange(GuiElement *guiElement) {
      for(int i=0; i<waveForm->waveFormArgs[waveForm->type].size(); i++) {
        if(guiElement == waveForm->argsGui[i]) {
          guiElement->getValue((void*)&waveForm->waveFormArgs[waveForm->type][i]);
          //if(waveForm->waveTableMode != WaveForm::WaveTableMode::None) {
            waveForm->prepareWaveTable();
          //}
        }
      }
    }
  };




  Panel *partialPatternPanel = NULL;
  std::vector<NumberBox*> partialFactorPatternArgsGui = std::vector<NumberBox*>(PartialSet::numPartialFactorPatternArgs, NULL);
  std::vector<NumberBox*> partialGainPatternArgsGui = std::vector<NumberBox*>(PartialSet::numPartialGainPatternArgs, NULL);
  ListBox *presetsGui = NULL;


  void addPartialPatternPanel() {
    if(partialPatternPanel) return;

    partialPatternPanel = new Panel("Partial pattern panel", 350, 590, 10, 10);
    double line = 10, lineHeight = 23;

    presetsGui = new ListBox("Preset", 10, line, 9);
    for(int i=0; i<partialPresets.size(); i++) {
      presetsGui->addItems(partialPresets[i].name);
    }
    partialPatternPanel->addChildElement(presetsGui);

    for(int i=0; i<PartialSet::numPartialFactorPatternArgs; i++) {
      partialFactorPatternArgsGui[i] = new NumberBox("Factor "+std::to_string(i+1), partialSet.factorParameters[i], NumberBox::FLOATING_POINT, -1e10, 1e10, 10, line += lineHeight, 7);
      partialPatternPanel->addChildElement(partialFactorPatternArgsGui[i]);
    }
    for(int i=0; i<PartialSet::numPartialGainPatternArgs; i++) {
      partialGainPatternArgsGui[i] = new NumberBox("Gain "+std::to_string(i+1), partialSet.gainParameters[i], NumberBox::FLOATING_POINT, -1e10, 1e10, 10, line += lineHeight, 7);
      partialPatternPanel->addChildElement(partialGainPatternArgsGui[i]);
    }


    partialPatternPanel->addGuiEventListener(new PartialPatternPanelListener(this));

    partialPatternPanel->size.set(200, line + lineHeight + 10);


    panel->addChildElement(partialPatternPanel);

    partialPatternPanel->setPreviousPosition();

    //guiRoot.addChildElement(modulatorPanel);
    //setOscillatorSubPanelPositions();
  }

  void updatePartialPatternPanel() {
    if(partialPatternPanel) {
      for(int i=0; i<PartialSet::numPartialFactorPatternArgs; i++) {
        partialFactorPatternArgsGui[i]->setValue(partialSet.factorParameters[i]);
      }
      for(int i=0; i<PartialSet::numPartialGainPatternArgs; i++) {
        partialGainPatternArgsGui[i]->setValue(partialSet.gainParameters[i]);
      }
    }
  }

  void removePartialPatternPanel() {
    if(!partialPatternPanel) return;
    panel->deleteChildElement(partialPatternPanel);
    partialPatternPanel = NULL;
    presetsGui = NULL;
    for(int i=0; i<PartialSet::numPartialFactorPatternArgs; i++) {
      partialFactorPatternArgsGui[i] = NULL;
    }
    for(int i=0; i<PartialSet::numPartialGainPatternArgs; i++) {
      partialGainPatternArgsGui[i] = NULL;
    }
  }

  struct PartialPatternPanelListener : public GuiEventListener {
    WaveForm *waveForm;
    PartialPatternPanelListener(WaveForm *waveForm) {
      this->waveForm = waveForm;
    }
    void onAction(GuiElement *guiElement) {
      if(guiElement == waveForm->presetsGui) {
        int value;
        guiElement->getValue((void*)&value);
        waveForm->setPartialPreset(value);
        waveForm->presetsGui->setValue(value);
      }
    }

    void onValueChange(GuiElement *guiElement) {
      for(int i=0; i<PartialSet::numPartialFactorPatternArgs; i++) {
        if(guiElement == waveForm->partialFactorPatternArgsGui[i]) {
          guiElement->getValue((void*)&waveForm->partialSet.factorParameters[i]);
          /*if(!waveForm->partialSet.usePattern) {
            waveForm->partialSet.usePattern = true;
            waveForm->usePartialPatternGui->setValue(true);
          }*/
          waveForm->applyPartialFactorPattern();
          waveForm->prepareWaveTable();
        }
      }
      for(int i=0; i<PartialSet::numPartialGainPatternArgs; i++) {
        if(guiElement == waveForm->partialGainPatternArgsGui[i]) {
          /*if(!waveForm->partialSet.usePattern) {
            waveForm->partialSet.usePattern = true;
            waveForm->usePartialPatternGui->setValue(true);
          }*/
          guiElement->getValue((void*)&waveForm->partialSet.gainParameters[i]);
          waveForm->applyPartialGainPattern();
          waveForm->prepareWaveTable();
        }
      }
    }
  };

  void applyPartialFactorPattern() {
    if(partialSet.pattern == PartialSet::Pattern::Equation1) {
      partialSet.factors[0] = 1.0;
      for(int i=1; i<partialSet.numPartials; i++) {
        partialSet.factors[i] = partialSet.factorParameters[0] * pow(partialSet.factorParameters[1], i);
        partialSet.factors[i] += partialSet.factorParameters[2] * pow(i, partialSet.factorParameters[3]);
        partialSet.factors[i] += i * partialSet.factorParameters[4] + partialSet.factorParameters[5];
        if(partialFactorsGui[i]) {
          partialFactorsGui[i]->setValue(partialSet.factors[i]);
        }
      }
    }
  }

  void applyPartialGainPattern() {
    if(partialSet.pattern == PartialSet::Pattern::Equation1) {
      for(int i=0; i<partialSet.numPartials; i++) {
        double x = (double)i/partialSet.numPartials;
        double a = partialSet.gainParameters[0];
        double b = partialSet.gainParameters[1];
        double c = partialSet.gainParameters[2];
        double d = partialSet.gainParameters[3];
        double e = partialSet.gainParameters[4];
        double t = partialSet.gainParameters[5];
        double f = (1.0-t) * pow(1.0-x, a) + t;
        double g = c * pow(1.0-x, b) + t;
        double h = sin(d * 2.0*PI * x + e*2.0*PI + 0.5*PI) * 0.5 + 0.5;
        partialSet.gains[i] = h * (f - g) + g;
        if(partialGainsGui[i]) {
          partialGainsGui[i]->setValue(partialSet.gains[i]);
        }
      }
    }
  }


  Panel *partialsPanel = NULL;
  std::vector<NumberBox*> partialFactorsGui = std::vector<NumberBox*>(PartialSet::maxNumPartials, NULL);
  std::vector<NumberBox*> partialGainsGui = std::vector<NumberBox*>(PartialSet::maxNumPartials, NULL);
  //std::vector<NumberBox*> partialFactorSpreadsGui = std::vector<NumberBox*>(PartialSet::maxNumPartials, NULL);
  //std::vector<NumberBox*> partialGainSpreadsGui = std::vector<NumberBox*>(PartialSet::maxNumPartials, NULL);

  void addPartialsPanel(bool update = false) {
    if(partialsPanel && !update) return;

    double line = 10, lineHeight = 23;
    double columnA = 10, columnB = 130, columnC = 210, columnD = 310;

    if(!partialsPanel) {
      partialsPanel = new Panel("Partials panel", 350, 590, 10, 10);

      Label *factorLabel = new Label("Factor", columnA, line);
      Label *gainLabel = new Label("Gain", columnB, line);
      //Label *factorSpreadLabel = new Label("Factor", columnA, line);
      //Label *SpreadGainLabel = new Label("Gain", columnB, line);
      partialsPanel->addChildElement(factorLabel);
      partialsPanel->addChildElement(gainLabel);

      partialsPanel->addGuiEventListener(new PartialsPanelListener(this));
      panel->addChildElement(partialsPanel);
    }
    for(int i=0; i<PartialSet::maxNumPartials; i++) {
      if(i < partialSet.numPartials) {
        line += lineHeight;
        if(partialFactorsGui[i] == NULL) {
          partialFactorsGui[i] = new NumberBox(""+std::to_string(i+1), partialSet.factors[i], NumberBox::FLOATING_POINT, -1e10, 1e10, columnA, line, 7);
          partialFactorsGui[i]->gap = 5;
          //partialFactorSpreadsGui[i] = new NumberBox("", partialSet.factorSpreads[i], NumberBox::FLOATING_POINT, -1e10, 1e10, columnB, line, 7);
          partialGainsGui[i] = new NumberBox("", partialSet.gains[i], NumberBox::FLOATING_POINT, -1e10, 1e10, columnB, line, 7);
          //partialGainSpreadsGui[i] = new NumberBox("", partialSet.gainSpreads[i], NumberBox::FLOATING_POINT, -1e10, 1e10, columnD, line, 7);

          partialsPanel->addChildElement(partialFactorsGui[i]);
          partialsPanel->addChildElement(partialGainsGui[i]);
        }
        else {
          partialFactorsGui[i]->setValue(partialSet.factors[i]);
          partialGainsGui[i]->setValue(partialSet.gains[i]);
        }
      }
      if(i >= partialSet.numPartials && partialFactorsGui[i] != NULL) {
        partialsPanel->deleteChildElement(partialFactorsGui[i]);
        partialsPanel->deleteChildElement(partialGainsGui[i]);
        partialFactorsGui[i] = NULL;
        partialGainsGui[i] = NULL;
      }
    }

    partialsPanel->setSize(230, line + lineHeight + 10);

    partialsPanel->setPreviousPosition();

    /*if(partialSet.pattern == PartialSet::Pattern::Equation1) {
      addPartialPatternPanel();
    }*/
  }

  void updatePartialsPanel() {
    addPartialsPanel(true);
  }

  void removePartialsPanel() {
    if(!partialsPanel) return;
    panel->deleteChildElement(partialsPanel);
    partialsPanel = NULL;
    for(int i=0; i<PartialSet::maxNumPartials; i++) {
      partialFactorsGui[i] = NULL;
      partialGainsGui[i] = NULL;
      //partialFactorSpreadsGui[i] = NULL;
      //partialGainSpreadsGui[i] = NULL;
    }
    removePartialPatternPanel();
  }

  struct PartialsPanelListener : public GuiEventListener {
    WaveForm *waveForm;
    PartialsPanelListener(WaveForm *waveForm) {
      this->waveForm = waveForm;
    }
    void onValueChange(GuiElement *guiElement) {
      for(int i=0; i<PartialSet::maxNumPartials; i++) {
        if(guiElement == waveForm->partialFactorsGui[i]) {
          guiElement->getValue((void*)&waveForm->partialSet.factors[i]);
        }
        if(guiElement == waveForm->partialGainsGui[i]) {
          guiElement->getValue((void*)&waveForm->partialSet.gains[i]);
        }
        /*if(guiElement == waveForm->partialFactorSpreadsGui[i]) {
          guiElement->getValue((void*)&waveForm->partialSet.factorSpreads[i]);
        }
        if(guiElement == waveForm->partialGainSpreadsGui[i]) {
          guiElement->getValue((void*)&waveForm->partialSet.gainSpreads[i]);
        }*/
      }
      //if(waveForm->waveTableMode != WaveForm::WaveTableMode::None) {
        waveForm->prepareWaveTable();
      //}
    }
  };


  Panel *fastNoiseWaveFormPanel = NULL;
  ListBox *fastNoiseTypeGui = NULL, *fastNoiseInterpolationTypeGui = NULL, *fastNoiseFractalTypeGui = NULL;
  ListBox *fastNoiseCellularReturnTypeGui = NULL, *fastNoiseCellularDistanceFunctionGui = NULL;
  NumberBox *fastNoiseSeedGui = NULL, *fastNoiseFrequencyGui = NULL;
  NumberBox *fastNoiseOctavesGui = NULL, *fastNoiseLacunarityGui = NULL, *fastNoiseGainGui = NULL;

  void addFastNoiseWaveFormPanel() {
    if(fastNoiseWaveFormPanel) return;

    fastNoiseWaveFormPanel = new Panel("Fastnoise waveform panel", 350, 590, 10, 10);
    //double line = 10, lineHeight = 23;

    ConstantWidthColumnPlacer layoutPlacer(250-20, 0, 10, 10);

    fastNoiseTypeGui = new ListBox("Noise type", layoutPlacer, 10);
    fastNoiseTypeGui->setItems(fastNoiseTypeNames);
    fastNoiseTypeGui->setValue(fastNoise.GetNoiseType());

    fastNoiseInterpolationTypeGui = new ListBox("Interpolation", layoutPlacer, 10);
    fastNoiseInterpolationTypeGui->setItems(fastNoiseInterpNames);
    fastNoiseInterpolationTypeGui->setValue(fastNoise.GetInterp());

    fastNoiseFractalTypeGui = new ListBox("Fractal type", layoutPlacer, 10);
    fastNoiseFractalTypeGui->setItems(fastNoiseFractalTypeNames);
    fastNoiseFractalTypeGui->setValue(fastNoise.GetFractalType());

    fastNoiseCellularDistanceFunctionGui = new ListBox("Cell dist. func.", layoutPlacer, 10);
    fastNoiseCellularDistanceFunctionGui->setItems(fastNoiseCellularDistanceFunctionNames);
    fastNoiseCellularDistanceFunctionGui->setValue(fastNoise.GetCellularDistanceFunction());

    fastNoiseCellularReturnTypeGui = new ListBox("Cell ret.", layoutPlacer, 10);
    fastNoiseCellularReturnTypeGui->setItems(fastNoiseCellularReturnTypeNames);
    fastNoiseCellularReturnTypeGui->setValue(fastNoise.GetCellularReturnType());

    fastNoiseSeedGui = new NumberBox("Seed", fastNoise.GetSeed(), NumberBox::INTEGER, 0, 1<<30, layoutPlacer, 10);
    fastNoiseFrequencyGui = new NumberBox("Frequency", fastNoise.GetFrequency(), NumberBox::FLOATING_POINT, 0, 1e10, layoutPlacer, 10);
    fastNoiseOctavesGui = new NumberBox("Octaves", fastNoise.GetFractalOctaves(), NumberBox::INTEGER, 0, 100, layoutPlacer, 10);
    fastNoiseLacunarityGui = new NumberBox("Lacunarity", fastNoise.GetFractalLacunarity(), NumberBox::FLOATING_POINT, 0, 1e10, layoutPlacer, 10);
    fastNoiseGainGui = new NumberBox("Gain", fastNoise.GetFractalGain(), NumberBox::FLOATING_POINT, 0, 1e10, layoutPlacer, 10);

    fastNoiseWaveFormPanel->addChildElement(fastNoiseTypeGui);
    fastNoiseWaveFormPanel->addChildElement(fastNoiseInterpolationTypeGui);
    fastNoiseWaveFormPanel->addChildElement(fastNoiseFractalTypeGui);
    fastNoiseWaveFormPanel->addChildElement(fastNoiseCellularDistanceFunctionGui);
    fastNoiseWaveFormPanel->addChildElement(fastNoiseCellularReturnTypeGui);

    fastNoiseWaveFormPanel->addChildElement(fastNoiseSeedGui);
    fastNoiseWaveFormPanel->addChildElement(fastNoiseFrequencyGui);
    fastNoiseWaveFormPanel->addChildElement(fastNoiseOctavesGui);
    fastNoiseWaveFormPanel->addChildElement(fastNoiseLacunarityGui);
    fastNoiseWaveFormPanel->addChildElement(fastNoiseGainGui);

    fastNoiseWaveFormPanel->addGuiEventListener(new FastNoiseWaveFormPanelListener(this));
    fastNoiseWaveFormPanel->size.set(250, layoutPlacer.getY() + 10);
    panel->addChildElement(fastNoiseWaveFormPanel);
    //setOscillatorSubPanelPositions();
  }

  void updateFastNoiseWaveFormPanel() {
    fastNoiseTypeGui->setValue(fastNoise.GetNoiseType());
    fastNoiseInterpolationTypeGui->setValue(fastNoise.GetInterp());
    fastNoiseFractalTypeGui->setValue(fastNoise.GetFractalType());
    fastNoiseCellularDistanceFunctionGui->setValue(fastNoise.GetCellularDistanceFunction());
    fastNoiseCellularReturnTypeGui->setValue(fastNoise.GetCellularReturnType());
    fastNoiseSeedGui->setValue(fastNoise.GetSeed());
    fastNoiseFrequencyGui->setValue(fastNoise.GetFrequency());
    fastNoiseOctavesGui->setValue(fastNoise.GetFractalOctaves());
    fastNoiseLacunarityGui->setValue(fastNoise.GetFractalLacunarity());
    fastNoiseGainGui->setValue(fastNoise.GetFractalGain());
  }

  void removeFastNoiseWaveFormPanel() {
    if(!fastNoiseWaveFormPanel) return;
    panel->deleteChildElement(fastNoiseWaveFormPanel);
    fastNoiseWaveFormPanel = NULL;
    fastNoiseTypeGui = NULL;
    fastNoiseInterpolationTypeGui = NULL;
    fastNoiseFractalTypeGui = NULL;
    fastNoiseCellularReturnTypeGui = NULL;
    fastNoiseCellularDistanceFunctionGui = NULL;
    fastNoiseSeedGui = NULL;
    fastNoiseFrequencyGui = NULL;
    fastNoiseOctavesGui = NULL;
    fastNoiseLacunarityGui = NULL;
    fastNoiseGainGui = NULL;
  }

  struct FastNoiseWaveFormPanelListener : public GuiEventListener {
    WaveForm *waveForm;
    FastNoiseWaveFormPanelListener(WaveForm *waveForm) {
      this->waveForm = waveForm;
    }
    void onValueChange(GuiElement *guiElement) {
      FastNoise &fastNoise = waveForm->fastNoise;

      if(guiElement == waveForm->fastNoiseTypeGui) {
        int value;
        guiElement->getValue((void*)&value);
        fastNoise.SetNoiseType((FastNoise::NoiseType)value);
      }
      if(guiElement == waveForm->fastNoiseInterpolationTypeGui) {
        int value;
        guiElement->getValue((void*)&value);
        fastNoise.SetInterp((FastNoise::Interp)value);
      }
      if(guiElement == waveForm->fastNoiseFractalTypeGui) {
        int value;
        guiElement->getValue((void*)&value);
        fastNoise.SetFractalType((FastNoise::FractalType)value);
      }
      if(guiElement == waveForm->fastNoiseCellularDistanceFunctionGui) {
        int value;
        guiElement->getValue((void*)&value);
        fastNoise.SetCellularDistanceFunction((FastNoise::CellularDistanceFunction)value);
      }
      if(guiElement == waveForm->fastNoiseCellularReturnTypeGui) {
        int value;
        guiElement->getValue((void*)&value);
        fastNoise.SetCellularReturnType((FastNoise::CellularReturnType)value);
      }

      if(guiElement == waveForm->fastNoiseSeedGui) {
        int value;
        guiElement->getValue((void*)&value);
        fastNoise.SetSeed(value);
      }
      if(guiElement == waveForm->fastNoiseOctavesGui) {
        int value;
        guiElement->getValue((void*)&value);
        fastNoise.SetFractalOctaves(value);
      }
      if(guiElement == waveForm->fastNoiseFrequencyGui) {
        double value;
        guiElement->getValue((void*)&value);
        fastNoise.SetFrequency(value);
      }
      if(guiElement == waveForm->fastNoiseLacunarityGui) {
        double value;
        guiElement->getValue((void*)&value);
        fastNoise.SetFractalLacunarity(value);
      }
      if(guiElement == waveForm->fastNoiseGainGui) {
        double value;
        guiElement->getValue((void*)&value);
        fastNoise.SetFractalGain(value);
      }
      //if(waveForm->waveTableMode != WaveForm::WaveTableMode::None) {
        waveForm->prepareWaveTable();
      //}
    }

    void onKeyDown(GuiElement *guiElement, Events &events) {
      if(events.sdlKeyCode == SDLK_F8 && events.lControlDown) {
        memcpy(&waveForm->fastNoiseCellularLookup, &waveForm->fastNoise, sizeof(waveForm->fastNoise));
        waveForm->prepareWaveTable();
      }
      /*if(events.sdlKeyCode == SDLK_F9 && events.lControlDown) {
        memcpy(&waveForm->fastNoisePartialFactorSpread, &waveForm->fastNoise, sizeof(waveForm->fastNoise));
        waveForm->prepareWaveTable();
      }*/
    }
  };


  double setSubPanelPositions() {
    if(!panel) return 0;
    double y = panel->size.y + 5;
    if(waveformGraphPanel) {
      waveformGraphPanel->setPosition(0, y);
      y += waveformGraphPanel->size.y + 5;
    }
    if(waveFormArgsPanel) {
      waveFormArgsPanel->setPosition(0, y);
      y += waveFormArgsPanel->size.y + 5;
    }
    if(fastNoiseWaveFormPanel) {
      fastNoiseWaveFormPanel->setPosition(0, y);
      y += fastNoiseWaveFormPanel->size.y + 5;
    }
    if(harmonicBandPanel) {
      harmonicBandPanel->setPosition(0, y);
      y += harmonicBandPanel->size.y + 5;
    }
    if(partialsPanel) {
      partialsPanel->setPosition(0, y);
      y += partialsPanel->size.y + 5;
    }
    
    return y;
  }


  static std::string getClassName() {
    return "waveform";
  }
  
  virtual std::string getBlockName() {
    return getClassName();
  }

  virtual void decodeParameters() {
    printf("DEBUGGING WaveForm::decodeParameters()...\n");
    //printToTerminal();
    readyToPrepareWaveTable = false;
    
    std::string typeName, waveTableModeName, waveTableSeamModeName, phaseModeName;
    getStringParameter("type", typeName);
    for(int i=0; i<typeNames.size(); i++) {
      if(typeName == typeNames[i]) {
        type = (WaveForm::Type)i;
        break;
      }
    }
    
    getStringParameter("waveTableMode", waveTableModeName);
    for(int i=0; i<waveTableModeNames.size(); i++) {
      if(waveTableModeName == waveTableModeNames[i]) {
        waveTableMode = (WaveForm::WaveTableMode)i;
        break;
      }
    }
    
    getStringParameter("waveTableSeamMode", waveTableSeamModeName);
    for(int i=0; i<waveTableSeamModeNames.size(); i++) {
      if(waveTableSeamModeName == waveTableSeamModeNames[i]) {
        waveTableSeamMode = (WaveForm::WaveTableSeamMode)i;
        break;
      }
    }

    if(getStringParameter("phaseMode", phaseModeName) > 0) {
      for(int i=0; i<phaseModeNames.size(); i++) {
        if(phaseModeName == phaseModeNames[i]) {
          phaseMode = (WaveForm::PhaseMode)i;
          break;
        }
      }
    }
    else {
      phaseMode = PhaseMode::AnyWithinRange;
    }

    
    getNumericParameter("waveformParameters", waveFormArgs[type], false);
    getVectorParameter("phaseOffsets", phaseStartLimits);
    waveTableSize = 2048;
    getNumericParameter("waveTableSize", waveTableSize);
    waveTableSize = clamp(waveTableSize, minWaveTableSize, maxWaveTableSize);
    getNumericParameter("lockedStereoPhase", sameLeftAndRightPhase);
    getNumericParameter("numSteps", numSteps);
    getNumericParameter("gaussianSmoothing", smoothingWindowLengthInCycles);
    getNumericParameter("pitchDependendGaussianSmoothing", usePitchDependendGaussianSmoothing);
    getNumericParameter("pitchDependendPartialAttenuation", usePitchDependendPartialAttenuation);
    getNumericParameter("pitchDependendGain", usePitchDependendGain);
    getNumericParameter("waveValueOffset", waveValueOffset);
    
    getNumericParameter("minHarmonicSmoothing", minHarmonicSmoothing);
    getNumericParameter("maxHarmonicSmoothing", maxHarmonicSmoothing);
    getNumericParameter("harmonicSmoothingSlope", harmonicSmoothingSlope);
    getNumericParameter("harmonicSmoothingPower", harmonicSmoothingPower);
    
    // FIXME
    factorDraggingGraph.waveForm = this;
    gainDraggingGraph.waveForm = this;
    partialAttenuationDraggingGraph.waveForm = this;
    pitchSmoothingnDraggingGraph.waveForm = this;
    pitchGainDraggingGraph.waveForm = this;
    
    harmonicBandConvolutionChanged = true;
    
    // FIXME
    if(partialSet.pattern == PartialSet::Pattern::DraggingGraphs) {
      gainDraggingGraph.update();
      factorDraggingGraph.update();
    }
    if(usePitchDependendPartialAttenuation) {
      partialAttenuationDraggingGraph.update();
    }
    if(usePitchDependendGaussianSmoothing) {
      printf("on WaveForm::decodeParameters() 1...\n");
      printf("usePitchDependendGaussianSmoothing == %d\n", usePitchDependendGaussianSmoothing);
      pitchSmoothingnDraggingGraph.update();
      printf("on WaveForm::decodeParameters() 2...\n");
    }
    if(usePitchDependendGain) {
      pitchGainDraggingGraph.update();
    }
    /*if(type == Type::SinBands) {
      //setWaveTableSize(harmonicBandWaveTableSize);
      waveTableSize = harmonicBandWaveTableSize;
    }*/
    
    
    readyToPrepareWaveTable = true;
    //prepareWaveTable();
  }

  virtual void encodeParameters() {
    clearParameters();
    putStringParameter("type", typeNames[type]);
    putStringParameter("waveTableMode", waveTableModeNames[waveTableMode]);
    putNumericParameter("waveTableSize", waveTableSize);
    putStringParameter("waveTableSeamMode", waveTableSeamModeNames[waveTableSeamMode]);
    putNumericParameter("waveformParameters", waveFormArgs[type]);
    putVectorParameter("phaseOffsets", phaseStartLimits);
    putStringParameter("phaseMode", phaseModeNames[phaseMode]);
    putNumericParameter("lockedStereoPhase", sameLeftAndRightPhase);
    putNumericParameter("numSteps", numSteps);
    putNumericParameter("gaussianSmoothing", smoothingWindowLengthInCycles);
    putNumericParameter("pitchDependendGaussianSmoothing", usePitchDependendGaussianSmoothing);
    putNumericParameter("pitchDependendPartialAttenuation", usePitchDependendPartialAttenuation);
    putNumericParameter("pitchDependendGain", usePitchDependendGain);
    putNumericParameter("waveValueOffset", waveValueOffset);
        
    putNumericParameter("minHarmonicSmoothing", minHarmonicSmoothing);
    putNumericParameter("maxHarmonicSmoothing", maxHarmonicSmoothing);
    putNumericParameter("harmonicSmoothingSlope", harmonicSmoothingSlope);
    putNumericParameter("harmonicSmoothingPower", harmonicSmoothingPower);


    partialSet.encodeParameters();
    
    if(type == WaveForm::Type::GradientNoise) {
      fastNoise.encodeParameters();
    }
    if(partialSet.pattern == PartialSet::Pattern::DraggingGraphs) {
      factorDraggingGraph.encodeParameters();
      gainDraggingGraph.encodeParameters();
    }
    if(usePitchDependendGaussianSmoothing) {
      pitchSmoothingnDraggingGraph.encodeParameters();
    }
    if(usePitchDependendPartialAttenuation) {
      partialAttenuationDraggingGraph.encodeParameters();
    }
    if(usePitchDependendGain) {
      pitchGainDraggingGraph.encodeParameters();
    }
  }
  
  virtual void onPrintParameters(std::string &content, int level) {
    
    partialSet.printParameters(content, level);
    if(type == Type::GradientNoise) {
      fastNoise.printParameters(content, level);
    }
    if(partialSet.pattern == PartialSet::Pattern::DraggingGraphs) {
      factorDraggingGraph.printParameters(content, level);
      gainDraggingGraph.printParameters(content, level);
    }
    if(usePitchDependendGaussianSmoothing) {
      pitchSmoothingnDraggingGraph.printParameters(content, level);
    }
    if(usePitchDependendPartialAttenuation) {
      partialAttenuationDraggingGraph.printParameters(content, level);
    }
    if(usePitchDependendGain) {
      pitchGainDraggingGraph.printParameters(content, level);
    }
  }
  

  virtual int onNewBlock(const std::string &blockName, int blockStartInd, int blockLevel) {
    printf("(debugging) at WaveForm::onNewBlock(), %s\n", blockName.c_str());
    if(blockName == PartialSet::getClassName()) {
      return partialSet.parse(content, blockStartInd, blockLevel);
    }
    if(blockName == FastNoise::getClassName()) {
      return fastNoise.parse(content, blockStartInd, blockLevel);
    }
    if(blockName == FactorDraggingGraph::getClassName()) {
      return factorDraggingGraph.parse(content, blockStartInd, blockLevel);
    }
    if(blockName == GainDraggingGraph::getClassName()) {
      return gainDraggingGraph.parse(content, blockStartInd, blockLevel);
    }
    if(blockName == PitchSmoothingnDraggingGraph::getClassName()) {
      pitchSmoothingnDraggingGraph.waveForm = NULL;
      return pitchSmoothingnDraggingGraph.parse(content, blockStartInd, blockLevel);
    }
    if(blockName == PartialAttenuationDraggingGraph::getClassName()) {
      partialAttenuationDraggingGraph.waveForm = NULL;
      return partialAttenuationDraggingGraph.parse(content, blockStartInd, blockLevel);
    }
    if(blockName == PitchGainDraggingGraph::getClassName()) {
      pitchGainDraggingGraph.waveForm = NULL; // FIXME these NULLs prevent dragging graph decodeParameters->update->onUpdate, which causes unexplaineble crash
      return pitchGainDraggingGraph.parse(content, blockStartInd, blockLevel);
    }
    return 0;
  }




};

