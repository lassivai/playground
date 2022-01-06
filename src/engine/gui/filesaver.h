#pragma once
//#include "../gui/gui.h"
#include "panel.h"
#include "textbox.h"
//#include "button.h"
#include "synthtitlebar.h"
#include "../textfileparser.h"
#include "../util.h"


struct FileSaver : public Panel {
  FileBrowser *fileBrowser = NULL;
  //Label *currentlyPlayingTrackLabel = NULL;
  
  std::string defaultTitle;
  std::string fileName;
  
  SynthTitleBar *titleBar = NULL;
  TextBox *filenameGui = NULL;
  //Button *saveButton = NULL;
  
  std::vector<std::string> existingFilePaths;
  std::vector<std::string> existingFileNames;

  std::thread savingThread;
  int fileSavingStatus = 0;
  
  HierarchicalTextFileParser *fileToBeSaved = NULL;
  
  bool doesFileNameExists = false;
  
  
  virtual ~FileSaver() {}
  
  FileSaver(HierarchicalTextFileParser *fileToBeSaved) : Panel("File saver") {
    init(fileToBeSaved);
    zLayer = 1;
  }
  
  void init(HierarchicalTextFileParser *fileToBeSaved) {
    this->fileToBeSaved = fileToBeSaved;
    setSize(450, 50);
    double line = 0, lineHeight = 31;
    
    defaultTitle = "Save " + fileToBeSaved->getBlockName();
    
    titleBar = new SynthTitleBar(defaultTitle, this);
    titleBar->addCloseButton();
    line += titleBar->size.y + 5;
    
    addChildElement(filenameGui = new TextBox("Name", "", 5, line, 30));
    
    setSize(titleBar->size.x, line + lineHeight + 5);
    
    addChildElement(fileBrowser = new FileBrowser("", fileToBeSaved->getDefaultFileDirectory(), 450, 450, 0, size.y+5));

    addGuiEventListener(new FileSaverListener(this));
    addGuiEventListener(new FileBrowserListener(this));

    updateDirectory();
  }
  
  void save() {
    if(fileName.size() > 0 && fileToBeSaved) {
      if(fileToBeSaved->save(fileBrowser->path + fileName)) {
        titleBar->setText("File saved!");
        fileSavingStatus = 1;
      }
      else {
        titleBar->setText("Couldn't save file...");
        fileSavingStatus = -1;
      }
    }
  }
  
  /*virtual void onPrerender(GeomRenderer &geomRenderer, TextGl &textRenderer, const Vec2d &displacement) {
    geomRenderer.fillColor.set(0.5, 0.5, 0.5, 0.5);
    geomRenderer.strokeColor.set(1, 1, 1, 0.5);
    geomRenderer.strokeWidth = 0;
    geomRenderer.strokeType = 1;
    geomRenderer.drawRect(size, displacement + size * 0.5);
  }*/
  

  virtual void onRender(GeomRenderer &geomRenderer, TextGl &textRenderer) {
    /*if(doesFileNameExists) {
      geomRenderer.texture = NULL;
      geomRenderer.strokeType = 1;
      geomRenderer.strokeWidth = 1;
      geomRenderer.strokeColor.set(1, 1, 1, 0.5);
      geomRenderer.fillColor.set(0, 0, 0, 0.75);
      std::string eventStr "Filename " + fileName "already"
      Vec2d eventStrSize = textRenderer.getDimensions(eventStr, 10);
      Vec2d size = eventStrSize + Vec2d(10, 0);
      Vec2d p = rect.pos+absolutePos - Vec2d(size.x*0.5, size.y + texture->w*0.5 + 5);
      //Vec2d p = rect.pos+absolutePos + Vec2d(-size.x*0.5, texture->w*0.5 + 3);
      
      geomRenderer.drawRectCorner(size.x, size.y, p.x, p.y);
      textRenderer.setColor(1, 1, 1, 0.93);
      textRenderer.print(eventStr, p.x+5, p.y+5, 10);
    }   */
  }
  

  /*static void loadActiveTrackFromFileX(FileSaver *fileSaver) {
    if(AudioFileIo::open(fileSaver->existingFilePaths[fileSaver->activeTrackIndex], fileSaver->audioRecordingTrack)) {
      fileSaver->fileLoadStatus = 2;
    }
    else {
      fileSaver->fileLoadStatus = -1;
    }
  }
  
  void loadActiveTrackFromFile() {
    if(activeTrackIndex >= 0 && activeTrackIndex < existingFilePaths.size() && fileLoadStatus <= 0) {
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
        loadingThreads[i].join();
      }
      loadingThreads.clear();
      audioRecordingTrack.toBeginning();
      audioRecordingTrack.onStart();
    
      if(fileLoadStatus == 2) {
        nowPlayingText = existingFileNames[activeTrackIndex];
        trackReady = true;
        fileLoadStatus = 0;
      }
      else {
        nowPlayingText = "";
        if(activeTrackIndex+1 < numTracks) {
          nextTrack();
        }
      }
    }
    
    if(fileLoadStatus == 0 && trackReady && activeTrackIndex >= 0 && activeTrackIndex < numTracks && isPlaying) {
      audioRecordingTrack.onUpdate(recordingPlaybackBuffer, recordingPlaybackBufferPosition);
      if(audioRecordingTrack.playbackFinished) {
        if(loopingMode == LoopTrack) {
          audioRecordingTrack.toBeginning();
          audioRecordingTrack.onStart();
        }
        else {
          nextTrack();
        }
      }
    }
  }*/
  

  
  bool checkFilenameExists() {
    doesFileNameExists = false;
    if(fileName.size() > 0 && fileToBeSaved) {
      for(int i=0; i<existingFileNames.size(); i++) {
        if(fileName == existingFileNames[i]) {
          doesFileNameExists = true;
          break;
        }
      }
    }
    if(fileSavingStatus == 0) {
      if(doesFileNameExists) {
        titleBar->setText("Overwrite file?");
      }
      else {
        titleBar->setText(defaultTitle);
      }
    }
    return doesFileNameExists;
  }
  
  void updateDirectory() {
    existingFilePaths = fileBrowser->directory.files;
    existingFileNames = fileBrowser->directory.fileNames;
    checkFilenameExists();
  }
  
  void updateFileName(const std::string &baseName) {
    if(baseName.size() > 0 && fileToBeSaved) {
      fileName = baseName;
      std::string targetExt = fileToBeSaved->getFileExtension();
      std::string fileExt = extractFileExtension(fileName);
      if(targetExt.size() > 0 && fileExt != targetExt) {
        fileName += "." + targetExt;
      }
    }
    checkFilenameExists();
  }
  
  struct FileBrowserListener : public GuiEventListener {
    FileSaver *fileSaver;
    FileBrowserListener(FileSaver *fileSaver) {
      this->fileSaver = fileSaver;
    }
    virtual void onMessage(GuiElement *guiElement, const std::string &msg) {
      if(msg == "dirChanged" && guiElement == fileSaver->fileBrowser) {
        fileSaver->updateDirectory();
      }
    }
    virtual void onAction(GuiElement *guiElement) {
      if(guiElement == fileSaver->fileBrowser->filesGui) {
        int index = -1;
        guiElement->getValue((void*)&index);
        if(index >= 0 && index < fileSaver->existingFileNames.size()) {
          fileSaver->updateFileName(fileSaver->existingFileNames[index]);
          fileSaver->filenameGui->setValue(fileSaver->fileName);
          fileSaver->checkFilenameExists();
        }
      }
    }
  };
  
  struct FileSaverListener : public GuiEventListener {
    FileSaver *fileSaver;
    FileSaverListener(FileSaver *fileSaver) {
      this->fileSaver = fileSaver;
    }
    virtual void onAction(GuiElement *guiElement) {
      if(guiElement == fileSaver->filenameGui) {
        fileSaver->save();
        fileSaver->fileBrowser->refreshDirectory();
      }
      if(guiElement == fileSaver->titleBar->closeGui) {
        fileSaver->closeRequested = true;
      }
    }
    virtual void onValueChange(GuiElement *guiElement) {
      if(guiElement == fileSaver->filenameGui) {
        std::string str;
        guiElement->getValue((void*)&str);
        fileSaver->updateFileName(str);
      }
    }
    virtual void onTextInput(GuiElement *guiElement, Events &events) {
      if(fileSaver->fileSavingStatus != 0) {
        fileSaver->fileSavingStatus = 0;
        fileSaver->titleBar->setText(fileSaver->defaultTitle);
      }
    }
  };
};
















