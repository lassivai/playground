#pragma once
#include <SDL2/SDL.h>
#include "scene.h"
#include "message.h"
#include "command.h"
#include "util.h"
#include "mathl.h"
#include <cstdlib>

// SDL keycodes
// å 229
// ä 228
// ö 246
// * 39
// ½ 167


struct Events
{
  static const int dragModeLinear = 1,
                   dragModeExponential = 2,
                   dragModeToPoint = 3,
                   dragModeToPointNormCoords = 4,
                   wheelModeBasic = 5,
                   releaseModeToMouse = 6,
                   releaseModeToMouseNormCoords = 7,
                   releaseModeRoundNegate = 8,
                   releaseModeToZero = 9,
                   releaseModeToOne = 10;

  static const int modifiersNone = 0,
            modifiersLControl = 1,
            modifiersLShift = 2,
            modifiersLAlt = 4,
            modifiersRControl = 8,
            modifiersRShift = 16,
            modifiersRAlt = 32;

  static const int buttonLeft = 1,
            buttonMiddle = 2,
            buttonRight = 4;


  bool quitRequested = false;
  int mouseX = 0, mouseY = 0, pMouseX = 0, pMouseY = 0;
  Vec2d m, mp, dragStartM;
  Vec2d md;
  Vec2d mw, mwp;
  int mouseButton = -1;
  bool mouseNowDownL, mouseNowDownM, mouseNowDownR;
  bool mouseDownL, mouseDownM, mouseDownR;
  bool mouseUpL, mouseUpM, mouseUpR;
  int mouseWheel = 0;
  bool rControlDown = false, lControlDown = false, lShiftDown = false, rShiftDown = false, lAltDown = false, rAltDown = false;

  int sdlMouseButton = -1;

  bool noModifiers = true;
  int numModifiersDown = 0;


  /*std::string inputText = "";
  std::string inputTextComposition = "";

  int inputTextCursorPos = 0;
  int inputTextSelectionLength = 0;
  int inputTextStart = 0;*/

  Scene *scene = NULL;

  Matrix3d *cameraMatrixRotScale = NULL;
  Matrix3d *cameraMatrix = NULL;

  //int screenW, screenH;
  //double aspectRatio;

  void init(Scene *scene) {
    this->scene = scene;
    SDL_StopTextInput();
  }

  void startTextInput() {
    SDL_StartTextInput();
    textInput.inputGrabbed = true;
    /*inputText = "";
    inputTextComposition = "";
    //SDL_SetTextInputRect(NULL);*/
    //printf("text input started\n");
  }
  void endTextInput() {
    textInput.inputGrabbed = false;
    SDL_StopTextInput();
    /*inputText = "";
    inputTextComposition = "";
    */
    //printf("text input ended\n");
  }

  void toggleTextInput() {
    if(SDL_IsTextInputActive() == SDL_FALSE) {
      startTextInput();
    }
    else {
      endTextInput();
    }
  }

  struct TextInput {
    bool inputGrabbed = false;
    std::string inputText;
    int cursorPos = 0;
    bool printToTerminal = false;
    int suggestionCounter = 0;
    std::string tabSuggestionStr = "";
    int inputHistoryIndex = -1;

    bool isCommandPrompt = false;

    void print() {
      std::string start = inputText.substr(0, cursorPos);
      std::string end = inputText.substr(cursorPos);
      printf("> %s|%s\n", start.c_str(), end.c_str());
    }

    std::string getTextToCursor() const {
      return inputText.substr(0, cursorPos);
    }
    std::string getTextFromCursor() const {
      return inputText.substr(cursorPos);
    }

    void resetTabSuggestionStr() {
      if(tabSuggestionStr.size() > 0) {
        tabSuggestionStr = "";
      }
      suggestionCounter = 0;
    }

    void setInputText(std::string inputText) {
      this->inputText = inputText;
      cursorPos = inputText.size();
    }

    void setInputText(const std::string &inputText, const std::string &inputTextToCursor) {
      this->inputText = inputText;
      cursorPos = inputTextToCursor.size();
    }

    /*void printMBC(std::string mbc) {
      wchar_t wc;
      mbtowc(NULL, NULL, 0);
      size_t s = mbtowc(&wc, mbc.c_str(), 4);
      printf("%s, %d, %lu\n", mbc.c_str(), (int)to_wstring(mbc).c_str()[0], mbc.size());
      for(int i=0; i<mbc.size(); i++) {
        printf("%d", (int)mbc[i]);
        printf("%s", i<mbc.size()-1 ? ", " : "\n");
      }
      printf("%d, %lu\n", (int)wc, s);
    }*/

    void onTextInput(std::string inputText) {
      if(cursorPos > this->inputText.size()) {
        cursorPos = this->inputText.size();
      }
      this->inputText.insert(cursorPos, inputText);

//      printMBC(inputText);

      cursorPos += inputText.size();
      resetTabSuggestionStr();
      if(printToTerminal) print();

      //messageQueue.addMessage(toNumbers(inputText));
    }


    void onKeyDown(Events &e) {
      if(cursorPos > inputText.size()) {
        cursorPos = inputText.size();
      }

      if(isCommandPrompt) {
        if(e.sdlKeyCode == SDLK_UP || e.sdlKeyCode == SDLK_DOWN) {
          int n = commandQueue.inputHistory.size();
          if(n > 0) {
            inputHistoryIndex += e.sdlKeyCode == SDLK_UP ? -1 : 1;
            if(inputHistoryIndex < -1) inputHistoryIndex = n - 1;
            if(inputHistoryIndex >= n) inputHistoryIndex = -1;
            if(inputHistoryIndex == -1) inputText = "";
            else inputText = commandQueue.inputHistory[inputHistoryIndex];
            cursorPos = inputText.size();
            if(printToTerminal) print();
          }
        }
        if(e.sdlKeyCode == SDLK_RETURN) {
          commandQueue.addCommand(inputText);
          inputText = "";
          cursorPos = 0;
          resetTabSuggestionStr();
          inputHistoryIndex = -1;
        }

        if(e.sdlKeyCode == SDLK_TAB) {
          if(inputText.size() > 0) {
            if(tabSuggestionStr.size() == 0) {
              tabSuggestionStr = inputText;
            }

            std::vector<int> inds;
            for(int i=0; i<commandQueue.commandTemplates.size(); i++) {
              std::string str = commandQueue.commandTemplates[i]->commandName.substr(0, tabSuggestionStr.size());
              if(str.compare(tabSuggestionStr) == 0) {
                inds.push_back(i);
              }
            }
            if(inds.size() > 0) {
              if(suggestionCounter < inds.size()) {
                inputText = commandQueue.commandTemplates[inds[suggestionCounter]]->commandName;
              }
              else {
                inputText = tabSuggestionStr;
              }
              suggestionCounter++;
              if(suggestionCounter > inds.size()) suggestionCounter = 0;
              cursorPos = inputText.size();
            }
          }
        }

        if(e.sdlKeyCode == SDLK_p && (e.lControlDown || e.rControlDown)) {
          inputGrabbed = false;
          e.endTextInput();
          resetTabSuggestionStr();
        }
      }

      if(e.sdlKeyCode == SDLK_LEFT) {
        if(cursorPos > 0) {
          cursorPos--;
          resetTabSuggestionStr();
          if(printToTerminal) print();
        }
      }
      if(e.sdlKeyCode == SDLK_RIGHT) {
        if(cursorPos < inputText.size()) {
          cursorPos++;
          resetTabSuggestionStr();
          if(printToTerminal) print();
        }
      }
      if(e.sdlKeyCode == SDLK_HOME) {
        if(cursorPos != 0) {
          cursorPos = 0;
          resetTabSuggestionStr();
          if(printToTerminal) print();
        }
      }
      if(e.sdlKeyCode == SDLK_END) {
        if(cursorPos != inputText.size()) {
          cursorPos = inputText.size();
          resetTabSuggestionStr();
          if(printToTerminal) print();
        }
      }
      if(e.sdlKeyCode == SDLK_DELETE && (e.lControlDown || e.rControlDown)) {
        if(inputText.size() > 0 && cursorPos < inputText.size()) {
          inputText.erase(cursorPos);
          cursorPos = inputText.size();
          resetTabSuggestionStr();
          if(printToTerminal) print();
        }
      }
      if(e.sdlKeyCode == SDLK_BACKSPACE && (e.lControlDown || e.rControlDown)) {
        if(inputText.size() > 0 && cursorPos > 0) {
          inputText.erase(0, cursorPos);
          cursorPos = 0;
          resetTabSuggestionStr();
          if(printToTerminal) print();
        }
      }

      if(e.sdlKeyCode == SDLK_BACKSPACE) {
        if(cursorPos > 0) {
          inputText.erase(cursorPos-1, 1);
          cursorPos--;
          resetTabSuggestionStr();
          if(printToTerminal) print();
        }
      }


      if(e.sdlKeyCode == SDLK_DELETE) {
        if(cursorPos < inputText.size()) {
          inputText.erase(cursorPos, 1);
          resetTabSuggestionStr();
          if(printToTerminal) print();
        }
      }

      if(e.sdlKeyCode == SDLK_ESCAPE) {
        inputGrabbed = false;
        e.endTextInput();
        resetTabSuggestionStr();
      }


    }

  };

  template<class T>
  struct InputVariableBind
  {
    int mouseButtons;

    T *variable;
    T variableAtDragStart;
    int modifiers;

    int mouseWheelMode = 0;
    T wheelFactor;

    int mouseDragMode = 0;
    T dragFactor;

    int mouseReleaseMode = 0;

    double defaultScale = 1.0;
    double *scale = &defaultScale;

    bool active = true;

    bool dragging = false;
    bool dragged = false;

    int applyCameraMatrix = 0;

    InputVariableBind() {}

    InputVariableBind(T *variable, int mode, T factor, int mouseButtons, int modifiers) {
      bind(variable, mode, factor, mouseButtons, modifiers);
    }

    InputVariableBind(T *variable, double *scale, int mode, T factor, int mouseButtons, int modifiers) {
      bind(variable, scale, mode, factor, mouseButtons, modifiers);
    }

    InputVariableBind(T *variable, int dragMode, T factor, int releaseMode, int mouseButtons, int modifiers) {
      bind(variable, &defaultScale, dragMode, factor, releaseMode, mouseButtons, modifiers);
    }

    InputVariableBind(T *variable, double *scale, int dragMode, T factor, int releaseMode, int mouseButtons, int modifiers) {
      bind(variable, scale, dragMode, factor, releaseMode, mouseButtons, modifiers);
    }

    virtual ~InputVariableBind() {}

    void bind(T *variable, int mode, T factor, int mouseButtons, int modifiers) {
      this->variable = variable;
      if(mode > 0 && mode < 5) {
        this->mouseDragMode = mode;
        dragFactor = factor;
      }

      this->modifiers = modifiers;
      this->mouseButtons = mouseButtons;
    }

    void bind(T *variable, double *scale, int mode, T factor, int mouseButtons, int modifiers) {
      this->variable = variable;
      if(mode > 0 && mode < 5) {
        this->mouseDragMode = mode;
        dragFactor = factor;
      }

      this->modifiers = modifiers;
      this->mouseButtons = mouseButtons;
      this->scale = scale;
    }

    void bind(T *variable, double *scale, int dragMode, T factor, int releaseMode, int mouseButtons, int modifiers) {
      this->variable = variable;
      if(dragMode > 0 && dragMode < 5) {
        this->mouseDragMode = dragMode;
        dragFactor = factor;
      }
      this->mouseReleaseMode = releaseMode;
      this->modifiers = modifiers;
      this->mouseButtons = mouseButtons;
      this->scale = scale;
    }

    virtual void preAction() {}
    virtual void postAction() {}

    void mouseWheel(Events &e) {
      if(!active) return;
      preAction();

      if(mouseWheelMode == wheelModeBasic) {
        *variable += wheelFactor * e.mouseWheel;
        onMouseWheel();
        onValueChanged();
      }

      postAction();
    }

    void mousePressed(Events &e) {
      if(!active) return;
      preAction();

      if(!(((mouseButtons & buttonLeft) == buttonLeft) != e.mouseDownL) &&
         !(((mouseButtons & buttonMiddle) == buttonMiddle) != e.mouseDownM) &&
         !(((mouseButtons & buttonRight) == buttonRight) != e.mouseDownR) &&
         !(((modifiers & modifiersRAlt) == modifiersRAlt) != e.rAltDown) &&
         !(((modifiers & modifiersRShift) == modifiersRShift) != e.rShiftDown) &&
         !(((modifiers & modifiersRControl) == modifiersRControl) != e.rControlDown) &&
         !(((modifiers & modifiersLAlt) == modifiersLAlt) != e.lAltDown) &&
         !(((modifiers & modifiersLShift) == modifiersLShift) != e.lShiftDown) &&
         !(((modifiers & modifiersLControl) == modifiersLControl) != e.lControlDown)) {

        variableAtDragStart = *variable;
        dragging = true;
        dragged = false;
        onMousePressed();
        postAction();
      }
    }


    bool setToMouse(Events &e, double &variable) {
      double d = e.mouseX - e.scene->screenW*0.5;
      if(d != variable) {
        variable = d;
        return true;
      }
      return false;
    }
    bool setToMouse(Events &e, Vec2d &variable) {
      Vec2d translate(e.mouseX - e.scene->screenW*0.5, e.mouseY - e.scene->screenH*0.5);
      if(variable != translate) {
        variable.set(translate);
        return true;
      }
      return false;
    }
    bool setToMouseNormCoords(Events &e, double &variable) {
      double v = (*scale) * map(e.mouseX, 0.0, e.scene->screenW-1.0, -e.scene->aspectRatio, e.scene->aspectRatio);
      if(variable != v) {
        variable = v;
        return true;
      }
      return false;
    }
    bool setToMouseNormCoords(Events &e, Vec2d &variable) {
      Vec2d translate((*scale)*map(e.mouseX, 0.0, e.scene->screenW-1.0, -e.scene->aspectRatio, e.scene->aspectRatio),
                     -(*scale)*map(e.mouseY, 0.0, e.scene->screenH-1.0, -1.0, 1.0));
      if(variable != translate) {
        variable.set(translate);
        return true;
      }
      return false;
    }

    bool dragLinear(Events &e, double &variable) {
      double dx = (e.mouseX - e.pMouseX) * dragFactor * (*scale);

      if(dx != 0) {
        variable += dx;
        return true;
      }
      return false;
    }
    bool dragExponential(Events &e, double &variable) {
      int dx = e.mouseX - e.dragStartM.x;
      if(dx > 0) {
        variable = variableAtDragStart * (1.0 + dx * dragFactor * (*scale));
        return true;
      }
      if(dx < 0) {
        variable = variableAtDragStart / (1.0 - dx * dragFactor * (*scale));
        return true;
      }
      return false;
    }

    bool dragLinear(Events &e, Vec2d &variable) {
      Vec2d d = e.m - e.mp;
      //Vec2d d = e.scene->toWorld(e.m) - e.scene->toWorld(e.mp);

      if(applyCameraMatrix == 1 && e.cameraMatrixRotScale) {
        d = (*e.cameraMatrixRotScale) * d;
      }
      if(applyCameraMatrix == 2 && e.cameraMatrix) {
        d = (*e.cameraMatrix) * d;
      }

      double dx = d.x * (dragFactor.x * (*scale));
      double dy = d.y * (dragFactor.y * (*scale));
      
      if(dx != 0 || dy != 0) {
        variable.x += dx;
        variable.y += dy;
        return true;
      }
      return false;
    }
    bool dragExponential(Events &e, Vec2d &variable) {
      Vec2d d = e.m - e.dragStartM;
      
      if(d.x > 0) {
        double x = variableAtDragStart.x / (1.0 + d.x * (dragFactor.x * (*scale)));
        if(x != variable.x) {
          variable.x = x;
          return true;
        }
      }
      if(d.x < 0) {
        double x = variableAtDragStart.x * (1.0 - d.x * (dragFactor.x * (*scale)));
        if(x != variable.x) {
          variable.x = x;
          return true;
        }
      }
      if(d.y > 0) {
        double y = variableAtDragStart.y / (1.0 + d.y * (dragFactor.y * (*scale)));
        if(y != variable.y) {
          variable.y = y;
          return true;
        }
      }
      if(d.y < 0) {
        double y = variableAtDragStart.y * (1.0 - d.y * (dragFactor.y * (*scale)));
        if(y != variable.y) {
          variable.y = y;
          return true;
        }
      }
      return false;
    }


    void mouseMotion(Events &e) {
      if(!active || !dragging) return;
      dragged = true;

      bool changed = false;

      if(mouseDragMode == dragModeLinear) {
        changed = dragLinear(e, *variable);
      }
      if(mouseDragMode == dragModeExponential) {
        changed = dragExponential(e, *variable);
      }
      if(mouseDragMode == dragModeToPoint) {
        changed = setToMouse(e, *variable);
      }
      if(mouseDragMode == dragModeToPointNormCoords) {
        changed = setToMouseNormCoords(e, *variable);
      }

      onMouseMotion();
      if(changed) {
        onValueChanged();
      }
      postAction();
    }



    bool toMouse(Events &e, double &variable) {
      double d = e.mouseX - e.scene->screenW*0.5;
      if(d != 0) {
        variable += d;
        return true;
      }
      return false;
    }
    bool toMouse(Events &e, Vec2d &variable) {
      Vec2d translate(e.mouseX - e.scene->screenW*0.5, e.mouseY - e.scene->screenH*0.5);
      if(translate.x != 0 || translate.y != 0) {
        variable.add(translate);
        return true;
      }
      return false;
    }
    bool toMouseNormCoords(Events &e, double &variable) {
      double d = (*scale) * map(e.mouseX, 0.0, e.scene->screenW-1, -e.scene->aspectRatio, e.scene->aspectRatio);
      if(d != 0) {
        variable += d;
        return true;
      }
      return false;
    }
    bool toMouseNormCoords(Events &e, Vec2d &variable) {
      Vec2d translate((*scale)*map(e.mouseX, 0.0, e.scene->screenW-1.0, -e.scene->aspectRatio, e.scene->aspectRatio),
                     -(*scale)*map(e.mouseY, 0.0, e.scene->screenH-1.0, -1.0, 1.0));
      if(translate.x != 0 || translate.y != 0) {
        variable.add(translate);
        return true;
      }
      return false;
    }

    bool roundNegate(Events &e, double &variable) {
      if(fract(variable) == 0.0) {
        if(variable != 0) {
          variable = -variable;
          return true;
        }
      }
      else {
        double v = round(variable);
        if(v != variable) {
          variable = v;
          return true;
        }
      }
      return false;
    }

    bool roundNegate(Events &e, Vec2d &variable) {
      if(fract(variable.x) == 0.0) {
        if(variable .x != 0) {
          variable.x = -variable.x;
          return true;
        }
      }
      else {
        double v = round(variable.x);
        if(v != variable.x) {
          variable.x = v;
          return true;
        }
      }
      if(fract(variable.y) == 0.0) {
        if(variable .y != 0) {
          variable.y = -variable.y;
          return true;
        }
      }
      else {
        double v = round(variable.y);
        if(v != variable.y) {
          variable.y = v;
          return true;
        }
      }
      return false;
    }

    void mouseReleased(Events &e) {
      if(!active) return;
      
      bool changed = false;
      
      if(dragging && !dragged) {
        if(mouseReleaseMode == releaseModeToMouse) {
          changed = toMouse(e, *variable);
        }
        if(mouseReleaseMode == releaseModeToMouseNormCoords) {
          changed = toMouseNormCoords(e, *variable);
        }
        if(mouseReleaseMode == releaseModeRoundNegate) {
          changed = roundNegate(e, *variable);
        }
        if(mouseReleaseMode == releaseModeToZero) {
          changed = setToZero(*variable);
        }
        if(mouseReleaseMode == releaseModeToOne) {
          changed = setToOne(*variable);
        }
      }
      dragging = false;

      onMouseReleased();
      if(changed) {
        onValueChanged();
      }

      postAction();
    }
    
    bool setToZero(double &variable) {
      if(variable != 0) {
        variable = 0;
        return true;
      }
      return false;
    }
    bool setToZero(Vec2d &variable) {
      if(variable.x != 0 || variable.y != 0) {
        variable.set(0, 0);
        return true;
      }
      return false;
    }
    bool setToOne(double &variable) {
      if(variable != 1) {
        variable = 1;
        return true;
      }
      return false;
    }
    bool setToOne(Vec2d &variable) {
      if(variable.x != 1 || variable.y != 1) {
        variable.set(1, 1);
        return true;
      }
      return false;
    }
    
    virtual void onMouseWheel() {}
    virtual void onMouseMotion() {}
    virtual void onMousePressed() {}
    virtual void onMouseReleased() {}
    virtual void onValueChanged() {}

  };

  //std::vector<InputVariableBindBase> inputBoundVariables;
  std::vector<InputVariableBind<double>*> inputBoundDoubles;
  std::vector<InputVariableBind<Vec2d>*> inputBoundVec2ds;

  SDL_Keycode sdlKeyCode;

  TextInput textInput;

};



static int keyboardPianoInput(int keyCode) {
  int n = -1000;
  if(keyCode == 167) n = -1+7;
  if(keyCode == SDLK_TAB) n = 0+7;
  if(keyCode == SDLK_1) n = 1+7;
  if(keyCode == SDLK_q) n = 2+7;
  if(keyCode == SDLK_2) n = 3+7;
  if(keyCode == SDLK_w) n = 4+7;
  if(keyCode == SDLK_e) n = 5+7;
  if(keyCode == SDLK_4) n = 6+7;
  if(keyCode == SDLK_r) n = 7+7;
  if(keyCode == SDLK_5) n = 8+7;
  if(keyCode == SDLK_t) n = 9+7;
  if(keyCode == SDLK_y) n = 10+7;
  if(keyCode == SDLK_7) n = 11+7;
  if(keyCode == SDLK_u) n = 12+7;
  if(keyCode == SDLK_8) n = 13+7;
  if(keyCode == SDLK_i) n = 14+7;
  if(keyCode == SDLK_9) n = 15+7;
  if(keyCode == SDLK_o) n = 16+7;
  if(keyCode == SDLK_p) n = 17+7;
  if(keyCode == 43) n = 18+7;
  if(keyCode == 229) n = 19+7;

  if(keyCode == SDLK_LSHIFT) n = -2-12;
  if(keyCode == SDLK_CAPSLOCK) n = -1-12;
  if(keyCode == 60) n = 0-12;
  if(keyCode == SDLK_a) n = 1-12;
  if(keyCode == SDLK_z) n = 2-12;
  if(keyCode == SDLK_s) n = 3-12;
  if(keyCode == SDLK_x) n = 4-12;
  if(keyCode == SDLK_c) n = 5-12;
  if(keyCode == SDLK_f) n = 6-12;
  if(keyCode == SDLK_v) n = 7-12;
  if(keyCode == SDLK_g) n = 8-12;
  if(keyCode == SDLK_b) n = 9-12;
  if(keyCode == SDLK_h) n = 10-12;
  if(keyCode == SDLK_n) n = 11-12;
  if(keyCode == SDLK_m) n = 12-12;
  if(keyCode == SDLK_k) n = 13-12;
  if(keyCode == 44) n = 14-12;
  if(keyCode == SDLK_l) n = 15-12;
  if(keyCode == 46) n = 16-12;
  if(keyCode == 45) n = 17-12;
  if(keyCode == 228) n = 18-12;
  if(keyCode == SDLK_RSHIFT) n = 19-12;

  if(n == -1000) return -1;
  else return 60 + n;
  //simpleSynth->startNote(n-12, time-simpleSynth->timeDifferencePaSdl);
}
