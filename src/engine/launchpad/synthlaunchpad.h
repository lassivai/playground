#pragma once
#include "launchpad.h"
#include "modes/minesweeper_lpm.h"
#include "modes/spectrum_lpm.h"
#include "modes/sequencer_lpm.h"
#include "modes/equalizer_lpm.h"
#include "modes/recordingtrack_lpm.h"
#include "../synth/synth.h"
#include "../gui/gui.h"
#include <vector>

/*
 * Minesweeper corner not working
 */

struct SynthLaunchPad : public LaunchPadInterface {

   bool debugRenderPad = false;

   // TODO get rid of these
   MineSweeperLaunchpadMode *mineSweeperLaunchpadMode = NULL;
   SpectrumLaunchpadMode *spectrumLaunchpadMode = NULL;
   SequencerLaunchpadMode *sequencerLaunchpadMode = NULL;
   
   std::vector<LaunchpadMode*> launchPadModes;
   int outputMode = 3;
   int inputMode = 0;

   const std::vector<std::string> padAssignments = { "Set control mode - / spect / mines / seq / eq / rec, (64, 79) + [n]" };

   virtual void printControls() {
     for(int i=0; i<padAssignments.size(); i++) {
       messageQueue.addMessage(to_wstring(padAssignments[i]));
     }
     launchPadModes[outputMode]->printControls();
   }

   void render(GeomRenderer &geomRenderer, double w, double h, double time) {
     if(debugRenderPad && midiPortIndex >= 0) {
       double t = min(min(w, h) * 0.9, 500);
       renderPad(geomRenderer, w*0.5, h*0.5, t, t, time);
     }
   }

   void update(double time, double dt) {
     //readMidiMessages();
     if(midiPortIndex < 0) return;
     launchPadModes[outputMode]->onUpdateGrid(time, dt);

     updatePad();
   }
   
   
   void onAction(int x, int y, KeyState keyState) {
     if(x >= 0 && x < 8 && y >= 0 && y < 8 && keyState == LaunchPadInterface::KeyState::PressedActive) {
       if(arePadsPressedAndCancelRelease(64, 79)) {
         int i = x + y*8;
         if(i < launchPadModes.size()) {
           outputMode = i;
           return;
         }
         padKeyStates[x+y*8] = LaunchPadInterface::KeyState::ReleasedInactive;
       }
     }
     launchPadModes[outputMode]->onAction(x, y, keyState);
   }
   
   Panel *panel = NULL;
   ListBox *inputModeGui = NULL;
   ListBox *outputModeGui = NULL;
   NumberBox *spectrumScalingGui = NULL;
   CheckBox *debugRenderPadGui = NULL;

   NumberBox *numberBlinkTimeGui = NULL;
   NumberBox *maxNumberBlinkedGui = NULL;

   void init(GuiElement *parentGuiElement, MidiInterface *midiInterface, std::vector<double> *spectrum, Synth *synth) {
     LaunchPadInterface::init(midiInterface);
     //resetMineSweeper();
     
     launchPadModes.push_back(new LaunchpadMode(this, "None"));
     launchPadModes.push_back(spectrumLaunchpadMode = new SpectrumLaunchpadMode(this, spectrum));
     launchPadModes.push_back(mineSweeperLaunchpadMode = new MineSweeperLaunchpadMode(this));
     launchPadModes.push_back(sequencerLaunchpadMode = new SequencerLaunchpadMode(this, synth, panel));
     launchPadModes.push_back(new EqualizerLaunchpadMode(this, synth->equalizer));
     launchPadModes.push_back(new RecordingTrackLaunchpadMode(this, synth));
     

     setOverrideKeystates(true, false, true, false);

     panel = new Panel("Synth Launchpad panel", 0, 0, 1, 1);
     parentGuiElement->addChildElement(panel);

     double line = -13, lineHeight = 23;
     /*inputModeGui = new ListBox("Input mode", 10, line+=lineHeight, 12);
     for(int i=0; i<launchPadModes.size(); i++) {
       inputModeGui->addItems(launchPadModes[i]->name);
     }
     inputModeGui->setValue(inputMode);
     panel->addChildElement(inputModeGui);*/

     outputModeGui = new ListBox("Mode", 10, line+=lineHeight, 12);
     for(int i=0; i<launchPadModes.size(); i++) {
       outputModeGui->addItems(launchPadModes[i]->name);
     }
     outputModeGui->setValue(outputMode);
     panel->addChildElement(outputModeGui);
     
     debugRenderPadGui = new CheckBox("Debug render", debugRenderPad, 10, line+=lineHeight);
     panel->addChildElement(debugRenderPadGui);

    spectrumScalingGui = new NumberBox("Spectrum scaling", spectrumLaunchpadMode->spectrumScaling, NumberBox::FLOATING_POINT, -1e10, 1e10, 10, line+=lineHeight, 10);
    panel->addChildElement(spectrumScalingGui);

    numberBlinkTimeGui = new NumberBox("Blink time", mineSweeperLaunchpadMode->numberBlinkTime, NumberBox::FLOATING_POINT, 0, 1e10, 10, line+=lineHeight, 10);
    panel->addChildElement(numberBlinkTimeGui);
    
    maxNumberBlinkedGui = new NumberBox("Max blinked number", mineSweeperLaunchpadMode->maxNumberBlinked, NumberBox::INTEGER, 1, 1<<30, 10, line+=lineHeight, 9);
    maxNumberBlinkedGui->incrementMode = NumberBox::IncrementMode::Linear;
    panel->addChildElement(maxNumberBlinkedGui);

     panel->setSize(320, line + lineHeight + 10);
     panel->setPosition(250, 10);
     panel->setVisible(false);
     panel->addGuiEventListener(new LaunchPadPanelListener(this));
   }
   
   struct LaunchPadPanelListener : public GuiEventListener {
     SynthLaunchPad *synthLaunchPad = NULL;
     LaunchPadPanelListener(SynthLaunchPad *synthLaunchPad) {
       this->synthLaunchPad = synthLaunchPad;
     }
     void onValueChange(GuiElement *guiElement) {
       if(guiElement == synthLaunchPad->inputModeGui) {
         synthLaunchPad->launchPadModes[synthLaunchPad->inputMode]->onInputActivated(false);
         guiElement->getValue((void*)&synthLaunchPad->inputMode);
         synthLaunchPad->launchPadModes[synthLaunchPad->inputMode]->onInputActivated(true);
       }
       if(guiElement == synthLaunchPad->outputModeGui) {
         synthLaunchPad->launchPadModes[synthLaunchPad->outputMode]->onOutputActivated(false);
         guiElement->getValue((void*)&synthLaunchPad->outputMode);
         synthLaunchPad->launchPadModes[synthLaunchPad->outputMode]->onOutputActivated(true);
       }
       if(guiElement == synthLaunchPad->debugRenderPadGui) {
         guiElement->getValue((void*)&synthLaunchPad->debugRenderPad);
       }
       if(guiElement == synthLaunchPad->spectrumScalingGui) {
         guiElement->getValue((void*)&synthLaunchPad->spectrumLaunchpadMode->spectrumScaling);
       }
       if(guiElement == synthLaunchPad->numberBlinkTimeGui) {
         guiElement->getValue((void*)&synthLaunchPad->mineSweeperLaunchpadMode->numberBlinkTime);
       }
       if(guiElement == synthLaunchPad->maxNumberBlinkedGui) {
         guiElement->getValue((void*)&synthLaunchPad->mineSweeperLaunchpadMode->maxNumberBlinked);
       }
     }
   };
};