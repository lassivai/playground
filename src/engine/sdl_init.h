#pragma once
#include <SDL2/SDL.h>
#include "mathl.h"

struct SDLInterface
{
  SDL_Window *window = NULL;
  SDL_Renderer *renderer = NULL;
  SDL_DisplayMode currentDisplayMode;
  SDL_GLContext glcontext;
  int screenW = 0, screenH = 0, screenS = 0;
  double aspectRatio = 0;

  double pixelWidthMeters, pixelHeightMeters;

  ~SDLInterface() {
    if(window) SDL_DestroyWindow(window);
    if(renderer) SDL_DestroyRenderer(renderer);
  }

  void setVsync(bool isVsync) {
    SDL_GL_SetSwapInterval(isVsync ? 1 : 0);
  }

  bool isVsync() {
    return SDL_GL_GetSwapInterval() != 0;
  }
  void setup(int w, int h, int fullscreen, int opengl,
                         int noBorder, int resizable, int inputGrabbed, int alwaysOnTop)
  {

    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
      printf("SDL_Init failed: %s\n", SDL_GetError());
    }
    SDL_version comp, link;
    SDL_VERSION(&comp);
    SDL_GetVersion(&link);
    printf("SDL version %d.%d.%d (if dynamically linked %d.%d.%d)\n",
        comp.major, comp.minor, comp.patch, link.major, link.minor, link.patch);
        
      
    /*printf("Available video drivers: ");
    for(int i=0; i<SDL_GetNumVideoDrivers(); i++) {
      printf("%s", SDL_GetVideoDriver(i));
      if(i < SDL_GetNumVideoDrivers()-1) {
        printf(", ");
      }
    }
    printf("\n");*/

    //SDL_EnableUNICODE(1);

    if(fullscreen == 2) {
      SDL_DisplayMode dm;
      if(SDL_GetCurrentDisplayMode(0, &dm) < 0) {
        printf("SDL_GetCurrentDisplayMode failed: %s\n", SDL_GetError());
      }
      else {
        w = dm.w;
        h = dm.h;
      }
    }

    Uint32 windowFlags = 0;
    if(fullscreen == 1) windowFlags = windowFlags | SDL_WINDOW_FULLSCREEN;
    if(fullscreen == 2) windowFlags = windowFlags | SDL_WINDOW_FULLSCREEN_DESKTOP;
    if(opengl == 1) windowFlags = windowFlags | SDL_WINDOW_OPENGL;
    if(noBorder == 1) windowFlags = windowFlags | SDL_WINDOW_BORDERLESS;
    if(resizable == 1) windowFlags = windowFlags | SDL_WINDOW_RESIZABLE;
    if(inputGrabbed == 1) windowFlags = windowFlags | SDL_WINDOW_INPUT_GRABBED;
    //if(alwaysOnTop == 1) windowFlags = windowFlags | SDL_WINDOW_ALWAYS_ON_TOP;


    this->window = SDL_CreateWindow("C++ sketches",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        w, h, windowFlags);
    if(this->window == NULL) {
      printf("SDL_CreateWindow failed: %s\n", SDL_GetError());
    }
    this->renderer = SDL_CreateRenderer(this->window, -1,
      SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
      /*|SDL_RENDERER_TARGETTEXTURE */ );

    //SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 0);

    if(opengl == 1) {
      this->glcontext = SDL_GL_CreateContext(this->window);
      if(this->glcontext == NULL) {
        printf("SDL_GL_CreateContext failed: %s\n", SDL_GetError());
      }
      if(SDL_GL_MakeCurrent(this->window, this->glcontext) < 0) {
        printf("SDL_GL_MakeCurrent failed: %s\n", SDL_GetError());
      }
    }

    if(SDL_GetCurrentDisplayMode(0, &this->currentDisplayMode) < 0) {
      printf("SDL_GetCurrentDisplayMode failed: %s\n", SDL_GetError());
    }
    //SDL_SetHintWithPriority(SDL_HINT_RENDER_SCALE_QUALITY, "2", SDL_HINT_OVERRIDE);

    //SDL_SetHint(SDL_HINT_IME_INTERNAL_EDITING, "0");

    SDL_SetRenderDrawBlendMode(this->renderer, SDL_BLENDMODE_BLEND);

    SDL_GL_SetSwapInterval(0);

    if(fullscreen > 0) {
      this->screenW = this->currentDisplayMode.w;
      this->screenH = this->currentDisplayMode.h;
    }
    else {
      this->screenW = w;
      this->screenH = h;
    }
    this->screenS = min(this->screenW, this->screenH);
    this->aspectRatio = (double)this->screenW/this->screenH;

    float wdpi, hdpi;
    SDL_GetDisplayDPI(0, NULL, &wdpi, &hdpi);

    this->pixelWidthMeters = 1.0 / (wdpi / (2.54*0.01));
    this->pixelHeightMeters = 1.0 / (hdpi / (2.54*0.01));
    
    setVsync(true);
    
    
  }


  void quit() {
    SDL_GL_DeleteContext(this->glcontext);
    SDL_DestroyWindow(this->window);
    SDL_Quit();
  }

};
