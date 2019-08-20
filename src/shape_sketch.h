#pragma once
#include "engine/sketch.h"

// FIXME
bool showCanvasTexture = false;
Vec4d canvasTextureOverlayColor;
Quadx canvasTextureQuad;
GlShader canvasTextureShader;


struct Brush {
  std::vector<Texture*> imgs;
  std::string name = "emptyBrush";

  ~Brush() {
    for(Texture *img : imgs) {
      delete img;
    }
  }

  bool loadImage(std::string filename) {
    Texture *img = new Texture();
    bool loaded = img->load(filename);
    if(loaded) {
      img->invertValue();
      imgs.push_back(img);
      name = filename.c_str();
      return true;
    }
    else {
      printf("Failed to load brush texture %s\n", filename.c_str());
      delete img;
      return false;
    }
  }

  // FIXME
  void loadImages(std::string filenameBase, int numImages) {

  }

};


struct CanvasTexture {
  Texture *imgOriginal = NULL;
  Texture *img = NULL;
  std::string name = "emptyCanvas";

  ~CanvasTexture() {
    if(imgOriginal) delete imgOriginal;
    if(img) delete img;
  }

  bool loadImage(std::string filename) {
    imgOriginal = new Texture();
    bool loaded = imgOriginal->load(filename);
    if(loaded) {
      name = filename.c_str();
      img = imgOriginal->getCopy();
      return true;
    }
    else {
      printf("Failed to load canvas texture %s\n", filename.c_str());
      delete img;
      return false;
    }
  }

  void reset() {
    resize(0, 0);
  }

  void resize(int w = 0, int h = 0) {
    delete img;
    if(w == 0 && h == 0) {
      img = imgOriginal->getCopy();
    }
    else {
      if(h == 0) {
        h = w * imgOriginal->h / imgOriginal->w;
      }
      img = imgOriginal->getCopy(w, h);
    }
  }

  void desaturate(bool desat) {
    if(desat) img->desaturate();
    else {
      int w = img->w, h = img->h;
      delete img;
      img = imgOriginal->getCopy(w, h);
    }
  }
};

struct Brushes
{
  const std::string fileExtension = ".brh";
  const std::string brushSettingsPath = "data/sketches/shapePainter/brushSettings/";

  FastNoise fastNoise;

  std::vector<Brush*> brushes;
  int activeBrushInd = 0;

  std::vector<CanvasTexture*> canvasTextures;
  int activeCanvasTextureInd = 0;
  bool useCanvasTexture = false;

  int size = 10;
  double densityMin = 0, densityMax = 1.0;
  Vec4d brushColor = Vec4d(0.6, 0, 0.3, 0.1);

  int brushVariationMode = 0;
  int activeSubBrush = 0;
  double brushVariationNoiseSpeed = 1.0;

  double randomOffsetMin = 0, randomOffsetMax = 0;

  const int numSmudges = 2;
  int smudgeAmount[2] = {1, 1};
  double smudgeRadius[2] = {0, 0};
  double smudgeNoiseSpeed[2] = {0.2, 0.2};
  double smudgeNoiseCounter[2] = {0, 0};

  int rotationMode = 0;
  double rotationMin = -40, rotationMax = 40;
  double rotationNoiseSpeed = 0.2;
  double rotationNoiseCounter = 0;

  int scaleMode = 0;
  double scaleMin = 0.5, scaleMax = 2.0;
  double scaleNoiseSpeed = 0.2;
  double scaleNoiseCounter = 0;

  bool useFeedbackCanvasTexture = false;

  Quadx brushQuad;
  GlShader shader;

  std::vector<std::string> brushNamesForSerializing;
  std::vector<std::string> canvasNamesForSerializing;
  std::string activeBrushNameForSerializing;
  std::string activeCanvasNameForSerializing;

  template<class Archive>
  void serialize(Archive &ar) {
  ar( brushNamesForSerializing, canvasNamesForSerializing,
      activeBrushNameForSerializing, activeCanvasNameForSerializing,
      activeBrushInd,
      size, densityMin, densityMax,
      brushColor,
      randomOffsetMin, randomOffsetMin,
      smudgeAmount, smudgeRadius, smudgeNoiseSpeed,
      rotationMode, rotationMin, rotationMax, rotationNoiseSpeed,
      scaleMode, scaleMin, scaleMax, scaleNoiseSpeed);
  }

  void saveBrushSettings(std::string filename, bool saveCanvases) {
    brushNamesForSerializing.clear();
    for(Brush *brush : brushes) {
      brushNamesForSerializing.push_back(brush->name);
    }

    canvasNamesForSerializing.clear();
    if(saveCanvases)  {
      for(CanvasTexture *canvasTexture : canvasTextures) {
        canvasNamesForSerializing.push_back(canvasTexture->name);
      }
    }

    filename = brushSettingsPath + filename + fileExtension;
    bool saved = saveCerealBinary(*this, filename);
    if(!saved) {
      messageQueue.addMessage("Failed to save brush settings \""+ filename + "\"");
    }
  }

  void loadBrushSettings(std::string filename, bool loadCanvases) {
    filename = brushSettingsPath + filename + fileExtension;
    bool loaded = loadCerealBinary(*this, filename);
    if(!loaded) {
      messageQueue.addMessage("Failed to load brush settings \""+ filename + "\"");
      return;
    }

    for(std::string &str : brushNamesForSerializing) {
      bool brushAlreadyLoaded = false;
      for(Brush *brush : brushes) {
        if(str.compare(brush->name) == 0) {
          brushAlreadyLoaded = true;
        }
      }
      if(!brushAlreadyLoaded) {
        loadBrushTexture(str);
      }
    }
    for(int i=0; i<brushes.size(); i++) {
      if(activeBrushNameForSerializing.compare(brushes[i]->name) == 0) {
        activeBrushInd = i;
        break;
      }
    }
    if(loadCanvases) {
      for(std::string &str : canvasNamesForSerializing) {
        bool canvasTesxtureAlreadyLoaded = false;
        for(CanvasTexture *canvasTexture : canvasTextures) {
          if(str.compare(canvasTexture->name) == 0) {
            canvasTesxtureAlreadyLoaded = true;
          }
        }
        if(!canvasTesxtureAlreadyLoaded) {
          loadCanvasTexture(str);
        }
      }
      for(int i=0; i<canvasTextures.size(); i++) {
        if(activeCanvasNameForSerializing.compare(canvasTextures[i]->name) == 0) {
          activeCanvasTextureInd = i;
          break;
        }
      }
    }
  }


  ~Brushes() {
    for(Brush *brush : brushes) {
      delete brush;
    }
    for(CanvasTexture *canvasTexture : canvasTextures)  {
      delete canvasTexture;
    }

  }

  Brushes() {}

  void init() {
    fastNoise.SetSeed(Random::getInt());
    fastNoise.SetFrequency(0.02);
    fastNoise.SetInterp(FastNoise::Hermite);
    fastNoise.SetNoiseType(FastNoise::Perlin);

    brushQuad.create(size, size);
    shader.create("data/glsl/texture.vert", "data/glsl/brush.frag");
    loadBrushTexture("data/brushes/1/comic_inking.png");
  }

  void reloadShader() {
    shader.remove();
    shader.create("data/glsl/texture.vert", "data/glsl/brush.frag");
  }

  void loadBrushTexture(std::string filename) {
    Brush *brush = new Brush();
    bool loaded = brush->loadImage(filename);
    if(loaded) {
      brushes.push_back(brush);
      activeBrushInd = brushes.size() - 1;
    }
    else delete brush;
  }
  void loadBrushImages(std::string filename, int numImages) {
    /*Brush *brush = new Brush();
    brush->loadImages(filename, numImages);
    brushes.push_back(brush);*/
  }
  void removeBrushTexture() {
    if(activeBrushInd >= brushes.size()) return;
    brushes.erase(brushes.begin()+activeBrushInd);
    if(activeBrushInd >= brushes.size()) {
      activeBrushInd = brushes.size()-1;
    }
    messageQueue.addMessage("Brush texture: " + (brushes.size() > 0 ? brushes[activeBrushInd]->name : ""));
  }
  void removeCanvasTexture() {
    if(activeCanvasTextureInd >= canvasTextures.size()) return;
    canvasTextures.erase(canvasTextures.begin()+activeCanvasTextureInd);
    if(activeCanvasTextureInd >= canvasTextures.size()) {
      activeCanvasTextureInd = canvasTextures.size()-1;
    }
    messageQueue.addMessage("Canvas texture: " + (canvasTextures.size() > 0 ? canvasTextures[activeCanvasTextureInd]->name : ""));
  }

  void selectNextBrush() {
    if(brushes.size() < 2) return;
    activeBrushInd = (activeBrushInd+1) % brushes.size();
    messageQueue.addMessage("Brush texture: " + brushes[activeBrushInd]->name);
  }
  void selectPreviousBrush() {
    if(brushes.size() < 2) return;
    activeBrushInd = (brushes.size()+activeBrushInd-1) % brushes.size();
    messageQueue.addMessage("Brush texture: " + brushes[activeBrushInd]->name);
  }

  void loadCanvasTexture(std::string filename) {
    CanvasTexture *canvasTexture = new CanvasTexture();
    bool loaded = canvasTexture->loadImage(filename);
    if(loaded) {
      canvasTextures.push_back(canvasTexture);
      activeCanvasTextureInd = canvasTextures.size() - 1;
      useCanvasTexture = true;
    }
    else delete canvasTexture;
  }
  void selectNextCanvas() {
    if(canvasTextures.size() < 2) return;
    activeCanvasTextureInd = (activeCanvasTextureInd+1) % canvasTextures.size();
    messageQueue.addMessage("Brush canvas texture: " + canvasTextures[activeCanvasTextureInd]->name);
  }
  void selectPreviousCanvas() {
    if(canvasTextures.size() < 2) return;
    activeCanvasTextureInd = (canvasTextures.size()+activeCanvasTextureInd-1) % canvasTextures.size();
    messageQueue.addMessage("Brush canvas texture: " + canvasTextures[activeCanvasTextureInd]->name);
  }

  CanvasTexture *getActiveCanvasTexture() {
    if(activeCanvasTextureInd >= canvasTextures.size()) return NULL;
    return canvasTextures[activeCanvasTextureInd];
  }

  void resetCanvasTexture() {
    if(activeCanvasTextureInd >= canvasTextures.size()) return;
    canvasTextures[activeCanvasTextureInd]->reset();
  }
  void resizeCanvasTexture(int w, int h) {
    if(activeCanvasTextureInd >= canvasTextures.size()) return;
    canvasTextures[activeCanvasTextureInd]->resize(w, h);
  }
  void desaturateCanvasTexture(bool desat) {
    if(activeCanvasTextureInd >= canvasTextures.size()) return;
    canvasTextures[activeCanvasTextureInd]->desaturate(desat);
  }

  void setSize(int size) {
    this->size = size;
  }

  void setOpacity(double opacity) {
    brushColor.w = opacity;
  }
  void setColor(Vec4d col) {
    brushColor = col;
  }


  void paint(Vec2d pos, Texture &actualCanvas) {
    if(activeBrushInd >= brushes.size()) return;
    //if(activeBrushImgs.size() == 0) return;
    Texture *brushTexture = brushes[activeBrushInd]->imgs[0];

    if(brushVariationMode == 1) {
      if(activeSubBrush >= brushes[activeBrushInd]->imgs.size()) {
        activeSubBrush = 0;
      }
      brushTexture = brushes[activeBrushInd]->imgs[activeSubBrush];
      activeSubBrush = (activeSubBrush+1) % brushes[activeBrushInd]->imgs.size();
    }
    if(brushVariationMode == 2) {
      brushTexture = brushes[activeBrushInd]->imgs[Random::getInt(0, brushes[activeBrushInd]->imgs.size()-1)];
    }
    if(brushVariationMode == 3) {
      int ind = (int)map(fastNoise.GetNoise(pos.x*brushVariationNoiseSpeed, pos.y*brushVariationNoiseSpeed), -1.0, 1.0, 0.0, (double)brushes[activeBrushInd]->imgs.size()-1);
      ind = clamp(ind, 0, (int)brushes[activeBrushInd]->imgs.size()-1);
      brushTexture = brushes[activeBrushInd]->imgs[ind];
    }

    int w = 0, h = 0;
    if(brushTexture->w >= brushTexture->h) {
      w = (int)((double)brushTexture->w/brushTexture->h*size);
      h = size;
    }
    else {
      h = (int)((double)brushTexture->h/brushTexture->w*size);
      w = size;
    }

    brushQuad.setSize(w, h);

    shader.activate();
    shader.setUniform4f("brushColor", brushColor);
    shader.setUniform1i("useCanvasTexture", useCanvasTexture ? 1 : 0);
    brushTexture->activate(shader, "brushTexture", 0);

    Texture *canvasTexture = NULL;

    bool swapCanvasRenderTarget = false;

    if(useCanvasTexture && activeCanvasTextureInd < canvasTextures.size()) {
      swapCanvasRenderTarget = useFeedbackCanvasTexture;
      canvasTexture = useFeedbackCanvasTexture ? &actualCanvas : canvasTextures[activeCanvasTextureInd]->img;
      shader.setUniform2f("canvasTextureSize", canvasTexture->w, canvasTexture->h);
      canvasTexture->activate(shader, "canvasTexture", 1);
    }
    BoundingRect brect;
    brect.set(pos.x, pos.y, 0, 0);

    int n0 = max(1, smudgeAmount[0]), n1 = max(1, smudgeAmount[1]);
    for(int s0 = 0; s0 < n0; s0++) {
      Vec2d deltaPos0(fastNoise.GetNoise(85 + smudgeNoiseCounter[0],
                      942 + smudgeNoiseCounter[0]) * smudgeRadius[0],
                      fastNoise.GetNoise(534 + smudgeNoiseCounter[0],
                      -8531 + smudgeNoiseCounter[0]) * smudgeRadius[0]);
      smudgeNoiseCounter[0] += smudgeNoiseSpeed[0] / n0;

      for(int s1 = 0; s1 < n1; s1++) {
        Vec2d deltaPos1(fastNoise.GetNoise(1342 + smudgeNoiseCounter[1],
                        568 + smudgeNoiseCounter[1]) * smudgeRadius[1],
                       fastNoise.GetNoise(-572 + smudgeNoiseCounter[1],
                         7268 + smudgeNoiseCounter[1]) * smudgeRadius[1]);
        smudgeNoiseCounter[1] += smudgeNoiseSpeed[1] / (n0 * n1);

        /*Vec2d deltaPos(fastNoise.GetNoise(1342 + pos.x * smudgeNoiseSpeed[smudgeInd],
                                          568 + pos.y * smudgeNoiseSpeed[smudgeInd]) * smudgeRadius[smudgeInd],
                       fastNoise.GetNoise(-572 + pos.x * smudgeNoiseSpeed[smudgeInd],
                                          7268 + pos.y * smudgeNoiseSpeed[smudgeInd]) * smudgeRadius[smudgeInd]);*/
        deltaPos1 += deltaPos0;

        if(randomOffsetMax > 0) {
          deltaPos1 += getRandomVec2d(randomOffsetMin, randomOffsetMax);
        }

        double rotation = 0;
        if(rotationMode == 1) {
          rotation = Random::getDouble(rotationMin, rotationMax);
        }
        if(rotationMode == 2) {
          rotation = map(fastNoise.GetNoise(-4063 + rotationNoiseCounter, 693), -1.0, 1.0, rotationMin, rotationMax);
          rotationNoiseCounter += rotationNoiseSpeed / (n0 * n1);
        }

        double scale = 1.0;
        if(scaleMode == 1) {
          scale = Random::getDouble(scaleMin, scaleMax);
        }
        if(scaleMode == 2) {
          scale = map(fastNoise.GetNoise(7032 + scaleNoiseCounter, -5737), -1.0, 1.0, scaleMin, scaleMax);
          scaleNoiseCounter += scaleNoiseSpeed / (n0 * n1);
        }

        double x = pos.x + deltaPos1.x, y =  pos.y + deltaPos1.y;
        brushQuad.render(x, y, rotation*PI/180.0, scale);

        // FIXME get more limited area based on rotation
        double t = (brushQuad.w+brushQuad.h) * scale;
        brect.set(x, y, t, t);

        if(swapCanvasRenderTarget) {
          canvasTexture->swapRenderTarget();
          canvasTexture->renderSwapTexture(brect.getCenterX(), brect.getCenterY(),
                                           brect.getWidth(), brect.getHeight(), true);
          //canvasTexture->inactivate(1);
          shader.activate();
          canvasTexture->activate(shader, "canvasTexture", 1);
        }
      }
    }

    shader.deActivate();
    brushTexture->inactivate(0);
    if(canvasTexture) canvasTexture->inactivate(1);
  }

};


struct ShapePainter
{
  FastNoise fastNoise;
  int renderMode = 0;
  Vec2d pp, pn, p0, p1, p2, p3;
  int numSegments = 100000;
  double dist = 0;
  bool start = true, finished = false, paused = true;
  double delay = 0.002;
  double timeCounter = 0;
  double prevTime = 0;
  int pointIndex = 0;
  int segment = 0;
  int curveIndex = 0;

  bool clearCanvasOnStart = true;

  ShapePainter() {
    fastNoise.SetSeed(Random::getInt());
    fastNoise.SetFrequency(0.01);
    fastNoise.SetInterp(FastNoise::Hermite);
    fastNoise.SetNoiseType(FastNoise::SimplexFractal);
  }

  void reset() {
    finished = true;
    start = false;
    paused = true;
  }

  void begin() {
    finished = false;
    start = true;
    paused = false;
  }

  void togglePause() {
    paused = !paused;
  }

  void paint(Texture &canvasImg, Shape &shape, Brushes &brushes, double dt, Scene *scene, SDLInterface *sdlInterface) {
    if(paused || (finished && !start) || shape.curves.size() == 0 || shape.curves[0].size() < 2) {
      return;
    }
    canvasImg.setRenderTarget();

    timeCounter += dt;

    double density = map(fastNoise.GetNoise(pp.x, pp.y), -1.0, 1.0, brushes.densityMin, brushes.densityMax);


    if(start) {
      if(clearCanvasOnStart) {
        clear(0, 0, 0, 0, sdlInterface);

        if(showCanvasTexture) {
          CanvasTexture *canvasTexture = brushes.getActiveCanvasTexture();
          if(canvasTexture) {
            Texture *img = canvasTexture->img;
            canvasTextureShader.activate();
            canvasTextureShader.setUniform2f("screenSize", img->w, img->h);
            canvasTextureShader.setUniform4f("colorify", canvasTextureOverlayColor);
            canvasTextureShader.setUniform1i("usePixelCoordinates", 1);
            img->activate(canvasTextureShader, "tex", 0);
            canvasTextureQuad.render();
            canvasTextureShader.deActivate();
            img->inactivate(0);
          }
        }
        canvasImg.swapRenderTarget();
        canvasImg.renderSwapTexture();


      }
      start = false;
      finished = false;
      segment = 0;
      pointIndex = 0;
      curveIndex = 0;
      timeCounter = 0;
    }

    int brushStrokeCount = 0;
    if(delay < 0.0001) brushStrokeCount = 1000;
    else {
      while(timeCounter > delay) {
        timeCounter -= delay;
        brushStrokeCount++;
      }
    }

    while(brushStrokeCount > 0) {

      int numPoints = shape.curves[curveIndex].size();

      bool skip = false;

      if(pointIndex == 0 && segment == 0) {
        BoundingRect br(shape.curves[curveIndex].points);
        //br.render(scene, sdlInterface);
        if(!br.isVisible(scene, sdlInterface)) {
          curveIndex++;
          //messageQueue.addMessage("curve skipped");
          skip = true;
        }
      }

      if(numPoints < 2) {
        curveIndex++;
        pointIndex = 0;
        skip = true;
      }

      if(!skip) {
        if(pointIndex == 0 && segment == 0) {
          dist = density;
        }

        if(numPoints > 2 && shape.curves[curveIndex].ring) {
          while(brushStrokeCount > 0) {

            if(segment == 0) {
              p0 = scene->toScreen(shape.curves[curveIndex].points[(pointIndex+numPoints-1) % numPoints]);
              p1 = scene->toScreen(shape.curves[curveIndex].points[pointIndex]);
              p2 = scene->toScreen(shape.curves[curveIndex].points[(pointIndex+1) % numPoints]);
              p3 = scene->toScreen(shape.curves[curveIndex].points[(pointIndex+2) % numPoints]);
              pp = p1;
            }

            while(brushStrokeCount > 0) {
              double f = (double)segment/(numSegments-1.0);
              Vec2d pn = catmullRom(f, 0.5, p0, p1, p2, p3);
              if(dist >= density) {
                brushStrokeCount--;

                brushes.paint(pn, canvasImg);
                dist = 0;
              }
              dist += distance(pp, pn);
              pp = pn;
              segment++;

              if(segment >= numSegments) {
                segment = 0;
                brushStrokeCount = 0;
                pointIndex++;
              }
            }
            if(pointIndex >= numPoints) {
              curveIndex++;
              pointIndex = 0;
              brushStrokeCount = 0;
            }
          }
        }
        else {
          while(brushStrokeCount > 0) {

            if(segment == 0) {
              p0 = scene->toScreen(shape.curves[curveIndex].points[max(pointIndex-1, 0)]);
              p1 = scene->toScreen(shape.curves[curveIndex].points[pointIndex]);
              p2 = scene->toScreen(shape.curves[curveIndex].points[min(pointIndex+1, numPoints-1)]);
              p3 = scene->toScreen(shape.curves[curveIndex].points[min(pointIndex+2, numPoints-1)]);
              pp = p1;
            }

            while(brushStrokeCount > 0) {
              double f = (double)segment/(numSegments-1.0);
              Vec2d pn = catmullRom(f, 0.5, p0, p1, p2, p3);
              if(dist >= density) {
                brushStrokeCount--;

                brushes.paint(pn, canvasImg);
                dist = 0;
              }
              dist += distance(pp, pn);
              pp = pn;
              segment++;

              if(segment >= numSegments) {
                segment = 0;
                brushStrokeCount = 0;
                pointIndex++;
              }
            }
            if(pointIndex >= numPoints-1) {
              curveIndex++;
              pointIndex = 0;
              brushStrokeCount = 0;
            }
          }
        }

      }
      if(curveIndex >= shape.curves.size()) {
        curveIndex = 0;
        brushStrokeCount = 0;
        finished = true;
        start = false;
        printf("ShapePainter finished rendering\n");
      }
    }
    canvasImg.unsetRenderTarget();

    /*if(curveIndex == 0 && pointIndex == 0 && segment == 0) {
      finished = true;
      start = false;
    }*/
  }

  /*void paintAtOnce(Texture &canvasImg, Shape &shape, Brushes &brushes, SDLInterface *sdlInterface) {
    setRenderTarget(canvasImg, sdlInterface);

    if(start) {
      dist = brushes.density;
    }

    for(int curveIndex=0; curveIndex < shape.curves.size(); curveIndex++) {
      int n = shape.curves[curveIndex].size();
      if(n > 1) {
        int numSegments = 10000;
        double dist = brushes.density;

        if(n > 2 && shape.ring) {
          for(int pointIndex=0; pointIndex < n; pointIndex++) {
            Vec2d p0 = shape.curves[curveIndex][(pointIndex+n-1) % n];
            Vec2d p1 = shape.curves[curveIndex][pointIndex];
            Vec2d p2 = shape.curves[curveIndex][(pointIndex+1) % n];
            Vec2d p3 = shape.curves[curveIndex][(pointIndex+2) % n];

            pp = p1;
            for(int segment=0; segment<numSegments; segment++) {
              double f = (double)segment/(numSegments-1.0);
              Vec2d pn = catmullRom(f, 0.5, p0, p1, p2, p3);
              if(dist >= brushes.density) {
                brushes.paint(pn, sdlInterface);
                dist = 0;
              }
              dist += distance(pp, pn);
              pp = pn;
            }
          }
        }
        else {
          for(int pointIndex=0; pointIndex < n-1; pointIndex++) {
            Vec2d p0 = shape.curves[curveIndex][max(pointIndex-1, 0)];
            Vec2d p1 = shape.curves[curveIndex][pointIndex];
            Vec2d p2 = shape.curves[curveIndex][min(pointIndex+1, n-1)];
            Vec2d p3 = shape.curves[curveIndex][min(pointIndex+2, n-1)];

            Vec2d pp = p1;
            for(int segment=1; segment<numSegments; segment++) {
              double f = (double)segment/(numSegments-1.0);
              Vec2d pn = catmullRom(f, 0.5, p0, p1, p2, p3);
              drawLine(pp, pn, sdlInterface);
              pp = pn;
            }
          }
        }
      }
    }
    setDefaultRenderTarget(sdlInterface);
  }*/

};



struct CharacterShapeSet {
  //std::unordered_map<char, Shape> idShapeMap;
  std::unordered_map<wchar_t, Shape> widShapeMap;

  //bool load8bit = false, save8bit = false;

  template<class Archive>
  void serialize(Archive &ar) {
    ar(widShapeMap);
  }
  /*template<class Archive>
  void save(Archive &ar) const {
    if(save8bit) ar(idShapeMap);
    else ar(widShapeMap);
  }
  template<class Archive>
  void load(Archive &ar) {
    if(load8bit) {
      ar(idShapeMap);
      for(auto &charShapePair : idShapeMap) {
        widShapeMap[charShapePair.first] = charShapePair.second;
      }
    }
    else ar(widShapeMap);
  }*/

  void setShape(wchar_t id, Shape &shape) {
    widShapeMap[id] = shape;
  }

  bool isShapeDefined(wchar_t id) {
    return widShapeMap.count(id) > 0;
  }

  void getShape(wchar_t id, Shape &shape) {
    if(widShapeMap.count(id) > 0) {
     shape = widShapeMap[id];
    }
  }

  void clear() {
    widShapeMap.clear();
  }

  std::wstring getDefinedCharacters() {
    std::wstring wstrOut = L"";
    int n = widShapeMap.size();
    int i = 0;
    for(auto &widShapePair : widShapeMap) {
      wstrOut += widShapePair.first;
      if(i < n-1) wstrOut += L", ";
      i++;
    }
    return wstrOut;
  }
};

struct ShapeSet {
  std::vector<Shape> shapes;
  Shape shape;

  virtual ~ShapeSet() {}

  void clear() {
    for(int i=0; i<shapes.size(); i++) {
      shapes[i].clear();
    }
    shapes.clear();
  }

  void combine() {
    shape.clear();
    for(int i=0; i<shapes.size(); i++) {
      for(int k=0; k<shapes[i].curves.size(); k++) {
        shape.curves.push_back(shapes[i].curves[k]);
      }
    }
  }

  void render(bool renderCurves, bool renderPoints, bool renderBoundingRects, bool renderBorders, int numSegments, Scene *scene, SDLInterface *sdlInterface) {
    /*for(int i=0; i<shapes.size(); i++) {
      if(renderCurves) {
        shapes[i].renderCurvesSmooth(scene, sdlInterface);
      }
      if(renderPoints) {
        shapes[i].renderPoints(scene, sdlInterface);
      }
      if(renderBoundingRects) {
        shapes[i].renderBoundingRect(scene, sdlInterface);
      }
    }*/
    shape.numSegments = numSegments;
    if(renderCurves) {
      shape.renderCurvesSmooth(scene, sdlInterface);
    }
    if(renderPoints) {
      shape.renderPoints(scene, sdlInterface);
    }
    if(renderBoundingRects) {
      shape.renderBoundingRect(scene, sdlInterface);
    }
    if(renderBorders) {
      renderBorder(scene, sdlInterface);
    }
  }
  virtual void renderBorder(Scene *scene, SDLInterface *sdlInterface)  {}
};




struct TextShapeSet : public ShapeSet {
  double scale = 1.0;
  double canvasWidth = 60, canvasHeight = 35;
  double letterSpacing = 0.15;

  std::string str;
  CharacterShapeSet *charShapes;

  ~TextShapeSet() {
    if(charShapes) delete charShapes;
  }

  void renderBorder(Scene *scene, SDLInterface *sdlInterface) {
    double x0 = scene->toScreenX(0), y0 = scene->toScreenY(0);
    double x1 = scene->toScreenX(canvasWidth), y1 = scene->toScreenY(canvasHeight);
    setColor(0.5, 0.5, 0.5, 0.9, sdlInterface);
    drawLine(x0, y0, x1, y0, sdlInterface);
    drawLine(x1, y0, x1, y1, sdlInterface);
    drawLine(x1, y1, x0, y1, sdlInterface);
    drawLine(x0, y1, x0, y0, sdlInterface);
  }

  void update() {
    set(str, charShapes);
  }

  bool isBreak(wchar_t c) {
    return c == ' ' || c == L'\n' || c == L'\n';
  }

  void set(const std::string &str, CharacterShapeSet *charShapes) {
    this->str = str;
    this->charShapes = charShapes;
    clear();
    Vec2d pos(0, 0);
    double lineHeight = scale * 2.0;
    std::wstring wstr = to_wstring(str);

    for(int i=0; i<wstr.size(); i++) {
      wchar_t wc = wstr[i];

      if(i > 1 && i < wstr.size()-3 && isBreak(wstr[i-1]) && charShapes->isShapeDefined(wstr[i+1]) && !isBreak(wstr[i+1])) {
        double currentWordWidth = 0;
        std::wstring tmpsw = L"";
        for(int k = i+1; k < wstr.size()-1 && !isBreak(wstr[k]); k++)  {
          if(!charShapes->isShapeDefined(wstr[k])) continue;
          Shape shape;
          charShapes->getShape(wstr[k], shape);
          currentWordWidth += shape.boundingRect.getWidth() + lineHeight * letterSpacing;
          tmpsw += wstr[k];
          //printf("%c, %f, %f, %f, %f\n", (char)wstr[k], shape.boundingRect.b.x, shape.boundingRect.a.x, shape.boundingRect.b.x- shape.boundingRect.a.x, shape.boundingRect.getWidth());
        }
        messageQueue.addMessage(std::to_wstring(currentWordWidth) + L", " + std::to_wstring(pos.x) + L", " + std::to_wstring(canvasWidth) + L", " + tmpsw);
        if(currentWordWidth < canvasWidth && pos.x + currentWordWidth > canvasWidth) {
          pos.y += lineHeight;
          pos.x = 0;
        }

      }

      if(wc == ' ') {
        pos.x += lineHeight * 0.25;
      }
      else if(wc == L'\n') {
        pos.x = 0;
        pos.y += lineHeight;
      }
      else if(wc == L'\t') {
        double tx = lineHeight * 0.25 * scale * 4.0 * letterSpacing;
        double sx = tx;
        while(tx <= pos.x) {
          sx += tx;
        }
        pos.x = sx;
      }
      else if(charShapes->isShapeDefined(wc)) {
        Shape shape;
        charShapes->getShape(wc, shape);
        double cw = shape.boundingRect.getWidth();
        if(pos.x + cw >= canvasWidth) {
          pos.x = 0;
          pos.y += lineHeight;
        }
        pos.x -= shape.boundingRect.a.x;
        double bx = shape.boundingRect.b.x;
        shape.translateShape(pos + Vec2d(0, 1));
        shapes.push_back(shape);
        pos.x += bx + lineHeight * letterSpacing;
      }
      else {
        printf("Warning at TextShapeSet.set(): undefined character \'%d\'\n", (int)wc);
      }
    }
    combine();
  }

};

struct ShapeSketch : public Sketch
{
  FastNoise fastNoise;

  TextShapeSet textShapeSet;

  Shape shape;
  Texture canvasImg;
  ShapePainter shapePainter;
  Brushes brushes;
  CharacterShapeSet characterShapeSet;

  Texture img;

  GlShader testShader;
  Texture testTexture, testTexture2;
  Quadx testQuad;

  CommandTemplate saveBrushSettingsCmdTmpl, loadBrushSettingsCmdTmpl;

  CommandTemplate loadBrushCmdTmpl, removeBrushCmdTmpl, setAllBrushSettingsCmdTmpl;
  CommandTemplate setBrushOpacityCmdTmpl, setBrushColorCmdTmpl, setBrushSizeCmdTmpl;
  CommandTemplate setBrushDensityCmdTmpl, setBrushRandomOffsetCmdTmpl;
  CommandTemplate setBrushSmudgeRadiusCmdTmpl, setBrushSmudgeAmountCmdTmpl;
  CommandTemplate setBrushSmudgeNoiseSpeedCmdTmpl;
  CommandTemplate setBrushRotationModeCmdTmpl, setBrushRotationLimitsCmdTmpl;
  CommandTemplate setBrushRotationNoiseSpeedCmdTmpl;
  CommandTemplate setBrushScaleModeCmdTmpl, setBrushScaleLimitsCmdTmpl;
  CommandTemplate setBrushScaleNoiseSpeedCmdTmpl;

  CommandTemplate loadCanvasTextureCmdTmpl, removeCanvasTextureCmdTmpl, setCanvasTextureOnCmdTmpl;
  CommandTemplate setCanvasTextureSizeCmdTmpl, setCanvasTextureMonochromeCmdTmpl, resetCanvasTextureCmdTmpl;

  CommandTemplate setCanvasTextureVisibleCmdTmpl, setCanvasTextureOverlayColorCmdTmpl;

  CommandTemplate clearShapeCmdTmpl, clearCharacterShapeSetCmdTmpl;
  CommandTemplate setShapePainterDelayCmdTmpl, setShapePainterResolutionCmdTmpl;
  CommandTemplate restartShapePainterCmdTmpl, resetShapePainterCmdTmpl;
  CommandTemplate setCharacterShapeCmdTmpl, getCharacterShapeCmdTmpl;
  CommandTemplate printDefinedCharacterShapesCmdTmpl;
  CommandTemplate saveCharacterShapeSetCmdTmpl, loadCharacterShapeSetCmdTmpl;

  CommandTemplate renderBoundingRectsCmdTmpl;
  CommandTemplate showLinesCmdTmpl;

  CommandTemplate setTextShapeSetCmdTmpl, loadTextShapeSetCmdTmpl;
  CommandTemplate clearTextShapeSetCmdTmpl;
  CommandTemplate setTextCanvasWidthCmdTmpl, setLetterSpacingCmdTmpl;

  CommandTemplate printBrushParametersCmdTmpl;
  CommandTemplate setShapePainterClearCanvasOnStartCmdTmpl;

  CommandTemplate reloadBrushShaderCmdTmpl;
  CommandTemplate setCanvasFeedbackCmdTmpl;


  bool renderBoundingRects = false;
  bool showLines = false;


  void onInit() {

    scaleBind->active = true;
    positionBind->active = true;

    fastNoise.SetSeed(64573);
    fastNoise.SetNoiseType(FastNoise::SimplexFractal);

    glSetup(sdlInterface);

    brushes.init();

    canvasImg.createDualTexture(screenW, screenH, true);

    img.load("data/losing_my_religion2.png");

    canvasTextureOverlayColor.set(1, 1, 1, 1);

    canvasTextureShader.create("data/glsl/texture.vert", "data/glsl/textureColor.frag");
    canvasTextureQuad.create(screenW, screenH);

    testShader.create("data/glsl/texture.vert", "data/glsl/texture2.frag");
    testTexture.load("data/my_new_religion.png");
    testTexture2.load("data/textures/0014.png");
    testQuad.create(testTexture.w, testTexture.h);


    reloadBrushShaderCmdTmpl.finishInitialization("reloadBrushShader");
    commandQueue.addCommandTemplate(&reloadBrushShaderCmdTmpl);




    printBrushParametersCmdTmpl.finishInitialization("printBrushParameters");
    commandQueue.addCommandTemplate(&printBrushParametersCmdTmpl);

    saveBrushSettingsCmdTmpl.addArgument("filepath", STR_STRING);
    saveBrushSettingsCmdTmpl.addArgument("saveCanvases", STR_BOOL, "false");
    saveBrushSettingsCmdTmpl.finishInitialization("saveBrushSettings");
    commandQueue.addCommandTemplate(&saveBrushSettingsCmdTmpl);

    loadBrushSettingsCmdTmpl.addArgument("filepath", STR_STRING);
    loadBrushSettingsCmdTmpl.addArgument("loadCanvases", STR_BOOL, "false");
    loadBrushSettingsCmdTmpl.finishInitialization("loadBrushSettings");
    commandQueue.addCommandTemplate(&loadBrushSettingsCmdTmpl);

    loadBrushCmdTmpl.addArgument("filepath", STR_STRING);
    loadBrushCmdTmpl.finishInitialization("loadBrush");
    commandQueue.addCommandTemplate(&loadBrushCmdTmpl);
    removeBrushCmdTmpl.finishInitialization("removeBrush");
    commandQueue.addCommandTemplate(&removeBrushCmdTmpl);

    setBrushOpacityCmdTmpl.addArgument("opacity", STR_DOUBLE, "0.01");
    setBrushOpacityCmdTmpl.finishInitialization("setBrushOpacity");
    commandQueue.addCommandTemplate(&setBrushOpacityCmdTmpl);

    setBrushColorCmdTmpl.addArgument("r", STR_DOUBLE, "0.6");
    setBrushColorCmdTmpl.addArgument("g", STR_DOUBLE, "0.0");
    setBrushColorCmdTmpl.addArgument("b", STR_DOUBLE, "0.3");
    setBrushColorCmdTmpl.addArgument("a", STR_DOUBLE, "0.1");
    setBrushColorCmdTmpl.finishInitialization("setBrushColor");
    commandQueue.addCommandTemplate(&setBrushColorCmdTmpl);

    setBrushDensityCmdTmpl.addArgument("min", STR_DOUBLE, "0.0");
    setBrushDensityCmdTmpl.addArgument("max", STR_DOUBLE, "1.0");
    setBrushDensityCmdTmpl.finishInitialization("setBrushDensity");
    commandQueue.addCommandTemplate(&setBrushDensityCmdTmpl);

    setBrushRandomOffsetCmdTmpl.addArgument("min", STR_DOUBLE, "0.0");
    setBrushRandomOffsetCmdTmpl.addArgument("max", STR_DOUBLE, "0.0");
    setBrushRandomOffsetCmdTmpl.finishInitialization("setBrushRandomOffset");
    commandQueue.addCommandTemplate(&setBrushRandomOffsetCmdTmpl);

    setBrushSizeCmdTmpl.addArgument("size", STR_INT, "6");
    setBrushSizeCmdTmpl.finishInitialization("setBrushSize");
    commandQueue.addCommandTemplate(&setBrushSizeCmdTmpl);

    setBrushSmudgeRadiusCmdTmpl.addArgument("radius", STR_DOUBLE, "5");
    setBrushSmudgeRadiusCmdTmpl.addArgument("smudgeIndex", STR_INT, "0");
    setBrushSmudgeRadiusCmdTmpl.finishInitialization("setBrushSmudgeRadius");
    commandQueue.addCommandTemplate(&setBrushSmudgeRadiusCmdTmpl);
    setBrushSmudgeAmountCmdTmpl.addArgument("amount", STR_INT, "0");
    setBrushSmudgeAmountCmdTmpl.addArgument("smudgeIndex", STR_INT, "0");
    setBrushSmudgeAmountCmdTmpl.finishInitialization("setBrushSmudgeAmount");
    commandQueue.addCommandTemplate(&setBrushSmudgeAmountCmdTmpl);
    setBrushSmudgeNoiseSpeedCmdTmpl.addArgument("speed", STR_DOUBLE, "0.2");
    setBrushSmudgeNoiseSpeedCmdTmpl.addArgument("smudgeIndex", STR_INT, "0");
    setBrushSmudgeNoiseSpeedCmdTmpl.finishInitialization("setBrushSmudgeNoiseSpeed");
    commandQueue.addCommandTemplate(&setBrushSmudgeNoiseSpeedCmdTmpl);

    setBrushRotationModeCmdTmpl.addArgument("mode", STR_INT, "0");
    setBrushRotationModeCmdTmpl.finishInitialization("setBrushRotationMode");
    commandQueue.addCommandTemplate(&setBrushRotationModeCmdTmpl);
    setBrushRotationLimitsCmdTmpl.addArgument("min", STR_DOUBLE, "0");
    setBrushRotationLimitsCmdTmpl.addArgument("max", STR_DOUBLE, "0");
    setBrushRotationLimitsCmdTmpl.finishInitialization("setBrushRotationLimits");
    commandQueue.addCommandTemplate(&setBrushRotationLimitsCmdTmpl);
    setBrushRotationNoiseSpeedCmdTmpl.addArgument("speed", STR_DOUBLE, "0.2");
    setBrushRotationNoiseSpeedCmdTmpl.finishInitialization("setBrushRotationNoiseSpeed");
    commandQueue.addCommandTemplate(&setBrushRotationNoiseSpeedCmdTmpl);

    setBrushScaleModeCmdTmpl.addArgument("mode", STR_INT, "0");
    setBrushScaleModeCmdTmpl.finishInitialization("setBrushScaleMode");
    commandQueue.addCommandTemplate(&setBrushScaleModeCmdTmpl);
    setBrushScaleLimitsCmdTmpl.addArgument("min", STR_DOUBLE, "1");
    setBrushScaleLimitsCmdTmpl.addArgument("max", STR_DOUBLE, "1");
    setBrushScaleLimitsCmdTmpl.finishInitialization("setBrushScaleLimits");
    commandQueue.addCommandTemplate(&setBrushScaleLimitsCmdTmpl);
    setBrushScaleNoiseSpeedCmdTmpl.addArgument("speed", STR_DOUBLE, "0.001");
    setBrushScaleNoiseSpeedCmdTmpl.finishInitialization("setBrushScaleNoiseSpeed");
    commandQueue.addCommandTemplate(&setBrushScaleNoiseSpeedCmdTmpl);

    setAllBrushSettingsCmdTmpl.addArgument("size", STR_INT, "6");
    setAllBrushSettingsCmdTmpl.addArgument("r", STR_DOUBLE, "0.6");
    setAllBrushSettingsCmdTmpl.addArgument("g", STR_DOUBLE, "0.0");
    setAllBrushSettingsCmdTmpl.addArgument("b", STR_DOUBLE, "0.3");
    setAllBrushSettingsCmdTmpl.addArgument("a", STR_DOUBLE, "0.1");
    setAllBrushSettingsCmdTmpl.addArgument("denMin", STR_DOUBLE, "0.0");
    setAllBrushSettingsCmdTmpl.addArgument("denMax", STR_DOUBLE, "1.0");
    setAllBrushSettingsCmdTmpl.addArgument("s0amount", STR_INT, "0");
    setAllBrushSettingsCmdTmpl.addArgument("s0radius", STR_DOUBLE, "5");
    setAllBrushSettingsCmdTmpl.addArgument("s0speed", STR_DOUBLE, "0.2");
    setAllBrushSettingsCmdTmpl.addArgument("s1amount", STR_INT, "0");
    setAllBrushSettingsCmdTmpl.addArgument("s1radius", STR_DOUBLE, "5");
    setAllBrushSettingsCmdTmpl.addArgument("s1speed", STR_DOUBLE, "0.2");
    setAllBrushSettingsCmdTmpl.addArgument("roMin", STR_DOUBLE, "0.0");
    setAllBrushSettingsCmdTmpl.addArgument("roMax", STR_DOUBLE, "0.0");
    setAllBrushSettingsCmdTmpl.addArgument("rotMode", STR_INT, "0");
    setAllBrushSettingsCmdTmpl.addArgument("rotMin", STR_DOUBLE, "0");
    setAllBrushSettingsCmdTmpl.addArgument("rotMax", STR_DOUBLE, "0");
    setAllBrushSettingsCmdTmpl.addArgument("rotSpeed", STR_DOUBLE, "0.2");
    setAllBrushSettingsCmdTmpl.addArgument("scMode", STR_INT, "0");
    setAllBrushSettingsCmdTmpl.addArgument("scMin", STR_DOUBLE, "1");
    setAllBrushSettingsCmdTmpl.addArgument("scMax", STR_DOUBLE, "1");
    setAllBrushSettingsCmdTmpl.addArgument("scMpeed", STR_DOUBLE, "0.001");
    setAllBrushSettingsCmdTmpl.finishInitialization("setBrushParameters");
    commandQueue.addCommandTemplate(&setAllBrushSettingsCmdTmpl);

    setCanvasFeedbackCmdTmpl.addArgument("value", STR_BOOL);
    setCanvasFeedbackCmdTmpl.finishInitialization("setCanvasFeedback");
    commandQueue.addCommandTemplate(&setCanvasFeedbackCmdTmpl);


    loadCanvasTextureCmdTmpl.addArgument("filepath", STR_STRING);
    loadCanvasTextureCmdTmpl.finishInitialization("loadCanvasTexture");
    commandQueue.addCommandTemplate(&loadCanvasTextureCmdTmpl);

    removeCanvasTextureCmdTmpl.finishInitialization("removeCanvasTexture");
    commandQueue.addCommandTemplate(&removeCanvasTextureCmdTmpl);

    setCanvasTextureOnCmdTmpl.addArgument("value", STR_BOOL);
    setCanvasTextureOnCmdTmpl.finishInitialization("setCanvasTextureOn");
    commandQueue.addCommandTemplate(&setCanvasTextureOnCmdTmpl);
    setCanvasTextureSizeCmdTmpl.addArgument("width", STR_INT);
    setCanvasTextureSizeCmdTmpl.addArgument("height", STR_INT);
    setCanvasTextureSizeCmdTmpl.finishInitialization("setCanvasTextureSize");
    commandQueue.addCommandTemplate(&setCanvasTextureSizeCmdTmpl);
    resetCanvasTextureCmdTmpl.finishInitialization("resetCanvasTexture");
    commandQueue.addCommandTemplate(&resetCanvasTextureCmdTmpl);
    setCanvasTextureMonochromeCmdTmpl.addArgument("value", STR_BOOL);
    setCanvasTextureMonochromeCmdTmpl.finishInitialization("setCanvasTextureMonochrome");
    commandQueue.addCommandTemplate(&setCanvasTextureMonochromeCmdTmpl);

    setCanvasTextureVisibleCmdTmpl.addArgument("value", STR_BOOL);
    setCanvasTextureVisibleCmdTmpl.finishInitialization("setCanvasTextureVisible");
    commandQueue.addCommandTemplate(&setCanvasTextureVisibleCmdTmpl);
    setCanvasTextureOverlayColorCmdTmpl.addArgument("r", STR_DOUBLE, "1");
    setCanvasTextureOverlayColorCmdTmpl.addArgument("g", STR_DOUBLE, "1");
    setCanvasTextureOverlayColorCmdTmpl.addArgument("b", STR_DOUBLE, "1");
    setCanvasTextureOverlayColorCmdTmpl.addArgument("a", STR_DOUBLE, "1");
    setCanvasTextureOverlayColorCmdTmpl.finishInitialization("setCanvasTextureOverlayColor");
    commandQueue.addCommandTemplate(&setCanvasTextureOverlayColorCmdTmpl);


    setShapePainterDelayCmdTmpl.addArgument("delay", STR_DOUBLE, "0.002");
    setShapePainterDelayCmdTmpl.finishInitialization("setShapePainterDelay");
    commandQueue.addCommandTemplate(&setShapePainterDelayCmdTmpl);

    setShapePainterResolutionCmdTmpl.addArgument("resolution", STR_INT, "100000");
    setShapePainterResolutionCmdTmpl.finishInitialization("setShapePainterResolution");
    commandQueue.addCommandTemplate(&setShapePainterResolutionCmdTmpl);

    restartShapePainterCmdTmpl.finishInitialization("restartShapePainter");
    commandQueue.addCommandTemplate(&restartShapePainterCmdTmpl);

    resetShapePainterCmdTmpl.finishInitialization("resetShapePainter");
    commandQueue.addCommandTemplate(&resetShapePainterCmdTmpl);

    setShapePainterClearCanvasOnStartCmdTmpl.addArgument("value", STR_BOOL, "true");
    setShapePainterClearCanvasOnStartCmdTmpl.finishInitialization("setShapePainterClearCanvasOnStart");
    commandQueue.addCommandTemplate(&setShapePainterClearCanvasOnStartCmdTmpl);




    loadCharacterShapeSetCmdTmpl.addArgument("filepath", STR_STRING);
    loadCharacterShapeSetCmdTmpl.finishInitialization("loadCharacterShapeSet");
    commandQueue.addCommandTemplate(&loadCharacterShapeSetCmdTmpl);

    saveCharacterShapeSetCmdTmpl.addArgument("filepath", STR_STRING);
    saveCharacterShapeSetCmdTmpl.finishInitialization("saveCharacterShapeSet");
    commandQueue.addCommandTemplate(&saveCharacterShapeSetCmdTmpl);

    getCharacterShapeCmdTmpl.addArgument("character", STR_STRING);
    getCharacterShapeCmdTmpl.finishInitialization("getCharacterShape");
    commandQueue.addCommandTemplate(&getCharacterShapeCmdTmpl);

    setCharacterShapeCmdTmpl.addArgument("character", STR_STRING);
    setCharacterShapeCmdTmpl.finishInitialization("setCharacterShape");
    commandQueue.addCommandTemplate(&setCharacterShapeCmdTmpl);

    printDefinedCharacterShapesCmdTmpl.finishInitialization("printDefinedCharacterShapes");
    commandQueue.addCommandTemplate(&printDefinedCharacterShapesCmdTmpl);

    clearShapeCmdTmpl.finishInitialization("clearShape");
    commandQueue.addCommandTemplate(&clearShapeCmdTmpl);

    clearCharacterShapeSetCmdTmpl.finishInitialization("clearCharacterShapeSet");
    commandQueue.addCommandTemplate(&clearCharacterShapeSetCmdTmpl);


    renderBoundingRectsCmdTmpl.addArgument("value", STR_BOOL);
    renderBoundingRectsCmdTmpl.finishInitialization("renderBoundingRects");
    commandQueue.addCommandTemplate(&renderBoundingRectsCmdTmpl);

    showLinesCmdTmpl.addArgument("value", STR_BOOL);
    showLinesCmdTmpl.finishInitialization("showLines");
    commandQueue.addCommandTemplate(&showLinesCmdTmpl);


    setTextShapeSetCmdTmpl.addArgument("text", STR_STRING);
    setTextShapeSetCmdTmpl.finishInitialization("setTextShapeSet");
    commandQueue.addCommandTemplate(&setTextShapeSetCmdTmpl);

    loadTextShapeSetCmdTmpl.addArgument("filename", STR_STRING);
    loadTextShapeSetCmdTmpl.finishInitialization("loadTextShapeSet");
    commandQueue.addCommandTemplate(&loadTextShapeSetCmdTmpl);

    clearTextShapeSetCmdTmpl.finishInitialization("clearTextShapeSet");
    commandQueue.addCommandTemplate(&clearTextShapeSetCmdTmpl);

    setTextCanvasWidthCmdTmpl.addArgument("width", STR_DOUBLE, "60");
    setTextCanvasWidthCmdTmpl.finishInitialization("setTextCanvasWidth");
    commandQueue.addCommandTemplate(&setTextCanvasWidthCmdTmpl);

    setLetterSpacingCmdTmpl.addArgument("amount", STR_DOUBLE, "0.15");
    setLetterSpacingCmdTmpl.finishInitialization("setLetterSpacing");
    commandQueue.addCommandTemplate(&setLetterSpacingCmdTmpl);


    clear(0, 0, 0, 1);
    updateScreen(sdlInterface);


    /*std::wstring wstr = L"Testing unicode characters std::wstring\n" \
                        L"ä " + toNumbers(L"ä") + L"\n" \
                        L"Ä " + toNumbers(L"Ä");
    messageQueue.addMessage(wstr);


    std::string str = "Testing unicode characters std::string\n" \
                      "ä " + toNumbers("ä") + "\n" \
                      "Ä " + toNumbers("Ä");
    messageQueue.addMessage(str);*/

  }



  void onUpdate() {
    for(int i=0; i<commandQueue.commands.size(); i++) {
      Command *cmd = NULL;

      if(cmd = commandQueue.popCommand(&reloadBrushShaderCmdTmpl)) {
        brushes.reloadShader();
        delete cmd;
      }

      if(cmd = commandQueue.popCommand(&printBrushParametersCmdTmpl)) {
        wchar_t buf[1024];
        std::swprintf(buf, 1024, L"Brush:\n" \
                          "name \t\t\t\t%s\n" \
                          "size \t\t\t\t%d\n" \
                          "color \t\t\t\t%.2f, %.2f, %.2f, %.2f\n" \
                          "density \t\t\t\t%.4f..%.4f\n" \
                          "smudge1 amount \t%d\n" \
                          "smudge1 radius \t%.4f\n" \
                          "smudge1 speed \t%.4f\n" \
                          "smudge2 amount \t%d\n" \
                          "smudge2 radius \t%.4f\n" \
                          "smudge2 speed \t%.4f\n" \
                          "random offset \t\t%.4f..%.4f\n" \
                          "rotation mode \t\t%d\n" \
                          "rotation limits \t\t%.4f..%.4f\n" \
                          "rotation speed \t\t%.4f\n" \
                          "scale mode \t\t\t%d\n" \
                          "scale limits \t\t%.4f..%.4f\n" \
                          "scale speed \t\t%.4f",
                          brushes.brushes[brushes.activeBrushInd]->name.c_str(), brushes.size, brushes.brushColor.x, brushes.brushColor.y, brushes.brushColor.z, brushes.brushColor.w, brushes.densityMin, brushes.densityMax, brushes.smudgeAmount[0], brushes.smudgeRadius[0], brushes.smudgeNoiseSpeed[0], brushes.smudgeAmount[1], brushes.smudgeRadius[1], brushes.smudgeNoiseSpeed[1], brushes.randomOffsetMin, brushes.randomOffsetMax, brushes.rotationMode, brushes.rotationMin, brushes.rotationMax, brushes.rotationNoiseSpeed, brushes.scaleMode, brushes.scaleMin, brushes.scaleMax, brushes.scaleNoiseSpeed);
        messageQueue.addMessage(buf);
        delete cmd;
      }

      if(cmd = commandQueue.popCommand(&setTextCanvasWidthCmdTmpl)) {
        double value = 0;
        setTextCanvasWidthCmdTmpl.fillValues(cmd, &value);
        textShapeSet.canvasWidth = value;
        textShapeSet.update();
        delete cmd;
      }

      if(cmd = commandQueue.popCommand(&setLetterSpacingCmdTmpl)) {
        double value = 0;
        setLetterSpacingCmdTmpl.fillValues(cmd, &value);
        textShapeSet.letterSpacing = value;
        textShapeSet.update();
        delete cmd;
      }

      if(cmd = commandQueue.popCommand(&setTextShapeSetCmdTmpl)) {
        std::string value = "";
        setTextShapeSetCmdTmpl.fillValues(cmd, &value);
        if(value.size() > 0) {
          textShapeSet.set(value, &characterShapeSet);
        }
        delete cmd;
      }
      if(cmd = commandQueue.popCommand(&loadTextShapeSetCmdTmpl)) {
        std::string value = "";
        loadTextShapeSetCmdTmpl.fillValues(cmd, &value);
        //value = "/home/lassi/prog/c/playground/build/" + value;
        TextFile file(value);
        if(file.status == File::CONTENT_READ) {
          textShapeSet.set(file.content, &characterShapeSet);
        }
        delete cmd;
      }
      if(cmd = commandQueue.popCommand(&clearTextShapeSetCmdTmpl)) {
        textShapeSet.clear();
        delete cmd;
      }


      if(cmd = commandQueue.popCommand(&renderBoundingRectsCmdTmpl)) {
        renderBoundingRectsCmdTmpl.fillValues(cmd, &renderBoundingRects);
        delete cmd;
      }
      if(cmd = commandQueue.popCommand(&showLinesCmdTmpl)) {
        showLinesCmdTmpl.fillValues(cmd, &showLines);
        delete cmd;
      }

      if(cmd = commandQueue.popCommand(&clearShapeCmdTmpl)) {
        shape.clear();
        delete cmd;
      }
      if(cmd = commandQueue.popCommand(&clearCharacterShapeSetCmdTmpl)) {
        characterShapeSet.clear();
        delete cmd;
      }

      if(cmd = commandQueue.popCommand(&printDefinedCharacterShapesCmdTmpl)) {
        messageQueue.addMessage(characterShapeSet.getDefinedCharacters());
        delete cmd;
      }
      if(cmd = commandQueue.popCommand(&setCharacterShapeCmdTmpl)) {
        std::string value = "";
        setCharacterShapeCmdTmpl.fillValues(cmd, &value);
        if(value.size() > 0) {
          characterShapeSet.setShape(to_wstring(value)[0], shape);
        }
        delete cmd;
      }
      if(cmd = commandQueue.popCommand(&getCharacterShapeCmdTmpl)) {
        std::string value = "";
        getCharacterShapeCmdTmpl.fillValues(cmd, &value);
        if(value.size() > 0) {
          characterShapeSet.getShape(to_wstring(value)[0], shape);
        }
        delete cmd;
      }

      if(cmd = commandQueue.popCommand(&loadCharacterShapeSetCmdTmpl)) {
        std::string value = "";
        loadCharacterShapeSetCmdTmpl.fillValues(cmd, &value);
        value = "data/sketches/shapePainter/" + value;
        loadCerealBinary(characterShapeSet, value);
        delete cmd;
      }
      if(cmd = commandQueue.popCommand(&saveCharacterShapeSetCmdTmpl)) {
        std::string value = "";
        saveCharacterShapeSetCmdTmpl.fillValues(cmd, &value);
        value = "data/sketches/shapePainter/" + value;
        saveCerealBinary(characterShapeSet, value);
        delete cmd;
      }

      if(cmd = commandQueue.popCommand(&loadBrushCmdTmpl)) {
        std::string value = "";
        loadBrushCmdTmpl.fillValues(cmd, &value);
        value = "data/brushes/" + value;
        brushes.loadBrushTexture(value);
        delete cmd;
      }
      if(cmd = commandQueue.popCommand(&removeBrushCmdTmpl)) {
        brushes.removeBrushTexture();
        delete cmd;
      }

      if(cmd = commandQueue.popCommand(&loadBrushSettingsCmdTmpl)) {
        std::string value = "";
        bool loadCanvases = false;
        loadBrushSettingsCmdTmpl.fillValues(cmd, &value, &loadCanvases);
        brushes.loadBrushSettings(value, loadCanvases);
        delete cmd;
      }
      if(cmd = commandQueue.popCommand(&saveBrushSettingsCmdTmpl)) {
        std::string value = "";
        bool saveCanvases = false;
        saveBrushSettingsCmdTmpl.fillValues(cmd, &value, &saveCanvases);
        brushes.saveBrushSettings(value, saveCanvases);
        delete cmd;
      }


      if(cmd = commandQueue.popCommand(&setBrushOpacityCmdTmpl)) {
        double value = 0;
        setBrushOpacityCmdTmpl.fillValues(cmd, &value);
        brushes.setOpacity(value);
        delete cmd;
      }
      if(cmd = commandQueue.popCommand(&setBrushColorCmdTmpl)) {
        Vec4d value;
        setBrushColorCmdTmpl.fillValues(cmd, &value.x, &value.y, &value.z, &value.w);
        brushes.setColor(value);
        delete cmd;
      }
      if(cmd = commandQueue.popCommand(&setBrushDensityCmdTmpl)) {
        double min = 0, max = 0;
        setBrushDensityCmdTmpl.fillValues(cmd, &min, &max);
        brushes.densityMin = min;
        brushes.densityMax = max;
        delete cmd;
      }
      if(cmd = commandQueue.popCommand(&setBrushRandomOffsetCmdTmpl)) {
        double min = 0, max = 0;
        setBrushRandomOffsetCmdTmpl.fillValues(cmd, &min, &max);
        brushes.randomOffsetMin = min;
        brushes.randomOffsetMax = max;
        delete cmd;
      }
      if(cmd = commandQueue.popCommand(&setBrushSizeCmdTmpl)) {
        int value = 0;
        setBrushSizeCmdTmpl.fillValues(cmd, &value);
        brushes.size = value;
        delete cmd;
      }

      if(cmd = commandQueue.popCommand(&setBrushSmudgeRadiusCmdTmpl)) {
        double value = 0;
        int index = 0;
        setBrushSmudgeRadiusCmdTmpl.fillValues(cmd, &value, &index);
        index = clamp(index, 0, brushes.numSmudges);
        brushes.smudgeRadius[index] = value;
        delete cmd;
      }
      if(cmd = commandQueue.popCommand(&setBrushSmudgeAmountCmdTmpl)) {
        int value = 0;
        int index = 0;
        setBrushSmudgeAmountCmdTmpl.fillValues(cmd, &value, &index);
        index = clamp(index, 0, brushes.numSmudges);
        brushes.smudgeAmount[index]  = value;
        delete cmd;
      }
      if(cmd = commandQueue.popCommand(&setBrushSmudgeNoiseSpeedCmdTmpl)) {
        double value;
        int index = 0;
        setBrushSmudgeNoiseSpeedCmdTmpl.fillValues(cmd, &value, &index);
        index = clamp(index, 0, brushes.numSmudges);
        brushes.smudgeNoiseSpeed[index]  = value;
        delete cmd;
      }

      if(cmd = commandQueue.popCommand(&setBrushRotationModeCmdTmpl)) {
        int value = 0;
        setBrushRotationModeCmdTmpl.fillValues(cmd, &value);
        brushes.rotationMode = value;
        delete cmd;
      }
      if(cmd = commandQueue.popCommand(&setBrushRotationLimitsCmdTmpl)) {
        double min = 0, max = 0;
        setBrushRotationLimitsCmdTmpl.fillValues(cmd, &min, &max);
        brushes.rotationMin = min;
        brushes.rotationMax = max;
        delete cmd;
      }
      if(cmd = commandQueue.popCommand(&setBrushRotationNoiseSpeedCmdTmpl)) {
        double value;
        setBrushRotationNoiseSpeedCmdTmpl.fillValues(cmd, &value);
        brushes.rotationNoiseSpeed = value;
        delete cmd;
      }
      if(cmd = commandQueue.popCommand(&setBrushScaleModeCmdTmpl)) {
        int value = 0;
        setBrushScaleModeCmdTmpl.fillValues(cmd, &value);
        brushes.scaleMode = value;
        delete cmd;
      }
      if(cmd = commandQueue.popCommand(&setBrushScaleLimitsCmdTmpl)) {
        double min = 0, max = 0;
        setBrushScaleLimitsCmdTmpl.fillValues(cmd, &min, &max);
        brushes.scaleMin = min;
        brushes.scaleMax = max;
        delete cmd;
      }
      if(cmd = commandQueue.popCommand(&setBrushScaleNoiseSpeedCmdTmpl)) {
        double value;
        setBrushScaleNoiseSpeedCmdTmpl.fillValues(cmd, &value);
        brushes.scaleNoiseSpeed = value;
        delete cmd;
      }
      if(cmd = commandQueue.popCommand(&setAllBrushSettingsCmdTmpl)) {
        double r, g, b, a, denMin, denMax, s0r, s0a, s0s, s1r, s1a, s1s, roMin, roMax;
        double rotMin, rotMax, rotSpeed, scaleMin, scaleMax, scaleSpeed;
        int size, smudge0amount, smudge1amount, rotMode, scaleMode;

        setAllBrushSettingsCmdTmpl.fillValues(cmd, &size, &r, &g, &b, &a, &denMin, &denMax, &smudge0amount, &s0r, &s0s, &smudge1amount, &s1r, &s1s, &roMin, &roMax, &rotMode, &rotMin, &rotMax, &rotSpeed, &scaleMode, &scaleMin, &scaleMax, &scaleSpeed);
        brushes.size = size;
        brushes.brushColor.set(r, g, b, a);
        brushes.densityMin = denMin;
        brushes.densityMax = denMax;
        brushes.smudgeAmount[0] = smudge0amount;
        brushes.smudgeRadius[0] = s0r;
        brushes.smudgeNoiseSpeed[0] = s0s;
        brushes.smudgeAmount[1] = smudge1amount;
        brushes.smudgeRadius[1] = s1r;
        brushes.smudgeNoiseSpeed[1] = s1s;
        brushes.randomOffsetMin = roMin;
        brushes.randomOffsetMax = roMax;
        brushes.rotationMode = rotMode;
        brushes.rotationMin = rotMin;
        brushes.rotationMax = rotMax;
        brushes.rotationNoiseSpeed = rotSpeed;
        brushes.scaleMode = scaleMode;
        brushes.scaleMin = scaleMin;
        brushes.scaleMax = scaleMax;
        brushes.scaleNoiseSpeed = scaleSpeed;
        delete cmd;
      }


      if(cmd = commandQueue.popCommand(&setCanvasFeedbackCmdTmpl)) {
        bool value = false;
        setCanvasFeedbackCmdTmpl.fillValues(cmd, &value);
        brushes.useFeedbackCanvasTexture = value;
        canvasImg.useSecondaryTexture = value;
        delete cmd;
      }

      if(cmd = commandQueue.popCommand(&loadCanvasTextureCmdTmpl)) {
        std::string value = "";
        loadCanvasTextureCmdTmpl.fillValues(cmd, &value);
        value = "data/textures/" + value;
        brushes.loadCanvasTexture(value);
        delete cmd;
      }
      if(cmd = commandQueue.popCommand(&removeCanvasTextureCmdTmpl)) {
        brushes.removeCanvasTexture();
        delete cmd;
      }

      if(cmd = commandQueue.popCommand(&setCanvasTextureOnCmdTmpl)) {
        bool value = false;
        setCanvasTextureOnCmdTmpl.fillValues(cmd, &value);
        brushes.useCanvasTexture = value;
        delete cmd;
      }
      if(cmd = commandQueue.popCommand(&setCanvasTextureMonochromeCmdTmpl)) {
        bool value = false;
        setCanvasTextureMonochromeCmdTmpl.fillValues(cmd, &value);
        brushes.desaturateCanvasTexture(value);
        delete cmd;
      }
      if(cmd = commandQueue.popCommand(&setCanvasTextureSizeCmdTmpl)) {
        int w = 0, h = 0;
        setCanvasTextureSizeCmdTmpl.fillValues(cmd, &w, &h);
        brushes.resizeCanvasTexture(w, h);
        delete cmd;
      }
      if(cmd = commandQueue.popCommand(&resetCanvasTextureCmdTmpl)) {
        brushes.resetCanvasTexture();
        delete cmd;
      }

      if(cmd = commandQueue.popCommand(&setCanvasTextureVisibleCmdTmpl)) {
        bool value = false;
        setCanvasTextureVisibleCmdTmpl.fillValues(cmd, &value);
        showCanvasTexture = value;
        delete cmd;
      }
      if(cmd = commandQueue.popCommand(&setCanvasTextureOverlayColorCmdTmpl)) {
        Vec4d value;
        setCanvasTextureOverlayColorCmdTmpl.fillValues(cmd, &value.x, &value.y, &value.z, &value.w);
        canvasTextureOverlayColor = value;
        delete cmd;
      }

      if(cmd = commandQueue.popCommand(&setShapePainterDelayCmdTmpl)) {
        double value = 0;
        setShapePainterDelayCmdTmpl.fillValues(cmd, &value);
        shapePainter.delay = value;
        delete cmd;
      }
      if(cmd = commandQueue.popCommand(&setShapePainterResolutionCmdTmpl)) {
        int value = 0;
        setShapePainterResolutionCmdTmpl.fillValues(cmd, &value);
        shapePainter.numSegments = value;
        delete cmd;
      }

      if(cmd = commandQueue.popCommand(&setShapePainterClearCanvasOnStartCmdTmpl)) {
        bool value = false;
        setShapePainterClearCanvasOnStartCmdTmpl.fillValues(cmd, &value);
        shapePainter.clearCanvasOnStart = value;
        delete cmd;
      }



      if(cmd = commandQueue.popCommand(&restartShapePainterCmdTmpl)) {
        shapePainter.reset();
        shapePainter.begin();
        delete cmd;
      }
      if(cmd = commandQueue.popCommand(&resetShapePainterCmdTmpl)) {
        shapePainter.reset();
        delete cmd;
      }
    }
  }


  void onQuit() {
  }

  void onKeyUp() {
  }

  void onKeyDown() {
    shape.onKeyDown(events, scene);

    if(events.sdlKeyCode == SDLK_SPACE) {
      if(shapePainter.finished) {
        shapePainter.begin();
      }
      else {
        shapePainter.togglePause();
      }
    }
    if(events.sdlKeyCode == SDLK_BACKSPACE) {
      shapePainter.reset();
      canvasImg.setRenderTarget();
      clear(0, 0, 0, 1, sdlInterface);
      canvasImg.unsetRenderTarget();
    }
    if(events.sdlKeyCode == SDLK_RETURN) {
      shapePainter.reset();
      shapePainter.begin();
    }


    if(events.sdlKeyCode == SDLK_1 && events.lControlDown) {
      commandQueue.addCommand("loadCanvasTexture 0001.png");
      commandQueue.addCommand("setCanvasTextureMonochrome 1");
      commandQueue.addCommand("setCanvasTextureVisible 1");
      commandQueue.addCommand("setCanvasFeedback 1");
      commandQueue.addCommand("loadBrushSettings bloody");
    }
    if(events.sdlKeyCode == SDLK_2 && events.lControlDown) {
      commandQueue.addCommand("loadCharacterShapeSet test.csw");
      commandQueue.addCommand("loadBrushSettings pencil01");
      commandQueue.addCommand("loadCanvasTexture 0003.png");
      commandQueue.addCommand("setCanvasTextureMonochrome 1");
      commandQueue.addCommand("setCanvasTextureVisible 1");
      commandQueue.addCommand("setShapePainterDelay 0");
      commandQueue.addCommand("setCanvasFeedback 0");
      commandQueue.addCommand("loadTextShapeSet /home/lassi/Documents/diary/suomeksi/16.4.2018");

    }

    /*if(events.sdlKeyCode == SDLK_l && events.noModifiers) {
      characterShapeSet.load8bit = !characterShapeSet.load8bit;
      printf("characterShapeSet.load8bit = %d\n", (int)characterShapeSet.load8bit);
    }
    if(events.sdlKeyCode == SDLK_s && events.noModifiers) {
      characterShapeSet.save8bit = !characterShapeSet.save8bit;
      printf("characterShapeSet.save8bit = %d\n", (int)characterShapeSet.save8bit);
    }*/

  }

  void onMouseWheel() {
    shape.onMouseWheel(events, scene);

    if(events.mouseWheel > 0 && events.lControlDown) {
      brushes.selectNextBrush();
    }
    if(events.mouseWheel < 0 && events.lControlDown) {
      brushes.selectPreviousBrush();
    }
    if(events.mouseWheel > 0 && events.lShiftDown) {
      brushes.selectNextCanvas();
    }
    if(events.mouseWheel < 0 && events.lShiftDown) {
      brushes.selectPreviousCanvas();
    }
  }


  void onMousePressed() {
    shape.onMousePressed(events, scene);
  }

  void onMouseReleased() {
    shape.onMouseReleased(events, scene);
  }

  void onMouseMotion() {
    shape.onMouseMotion(events, scene);
  }




  void onDraw() {
    clear(0, 0, 0, 1);

    //brushes.paint(events.m, sdlInterface);

    //img.render(events.m, sdlInterface);
    /*setRenderTarget(canvasImg, sdlInterface);
    brushes.paint(events.m, sdlInterface);
    setDefaultRenderTarget(sdlInterface);*/


    if(showCanvasTexture) {
      CanvasTexture *canvasTexture = brushes.getActiveCanvasTexture();
      if(canvasTexture) {
        Texture *img = canvasTexture->img;
        canvasTextureShader.activate();
        canvasTextureShader.setUniform2f("screenSize", img->w, img->h);
        canvasTextureShader.setUniform4f("colorify", canvasTextureOverlayColor);
        canvasTextureShader.setUniform1i("usePixelCoordinates", 1);
        img->activate(canvasTextureShader, "tex", 0);
        canvasTextureQuad.render();
        canvasTextureShader.deActivate();
        img->inactivate(0);
      }
    }

    if(events.rControlDown) {
      testShader.activate();
      //testShader.setUniform2f("screenSize", testTexture2.w, testTexture2.h);
      testTexture.activate(testShader, "tex", 0);
      testTexture2.activate(testShader, "tex2", 1);
      testQuad.render((double)events.mouseX, (double)events.mouseY);//, scale, rotation);
      testShader.deActivate();
      testTexture.inactivate(0);
      testTexture2.inactivate(1);
    }

    if(textShapeSet.shapes.size() > 0) {
      if(shapePainter.paused) {
        textShapeSet.render(true, false, renderBoundingRects, true, 16, scene, sdlInterface);
      }
      else {
        shapePainter.paint(canvasImg, textShapeSet.shape, brushes, dt, scene, sdlInterface);
        canvasImg.render();
      }
    }
    else {
      if(shapePainter.paused) {
        shape.renderCurvesSmooth(scene, sdlInterface);
        shape.renderPoints(scene, sdlInterface);
        if(renderBoundingRects) {
          shape.renderBoundingRect(scene, sdlInterface);
        }
        if(showLines) {
          setColor(1, 1, 1, 0.2, sdlInterface);
          for(int i=0; i<5; i++) {
            double y = scene->toScreenY(-1.0 + 0.2 + 0.4*i);
            drawLine(0, y, screenW, y, sdlInterface);
          }
        }

      }
      else {
        shapePainter.paint(canvasImg, shape, brushes, dt, scene, sdlInterface);
        canvasImg.render();
      }
    }

    if(events.textInput.inputGrabbed) {
      console.render(sdlInterface);
    }

    wchar_t buf[64];
    std::swprintf(buf, 64, L"fps %.0f", round(timer.fps));
    textRenderer.print(buf, 5, 5, 10, 1.0, sdlInterface);

    /*text.print("Testing text rendering with GL textures", 10, 10, 10, 1.0, sdlInterface);
    text.print("Testing text rendering with GL textures", 10, 40, 12, 1.0, sdlInterface);

    canvasImg.setRenderTarget();
    clear(0, 0, 0, 0);
    //fontxx.print("Testing text rendering to texture with SDL textures", 10, 10, 10, 1.0, sdlInterface);
    //fontxx.print("Testing text rendering to texture with SDL textures", 10, 40, 12, 1.0, sdlInterface);
    text.print("Testing text rendering to texture with GL textures", 10, 100, 10, 1.0, sdlInterface);
    text.print("Testing text rendering to texture with GL textures", 10, 130, 12, 1.0, sdlInterface);
    canvasImg.unsetRenderTarget();
    canvasImg.renderFlipY();*/

    /*double y = 10;
    for(int i = 65; i >= 6; i-=1) {
      //double scale = (double)i/65;
      wchar_t buf[256];
      std::swprintf(buf, 256, L"%d, %.2f, Testing 123 µ€$¤", i, 1.0/timer.dt);
      Vec2d size = fontxx.getDimensions(buf, i, 1.0);
      fontxx.print(buf, 10, y + scene->worldPosition.y*5000, i, 1.0, sdlInterface);
      y += size.y;
    }*/

  }




};
