#pragma once
//#include "../gui/gui.h"
#include "panel.h"
#include "textbox.h"
//#include "button.h"
#include "synthtitlebar.h"
#include "../textfileparser.h"
#include "../util.h"


struct FileLoader : public Panel {
  FileBrowser *fileBrowser = NULL;
  //Label *currentlyPlayingTrackLabel = NULL;
  
  std::string defaultTitle;
  std::string fileName;
  
  SynthTitleBar *titleBar = NULL;
  TextBox *filenameGui = NULL;
  //Button *loadButton = NULL;
  
  std::vector<std::string> existingFilePaths;
  std::vector<std::string> existingFileNames;

  //std::thread savingThread;
  int fileLoadingStatus = 0;
  
  HierarchicalTextFileParser *whereToLoadFile = NULL;
  
  bool doesFileNameExists = false;
  
  std::function<void(HierarchicalTextFileParser *loadedFile)> onFileLoaded;
  
  virtual ~FileLoader() {}
  
  FileLoader(HierarchicalTextFileParser *whereToLoadFile) : Panel("File loader") {
    init(whereToLoadFile);
    onFileLoaded = [](HierarchicalTextFileParser *loadedFile){};
  }
  
  void init(HierarchicalTextFileParser *whereToLoadFile) {
    this->whereToLoadFile = whereToLoadFile;
    setSize(450, 50);
    double line = 0, lineHeight = 31;
    
    defaultTitle = "Load " + whereToLoadFile->getBlockName();
    
    titleBar = new SynthTitleBar(defaultTitle, this);
    titleBar->addCloseButton();
    line += titleBar->size.y + 5;
    
    addChildElement(filenameGui = new TextBox("Name", "", 5, line, 30));
    
    setSize(titleBar->size.x, line + lineHeight + 5);
    
    addChildElement(fileBrowser = new FileBrowser("", whereToLoadFile->getDefaultFileDirectory(), 450, 450, 0, size.y+5));

    addGuiEventListener(new FileLoaderListener(this));
    addGuiEventListener(new FileBrowserListener(this));

    updateDirectory();
  }
  
  void load() {
    if(fileName.size() > 0 && whereToLoadFile) {
      if(whereToLoadFile->open(fileBrowser->path + fileName)) {
        titleBar->setText("File loaded!");
        fileLoadingStatus = 1;
        printf("File '%s' loaded!\n", std::string(fileBrowser->path + fileName).c_str());
        sendMessage("fileLoaded");
      }
      else {
        titleBar->setText("Couldn't load file...");
        fileLoadingStatus = -1;
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
  

  /*static void loadActiveTrackFromFileX(FileLoader *fileLoader) {
    if(AudioFileIo::open(fileLoader->existingFilePaths[fileLoader->activeTrackIndex], fileLoader->audioRecordingTrack)) {
      fileLoader->fileLoadStatus = 2;
    }
    else {
      fileLoader->fileLoadStatus = -1;
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
    if(fileName.size() > 0 && whereToLoadFile) {
      for(int i=0; i<existingFileNames.size(); i++) {
        if(fileName == existingFileNames[i]) {
          doesFileNameExists = true;
          break;
        }
      }
    }
    /*if(doesFileNameExists) {
      titleBar->setText("Overwrite file?");
    }
    else {*/
      titleBar->setText(defaultTitle);
    //}
    return doesFileNameExists;
  }
  
  void updateDirectory() {
    existingFilePaths = fileBrowser->directory.files;
    existingFileNames = fileBrowser->directory.fileNames;
    checkFilenameExists();
  }
  
  void updateFileName(const std::string &baseName) {
    if(baseName.size() > 0 && whereToLoadFile) {
      fileName = baseName;
      std::string targetExt = whereToLoadFile->getFileExtension();
      std::string fileExt = extractFileExtension(fileName);
      if(targetExt.size() > 0 && fileExt != targetExt) {
        fileName += "." + targetExt;
      }
    }
    checkFilenameExists();
  }
  
  struct FileBrowserListener : public GuiEventListener {
    FileLoader *fileLoader;
    FileBrowserListener(FileLoader *fileLoader) {
      this->fileLoader = fileLoader;
    }
    virtual void onMessage(GuiElement *guiElement, const std::string &msg) {
      if(msg == "dirChanged" && guiElement == fileLoader->fileBrowser) {
        fileLoader->updateDirectory();
      }
    }
    virtual void onAction(GuiElement *guiElement) {
      if(guiElement == fileLoader->fileBrowser->filesGui) {
        int index = -1;
        guiElement->getValue((void*)&index);
        if(index >= 0 && index < fileLoader->existingFileNames.size()) {
          fileLoader->updateFileName(fileLoader->existingFileNames[index]);
          fileLoader->filenameGui->setValue(fileLoader->fileName);
          fileLoader->load();
        }
      }
    }
  };
  
  struct FileLoaderListener : public GuiEventListener {
    FileLoader *fileLoader;
    FileLoaderListener(FileLoader *fileLoader) {
      this->fileLoader = fileLoader;
    }
    virtual void onAction(GuiElement *guiElement) {
      if(guiElement == fileLoader->filenameGui) {
        fileLoader->load();
      }
      if(guiElement == fileLoader->titleBar->closeGui) {
        fileLoader->closeRequested = true;
      }
    }
    virtual void onValueChange(GuiElement *guiElement) {
      if(guiElement == fileLoader->filenameGui) {
        std::string str;
        guiElement->getValue((void*)&str);
        fileLoader->updateFileName(str);
      }
    }
    virtual void onTextInput(GuiElement *guiElement, Events &events) {
      if(fileLoader->fileLoadingStatus != 0) {
        fileLoader->fileLoadingStatus = 0;
        fileLoader->titleBar->setText(fileLoader->defaultTitle);
      }
    }
  };
};
















