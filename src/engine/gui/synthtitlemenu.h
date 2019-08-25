#pragma once

#include "menu.h"
#include "filesaver.h"
#include "fileloader.h"
#include "../textfileparser.h"
//#include "../synth/instrument.h"


struct SynthTitleMenu : public Menu
{
  FileSaver *fileSaver = NULL;
  FileLoader *fileLoader = NULL;
  HierarchicalTextFileParser *fileToBeSaved = NULL, *whereToLoadFile = NULL;
  GuiEventListener *fileLoaderListener = NULL; // FIXME
  MenuButton *layoutGui = NULL;
  MenuButton *saveGui = NULL;
  MenuButton *loadGui = NULL;


  virtual ~SynthTitleMenu() {} 
  
  
  virtual void onUpdate(double time, double dt) {
    if(fileSaver && fileSaver->closeRequested) {
      parent->deleteChildElement(fileSaver);
      fileSaver = NULL;
      //saveGui->setValue(false);
    }
    if(fileLoader && fileLoader->closeRequested) {
      parent->deleteChildElement(fileLoader);
      fileLoader = NULL;
      //saveGui->setValue(false);
    }
  }

  
  SynthTitleMenu(const std::string &name) : Menu(name) {
    addGuiEventListener(new SynthTitleMenuListener(this));
  }
  
  void addLayoutMenuItem() {
    layoutGui = addMenuButton("Layout");
  }
  void addSaveMenuItem(HierarchicalTextFileParser *fileToBeSaved) {
    this->fileToBeSaved = fileToBeSaved;
    saveGui = addMenuButton("Save");
  }
  void addLoadMenuItem(HierarchicalTextFileParser *whereToLoadFile) {
    this->whereToLoadFile = whereToLoadFile;
    loadGui = addMenuButton("Load");
  }

  struct SynthTitleMenuListener : public GuiEventListener {
    SynthTitleMenu *synthTitleMenu = NULL;
    SynthTitleMenuListener(SynthTitleMenu *synthTitleMenu) : synthTitleMenu(synthTitleMenu) {}
    virtual void onAction(GuiElement *guiElement) {
      if(guiElement == synthTitleMenu->saveGui) {
        if(!synthTitleMenu->fileSaver) {
          synthTitleMenu->parent->addChildElement(synthTitleMenu->fileSaver = new FileSaver(synthTitleMenu->fileToBeSaved));
          synthTitleMenu->fileSaver->setPosition(synthTitleMenu->parent->size.x + 5, 20);
          //synthTitleMenu->fileSaver->setAbsolutePosition(GuiElement::screenW*0.5 - synthTitleMenu->fileSaver->size.x*0.5 - 10,
          //                                               GuiElement::screenH*0.5 - synthTitleMenu->fileSaver->size.y*0.5 - 10);

          /*if(synthTitleMenu->fileLoader) {
            synthTitleMenu->fileSaver->setPosition(synthTitleMenu->fileSaver->pos.x, synthTitleMenu->fileSaver->pos.y + synthTitleMenu->fileLoader->pos.y + synthTitleMenu->fileLoader->size.y +5);
          }*/

        }
        synthTitleMenu->closeRequested = true;
      }
      if(guiElement == synthTitleMenu->loadGui) {
        if(!synthTitleMenu->fileLoader) {
          synthTitleMenu->parent->addChildElement(synthTitleMenu->fileLoader = new FileLoader(synthTitleMenu->whereToLoadFile));
          synthTitleMenu->fileLoader->setPosition(synthTitleMenu->parent->size.x + 25, 40);
          //synthTitleMenu->fileLoader->setAbsolutePosition(GuiElement::screenW*0.5 - synthTitleMenu->fileLoader->size.x*0.5 + 10,
          //                                                GuiElement::screenH*0.5 - synthTitleMenu->fileLoader->size.y*0.5 + 10);

          
          /*if(synthTitleMenu->fileSaver) {
            synthTitleMenu->fileLoader->setPosition(synthTitleMenu->fileLoader->pos.x, synthTitleMenu->fileLoader->pos.y + synthTitleMenu->fileSaver->pos.y + synthTitleMenu->fileSaver->size.y +5);
          }*/
          
           // FIXME
          if(synthTitleMenu->fileLoaderListener) {
            synthTitleMenu->fileLoader->addGuiEventListener(synthTitleMenu->fileLoaderListener);
          }
          /*synthTitleMenu->fileLoader->onFileLoaded = [](HierarchicalTextFileParser *loadedFile) {
            Instrument *instrument = NULL;
            if(instrument = dynamic_cast<Instrument*>(loadedFile)) {
              instrument->reinitPanel();
            }
          };*/
        }
        synthTitleMenu->closeRequested = true;
      }
      if(guiElement == synthTitleMenu->layoutGui) {
        //synthTitleMenu->closeRequested = true;
      }
    }
  };

};
