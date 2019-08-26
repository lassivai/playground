#pragma once
#include "engine/sketch.h"

/* TODO
 * Synth Launchpad
 * - zoom into frequency range by pressing two keys
 *   - even better option: slide from left or right, multiplying each time either 0.5..1.0 or 1.0..2.0 times current range)
 *   - or both, depending on the row/column
 * - every possible live performance capability so we can start making music on the go :)
 *
 * - 2D flame stuff has changed somehow...
 * - ear trainer not working
 *
 * - wrap panel stuff into classes, and use visibility option to control visibility rather than creating/removing again
 *   (for the most part DONE)
 */

 /*
  * TODO develop somwthing more interesting from this
  *  - we need a kaleidoscope, soon!
  *    - I think that could be done quite easily either by geometric means, or alternatively rendering to buffer image and copying that
  *
  */







struct SynthSketch : public Sketch
{
  struct GuiControlPanel : public Panel {
    NumberBox *spectrumResolutionGui = NULL;
    NumberBox *spectrumFrequencyMinGui = NULL, *spectrumFrequencyMaxGui = NULL;

    CheckBox *showStereoOscilloscopeGui = NULL;
    NumberBox *stereoOscilloscopeLengthGui = NULL;

    ListBox *backgroundVisualizationGui = NULL;

    ListBox *spectrumModeGui = NULL;

    //NumberBox *spectrumAveragingGui = NULL;

    NumberBox *visibleSpectrumSizeGui = NULL;

    CheckBox *isKeyboardPianoActiveGui = NULL;
    
    CheckBox *screenKeysActiveGui = NULL;
    NumberBox *screenKeyboardMinNoteGui = NULL;
    NumberBox *screenKeyboardMaxNoteGui = NULL;
    ListBox *keyBaseNoteGui = NULL;

    ListBox *volumeUnitGui = NULL;

    CheckBox *spectrumAsNotesGui = NULL;

    NumberBox *showSpectrumNoteThresholdGui = NULL;
    CheckBox *roundSpectrumNoteLocationGui = NULL;
    ListBox *spectrumNoteInfoModeGui = NULL;

    NumberBox *spectrumGraphScalingGui = NULL;
    NumberBox *spectrumGraphFeedbackGui = NULL;
    ColorBox *spectrumGraphColorGui = NULL;

    ListBox *volumeLevelModeGui = NULL;

    //CheckBox *showWaveformGui = NULL;
    ListBox *waveformModeGui = NULL;
    NumberBox *waveformScalingGui = NULL;
    NumberBox *waveFormLowestFrequencyGui = NULL;
    NumberBox *waveformFeedbackGui = NULL;
    
    CheckBox *showEnvelopeGui = NULL;
    NumberBox *envelopeScalingGui = NULL;
    NumberBox *stereoOscilloscopeScalingGui = NULL;
    CheckBox *normalizeStereoOscilloscopeGui;

    ListBox *graphColorGui = NULL;

    Synth *synth = NULL;
    GuiElement *parentGuiElement = NULL;
    SynthSketch *synthSketch = NULL;
    
    CheckBox *rotatingRectsGui = NULL;
    
    NumberBox *numWaveTablePreparingThreadsGui = NULL;

    GuiControlPanel(SynthSketch *synthSketch, Synth *synth, GuiElement *parentGuiElement) : Panel("Gui control panel") {
      init(synthSketch, synth, parentGuiElement);
    }

    void init(SynthSketch *synthSketch, Synth *synth, GuiElement *parentGuiElement) {
      this->synthSketch = synthSketch;
      this->synth = synth;
      this->parentGuiElement = parentGuiElement;
      

      this->addGuiEventListener(new GuiControlPanelListener(this));
      parentGuiElement->addChildElement(this);

      double line = 10, lineHeight = 23;

      spectrumResolutionGui = new NumberBox("Spectrum transform size", synth->delayLineFFTW.getSize(), NumberBox::INTEGER, 128, 1<<24, 10, line, 8);
      spectrumResolutionGui->incrementMode = NumberBox::IncrementMode::Power;
      spectrumFrequencyMinGui = new NumberBox("Spectrum freq. limits", synthSketch->spectrumFrequencyLimits.x, NumberBox::FLOATING_POINT, 0, synth->sampleRate/2, 10, line+=lineHeight, 8);
      spectrumFrequencyMaxGui = new NumberBox("", synthSketch->spectrumFrequencyLimits.y, NumberBox::FLOATING_POINT, 1, synth->sampleRate/2, 10+180+95, line, 8);

      volumeUnitGui = new ListBox("Volume unit", 10, line+=lineHeight, 8);
      volumeUnitGui->setItems(synthSketch->volumeUnitNames);
      volumeUnitGui->setValue(synthSketch->volumeUnit);
      this->addChildElement(volumeUnitGui);

      spectrumGraphFeedbackGui = new NumberBox("Spectrum feedback", synthSketch->spectrumGraphFeedback, NumberBox::FLOATING_POINT, 0, 1, 10, line += lineHeight, 8);

      spectrumAsNotesGui = new CheckBox("Spectrum as notes", synthSketch->spectrumAsNotes, 10, line += lineHeight);

      spectrumNoteInfoModeGui = new ListBox("Spectrum notes", 10, line+=lineHeight, 16);
      spectrumNoteInfoModeGui->setItems(synthSketch->spectrumNoteInfoNames);
      spectrumNoteInfoModeGui->setValue(synthSketch->spectrumNoteInfoMode);
      this->addChildElement(spectrumNoteInfoModeGui);

      roundSpectrumNoteLocationGui = new CheckBox("Round spectrum note postion", synthSketch->roundSpectrumNoteLocation, 10, line += lineHeight);
      this->addChildElement(roundSpectrumNoteLocationGui);

      showSpectrumNoteThresholdGui = new NumberBox("Spectrum note threshold", synthSketch->showSpectrumNoteThreshold, NumberBox::FLOATING_POINT, -1e10, 1e10, 10, line += lineHeight, 8);

      spectrumModeGui = new ListBox("Spectrum graph style", 10, line+=lineHeight, 12);
      spectrumModeGui->setItems(synthSketch->spectrumModeNames);
      spectrumModeGui->setValue(synthSketch->spectrumMode);
      this->addChildElement(spectrumModeGui);

      visibleSpectrumSizeGui = new NumberBox("Spectrum graph points", synthSketch->visibleSpectrumSize, NumberBox::INTEGER, 1, synthSketch->screenW, 10, line += lineHeight, 8);
      spectrumGraphScalingGui = new NumberBox("Spectrum graph scaling", synthSketch->spectrumGraphScaling, NumberBox::FLOATING_POINT, -1e10, 1e10, 10, line += lineHeight, 8);

      backgroundVisualizationGui = new ListBox("Background", 10, line+=lineHeight, 12);
      backgroundVisualizationGui->setItems(synthSketch->backgroundVisualizationNames);
      backgroundVisualizationGui->setValue(synthSketch->backgroundVisualization);
      this->addChildElement(backgroundVisualizationGui);

      volumeLevelModeGui = new ListBox("Volume level bars", 10, line+=lineHeight, 8);
      volumeLevelModeGui->setItems(synthSketch->volumeLevelModeNames);
      volumeLevelModeGui->setValue(synthSketch->volumeLevelMode);

      //showWaveformGui = new CheckBox("Waveform", synthSketch->showWaveform, 10, line += lineHeight);
      waveformModeGui = new ListBox("Waveform", 10, line+=lineHeight, 8);
      waveformModeGui->setItems(synthSketch->waveformModeNames);
      waveformModeGui->setValue(synthSketch->waveformMode);
      
      waveformScalingGui = new NumberBox("Scale", synthSketch->waveformScaling, NumberBox::FLOATING_POINT, -1e10, 1e10, 10+200, line, 8);
      waveFormLowestFrequencyGui = new NumberBox("Waveform frequency limit", synthSketch->waveFormLowestFrequency, NumberBox::FLOATING_POINT, 1, 1e10, 10, line+=lineHeight, 8);
      waveformFeedbackGui = new NumberBox("Waveform feedback", synthSketch->waveFormFeedback, NumberBox::FLOATING_POINT, 0, 1, 10, line+=lineHeight, 8);

      showEnvelopeGui = new CheckBox("Envelope", synthSketch->showEnvelope, 10, line += lineHeight);
      envelopeScalingGui = new NumberBox("Scale", synthSketch->envelopeScaling, NumberBox::FLOATING_POINT, -1e10, 1e10, 10+110, line, 8);

      showStereoOscilloscopeGui = new CheckBox("Stereo oscilloscope", synthSketch->showStereoOscilloscope, 10, line += lineHeight);
      stereoOscilloscopeScalingGui = new NumberBox("Scale", synth->stereoOscilloscope.scaling, NumberBox::FLOATING_POINT, -1e10, 1e10, 10+180, line, 8);
      normalizeStereoOscilloscopeGui = new CheckBox("Normalized", synth->stereoOscilloscope.normalizeSize, 10, line += lineHeight);
      stereoOscilloscopeLengthGui = new NumberBox("Length", synth->stereoOscilloscope.trailLength, NumberBox::INTEGER, 0, synth->stereoOscilloscope.maxTrailLength, 10+180, line, 8);
      //stereoOscilloscopeLengthGui->incrementMode = NumberBox::IncrementMode::Power;

      //showMandelbrotVisualizerGui = new CheckBox("Mandelbrot", showMandelbrotVisualizer, 10, line += lineHeight);
      
      addChildElement(spectrumGraphColorGui = new ColorBox("Spectrum color", synthSketch->spectrumGraphColor, 10, line+=lineHeight));

      graphColorGui = new ListBox("Graph color", 10, line+=lineHeight, 8);
      graphColorGui->setItems(synthSketch->graphColorNames);
      graphColorGui->setValue(synthSketch->graphColorIndex);

      addChildElement(rotatingRectsGui = new CheckBox("Rect visualization", synthSketch->rotatingRectVisualization.isVisible, 10, line += lineHeight));
      
      isKeyboardPianoActiveGui = new CheckBox("Keyboard piano", synth->isKeyboardPianoActive, 10, line += lineHeight);

      screenKeysActiveGui = new CheckBox("Screen keys", synth->screenKeysActive, 10, line += lineHeight);

      screenKeyboardMinNoteGui = new NumberBox("", synth->screenKeyboardMinNote, NumberBox::INTEGER, 0, 126, 135, line, 4);
      screenKeyboardMinNoteGui->incrementMode = NumberBox::IncrementMode::Linear;
      screenKeyboardMaxNoteGui = new NumberBox("", synth->screenKeyboardMaxNote, NumberBox::INTEGER, 0, 126, 135 + 70, line, 4);
      screenKeyboardMaxNoteGui->incrementMode = NumberBox::IncrementMode::Linear;
      this->addChildElement(screenKeyboardMinNoteGui);
      this->addChildElement(screenKeyboardMaxNoteGui);

      keyBaseNoteGui = new ListBox("Key base", 135 + 70 + 70, line, 2);
      keyBaseNoteGui->addItems("C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B");
      keyBaseNoteGui->setValue(synth->keyBaseNote);
      this->addChildElement(keyBaseNoteGui);
      
      addChildElement(numWaveTablePreparingThreadsGui = new NumberBox("Wavetable update threads", numWaveTablePreparingThreads, NumberBox::INTEGER, 1, 100, 10, line+=lineHeight, 4));

      this->addChildElement(spectrumResolutionGui);
      this->addChildElement(spectrumFrequencyMinGui);
      this->addChildElement(spectrumFrequencyMaxGui);
      this->addChildElement(spectrumAsNotesGui);
      this->addChildElement(showStereoOscilloscopeGui);
      this->addChildElement(stereoOscilloscopeLengthGui);
      this->addChildElement(isKeyboardPianoActiveGui);
      this->addChildElement(screenKeysActiveGui);
      //this->addChildElement(spectrumAveragingGui);
      this->addChildElement(spectrumGraphFeedbackGui);
      this->addChildElement(visibleSpectrumSizeGui);
      this->addChildElement(showSpectrumNoteThresholdGui);
      this->addChildElement(spectrumGraphScalingGui);
      this->addChildElement(volumeLevelModeGui);
      //this->addChildElement(showWaveformGui);
      this->addChildElement(waveformModeGui);
      this->addChildElement(showEnvelopeGui);
      this->addChildElement(waveformScalingGui);
      this->addChildElement(waveFormLowestFrequencyGui);
      this->addChildElement(waveformFeedbackGui);
      this->addChildElement(envelopeScalingGui);
      this->addChildElement(stereoOscilloscopeScalingGui);
      this->addChildElement(normalizeStereoOscilloscopeGui);
      this->addChildElement(graphColorGui);

      this->setSize(410, line + lineHeight + 10);
    }


    struct GuiControlPanelListener : public GuiEventListener {
      GuiControlPanel *guiControlPanel;
      GuiControlPanelListener(GuiControlPanel *guiControlPanel) {
        this->guiControlPanel = guiControlPanel;
      }
      void onValueChange(GuiElement *guiElement) {
        if(guiElement == guiControlPanel->numWaveTablePreparingThreadsGui) {
          guiElement->getValue((void*)&numWaveTablePreparingThreads);
        }
        if(guiElement == guiControlPanel->normalizeStereoOscilloscopeGui) {
          guiElement->getValue((void*)&guiControlPanel->synthSketch->synth->stereoOscilloscope.normalizeSize);
        }
        if(guiElement == guiControlPanel->rotatingRectsGui) {
          guiElement->getValue((void*)&guiControlPanel->synthSketch->rotatingRectVisualization.isVisible);
        }
        if(guiElement == guiControlPanel->rotatingRectsGui) {
          guiElement->getValue((void*)&guiControlPanel->synthSketch->rotatingRectVisualization.isVisible);
          guiControlPanel->synthSketch->rotatingRectVisualization.rotatingRectVisualizationPanel->setVisible(guiControlPanel->synthSketch->rotatingRectVisualization.isVisible);
        }
        if(guiElement == guiControlPanel->graphColorGui) {
          guiElement->getValue((void*)&guiControlPanel->synthSketch->graphColorIndex);
        }
        if(guiElement == guiControlPanel->spectrumGraphColorGui) {
          guiElement->getValue((void*)&guiControlPanel->synthSketch->spectrumGraphColor);
        }
        if(guiElement == guiControlPanel->envelopeScalingGui) {
          guiElement->getValue((void*)&guiControlPanel->synthSketch->envelopeScaling);
        }
        if(guiElement == guiControlPanel->waveformScalingGui) {
          guiElement->getValue((void*)&guiControlPanel->synthSketch->waveformScaling);
        }
        if(guiElement == guiControlPanel->waveFormLowestFrequencyGui) {
          guiElement->getValue((void*)&guiControlPanel->synthSketch->waveFormLowestFrequency);
        }
        if(guiElement == guiControlPanel->waveformFeedbackGui) {
          guiElement->getValue((void*)&guiControlPanel->synthSketch->waveFormFeedback);
        }
        if(guiElement == guiControlPanel->stereoOscilloscopeScalingGui) {
          guiElement->getValue((void*)&guiControlPanel->synthSketch->synth->stereoOscilloscope.scaling);
        }
        /*if(guiElement == guiControlPanel->showWaveformGui) {
          guiElement->getValue((void*)&guiControlPanel->synthSketch->showWaveform);
        }*/
        if(guiElement == guiControlPanel->waveformModeGui) {
          guiElement->getValue((void*)&guiControlPanel->synthSketch->waveformMode);
        }
        if(guiElement == guiControlPanel->showEnvelopeGui) {
          guiElement->getValue((void*)&guiControlPanel->synthSketch->showEnvelope);
        }
        if(guiElement == guiControlPanel->volumeLevelModeGui) {
          guiElement->getValue((void*)&guiControlPanel->synthSketch->volumeLevelMode);
        }
        if(guiElement == guiControlPanel->spectrumGraphScalingGui) {
          guiElement->getValue((void*)&guiControlPanel->synthSketch->spectrumGraphScaling);
        }
        if(guiElement == guiControlPanel->spectrumGraphFeedbackGui) {
          guiElement->getValue((void*)&guiControlPanel->synthSketch->spectrumGraphFeedback);
        }
        if(guiElement == guiControlPanel->spectrumGraphScalingGui) {
          guiElement->getValue((void*)&guiControlPanel->synthSketch->spectrumGraphScaling);
        }
        if(guiElement == guiControlPanel->roundSpectrumNoteLocationGui) {
          guiElement->getValue((void*)&guiControlPanel->synthSketch->roundSpectrumNoteLocation);
        }
        if(guiElement == guiControlPanel->spectrumNoteInfoModeGui) {
          guiElement->getValue((void*)&guiControlPanel->synthSketch->spectrumNoteInfoMode);
        }
        if(guiElement == guiControlPanel->showSpectrumNoteThresholdGui) {
          guiElement->getValue((void*)&guiControlPanel->synthSketch->showSpectrumNoteThreshold);
        }
        if(guiElement == guiControlPanel->visibleSpectrumSizeGui) {
          guiElement->getValue((void*)&guiControlPanel->synthSketch->visibleSpectrumSize);
        }
        if(guiElement == guiControlPanel->isKeyboardPianoActiveGui) {
          guiElement->getValue((void*)&guiControlPanel->synth->isKeyboardPianoActive);
        }
        if(guiElement == guiControlPanel->screenKeysActiveGui) {
          guiElement->getValue((void*)&guiControlPanel->synth->screenKeysActive);
        }
        if(guiElement == guiControlPanel->showStereoOscilloscopeGui) {
          guiElement->getValue((void*)&guiControlPanel->synthSketch->showStereoOscilloscope);
        }
        if(guiElement == guiControlPanel->spectrumAsNotesGui) {
          guiElement->getValue((void*)&guiControlPanel->synthSketch->spectrumAsNotes);
        }
        if(guiElement == guiControlPanel->spectrumModeGui) {
          guiElement->getValue((void*)&guiControlPanel->synthSketch->spectrumMode);
        }
        if(guiElement == guiControlPanel->backgroundVisualizationGui) {
          guiElement->getValue((void*)&guiControlPanel->synthSketch->backgroundVisualization);
          guiControlPanel->synthSketch->spectrumMap.setActive(guiControlPanel->synthSketch->backgroundVisualization == 1);
          guiControlPanel->synthSketch->mandelbrotVisualizer.setActive(guiControlPanel->synthSketch->backgroundVisualization == 2);
          guiControlPanel->synthSketch->flameVisualizer.setActive(guiControlPanel->synthSketch->backgroundVisualization == 4);
        }

        if(guiElement == guiControlPanel->volumeUnitGui) {
          guiElement->getValue((void*)&guiControlPanel->synthSketch->volumeUnit);
        }
        if(guiElement == guiControlPanel->stereoOscilloscopeLengthGui) {
          int value;
          guiElement->getValue((void*)&value);
          if(value != guiControlPanel->synth->stereoOscilloscope.trailLength) {
            guiControlPanel->synth->stereoOscilloscope.trailLength = value;
            guiControlPanel->synth->stereoOscilloscope.trailPosition = 0;
            //guiControlPanel->synth->stereoOscilloscope.trail.resize(value);
          }
        }
        if(guiElement == guiControlPanel->spectrumFrequencyMinGui) {
          guiElement->getValue((void*)&guiControlPanel->synthSketch->spectrumFrequencyLimits.x);
          if(guiControlPanel->synthSketch->spectrumFrequencyLimits.x > guiControlPanel->synthSketch->spectrumFrequencyLimits.y) {
            guiControlPanel->synthSketch->spectrumFrequencyLimits.y = guiControlPanel->synthSketch->spectrumFrequencyLimits.x;
          }
        }
        if(guiElement == guiControlPanel->spectrumFrequencyMaxGui) {
          guiElement->getValue((void*)&guiControlPanel->synthSketch->spectrumFrequencyLimits.y);
          if(guiControlPanel->synthSketch->spectrumFrequencyLimits.x > guiControlPanel->synthSketch->spectrumFrequencyLimits.y) {
            guiControlPanel->synthSketch->spectrumFrequencyLimits.x = guiControlPanel->synthSketch->spectrumFrequencyLimits.y;
          }
        }
        if(guiElement == guiControlPanel->spectrumResolutionGui) {
          int value;
          guiElement->getValue((void*)&value);
          guiControlPanel->synth->delayLineFFTW.initialize(value, false);
          guiControlPanel->synth->delayLineFFTWRight.initialize(value, false);
        }
        if(guiElement == guiControlPanel->screenKeyboardMinNoteGui) {
          guiElement->getValue((void*)&guiControlPanel->synth->screenKeyboardMinNote);
          if(guiControlPanel->synth->screenKeyboardMinNote > guiControlPanel->synth->screenKeyboardMaxNote) {
            guiControlPanel->synth->screenKeyboardMaxNote = guiControlPanel->synth->screenKeyboardMinNote;
            guiControlPanel->screenKeyboardMaxNoteGui->setValue(guiControlPanel->synth->screenKeyboardMaxNote);
          }
          guiControlPanel->synth->updateNoteSequencerRects();
        }
        if(guiElement == guiControlPanel->screenKeyboardMaxNoteGui) {
          guiElement->getValue((void*)&guiControlPanel->synth->screenKeyboardMaxNote);
          if(guiControlPanel->synth->screenKeyboardMinNote > guiControlPanel->synth->screenKeyboardMaxNote) {
            guiControlPanel->synth->screenKeyboardMinNote = guiControlPanel->synth->screenKeyboardMaxNote;
            guiControlPanel->screenKeyboardMinNoteGui->setValue(guiControlPanel->synth->screenKeyboardMinNote);
          }
          guiControlPanel->synth->updateNoteSequencerRects();
        }
        if(guiElement == guiControlPanel->keyBaseNoteGui) {
          guiElement->getValue((void*)&guiControlPanel->synth->keyBaseNote);
        }
      }

      void onAction(GuiElement *guiElement) {
        if(guiElement == guiControlPanel->spectrumFrequencyMinGui) {
          guiControlPanel->synthSketch->spectrumFrequencyLimits.x = 0;
          guiControlPanel->spectrumFrequencyMinGui->setValue(guiControlPanel->synthSketch->spectrumFrequencyLimits.x);
        }
        if(guiElement == guiControlPanel->spectrumFrequencyMaxGui) {
          guiControlPanel->synthSketch->spectrumFrequencyLimits.y = guiControlPanel->synthSketch->screenW-1;
          guiControlPanel->spectrumFrequencyMaxGui->setValue(guiControlPanel->synthSketch->spectrumFrequencyLimits.y);
        }
      }

      void onKeyDown(GuiElement *guiElement, Events &events) {
        if(guiElement == guiControlPanel->backgroundVisualizationGui && events.sdlKeyCode == SDLK_HOME) {
          if(guiControlPanel->synthSketch->backgroundVisualization == 2) {
            if(guiControlPanel->synthSketch->mandelbrotVisualizer.getPanel()) guiControlPanel->synthSketch->mandelbrotVisualizer.removePanel(guiControlPanel->parentGuiElement);
            else guiControlPanel->synthSketch->mandelbrotVisualizer.addPanel(guiControlPanel->parentGuiElement);
          }
          if(guiControlPanel->synthSketch->backgroundVisualization == 3) {
            if(guiControlPanel->synthSketch->warpVisualizer.getPanel()) guiControlPanel->synthSketch->warpVisualizer.removePanel(guiControlPanel->parentGuiElement);
            else guiControlPanel->synthSketch->warpVisualizer.addPanel(guiControlPanel->parentGuiElement);
          }
        }
      }

    };
  };
  
  
  Synth *synth = NULL;

  int infoMode = 0;

  Quad quad;
  GlShader synthShader;

  EarTrainer earTrainer;

  bool isDebugMode = false;

  // FIXME these do not belong here
  //std::vector<Panel*> postProcessingEffectPanels;
  bool postProcessingEffectPanelsVisible = false, inputPostProcessingEffectPanelsVisible = false;

  CommandTemplate saveInstrumentCmdTmpl, loadInstrumentCmdTmpl;
  CommandTemplate addInstrumentCmdTmpl, removeInstrumentCmdTmpl;
  CommandTemplate duplicateInstrumentCmdTmpl;
  CommandTemplate addDrumPadCmdTmpl;

  CommandTemplate setSpectrumColorMapCmdTmpl;

  CommandTemplate saveFlameVisualizerSettingsCmdTmpl, loadFlameVisualizerSettingsCmdTmpl;
  CommandTemplate resetFlameVisualizerSettingsCmdTmpl;

  CommandTemplate saveReverbPresetCmdTmpl;
  
  CommandTemplate printPadControlsCmdTmpl;
  
  CommandTemplate sendSysExMessageCmdTmpl;

  bool addInstrumentRequested = false, removeInstrumentRequested = false, removeAllInstrumentsRequested = false;
  bool addDrumPadRequested = false;
  bool duplicateInstrumentRequested = false;
  std::string duplicateInstrumentName;


  int volumeUnit = 1;

  const std::vector<std::string> spectrumModeNames = {"None", "Bars", "Y-Lines", "X-lines", "XY-lines", "Smooth fill", "Smooth line"};
  int spectrumMode = 4;

  int visibleSpectrumSize = 99999;

  double spectrumGraphScaling = 0.5;
  double spectrumGraphFeedback = 0.8;

  const std::vector<std::string> backgroundVisualizationNames = {"None", "Spectrum", "Mandelbrot", "Warp torrents", "Fractal Flame"};
  int backgroundVisualization = 1;

  WarpVisualizer warpVisualizer;
  MandelbrotVisualizer mandelbrotVisualizer;
  FlameVisualizer flameVisualizer;
  SpectrumMap spectrumMap;

  int volumeLevelMode = 2;
  int graphColorIndex = 0;
  
  Vec4d spectrumGraphColor = Vec4d(1, 1, 1, 0.33);
  
  
  std::vector<double> spectrumGraph, spectrumGraphPrevious, spectrumGraphOriginal;
  
  bool spectrumAsNotes = false;

  Vec2d spectrumFrequencyLimits = Vec2d(1, 10000);

  double showSpectrumNoteThreshold = 2;
  bool roundSpectrumNoteLocation = false;
  const std::vector<std::string> spectrumNoteInfoNames = {"None", "Note", "Note/Oct", "Note/Oct/Cents", "Note/Oct/Cents/Freq"};
  int spectrumNoteInfoMode = 0;

  std::unordered_map<std::string, NoteInfo> retainedSpectrumNotes;

  enum WaveformMode { NoWaveform, LeftWaveform, RightWaveform, SummedWaveform, BothWaveforms};
  const std::vector<std::string> waveformModeNames = { "None", "Left", "Right", "Sum", "Both"};
  WaveformMode waveformMode = LeftWaveform;
  //bool showWaveform = true;
  bool showStereoOscilloscope = false;
  double waveformScaling = 0.6;
  double waveFormLowestFrequency = 40;
  double waveFormFeedback = 0.8;
  std::vector<Vec2d> waveForm;
  
  bool showEnvelope = false;
  double envelopeScaling = 1.0;
  

  SynthLaunchPad *synthLaunchPad = NULL;

  const std::vector<std::string> graphColorNames = {"White", "Black"};
  const std::vector<std::string> volumeLevelModeNames = {"None", "Left", "Right", "Top left", "Top right"};
  const std::vector<std::string> volumeUnitNames = {"Amplitude", "Decibel"};

  GuiControlPanel *guiControlPanel;
  SynthGui synthGui;

  
  RotatingRectVisualization rotatingRectVisualization;

  AudioPlayer *audioPlayer = NULL;

  void onReloadShaders() {
    synthShader.create("data/glsl/basic.vert", "data/glsl/simplesynth.frag");
    mandelbrotVisualizer.loadShader();
    warpVisualizer.loadShader();
    flameVisualizer.loadShader();
  }

  void onInit() {
    warpVisualizer.init();
    mandelbrotVisualizer.init(events, screenW, screenH);
    flameVisualizer.init(events, screenW, screenH, &guiRoot);
    spectrumMap.init(&guiRoot);
    spectrumMap.setActive(true);

    visibleSpectrumSize = min(visibleSpectrumSize, screenW);

    onReloadShaders();
    //std::string str = std::to_string(3.1415926535);
    //printf("%s\n", str.c_str());

    saveReverbPresetCmdTmpl.addArgument("filepath", STR_STRING);
    saveReverbPresetCmdTmpl.finishInitialization("saveReverb");
    commandQueue.addCommandTemplate(&saveReverbPresetCmdTmpl);

    saveInstrumentCmdTmpl.addArgument("filepath", STR_STRING);
    saveInstrumentCmdTmpl.finishInitialization("saveInstrument");
    commandQueue.addCommandTemplate(&saveInstrumentCmdTmpl);

    loadInstrumentCmdTmpl.addArgument("filepath", STR_STRING);
    loadInstrumentCmdTmpl.finishInitialization("loadInstrument");
    commandQueue.addCommandTemplate(&loadInstrumentCmdTmpl);

    addInstrumentCmdTmpl.finishInitialization("addInstrument");
    commandQueue.addCommandTemplate(&addInstrumentCmdTmpl);
    
    addDrumPadCmdTmpl.finishInitialization("addDrumPad");
    commandQueue.addCommandTemplate(&addDrumPadCmdTmpl);

    removeInstrumentCmdTmpl.addArgument("wildcard", STR_STRING, "xxx");
    removeInstrumentCmdTmpl.finishInitialization("removeInstrument");
    commandQueue.addCommandTemplate(&removeInstrumentCmdTmpl);

    duplicateInstrumentCmdTmpl.addArgument("name", STR_STRING);
    duplicateInstrumentCmdTmpl.finishInitialization("duplicateInstrument");
    commandQueue.addCommandTemplate(&duplicateInstrumentCmdTmpl);

    setSpectrumColorMapCmdTmpl.addArgument("index", STR_INT, "0");
    setSpectrumColorMapCmdTmpl.finishInitialization("setSpectrumColorMap");
    commandQueue.addCommandTemplate(&setSpectrumColorMapCmdTmpl);

    saveFlameVisualizerSettingsCmdTmpl.addArgument("filename", STR_STRING);
    saveFlameVisualizerSettingsCmdTmpl.finishInitialization("saveFlameVisualizerSettings");
    commandQueue.addCommandTemplate(&saveFlameVisualizerSettingsCmdTmpl);

    loadFlameVisualizerSettingsCmdTmpl.addArgument("filename", STR_STRING);
    loadFlameVisualizerSettingsCmdTmpl.finishInitialization("loadFlameVisualizerSettings");
    commandQueue.addCommandTemplate(&loadFlameVisualizerSettingsCmdTmpl);

    resetFlameVisualizerSettingsCmdTmpl.finishInitialization("resetFlameVisualizerSettings");
    commandQueue.addCommandTemplate(&resetFlameVisualizerSettingsCmdTmpl);

    printPadControlsCmdTmpl.finishInitialization("printPadControls");
    commandQueue.addCommandTemplate(&printPadControlsCmdTmpl);
    
    sendSysExMessageCmdTmpl.addArgument("channel", STR_INT);
    sendSysExMessageCmdTmpl.addArgument("type", STR_INT);
    sendSysExMessageCmdTmpl.addArgument("arg0", STR_STRING, " ");
    sendSysExMessageCmdTmpl.addArgument("arg1", STR_STRING, " ");
    sendSysExMessageCmdTmpl.addArgument("arg2", STR_STRING, " ");
    sendSysExMessageCmdTmpl.addArgument("arg3", STR_STRING, " ");
    sendSysExMessageCmdTmpl.addArgument("arg4", STR_STRING, " ");
    sendSysExMessageCmdTmpl.addArgument("arg5", STR_STRING, " ");
    sendSysExMessageCmdTmpl.addArgument("arg6", STR_STRING, " ");
    sendSysExMessageCmdTmpl.addArgument("arg7", STR_STRING, " ");
    sendSysExMessageCmdTmpl.addArgument("arg8", STR_STRING, " ");
    sendSysExMessageCmdTmpl.addArgument("arg9", STR_STRING, " ");
    sendSysExMessageCmdTmpl.finishInitialization("sendSysExMessage");
    commandQueue.addCommandTemplate(&sendSysExMessageCmdTmpl);

    if(cliArgs.hasKey("-debug")) {
      isDebugMode = true;
    }

    int sampleRate = -1;
    int framesPerBuffer = 256;
    double suggestedOutputLatency = -1;
    int suggestedAudioDevice = -1;

    if(cliArgs.numValues("-samplerate") > 0) {
      sampleRate = atoi(cliArgs.getValues("-samplerate")[0].c_str());
    }
    if(cliArgs.numValues("-framesperbuffer") > 0) {
      framesPerBuffer = atof(cliArgs.getValues("-framesperbuffer")[0].c_str());
    }
    if(cliArgs.numValues("-latency") > 0) {
      suggestedOutputLatency = atof(cliArgs.getValues("-latency")[0].c_str());
    }
    if(cliArgs.numValues("-device") > 0) {
      suggestedAudioDevice = atoi(cliArgs.getValues("-device")[0].c_str());
    }
    
    synth = new Synth(sampleRate, framesPerBuffer, suggestedOutputLatency, screenW, screenH, suggestedAudioDevice);
    paInterface = synth;

    //synthShader.create("data/glsl/basic.vert", "data/glsl/simplesynth.frag");
    quad.create(screenW, screenH);

    earTrainer.init(synth);

    synthLaunchPad = new SynthLaunchPad();
    synthLaunchPad->init(&guiRoot, &midiInterface, &spectrumGraph, synth);
    
    synthGui.init(synth, &midiInterface, &guiRoot, screenW, screenH);
    
    guiControlPanel = new GuiControlPanel(this, synth, &guiRoot);
    guiControlPanel->setPosition(10, screenH-10-guiControlPanel->size.y);
    guiControlPanel->setVisible(false);
    
    rotatingRectVisualization.init(guiControlPanel);
    
    guiRoot.addChildElement(audioPlayer = new AudioPlayer((int)synth->sampleRate, synth->framesPerBuffer));
    audioPlayer->setVisible(false);

    midiInterface.addMidiMessageListener(synth->getMidiMessageListener());
    midiInterface.addMidiMessageListener(synthGui.getMidiMessageListener());
    midiInterface.addMidiMessageListener(synthLaunchPad->getMidiMessageListener());
    midiInterface.addMidiMessageListener(earTrainer.getMidiMessageListener());
  }


  void onQuit() {
    if(synthLaunchPad) {
      synthLaunchPad->finish();
    }

    synth->onQuit();
  }





  void onKeyUp() {
    synth->onKeyUpKeyboardPiano(events);

    if(backgroundVisualization == 4) {
      flameVisualizer.onKeyUp(events);
    }
  }

  void onKeyDown() {

    synth->onKeyDownLooperControls(events);

    synth->onKeyDownKeyboardPiano(events);
    
    if(backgroundVisualization == 1) {
      spectrumMap.onKeyDown(events);
    }
    else if(backgroundVisualization == 4) {
      flameVisualizer.onKeyDown(events);
    }

    synthGui.onKeyDown(events);

    if(events.sdlKeyCode == SDLK_F11) {
      if(earTrainer.panel) {
        earTrainer.removePanel(&guiRoot);
        earTrainer.isActive = false;
      }
      else {
        earTrainer.isActive = true;
        earTrainer.addPanel(&guiRoot);
      }
    }
    earTrainer.onKeyDown(events);

    if(events.sdlKeyCode == SDLK_F1 && events.lControlDown && events.lShiftDown) {
      infoMode = (infoMode+1) % 4;
    }

    if(events.sdlKeyCode == SDLK_F10 && events.lControlDown && events.lShiftDown) {
      printf("instruments:\n");
      for(int i=0; i<synth->instruments.size(); i++) {
        printf("%d %s, %s %s\n", i, synth->instruments[i]->name.c_str(), synth->instruments[i]->isActive ? "[active]" : "", synth->instruments[i]->inactivationRequested ? "[inactivation requested]" : "");
      }
    }

    // FIXME Gonna do that effect track interface soon enough...
    if(events.sdlKeyCode == SDLK_F7 && events.numModifiersDown == 0) {
      postProcessingEffectPanelsVisible = !postProcessingEffectPanelsVisible;
      Vec2d pos(40, 40);
      if(postProcessingEffectPanelsVisible) {
        for(int i=0; i<synth->postProcessingEffects.size(); i++) {
          Panel *panel = synth->postProcessingEffects[i]->addPanel(&guiRoot);
          panel->pos = pos;
          pos.y += panel->size.y + 5;
        }
      }
      else {
        for(int i=0; i<synth->postProcessingEffects.size(); i++) {
          synth->postProcessingEffects[i]->removePanel(&guiRoot);
        }
      }
    }
    if(events.sdlKeyCode == SDLK_F7 && events.numModifiersDown == 1 && (events.lControlDown || events.rControlDown)) {
      inputPostProcessingEffectPanelsVisible = !inputPostProcessingEffectPanelsVisible;
      Vec2d pos(240, 40);
      if(inputPostProcessingEffectPanelsVisible) {
        for(int i=0; i<synth->inputPostProcessingEffects.size(); i++) {
          Panel *panel = synth->inputPostProcessingEffects[i]->addPanel(&guiRoot);
          panel->pos = pos;
          pos.y += panel->size.y + 5;
        }
      }
      else {
        for(int i=0; i<synth->inputPostProcessingEffects.size(); i++) {
          synth->inputPostProcessingEffects[i]->removePanel(&guiRoot);
        }
      }
    }

    if(events.sdlKeyCode == SDLK_F2 && events.numModifiersDown == 0) {
      if(guiControlPanel) {
        guiControlPanel->toggleVisibility();
      }
    }
    
    if(events.sdlKeyCode == SDLK_F12 && events.numModifiersDown == 0) {
      synth->reset();
      waveForm.assign(waveForm.size(), Vec2d::Zero);
    }

    /*if(events.sdlKeyCode == SDLK_F12 && events.lControlDown) {
      for(int i=0; i<synth->postProcessingEffects.size(); i++) {
        synth->postProcessingEffects[i]->isActive = !synth->postProcessingEffects[i]->isActive;
        if(synth->postProcessingEffects[i]->isActive) printf("Effect activated\n");
        else printf("Effect inactivated\n");
      }
    }*/

    if(events.sdlKeyCode == SDLK_F10) {
      if(synthLaunchPad) {
        synthLaunchPad->panel->toggleVisibility();
      }
    }
    if(events.sdlKeyCode == SDLK_F8) {
      if(audioPlayer) {
        audioPlayer->toggleVisibility();
      }
    }
  }

  void toggleUseEnvelopeLut() {
    /*synth->getInstrument()->getOscillatorSet()->getAmplitudeEnvelope()->useLut = !synth->getInstrument()->getOscillatorSet()->getAmplitudeEnvelope()->useLut;
    if(synth->getInstrument()->getOscillatorSet()->getAmplitudeEnvelope()->useLut) {
      synth->getInstrument()->getOscillatorSet()->getAmplitudeEnvelope()->prepareLookupBuffers();
    }*/
  }

  void onMousePressed() {
    if(!guiRoot.isPointWithinChildElements(events.m)) {
      synth->onMousePressedScreenKeys(events, screenW, screenH);
    }
  }
  void onMouseReleased() {
    synth->onMouseReleasedScreenKeys(events, screenW, screenH);
  }

  void onMouseMotion() {
    if(backgroundVisualization == 4) {
      flameVisualizer.onMouseMotion(events);
    }
  }

  void onMouseWheel() {
    if(backgroundVisualization == 4) {
      flameVisualizer.onMouseWheel(events);
    }
  }

  void loadInstrument(const std::string &path, bool isAbsolutePath = false) {
    if(synth->loadInstrumentFromFile(path, isAbsolutePath)) {
      messageQueue.addMessage(to_wstring("Instrument '" + extractFilename(path) + "' loaded!"));
    }
  }
  void onUpdate() {
    //synth->onUpdateGetMidiMessages(midiInterface);

    for(int i=0; i<commandQueue.commands.size(); i++) {
      Command *cmd = NULL;


      if(cmd = commandQueue.popCommand(&sendSysExMessageCmdTmpl)) {
        int channel = 0;
        int type = 0;
        std::vector<std::string> args;
        sendSysExMessageCmdTmpl.fillValues(cmd, &channel, &type);
        for(int i=0; i<sendSysExMessageCmdTmpl.getNumArguments()-2; i++) {
          std::string str;
          sendSysExMessageCmdTmpl.fillValueInd(cmd, &str, 2+i);
          args.push_back(str);
        }
        SysExMessage sysExMessage;
        sysExMessage.beginMessage();
        sysExMessage.bytes.push_back((unsigned char)type);
        for(int i=0; i<args.size(); i++) {
          if(i > 0) sysExMessage.bytes.push_back(' ');
          for(int k=0; k<args[i].size(); k++) {
            sysExMessage.bytes.push_back(args[i][k]);
          }
        }
        sysExMessage.endMessage();
        midiInterface.sendSysExMessage(sysExMessage);
        sysExMessage.createLcdMessage("test..");
        midiInterface.sendSysExMessage(sysExMessage);
        delete cmd;
      }

      if(cmd = commandQueue.popCommand(&saveFlameVisualizerSettingsCmdTmpl)) {
        std::string filename;
        saveFlameVisualizerSettingsCmdTmpl.fillValues(cmd, &filename);
        if(filename.size() > 0) {
          flameVisualizer.saveToFile(filename);
        }
        delete cmd;
      }
      if(cmd = commandQueue.popCommand(&loadFlameVisualizerSettingsCmdTmpl)) {
        std::string filename;
        loadFlameVisualizerSettingsCmdTmpl.fillValues(cmd, &filename);
        if(filename.size() > 0) {
          flameVisualizer.loadFromFile(filename);
        }
        delete cmd;
      }
      if(cmd = commandQueue.popCommand(&resetFlameVisualizerSettingsCmdTmpl)) {
        flameVisualizer.reset();
        delete cmd;
      }
      if(cmd = commandQueue.popCommand(&printPadControlsCmdTmpl)) {
        synthLaunchPad->printControls();
        delete cmd;
      }

      if(cmd = commandQueue.popCommand(&addInstrumentCmdTmpl)) {
        addInstrumentRequested = true;
        synth->synthThreadDisableRequested = true;
        delete cmd;
      }
      
      if(cmd = commandQueue.popCommand(&addDrumPadCmdTmpl)) {
        addDrumPadRequested = true;
        synth->synthThreadDisableRequested = true;
        delete cmd;
      }
            
      if(cmd = commandQueue.popCommand(&duplicateInstrumentCmdTmpl)) {
        duplicateInstrumentCmdTmpl.fillValues(cmd, &duplicateInstrumentName);
        duplicateInstrumentRequested = true;
        synth->synthThreadDisableRequested = true;
        delete cmd;
      }
      if(cmd = commandQueue.popCommand(&removeInstrumentCmdTmpl)) {
        std::string wildcard = "";
        removeInstrumentCmdTmpl.fillValues(cmd, &wildcard);
        if(wildcard.compare("*") == 0) {
          removeAllInstrumentsRequested = true;
        }
        else {
          removeInstrumentRequested = true;
        }
        synth->synthThreadDisableRequested = true;
        delete cmd;
      }

      if(cmd = commandQueue.popCommand(&saveInstrumentCmdTmpl)) {
        std::string filename = "";
        saveInstrumentCmdTmpl.fillValues(cmd, &filename);
        if(synth->saveInstrumentToFile(filename)) {
          messageQueue.addMessage(to_wstring("Instrument '" + filename + "' saved!"));
        }
        delete cmd;
      }
      
      if(cmd = commandQueue.popCommand(&saveReverbPresetCmdTmpl)) {
        std::string filename = "";
        saveReverbPresetCmdTmpl.fillValues(cmd, &filename);
        //if(synth->saveEffectToFile(synth->reverb, filename)) {
        if(synth->saveReverbToFile(filename)) {
          messageQueue.addMessage(to_wstring("Reverb '" + filename + "' saved!"));
        }
        delete cmd;
      }

      if(cmd = commandQueue.popCommand(&loadInstrumentCmdTmpl)) {
        std::string path = "";
        loadInstrumentCmdTmpl.fillValues(cmd, &path);

        if(path.compare("*") == 0) {
          path = "";
        }

        if(isDirectory(synth->defaultInstrumentPath + path)) {
          Directory dir(synth->defaultInstrumentPath + path);
          std::vector<std::string> files;
          dir.getFilesRecursively(files);

          for(int i=0; i<files.size(); i++) {
            loadInstrument(files[i], true);
          }
        }
        else {
          loadInstrument(path);
        }
        synthGui.onInstrumentListUpdated();

        delete cmd;
      }

    }

    if(synth->synthThreadDisableRequested) {
      synth->synthThreadDisableRequested = false;
      synth->synthThreadDisabled = true;

      while(synth->synthThreadExecuting) {}

      if(synth->numCurrentlyPlayingNotes != synth->requestedNumberOfNotes) {
        synth->requestedNumberOfNotes = synth->setNumCurrentlyPlayingNotes(synth->requestedNumberOfNotes);
      }
      if(synth->numCurrentlyPlayingNotesPresampled != synth->requestedNumberOfNotesPresampled) {
        synth->requestedNumberOfNotesPresampled = synth->setNumCurrentlyPlayingNotesPresampled(synth->requestedNumberOfNotesPresampled);
      }

      if(addInstrumentRequested) {
        addInstrumentRequested = false;
        synth->addInstrument();
        synthGui.onInstrumentListUpdated();
      }
      if(addDrumPadRequested) {
        addDrumPadRequested = false;
        synth->addDrumPad();
        synthGui.onInstrumentListUpdated();
      }
      if(duplicateInstrumentRequested) {
        duplicateInstrumentRequested = false;
        synth->duplicateInstrument(duplicateInstrumentName);
        synthGui.onInstrumentListUpdated();
      }
      if(removeInstrumentRequested) {
        removeInstrumentRequested = false;
        /*if(synth->getInstrument()->instrumentPanel) {
          synth->getInstrument()->removePanel(&guiRoot);
        }*/
        synth->removeInstrument();
        synthGui.onInstrumentListUpdated();
        // FIXME visibility...
        //synth->getInstrument()->addPanel(&guiRoot);
        //synth->getInstrument()->getPanel()->setPosition(screenW - synth->getInstrument()->getPanel()->size.x - 10, 10);
      }
      if(removeAllInstrumentsRequested) {
        removeAllInstrumentsRequested = false;
        for(int i=synth->instruments.size()-1; i>0; i--) {
          /*if(synth->instruments[i]->instrumentPanel) {
            synth->instruments[i]->removePanel(&guiRoot);
          }*/
          synth->removeInstrument(i);
        }
        synthGui.onInstrumentListUpdated();
        // Fixme visibility...
        //synth->getInstrument()->addPanel(&guiRoot);
        //synth->getInstrument()->getPanel()->setPosition(screenW - synth->getInstrument()->getPanel()->size.x - 10, 10);
      }
      synth->synthThreadDisabled = false;
    }


    synth->updateAudioRecording();

    /* TODO Midi events read in seperate thread to minimize the latency
     *
     */
    /*for(int i=0; i<midiInterface.midiMessages.size(); i++) {
      
      //midiInterface.midiMessages[i].print();
      
      if(midiInterface.midiPorts[midiInterface.midiMessages[i].midiPortIndex].type == MidiInterface::MidiPort::Type::LaunchPad) {
        if(synthGui.midiEventPanel) {
          synthGui.midiEventPanel->update(midiInterface.midiMessages[i]);
        }
        continue;
      }
      
      HuiMessage huiMessage;
      int size = huiMessage.decode(midiInterface.midiMessages, i);
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
          synth->progressLoopTimeByBeats(-1);
        }
        if(huiMessage.type == HuiMessage::Type::UpPressed) {
          synth->progressLoopTimeByBeats(1);
        }
        synthGui.onHuiMessage(huiMessage);
        i += size - 1;
        continue;
      }

      int midiPortIndex = midiInterface.midiMessages[i].midiPortIndex;
      int channel = midiInterface.midiMessages[i].getMessageChannel() + 1;

      std::vector<int> instrumentTrackIndices;
      for(int k=0; k<synth->numInstrumentTracks; k++) {
        if(channel != 0 && (channel == synth->instrumentTracks[k].midiInChannel || synth->instrumentTracks[k].midiInChannel == 0) && midiPortIndex == synth->instrumentTracks[k].midiPortIndex) {
          //instrumentTrackIndex = synth->instrumentTracks[i].instrumentIndex;
          instrumentTrackIndices.push_back(k);
        }
      }
      //if(instrumentTrackIndices.size() == 0 && synth->instruments.size() > 0 && synth->getInstrumentTrack()->midiInChannel == 0) {
        //instrumentIndex = synth->activeInstrumentIndex;
        //instrumentTrackIndices.push_back(synth->activeInstrumentIndex);
      //}

      if(instrumentTrackIndices.size() != 0) {
        std::set<int> instrumentIndices;
        for(int k=0; k<instrumentTrackIndices.size(); k++) {
          instrumentIndices.insert(synth->instrumentTracks[instrumentTrackIndices[k]].instrumentIndex);
        }

        if(midiInterface.midiMessages[i].getMessageType() == MidiMessage::ControlChange) {
          //if(midiInterface.midiMessages[i].dataByte1 == MidiMessage::ChannelVolume) {
          //  for(int k=0; k<instrumentTrackIndices.size(); k++) {
            //  synth->instrumentTracks[instrumentTrackIndices[k]].volume = (double)midiInterface.midiMessages[i].dataByte2 / 127.0;
          //  }
          //}

          if(midiInterface.midiMessages[i].dataByte1 == MidiMessage::SustainPedal) {
            for(int inin : instrumentIndices) {
              synth->setSustain(midiInterface.midiMessages[i].dataByte2 >= 64, inin);
            }
          }
        }

        bool noteOffWithNoteOn = false;

        if(midiInterface.midiMessages[i].getMessageType() == MidiMessage::NoteOn) {
          if(midiInterface.midiMessages[i].dataByte2 > 0) {
            int pitch = midiInterface.midiMessages[i].dataByte1;
            if(earTrainer.isActive) {
              earTrainer.guessPitch(pitch);
            }
            else {
              for(int k=0; k<instrumentTrackIndices.size(); k++) {
                synth->midiTrackNoteOn(midiInterface.midiMessages[i].dataByte1, midiInterface.midiMessages[i].dataByte2, midiPortIndex, instrumentTrackIndices[k]);
              }
            }
          }
          else {
            noteOffWithNoteOn = true;
          }
        }
        if(noteOffWithNoteOn || midiInterface.midiMessages[i].getMessageType() == MidiMessage::NoteOff) {
          for(int k=0; k<instrumentTrackIndices.size(); k++) {
            synth->midiTrackNoteOff(midiInterface.midiMessages[i].dataByte1, midiInterface.midiMessages[i].dataByte2, midiPortIndex, instrumentTrackIndices[k]);
          }
        }
      }
      synthGui.onMidiMessage(midiInterface.midiMessages[i]);
    }*/
    // FIXME 
    if(synthLaunchPad) {
      synthLaunchPad->midiPortIndex = -1;
      for(int i=0; i<synth->numInstrumentTracks; i++) {
        if(midiInterface.midiPorts[synth->instrumentTracks[i].midiPortIndex]->type == MidiPortType::LaunchPad) {
          synthLaunchPad->midiPortIndex = synth->instrumentTracks[i].midiPortIndex;
          if(synthLaunchPad->sequencerLaunchpadMode) {
            synthLaunchPad->sequencerLaunchpadMode->instrumentTrackIndex = i;
          }
          break;
        }
      }
      synthLaunchPad->update(time, dt);
    }

    if(audioPlayer) {
      audioPlayer->update(synth->recordingPlaybackBuffer, synth->recordingPlaybackBufferPosition);
    }

    synthGui.onUpdate(time, dt);
    
  }




  Vec2d volumeMeterMaxLevel;
  double volumeMeterMaxLevelDropRate = 0.1;
  double volumeMeterTooLoudThreshold = 0.666;

  std::vector<Vec2d> envelopeBuffer;
  int envelopeBufferPosition = 0;

  // FIXME get rid of this
  double interpolate(const std::vector<double> &srcArr, int destinationArraySize, int destinationArrayIndex) {
    double d = (double)destinationArrayIndex/destinationArraySize * srcArr.size();
    int a = int(d);
    int b = a + 1;
    double f = d - a;
    if(a < 0) return srcArr[0];
    if(b >= srcArr.size()) return srcArr[srcArr.size()-1];
    return (1.0 - f) * srcArr[a] + f * srcArr[b];
  }
  

  void onDraw() {
    clear(0, 0, 0, 1);

    if(isDebugMode) return;

    bool isScreenKeys = synth->isScreenKeysVisible();
    bool isDarkGraphColors = isScreenKeys || graphColorIndex == 1;

    Vec2d delayLineRms = synth->delayLine.getRMS(0.05) * 2.0;

    for(int i=0; i<synth->delayLineFFTW.getSize(); i++) {
      Vec2d v = synth->delayLine.getOutputSample(i);
      synth->delayLineFFTW.input[i] = v.x;
      //synth->delayLineFFTWRight.input[i] = v.y;
      //maxAmp = max(maxAmp, synth->delayLineFFTW.input[i]);
    }
    synth->delayLineFFTW.transform();

    if(spectrumGraph.size() != visibleSpectrumSize) {
      spectrumGraph.resize(visibleSpectrumSize);
      spectrumGraphOriginal.resize(visibleSpectrumSize);
      spectrumGraphPrevious.assign(visibleSpectrumSize, 0);
    }
    else {
      spectrumGraphPrevious = spectrumGraph;
    }

    if(spectrumAsNotes) {
      double minNote = freqToNote(spectrumFrequencyLimits.x);
      double maxNote = freqToNote(spectrumFrequencyLimits.y);
      for(int i=0; i<spectrumGraph.size(); i++) {
        spectrumGraph[i] = getPitchAmplitudeFromFrequencySpectrum(synth->sampleRate, synth->delayLineFFTW.output, minNote, maxNote, i, spectrumGraph.size());
      }
    }
    else {
      int f0 = (int)map(spectrumFrequencyLimits.x, 0.0, synth->sampleRate/2.0, 0, synth->delayLineFFTW.output.size()/2);
      int f1 = (int)map(spectrumFrequencyLimits.y, 0.0, synth->sampleRate/2.0, 0, synth->delayLineFFTW.output.size()/2);

      resizeArray(synth->delayLineFFTW.output, spectrumGraph, f0, f1);
    }



    for(int i=0; i<spectrumGraph.size(); i++) {
      if(std::isnan(spectrumGraph[i])) {
        spectrumGraph[i] = 0.0;
      }
      if(volumeUnit == 1) {
        if(spectrumGraph[i] <= 1.0e-5) {
          spectrumGraph[i] = 0;
        }
        else {
          spectrumGraph[i] = log10(spectrumGraph[i]);
          spectrumGraph[i] = (spectrumGraph[i] + 5.0) / 5.0;
        }
      }
    }
    spectrumGraphOriginal = spectrumGraph;
    for(int i=0; i<spectrumGraph.size(); i++) {
      spectrumGraph[i] = spectrumGraph[i] * (1.0-spectrumGraphFeedback) + spectrumGraphPrevious[i] * spectrumGraphFeedback;
    }


    int fr0 = (int)(spectrumFrequencyLimits.x * synth->delayLineFFTW.output.size()/synth->sampleRate);
    int fr1 = (int)(spectrumFrequencyLimits.y * synth->delayLineFFTW.output.size()/synth->sampleRate);
    int n = 0;
    double mean = 0;
    for(int i=fr0; i<fr1; i++) {
      if(synth->delayLineFFTW.output[i] > 1.0e-10) {
        n++;
        mean += synth->delayLineFFTW.output[i];
      }
    }
    mean /= n;

    std::vector<int> peakIndexes;
    n = 0;
    for(int i=1; i<synth->delayLineFFTW.output.size()/2-1; i++) {
      double a = synth->delayLineFFTW.output[i-1];
      double b = synth->delayLineFFTW.output[i];
      double c = synth->delayLineFFTW.output[i+1];
      if(a < b && b > c && b > mean * showSpectrumNoteThreshold*synth->masterVolume) {
        peakIndexes.push_back(i);
        n++;
      }
    }

    for(int i=0; i<n-1; i++) {
      double a = freqToNote((double)peakIndexes[i]/synth->delayLineFFTW.output.size()*synth->sampleRate);
      double b = freqToNote((double)peakIndexes[i+1]/synth->delayLineFFTW.output.size()*synth->sampleRate);
      if(fabs(a-b) < 1.0) {
        if(synth->delayLineFFTW.output[peakIndexes[i]] < synth->delayLineFFTW.output[peakIndexes[i+1]]) {
          peakIndexes.erase(peakIndexes.begin()+i);
        }
        else {
          peakIndexes.erase(peakIndexes.begin()+i+1);
        }
        i--;
        n--;
      }
    }

    std::vector<NoteInfo> peakNotes(peakIndexes.size());
    for(int i=0; i<peakIndexes.size(); i++) {
      peakNotes[i].setFromFrequency((double)peakIndexes[i]/synth->delayLineFFTW.output.size()*synth->sampleRate);
      peakNotes[i].volume = synth->delayLineFFTW.output[peakIndexes[i]];
      peakNotes[i].time = time;
      std::string noteKeyStr = peakNotes[i].noteName + std::to_string(peakNotes[i].octave);
      retainedSpectrumNotes[noteKeyStr] = peakNotes[i];
    }

    if(isScreenKeys) {
      synthShader.activate();
      synthShader.setUniform2f("screenSize", (float)screenW, (float) screenH);
      synthShader.setUniform2f("mousePos", (float)events.mouseX, (float) events.mouseY);
      synthShader.setUniform1d("time", synth->getPaTime());
      synth->setShaderUniforms(synthShader);
      quad.render();
      synthShader.deActivate();
    }
    else if(backgroundVisualization == 1) {
      spectrumMap.update(screenW, screenH, spectrumGraphOriginal, spectrumGraph, synth->delayLine, spectrumFrequencyLimits, spectrumAsNotes, volumeUnit);
      spectrumMap.render();
    }
    else if(backgroundVisualization == 2) {
      mandelbrotVisualizer.update(spectrumGraph);
      mandelbrotVisualizer.render(screenW, screenH, time, events);
    }
    else if(backgroundVisualization == 3) {
      warpVisualizer.update(spectrumGraph, dt);
      warpVisualizer.render(screenW, screenH, time, events);
    }
    else if(backgroundVisualization == 4) {
      flameVisualizer.update(spectrumGraph, (delayLineRms.x + delayLineRms.y) * 0.5);
      flameVisualizer.render(textRenderer, screenW, screenH, time, dt, events, isDarkGraphColors);
    }


    synth->drawSynthNoteLayer(screenW, screenH, geomRenderer);

    //scaleArray(spectrumGraph, spectrumGraphScaling);

    double line = -13, lineHeight = 23;
    char buf[128];

    if(infoMode > 0) {
      geomRenderer.texture = NULL;
      geomRenderer.fillColor = isScreenKeys ? Vec4d(0, 0, 0, 0.5) : Vec4d(1.0, 1.0, 1.0, 0.5);
      geomRenderer.strokeColor.set(0, 0, 0, 1);
      geomRenderer.strokeType = 0;
      geomRenderer.strokeWidth = 1;
      double cpuLoad = synth->getCpuLoad();
      geomRenderer.drawRect(cpuLoad*screenW, 5, cpuLoad*screenW*0.5, 2.5);
      textRenderer.setColor(1, 1, 1, 0.6);
      std::sprintf(buf, "cpu load %.2f %%", cpuLoad*100.0);
      textRenderer.print(buf, 10, line += lineHeight, 12);
      std::sprintf(buf, "fps %.4f ", dt == 0 ? 0 : 1.0/dt);
      textRenderer.print(buf, 10, line += lineHeight, 12);

      if(infoMode > 1) {
        int k = synth->getInstrument()->getTotalOscillatorCount() * synth->numCurrentlyPlayingNotes;
        textRenderer.print("oscillators "+std::to_string(k), 10, line += lineHeight, 12);
      }
      if(infoMode > 2) {
        std::sprintf(buf, "time %.2f s", synth->getPaTime());
        textRenderer.print(buf, 10, line += lineHeight, 12);
        std::sprintf(buf, "frame %d", synth->loopFrame);
        textRenderer.print(buf, 10, line += lineHeight, 12);
      }
    }

    if(isScreenKeys && !guiRoot.isInputGrabbedAnywhere && (events.lControlDown || events.rControlDown)) {
      int note = synth->getMouseHoverScreenKeysPitch(events, screenW, screenH);
      NoteInfo noteInfo;
      noteInfo.setFromPitch(note);
      Vec2d size = textRenderer.getDimensions(noteInfo.toString(), 12);
      double dx = - size.x / 2;
      if(events.mouseX - size.x/2 < 0) dx += events.mouseX - size.x/2;
      if(events.mouseX + size.x/2 > screenW-1) dx -= screenW-1 - (events.mouseX + size.x/2);
      textRenderer.print(noteInfo.toString(), events.mouseX + dx , events.mouseY-20, 12);
      /*double frequency = noteToFreq(note);
      std::sprintf(buf, "note %d, %.1f Hz", note, frequency);
      textRenderer.print(buf, 10, line += lineHeight, 12);*/
    }

    if(!isScreenKeys && !guiRoot.isInputGrabbedAnywhere && (events.lControlDown || events.rControlDown)) {
      NoteInfo noteInfo;
      if(spectrumAsNotes) {
        double minPitch = freqToNote(spectrumFrequencyLimits.x);
        double maxPitch = freqToNote(spectrumFrequencyLimits.y);
        double pitch = map(events.mouseX, 0, screenW-1, minPitch, maxPitch);
        noteInfo.setFromPitch(pitch);
      }
      else {
        double frequency = map(events.mouseX, 0.0, screenW-1, spectrumFrequencyLimits.x, spectrumFrequencyLimits.y);
        noteInfo.setFromFrequency(frequency);
      }
      Vec2d size = textRenderer.getDimensions(noteInfo.toString(), 12);
      double dx = - size.x / 2;
      if(events.mouseX - size.x/2 < 0) dx += events.mouseX - size.x/2;
      if(events.mouseX + size.x/2 > screenW-1) dx -= screenW-1 - (events.mouseX + size.x/2);
      textRenderer.print(noteInfo.toString(), events.mouseX + dx, events.mouseY-20, 12);
    }


    Vec4d colorMeterDefaultColor = isDarkGraphColors ? Vec4d(0, 0, 0, 1) : Vec4d(1.0, 1.0, 1.0, 1);
    Vec4d colorMeterTooLoudColor = isDarkGraphColors ? Vec4d(0.4, 0, 0, 1) : Vec4d(0.4, .0, .0, 1);
    Vec4d colorMeterMaxDefaultColor = isDarkGraphColors ? Vec4d(0, 0, 0, 0.5) : Vec4d(0.5, 0.5, 0.5, 0.5);
    Vec4d colorMeterMaxTooLoudColor = isDarkGraphColors ? Vec4d(0.3, 0, 0, 0.5) : Vec4d(0.3, 0, 0, 0.5);

    if(volumeLevelMode > 0) {

      double leftVol = clamp(amplitudeToVol(delayLineRms.x), 0, 1);
      double rightVol = clamp(amplitudeToVol(delayLineRms.y), 0, 1);
      double maxVol = clamp(amplitudeToVol(volumeMeterTooLoudThreshold), 0, 1);
      
      Vec2d levels(min(maxVol, leftVol), min(maxVol, rightVol));
      
      volumeMeterMaxLevel.x = min(1.0, max(volumeMeterMaxLevel.x, leftVol));
      volumeMeterMaxLevel.y = min(1.0, max(volumeMeterMaxLevel.y, rightVol));

      double x0, x1, x2, x3, x4, x5;
      double y0, y1, y2, y3, y4, y5;
      double w0, w1, w2, w3, w4, w5;
      double h0, h1, h2, h3, h4, h5;

      if(volumeLevelMode == 1 || volumeLevelMode == 2) {
        w0 = w1 = w2 = w3 = w4 = w5 = 10;
        h0 = volumeMeterMaxLevel.x*screenH;
        h1 = volumeMeterMaxLevel.y*screenH;
        h2 = leftVol*screenH;
        h3 = rightVol*screenH;
        h4 = levels.x*screenH;
        h5 = levels.y*screenH;
        if(volumeLevelMode == 1) {
          x0 = x2 = x4 = 5;
          x1 = x3 = x5 = 15;
        }
        else {
          x0 = x2 = x4 = screenW - 15;
          x1 = x3 = x5 = screenW - 5;
        }
        y0 = screenH - h0*0.5;
        y1 = screenH - h1*0.5;
        y2 = screenH - h2*0.5;
        y3 = screenH - h3*0.5;
        y4 = screenH - h4*0.5;
        y5 = screenH - h5*0.5;
      }
      if(volumeLevelMode == 3 || volumeLevelMode == 4) {
        h0 = h1 = h2 = h3 = h4 = h5 = 8;
        w0 = volumeMeterMaxLevel.x*screenW;
        w1 = volumeMeterMaxLevel.y*screenW;
        w2 = leftVol*screenW;
        w3 = rightVol*screenW;
        w4 = levels.x*screenW;
        w5 = levels.y*screenW;
        y0 = y2 = y4 = 4;
        y1 = y3 = y5 = 12;
        if(volumeLevelMode == 3) {
          x0 = w0*0.5;
          x1 = w1*0.5;
          x2 = w2*0.5;
          x3 = w3*0.5;
          x4 = w4*0.5;
          x5 = w5*0.5;
        }
        else {
         x0 = screenW - w0*0.5;
         x1 = screenW - w1*0.5;
         x2 = screenW - w2*0.5;
         x3 = screenW - w3*0.5;
         x4 = screenW - w4*0.5;
         x5 = screenW - w5*0.5;
       }
      }

      geomRenderer.startRendering();

      geomRenderer.texture = NULL;
      geomRenderer.strokeType = 0;
      geomRenderer.strokeWidth = 1;
      geomRenderer.strokeColor.set(0, 0, 0, 1);

      if(volumeMeterMaxLevel.x > maxVol) geomRenderer.fillColor = colorMeterMaxTooLoudColor;
      else geomRenderer.fillColor = colorMeterMaxDefaultColor;
      geomRenderer.drawRect(w0, h0, x0, y0);
      if(volumeMeterMaxLevel.y > maxVol) geomRenderer.fillColor = colorMeterMaxTooLoudColor;
      else geomRenderer.fillColor = colorMeterMaxDefaultColor;
      geomRenderer.drawRect(w1, h1, x1, y1);

      volumeMeterMaxLevel.x -= volumeMeterMaxLevelDropRate * dt;
      volumeMeterMaxLevel.y -= volumeMeterMaxLevelDropRate * dt;

      if(leftVol > maxVol) {
        geomRenderer.fillColor = colorMeterTooLoudColor;
        geomRenderer.drawRect(w2, h2, x2, y2);
      }
      if(rightVol > maxVol) {
        geomRenderer.fillColor = colorMeterTooLoudColor;
        geomRenderer.drawRect(w3, h3, x3, y3);
      }
      geomRenderer.fillColor = colorMeterDefaultColor;
      geomRenderer.drawRect(w4, h4, x4, y4);
      geomRenderer.drawRect(w5, h5, x5, y5);

      geomRenderer.endRendering();
    }

    if(isDarkGraphColors && rgbToHsl(spectrumGraphColor.x, spectrumGraphColor.y, spectrumGraphColor.z).z > 0.66) {
      geomRenderer.strokeColor.set(0, 0, 0, 0.33);
      geomRenderer.fillColor.set(0, 0, 0, 0.33);
    }
    else {
      geomRenderer.strokeColor = spectrumGraphColor;  
      geomRenderer.fillColor = spectrumGraphColor;  
    }


    if(spectrumMode == 1) {
      geomRenderer.startRendering();
      geomRenderer.strokeType = 0;
      for(int i=0; i<spectrumGraph.size(); i++) {
        double x0 = round(map(i, 0.0, spectrumGraph.size()-1, 0.0, screenW-1));
        double x1 = round(map(i+1, 0.0, spectrumGraph.size()-1, 0.0, screenW-1))+1;
        double y = screenH - spectrumGraph[i] * spectrumGraphScaling * screenH;
        geomRenderer.drawRectCorners(x0, screenH, x1, screenH - spectrumGraph[i] * screenH);
      }
      geomRenderer.endRendering();
    }

    geomRenderer.startRendering(GeomRenderer::RendererType::FastStrokeRenderer);

    if(spectrumMode == 2 || spectrumMode == 3 || spectrumMode == 4) {
      geomRenderer.strokeWidth = 2;
      geomRenderer.strokeType = 1;
      for(int i=0; i<spectrumGraph.size(); i++) {
        double x = map(i, 0.0, spectrumGraph.size()-1, 0.0, screenW-1);
        double y = screenH - spectrumGraph[i] * spectrumGraphScaling * screenH;
        if(spectrumMode == 2 || spectrumMode == 4) {
          geomRenderer.drawLine(x, screenH, x, y);
        }
        if(i < spectrumGraph.size()-1 && (spectrumMode == 3 || spectrumMode == 4)) {
          double x2 = map(i+1, 0.0, spectrumGraph.size()-1, 0.0, screenW-1);
          double y2 = screenH - spectrumGraph[i+1] * spectrumGraphScaling * screenH;
          geomRenderer.drawLine(x, y, x2, y2);
        }
      }
    }
    if(spectrumMode == 5 || spectrumMode == 6) {
      geomRenderer.strokeWidth = 2;
      geomRenderer.strokeType = 1;
      std::vector<double> spect(screenW);
      resizeArray(spectrumGraph, spect, 0, spectrumGraph.size(), 0, spect.size(), true);
      for(int i=0; i<spect.size(); i++) {
        double x = map(i, 0.0, spect.size()-1, 0.0, screenW-1);
        double y = screenH - spect[i] * spectrumGraphScaling * screenH;
        if(spectrumMode == 5) {
          geomRenderer.drawLine(x, screenH, x, y);
        }
        if(i < spect.size()-1 && (spectrumMode == 6)) {
          double x2 = map(i+1, 0.0, spect.size()-1, 0.0, screenW-1);
          double y2 = screenH - spect[i+1] * spectrumGraphScaling * screenH;
          geomRenderer.drawLine(x, y, x2, y2);
        }
      }
    }

    if(showStereoOscilloscope) {
      synth->stereoOscilloscope.render(geomRenderer, screenW, screenH, isDarkGraphColors, (delayLineRms.x + delayLineRms.y) * 0.5);
    }

    if(waveformMode != NoWaveform) {
      double d = synth->getPaTime();

      geomRenderer.strokeWidth = 2;
      if(isDarkGraphColors) geomRenderer.strokeColor.set(0, 0, 0, 0.9);
      else geomRenderer.strokeColor.set(1, 1, 1, 0.9);


      if(waveForm.size() != screenW) {
        waveForm.assign(screenW, Vec2d::Zero);
      }

      synth->delayLine.getWaveForm(waveForm, waveFormLowestFrequency, waveFormFeedback, 1.0/50.0, screenH*0.2 * waveformScaling);
      
      double absMaxVal = 0;
      for(int i=0; i<waveForm.size(); i++) {
        absMaxVal = max(abs(waveForm[i].x), absMaxVal);
      }
      
      double scale = pow(absMaxVal / (screenH*0.2 * waveformScaling), 1.0/5.0) * (screenH*0.2 * waveformScaling) / absMaxVal;
      // tavoitteena siis saada aaltomuoto näkyviin hiljaisillakin äänillä. Noin.
      
      for(int i=0; i<screenW; i++) {
        if(i > 0) {
          if(waveformMode == RightWaveform || waveformMode == BothWaveforms) {
            geomRenderer.strokeColor.set(0.5, 0.5, 0.5, 0.9);
            geomRenderer.drawLine(i-1, screenH*0.2-waveForm[i-1].y*scale, i, screenH*0.2-waveForm[i].y*scale);
          }
          if(waveformMode == LeftWaveform || waveformMode == BothWaveforms) {
            if(isDarkGraphColors) geomRenderer.strokeColor.set(0, 0, 0, 0.9);
            else geomRenderer.strokeColor.set(1, 1, 1, 0.9);
            geomRenderer.drawLine(i-1, screenH*0.2-waveForm[i-1].x*scale, i, screenH*0.2-waveForm[i].x*scale);
          }
        }
      }

    }

    {
      if(envelopeBuffer.size() != screenW) {
        envelopeBuffer.resize(screenW);
        envelopeBufferPosition = 0;
      }
      int t = (int)(dt * synth->delayLine.sampleRate);
      double maxVal = -1e10, minVal = 1e10;
      for(int i=0; i<t; i++) {
         double v = synth->synthStereoPastByIndex(i).x;
         if(v > maxVal) maxVal = v;
         if(v < minVal) minVal = v;
      }
      envelopeBuffer[envelopeBufferPosition].set(minVal, maxVal);
      envelopeBufferPosition = (envelopeBufferPosition+1) % envelopeBuffer.size();

      if(showEnvelope) {
        if(isDarkGraphColors) geomRenderer.strokeColor.set(0, 0, 0, 0.4);
        else geomRenderer.strokeColor.set(1, 1, 1, 0.4);
        for(int i=0; i<envelopeBuffer.size(); i++) {
          Vec2d p = envelopeBuffer[(envelopeBufferPosition+i) % envelopeBuffer.size()];
          geomRenderer.drawLine(i, screenH*0.6+p.x * screenH*0.2 * envelopeScaling, i, screenH*0.6+p.y * screenH*0.2 * envelopeScaling);
        }
      }
    }
    
    geomRenderer.endRendering();

    
    rotatingRectVisualization.update(geomRenderer, spectrumGraph, screenW, screenH);
    

    if(synthLaunchPad) {
      synthLaunchPad->render(geomRenderer, screenW, screenH, time);
    }

    if(spectrumNoteInfoMode > 0) {
      //printf("#####\n");
      for(auto &stringNoteInfoPair : retainedSpectrumNotes) {
        if(time - stringNoteInfoPair.second.time <= 5.0) {
          peakNotes.push_back(stringNoteInfoPair.second);
        }
      }
      for(int i=0; i<peakNotes.size(); i++) {
        char buf[128], buf2[128];
        int levels = 12;
        if(spectrumNoteInfoMode == 1) {
          std::sprintf(buf, "%s", peakNotes[i].noteName.c_str());
        }
        if(spectrumNoteInfoMode == 2) {
          std::sprintf(buf, "%s%d", peakNotes[i].noteName.c_str(), peakNotes[i].octave);
        }
        if(spectrumNoteInfoMode == 3) {
          std::sprintf(buf, "%s%d%s%d", peakNotes[i].noteName.c_str(), peakNotes[i].octave,
                                                     peakNotes[i].centsDeviation >= 0 ? "+" : "-", abs(peakNotes[i].centsDeviation));
        }
        if(spectrumNoteInfoMode == 4) {
          std::sprintf(buf, "%s%d%s%d %.2f Hz", peakNotes[i].noteName.c_str(), peakNotes[i].octave,
                                                     peakNotes[i].centsDeviation >= 0 ? "+" : "-", abs(peakNotes[i].centsDeviation),
                                                     peakNotes[i].frequency);
        }
        if(roundSpectrumNoteLocation) {
          std::sprintf(buf2, "%s%d", peakNotes[i].noteName.c_str(), peakNotes[i].octave);
        }
        Vec2d size = textRenderer.getDimensions(roundSpectrumNoteLocation ? buf2 : buf, 12);

        double x, y = screenH - 0.85 * size.y * (peakNotes[i].roundedPitch % levels) - 200;
        //double x, y = screenH - size.y - 150;
        if(spectrumAsNotes) {
          double t = roundSpectrumNoteLocation ? peakNotes[i].roundedPitch : peakNotes[i].absolutePitch;
          x = map(t, freqToNote(spectrumFrequencyLimits.x), freqToNote(spectrumFrequencyLimits.y), 0, screenW-1) - size.x/2;
        }
        else {
          double t = roundSpectrumNoteLocation ? noteToFreq(peakNotes[i].roundedPitch) : peakNotes[i].frequency;
          x = map(t, spectrumFrequencyLimits.x, spectrumFrequencyLimits.y, 0, screenW-1) - size.x/2;
        }

        textRenderer.setColor(1, 1, 1, exp(3.0*(peakNotes[i].time - time)));
        textRenderer.print(buf, x, y, 12);
      }
    }
    textRenderer.setColor(1, 1, 1, 1);

    if(synth->recordNoteTrackActive) {
      geomRenderer.texture = NULL;
      if(isDarkGraphColors) {
        geomRenderer.fillColor.set(0, 0, 0, 0.2);
        geomRenderer.strokeColor.set(0, 0, 0, 1);
      }
      else {
        geomRenderer.fillColor.set(1, 1, 1, 0.44);
        geomRenderer.strokeColor.set(1, 1, 1, 1);
      }
      geomRenderer.strokeType = 1;
      geomRenderer.strokeWidth = 1;
      double w = min(250, screenW * 0.8);
      double h = 30;
      double x = screenW - w - 20;
      double y = screenH - h - 20;
      geomRenderer.drawRectCorner(w, h, x, y);
      
      if(isDarkGraphColors) {
        geomRenderer.fillColor.set(0, 0, 0, 0.85);
        geomRenderer.strokeColor.set(0, 0, 0, 1);
      }
      else {
        geomRenderer.fillColor.set(1, 1, 1, 0.7);
        geomRenderer.strokeColor.set(1, 1, 1, 1);
      }
      geomRenderer.strokeType = 0;
      geomRenderer.strokeWidth = 0;
      geomRenderer.drawRectCorner(w * synth->trackPreSamplingProgress, h, x, y);
      if(isDarkGraphColors) {
        textRenderer.setColor(1, 1, 1, 0.75);
      }
      else {
        textRenderer.setColor(0, 0, 0, 0.75);
      }
      textRenderer.print(std::to_string((int)(synth->trackPreSamplingProgress*100.0)) + " %", x + w*0.5 - 18, y + 3, 14);
    }

    if(synth->recordingAudio) {
      geomRenderer.texture = NULL;
      geomRenderer.fillColor.set(0.6, 0, 0, 0.5);
      geomRenderer.strokeColor.set(0.6, 0, 0, 0.5);
      geomRenderer.strokeType = 0;
      geomRenderer.strokeWidth = 1;
      geomRenderer.drawRect(screenW, 8, screenW*0.5, 2);
      geomRenderer.drawRect(screenW, 8, screenW*0.5, screenH-3);
      geomRenderer.drawRect(8, screenH, 2, screenH*0.5);
      geomRenderer.drawRect(8, screenH, screenW-3, screenH*0.5);
    }



    if(console.inputGrabbed) {
      console.render(sdlInterface);
    }



  }

};
