#pragma once
#include <SDL2/SDL.h>
//#include <SDL2/SDL_image.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>
#include <ctgmath>
#include <cstring>
#include <string>
#include "util.h"
#include "mathl.h"
#include "sketch.h"
#include "colors.h"




/*void setColor(double r, double g, double b, double a, SDLInterface *sdlInterface) {
  SDL_SetRenderDrawColor(sdlInterface->renderer, toByte(r), toByte(g), toByte(b), toByte(a));
}*/
/*void setColor(int r, int g, int b, int a, SDLInterface *sdlInterface) {
  SDL_SetRenderDrawColor(sdlInterface->renderer, r, g, b, a);
}*/

static void setColor(double r, double g, double b, double a, SDLInterface *sdlInterface) {
  SDL_SetRenderDrawColor(sdlInterface->renderer, toByte(r), toByte(g), toByte(b), toByte(a));
  glColor4f(r, g, b, a);
}

static void setColor(const Vec3d &col, double a, SDLInterface *sdlInterface) {
  SDL_SetRenderDrawColor(sdlInterface->renderer, toByte(col.x), toByte(col.y), toByte(col.z), toByte(a));
  glColor4f(col.x, col.y, col.z, a);
}
static void setColor(const Vec3d &col, SDLInterface *sdlInterface) {
  SDL_SetRenderDrawColor(sdlInterface->renderer, toByte(col.x), toByte(col.y), toByte(col.z), 255);
  glColor4f(col.x, col.y, col.z, 1);
}

static void setColor(const Vec4d &col, SDLInterface *sdlInterface) {
  SDL_SetRenderDrawColor(sdlInterface->renderer, toByte(col.x), toByte(col.y), toByte(col.z), toByte(col.w));
  glColor4f(col.x, col.y, col.z, col.w);
}
static void clear(float r, float g, float b, float a) {
  glClearColor(r, g, b, a);
  glClear(GL_COLOR_BUFFER_BIT);
}

static void clear(float r, float g, float b, float a, SDLInterface *sdlInterface) {
  setColor(r, g, b, a, sdlInterface);
  glClear(GL_COLOR_BUFFER_BIT);
  SDL_RenderClear(sdlInterface->renderer);
}

static void drawPoint(int x, int y, SDLInterface *sdlInterface) {
  SDL_RenderDrawPoint(sdlInterface->renderer, x, y);
}
static void drawPoint(float x, float y, SDLInterface *sdlInterface) {
  SDL_RenderDrawPoint(sdlInterface->renderer, (int)round(x), (int)round(y));
}
static void drawPoint(double x, double y, SDLInterface *sdlInterface) {
  SDL_RenderDrawPoint(sdlInterface->renderer, (int)round(x), (int)round(y));
}
static void drawPoint(Vec2d p, SDLInterface *sdlInterface) {
  SDL_RenderDrawPoint(sdlInterface->renderer, (int)round(p.x), (int)round(p.y));
}
static void drawLine(double x1, double y1, double x2, double y2, SDLInterface *sdlInterface) {
  SDL_RenderDrawLine(sdlInterface->renderer, (int)round(x1), (int)round(y1), (int)round(x2), (int)round(y2));
}

static void drawLine(const Vec2d &a, const Vec2d &b, SDLInterface *sdlInterface) {
  SDL_RenderDrawLine(sdlInterface->renderer, (int)round(a.x), (int)round(a.y), (int)round(b.x), (int)round(b.y));
}

static void fillRectCenter(double x, double y, double w, double h, SDLInterface *sdlInterface) {
  SDL_Rect rect = {(int)round(x-w*0.5), (int)round(y-h*0.5), (int)round(w), (int)round(h)};
  SDL_RenderFillRect(sdlInterface->renderer, &rect);
}
static void fillRect(double x, double y, double r, SDLInterface *sdlInterface) {
  SDL_Rect rect = {(int)round(x-r*0.5), (int)round(y-r*0.5), (int)round(r), (int)round(r)};
  SDL_RenderFillRect(sdlInterface->renderer, &rect);
}
static void fillRect(Vec2d p, double w, double h, SDLInterface *sdlInterface) {
  SDL_Rect rect = {(int)round(p.x-w*0.5), (int)round(p.y-h*0.5), (int)round(w), (int)round(h)};
  SDL_RenderFillRect(sdlInterface->renderer, &rect);
}
static void fillRect(Vec2d p, double r, SDLInterface *sdlInterface) {
  SDL_Rect rect = {(int)round(p.x-r*0.5), (int)round(p.y-r*0.5), (int)round(r), (int)round(r)};
  SDL_RenderFillRect(sdlInterface->renderer, &rect);
}
static void fillRectCorners(double x1, double y1, double x2, double y2, SDLInterface *sdlInterface) {
  SDL_Rect rect = {(int)round(x1), (int)round(y1), (int)round(x2-x1), (int)round(y2-y1)};
  SDL_RenderFillRect(sdlInterface->renderer, &rect);
}

static void fillRect(const Vec2d &a, const Vec2d &b, SDLInterface *sdlInterface) {
  SDL_Rect rect = {(int)round(a.x), (int)round(a.y), (int)round(b.x-a.x), (int)round(b.y-a.y)};
  SDL_RenderFillRect(sdlInterface->renderer, &rect);
}
static void drawRect(const Vec2d &a, const Vec2d &b, SDLInterface *sdlInterface) {
  drawLine(a.x, a.y, a.x, b.y, sdlInterface);
  drawLine(a.x, a.y, b.x, a.y, sdlInterface);
  drawLine(a.x, b.y, b.x, b.y, sdlInterface);
  drawLine(b.x, a.y, b.x, b.y, sdlInterface);
}

static void setAlphaBlending(bool b, SDLInterface *sdlInterface) {
  SDL_SetRenderDrawBlendMode(sdlInterface->renderer, b ? SDL_BLENDMODE_BLEND : SDL_BLENDMODE_NONE);
}

static void drawCircle(double x, double y, double r, SDLInterface *sdlInterface) {
  r = r < 1 ? 1 : r;
  double x1 = x + r, y1 = y;
  int k = 2*int(r);
  for(double d=TAU/k; d<=TAU+TAU/k; d+=TAU/k) {
    double x2 = x + cos(d) * r;
    double y2 = y + sin(d) * r;
    drawLine(x1, y1, x2, y2, sdlInterface);
    x1 = x2;
    y1 = y2;
  }
}

static void drawCircle(Vec2d p, double r, SDLInterface *sdlInterface) {
  drawCircle(p.x, p.y, r, sdlInterface);
}



static void updateScreen(SDLInterface *sdlInterface) {
  SDL_GL_SwapWindow(sdlInterface->window);
  //SDL_RenderPresent(sdlInterface->renderer);
  //SDL_UpdateWindowSurface(sdlInterface->window);
}
