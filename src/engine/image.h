#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>
#include <ctgmath>
#include <cstring>
#include <string>
#include "util.h"
#include "message.h"
#include "mathl.h"
#include "sketch.h"
#include "colors.h"
#include "external/lodepng/lodepng.h"

struct Image
{
  SDL_Texture *texture = NULL;
  unsigned int *pixels = NULL;
  bool indirectPixelsApplied = false;
  int w = 0, h = 0;

  /*std::string serializationName;

  // FIXME
  template <class Archive> void save(Archive &ar) const {
    serializationName = "serialized_image_" + getTimeString() + "_" + std::to_string(Random::getInt()) + ".png";
    //saveToFile(serializationName);
    ar(serializationName);
  }

  template <class Archive>
  void load(Archive &ar){
    ar(serializationName);
  }*/

  ~Image() {
    if(pixels) delete [] pixels;
    if(texture) SDL_DestroyTexture(texture);
  }

  Image() {}


  Image(int w, int h, SDLInterface *sdlInterface) {
    create(w, h, sdlInterface);
  }



  bool load(const char *filename, SDLInterface *sdlInterface) {
    std::vector<unsigned char> pngData;
    unsigned int w = 0, h = 0;
    std::vector<unsigned char> src;
    lodepng::load_file(pngData, filename);

    unsigned int error = lodepng::decode(src, w, h, pngData);

    if(error) {
      std::cout << "Error " << error << " loading png: " << lodepng_error_text(error) << "\n"; return false;
    }

    create(w, h, sdlInterface);
    unsigned int *pixelsDst;
    accessPixels(&pixelsDst);

    for(int i=0; i<w*h; i++) {
      pixelsDst[i] = src[i*4] << 24 | src[i*4+1] << 16 | src[i*4+2] << 8 | src[i*4+3];
      //printf();
    }

    applyPixels();

    return true;
  }


  void loadNonStreaming(const char *filename, SDLInterface *sdlInterface) {
    if(pixels) delete [] pixels;
    if(texture) SDL_DestroyTexture(texture);

    texture = IMG_LoadTexture(sdlInterface->renderer, filename);
    if(texture == NULL) {
      printf("Error at Image.load: unable to load image %s\n", filename);
    }
    queryTexture(texture);
  }

  void queryTexture(SDL_Texture *texture) {
    if(texture) {
      Uint32 format;
      int access, w, h;
      SDL_QueryTexture(texture, &format, &access, &w, &h);
      this->w = w;
      this->h = h;
    }
  }

  void create(int w, int h, SDLInterface *sdlInterface) {
    if(pixels) delete [] pixels;
    if(texture) SDL_DestroyTexture(texture);

    this->w = w;
    this->h = h;
    texture = SDL_CreateTexture(sdlInterface->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, w, h);
    // SDL_TEXTUREACCESS_STATIC, SDL_TEXTUREACCESS_TARGET
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
  }
  void createRenderTarget(int w, int h, SDLInterface *sdlInterface) {
    if(pixels) delete [] pixels;
    if(texture) SDL_DestroyTexture(texture);

    this->w = w;
    this->h = h;
    texture = SDL_CreateTexture(sdlInterface->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);
    // SDL_TEXTUREACCESS_STATIC, SDL_TEXTUREACCESS_TARGET
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
  }

  void prepareIndirectPixelArray() {
    if(pixels == NULL) {
      pixels = new unsigned int[w*h];
    }
    indirectPixelsApplied = false;
  }

  void applyIndirectPixelArray() {
    if(pixels == NULL || indirectPixelsApplied) {
      return;
    }
    unsigned int *directPixels;
    int pitch;
    if(SDL_LockTexture(texture, NULL, (void **)&directPixels, &pitch) < 0) {
      printf("SDL_LockTexture failed: %s\n", SDL_GetError());
      return;
    }
    memcpy(directPixels, this->pixels, w*h*sizeof(unsigned int));
    SDL_UnlockTexture(texture);
    indirectPixelsApplied = true;
  }

  int accessPixels(unsigned int **pixels) {
    int pitch;
    if(SDL_LockTexture(texture, NULL, (void **)pixels, &pitch) < 0) {
      printf("SDL_LockTexture failed: %s\n", SDL_GetError());
    }
    return pitch;
  }

  void applyPixels() {
    SDL_UnlockTexture(texture);
  }

  void render(SDLInterface *sdlInterface) {
    SDL_RenderCopy(sdlInterface->renderer, texture, NULL, NULL);
  }

  /*void render(float dx, float dy, SDLInterface *sdlInterface) {
    SDL_Rect dst = {(int)dx-w/2, (int)dy-h/2, w, h};
    SDL_RenderCopy(sdlInterface->renderer, texture, NULL, &dst);
  }*/

  void render(double dx, double dy, SDLInterface *sdlInterface) {
    SDL_Rect dst = {(int)dx, (int)dy, w, h};
    SDL_RenderCopy(sdlInterface->renderer, texture, NULL, &dst);
  }

  void render(const Vec2d &pos, SDLInterface *sdlInterface) {
    SDL_Rect dst = {(int)pos.x-w/2, (int)pos.y-h/2, w, h};
    SDL_RenderCopy(sdlInterface->renderer, texture, NULL, &dst);
  }

  void renderCorners(double x1, double y1, double x2, double y2, SDLInterface *sdlInterface) {
    SDL_Rect dst = {(int)x1, (int)y1, (int)(x2-x1), (int)(y2-y1)};
    SDL_RenderCopy(sdlInterface->renderer, texture, NULL, &dst);
  }

  void render(double sx, double sy, double sw, double sh, double dx, double dy, double dw, double dh, SDLInterface *sdlInterface) {
    SDL_Rect src = {(int)sx, (int)sy, (int)sw, (int)sh};
    SDL_Rect dst = {(int)dx, (int)dy, (int)dw, (int)dh};
    SDL_RenderCopy(sdlInterface->renderer, texture, &src, &dst);
    //SDL_RenderCopyEx(sdlInterface->renderer, texture, &src, &dst, 0, NULL, SDL_FLIP_NONE);
  }

  Image *copyArea(int sx, int sy, int sw, int sh, SDLInterface *sdlInterface) {
    if(sx < 0 || sy < 0 || sw < 1 || sh < 1 || sx + sw > w || sy + sh > h) {
      printf("Error at Image.copyArea: invalid source dimensions (%d, %d) %dx%d, size %dx%d\n", sx, sy, sw, sh, w, h);
      return NULL;
    }
    Image *dst = new Image(sw, sh, sdlInterface);

    unsigned int *pixelsDst, *pixelsSrc;
    dst->accessPixels(&pixelsDst);
    this->accessPixels(&pixelsSrc);

    for(int i=0; i<sw; i++) {
      for(int j=0; j<sh; j++) {
        pixelsDst[i + j*sw] = pixelsSrc[i+sx + (j+sy)*w];
      }
    }

    dst->applyPixels();
    this->applyPixels();

    return dst;
  }

  Image *getCopy(SDLInterface *sdlInterface) {
    Image *copy = new Image();
    copy->create(w, h, sdlInterface);
    unsigned int *pixelsDst, *pixelsSrc;
    copy->accessPixels(&pixelsDst);
    this->accessPixels(&pixelsSrc);
    memcpy(pixelsDst, pixelsSrc, w*h*sizeof(unsigned int));
    copy->applyPixels();
    this->applyPixels();
    //copy->createRenderTarget(w, h, sdlInterface);
    //copy->setRenderTarget(sdlInterface);
    //render(sdlInterface);
    //unsetRenderTarget(sdlInterface);
    return copy;
  }

  void copy(Image &dst) {
    if(w == dst.w && h == dst.h) {
      /*printf("Error at Image.copy: sizes don't match");
      return;*/
      unsigned int *pixelsDst, *pixelsSrc;
      dst.accessPixels(&pixelsDst);
      this->accessPixels(&pixelsSrc);
      memcpy(pixelsDst, pixelsSrc, w*h*sizeof(unsigned int));
      dst.applyPixels();
      this->applyPixels();
    }
    else {
      copyBilinear(dst);
    }
  }

  void copyNearestNeighbour(Image &dst) {
    unsigned int *pixelsSrc, *pixelsDst;
    this->accessPixels(&pixelsSrc);
    dst.accessPixels(&pixelsDst);
    for(int i=0; i<dst.w; i++) {
      double x = map(i, 0.0, dst.w-1.0, 0.0, this->w-1.0);
      int px0 = (int)x;

      for(int j=0; j<dst.h; j++) {
        double y = map(j, 0.0, dst.h-1.0, 0.0, this->h-1.0);
        int py0 = (int)y;

        pixelsDst[i+j*dst.w] = pixelsSrc[px0 + py0*this->w];
      }
    }
    this->applyPixels();
    dst.applyPixels();
  }

  void copyBilinear(Image &dst) {
    unsigned int *pixelsSrc, *pixelsDst;
    this->accessPixels(&pixelsSrc);
    dst.accessPixels(&pixelsDst);
    for(int i=0; i<dst.w; i++) {
      double x = map(i, 0.0, dst.w-1.0, 0.0, this->w-1.0);
      int px0 = clamp((int)x, 0, w-1);
      int px1 = clamp(px0+1, 0, w-1);
      double fx = x-px0;

      for(int j=0; j<dst.h; j++) {
        double y = map(j, 0.0, dst.h-1.0, 0.0, this->h-1.0);
        int py0 = clamp((int)y, 0, h-1);
        int py1 = clamp(py0+1, 0, h-1);
        double fy = y-py0;

        if(py0 >= h || py1 >= h || px0 >= w || px1 >= w || px0 < 0 || py0 < 0 || px1 < 0 || py1 < 0) {
          printf("Error at Image.copyBilinear: Pixel coordinates out of range (%d, %d) (%d, %d), %dx%d\n", px0, py0, px1, py1, w, h);
        }

        Vec4d p0Col(pixelsSrc[px0 + py0*this->w]);
        Vec4d p1Col(pixelsSrc[px1 + py0*this->w]);
        Vec4d p2Col(pixelsSrc[px0 + py1*this->w]);
        Vec4d p3Col(pixelsSrc[px1 + py1*this->w]);

        Vec4d col1 = p0Col * (1.0-fx) + p1Col * fx;
        Vec4d col2 = p2Col * (1.0-fx) + p3Col * fx;
        Vec4d col = col1 * (1.0-fy) + col2 * fy;

        pixelsDst[i+j*dst.w] = col.toColorRGBA();
      }
    }
    this->applyPixels();
    dst.applyPixels();
  }

  void copyBiCatmullRom(Image &dst, double curvature) {
    unsigned int *pixelsSrc, *pixelsDst;
    this->accessPixels(&pixelsSrc);
    dst.accessPixels(&pixelsDst);
    for(int i=0; i<dst.w; i++) {
      double x = map(i, 0.0, dst.w-1.0, 0.0, this->w-1.0);
      int px1 = clamp((int)x, 0, w-1);
      int px[] = {max(px1-1, 0), px1, min(px1+1, this->w-1), min(px1+2, this->w-1)};
      double fx = x - px1;

      for(int j=0; j<dst.h; j++) {
        double y = map(j, 0.0, dst.h-1.0, 0.0, this->h-1.0);
        int py1 = clamp((int)y, 0, h-1);
        int py[] = {max(py1-1, 0), py1, min(py1+1, this->h-1), min(py1+2, this->h-1)};
        double fy = y - py1;

        Vec4d cols[16];
        for(int ip=0; ip<4; ip++) {
          for(int jp=0; jp<4; jp++) {
            cols[ip+4*jp] = pixelsSrc[px[ip] + py[jp]*this->w];
          }
        }
        Vec4d col = biCatmullRom(fx, fy, curvature, cols);

        pixelsDst[i+j*dst.w] = col.toColorRGBA();
      }
    }
    this->applyPixels();
    dst.applyPixels();
  }

  void colorify(Vec4d color) {
    unsigned int *pixels;
    accessPixels(&pixels);
    for(int i=0; i<w*h; i++) {
      Vec4d pix(pixels[i]);
      pix *= color;
      //printf("%f, %f, %f, %f\n", pix.x, pix.y, pix.z, pix.w);
      pixels[i] = pix.toColorRGBA();
    }
    //printf("################################\n");
    applyPixels();
  }
  inline void colorify(const double &r, const double &g, const double &b, const double &a) {
    colorify(Vec4d(r, g, b, a));
  }

  void invertValue() {
    unsigned int *pixels;
    accessPixels(&pixels);

    for(int i=0; i<w*h; i++) {
      unsigned char r = 255 - toR(pixels[i]);
      unsigned char g = 255 - toG(pixels[i]);
      unsigned char b = 255 - toB(pixels[i]);
      unsigned char a = toA(pixels[i]);
      pixels[i] = r << 24 | g << 16 | b << 8 | a;
    }
    applyPixels();
  }

  /*bool load(std::string filename, SDLInterface *sdlInterface) {
    SDL_Surface *sur = IMG_Load(filename.c_str());

    if(!sur) {
      printf("Error at Image.load: failed to load \'%s\'", filename.c_str());
      return false;
    }

    if(texture) SDL_DestroyTexture(texture);
    create(sur->w, sur->h, sdlInterface);

    unsigned int *dst;
    accessPixels(&dst);

    memcpy(dst, sur->pixels, w * h * sizeof(unsigned int));

    applyPixels();

    SDL_FreeSurface(sur);

  }*/

  void saveToFile() {
    saveToFile("XXX");
  }

  void saveToFile(std::string filename) {
    //printf("saving file...\n");
    unsigned int *pixels;
    accessPixels(&pixels);
    SDL_Surface *surf = SDL_CreateRGBSurface(0, w, h, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
    memcpy(surf->pixels, pixels, sizeof(unsigned int)*w*h);
    if(!surf) {
      printf("failed to create surface...\n");
      printf("%s\n", SDL_GetError());
    }
    if(filename.compare("XXX") == 0) {
      filename = getTimeString("%F_%T");
      filename = "output/" + filename + ".png";
    }
    createDirectories(filename);

    //FIXME
    IMG_SavePNG(surf, filename.c_str());
    SDL_FreeSurface(surf);
    applyPixels();

    std::string msg = "image saved: " + filename;

    printf("%s\n", msg.c_str());

    messageQueue.addMessage(to_wstring(msg));
  }

  void transform(Image &imgDst, const Eigen::Matrix3d &m) {
    unsigned int *pixelsSrc, *pixelsDst;
    accessPixels(&pixelsSrc);
    imgDst.accessPixels(&pixelsDst);
    memset(pixelsDst, 0, sizeof(unsigned int)*imgDst.w*imgDst.h);

    Eigen::Vector3d v, t;

    for(int i=0; i<w; i++) {
      for(int j=0; j<h; j++) {
        v << i, j, 1;
        t = m * v;
        if(t(0) >= 0 && t(0) < imgDst.w && t(1) >= 0 && t(1) < imgDst.h) {
          int ind = int(t(0)) + int(t(1)) * imgDst.h;
          pixelsDst[ind] = pixelsSrc[i+j*w];
        }
      }
    }

  }

  /*Uint32 getGlTexture() {
    return texture->driverdata->texture;
  }*/
  /*void unsetRenderTarget(SDLInterface *sdlInterface) {

    int ret = SDL_SetRenderTarget(sdlInterface->renderer, NULL);
    if(ret != 0) {
      printf("Error. Unable to setup default render target.\n");
    }
  }*/

  /*void setRenderTarget(SDLInterface *sdlInterface) {

    int ret = SDL_SetRenderTarget(sdlInterface->renderer, texture);
    if(ret != 0) {
      printf("Error. Unable to setup texture as render target.\n");
    }
    GLuint framebufferID = 0;
    glGenFrameBuffers(1, &framebufferID);
    glBindFrameBuffer(GL_FRAMEBUFFER, framebufferID);*/
    //GLuint tex;
    //glGenTextures(1, &tex);
    //glBindTexture(GL_TEXTURE_2D, tex);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    //GLuint depthbuffer;
    //glGenRenderbuffers(1, &renderbuffer);
    //glBindRenderBuffer(GL_RENDERBUFFER, depthbuffer);
    //glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h);
    //glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthbuffer);

    /*glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, tex, 0);
    GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, drawBuffers);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
      printf("Error. Unable to setup framebuffer.\n");
    }
  }**/

  void bindGlTexture() {
    SDL_GL_BindTexture(texture, NULL, NULL);
  }
  void unbindGlTexture() {
    SDL_GL_UnbindTexture(texture);
  }
};



static void setRenderTarget(Image &img, SDLInterface *sdlInterface) {
  int ret = SDL_SetRenderTarget(sdlInterface->renderer, img.texture);
  if(ret != 0) {
    printf("Error. Unable to set render target.\n");
  }
}
static void setDefaultRenderTarget(SDLInterface *sdlInterface) {
  int ret = SDL_SetRenderTarget(sdlInterface->renderer, NULL);
  if(ret != 0) {
    printf("Error. Unable to set default render target.\n");
  }
}
