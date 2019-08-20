#pragma once


#include "util.h"
#include "gfxwrap.h"
#include "image.h"
//#include "fontl.h"
#include <cstdio>
#include <string>
#include <cstring>
#include <vector>
#include <unordered_map>
#include <set>

/*struct FontCharacter {
  int id = 0;
  int x = 0, y = 0, w = 0, h = 0;
  int xOffset = 0, yOffset = 0;
  int xAdvance = 0;
};*/

struct FontCharacter {
  int id = 0;
  int x = 0, y = 0, w = 0, h = 0;
  int xOffset = 0, yOffset = 0;
  int xAdvance = 0;
};


struct TextGl
{
  struct FontDescription {
    //std::vector<FontCharacter> fontChars;
    std::unordered_map<int, FontCharacter> idFontCharMap;
    std::string name = "";
    std::set<int> missingCharacters;
    int size = 0;
    int bold = 0, italic = 0, smooth = 0;
    int spacingA = 0, spacingB = 0;
    int lineHeight = 0, base = 0;
    int scaleW = 0, scaleH = 0;
  };

  struct FontCharacterxx {
    std::unordered_map<int, Texture*> characterImages;
    FontDescription *fontDescription = NULL;

    ~FontCharacterxx() {
      for(auto &intTexturePair : characterImages) {
        delete intTexturePair.second;
      }
    }
  };

  std::unordered_map<unsigned int, FontDescription*> fontDescriptions;
  std::unordered_map<unsigned int, FontCharacterxx> sizeFontCharacterxMap;

  int tabSize = 4;
  bool initialized = false;

  Vec4d textColor;

  ~TextGl() {
    for(auto &uintFontDescPair : fontDescriptions) {
      delete uintFontDescPair.second;
    }
  }

  void setColor(const Vec4d &textColor) {
    this->textColor = textColor;
  }
  void setColor(double r, double g, double b, double a) {
    this->textColor.set(r, g, b, a);
  }

  void setDefaultColor() {
    this->textColor.set(1, 1, 1, 1);
  }

  double getLineHeight(int size, double scale = 1.0) {
    int actualSize = (int)(scale * size);
    int closestInitialSize = getClosestKeyAboveOrBelow(actualSize, fontDescriptions);
    
    double scaleFactor = (double)actualSize/closestInitialSize;
    FontDescription *fontDescription = fontDescriptions[closestInitialSize];
    if(!fontDescription) {
     printf("Error at TextGL.getLineHeight: Couldn't get font description size %d!\n", closestInitialSize);
     return 20;
    }
    return scaleFactor*fontDescription->lineHeight;
  }

  Vec2d getDimensions(std::string str, int size, double scale, SDLInterface *sdlInterface = NULL) {
    return getDimensions(to_wstring(str), 0.0, 0.0, size, scale, sdlInterface);
  }
  Vec2d getDimensions(std::string str, int size, SDLInterface *sdlInterface = NULL) {
    return getDimensions(to_wstring(str), 0.0, 0.0, size, 1.0, sdlInterface);
  }
  Vec2d getDimensions(std::wstring str, int size, SDLInterface *sdlInterface = NULL) {
    return getDimensions(str, 0.0, 0.0, size, 1.0, sdlInterface);
  }
  Vec2d getDimensions(std::wstring str, int size, double scale, SDLInterface *sdlInterface = NULL) {
    return getDimensions(str, 0.0, 0.0, size, scale, sdlInterface);
  }

  Vec2d getDimensions(std::wstring str, double x, double y, int size, double scale, SDLInterface *sdlInterface = NULL) {
    if(!initialized) {
      printf("Error at TextGl.print: Font not initialized.\n");
      return Vec2d();
    }

    int actualSize = (int)(scale * size);
    if(actualSize < 6) {
      printf("Error at TextGl.getDimensions: too small font size %d\n", size);
      return Vec2d();
    }
    int closestInitialSize = getClosestKeyAboveOrBelow(actualSize, fontDescriptions);

    if(closestInitialSize == -1 || fontDescriptions.count(closestInitialSize) == 0) {
      printf("Error at TextGl.getDimensions: no appropriate font size found %d\n", closestInitialSize);
      return Vec2d();
    }

    double scaleFactor = (double)actualSize/closestInitialSize;
    FontDescription *fontDescription = fontDescriptions[closestInitialSize];

    Vec2d d(0, scaleFactor*fontDescription->lineHeight);

    if(str.length() < 1) return d;

    double maxX = 0;

    for(int i=0; i<str.length(); i++) {
      int c = str[i];
      if(c == '\n') {
        d.y += fontDescription->lineHeight * scaleFactor;
        d.x = 0;
      }
      else if(c == ' ') {
        d.x += fontDescription->lineHeight * 0.18 * scaleFactor;
      }
      else if(c == '\t') {
        double tx = fontDescription->lineHeight * 0.18 * scaleFactor * tabSize;
        double sx = tx;
        while(sx <= d.x) {
          sx += tx;
        }
        d.x = sx;
      }
      else {
        if(sdlInterface) {
          double cw = doNotDrawCharacter(c, actualSize, fontDescription);
          if(x + d.x + cw >= sdlInterface->screenW) {
            d.y += fontDescription->lineHeight * scaleFactor;
            d.x = 0;
          }
        }

        d.x += doNotDrawCharacter(c, scaleFactor, fontDescription);
        //d.x += fontDescription->lineHeight * 0.05 * scaleFactor;
      }
      maxX = max(d.x, maxX);
    }
    return Vec2d(maxX, d.y);
  }

  Vec2d print(std::string str, const Vec2d &pos, int size, double scale = 1.0, SDLInterface *sdlInterface = NULL) {
    return print(to_wstring(str), pos.x, pos.y, size, scale, sdlInterface);
  }

  Vec2d print(std::string str, double x, double y, int size, double scale, SDLInterface *sdlInterface = NULL) {
    return print(to_wstring(str), x, y, size, scale, sdlInterface);
  }
  Vec2d print(std::string str, double x, double y, int size, SDLInterface *sdlInterface = NULL) {
    return print(to_wstring(str), x, y, size, 1.0, sdlInterface);
  }
  Vec2d print(std::wstring str, double x, double y, int size, SDLInterface *sdlInterface = NULL) {
    return print(str, x, y, size, 1.0, sdlInterface);
  }

  Vec2d print(std::wstring str, double x, double y, int size, double scale, SDLInterface *sdlInterface = NULL) {
    if(!initialized) {
      printf("Error at TextGl.print: Font not initialized.\n");
      return  Vec2d();
    }
    int actualSize = (int)(scale * size);
    if(actualSize < 6) {
      printf("Error at TextGl.print: too small font size %d\n", size);
      return Vec2d();
    }
    int closestInitialSize = getClosestKeyAboveOrBelow(actualSize, fontDescriptions);
    if(closestInitialSize == -1) {
      printf("Error which should not have happened in any case happened at TextGl.print.\n");
      return Vec2d();
    }
    double scaleFactor = (double)actualSize/closestInitialSize;
    FontDescription *fontDescription = fontDescriptions[closestInitialSize];

    if(sizeFontCharacterxMap.count(actualSize) == 0) {
      prepareScaledCharacterImages(actualSize, fontDescription);
    }

    Vec2d d;
    double maxX = 0;

    for(int i=0; i<str.length(); i++) {
      int c = str[i];

      if(c == '\n') {
        d.y += fontDescription->lineHeight * scaleFactor;
        d.x = 0;
      }
      else if(c == ' ') {
        d.x += fontDescription->lineHeight * 0.18 * scaleFactor;
      }
      else if(c == '\t') {
        double tx = fontDescription->lineHeight * 0.18 * scaleFactor * tabSize;
        double sx = tx;
        while(sx <= d.x) {
          sx += tx;
        }
        d.x = sx;
      }
      else {
        if(sdlInterface) {
          double cw = doNotDrawCharacter(c, actualSize, fontDescription);
          if(x + d.x + cw >= sdlInterface->screenW) {
            d.y += fontDescription->lineHeight * scaleFactor;
            d.x = 0;
          }
        }

        d.x += drawCharacter(c, x+d.x, y+d.y, actualSize, fontDescription);
        //d.x += fontDescription->lineHeight * 0.05 * scaleFactor;
      }

      maxX = max(d.x, maxX);

    }
    return Vec2d(maxX, d.y + fontDescription->lineHeight*scaleFactor);
  }

private:

  void prepareInitialCharacterImages(Texture *fontImage, FontDescription *fontDescription) {
    printf("TextGl: Creating initial font textures, \"%s\",  size %d\n", fontDescription->name.c_str(), fontDescription->size);
    for(const auto &idFontChar : fontDescription->idFontCharMap) {
      int id = idFontChar.first;
      //FontCharacter &fc = idFontChar.second;
      FontCharacter &fc = fontDescription->idFontCharMap[id];
      if(fc.w > 0 && fc.h > 0) {
        Texture *characterImg = fontImage->copyArea(fc.x, fc.y, fc.w, fc.h);
        if(!characterImg) {
          printf("Error at TextGl.prep.init.: Failed to create character texture (%dx%d) for char %c, id %d\n", fc.w, fc.h, id, id);
        }
        else {
          sizeFontCharacterxMap[fontDescription->size].characterImages[id] = characterImg;
        }
      }
      else {
        sizeFontCharacterxMap[fontDescription->size].characterImages[id] = NULL;
      }
    }
  }
  void prepareScaledCharacterImages(int size, FontDescription *fontDescription) {
    printf("TextGl: Creating scaled font textures, \"%s\",  size %d\n", fontDescription->name.c_str(), size);
    for(const auto &idFontChar : fontDescription->idFontCharMap) {
      int id = idFontChar.first;
      Texture *characterImg = sizeFontCharacterxMap[fontDescription->size].characterImages[id];
      if(characterImg) {
        double scale = (double)size/fontDescription->size;
        int w = (int)(scale * characterImg->w);
        int h = (int)(scale * characterImg->h);
        if(w == 0 || h == 0) {
          sizeFontCharacterxMap[size].characterImages[id] = NULL;
        }
        else {
          //printf("id %d, %dx%d -> %dx%d\n", id, characterImg->w, characterImg->h, w, h);
          Texture *scaledCharacterImg = new Texture();
          if(scaledCharacterImg->create(w, h)) {
            characterImg->copyBilinear(*scaledCharacterImg);
            sizeFontCharacterxMap[size].characterImages[id] = scaledCharacterImg;
          }
          else {
            printf("Error at TextGl.prepareScaledCharacterImages: failed to create texture (%d x %d), id %d\n", w, h, id);
          }
        }
      }
      else {
        sizeFontCharacterxMap[size].characterImages[id] = NULL;
      }
    }
  }

  double drawCharacter(int id, double x, double y, int actualSize, FontDescription *fontDescription) {
    /*int actualSize = (int)(scaleFactor * fontDescription->size);
    if(actualSize < 6) {
      printf("Error at Fontx.drawCharacter: too small font size %d\n", actualSize);
      return 0;
    }*/
    if(fontDescription->idFontCharMap.count(id) == 0) {
      if(fontDescription->missingCharacters.count(id) == 0) {
        fontDescription->missingCharacters.insert(id);
        //printf("Warning at TextGl.drawCharacter: id %d not found in font description\n", id);
      }
      return 0;
    }

    double scaleFactor = (double)actualSize / fontDescription->size;

    if(sizeFontCharacterxMap.count(actualSize) == 0) {
      prepareScaledCharacterImages(actualSize, fontDescription);
    }

    if(sizeFontCharacterxMap[actualSize].characterImages.count(id) == 0) {
      //printf("Error at TextGl.drawCharacter: id %d not found in character images\n", id);
      return 0;
    }


    FontCharacter &fc = fontDescription->idFontCharMap[id];
    Texture *charImage = sizeFontCharacterxMap[actualSize].characterImages[id];
    if(charImage != NULL) {
      charImage->useBottomRightUV = false;
      charImage->render(int(x+fc.xOffset*scaleFactor), int(y+fc.yOffset*scaleFactor), textColor);
    }
    /*FontCharacter &fc = idFontCharMap[id];
    fontImages[size]->render(fc.x*scale, fc.y*scale, fc.w*scale, fc.h*scale, x+fc.xOffset*scale, y+fc.yOffset*scale, fc.w*scale, fc.h*scale, sdlInterface);*/
    return fc.xAdvance * scaleFactor;
  }


  double doNotDrawCharacter(int id, double scaleFactor, FontDescription *fontDescription) {
    if(fontDescription->idFontCharMap.count(id) < 1) {
      if(fontDescription->missingCharacters.count(id) == 0) {
        fontDescription->missingCharacters.insert(id);
        //printf("Warning at TextGl.doNotDrawCharacter: id %d not found\n", id);
      }
      return 0;
    }
    FontCharacter &fc = fontDescription->idFontCharMap[id];
    return fc.xAdvance * scaleFactor;
  }


public:

  void load(const char *descriptionFilename, const char *imageFilename) {

    FontDescription *fontDescription = new FontDescription();

    TextFile descriptionFile;
    descriptionFile.readContent(descriptionFilename);
    int numCharacters = 0;

    if(descriptionFile.status == File::CONTENT_READ) {
      std::string str(descriptionFile.content);
      std::string line;
      int i = 0;
      int lineNumber = 0;
      while(true) {
        line = nextLine(str, i);
        if(line.length() == 0) break;
        int ret = 0;
        lineNumber++;

        if(line.find("info") != -1) {
          int ind = line.find("size");
          ret = sscanf(line.substr(ind, line.length()-1).c_str(), "size=%d bold=%d italic=%d smooth=%d spacing=%d,%d", &fontDescription->size, &fontDescription->bold, &fontDescription->italic, &fontDescription->smooth, &fontDescription->spacingA, &fontDescription->spacingB);
          if(ret > 0) {
            //this->name = nameStr;
          }
          else {
            printf("Error at Font.load: invalid description file line %d:\n%s\n", lineNumber, line.c_str());
          }
        }
        else if(line.find("common") != -1) {
          ret = sscanf(line.c_str(), "common lineHeight=%d base=%d scaleW=%d scaleH=%d pages=%*s", &fontDescription->lineHeight, &fontDescription->base, &fontDescription->scaleW, &fontDescription->scaleH);
          if(ret > 0) {
            //printf("<><> %d %d %d %d\n", lineHeight, base, scaleW, scaleH);
          }
          else {
            printf("Error at Font.load: invalid description file line %d:\n%s\n", lineNumber, line.c_str());
          }
        }
        else if(line.find("char") != -1) {
          FontCharacter fc;
          ret = sscanf(line.c_str(), "char id=%d x=%d y=%d width=%d height=%d xoffset=%d yoffset=%d xadvance=%d page=%*s", &fc.id, &fc.x, &fc.y, &fc.w, &fc.h, &fc.xOffset, &fc.yOffset, &fc.xAdvance);
          if(ret > 0) {
            fontDescription->idFontCharMap[fc.id] = fc;
            //printf("%d %d %d", fc.id, fc.x, fc.y);
            numCharacters++;
          }
          else {
            printf("Error at Font.load: invalid description file line %d:\n%s\n", lineNumber, line.c_str());
          }
        }
      }
    }
    if(numCharacters == 0) {
      printf("Error at Font.load: unable to read font desciption file %s\n", descriptionFilename);
      return;
    }

    textColor.set(1, 1, 1, 1);

    Texture *fontImage = new Texture(imageFilename);

    if(fontImage->textureID != 0) {
      initialized = true;
      prepareInitialCharacterImages(fontImage, fontDescription);
      fontDescription->name = extractFilename(descriptionFilename);
      fontDescriptions[fontDescription->size] = fontDescription;
    }
    else {
      printf("Error at TextGl.load: Couldn't load font texture %s\n", imageFilename);
    }
    delete fontImage;
  }

private:
  std::string nextLine(std::string str, int &index) {
    std::string ret("");
    int j = 0;
    j = str.find("\n", index);
    if(j == -1) return ret;
    ret = str.substr(index, j-index);
    index = j+1;
    return ret;
  }

};
