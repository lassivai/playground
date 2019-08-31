#pragma once
#include "synthpanel.h"
#include "infopanel.h"
#include "looperpanel.h"
#include "instrumenttrackpanel.h"
//#include "effecttrackpanel.h"
#include "recordingpanel.h"
#include "audiotrackpanel.h"
#include "../../rtmidi_init.h"

#include <queue>

/*
 I have a problem that notes are not always stopped. Either:
   1) midi keyboard doesn't send the note off message after note on message (this would be very hard to fix)
   2) the note off message is sent so soon after note on, that my program isn't ready to recognise it (this would be easily fixed)
   3) some other reason why my program doesn't respond to the note off message (fixable, but needs some debugging)

Need to do some sort of debugging view for those note on and note off messages...

*/

struct CurrentlyPlayingNotePanel : public Panel {
  
  struct NoteRect : public Rect {
    bool isMouseHovering = false;
    Vec2d hoverPos;
  };
  
  Synth *synth = NULL;
  
  std::vector<NoteRect> liveNoteRects, recordedNoteRects;
  
  enum Mode { ShowLiveAndRecordedNotes, ShowOnlyLiveNotes };
  Mode mode = ShowOnlyLiveNotes;
  
  CurrentlyPlayingNotePanel(Synth *synth) : Panel("Currently playing note panel") {
    init(synth);
  }
  
  void init(Synth *synth) {
    this->synth = synth;
    setSize(40, 200);
  }

  void renderNoteInfo(Vec2d pos, Note &note, GeomRenderer &geomRenderer, TextGl &textRenderer) {
    geomRenderer.texture = NULL;
    geomRenderer.strokeType = 1;
    geomRenderer.strokeWidth = 1;
    geomRenderer.strokeColor.set(1, 1, 1, 0.5);
    geomRenderer.fillColor.set(0, 0, 0, 0.75);
    
    std::string noteStr = format("instrument %s\npitch %.4f\nfrequency %.4f\nvolume %.4f",
                                  note.instrumentName.c_str(), note.pitch, note.frequency, note.volume);
    
    Vec2d noteStrSize = textRenderer.getDimensions(noteStr, 10);

    Vec2d size = noteStrSize + Vec2d(10, 10);
    
    if(pos.x+15 + size.x >= geomRenderer.screenW) {
      pos.x = pos.x - 30 - size.x;
    }
    
    geomRenderer.drawRectCorner(size.x, size.y, pos.x+15, pos.y);
    textRenderer.setColor(1, 1, 1, 0.9);
    textRenderer.print(noteStr, pos.x+3+15, pos.y+5, 10);
  }

  

  virtual void onRender(GeomRenderer &geomRenderer, TextGl &textRenderer) {
    //Panel::onPrerender(geomRenderer, textRenderer, displacement);
    if(!isVisible) return;
    
    liveNoteRects.resize(synth->numCurrentlyPlayingNotes);
    recordedNoteRects.resize(synth->numCurrentlyPlayingNotesPresampled);
    
    int recordedNotesPlaying = 0;
    for(int i=0; i<synth->numCurrentlyPlayingNotesPresampled; i++) {
      if(synth->currentlyPlayingNotesPresampled[i].volume > 0) {
        recordedNotesPlaying++;
      }
    }
    bool showRecordedNotes = mode == ShowLiveAndRecordedNotes || recordedNotesPlaying > 0|| synth->isLooperPlaying;
    
    double width = 44;
    double itemHeight = 8;
    if(showRecordedNotes) {
      setSize(width, itemHeight * (synth->numCurrentlyPlayingNotes + synth->numCurrentlyPlayingNotesPresampled) + 11);
    }
    else {
      setSize(width, itemHeight * (synth->numCurrentlyPlayingNotes) + 5);
    }
    geomRenderer.texture = NULL;
    geomRenderer.strokeType = 1;
    geomRenderer.strokeWidth = 1;
    geomRenderer.strokeColor.set(1, 1, 1, 1);
    geomRenderer.fillColor.set(1, 1, 1, 0.2);
    
    double time = synth->getPaTime();
    double hx = 4;
    double padding = 4;
    for(int i=0; i<synth->numCurrentlyPlayingNotes; i++) {
      liveNoteRects[i].set(width - padding*2, itemHeight-3, absolutePos.x+width/2, hx+absolutePos.y+itemHeight*i+(itemHeight-3)*0.5);
      
      if(synth->currentlyPlayingNotes[i].volume > 0) {
        double progress;
        if(synth->currentlyPlayingNotes[i].isHolding) {
          geomRenderer.fillColor.set(0, 0, 0, 0.7);
          progress = clamp((time-synth->currentlyPlayingNotes[i].startTime) / synth->currentlyPlayingNotes[i].noteFullLengthSecs, 0, 1);
        }
        else {
          geomRenderer.fillColor.set(0, 0, 0, 0.7);
          progress = clamp((time-synth->currentlyPlayingNotes[i].startTime) / synth->currentlyPlayingNotes[i].noteActualLength, 0, 1);
        }
        geomRenderer.strokeColor.set(1, 1, 1, 1);
        
        geomRenderer.drawRect(liveNoteRects[i].w, liveNoteRects[i].h, liveNoteRects[i].pos.x, liveNoteRects[i].pos.y);
        
        geomRenderer.fillColor.set(1, 1, 1, 0.88);
        geomRenderer.strokeType = 0;
        geomRenderer.drawRectCorner(liveNoteRects[i].w*progress, liveNoteRects[i].h, liveNoteRects[i].pos.x-liveNoteRects[i].w*0.5, liveNoteRects[i].pos.y-liveNoteRects[i].h*0.5);
        geomRenderer.strokeType = 1;
      }
      else {
        geomRenderer.strokeColor.set(0.4, 0.4, 0.4, 0.7);
        geomRenderer.fillColor.set(0.2, 0.2, 0.2, 0.7);
        geomRenderer.drawRect(liveNoteRects[i].w, liveNoteRects[i].h, liveNoteRects[i].pos.x, liveNoteRects[i].pos.y);
      }
    }
    //geomRenderer.strokeWidth = 1.5; // FIXME something's wrong with the stroke width

    if(showRecordedNotes) {
      hx = 4+4+2+itemHeight * synth->numCurrentlyPlayingNotes;
      
      for(int i=0; i<synth->numCurrentlyPlayingNotesPresampled; i++) {
        recordedNoteRects[i].set(width - padding*2, itemHeight-3, absolutePos.x+width/2, hx+absolutePos.y+itemHeight*i+(itemHeight-3)*0.5);
        if(synth->currentlyPlayingNotesPresampled[i].volume > 0) {
          double progress = clamp((time-synth->currentlyPlayingNotesPresampled[i].startTime) / synth->currentlyPlayingNotesPresampled[i].noteFullLengthSecs, 0, 1);
          if(time-synth->currentlyPlayingNotesPresampled[i].startTime < synth->currentlyPlayingNotesPresampled[i].noteLength) {
            geomRenderer.fillColor.set(0, 0, 0, 0.7);
          }
          else {
            geomRenderer.fillColor.set(0, 0, 0, 0.7);
          }
          geomRenderer.strokeColor.set(1, 1, 1, 1);
          geomRenderer.drawRect(recordedNoteRects[i].w, recordedNoteRects[i].h, recordedNoteRects[i].pos.x, recordedNoteRects[i].pos.y);
          
          geomRenderer.fillColor.set(1, 1, 1, 0.88);
          geomRenderer.strokeType = 0;
          geomRenderer.drawRectCorner(recordedNoteRects[i].w*progress, recordedNoteRects[i].h, recordedNoteRects[i].pos.x-recordedNoteRects[i].w*0.5, recordedNoteRects[i].pos.y-recordedNoteRects[i].h*0.5);
          geomRenderer.strokeType = 1;
        }
        else {
          geomRenderer.strokeColor.set(0.4, 0.4, 0.4, 0.7);
          geomRenderer.fillColor.set(0.2, 0.2, 0.2, 0.7);
          geomRenderer.drawRect(recordedNoteRects[i].w, recordedNoteRects[i].h, recordedNoteRects[i].pos.x, recordedNoteRects[i].pos.y);
        }
      }
    }
    if(mouseHovering) {
      for(int i=0; i<min(synth->currentlyPlayingNotes.size(), liveNoteRects.size()); i++) {
        if(liveNoteRects[i].isMouseHovering) {
          renderNoteInfo(liveNoteRects[i].hoverPos, synth->currentlyPlayingNotes[i], geomRenderer, textRenderer);
        }
      }
      if(mode == ShowLiveAndRecordedNotes) {
        for(int i=0; i<min(synth->currentlyPlayingNotesPresampled.size(), recordedNoteRects.size()); i++) {
          if(recordedNoteRects[i].isMouseHovering) {
            renderNoteInfo(recordedNoteRects[i].hoverPos, synth->currentlyPlayingNotesPresampled[i], geomRenderer, textRenderer);
          }
        }
      }
    }
  }
  bool mouseHovering = false;
  
  virtual void onMousePressed(Events &events) {
    Panel::onMousePressed(events);
    if(isPointWithin(events.m) && events.mouseButton == 1) {
      mode = (Mode)(1 - mode);
    }
  }
  
  virtual void onMouseMotion(Events &events) {
    Panel::onMouseMotion(events);
    if(!(mouseHovering = isPointWithin(events.m))) return;

    for(int i=0; i<min(synth->currentlyPlayingNotes.size(), liveNoteRects.size()); i++) {
      if(liveNoteRects[i].isPointWithin(events.m) && synth->currentlyPlayingNotes[i].volume > 0) {
        liveNoteRects[i].isMouseHovering = true;
        liveNoteRects[i].hoverPos = events.m;
      }
      else {
        liveNoteRects[i].isMouseHovering = false;
      }
    }
    for(int i=0; i<min(synth->currentlyPlayingNotesPresampled.size(), recordedNoteRects.size()); i++) {
      if(recordedNoteRects[i].isPointWithin(events.m) && synth->currentlyPlayingNotesPresampled[i].volume > 0) {
        recordedNoteRects[i].isMouseHovering = true;
        recordedNoteRects[i].hoverPos = events.m;
      }
      else {
        recordedNoteRects[i].isMouseHovering = false;
      }
    }
  }
};



struct NoteMidiEventPanel : public Panel {
  
  struct NoteEventInfo {
    enum Type {Irrelevant, NoteOn, NoteOff};
    const std::vector<std::string> typeNames = {"Irrelevant", "NoteOn", "NoteOff"};
    
    NoteEventInfo() {}
    NoteEventInfo(Type type, double time, MidiMessage midiMessage) {
      this->type = type;
      this->time = time;
      this->midiMessage = midiMessage;
    }
    
    Type type = Irrelevant;
    double time;
    MidiMessage midiMessage;
    Rect rect;
    bool isMouseHovering = false;
    Vec2d hoverPos;
    
    void renderInfo(GeomRenderer &geomRenderer, TextGl &textRenderer) {
      if(isMouseHovering) {
        geomRenderer.texture = NULL;
        geomRenderer.strokeType = 1;
        geomRenderer.strokeWidth = 1;
        geomRenderer.strokeColor.set(1, 1, 1, 0.5);
        geomRenderer.fillColor.set(0, 0, 0, 0.75);
        std::string eventStr = format("type %s\ntime %.4f\nstatus high %d\nstatus low %d\ndata 1 %d\ndata 2 %d\nstamp %.4f\nport %d",
                                      typeNames[type].c_str(), time, midiMessage.statusByte >> 4, midiMessage.statusByte & 15, midiMessage.dataByte1, midiMessage.dataByte2, midiMessage.stamp, midiMessage.midiPortIndex);
        Vec2d eventStrSize = textRenderer.getDimensions(eventStr, 10);
        Vec2d size = eventStrSize + Vec2d(10, 10);
        geomRenderer.drawRectCorner(size.x, size.y, hoverPos.x+15, hoverPos.y);
        textRenderer.setColor(1, 1, 1, 0.9);
        textRenderer.print(eventStr, hoverPos.x+3+15, hoverPos.y+5, 10);
      }      
    }
  };
  
  struct PitchInfo {
    std::vector<NoteEventInfo> noteEvents;
    long numNotesOn = 0, numNotesOff = 0;
  };
  
  std::vector<PitchInfo> pitchNoteEvents;
  MidiInterface *midiInterface = NULL;
  bool mouseHovering = false;
  
  
  NoteMidiEventPanel(MidiInterface *midiInterface) : Panel("Note midi event panel") {
    pitchNoteEvents.resize(128);
    
    this->midiInterface = midiInterface;
    setSize(200, 23+6);
  }
  
  void update(const MidiMessage &midiMessage) {
    if(midiMessage.getMessageType() == MidiMessage::NoteOn && midiMessage.dataByte2 > 0) {
      unsigned char pitch = midiMessage.dataByte1;
      pitchNoteEvents[pitch].noteEvents.push_back(NoteEventInfo(NoteEventInfo::Type::NoteOn, SDL_GetTicks()*0.001, midiMessage));
      pitchNoteEvents[pitch].numNotesOn++;
    }
    else if(midiMessage.getMessageType() == MidiMessage::NoteOff || (midiMessage.getMessageType() == MidiMessage::NoteOn && midiMessage.dataByte2 == 0)) {
      unsigned char pitch = midiMessage.dataByte1;
      pitchNoteEvents[pitch].noteEvents.push_back(NoteEventInfo(NoteEventInfo::Type::NoteOff, SDL_GetTicks()*0.001, midiMessage));
      pitchNoteEvents[pitch].numNotesOff++;
    }
    prerenderingNeeded = true;
  }
  
  virtual void onMouseMotion(Events &events) {
    Panel::onMouseMotion(events);
      if((mouseHovering = isPointWithin(events.m))) {
      
      for(int i=0; i<128; i++) {
        for(int k=0; k<pitchNoteEvents[i].noteEvents.size(); k++) {
          pitchNoteEvents[i].noteEvents[k].isMouseHovering = pitchNoteEvents[i].noteEvents[k].rect.isPointWithin(events.m);
          pitchNoteEvents[i].noteEvents[k].hoverPos = events.m;
        }
      }
    }
  }
  
  virtual void onRender(GeomRenderer &geomRenderer, TextGl &textRenderer) {
    Panel::onRender(geomRenderer, textRenderer);
    if(mouseHovering) {
      for(int i=0; i<128; i++) {
        for(int k=0; k<pitchNoteEvents[i].noteEvents.size(); k++) {
          pitchNoteEvents[i].noteEvents[k].renderInfo(geomRenderer, textRenderer);
        }
      }
    }
  }
  virtual void prepare(GeomRenderer &geomRenderer, TextGl &textRenderer) {
    Panel::prepare(geomRenderer, textRenderer);
    
    double itemHeight = 18;
    Vec2d boxSize(5, 10);
    double gap = 3;
    int numPitchesActive = 0;
    int maxEvents = 0;
    for(int i=0; i<128; i++) {
      if(pitchNoteEvents[i].noteEvents.size() > 0) {
        numPitchesActive++;
        maxEvents = max(maxEvents, pitchNoteEvents[i].noteEvents.size());
      }
    }
  
    Vec2d dim = textRenderer.getDimensions("pitch 127, xxxx/xxxx, xxxx/xxxx ", 10);
    setSize(dim.x + (boxSize.x+gap) * maxEvents + 10, itemHeight * numPitchesActive + 10);
  }
  
  virtual void onPrerender(GeomRenderer &geomRenderer, TextGl &textRenderer, const Vec2d &displacement) {
    
    double itemHeight = 18;
    Vec2d boxSize(5, 10);
    double gap = 3;
    /*int numPitchesActive = 0;
    int maxEvents = 0;
    for(int i=0; i<128; i++) {
      if(pitchNoteEvents[i].noteEvents.size() > 0) {
        numPitchesActive++;
        maxEvents = max(maxEvents, pitchNoteEvents[i].noteEvents.size());
      }
    }*/
    Vec2d dim = textRenderer.getDimensions("pitch 127, xxxx/xxxx, xxxx/xxxx ", 10);
    
    geomRenderer.texture = NULL;
    geomRenderer.strokeType = 1;
    geomRenderer.strokeWidth = 1;
    geomRenderer.strokeColor.set(1, 1, 1, 0.5);
    geomRenderer.fillColor.set(0, 0, 0, 0.75);
    

    
    Panel::onPrerender(geomRenderer, textRenderer, displacement);
    int j = 0;
    for(int i=0; i<128; i++) {
      if(pitchNoteEvents[i].noteEvents.size() > 0) {
        textRenderer.setColor(1, 1, 1, 0.9);
        long numOn = 0, numOff = 0;
        for(int q=0; q<midiInterface->midiPorts.size(); q++) {
          numOn += midiInterface->midiPorts[q]->numNotesOn[i];
          numOff += midiInterface->midiPorts[q]->numNotesOff[i];
        }
        textRenderer.print("pitch "+std::to_string(i) + ", " + std::to_string(numOn) + "/" + std::to_string(numOff) + ", " + std::to_string(pitchNoteEvents[i].numNotesOn) + "/" + std::to_string(pitchNoteEvents[i].numNotesOff), displacement.x + 5, displacement.y + 5+j*itemHeight, 10);
        for(int k=0; k<pitchNoteEvents[i].noteEvents.size(); k++) {
          if(pitchNoteEvents[i].noteEvents[k].type == NoteEventInfo::NoteOn) {
            geomRenderer.fillColor.set(0.1, 0.2, 1.0, 0.6);
          }
          else if(pitchNoteEvents[i].noteEvents[k].type == NoteEventInfo::NoteOff) {
            geomRenderer.fillColor.set(1.0, 0.2, 0.2, 0.6);
          }
          else {
            geomRenderer.fillColor.set(1.0, 1.0, 0, 0.6);
          }
          double w = boxSize.x, h = boxSize.y, x = dim.x+(gap+boxSize.x)*k, y = j*itemHeight+7;
          pitchNoteEvents[i].noteEvents[k].rect.set(w, h, absolutePos.x+x+w*0.5, absolutePos.y+y+w*0.5);
          geomRenderer.drawRectCorner(w, h, displacement.x+x, displacement.y+y);
        }
        j++;
      }
    }
  }

  void reset() {
    for(int i=0; i<128; i++) {
      pitchNoteEvents[i].noteEvents.clear();
    }
  }

  virtual void onKeyDown(Events &events) {
    Panel::onKeyDown(events);
    if(!isPointWithin(events.m)) return;
    if(events.sdlKeyCode == SDLK_r) {
      reset();
    }
  }
};


struct MidiEventPanel : public Panel {
  std::vector<std::string> previousMessages;
  const int maxNumMessagesInQueue = 100;
  int numMessagesShown = 10;
    
  NoteMidiEventPanel *noteMidiEventPanel = NULL;
    
  MidiEventPanel(MidiInterface *midiInterface) : Panel("Midi event panel") {
    init(midiInterface);
  }
  
  void update(const MidiMessage &midiMessage) {
    pushMessage(midiMessage.toString());
    if(noteMidiEventPanel) {
      noteMidiEventPanel->update(midiMessage);
    }
  }
  void update(const HuiMessage &huiMessage) {
    pushMessage(huiMessage.toString());
  }
  void update(const SysExMessage &sysExMessage) {
    pushMessage(sysExMessage.toString());
  }
  
  void pushMessage(const std::string &msgStr) {
    previousMessages.push_back(msgStr);
    if(previousMessages.size() > maxNumMessagesInQueue) {
      previousMessages.erase(previousMessages.begin());
    }
    prerenderingNeeded = true;
  }
  
  void init(MidiInterface *midiInterface) {
    setSize(200, 23+6);
    addChildElement(noteMidiEventPanel = new NoteMidiEventPanel(midiInterface));
    noteMidiEventPanel->setPosition(300, 0);
  }
  
  virtual void onPrerender(GeomRenderer &geomRenderer, TextGl &textRenderer, const Vec2d &displacement) {
    if(previousMessages.size() < 1) return;
    std::string strA = previousMessages[previousMessages.size()-1];
    Vec2d dimA = textRenderer.getDimensions(strA, 12);
    
    std::string str;
    int n = min(numMessagesShown-1, previousMessages.size()-1);
    for(int i=0; i<n; i++) {
      if(i > 0) str += "\n";
      str += previousMessages[previousMessages.size()-1-n+i];
    }
    Vec2d dim = textRenderer.getDimensions(str, 10);
    Vec2d dimB = textRenderer.getDimensions("abc", 10);
    
    setSize(max(size.x, max(dimA.x, dim.x) + 6), dimA.y + (dimB.y*(numMessagesShown-1))+6);
    
    Panel::onPrerender(geomRenderer, textRenderer, displacement);

    textRenderer.setColor(1, 1, 1, 0.66);
    textRenderer.print(str, displacement.x+3, displacement.y+3, 10);
    textRenderer.setColor(1, 1, 1, 1);
    textRenderer.print(strA, displacement.x+3, displacement.y+dim.y+3, 12);
    
    /*geomRenderer.fillColor.set(0.5, 0.5, 0.5, 0.5);
    geomRenderer.strokeColor.set(1, 1, 1, 0.5);
    geomRenderer.strokeWidth = 0;
    geomRenderer.strokeType = 1;
    geomRenderer.drawRect(size, displacement + size * 0.5);*/
  }
  
};


struct SynthGui
{
  struct SynthGuiMidiListener : public MidiMessageListener {
    SynthGui *synthGui;
    SynthGuiMidiListener(SynthGui *synthGui) {
      this->synthGui = synthGui;
    }
    
    std::vector<long> numNotesOn = std::vector<long>(128, 0), numNotesOff = std::vector<long>(128, 0);
    long numMsgs = 0;
    
    virtual void onMessage(const MidiMessage &midiMessage, MidiPortType midiPortType) {
      printf("SynthGuiMidiListener::onMessage(), beginning\n");
      if(midiMessage.getMessageType() == MidiMessage::NoteOn) {
        numNotesOn[midiMessage.dataByte1]++;
      }
      else if(midiMessage.getMessageType() == MidiMessage::NoteOff) {
        numNotesOff[midiMessage.dataByte1]++;
      }
      synthGui->onMidiMessage(midiMessage);
      
      /*for(int r=0; r<128; r++) {
        if(numNotesOn[r] > 0 || numNotesOff[r] > 0) {
          printf("synth gui, pitch %d, %lu/%lu\n", r, numNotesOn[r], numNotesOff[r]);
        }
      }*/
      numMsgs++;
      printf("SynthGuiMidiListener::onMessage(), end (total %lu)\n", numMsgs);
    }
    
  };
  SynthGuiMidiListener *synthGuiMidiListener = NULL;
  
  MidiMessageListener *getMidiMessageListener() {
    return synthGuiMidiListener ? synthGuiMidiListener : synthGuiMidiListener = new SynthGuiMidiListener(this);
  }
  
  Synth *synth = NULL;
  GuiElement *parentGuiElement = NULL;  

  SynthPanel *synthPanel = NULL;
  InfoPanel *infoPanel = NULL;
  InstrumentTrackPanel *instrumentTrackPanel = NULL;
  LooperPanel *looperPanel = NULL;
  RecordingPanel *recordingPanel = NULL;
  AudioRecordingTrackPanel *audioRecordingTrackPanel = NULL;
  //MidiEventPanel *midiEventPanel = NULL;
  CurrentlyPlayingNotePanel *currentlyPlayingNotePanel = NULL;

  void init(Synth *synth, MidiInterface *midiInterface, GuiElement *parentGuiElement, int screenW, int screenH) {
    this->synth = synth;
    this->parentGuiElement = parentGuiElement;
    
    instrumentTrackPanel = new InstrumentTrackPanel(synth, midiInterface, parentGuiElement);
    instrumentTrackPanel->setPosition(6, 6);
    instrumentTrackPanel->setVisible(true);

    synthPanel = new SynthPanel(synth, parentGuiElement); 
    synthPanel->setPosition(6, instrumentTrackPanel->pos.y + instrumentTrackPanel->size.y + 5);
    synthPanel->setVisible(false);
    
    
    looperPanel = new LooperPanel(synth, parentGuiElement);
    looperPanel->setPosition(150, 0);
    looperPanel->setVisible(false);

    recordingPanel = new RecordingPanel(synth, parentGuiElement);
    recordingPanel->setPosition(250, 90);
    recordingPanel->setVisible(false);
    
    parentGuiElement->addChildElement(infoPanel = new InfoPanel());
    infoPanel->setVisible(true);
    infoPanel->setPosition(screenW - infoPanel->size.x - 6, 6);
    
    parentGuiElement->addChildElement(synth->stereoOscilloscope.getPanel());
    synth->stereoOscilloscope.getPanel()->setVisible(true);
    synth->stereoOscilloscope.getPanel()->setPosition(screenW - synth->stereoOscilloscope.getPanel()->size.x - 6, infoPanel->pos.y + infoPanel->size.y + 6);
    
    audioRecordingTrackPanel = new AudioRecordingTrackPanel(synth, parentGuiElement);
    audioRecordingTrackPanel->setPosition(400, 6);
    audioRecordingTrackPanel->setVisible(false);
    
    /*parentGuiElement->addChildElement(midiEventPanel = new MidiEventPanel(midiInterface));
    midiEventPanel->setVisible(false);
    midiEventPanel->setPosition(300, 300);*/
    
    parentGuiElement->addChildElement(currentlyPlayingNotePanel = new CurrentlyPlayingNotePanel(synth));
    currentlyPlayingNotePanel->setVisible(true);
    currentlyPlayingNotePanel->setPosition(screenW - currentlyPlayingNotePanel->size.x - 6, synth->stereoOscilloscope.getPanel()->pos.y + synth->stereoOscilloscope.getPanel()->size.y + 6);
  }
  
  void onQuit() {
    /*if(infoPanel) {
      parentGuiElement->deleteChildElement(infoPanel)
    }*/
  }
  
  void onUpdate(double time, double dt) {
    if(infoPanel) {
      int numOscillators = synth->getInstrument()->getTotalOscillatorCount() * synth->numCurrentlyPlayingNotes;
      infoPanel->update(synth->getCpuLoad(), dt, numOscillators);
    }
    if(looperPanel) {
      looperPanel->onUpdate();
    }
    if(recordingPanel) {
      recordingPanel->onUpdate();
      //recordingPanel->recordingAudioGui->setValue(synth->recordingAudio);
    }
    if(audioRecordingTrackPanel) {
      audioRecordingTrackPanel->onUpdate(time, dt);
    }
    
    //synth->instruments[0]->toggleGuiVisibility(parentGuiElement);
    //synth->instruments[0]->isActive = true;
  }
  
  void onInstrumentListUpdated() {
    if(instrumentTrackPanel) {
      instrumentTrackPanel->update();
    }
  }
  
  void onKeyDown(Events &events) {
    if(events.sdlKeyCode == SDLK_F1 && events.numModifiersDown == 0) {
      if(synthPanel) {
        synthPanel->toggleVisibility();
      }
      /*if(synth->getInstrument()->instrumentPanel) {
        printf("(debugging) at SynthSketch::onKeyDown()... 3\n");
        synth->getInstrument()->removePanel(&guiRoot);
      }*/
      /*if(synth->getInstrument()->instrumentPanel) {
        synth->getInstrument()->addPanel(&guiRoot);
        synth->getInstrument()->getPanel()->setPosition(screenW - synth->getInstrument()->getPanel()->size.x - 10, 10);
      }*/
    }

    if(events.sdlKeyCode == SDLK_F1 && events.lControlDown) {
      if(infoPanel) {
        infoPanel->toggleVisibility();
      }
    }
    /*if(events.sdlKeyCode == SDLK_F1 && events.lShiftDown) {
      if(midiEventPanel) {
        midiEventPanel->toggleVisibility();
      }
    }*/
    if(events.sdlKeyCode == SDLK_F2 && events.lControlDown) {
      if(currentlyPlayingNotePanel) {
        currentlyPlayingNotePanel->toggleVisibility();
      }
    }
    if(events.sdlKeyCode == SDLK_F3 && events.numModifiersDown == 0) {
      if(instrumentTrackPanel) {
        instrumentTrackPanel->toggleVisibility();
      }
    }
    if(events.sdlKeyCode == SDLK_F4 && events.numModifiersDown == 0) {
      if(audioRecordingTrackPanel) {
        audioRecordingTrackPanel->toggleVisibility();
      }
    }
    if(events.sdlKeyCode == SDLK_F5 && events.numModifiersDown == 0) {
      if(looperPanel) {
        looperPanel->toggleVisibility();
      }
    }
    if(events.sdlKeyCode == SDLK_F6 && events.numModifiersDown == 0) {
      if(recordingPanel) {
        recordingPanel->toggleVisibility();
      }
    }
    if(events.sdlKeyCode == SDLK_F9 && events.numModifiersDown == 0) {
      synth->stereoOscilloscope.getPanel()->toggleVisibility();
    }
  }
  
  void onHuiMessage(HuiMessage &huiMessage) {
    if(huiMessage.type == HuiMessage::Type::RecordPressed) {
      if(looperPanel) {
        looperPanel->editLooperTrackGui->setValue(synth->isRecordingMode);
      }
    }
    //midiEventPanel->update(huiMessage);
  }
  
  void onMidiMessage(const MidiMessage &midiMessage) {
    int channel = midiMessage.getMessageChannel() + 1;
    
    if(synth->numInstrumentTracks < 1) return;
    
    std::vector<int> instrumentTrackIndices;
    for(int k=0; k<synth->numInstrumentTracks; k++) {
      if(channel != 0 && channel == synth->instrumentTracks[k].midiInChannel) {
        instrumentTrackIndices.push_back(k);
      }
    }
    
    if(instrumentTrackIndices.size() != 0) {

      if(midiMessage.getMessageType() == MidiMessage::ProgramChange) {
        for(int k=0; k<instrumentTrackIndices.size(); k++) {
          synth->instrumentTracks[instrumentTrackIndices[k]].instrumentIndex += midiMessage.controlChangeDirection;
          
          if(synth->instrumentTracks[instrumentTrackIndices[k]].instrumentIndex < 0) {
            synth->instrumentTracks[instrumentTrackIndices[k]].instrumentIndex = synth->instruments.size() - 1;
          }
          else if(synth->instrumentTracks[instrumentTrackIndices[k]].instrumentIndex >= synth->instruments.size()) {
            synth->instrumentTracks[instrumentTrackIndices[k]].instrumentIndex = 0;
          }
          if(instrumentTrackPanel) {
            instrumentTrackPanel->activeInstrumentNameGuis[instrumentTrackIndices[k]]->setValue(synth->instrumentTracks[instrumentTrackIndices[k]].instrumentIndex);
          }
        }
      }

      if(midiMessage.getMessageType() == MidiMessage::ControlChange) {
        
        if(midiMessage.dataByte1 == MidiMessage::ChannelVolume) {
          for(int k=0; k<instrumentTrackIndices.size(); k++) {
            synth->instrumentTracks[instrumentTrackIndices[k]].volume = (double)midiMessage.dataByte2 / 127.0;
            if(instrumentTrackPanel) {
              instrumentTrackPanel->activeInstrumentVolumeGuis[instrumentTrackIndices[k]]->setValue(synth->instrumentTracks[instrumentTrackIndices[k]].volume);
            }
          }
        }
        
      }
    }
    //midiEventPanel->update(midiMessage);
  }
  
  
  
};


