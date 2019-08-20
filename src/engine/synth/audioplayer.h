#pragma once
#include "../gui/gui.h"
#include "recordingtrack.h"
#include "sndfileio.h"
/* TODO
 * - extract audio file meta data such as song, artist, album...
 * - extract only supported audio files
 * - mp3 (DONE) and mp4 (m4a) loading (MP4 file loading done partially, apparently decoders needed to get the audio in waveform)
 * - file streaming from disk
 *
 * - abort loading thread during quit
 */


struct AudioPlayer : public Panel {
  FileBrowser *fileBrowser = NULL;
  //Label *currentlyPlayingTrackLabel = NULL;
  TextBox *trackTimeGui = NULL;
  
  std::vector<std::string> trackPaths;
  std::vector<std::string> trackNames;
  
  AudioRecordingTrack audioRecordingTrack;
  
  bool isPlaying = false;
  
  std::string nowPlayingText = "";
  bool trackReady = false;
  
  enum LoopingMode { NoLooping, LoopTrack, LoopTrackList, NumLoopingModes };
  LoopingMode loopingMode = LoopTrackList; // TODO GUI
  
  bool shuffleTracks = false;
  
  int activeTrackIndex = -1;
  int numTracks = 0;
  
  bool seekingTrackActive = false;
  double seekingPosition = 0;
  
  // TODO button gui element
  enum ButtonStatus { Released, Pressed };
  ButtonStatus playButtonStatus, stopButtonStatus, nextButtonStatus, previousButtonStatus, progressButtonStatus;
  Rect playPauseRect, stopRect, nextTrackRect, previousTrackRect;
  Rect progressRect, progressBar;
  Vec2d progressionLimits;
  
  Rect volumeBar, volumeBarCentered;
  
  Rect repeatRect, showFileBrowserRect;
  
  int numVolumeLevels = 10;
  int volumeLevel = 10;
  
  bool progressTrackList = true;
  
  double progressTrackListWaitingTime = 0;
  TicToc progressTrackListWaitingTicToc;
  bool waitingToProgressTrackList = false;
  
  virtual ~AudioPlayer() {
    for(int i=0; i<loadingThreads.size(); i++) {
      if(loadingThreads[i].joinable()) {
        loadingThreads[i].join();
      }
    }
    loadingThreads.clear();
  }
  
  Texture repeatTrackListTexture, repeatTrackTexture, repeatNoneTexture;
  Texture playTexture, pauseTexture, stopTexture;
  Texture nextTrackTexture, previousTrackTexture;
  Texture seekerTexture;
  
  Texture showFileBrowserTexture;
  
  std::vector<std::thread> loadingThreads;
  int fileLoadStatus = 0;
  
  AudioPlayer(int sampleRate, int framesPerBuffer) : Panel("Audioplayer") {
    init(sampleRate, framesPerBuffer);
  }
  
  void init(int sampleRate, int framesPerBuffer) {
    nowPlayingText = "";
    
    setDimensions(450);
    
    
    addChildElement(fileBrowser = new FileBrowser("", "/home/lassi/prog/c/playground/build/output/synth/recordings", 450, 450, 0, size.y+5));
    
    addGuiEventListener(new FileBrowserListener(this));
    addGuiEventListener(new AudioPlayerListener(this));
    backgroundColor.set(0.1, 0.1, 0.1, 0.5);
    borderColor.set(1, 1, 1, 0.5);
    
    playTexture.load("data/synth/textures/play.png", false);
    pauseTexture.load("data/synth/textures/pause.png", false);
    stopTexture.load("data/synth/textures/stop.png", false);
    nextTrackTexture.load("data/synth/textures/next.png", false);
    previousTrackTexture.load("data/synth/textures/previous.png", false);
    repeatTrackListTexture.load("data/synth/textures/repeat_list.png", false);
    repeatTrackTexture.load("data/synth/textures/repeat_track.png", false);
    repeatNoneTexture.load("data/synth/textures/repeat_none.png", false);
    showFileBrowserTexture.load("data/synth/textures/gui.png", false);
    seekerTexture.load("data/synth/textures/seeker.png", false);
    
    audioRecordingTrack.init(sampleRate, framesPerBuffer, 5);
    
    updateDirectory();
  }
  
  void setDimensions(double width) {
    
    double line = 5+23, lineHeight = 23;
    
    //addChildElement(currentlyPlayingTrackLabel = new Label("Nothing playing", 5, line += lineHeight));
    //currentlyPlayingTrackLabel->labelColor.set(0, 0, 0, 1);
    double w = 16, h = 16, x = 18;
    double volumeBarWidth = 60;
    playPauseRect.set(w, h, x, line+=(lineHeight-8));
    stopRect.set(14, 14, x+25, line);
    previousTrackRect.set(w, h, x+25*2, line);
    nextTrackRect.set(w, h, x+25*3, line);

    repeatRect.set(w, h, x+25*4, line);
    showFileBrowserRect.set(w, h, x+25*5, line);
    
    progressRect.set(w, h, x+25*6-5, line);
    progressBar.set(width-(x+25*6-5+10+10+volumeBarWidth), h, x+25*6-5, line);
    
    volumeBar.set(volumeBarWidth, h, width - (volumeBarWidth + 10), line);
    volumeBarCentered = volumeBar;
    volumeBarCentered.pos.x = volumeBar.pos.x + volumeBar.w * 0.5;
    
    setSize(width, line+16+5);
  }
  
  virtual void onPrerender(GeomRenderer &geomRenderer, TextGl &textRenderer, const Vec2d &displacement) {
    geomRenderer.fillColor.set(0.5, 0.5, 0.5, 0.5);
    geomRenderer.strokeColor.set(1, 1, 1, 0.5);
    geomRenderer.strokeWidth = 0;
    geomRenderer.strokeType = 1;
    geomRenderer.drawRect(size, displacement + size * 0.5);
  }
  
  // FIXME buttons wrapped into classes
  virtual void onRender(GeomRenderer &geomRenderer, TextGl &textRenderer) {

    
    textRenderer.setColor(0, 0, 0, 1);
    textRenderer.print(nowPlayingText, absolutePos.x+5, absolutePos.y+5, 12);
    
    if(trackReady) {
      Vec2d tt = textRenderer.getDimensions(nowPlayingText, 12);
      int p = (int)ceil(tt.x + 120);
      if(p > size.x && size.x >= 450) {
        setDimensions(p);
        return;
      }
      if(tt.x + 120 <= 450 && size.x > 450) {
        setDimensions(450);
        return;
      }
      
      
      std::string timeStr;
      if(seekingTrackActive) {
        textRenderer.setColor(0.4, 0, 0, 1);
        timeStr = formatTime(seekingPosition * audioRecordingTrack.audioRecording.getLengthInSeconds());
      }
      else {
        textRenderer.setColor(0, 0, 0, 1);
        timeStr = formatTime(audioRecordingTrack.recordingPlayback.getPositionInSeconds());
      }
      timeStr = timeStr + " / " + formatTime(audioRecordingTrack.audioRecording.getLengthInSeconds());
      Vec2d ts = textRenderer.getDimensions(timeStr, 12);
      
      textRenderer.print(timeStr, absolutePos.x + size.x-5-ts.x, absolutePos.y+5, 12);
    }

    geomRenderer.texture = NULL;
    geomRenderer.fillColor.set(0.5, 0.5, 0.5, 0.3);
    geomRenderer.strokeColor.set(0.9, 0.9, 0.9, 0.3);
    geomRenderer.strokeType = 1;
    geomRenderer.strokeWidth = 1;

    if(!isPlaying || !trackReady) {
      playTexture.render(playPauseRect.pos+absolutePos, !trackReady ? Vec4d(0, 0, 0, 0.3) : Vec4d(0, 0, 0, 0.8));
    }
    else {
      pauseTexture.render(playPauseRect.pos+absolutePos, Vec4d(0, 0, 0, 0.8));
    }

    stopTexture.render(stopRect.pos+absolutePos, !trackReady ? Vec4d(0, 0, 0, 0.3) : Vec4d(0, 0, 0, 0.8));

    previousTrackTexture.render(previousTrackRect.pos+absolutePos, !trackReady ? Vec4d(0, 0, 0, 0.3) : Vec4d(0, 0, 0, 0.8));

    nextTrackTexture.render(nextTrackRect.pos+absolutePos, !trackReady ? Vec4d(0, 0, 0, 0.3) : Vec4d(0, 0, 0, 0.8));

    if(loopingMode == NoLooping) {
      repeatNoneTexture.render(repeatRect.pos+absolutePos, Vec4d(0, 0, 0, 1));
    }
    else if(loopingMode == LoopTrack) {
      repeatTrackTexture.render(repeatRect.pos+absolutePos, Vec4d(0, 0, 0, 0.8));
    }
    else if(loopingMode == LoopTrackList) {
      repeatTrackListTexture.render(repeatRect.pos+absolutePos, Vec4d(0, 0, 0, 0.8));
    }
    
    if(!fileBrowser || !fileBrowser->isVisible) {
      showFileBrowserTexture.render(showFileBrowserRect.pos+absolutePos, Vec4d(0, 0, 0, 0.3));
    }
    else {
      showFileBrowserTexture.render(showFileBrowserRect.pos+absolutePos, Vec4d(0, 0, 0, 0.8));
    }


    if(!seekingTrackActive) {
      geomRenderer.strokeColor.set(0, 0, 0, 0.66);
      geomRenderer.fillColor.set(0, 0, 0, 0.66);
    }
    else {
      geomRenderer.strokeColor.set(0.3, 0.0, 0.0, 0.8);
      geomRenderer.fillColor.set(0.3, 0.0, 0.0, 0.8);
    }
    if(audioRecordingTrack.audioRecording.getLengthInSeconds() > 0) {
      if(seekingTrackActive) {
        progressRect.pos.x = progressBar.pos.x + progressBar.w * seekingPosition;
      }
      else {
        progressRect.pos.x = progressBar.pos.x + progressBar.w * audioRecordingTrack.recordingPlayback.getPositionInSeconds()/ audioRecordingTrack.audioRecording.getLengthInSeconds();
      }
    }
    else {
      progressRect.pos.x = progressBar.pos.x;
    }
    geomRenderer.drawLine(progressBar.pos, progressBar.pos+Vec2d(progressBar.w, 0), absolutePos);
    seekerTexture.render(progressRect.pos+absolutePos, !trackReady ? Vec4d(0, 0, 0, 0.3) : (!seekingTrackActive ? Vec4d(0, 0, 0, 0.8) : Vec4d(0.3, 0, 0, 0.8)));


    for(int i=0; i<numVolumeLevels; i++) {
      double dx = map(i, 0, numVolumeLevels-1, 0, volumeBar.w);
      if(i < volumeLevel) {
        geomRenderer.strokeColor.set(0, 0, 0, 1);
        geomRenderer.fillColor.set(0.15, 0.15, 0.15, 0.7);
      }
      else {
        geomRenderer.strokeColor.set(0, 0, 0, 1);
        geomRenderer.fillColor.set(0.5, 0.5, 0.5, 0.5);
      }
      geomRenderer.drawRect(4, volumeBar.h, floor(absolutePos.x+volumeBar.pos.x + dx), absolutePos.y+volumeBar.pos.y);
    }
    

  }
  
  void playPauseTrack() {
    if(!(activeTrackIndex >= 0 && activeTrackIndex < numTracks) || fileLoadStatus >= 1) return;
    
    if(isPlaying) {
      isPlaying = false;
    }
    else {
      isPlaying = true;
      if(activeTrackIndex < 0) {
        activeTrackIndex = 0;
        loadActiveTrackFromFile();
      }
      else {
        audioRecordingTrack.onStart();
      }
    }
    //printf("||> %d, %d\n", isPlaying, activeTrackIndex);
  }
  
  void stopTrack() {
    if(!(activeTrackIndex >= 0 && activeTrackIndex < numTracks) || fileLoadStatus >= 1) return;
    
    isPlaying = false;
    activeTrackIndex = -1;
    fileBrowser->filesGui->setActiveItemIndex(activeTrackIndex);
    nowPlayingText = "";
    trackReady = false;
    
    audioRecordingTrack.toBeginning();
  }
  
  void nextTrack() {
    if(!(activeTrackIndex >= 0 && activeTrackIndex < numTracks) || fileLoadStatus >= 1) return;
    
    if(activeTrackIndex == numTracks-1 && loopingMode == NoLooping) {
      isPlaying = false;
    }
    
    activeTrackIndex = (activeTrackIndex+1) % numTracks;
    fileBrowser->filesGui->setActiveItemIndex(activeTrackIndex);
    loadActiveTrackFromFile();
  }
    
  
  void previousTrack() {
    if(!(activeTrackIndex >= 0 && activeTrackIndex < numTracks) || fileLoadStatus >= 1) return;
    
    activeTrackIndex = (activeTrackIndex-1 + numTracks) % numTracks;
    fileBrowser->filesGui->setActiveItemIndex(activeTrackIndex);
    loadActiveTrackFromFile();
  }
  

  
  
  
  /*void loadActiveTrackFromFile() {
    if(activeTrackIndex >= 0 && activeTrackIndex < trackPaths.size()) {
      if(AudioFileIo::open(trackPaths[activeTrackIndex], audioRecordingTrack)) {
        audioRecordingTrack.toBeginning();
        audioRecordingTrack.onStart();
      
      //currentlyPlayingTrackLabel->setText(trackNames[activeTrackIndex]+ " (" + formatTime(audioRecordingTrack.audioRecording.getLengthInSeconds()) + ")");
        nowPlayingText = trackNames[activeTrackIndex];//+ " (" + formatTime(audioRecordingTrack.audioRecording.getLengthInSeconds()) + ")";
        trackReady = true;
        seekingTrackActive = false;
      }
    }
  }*/
  static void loadActiveTrackFromFileX(AudioPlayer *audioPlayer) {
    if(AudioFileIo::open(audioPlayer->trackPaths[audioPlayer->activeTrackIndex], audioPlayer->audioRecordingTrack)) {
      audioPlayer->fileLoadStatus = 2;
    }
    else {
      audioPlayer->fileLoadStatus = -1;
    }
    //audioPlayer->onTrackLoaded(audioPlayer->fileLoadStatus == 2);
    //printf("debugging at AudioPlayer.loadActiveTrackFromFileX()...\n");
  }
  virtual void onTrackLoaded(bool trackLoaded) {
    printf("debugging at AudioPlayer.onTrackLoaded()...\n");
  }
  
  void loadActiveTrackFromFile() {
    if(activeTrackIndex >= 0 && activeTrackIndex < trackPaths.size() && fileLoadStatus <= 0) {
      fileLoadStatus = 1;
      trackReady = false;
      seekingTrackActive = false;
      nowPlayingText = "Loading...";
      loadingThreads.push_back(std::thread(loadActiveTrackFromFileX, this));
    }
  }
  
  void update(std::vector<Vec2d> &recordingPlaybackBuffer, long recordingPlaybackBufferPosition) {
    if(fileLoadStatus == 2 || fileLoadStatus < 0) {
      for(int i=0; i<loadingThreads.size(); i++) {
        if(loadingThreads[i].joinable()) {
          loadingThreads[i].join();
        }
      }
      loadingThreads.clear();
      audioRecordingTrack.toBeginning();
      audioRecordingTrack.onStart();
    
      if(fileLoadStatus == 2) {
        if(activeTrackIndex >= 0 && activeTrackIndex < trackNames.size()) {
          nowPlayingText = trackNames[activeTrackIndex];
        }
        trackReady = true;
        fileLoadStatus = 0;
        onTrackLoaded(true);
      }
      else {
        nowPlayingText = "";
        if(activeTrackIndex+1 < numTracks) {
          nextTrack();
        }
        onTrackLoaded(false);
      }
    }
    if(progressTrackList && waitingToProgressTrackList) {
      if(progressTrackListWaitingTicToc.toc() >= progressTrackListWaitingTime) {
        waitingToProgressTrackList = false;
        nextTrack();
      }
    }
    else if(fileLoadStatus == 0 && trackReady && activeTrackIndex >= 0 && activeTrackIndex < numTracks && isPlaying) {
      audioRecordingTrack.onUpdate(recordingPlaybackBuffer, recordingPlaybackBufferPosition);
      if(audioRecordingTrack.playbackFinished) {
        if(loopingMode == LoopTrack) {
          audioRecordingTrack.toBeginning();
          audioRecordingTrack.onStart();
        }
        else if(progressTrackList) {
          waitingToProgressTrackList = true;
          progressTrackListWaitingTicToc.tic();
        }
      }
    }
    
  }
  
  
  
  void updateDirectory() {
    trackPaths = fileBrowser->directory.files;
    trackNames = fileBrowser->directory.fileNames;
    numTracks = trackPaths.size();
    /*for(int i=0; i<audioPlayer->trackNames.size(); i++) {
      printf("%s\n", audioPlayer->trackNames[i].c_str());
    }*/
  }
  
  struct FileBrowserListener : public GuiEventListener {
    AudioPlayer *audioPlayer;
    FileBrowserListener(AudioPlayer *audioPlayer) {
      this->audioPlayer = audioPlayer;
    }
    /*virtual void onValueChange(GuiElement *guiElement) {
    }*/
    virtual void onMessage(GuiElement *guiElement, const std::string &msg) {
      if(msg == "dirChanged" && guiElement == audioPlayer->fileBrowser) {
        audioPlayer->stopTrack();
        audioPlayer->updateDirectory();
      }
    }
    virtual void onAction(GuiElement *guiElement) {
      if(guiElement == audioPlayer->fileBrowser->filesGui) {
        int index = -1;
        guiElement->getValue((void*)&index);
        if(index >= 0 && index < audioPlayer->trackPaths.size()) {
          audioPlayer->activeTrackIndex = index;
          audioPlayer->loadActiveTrackFromFile();
          //printf("load %d\n", audioPlayer->activeTrackIndex);
        }
      }
    }
  };
  
  struct AudioPlayerListener : public GuiEventListener {
    AudioPlayer *audioPlayer;
    AudioPlayerListener(AudioPlayer *audioPlayer) {
      this->audioPlayer = audioPlayer;
    }
    virtual void onMousePressed(GuiElement *guiElement, Events &events) {
      if(guiElement != audioPlayer) return;
      //printf("mouse pressed\n");
      Vec2d p = events.m - audioPlayer->absolutePos;
      //printf("(%f, %f), (%f, %f), (%f, %f)\n", audioPlayer->playPauseRect.pos.x, audioPlayer->playPauseRect.pos.y, events.m.x, events.m.y, p.x, p.y);
      audioPlayer->draggable = false;
      
      if(audioPlayer->playPauseRect.isPointWithin(p)) {
        //printf("mouse pressed within\n");
        audioPlayer->playButtonStatus = Pressed;
        audioPlayer->playPauseTrack();
      }
      else if(audioPlayer->stopRect.isPointWithin(p)) {
        audioPlayer->stopButtonStatus = Pressed;
        audioPlayer->stopTrack();
      }
      else if(audioPlayer->previousTrackRect.isPointWithin(p)) {
        audioPlayer->previousButtonStatus = Pressed;
        audioPlayer->previousTrack();
      }
      else if(audioPlayer->nextTrackRect.isPointWithin(p)) {
        audioPlayer->nextButtonStatus = Pressed;
        audioPlayer->nextTrack();
      }
      else if(audioPlayer->repeatRect.isPointWithin(p)) {
        audioPlayer->loopingMode = (AudioPlayer::LoopingMode)((audioPlayer->loopingMode+1) % AudioPlayer::LoopingMode::NumLoopingModes);
      }
      else if(audioPlayer->showFileBrowserRect.isPointWithin(p) && audioPlayer->fileBrowser) {
        audioPlayer->fileBrowser->toggleVisibility();
      }
      else if(audioPlayer->progressRect.isPointWithin(p) && audioPlayer->audioRecordingTrack.audioRecording.getLengthInSeconds() > 0) {
        audioPlayer->seekingTrackActive = true;
        audioPlayer->seekingPosition = audioPlayer->audioRecordingTrack.recordingPlayback.getPositionInSeconds() / audioPlayer->audioRecordingTrack.audioRecording.getLengthInSeconds();
      }
      else {
        audioPlayer->draggable = true;
      }
    }
    
    virtual void onMouseReleased(GuiElement *guiElement, Events &events) {
      if(guiElement != audioPlayer) return;
      if(audioPlayer->playButtonStatus == Pressed) {
        audioPlayer->playButtonStatus = Released;
      }
      if(audioPlayer->stopButtonStatus == Pressed) {
        audioPlayer->stopButtonStatus = Released;
      }
      if(audioPlayer->previousButtonStatus == Pressed) {
        audioPlayer->previousButtonStatus = Released;
      }
      if(audioPlayer->nextButtonStatus == Pressed) {
        audioPlayer->nextButtonStatus = Released;
      }
      if(audioPlayer->nextButtonStatus == Pressed) {
        audioPlayer->nextButtonStatus = Released;
      }
      if(audioPlayer->seekingTrackActive) {
        audioPlayer->audioRecordingTrack.setTimePostion(audioPlayer->seekingPosition * audioPlayer->audioRecordingTrack.audioRecording.getLengthInSeconds());
        audioPlayer->seekingTrackActive = false;
      }
      audioPlayer->draggable = true;
    }
    
    virtual void onMouseMotion(GuiElement *guiElement, Events &events) {
      if(guiElement != audioPlayer) return;
      if(audioPlayer->seekingTrackActive) {
        double dx = (events.m.x - events.mp.x) / audioPlayer->progressBar.w;
        audioPlayer->seekingPosition = clamp(audioPlayer->seekingPosition + dx, 0, 1);
      }
    }
    
    virtual void onMouseWheel(GuiElement *guiElement, Events &events) {
      if(guiElement != audioPlayer) return;
      Vec2d p = events.m - audioPlayer->absolutePos;
      if(audioPlayer->volumeBarCentered.isPointWithin(p)) {
        audioPlayer->volumeLevel = clamp(audioPlayer->volumeLevel + events.mouseWheel, 0, audioPlayer->numVolumeLevels);
        audioPlayer->audioRecordingTrack.volume = (double)audioPlayer->volumeLevel / audioPlayer->numVolumeLevels;
      }
    }
    
  };
};
















