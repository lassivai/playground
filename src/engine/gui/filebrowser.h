#pragma once
#include <string>
#include <vector>
#include "../util.h"
#include "scrollablelist.h"
#include "label.h"
#include "textbox.h"
#include "panel.h"

struct FileBrowser : public Panel
{
  Directory directory;
  ScrollableList *directoriesGui = NULL, *filesGui = NULL;
  Label *label = NULL;
  TextBox *currentPathGui = NULL;
  std::string path = "";

  bool showHiddenFiles = false;
  
  struct ScrollableListState {
    int scrollPosition = 0;
    int activeItemIndex = 0;
    ScrollableListState(int scrollPosition = 0, int activeItemIndex = 0) {
      this->scrollPosition = scrollPosition;
      this->activeItemIndex = activeItemIndex;
    }
    ScrollableListState(ScrollableList *scrollableList) {
      this->scrollPosition = scrollableList->scrollPosition;
      this->activeItemIndex = scrollableList->activeItemIndex;
    }
  };
  
  std::vector<ScrollableListState> directoryStateStack;
  ScrollableListState directoryState;  
  
  void pushDirectoryState() {
    //printf("pushDirectoryState, %lu\n", directoryStateStack.size());
    //directoryStateStack.push_back(ScrollableListState(directoriesGui));
    directoryStateStack.push_back(directoryState);
    //printf("> %d, %d, %d, %d\n", directoryState.scrollPosition, directoryState.activeItemIndex, directoryStateStack[directoryStateStack.size()-1].scrollPosition, directoryStateStack[directoryStateStack.size()-1].activeItemIndex);
    
  }
  void popDirectoryState() {
    //printf("popDirectoryState, %lu\n", directoryStateStack.size());
    if(directoryStateStack.size() > 0) {
      //printf("< %d, %d\n", directoryStateStack[directoryStateStack.size()-1].scrollPosition, directoryStateStack[directoryStateStack.size()-1].activeItemIndex);
      directoryState.scrollPosition = directoriesGui->scrollPosition = directoryStateStack[directoryStateStack.size()-1].scrollPosition;
      directoryState.activeItemIndex = directoriesGui->activeItemIndex = directoryStateStack[directoryStateStack.size()-1].activeItemIndex;
      directoryStateStack.erase(directoryStateStack.begin()+directoryStateStack.size()-1);
      directoriesGui->prerenderingNeeded = true;
    }
  }
  
  FileBrowser(const std::string &name, const std::string &path, double w, double h, double x, double y) : Panel(name, w, h, x, y) {
    double line = 6-23, lineHeight = 23;
    
    if(name.size() > 0) {
      label = new Label(name, 10, line+=lineHeight);
    }
    addChildElement(currentPathGui = new TextBox("Path", path, 6, line+=lineHeight, 38));
    currentPathGui->loseFocusOnAction = false;
    currentPathGui->inputTextSize = 10;
    line += lineHeight;
    addChildElement(directoriesGui = new ScrollableList("", w*0.5 - 9, h-line - 6, 6, line));
    addChildElement(filesGui = new ScrollableList("", w*0.5 - 9, h-line - 6, 6+6+w*0.5-9, line));
    
    setPath(path);
    
    addGuiEventListener(new FileBrowserListener(this));
    //pushDirectoryState();
  }
  
  void refreshDirectory() {
    setPath(path);
  }
  
  void setPath(int index) {
    if(index >= 0 && index < directory.directoryNames.size()) {
      if(directory.directoryNames[index] == "..") {
        if(path == "/") return;
        setPath(path.substr(0, path.find_last_of('/', path.size()-2)));
      }
      else {
        setPath(path + directory.directoryNames[index]);
      }
    }
  }
  
  void setPath(std::string path) {
    //printf("p: %s\n", path.c_str());
    if(path == "..") {
      if(this->path == "/") return;
       path = this->path.substr(0, this->path.find_last_of('/', this->path.size()-2));
        //printf(".., %s\n", path.c_str());
    }
    if(path.size() == 0) {
      path = "/";
    }
    if(isDirectory(path)) {
      if(path[path.size()-1] != '/') {
        this->path = path + "/";
      }
      else {
        this->path = path;
      }
      
      directory.getContent(this->path, false);
      if(this->path != "/") {
        directory.directoryNames.insert(directory.directoryNames.begin(), "..");
      }
      directoriesGui->setItems(directory.directoryNames);
      filesGui->setItems(directory.fileNames);
      currentPathGui->setValue(this->path);
      
      
      sendMessage("dirChanged");
    }
  }
  
  void onKeyDown(Events &events) {
    GuiElement::onKeyDown(events);
    if(events.sdlKeyCode == SDLK_RETURN) {
      if(directoriesGui->isInputGrabbed) {
        directoriesGui->setInputGrabbed(false);
        filesGui->setInputGrabbed(true);
      }
      else if(filesGui->isInputGrabbed) {
        directoriesGui->setInputGrabbed(true);
        filesGui->setInputGrabbed(false);
      }
    }
  }
  void resetViews() {
    directoriesGui->scrollPosition = 0;
    directoriesGui->activeItemIndex = 0;
    filesGui->scrollPosition = 0;
    filesGui->activeItemIndex = 0;
  }
  
  struct FileBrowserListener : public GuiEventListener {
    FileBrowser *fileBrowser;
    FileBrowserListener(FileBrowser *fileBrowser) {
      this->fileBrowser = fileBrowser;
    }
    virtual void onAction(GuiElement *guiElement) {
      if(guiElement == fileBrowser->currentPathGui) {
        std::string path;
        guiElement->getValue((void*)&path);
        fileBrowser->setPath(path);
        fileBrowser->onAction();
        fileBrowser->resetViews();
      }
      
      if(guiElement == fileBrowser->directoriesGui) {
        int index = -1;
        guiElement->getValue((void*)&index);
        if(index >= 0) {
          if(fileBrowser->directory.directoryNames[index] == "..") {
            fileBrowser->popDirectoryState();
          }
          else {
            fileBrowser->pushDirectoryState();
            fileBrowser->resetViews();
          }
        }
        
        fileBrowser->setPath(index);
        fileBrowser->onAction();
      }
    }
    
    virtual void onValueChange(GuiElement *guiElement) {
      if(guiElement == fileBrowser->directoriesGui) {
        fileBrowser->directoryState.scrollPosition = fileBrowser->directoriesGui->scrollPosition;
        fileBrowser->directoryState.activeItemIndex = fileBrowser->directoriesGui->activeItemIndex;
      }
    }
    
    virtual void onKeyDown(GuiElement *guiElement, Events &events) {
      if(guiElement == fileBrowser->directoriesGui) {
        if(events.sdlKeyCode == SDLK_LEFT) {
          fileBrowser->setPath("..");
          fileBrowser->popDirectoryState();
        }
      }
    }
  };
  
};