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
#include "/home/lassi/github/lodepng/lodepng.h"
#include "glwrap.h"


static Quadx *defaultTextureQuad = NULL, *defaultTextureQuadBottomRightUV = NULL;
static GlShader *defaultTextureShader = NULL;

struct Texture
{
  enum PixelType { RGBA, FLOAT, INT, UINT };
  enum TextureType { Texture2D, TextureArray2D };

  PixelType pixelType = PixelType::RGBA;
  TextureType textureType = TextureType::Texture2D;
  int textureArraySize = 1;

  GLuint textureID = 0, texture2ID = 0;
  GLuint depthBufferID = 0;
  GLuint fboID = 0;

  std::vector<float> rgbaPixels;
  std::vector<float> floatPixels;
  std::vector<unsigned int> uintPixels;
  std::vector<int> intPixels;

  int w = 0, h = 0;

  Vec4d overlayColor;
  bool useBottomRightUV = true;
  bool useSecondaryTexture = false;
  bool secondaryTextureFBOSwapState = false;

  std::string name;


  virtual ~Texture() {
    if(textureID != 0) {
      glDeleteTextures(1, &textureID);
    }
    if(texture2ID != 0) {
      glDeleteTextures(1, &texture2ID);
    }
  }

  Texture() {}


  Texture(int w, int h, bool useBottomRightUV = true, bool createSecondaryTexture = false) {
    create(w, h, useBottomRightUV, createSecondaryTexture);
  }

  Texture(const std::string &filename) {
    load(filename);
  }

  void saveToFile() {
    saveToFile("XXX");
  }
  
  /*void disableFiltering() {
    glBindTexture(GL_TEXTURE_2D, textureID);
    checkGlError("Texture.saveToFile()", "glBindTexture()");
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    checkGlError("Texture.createTexture()", "glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);");  
  }*/

  void enableFiltering(bool value) {
    glBindTexture(GL_TEXTURE_2D, textureID);
    checkGlError("Texture.saveToFile()", "glBindTexture()");
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, value ? GL_LINEAR : GL_NEAREST);
    checkGlError("Texture.createTexture()", "glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);");  
  }

  /* FIXME
  * save alpha channel optionably
  */
  void saveToFile(std::string filename) {
    std::vector<unsigned char> rgbaPixels(w*h*4, 0.0);

    glBindTexture(GL_TEXTURE_2D, textureID);
    checkGlError("Texture.saveToFile()", "glBindTexture()");
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, &rgbaPixels[0]);
    checkGlError("Texture.saveToFile(" + filename+")", "glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, rgbaPixels)");

    for(int i=3; i<w*h*4; i+=4) {
      rgbaPixels[i] = 255;
    }

    if(filename.compare("XXX") == 0) {
      filename = getTimeString("%F_%T");
      filename = "output/" + filename + ".png";
    }
    createDirectories(filename);

    lodepng::encode(filename, &rgbaPixels[0], w, h);

    std::string msg = "image saved: " + filename;
    printf("%s\n", msg.c_str());
    messageQueue.addMessage(to_wstring(msg));
  }


  bool load(const std::string &filename, bool useBottomRightUV = true) {
    std::vector<unsigned char> pngData;
    unsigned int w = 0, h = 0;
    std::vector<unsigned char> src;
    lodepng::load_file(pngData, filename.c_str());
    
    this->useBottomRightUV = useBottomRightUV;

    unsigned int error = lodepng::decode(src, w, h, pngData);

    if(error) {
      printf("Error at Texture.load(\"%s\"): %s\n", filename.c_str(), lodepng_error_text(error));
      return false;
    }
    this->w = w;
    this->h = h;

    // FIXME

    if(pixelType == PixelType::RGBA) {
      rgbaPixels.resize(w*h*4, 0);
      for(int i=0; i<w*h*4; i++) {
        rgbaPixels[i] = src[i] / 255.0;
      }
    }
    if(pixelType == PixelType::FLOAT) {
      floatPixels.resize(w*h, 0);
      for(int i=0; i<w*h*4; i+=4) {
        floatPixels[i] = (src[i] + src[i+1] + src[i+2] + src[i+3]) / (255.0 * 4.0);
      }
    }
    if(pixelType == PixelType::INT) {
      intPixels.resize(w*h, 0);
      for(int i=0; i<w*h*4; i+=4) {
        intPixels[i] = src[i] + src[i+1] + src[i+2] + src[i+3];
      }
    }
    if(pixelType == PixelType::UINT) {
      uintPixels.resize(w*h*4, 0);
      for(int i=0; i<w*h*4; i+=4) {
        uintPixels[i] = src[i] + src[i+1] + src[i+2] + src[i+3];
      }
    }


    /*for(int i=0; i<w*h; i++) {
      rgbaPixelsBytes[i] = src[i*4] << 24 | src[i*4+1] << 16 | src[i*4+2] << 8 | src[i*4+3];
    }*/

    createTexture();

    this->name = filename;

    return true;
  }


  bool createDualTexture(int w, int h, bool useBottomRightUV = true) {
    return create(w, h, useBottomRightUV, true);
  }

  bool createTextureRenderTargetTesting(int w, int h) {
    return create(w, h, true, false, true);
  }

  bool create(int w, int h, PixelType pixelType, bool useBottomRightUV = true, bool createSecondaryTexture = false, bool noInitialPixelData = false) {
    this->pixelType = pixelType;
    return create(w, h, useBottomRightUV, createSecondaryTexture, noInitialPixelData);
  }

  bool createArray(int w, int h, int textureArraySize, PixelType pixelType = PixelType::RGBA, bool useBottomRightUV = true, bool createSecondaryTexture = false, bool noInitialPixelData = false) {
    this->pixelType = pixelType;
    this->textureArraySize = textureArraySize;
    this->textureType = TextureType::TextureArray2D;
    return create(w, h, useBottomRightUV, createSecondaryTexture, noInitialPixelData);
  }

  bool create(int w, int h, bool useBottomRightUV = true, bool createSecondaryTexture = false, bool noInitialPixelData = false) {
    this->w = w;
    this->h = h;
    this->useBottomRightUV = useBottomRightUV;

    if(pixelType == PixelType::RGBA) {
      rgbaPixels.resize(w*h*4*textureArraySize, 0);
    }
    if(pixelType == PixelType::FLOAT) {
      floatPixels.resize(w*h*textureArraySize, 0);
    }
    if(pixelType == PixelType::INT) {
      intPixels.resize(w*h*textureArraySize, 0);
    }
    if(pixelType == PixelType::UINT) {
      uintPixels.resize(w*h*textureArraySize, 0);
    }

    bool ret = createTexture(false, noInitialPixelData);
    bool ret2 = true;
    if(createSecondaryTexture) {
      ret2 = createTexture(true, noInitialPixelData);
    }

    if(!defaultTextureQuad) {
      defaultTextureQuad = new Quadx(1, 1, false);
    }
    if(!defaultTextureQuadBottomRightUV) {
      defaultTextureQuadBottomRightUV = new Quadx(1, 1, true);
    }
    if(!defaultTextureShader) {
      defaultTextureShader = new GlShader("data/glsl/texture.vert", "data/glsl/textureColor.frag");
    }

    overlayColor.set(1, 1, 1, 1);

    return ret && ret2;
  }


  bool createTexture(bool secondaryTexture = false, bool noInitialPixelData = false) {
    if(secondaryTexture) {
      if(texture2ID != 0) {
        glDeleteTextures(1, &texture2ID);
        texture2ID = 0;
      }
      glGenTextures(1, &texture2ID);
    }
    else {
      if(textureID != 0) {
        glDeleteTextures(1, &textureID);
        textureID = 0;
      }
      glGenTextures(1, &textureID);
    }
    GLuint textureID = secondaryTexture ? this->texture2ID : this->textureID;

    checkGlError("Texture.createTexture()", "glGenTextures()");

    GLenum target = GL_TEXTURE_2D;
    if(textureType == TextureType::TextureArray2D) {
      target = GL_TEXTURE_2D_ARRAY;
    }

    glBindTexture(target, textureID);
    checkGlError("Texture.createTexture()", "glBindTexture()");

    glTexParameteri(target, GL_TEXTURE_BASE_LEVEL, 0);
    checkGlError("Texture.createTexture()", "glTexParameteri()");

    glTexParameteri(target, GL_TEXTURE_MAX_LEVEL, 0);
    checkGlError("Texture.createTexture()", "glTexParameteri()");

    /*if(pixelType == PixelType::RGBA) {
      if(noInitialPixelData) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
        checkGlError("Texture.createTexture()", "glTexImage2D()");

      }
      else {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_FLOAT, rgbaPixels);
        checkGlError("Texture.createTexture()", "glTexImage2D()");
      }
    }
    else {
      if(noInitialPixelData) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_R32UI, GL_UNSIGNED_INT, 0);
        checkGlError("Texture.createTexture()", "glTexImage2D()");
      }
      else {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R32UI, w, h, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, uintPixels);
        checkGlError("Texture.createTexture()", "glTexImage2D()");
      }
    }*/

    if(textureType == TextureType::Texture2D) {
      if(pixelType == PixelType::RGBA) {
        glTexImage2D(target, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_FLOAT, noInitialPixelData ? 0 : rgbaPixels.data());
        checkGlError("Texture.createTexture()", "glTexImage2D()");
      }
      if(pixelType == PixelType::FLOAT) {
        glTexImage2D(target, 0, GL_R32F, w, h, 0, GL_RED, GL_FLOAT, noInitialPixelData ? 0 : floatPixels.data());
        checkGlError("Texture.createTexture()", "glTexImage2D()");
      }
      if(pixelType == PixelType::INT) {
        glTexImage2D(target, 0, GL_R32I, w, h, 0, GL_RED_INTEGER, GL_INT, noInitialPixelData ? 0 : intPixels.data());
        checkGlError("Texture.createTexture()", "glTexImage2D()");
      }
      if(pixelType == PixelType::UINT) {
        glTexImage2D(target, 0, GL_R32UI, w, h, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, noInitialPixelData ? 0 : uintPixels.data());
        checkGlError("Texture.createTexture()", "glTexImage2D()");
      }
    }
    if(textureType == TextureType::TextureArray2D) {
      if(pixelType == PixelType::RGBA) {
        glTexStorage3D(target, 1, GL_RGBA32F, w, h, textureArraySize);
        checkGlError("Texture.createTexture()", "glTexStorage3D()");
        glTexSubImage3D(target, 0, 0, 0, 0, w, h, textureArraySize, GL_RGBA, GL_FLOAT, noInitialPixelData ? 0 : rgbaPixels.data());
        checkGlError("Texture.createTexture()", "glTexSubImage3D()");
      }
      if(pixelType == PixelType::FLOAT) {
        glTexStorage3D(target, 1, GL_R32F, w, h, textureArraySize);
        checkGlError("Texture.createTexture()", "glTexStorage3D()");
        glTexSubImage3D(target, 0, 0, 0, 0, w, h, textureArraySize, GL_RED, GL_FLOAT, noInitialPixelData ? 0 : floatPixels.data());
        checkGlError("Texture.createTexture()", "glTexSubImage3D()");
      }
      if(pixelType == PixelType::INT) {
        glTexStorage3D(target, 1, GL_R32I, w, h, textureArraySize);
        checkGlError("Texture.createTexture()", "glTexStorage3D()");
        glTexSubImage3D(target, 0, 0, 0, 0, w, h, textureArraySize, GL_RED_INTEGER, GL_INT, noInitialPixelData ? 0 : intPixels.data());
        checkGlError("Texture.createTexture()", "glTexSubImage3D()");
      }
      if(pixelType == PixelType::UINT) {
        glTexStorage3D(target, 1, GL_R32UI, w, h, textureArraySize);
        checkGlError("Texture.createTexture()", "glTexStorage3D()");
        glTexSubImage3D(target, 0, 0, 0, 0, w, h, textureArraySize, GL_RED_INTEGER, GL_UNSIGNED_INT, noInitialPixelData ? 0 : uintPixels.data());
        checkGlError("Texture.createTexture()", "glTexSubImage3D()");
      }
    }


    //glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    checkGlError("Texture.createTexture()", "glTexParameteri()");
    glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    checkGlError("Texture.createTexture()", "glTexParameteri()");
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    checkGlError("Texture.createTexture()", "glTexParameteri()");
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    checkGlError("Texture.createTexture()", "glTexParameteri()");

    glTexParameteri(target, GL_TEXTURE_MIN_LOD, -1000);
    checkGlError("Texture.createTexture()", "glTexParameteri()");
    glTexParameteri(target, GL_TEXTURE_MAX_LOD, 1000);
    checkGlError("Texture.createTexture()", "glTexParameteri()");


    glPixelStorei(GL_UNPACK_SWAP_BYTES, GL_FALSE);
    checkGlError("Texture.createTexture()", "glPixelStorei(GL_UNPACK_SWAP_BYTES, GL_FALSE)");
    glPixelStorei(GL_UNPACK_LSB_FIRST, GL_FALSE);
    checkGlError("Texture.createTexture()", "glPixelStorei(GL_UNPACK_LSB_FIRST, GL_FALSE)");
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    checkGlError("Texture.createTexture()", "glPixelStorei(GL_UNPACK_ROW_LENGTH, 0)");
    glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
    checkGlError("Texture.createTexture()", "glPixelStorei(GL_UNPACK_SKIP_ROWS, 0)");
    glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
    checkGlError("Texture.createTexture()", "glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0)");

    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    checkGlError("Texture.createTexture()", "glPixelStorei(GL_UNPACK_ALIGNMENT, 1)");
    glPixelStorei(GL_PACK_ALIGNMENT, 4);
    checkGlError("Texture.createTexture()", "glPixelStorei(GL_PACK_ALIGNMENT, 1)");



    //glBindTexture(GL_TEXTURE_2D, 0);
    //checkGlError("Texture.createTexture()", "glBindTexture(GL_TEXTURE_2D, 0)");

    /*glGenRenderbuffers(1, &depthBufferID);
    checkGlError("Texture.createTexture()", "glGenRenderbuffers(1, &depthBufferID)");
    glBindRenderbuffer(GL_RENDERBUFFER, depthBufferID);
    checkGlError("Texture.createTexture()", "glBindRenderbuffer(GL_RENDERBUFFER, depthBufferID)");
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, w, h);
    checkGlError("Texture.createTexture()", "glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, w, h)");*/



    return true;
  }


  // FIXME consistency, please
  
  void renderCenter(double x, double y, const Vec4d &overlayColor) {
    render(x, y, 0.0, w, h, 1.0, 1.0, overlayColor);
  }
  void renderCenter(double x, double y) {
    render(x, y, 0.0, w, h, 1.0, 1.0, overlayColor);
  }
  void renderCenter(double x, double y, double rotation) {
    render(x, y, rotation, w, h, 1.0, 1.0, overlayColor);
  }
  
  void render() {
    render(w*0.5, h*0.5, 0.0, w, h, 1.0, 1.0, overlayColor);
  }
  void renderFlipY() {
    render(w*0.5, h*0.5, 0.0, w, h, 1.0, -1.0, overlayColor);
  }
  void render(double x, double y) {
    render(x+w*0.5, y+h*0.5, 0.0, w, h, 1.0, 1.0, overlayColor);
  }
  void render(double x, double y, double w, double h) {
    render(x+w*0.5, y+h*0.5, 0.0, w, h, 1.0, 1.0, overlayColor);
  }
  void render(const Vec2d &pos) {
    render(pos.x+w*0.5, pos.y+h*0.5, 0.0, w, h, 1.0, 1.0, overlayColor);
  }
  void render(const Vec2d &pos, const Vec4d &overlayColor) {
    render(pos.x, pos.y, 0.0, w, h, 1.0, 1.0, overlayColor);
  }
  void render(const Vec2d &pos, double rotation, const Vec4d &overlayColor) {
    render(pos.x, pos.y, rotation, w, h, 1.0, 1.0, overlayColor);
  }
  void renderFlipY(double x, double y) {
    render(x+w*0.5, y+h*0.5, 0.0, w, h, 1.0, -1.0, overlayColor);
  }
  void render(double x, double y, double rotation) {
    render(x, y, rotation, w, h, 1.0, 1.0, overlayColor);
  }
  void renderCorners(double x1, double y1, double x2, double y2) {
    render(x1, y1, 0.0, x2-x1, y2-y1, 1.0, 1.0, overlayColor);
  }

  void render(const Vec4d &overlayColor) {
    render(w*0.5, h*0.5, 0.0, w, h, 1.0, 1.0, overlayColor);
  }
  void renderFlipY(const Vec4d &overlayColor) {
    render(w*0.5, h*0.5, 0.0, w, h, 1.0, -1.0, overlayColor);
  }
  void render(double x, double y, const Vec4d &overlayColor) {
    render(x+w*0.5, y+h*0.5, 0.0, w, h, 1.0, 1.0, overlayColor);
  }
  void renderFlipY(double x, double y, const Vec4d &overlayColor) {
    render(x+w*0.5, y+h*0.5, 0.0, w, h, 1.0, -1.0, overlayColor);
  }
  void render(double x, double y, double rotation, const Vec4d &overlayColor) {
    render(x, y, rotation, w, h, 1.0, 1.0, overlayColor);
  }
  void renderCorners(double x1, double y1, double x2, double y2, const Vec4d &overlayColor) {
    render(x1, y1, 0.0, x2-x1, y2-y1, 1.0, 1.0, overlayColor);
  }

  void render(double x, double y, double rotation, double w, double h, double scaleX, double scaleY, const Vec4d &overlayColor) {
    if(textureID == 0) {
      printf("Warning at Texture.render: GL texture not initialized\n");
      return;
    }

    defaultTextureShader->activate();
    defaultTextureShader->setUniform4f("colorify", overlayColor);
    defaultTextureShader->setUniform1i("usePixelCoordinates", 0);
    defaultTextureShader->setUniform1i("flipY", 1);
    this->activate(*defaultTextureShader, "tex", 0);
    Quadx *quad = useBottomRightUV ? defaultTextureQuadBottomRightUV : defaultTextureQuad;
    quad->setSize(w, h);
    quad->render(x, y, rotation, scaleX, scaleY);
    defaultTextureShader->deActivate();
    this->inactivate(0);
  }

  void renderSwapTexture(double clipX = 0, double clipY = 0, double clipW = 0, double clipH = 0, bool useClipArea = false) {
    if(textureID == 0 || texture2ID == 0) {
      printf("Warning at Texture.renderToSwapTexture: GL textures not initialized\n");
      return;
    }


    defaultTextureShader->activate();
    defaultTextureShader->setUniform4f("colorify", 1, 1, 1, 1);
    this->activate(*defaultTextureShader, "tex", 2);
    Quadx *quad = useBottomRightUV ? defaultTextureQuadBottomRightUV : defaultTextureQuad;
    if(!useClipArea) {
      defaultTextureShader->setUniform1i("usePixelCoordinates", 0);
      quad->setSize(w, h);
      quad->render(w*0.5, h*0.5);
    }
    else {
      defaultTextureShader->setUniform1i("usePixelCoordinates", 1);
      defaultTextureShader->setUniform1i("flipY", 0);
      defaultTextureShader->setUniform2f("screenSize", w, h);
      quad->setSize(clipW, clipH);
      quad->render(clipX, clipY);
    }

    defaultTextureShader->deActivate();
    this->inactivate(2);
  }



  // FIXME subregion stuff not usable !!!
  void applyPixels(int dx = 0, int dy = 0, int w = 0, int h = 0, int arrayIndex = -1, int arraySize = -1) {
    if(textureID == 0) {
      createTexture();
      return;
    }
    if(w == 0) w = this->w;
    if(h == 0) h = this->h;
    if(arrayIndex < 0) arrayIndex = 0;
    if(arraySize < 0) arraySize = textureArraySize;

    if(arrayIndex >= textureArraySize || arrayIndex+arraySize > textureArraySize) {
      printf("Error at Texture::applyPixels(), invalid array dimensions %d, %d, %d\n", arrayIndex, arraySize, textureArraySize);
      return;
    }

    // feature removed, which haven't been implemented, fix
    dx = 0;
    w = this->w;

    GLenum target = GL_TEXTURE_2D;
    if(textureType == TextureType::TextureArray2D) {
      target = GL_TEXTURE_2D_ARRAY;
    }

    glBindTexture(target, textureID);
    checkGlError("Texture.applyPixels()", "glBindTexture()");

    if(textureType == TextureType::Texture2D) {
      if(pixelType == PixelType::RGBA) {
        glTexSubImage2D(target, 0, dx, dy, w, h, GL_RGBA, GL_FLOAT, rgbaPixels.data() + (dx+dy*w)*4);
      }
      if(pixelType == PixelType::FLOAT) {
        glTexSubImage2D(target, 0, dx, dy, w, h, GL_RED, GL_FLOAT, floatPixels.data() + (dx+dy*w));
      }
      if(pixelType == PixelType::INT) {
        glTexSubImage2D(target, 0, dx, dy, w, h, GL_RED_INTEGER, GL_INT, intPixels.data() + (dx+dy*w));
      }
      if(pixelType == PixelType::UINT) {
        glTexSubImage2D(target, 0, dx, dy, w, h, GL_RED_INTEGER, GL_UNSIGNED_INT, uintPixels.data() + (dx+dy*w));
      }
      checkGlError("Texture.applyPixels()", "glTexSubImage2D()");
    }

    if(textureType == TextureType::TextureArray2D) {
      if(pixelType == PixelType::RGBA) {
        glTexSubImage3D(target, 0, dx, dy, arrayIndex, w, h, arraySize, GL_RGBA, GL_FLOAT, rgbaPixels.data() + (dx+dy*w+arrayIndex*w*h)*4);
      }
      if(pixelType == PixelType::FLOAT) {
        glTexSubImage3D(target, 0, dx, dy, arrayIndex, w, h, arraySize, GL_RED, GL_FLOAT, floatPixels.data() + (dx+dy*w+arrayIndex*w*h));
      }
      if(pixelType == PixelType::INT) {
        glTexSubImage3D(target, 0, dx, dy, arrayIndex, w, h, arraySize, GL_RED_INTEGER, GL_INT, intPixels.data() + (dx+dy*w+arrayIndex*w*h));
      }
      if(pixelType == PixelType::UINT) {
        glTexSubImage3D(target, 0, dx, dy, arrayIndex, w, h, arraySize, GL_RED_INTEGER, GL_UNSIGNED_INT, uintPixels.data() + (dx+dy*w+arrayIndex*w*h));
      }
      checkGlError("Texture.applyPixels()", "glTexSubImage3D()");
    }


    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_FLOAT, rgbaPixels);
    //checkGlError("Texture.applyPixels()", "glTexImage2D()");
  }

  void loadGlPixels() {

    GLenum target = GL_TEXTURE_2D;
    if(textureType == TextureType::TextureArray2D) {
      target = GL_TEXTURE_2D_ARRAY;
    }

    glBindTexture(target, textureID);
    checkGlError("Texture.loadGlPixels()", "glBindTexture()");


    if(pixelType == PixelType::RGBA) {
      glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, rgbaPixels.data());
    }
    if(pixelType == PixelType::FLOAT) {
      glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_FLOAT, floatPixels.data());
    }
    if(pixelType == PixelType::INT) {
      glGetTexImage(GL_TEXTURE_2D, 0, GL_RED_INTEGER, GL_INT, intPixels.data());
    }
    if(pixelType == PixelType::UINT) {
      glGetTexImage(GL_TEXTURE_2D, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, uintPixels.data());
    }
    checkGlError("Texture.loadGlPixels()", "glGetTexImage()");
  }





  Texture *copyArea(int sx, int sy, int sw, int sh) {
    if(pixelType != PixelType::RGBA) {
      printf("Error at Texture.copyArea: other pixel types not supported, yet, than RGBA \n");
      return NULL;
    }
    if(sx < 0 || sy < 0 || sw < 1 || sh < 1 || sx + sw > w || sy + sh > h) {
      printf("Error at Texture.copyArea: invalid source dimensions (%d, %d) %dx%d, size %dx%d\n", sx, sy, sw, sh, w, h);
      return NULL;
    }
    Texture *dst = new Texture(sw, sh);

    for(int i=0; i<sw; i++) {
      for(int j=0; j<sh; j++) {
        //rgbaPixelsDst[i + j*sw] = rgbaPixelsSrc[i+sx + (j+sy)*w];
        int indDst = (i + j*sw) * 4;
        int indSrc = (i+sx + (j+sy)*w) * 4;
        dst->rgbaPixels[indDst+0] = rgbaPixels[indSrc+0];
        dst->rgbaPixels[indDst+1] = rgbaPixels[indSrc+1];
        dst->rgbaPixels[indDst+2] = rgbaPixels[indSrc+2];
        dst->rgbaPixels[indDst+3] = rgbaPixels[indSrc+3];
      }
    }

    dst->applyPixels();

    return dst;
  }

  Texture *getCopy() {
    Texture *copy = new Texture(w, h);
    memcpy(copy->rgbaPixels.data(), rgbaPixels.data(), w*h*4*sizeof(float));
    copy->applyPixels();
    return copy;
  }

  Texture *getCopy(int w, int h) {
    Texture *copy = new Texture(w, h);
    if(this->w == copy->w && this->h == copy->h) {
      memcpy(copy->rgbaPixels.data(), rgbaPixels.data(), w*h*4*sizeof(float));
      copy->applyPixels();
    }
    else {
      copyBilinear(*copy);
    }
    return copy;
  }

  void copy(Texture &dst) {
    if(w == dst.w && h == dst.h) {
      /*printf("Error at Image.copy: sizes don't match");
      return;*/
      memcpy(dst.rgbaPixels.data(), rgbaPixels.data(), w*h*4*sizeof(float));
      dst.applyPixels();
    }
    else {
      copyBilinear(dst);
    }
  }

  void copyNearestNeighbour(Texture &dst) {
    for(int i=0; i<dst.w; i++) {
      double x = map(i, 0.0, dst.w-1.0, 0.0, this->w-1.0);
      int px0 = (int)x;

      for(int j=0; j<dst.h; j++) {
        double y = map(j, 0.0, dst.h-1.0, 0.0, this->h-1.0);
        int py0 = (int)y;

        int indDst = (i+j*dst.w) * 4;
        int indSrc = (px0 + py0*this->w) * 4;
        dst.rgbaPixels[indDst+0] = rgbaPixels[indSrc+0];
        dst.rgbaPixels[indDst+1] = rgbaPixels[indSrc+1];
        dst.rgbaPixels[indDst+2] = rgbaPixels[indSrc+2];
        dst.rgbaPixels[indDst+3] = rgbaPixels[indSrc+3];
      }
    }
    dst.applyPixels();
  }

  void copyBilinear(Texture &dst) {
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
          printf("Error at Texture.copyBilinear: Pixel coordinates out of range (%d, %d) (%d, %d), %dx%d\n", px0, py0, px1, py1, w, h);
        }

        int indSrc0 = (px0 + py0*this->w) * 4;
        int indSrc1 = (px1 + py0*this->w) * 4;
        int indSrc2 = (px0 + py1*this->w) * 4;
        int indSrc3 = (px1 + py1*this->w) * 4;

        Vec4d p0Col(rgbaPixels[indSrc0], rgbaPixels[indSrc0+1], rgbaPixels[indSrc0+2], rgbaPixels[indSrc0+3]);
        Vec4d p1Col(rgbaPixels[indSrc1], rgbaPixels[indSrc1+1], rgbaPixels[indSrc1+2], rgbaPixels[indSrc1+3]);
        Vec4d p2Col(rgbaPixels[indSrc2], rgbaPixels[indSrc2+1], rgbaPixels[indSrc2+2], rgbaPixels[indSrc2+3]);
        Vec4d p3Col(rgbaPixels[indSrc3], rgbaPixels[indSrc3+1], rgbaPixels[indSrc3+2], rgbaPixels[indSrc3+3]);

        Vec4d col1 = p0Col * (1.0-fx) + p1Col * fx;
        Vec4d col2 = p2Col * (1.0-fx) + p3Col * fx;
        Vec4d col = col1 * (1.0-fy) + col2 * fy;

        int indDst = (i+j*dst.w) * 4;
        dst.rgbaPixels[indDst+0] = col.x;
        dst.rgbaPixels[indDst+1] = col.y;
        dst.rgbaPixels[indDst+2] = col.z;
        dst.rgbaPixels[indDst+3] = col.w;
      }
    }
    dst.applyPixels();
  }

  // FIXME manage different pixel types
  inline float getPixelR(int x, int y) {
    return rgbaPixels[(x+y*w)*4];
  }
  inline float getPixelG(int x, int y) {
    return rgbaPixels[(x+y*w)*4+1];
  }
  inline float getPixelB(int x, int y) {
    return rgbaPixels[(x+y*w)*4+2];
  }
  inline float getPixelA(int x, int y) {
    return rgbaPixels[(x+y*w)*4+3];
  }
  inline void setPixel(int x, int y, double r, double g, double b, double a) {
    int ind = (x+y*w)*4;
    rgbaPixels[ind + 0] = r;
    rgbaPixels[ind + 1] = g;
    rgbaPixels[ind + 2] = b;
    rgbaPixels[ind + 3] = a;
  }
  inline void setPixel(int x, int y, const Vec3d &col, double a = 1) {
    int ind = (x+y*w)*4;
    rgbaPixels[ind + 0] = clamp(col.x, 0.0, 1.0);
    rgbaPixels[ind + 1] = clamp(col.y, 0.0, 1.0);
    rgbaPixels[ind + 2] = clamp(col.z, 0.0, 1.0);
    rgbaPixels[ind + 3] = a;
  }

  inline void setPixel(int x, int y, const Vec4d &col) {
    int ind = (x+y*w)*4;
    rgbaPixels[ind + 0] = clamp(col.x, 0.0, 1.0);
    rgbaPixels[ind + 1] = clamp(col.y, 0.0, 1.0);
    rgbaPixels[ind + 2] = clamp(col.z, 0.0, 1.0);
    rgbaPixels[ind + 3] = clamp(col.w, 0.0, 1.0);
  }


  inline void setPixel(int ind, double r, double g, double b, double a) {
    ind = ind * 4;
    rgbaPixels[ind + 0] = r;
    rgbaPixels[ind + 1] = g;
    rgbaPixels[ind + 2] = b;
    rgbaPixels[ind + 3] = a;
  }
  inline void setPixel(int ind, const Vec3d &col, double a = 1) {
    ind = ind * 4;
    rgbaPixels[ind + 0] = clamp(col.x, 0.0, 1.0);
    rgbaPixels[ind + 1] = clamp(col.y, 0.0, 1.0);
    rgbaPixels[ind + 2] = clamp(col.z, 0.0, 1.0);
    rgbaPixels[ind + 3] = a;
  }

  inline void setVoxel(int x, int y, int z, const Vec4d &col) {
    int ind = (x + y*w + z*w*h) * 4;
    if(ind >= 0 && ind < rgbaPixels.size()) {
      rgbaPixels[ind + 0] = clamp(col.x, 0.0, 1.0);
      rgbaPixels[ind + 1] = clamp(col.y, 0.0, 1.0);
      rgbaPixels[ind + 2] = clamp(col.z, 0.0, 1.0);
      rgbaPixels[ind + 3] = clamp(col.w, 0.0, 1.0);
    }
  }

  inline void clearGl() {
    if(pixelType == PixelType::RGBA) {
      glClearTexImage(textureID, 0, GL_RGBA, GL_FLOAT, 0);
    }
    if(pixelType == PixelType::FLOAT) {
      glClearTexImage(textureID,  0,  GL_RED, GL_FLOAT, 0);
    }
    if(pixelType == PixelType::INT) {
      glClearTexImage(textureID,  0, GL_RED_INTEGER, GL_INT, 0);
    }
    if(pixelType == PixelType::UINT) {
      glClearTexImage(textureID, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, 0);
    }
    checkGlError("Texture.createTexture()", "glClearTexImage()");
  }


  inline void clear(const Vec4d &color) {
    clear(color.x, color.y, color.z, color.w);
  }

  void clear(double r, double g, double b, double a) {
    if(pixelType != PixelType::RGBA) {
      printf("Error at Texture clear(double, double, double, double), funtion only supported with PixelType::RGBA\n");
      return;
    }
    for(int i=0; i<w*h; i++) {
      int ind = i * 4;
      rgbaPixels[ind+0] = r;
      rgbaPixels[ind+1] = g;
      rgbaPixels[ind+2] = b;
      rgbaPixels[ind+3] = a;
    }
    applyPixels();
  }
  void clear(int value) {
    if(pixelType == PixelType::RGBA) {
      printf("Error at Texture clear(int), funtion not supported with PixelType::RGBA\n");
      return;
    }
    if(pixelType == PixelType::INT) {
      for(int i=0; i<w*h; i++) {
        intPixels[i] = value;
      }
    }
    if(pixelType == PixelType::UINT) {
      for(int i=0; i<w*h; i++) {
        uintPixels[i] = value;
      }
    }
    if(pixelType == PixelType::FLOAT) {
      for(int i=0; i<w*h; i++) {
        floatPixels[i] = value;
      }
    }
    applyPixels();
  }
  void clear(unsigned int value) {
    if(pixelType == PixelType::RGBA) {
      printf("Error at Texture clear(unsigned int), funtion not supported with PixelType::RGBA\n");
      return;
    }
    if(pixelType == PixelType::INT) {
      for(int i=0; i<w*h; i++) {
        intPixels[i] = value;
      }
    }
    if(pixelType == PixelType::UINT) {
      for(int i=0; i<w*h; i++) {
        uintPixels[i] = value;
      }
    }
    if(pixelType == PixelType::FLOAT) {
      for(int i=0; i<w*h; i++) {
        floatPixels[i] = value;
      }
    }
    applyPixels();
  }
  void clear(float value) {
    if(pixelType == PixelType::RGBA) {
      printf("Error at Texture clear(float), funtion not supported with PixelType::RGBA\n");
      return;
    }
    if(pixelType == PixelType::INT) {
      int intValue = (int)value;
      for(int i=0; i<w*h; i++) {
        intPixels[i] = intValue;
      }
    }
    if(pixelType == PixelType::UINT) {
      unsigned int uintValue = (unsigned int)value;
      for(int i=0; i<w*h; i++) {
        uintPixels[i] = uintValue;
      }
    }
    if(pixelType == PixelType::FLOAT) {
      for(int i=0; i<w*h; i++) {
        floatPixels[i] = value;
      }
    }
    applyPixels();
  }

  void clear() {
    if(pixelType == PixelType::RGBA) {
      rgbaPixels.assign(rgbaPixels.size(), 0);
    }
    if(pixelType == PixelType::FLOAT) {
      floatPixels.assign(floatPixels.size(), 0);
    }
    if(pixelType == PixelType::INT) {
      intPixels.assign(intPixels.size(), 0);
    }
    if(pixelType == PixelType::UINT) {
      uintPixels.assign(uintPixels.size(), 0);
    }
    applyPixels();
  }

  void clearArrayLayer(int layer) {

    if(pixelType == PixelType::RGBA) {
      memset(rgbaPixels.data() + layer * w*h*4, 0, w*h*4*sizeof(float));
    }
    if(pixelType == PixelType::FLOAT) {
      memset(floatPixels.data() + layer * w*h, 0, w*h*sizeof(float));
    }
    if(pixelType == PixelType::INT) {
      memset(intPixels.data() + layer * w*h, 0, w*h*sizeof(int));
    }
    if(pixelType == PixelType::UINT) {
      memset(uintPixels.data() + layer * w*h, 0, w*h*sizeof(unsigned int));
    }
    applyPixels(0, 0, w, h, layer, 1);
  }

  inline void colorify(const Vec4d &color) {
    colorify(color.x, color.y, color.z, color.w);
  }
  void colorify(double r, double g, double b, double a) {
    for(int i=0; i<w*h; i++) {
      int ind = i * 4;
      rgbaPixels[ind+0] *= r;
      rgbaPixels[ind+1] *= g;
      rgbaPixels[ind+2] *= b;
      rgbaPixels[ind+3] *= a;
    }
    applyPixels();
  }


  void invertValue() {
    if(pixelType == PixelType::RGBA) {
      for(int i=0; i<w*h; i+=4) {
        rgbaPixels[i+0] = 1.0 - rgbaPixels[i+0];
        rgbaPixels[i+1] = 1.0 - rgbaPixels[i+1];
        rgbaPixels[i+2] = 1.0 - rgbaPixels[i+2];
      }
    }
    if(pixelType == PixelType::FLOAT) {
      for(int i=0; i<w*h; i++) {
        floatPixels[i] = 1.0 - floatPixels[i];
      }
    }
    applyPixels();
  }

  void desaturate() {
    for(int i=0; i<w*h; i+=4) {
      Vec3d hsv = rgbToHsv(rgbaPixels[i+0], rgbaPixels[i+1], rgbaPixels[i+2]);
      rgbaPixels[i+0] = hsv.z;
      rgbaPixels[i+1] = hsv.z;
      rgbaPixels[i+2] = hsv.z;
      //rgbaPixels[i+3] = rgbaPixels[i+3];
    }
    applyPixels();
  }



  /*void transform(Image &imgDst, const Eigen::Matrix3d &m) {
    unsigned int *rgbaPixelsSrc, *rgbaPixelsDst;
    accessPixels(&rgbaPixelsSrc);
    imgDst.accessPixels(&rgbaPixelsDst);
    memset(rgbaPixelsDst, 0, sizeof(unsigned int)*imgDst.w*imgDst.h);

    Eigen::Vector3d v, t;

    for(int i=0; i<w; i++) {
      for(int j=0; j<h; j++) {
        v << i, j, 1;
        t = m * v;
        if(t(0) >= 0 && t(0) < imgDst.w && t(1) >= 0 && t(1) < imgDst.h) {
          int ind = int(t(0)) + int(t(1)) * imgDst.h;
          rgbaPixelsDst[ind] = rgbaPixelsSrc[i+j*w];
        }
      }
    }

  }*/

  void swapRenderTarget() {
    secondaryTextureFBOSwapState = !secondaryTextureFBOSwapState;
    setRenderTarget();
  }

  void setRenderTarget() {
    if(textureType == TextureType::TextureArray2D) {
      printf("Error at Texture::setRenderTarget(), function not supported with TextureType::TextureArray2D\n");
      return;
    }
    if(fboID == 0) {
      glGenFramebuffers(1, &fboID);
      checkGlError("Texture.setRenderTarget()", "glGenFramebuffers(1, &fboID)");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, fboID);
    checkGlError("Texture.setRenderTarget()", "glBindFramebuffer(GL_FRAMEBUFFER, fboID)");


    GLuint textureID =  (useSecondaryTexture && secondaryTextureFBOSwapState) ? this->texture2ID : this->textureID;

    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0);
    checkGlError("Texture.setRenderTarget()", "glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0)");
    //glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, textureID, 0);

    //glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_COMPONENT24, GL_TEXTURE_2D, depthBufferID, 0);
    //checkGlError("Texture.setRenderTarget()", "glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0)");

    //glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBufferID);
    //checkGlError("Texture.setRenderTarget()", "glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBufferID)");


    /*GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers
    checkGlError("Texture.setRenderTarget()", "glDrawBuffers(1, DrawBuffers)");*/


    GLenum fboStatus = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);

    if(fboStatus != GL_FRAMEBUFFER_COMPLETE) {
      printf("Error at Texture.setRenderTarget: incomplete FBO (");
      if(fboStatus == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT) printf("GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT)\n");
      else if(fboStatus == GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT) printf("GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT)\n");
      else if(fboStatus == GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER) printf("GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER)\n");
      else if(fboStatus == GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER) printf("GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER)\n");
      else if(fboStatus == GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE) printf("GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE)\n");
      else if(fboStatus == GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS) printf("GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS)\n");
      else if(fboStatus == GL_FRAMEBUFFER_UNSUPPORTED) printf("GL_FRAMEBUFFER_UNSUPPORTED)\n");
      else printf("unknown error)\n");
    }
  }

  void unsetRenderTarget() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0​, GL_TEXTURE_2D, textureID, 0);
  }

  void activateImageStorage(GlShader &shader, const std::string &textureNameGLSL, int textureUnit) {
    GLuint textureID =  (useSecondaryTexture && !secondaryTextureFBOSwapState) ? this->texture2ID : this->textureID;

    int textureLocation = glGetUniformLocation(shader.program, textureNameGLSL.c_str());
    checkGlError("Texture.activateImageStorage()", "glGetUniformLocation, "+ shader.nameFrag + ", " + textureNameGLSL);

    glUniform1i(textureLocation, textureUnit);
    checkGlError("Texture.activateImageStorage()", "glUniform1i()");

    glActiveTexture(GL_TEXTURE0 + textureUnit);
    checkGlError("Texture.activate()", "glActiveTexture(GL_TEXTURE0)");

    if(pixelType == PixelType::RGBA) {
      glBindImageTexture(textureUnit, textureID, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
    }
    if(pixelType == PixelType::FLOAT) {
      glBindImageTexture(textureUnit, textureID, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    }
    if(pixelType == PixelType::INT) {
      glBindImageTexture(textureUnit, textureID, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32I);
    }
    if(pixelType == PixelType::UINT) {
      glBindImageTexture(textureUnit, textureID, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
    }
    checkGlError("Texture.activateImageStorage()", "glBindImageTexture();");

  }

  void activate(GlShader &shader, const std::string &textureNameGLSL, int textureUnit) {
    GLuint textureID =  (useSecondaryTexture && !secondaryTextureFBOSwapState) ? this->texture2ID : this->textureID;

    int textureLocation = glGetUniformLocation(shader.program, textureNameGLSL.c_str());
    checkGlError("Texture.activate()", "glGetUniformLocation, "+ shader.nameFrag + ", " + textureNameGLSL);

    glUniform1i(textureLocation, textureUnit);
    checkGlError("Texture.activate()", "glUniform1i()");

    glActiveTexture(GL_TEXTURE0 + textureUnit);
    checkGlError("Texture.activate()", "glActiveTexture(GL_TEXTURE0)");

    if(textureType == TextureType::Texture2D) {
      glBindTexture(GL_TEXTURE_2D, textureID);
      checkGlError("Texture.activate()", "glBindTexture(GL_TEXTURE_2D, textureID);");
    }
    if(textureType == TextureType::TextureArray2D) {
      glBindTexture(GL_TEXTURE_2D_ARRAY, textureID);
      checkGlError("Texture.activate()", "glBindTexture(GL_TEXTURE_2D_ARRAY, textureID);");
    }

    /*GLenum error;
    while((error = glGetError()) != GL_NO_ERROR) {
      printf("Error at Texture.activate: %s. %s, %s, %s, %d, (%d x %d)\n", gluErrorString(error), shader.nameVert.c_str(), shader.nameFrag.c_str(), textureNameGLSL.c_str(), textureUnit, w, h);
    }*/
  }
  void inactivate(int textureUnit) {
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    checkGlError("Texture.inactivate()", "glActiveTexture(GL_TEXTURE0)");

    if(textureType == TextureType::Texture2D) {
      glBindTexture(GL_TEXTURE_2D, 0);
      checkGlError("Texture.inactivate()", "glBindTexture(GL_TEXTURE_2D, 0);");
    }
    if(textureType == TextureType::TextureArray2D) {
      glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
      checkGlError("Texture.inactivate()", "glBindTexture(GL_TEXTURE_2D_ARRAY, 0);");
    }
  }
  void activate() {
    GLuint textureID =  (useSecondaryTexture && !secondaryTextureFBOSwapState) ? this->texture2ID : this->textureID;

    glActiveTexture(GL_TEXTURE0);
    checkGlError("Texture.activate()", "glActiveTexture(GL_TEXTURE0)");

    if(textureType == TextureType::Texture2D) {
      glBindTexture(GL_TEXTURE_2D, textureID);
      checkGlError("Texture.activate()", "glBindTexture(GL_TEXTURE_2D, textureID);");
    }
    if(textureType == TextureType::TextureArray2D) {
      glBindTexture(GL_TEXTURE_2D_ARRAY, textureID);
      checkGlError("Texture.activate()", "glBindTexture(GL_TEXTURE_2D_ARRAY, textureID);");
    }

  }
  void inactivate() {
    glActiveTexture(GL_TEXTURE0);
    checkGlError("Texture.inactivate()", "glActiveTexture(GL_TEXTURE0)");

    if(textureType == TextureType::Texture2D) {
      glBindTexture(GL_TEXTURE_2D, 0);
      checkGlError("Texture.inactivate()", "glBindTexture(GL_TEXTURE_2D, 0);");
    }
    if(textureType == TextureType::TextureArray2D) {
      glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
      checkGlError("Texture.inactivate()", "glBindTexture(GL_TEXTURE_2D_ARRAY, 0);");
    }
  }
};



/*void setRenderTarget(Image &img, SDLInterface *sdlInterface) {
  int ret = SDL_SetRenderTarget(sdlInterface->renderer, img.texture);
  if(ret != 0) {
    printf("Error. Unable to set render target.\n");
  }
}
void setDefaultRenderTarget(SDLInterface *sdlInterface) {
  int ret = SDL_SetRenderTarget(sdlInterface->renderer, NULL);
  if(ret != 0) {
    printf("Error. Unable to set default render target.\n");
  }
}*/
