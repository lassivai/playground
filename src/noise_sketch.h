#pragma once
#include "engine/sketch.h"

/* Presets
 *
 * Value, freq 4-8
 * Value fractal, 17-70, oct 4, lac 3, FBM
 * Value fractal, octaves 2, rigid multi, freq 5-20, lac 3
 * Simplex fractal, FBM, hermite, octaves 3, freq 13-20, lac 2-3, gain 0.7-1
 * SimplexFractal, FBM, hermite, octaves 4, freq 1-6, lac 2-3, gain 0.7-1
 * Cellular, freq 5-30, distance (2 sub/2 div), euclidean
 *
 *
 *
 */

struct NoiseSketch : public Sketch
{
  GlScene glScene;
  Quad quad;
  GlShader shader;

  Image img;

  FastNoise fastNoise, fastNoiseCellularLookup;

  double scale = 1.0;
  Vec2d pos;
  double separation = 1;

  Events::InputVariableBind<double> *scaleBind, *separationBind;
  Events::InputVariableBind<Vec2d> *posBind;

  /*Vec2d dragStartPos;
  double startDragScale, startDragSeparation;

  bool moveDrag = false, scaleDrag = false, separationDrag = false;
  bool dragged = false;*/

  int noiseType = 0;
  int interp = 0;
  int fractalType = 0;
  int cellularDistanceFunction = 0;
  int cellularReturnType = 0;

  double *pixelCoordinateBufferX, *pixelCoordinateBufferY;

  void onInit() {

    scaleBind = new Events::InputVariableBind<double>(&scale, Events::dragModeExponential, 0.02, Events::buttonRight, 0);
    separationBind = new Events::InputVariableBind<double>(&separation, Events::dragModeExponential, 0.02, Events::buttonMiddle, 0);
    posBind = new Events::InputVariableBind<Vec2d>(&pos, &scale, Events::dragModeLinear, Vec2d(-2.0/screenH, -2.0/screenH), Events::buttonLeft, 0);

    events.inputBoundDoubles.push_back(scaleBind);
    events.inputBoundDoubles.push_back(separationBind);
    events.inputBoundVec2ds.push_back(posBind);

    img.create(screenW, screenH, sdlInterface);

    glSetup();

    //glScene.create();
    shader.create("data/glsl/texture.vert", "data/glsl/texture.frag");
    //quad.create(screenW, screenH);

    fastNoise.SetSeed(64573);
    fastNoise.SetFrequency(100.0);
    fastNoise.SetInterp(FastNoise::Linear);
    fastNoise.SetNoiseType(FastNoise::Value);
    fastNoise.SetFractalType(FastNoise::FBM);
    fastNoise.SetCellularNoiseLookup(&fastNoiseCellularLookup);


    pixelCoordinateBufferX = new double[screenW];
    pixelCoordinateBufferY = new double[screenH];

    for(int i=0; i<screenW; i++) {
      pixelCoordinateBufferX[i] = map(i, 0, screenW-1, -aspectRatio, aspectRatio);
    }
    for(int i=0; i<screenH; i++) {
      pixelCoordinateBufferY[i] = map(i, 0, screenH-1, -1.0, 1.0);
    }

  }

  void onQuit() {
    delete [] pixelCoordinateBufferX;
    delete [] pixelCoordinateBufferY;
  }

  void onKeyUp() {
  }

  void onKeyDown() {
    if(events.sdlKeyCode == SDLK_n) {
      noiseType = (noiseType+1) % 10;
      if(noiseType == 0) fastNoise.SetNoiseType(FastNoise::Value);
      if(noiseType == 1) fastNoise.SetNoiseType(FastNoise::ValueFractal);
      if(noiseType == 2) fastNoise.SetNoiseType(FastNoise::Perlin);
      if(noiseType == 3) fastNoise.SetNoiseType(FastNoise::PerlinFractal);
      if(noiseType == 4) fastNoise.SetNoiseType(FastNoise::Simplex);
      if(noiseType == 5) fastNoise.SetNoiseType(FastNoise::SimplexFractal);
      if(noiseType == 6) fastNoise.SetNoiseType(FastNoise::Cellular);
      if(noiseType == 7) fastNoise.SetNoiseType(FastNoise::WhiteNoise);
      if(noiseType == 8) fastNoise.SetNoiseType(FastNoise::Cubic);
      if(noiseType == 9) fastNoise.SetNoiseType(FastNoise::CubicFractal);
    }
    if(events.sdlKeyCode == SDLK_f) {
      fractalType = (fractalType+1) % 3;
      if(fractalType == 0) fastNoise.SetFractalType(FastNoise::FBM);
      if(fractalType == 1) fastNoise.SetFractalType(FastNoise::Billow);
      if(fractalType == 2) fastNoise.SetFractalType(FastNoise::RigidMulti);
    }
    if(events.sdlKeyCode == SDLK_i) {
      interp = (interp+1) % 3;
      if(interp == 0) fastNoise.SetInterp(FastNoise::Linear);
      if(interp == 1) fastNoise.SetInterp(FastNoise::Hermite);
      if(interp == 2) fastNoise.SetInterp(FastNoise::Quintic);
    }
    if(events.sdlKeyCode == SDLK_d) {
      cellularDistanceFunction = (cellularDistanceFunction+1) % 3;
      if(cellularDistanceFunction == 0) fastNoise.SetCellularDistanceFunction(FastNoise::Euclidean);
      if(cellularDistanceFunction == 1) fastNoise.SetCellularDistanceFunction(FastNoise::Manhattan);
      if(cellularDistanceFunction == 2) fastNoise.SetCellularDistanceFunction(FastNoise::Natural);
    }
    if(events.sdlKeyCode == SDLK_r) {
      cellularReturnType = (cellularReturnType+1) % 8;
      if(cellularReturnType == 0) fastNoise.SetCellularReturnType(FastNoise::CellValue);
      if(cellularReturnType == 1) fastNoise.SetCellularReturnType(FastNoise::NoiseLookup);
      if(cellularReturnType == 2) fastNoise.SetCellularReturnType(FastNoise::Distance);
      if(cellularReturnType == 3) fastNoise.SetCellularReturnType(FastNoise::Distance2);
      if(cellularReturnType == 4) fastNoise.SetCellularReturnType(FastNoise::Distance2Add);
      if(cellularReturnType == 5) fastNoise.SetCellularReturnType(FastNoise::Distance2Sub);
      if(cellularReturnType == 6) fastNoise.SetCellularReturnType(FastNoise::Distance2Mul);
      if(cellularReturnType == 7) fastNoise.SetCellularReturnType(FastNoise::Distance2Div);
    }
    if(events.sdlKeyCode >= SDLK_1 && events.sdlKeyCode <= SDLK_9) {
      int n = events.sdlKeyCode - SDLK_1 + 1;
      if(events.lControlDown) {
        fastNoise.SetFractalOctaves(n);
      }
      if(events.lShiftDown) {
        fastNoise.SetFractalLacunarity(3.0/n);
      }
      if(events.lAltDown) {
        fastNoise.SetFractalGain(1.0/n);
      }
      if(events.rControlDown) {
        fastNoise.SetFrequency(pow(10.0, n-5));
      }


    }


    if(events.sdlKeyCode == SDLK_q) {
      memcpy(&fastNoiseCellularLookup, &fastNoise, sizeof(fastNoise));
    }

  }


  void onMousePressed() {
  }

  void onMouseReleased() {
  }

  void onMouseMotion() {
  }

  void onUpdate() {

  }

  void onDraw() {
    //clear(0, 0, 0, 1);

    /*shader.activate();

    shader.setUniform2f("screenSize", screenW, screenH);
    shader.setUniform2f("mousePos", events.mouseX, events.mouseY);
    shader.setUniform1f("time", time);
    quad.render();
    shader.deActivate();*/

    unsigned int *pixels;
    img.accessPixels(&pixels);

    double t = SDL_GetTicks();

    for(int i=0; i<img.w; i++) {
      double x = pixelCoordinateBufferX[i];
      x = pos.x + x * scale;
      for(int j=0; j<img.h; j++) {
        double y = pixelCoordinateBufferY[j];
        y = pos.y + y * scale;
        double r = fastNoise.GetNoise(x, y)/2.0 + 0.5;
        double g = fastNoise.GetNoise(x+separation, y)/2.0 + 0.5;
        double b = fastNoise.GetNoise(x, y-separation)/2.0 + 0.5;
        pixels[i+j*img.w] = toRGBA(r, g, b, 1.0);
      }
    }
    t = (SDL_GetTicks() - t) / (img.w*img.h) * 1.0e6;

    img.applyPixels();
    img.render(sdlInterface);

    /*Vec2d topLeft(pixelCoordinateBufferX[0]*scale+pos.x,
                  pixelCoordinateBufferY[0]*scale+pos.y);
    Vec2d bottomRight(pixelCoordinateBufferX[screenW-1]*scale+pos.x,
                      pixelCoordinateBufferY[screenH-1]*scale+pos.y);

    double xSpan = topLeft.x - bottomRight.x;
    double ySpan = topLeft.y - bottomRight.y;*/

    double xSpan = 2.0 * aspectRatio * scale;
    double ySpan = 2.0 * scale;

    //shader.activate();

    /*shader.setUniform2f("screenSize", screenW, screenH);
    shader.setUniform2f("mousePos", events.mouseX, events.mouseY);
    shader.setUniform1f("time", time);*/

    std::wstring str = L"fps " + std::to_wstring(int(round(1.0/dt)));

    wchar_t buf[256];
    std::swprintf(buf, 256, L"\nnoise gen time %.0f ns", t);
    str += buf;

    std::swprintf(buf, 256, L"\nx value span %.4g ", xSpan);
    str += buf;

    str += L"\n";


    if(noiseType == 0) str += L"noise type: value\n";
    if(noiseType == 1) str += L"noise type: value fractal\n";
    if(noiseType == 2) str += L"noise type: perlin\n";
    if(noiseType == 3) str += L"noise type: perlin fractal\n";
    if(noiseType == 4) str += L"noise type: simplex\n";
    if(noiseType == 5) str += L"noise type: simplex fractal\n";
    if(noiseType == 6) str += L"noise type: cellular\n";
    if(noiseType == 7) str += L"noise type: white noise\n";
    if(noiseType == 8) str += L"noise type: cubic\n";
    if(noiseType == 9) str += L"noise type: cubic fractal\n";

    if(fractalType == 0) str += L"fractal type: FBM\n";
    if(fractalType == 1) str += L"fractal type: billow\n";
    if(fractalType == 2) str += L"fractal type: rigid multi\n";

    if(interp == 0) str += L"interpolation: linear\n";
    if(interp == 1) str += L"interpolation: hermite\n";
    if(interp == 2) str += L"interpolation: quintic\n";

    if(cellularDistanceFunction == 0) str += L"cellular distance function: euclidean\n";
    if(cellularDistanceFunction == 1) str += L"cellular distance function: manhattan\n";
    if(cellularDistanceFunction == 2) str += L"cellular distance function: natural\n";

    if(cellularReturnType == 0) str += L"cell return type: Cell Value\n";
    if(cellularReturnType == 1) str += L"cell return type: Noise lookup\n";
    if(cellularReturnType == 2) str += L"cell return type: Distance\n";
    if(cellularReturnType == 3) str += L"cell return type: Distance 2\n";
    if(cellularReturnType == 4) str += L"cell return type: Distance 2 Add\n";
    if(cellularReturnType == 5) str += L"cell return type: Distance 2 Sub\n";
    if(cellularReturnType == 6) str += L"cell return type: Distance 2 Mul\n";
    if(cellularReturnType == 7) str += L"cell return type: Distance 2 Div\n";

    str += L"frequency: " + std::to_wstring(fastNoise.GetFrequency()) + L"\n";
    str += L"fractal lacunarity: " + std::to_wstring(fastNoise.GetFractalLacunarity()) + L"\n";
    str += L"fractal gain: " + std::to_wstring(fastNoise.GetFractalGain()) + L"\n";
    str += L"fractal octaves: " + std::to_wstring(fastNoise.GetFractalOctaves()) + L"\n";


    //SDL_SetRenderDrawColor(sdlInterface->renderer, 255, 0, 0, 255);
    glColor4f(0.0, 0, 0.0, 1);
    textRenderer.print(str, 10, 10, 17, 1, sdlInterface);
    glColor4f(1, 1, 1, 1);
    //shader.deActivate();

  }

};
