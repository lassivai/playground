#pragma once
#include <SDL2/SDL.h>
#include <string>
#include <cstdio>
#include "textgl.h"

struct Console {
  TextGl *textRenderer;
  int textSizePrompt = 12, textSizeMessages = 10;
  //std::vector<std::string> lines;
  Texture canvasImg, minimalCanvasImg;
  bool active = false;
  bool minimalInterface = false;
  double lineHeight = 0;

  int scroll = 0;

  bool inputGrabbed = false;

  std::string inputText, inputTextToCursor;


  void onTextInput(const Events &events) {
    this->inputText = events.textInput.inputText;
    this->inputTextToCursor = events.textInput.getTextToCursor();
  }

  void onMouseWheel(Events &events) {
    scroll += events.mouseWheel;
    scroll = clamp(scroll, 0, (int)messageQueue.permQueue.size()-1);
  }


  void init(TextGl *textRenderer, SDLInterface *sdlInterface) {
    canvasImg.create(sdlInterface->screenW, sdlInterface->screenH, true);

    lineHeight = textRenderer->getLineHeight(textSizePrompt);
    minimalCanvasImg.create(sdlInterface->screenW, lineHeight+4, true);
    this->textRenderer = textRenderer;
  }

  void preRender(SDLInterface *sdlInterface) {
    if(minimalInterface) {
      minimalCanvasImg.setRenderTarget();
      clear(0, 0, 0, 0.5, sdlInterface);

      setColor(1, 1, 1, 0.75, sdlInterface);
      drawLine(0, 0, sdlInterface->screenW-1, 0, sdlInterface);

      double cursorX = 10 + textRenderer->getDimensions("> "+inputTextToCursor, textSizePrompt).x;

      if((SDL_GetTicks() / 500) % 2 == 0) {
        setColor(1, 1, 1, 0.75, sdlInterface);
        drawLine(cursorX, 7, cursorX, minimalCanvasImg.h - 3, sdlInterface);
      }

      textRenderer->print("> " + inputText, 10, 7, textSizePrompt, sdlInterface);


      minimalCanvasImg.unsetRenderTarget();
      /*minimalCanvasImg.renderFlipY(0, sdlInterface->screenH - 1 - minimalCanvasImg.h);

      setColor(1, 1, 1, 0.75, sdlInterface);
      double y = sdlInterface->screenH - lineHeight*1.0 - 3;
      for(int i=0; i<messageQueue.tmpQueue.size(); i++) {
        std::wstring &str = messageQueue.tmpQueue[messageQueue.tmpQueue.size()-1 - i]->str;
        Vec2d dim = textRenderer->getDimensions(str, textSizeMessages, sdlInterface);
        y -= dim.y;
        textRenderer->print(str, 10, y, textSizeMessages, sdlInterface);
        if(y + lineHeight < 0) break;
      }*/
    }
    else {
      canvasImg.setRenderTarget();
      clear(0, 0, 0, 0.5, sdlInterface);

      setColor(1, 1, 1, 0.75, sdlInterface);
      drawLine(0, sdlInterface->screenH-1-lineHeight-4, sdlInterface->screenW-1, sdlInterface->screenH-1-lineHeight-4, sdlInterface);

      double cursorX = 10 + textRenderer->getDimensions("> "+inputTextToCursor, textSizePrompt).x;

      if((SDL_GetTicks() / 500) % 2 == 0) {
        setColor(1, 1, 1, 0.75, sdlInterface);
        drawLine(cursorX, sdlInterface->screenH-1 - lineHeight + 3, cursorX, sdlInterface->screenH-1 - 3, sdlInterface);
      }


      textRenderer->print("> " + inputText, 10, sdlInterface->screenH-1-lineHeight+3, textSizePrompt, sdlInterface);


      setColor(1, 1, 1, 0.75, sdlInterface);
      double y = sdlInterface->screenH - lineHeight*1.0 - 3;

      for(int i=scroll; i<messageQueue.permQueue.size(); i++) {
        std::wstring &str = messageQueue.permQueue[messageQueue.permQueue.size()-1 - i]->str;
        Vec2d dim = textRenderer->getDimensions(str, textSizeMessages, sdlInterface);
        y -= dim.y;
        textRenderer->print(str, 10, y, textSizeMessages, sdlInterface);
        if(y + lineHeight < 0) break;
      }

      canvasImg.unsetRenderTarget();
      //canvasImg.render();
    }
  }
  
  void render(SDLInterface *sdlInterface) {
    if(minimalInterface) {
      minimalCanvasImg.renderFlipY(0, sdlInterface->screenH - 1 - minimalCanvasImg.h);

      setColor(1, 1, 1, 0.75, sdlInterface);
      double y = sdlInterface->screenH - lineHeight*1.0 - 3;
      for(int i=0; i<messageQueue.tmpQueue.size(); i++) {
        std::wstring &str = messageQueue.tmpQueue[messageQueue.tmpQueue.size()-1 - i]->str;
        Vec2d dim = textRenderer->getDimensions(str, textSizeMessages, sdlInterface);
        y -= dim.y;
        textRenderer->print(str, 10, y, textSizeMessages, sdlInterface);
        if(y + lineHeight < 0) break;
      }
    }
    else {
      canvasImg.render();
    }
  }

};
