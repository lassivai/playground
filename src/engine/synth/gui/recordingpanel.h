#pragma once
#include "../synth.h"


struct RecordingPanel : public Panel
{
  Panel *recordingPanel = NULL;
  CheckBox *recordingAudioGui = NULL;
  CheckBox *playRecordedAudioGui = NULL;
  NumberBox *recordingMaxLengthGui = NULL;
  CheckBox *normalizeRecordingGui = NULL;
  TextBox *audioRecordingFilenameGui = NULL;
  Label *recordingProgressionGui = NULL;

  Synth *synth = NULL;
  GuiElement *parentGuiElement = NULL;

  RecordingPanel(Synth *synth, GuiElement *parentGuiElement) : Panel("Recording panel") {
    init(synth, parentGuiElement);
  }

  void init(Synth *synth, GuiElement *parentGuiElement) {
    this->addGuiEventListener(new RecordingPanelListener(this));
    this->synth = synth;
    
    parentGuiElement->addChildElement(this);

    double line = 10, lineHeight = 23;

    recordingAudioGui = new CheckBox("Record", synth->recordingAudio, 10, line);
    playRecordedAudioGui = new CheckBox("Play recording", synth->isPlayingRecordedAudio, 10, line+=lineHeight);

    char buf[128];
    std::sprintf(buf, "Time 0.00/%.2f", synth->audioRecordingMaxLengthInSeconds);
    recordingProgressionGui = new Label(buf, 10, line+=lineHeight);

    recordingMaxLengthGui = new NumberBox("Max length (s)", synth->audioRecordingMaxLengthInSeconds, NumberBox::FLOATING_POINT, 0.01, 20 * 60, 10, line+=lineHeight);
    recordingMaxLengthGui->incrementMode = NumberBox::IncrementMode::Linear;

    normalizeRecordingGui = new CheckBox("Normalize recording", synth->audioRecording.normalize, 10, line+=lineHeight);

    audioRecordingFilenameGui = new TextBox("Save filename", "unnamed", 10, line+=lineHeight);

    this->addChildElement(recordingAudioGui);
    this->addChildElement(playRecordedAudioGui);
    this->addChildElement(recordingProgressionGui);
    this->addChildElement(recordingMaxLengthGui);
    this->addChildElement(normalizeRecordingGui);
    this->addChildElement(audioRecordingFilenameGui);

    this->setSize(350, line + lineHeight + 10);
  }

  void onUpdate() {
    char buf[128];
    std::sprintf(buf, "Time %.2f/%.2f s", (double)synth->audioRecording.seeker/synth->audioRecording.sampleRate,
                                          (double)synth->audioRecording.samples.size()/synth->audioRecording.sampleRate);
    recordingProgressionGui->setText(buf);
    recordingAudioGui->setValue(synth->recordingAudio);
  }


  struct RecordingPanelListener : public GuiEventListener {
    RecordingPanel *recordingPanel;
    RecordingPanelListener(RecordingPanel *recordingPanel) {
      this->recordingPanel = recordingPanel;
    }
    void onValueChange(GuiElement *guiElement) {
      if(guiElement == recordingPanel->recordingAudioGui) {
        bool value;
        guiElement->getValue((void*)&value);
        if(value == true) {
          recordingPanel->synth->startAudioRecording();
        }
        else {
          recordingPanel->synth->endAudioRecording();
        }
      }
      if(guiElement == recordingPanel->playRecordedAudioGui) {
        bool value;
        guiElement->getValue((void*)&value);
        if(value == true) {
          recordingPanel->synth->playRecordedAudio();
        }
        else {
          recordingPanel->synth->stopRecordedAudio();
        }
      }
      if(guiElement == recordingPanel->recordingMaxLengthGui) {
        guiElement->getValue((void*)&recordingPanel->synth->audioRecordingMaxLengthInSeconds);
      }
      if(guiElement == recordingPanel->normalizeRecordingGui) {
        guiElement->getValue((void*)&recordingPanel->synth->audioRecording.normalize);
      }
    }

    void onAction(GuiElement *guiElement) {
      if(guiElement == recordingPanel->audioRecordingFilenameGui) {
        std::string filename;
        guiElement->getValue((void*)&filename);
        if(filename.size() > 0) {
          recordingPanel->synth->saveAudioRecordingToFile(filename);
        }
      }
    }
  };

};