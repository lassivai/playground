#pragma once
#include "recording.h"
//#include "../events.h"
#include "../gui/gui.h"
/* Recording buffer should be few seconds longer than the looper duration to capture the notes that start at the end of loop,
 * but don't go off before loop is repeated.
 * Instead, if the recording is continued at the beginning on top of the early part of the loop, the starting sounds weird.
 * 
 * Progammer can't help in this respect (this time). The user shall choose whether to
 *  1) Record continuosly on top of the previous recording when loop is repeated
 *  2) Record only single run of the loop
 *    a) cutting the notes that haven't finished, or
 *    b) having those additional few seconds at the end recorded, and played back when loop is repeated
 *
 * Jos siellä sattuu olemaan uteliaita. Tarkoituksena siis pystyä nauhoittamaan ulkoista ääntä, ja soittamaan se syntikassa olevan sekvensserin (eli nuoteiksi 
 * tallennetun syntikkasoundin) kanssa rinnatusten. Kaikenlaisia mahdollisuuksia miten juttua hyödyntää. Tuohon launchpad ruudukko-ohjaimeen ajattelin tehdä 
 * jutun jonka avulla voi helposti valita missä järjestyksessä ja miten rinnastusten (päällekkäin) nääniraitoja soittaa...
 *
 * Ihan ensimmäinen testi varmaan tulee olemaan nokkahuiluorkesteri! (vähintään duo. nokkahuiluja löytyy nurkista basso, tenori, al
 tto, sopraano ja sopraniino...)
 *
 */

struct AudioRecordingTrack {
  /* TODO
   *  - zoomable, movable view of audio recording waveform
   *  - there's an easy way of doing this: sample one point in waveform closest to each pixel. But it will look very "pixelated" and ugly when zooming and moving (DONE)
   *  - there's a hard way of doing this: have an average of all the points of the waveform under the current pixel (there might be hundreds of thousands of samples
        under each pixel if the recording is long, so it can't be calculated each frame with brute force - but DONE)
   *     - I might try hierarchical "mipmap" levels similarly as in rendering textures, so that there would be, for instance...
   *        - 1/10 of the lenght of the original recording pre-averaged array
   *        - 1/10 of the length of the previous array
   *        - etc.
   *  - I am paricularly interested about this feature concidering the upcoming next feature: sample player. One could explore any recording, music or whatever,
*    and test all the little pieces of waveforms how they might sound as wavetable synth instruments   ff
   *    - I will automate the process to some extent so that one could select approximately a region and the automation would find appropriate wave cycle, 
   *      possibly several cycles and taking average of those. Or the automation would play every cycle in that region and good sounding stuff could be selected based
   *      on those played samples
   */
   
  struct TrackGraphPanel : public GraphPanel {
    // FIXME use circular buffer class
    
    enum DraggingMode { None, ZoomDrag, MoveDrag };
    DraggingMode draggingMode = None;
    std::vector<double> envelopeHigh;
    std::vector<double> envelopeLow;
    std::vector<double> waveForm;

    Vec2d timeRange;
    int numAverageArrays = 10;
    double averageArrayFactor = 0.1;
    std::vector<std::vector<Vec2d>> averageArrays;

    AudioRecordingTrack *audioRecordingTrack = NULL;

    TrackGraphPanel(AudioRecordingTrack *audioRecordingTrack) {
      timeRange.set(0, 10);
      
      this->audioRecordingTrack = audioRecordingTrack;
      envelopeHigh.resize(300, 0);
      envelopeLow.resize(300, 0);
      waveForm.resize(300, 0);
      this->numHorizontalMarks = 0;
      this->numVerticalMarks = 0;
      this->horizontalAxisLimits.set(timeRange.x, timeRange.y);
      this->verticalAxisLimits.set(-1, 1);
      showAxes = false;
      this->numHorizontalAxisLabels = 2;
      this->horizontalAxisUnit = "s";
      //this->graphAsArea = true;
      this->data = &waveForm;
      labelColor.set(1, 1, 1, 1);
      //this->data = &envelopeHigh;
      //this->data2 = &envelopeLow;
      this->prerenderingNeeded = true;
      
      
      
      this->setSize(300, 80);
      
      addGuiEventListener(new TrackGraphPanelListener(this));
    }

    //double timer = 0;
    //double updateIntervalSecs = 0;

    void update() {
      if(!isVisible) return;
      
      if(audioRecordingTrack->isRecording) {
        if(audioRecordingTrack->audioRecording.getLengthInSeconds() > timeRange.y) {
          timeRange.x = audioRecordingTrack->audioRecording.getLengthInSeconds() - (timeRange.y - timeRange.x);
          timeRange.y = audioRecordingTrack->audioRecording.getLengthInSeconds();
          this->horizontalAxisLimits.set(timeRange.x, timeRange.y);
        }
      }
      else if(audioRecordingTrack->recordingPlayback.status == RecordingPlayback::Status::Playing) {
        if(audioRecordingTrack->recordingPlayback.getPositionInSeconds() > timeRange.y) {
          timeRange.x = audioRecordingTrack->recordingPlayback.getPositionInSeconds() - (timeRange.y - timeRange.x);
          timeRange.y = audioRecordingTrack->recordingPlayback.getPositionInSeconds();
          this->horizontalAxisLimits.set(timeRange.x, timeRange.y);
        }
      }


      double sampleRate = audioRecordingTrack->delayLine->sampleRate;
      
      // I try first the first and ugly solution, that was the easy part for now x|
      // now I did visually correctly, but with brute force so it was very slow to calculate the average envelope for the whole recording
      // the hardest part would be fast and correctly. Maybe tomorrow.
      
      // The zooming interface needs fixing too... now it zoomed too fast at wide field, and too slow at narrow field. Stupid little bug over there, I think..

      if(timeRange.y - timeRange.x == (double)size.x/sampleRate) {

        waveForm.assign(waveForm.size(), 0);
        double maxValue = 0;

        for(int i=0; i<waveForm.size(); i++) {
          
          long a = (long)map(i, 0.0, waveForm.size(), timeRange.x*sampleRate, timeRange.y*sampleRate);
          if(a >= audioRecordingTrack->audioRecording.numVariableSamples) break;
          
          waveForm[i] = audioRecordingTrack->audioRecording.samples[a].x;
          maxValue = max(maxValue, fabs(waveForm[i]));
        }
      
        if(maxValue != 0) {
          for(int i=0; i<waveForm.size(); i++) {
            waveForm[i] /= maxValue;
          }
        }
        
        this->graphAsArea = false;
        this->data = &waveForm;
      }
      
      else {
        envelopeLow.assign(envelopeLow.size(), 0);
        envelopeHigh.assign(envelopeHigh.size(), 0);
        double maxValue = 0;

        for(int i=0; i<envelopeHigh.size(); i++) {
          long a = (long)map(i, 0.0, envelopeHigh.size(), timeRange.x*sampleRate, timeRange.y*sampleRate);
          long b = (long)map(i+1, 0.0, envelopeLow.size(), timeRange.x*sampleRate, timeRange.y*sampleRate);
          if(a >= audioRecordingTrack->audioRecording.numVariableSamples) break;
          
          double minVal = 1e10, maxVal = -1e10;
          
          for(int k=a; k<b; k++) {
            if(k >= audioRecordingTrack->audioRecording.numVariableSamples) break;
            
            minVal = min(minVal, audioRecordingTrack->audioRecording.samples[k].x);
            maxVal = max(maxVal, audioRecordingTrack->audioRecording.samples[k].x);
          }
          envelopeLow[i] = minVal;
          envelopeHigh[i] = maxVal;
          maxValue = max(maxValue, max(fabs(maxVal), fabs(minVal)));
        }
      

        if(maxValue != 0) {
          for(int i=0; i<waveForm.size(); i++) {
            envelopeLow[i] /= maxValue;
            envelopeHigh[i] /= maxValue;
          }
        }
        this->graphAsArea = true;
        this->data = &envelopeHigh;
        this->data2 = &envelopeLow;

      }
      prerenderingNeeded = true;

    }

    virtual void onPrerender(GeomRenderer &geomRenderer, TextGl &textRenderer, const Vec2d &displacement) {
      GraphPanel::onPrerender(geomRenderer, textRenderer, displacement);


      if(audioRecordingTrack->recordingPlayback.status == RecordingPlayback::Status::Playing) {
        double x = map(audioRecordingTrack->recordingPlayback.getPositionInSeconds(), timeRange.x, timeRange.y, 6.0, size.x-6.0);
        if(x >= 6 && x < size.x-6) {
          geomRenderer.strokeWidth = 1.0;
          geomRenderer.strokeColor.set(1, 1, 1, 0.6);
          geomRenderer.strokeType = 1;
          geomRenderer.drawLine(x, 10, x, 70, displacement);
        }
      }
      textRenderer.setColor(1, 1, 1, 0.6);
      char buf[128];
      std::sprintf(buf, "%s", audioRecordingTrack->name.c_str());
      textRenderer.print(buf, displacement.x + 10, displacement.y + 3, 10);
    }
    
    struct TrackGraphPanelListener : public GuiEventListener {
      TrackGraphPanel *trackGraphPanel;
      TrackGraphPanelListener(TrackGraphPanel *trackGraphPanel) {
        this->trackGraphPanel = trackGraphPanel;
      }
      virtual void onMouseMotion(GuiElement *guiElement, Events &events)  {
        if(trackGraphPanel->draggingMode == TrackGraphPanel::DraggingMode::MoveDrag) {
          double delta = -(events.m.x - events.mp.x) * 1.0/trackGraphPanel->size.x * (trackGraphPanel->timeRange.y - trackGraphPanel->timeRange.x);
          
          if(trackGraphPanel->timeRange.x + delta < 0) {
            trackGraphPanel->timeRange.y = trackGraphPanel->timeRange.y - trackGraphPanel->timeRange.x;
            trackGraphPanel->timeRange.x = 0;
          }
          else {
            trackGraphPanel->timeRange.x += delta;
            trackGraphPanel->timeRange.y += delta;
          }
          
          trackGraphPanel->horizontalAxisLimits.set(trackGraphPanel->timeRange.x, trackGraphPanel->timeRange.y);
          
          trackGraphPanel->dragging = false;
          trackGraphPanel->update();
        }

        if(trackGraphPanel->draggingMode == TrackGraphPanel::DraggingMode::ZoomDrag) {
          double delta = (events.m.x - events.mp.x) * 1.0/trackGraphPanel->size.x;
          
          double range = trackGraphPanel->timeRange.y - trackGraphPanel->timeRange.x;
          
          if(delta > 0) {
            range = range * (1.0 + delta * 0.333);
          }
          if(delta < 0) {
            range = range / (1.0 - delta * 0.333);
          }
          /*if(delta > 0) {
            range = range * (1.0 + delta * 0.1 * pow(range, -0.2));
          }
          if(delta < 0) {
            range = range / (1.0 - delta * 0.1 * pow(range, -0.2));
          }*/
          
          range = clamp(range, (double)trackGraphPanel->size.x/trackGraphPanel->audioRecordingTrack->delayLine->sampleRate, 24*60*60);
          
          /*double midPoint = (trackGraphPanel->timeRange.y + trackGraphPanel->timeRange.x) * 0.5;
          
          if(midPoint - range*0.5 < 0) {
            trackGraphPanel->timeRange.x = 0;
            trackGraphPanel->timeRange.y = range;
          }
          else {
            trackGraphPanel->timeRange.x = midPoint - range*0.5;
            trackGraphPanel->timeRange.y = midPoint + range*0.5;
          }*/
          trackGraphPanel->timeRange.y = trackGraphPanel->timeRange.x + range;
          
          trackGraphPanel->horizontalAxisLimits.set(trackGraphPanel->timeRange.x, trackGraphPanel->timeRange.y);
          
          trackGraphPanel->dragging = false;
          trackGraphPanel->update();

        }
        
      }
      virtual void onMousePressed(GuiElement *guiElement, Events &events)  {
        if(!trackGraphPanel->isPointWithin(events.m)) return;
        if(events.mouseButton == 1) {
          trackGraphPanel->draggingMode = TrackGraphPanel::DraggingMode::MoveDrag;
        }
        if(events.mouseButton == 2) {
          trackGraphPanel->draggingMode = TrackGraphPanel::DraggingMode::ZoomDrag;
        }
      }
      virtual void onMouseReleased(GuiElement *guiElement, Events &events)  {
        trackGraphPanel->draggingMode = TrackGraphPanel::DraggingMode::None;
      }
    };
  };
  
  
  AudioRecording audioRecording;
  RecordingPlayback recordingPlayback;
  AudioFileMetaData audioFileMetaData;
  
  TrackGraphPanel *trackGraphPanel = NULL;
  
  enum RecordMode { Continuous, Looping };
  const std::vector<std::string> recordModeNames = { "Cont.", "Looping" };
  
  std::string name = "<unnamed>";
  
  bool isRecording = false;
  
  bool isLooping = false;

  RecordMode recordMode = Continuous;
  double volume = 1.0;
  bool isMuted = false;
  double recordingLength = 0;
  
  bool playbackFinished = false;
  
  DelayLine *delayLine;
  
  Equalizer *equalizer = NULL;
  std::vector<PostProcessingEffect*> postProcessingEffects;
  
  bool isRecorded = false;
  
  void init(int sampleRate, int framesPerBuffer, double delayLineLength = 5) {
    this->delayLine = new DelayLine(sampleRate, delayLineLength);
    audioRecording.initialize(*delayLine);
    recordingPlayback.initialize(&audioRecording.samples, framesPerBuffer, delayLine->sampleRate);
    
    postProcessingEffects.push_back(equalizer = new Equalizer(delayLine->sampleRate));
    equalizer->isActive = false;
  }
  
  void init(DelayLine *delayLine, int framesPerBuffer) {
    this->delayLine = delayLine;
    audioRecording.initialize(*delayLine);
    recordingPlayback.initialize(&audioRecording.samples, framesPerBuffer, delayLine->sampleRate);
    
    postProcessingEffects.push_back(equalizer = new Equalizer(delayLine->sampleRate));
    equalizer->isActive = false;
  }
  
  void reset() {
    audioRecording.initialize(*delayLine);
    isRecorded = false;
    recordingLength = 0;
    recordingPlayback.reset();
    playbackFinished = true;
  }

  void onUpdate(std::vector<Vec2d> &recordingPlaybackBuffer, long recordingPlaybackBufferPosition) {
    if(isRecording) {
      audioRecording.update(*delayLine);
      recordingLength = audioRecording.getPositionInSeconds();
      if(recordMode == Looping) {
        
      }
    }
    else if(!playbackFinished && !isMuted) {
      recordingPlayback.fillPlaybackBuffer(recordingPlaybackBuffer, recordingPlaybackBufferPosition, volume, &postProcessingEffects);
      if(recordingPlayback.status == RecordingPlayback::Finished || recordingPlayback.getPositionInSeconds() >= recordingLength) {
        if(isLooping) {
          toBeginning();
        }
        else {
          playbackFinished = true;
          recordingPlayback.status = RecordingPlayback::Status::Initialized;
        }
      }
    }
    if(trackGraphPanel) {
      trackGraphPanel->update();
    }
  }
  
  void onStart() {
    if(isRecording) {
      audioRecording.initialize(*delayLine);
      isRecorded = true;
      recordingLength = 0;
    }
    else {
      recordingPlayback.reset();
      recordingLength = audioRecording.getLengthInSeconds();
    }
  }

  void setRecording(bool isRecording) {
    this->isRecording = isRecording;
  }
  void toggleRecording() {
    this->isRecording = !isRecording;
  }
  void toBeginning() {
    recordingPlayback.toBeginning();
    playbackFinished = false;
  }
  
  void setTimePostion(double time) {
    recordingPlayback.setTimePostion(time);
    playbackFinished = false;
  }
  
  Panel *getPanel() {
    return trackGraphPanel ? trackGraphPanel : trackGraphPanel = new TrackGraphPanel(this);
  }

};









