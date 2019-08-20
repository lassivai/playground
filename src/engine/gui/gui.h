#pragma once

#include "guielement.h"
#include "synthguilayout.h"

#include "panel.h"
#include "label.h"
#include "checkbox.h"
#include "textbox.h"
#include "numberbox.h"
#include "colorbox.h"
#include "listbox.h"
#include "button.h"
#include "rotaryknob.h"
#include "titlebar.h"

#include "rangebox.h"
#include "graphpanel.h"
#include "fastnoisepanel.h"
#include "scrollablelist.h"
#include "menu.h"

#include "filebrowser.h"
#include "synthtitlebar.h"
#include "synthtitlemenu.h"
#include "filesaver.h"
#include "fileloader.h"

#include "synthtitlebarext.h"



struct PanelInterface {
  Vec2d previousPanelPosition;

  virtual ~PanelInterface() {}
  virtual Panel *addPanel(GuiElement *parentElement, const std::string &title = "") = 0;
  virtual void removePanel(GuiElement *parentElement) {
    if(getPanel()) {
      previousPanelPosition = getPanel()->pos;
    }
  }
  virtual void updatePanel() = 0;
  virtual Panel *getPanel() = 0;

  bool setPreviousPanelPosition() {
    if(getPanel()) {
      getPanel()->pos = previousPanelPosition;
      return true;
    }
    return false;
  }

};
