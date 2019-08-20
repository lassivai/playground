#pragma once
#include "../matrix.h"
#include "../sketch.h"

static int cycleValue(int val, int min, int max);
static int cycleValue(int val, int max);
static double cycleValue(double val, double max);

struct Scene3D;


struct KeyFrame {
  double camCenterX = 0, camCenterY = 0, camCenterZ = 0;
  double camRotX = 0, camRotY = 0, camDistance = 10000;
  int redChannel = 0, greenChannel = 0, blueChannel = 0;
  double redFactor = 1.0, greenFactor = 1.0, blueFactor = 1.0, intensityFactor = 1.0;
  double deltaOffset = 0;
  std::vector<double> rayAttenuation = {0.5, 0.5, 0.0, 0.0, 1.0};
  double attenuationAlphaFactor = 1.0;
  double colorAlphaFactor = 1.0;
  double colorRayStrengthFactor = 1.0;

  int shadingMode = 0;
  int traverseMode = 0;

  bool dynamicLevels = true;
  bool lockDynamicLevel = false;

  double minX = 0, minY = 0, minZ = 0, maxX = 1, maxY = 1, maxZ = 1;
};

struct Animation
{
  std::vector<KeyFrame> keyFrames;
  KeyFrame f0, f1, f2, f3;

  int currentKeyFrame = 0;
  int currentSubFrame = 0;
  double fraction = 0;
  double curvature = 0.5;
  int numSubFrames = 10;

  Animation();
  bool isLastFrame(int loopMode);
  bool deltaFrame(int d, int loopMode);
  void setFrame(int keyFrame, int subFrame);
  void removeCurrentKeyFrame();
  void reset();
  double na(double a, double b);
  void applyToScene(Scene3D &scene);
  void applyFromScene(int keyFrameIndex, bool insert, Scene3D &scene);
  void applyNonCameraInfoToAllTheRest(Scene3D &scene);
  int getKeyFrameCount();
};

struct Ray
{
  Vec3d origin, direction;
  Vec3d originEye, directionEye;

  Ray();
  Ray(double dx, double dy, double dz);
  Ray(double ox, double oy, double oz, double dx, double dy, double dz);
  void update(MatrixStack mat);
};





struct Box
{
  Vec3d pos, posEye;
  double w = 1, h = 1, d = 1;

  std::vector<Vec3d> vertices, verticesEye;

  Box();
  Box(double s);
  Box(double w, double h, double d);
  void update(MatrixStack &mat);
};


/* TODO
 * - alpha blending is currently based on color values, option to utilize alpha channel would be appropriate
 */

struct VoxelGrid : public Box
{
  double minX = 0, minY = 0, minZ = 0, maxX = 1, maxY = 1, maxZ = 1;

  std::vector<Vec4d> voxels;

  Vec2d xRange, yRange, zRange;
  std::vector<Vec4d> voxelCoordinates;


  //ImgWrapper img;

  VoxelGrid();

  /*VoxelGrid(ImgWrapper img) {
    updateVoxelGrid(img);
  }*/

  void setVoxel(int x, int y, int z, const Vec4d &value);

  //void updateVoxelGrid(ImgWrapper img) {
  void setSize(int w, int h, int d);
  Vec4d traverseGridFast(Vec3d start, Vec3d end, Scene3D &scene);
  Vec4d traverseGrid(Vec3d start, Vec3d end, Scene3D &scene);

};


std::vector<double> intersectRayAAB(Ray &ray, VoxelGrid &grid);




struct Scene3D
{
  int w, h;
  double camCenterX = 0, camCenterY = 0, camCenterZ = 0;
  double camRotX = 0, camRotY = 0, camDistance = 10000;
  int redChannel = 0, greenChannel = 1, blueChannel = 2;
  double redFactor = 1.0, greenFactor = 1.0, blueFactor = 1.0, intensityFactor = 1.0;
  double deltaOffset = 0;
  std::vector<double> rayAttenuation = {0.5, 0.0, 1.0, 0.5};
  double attenuationAlphaFactor = 1.0;
  double colorAlphaFactor = 1.0;
  double colorRayStrengthFactor = 1.0;

  int shadingMode = 0;
  int traverseMode = 0;

  bool dynamicLevels = true;
  bool lockDynamicLevel = false;

  MatrixStack mat;
  VoxelGrid voxelGrid;

  Texture rayTracedSceneCurrent;
  std::vector<Texture> rayTracedScene;
  bool rayTracingDone = false;
  double renderingTime = 0;
  bool stayInPreview = false;
  int previewRenderIndex = 1, finalRenderIndex = 3;
  bool useRayTracing = true;
  std::vector<std::string> shadingModes = {"sum", "max int.", "first voxel"};

  double previousMax = 1.0, previousMaxTmp = 0.0;
  double previousMin = 0.0, previousMinTmp = 0.0;

  //ImgWrapper img;

  bool maxIntOrAlpha = true;

  int cameraMode = 0;

  Animation anim;
  bool useAnimation = false;
  bool animationRunning = false;
  int loopMode = 0;
  const std::vector<std::string> loopModes = {"no loop", "single loop", "loop"};
  int animationStep = 1;
  bool saveToFile = false;
  int saveFrameCounter = 0;
  std::string baseFilename = "";

  // to be included in key frames

  const std::vector<std::string> flipModes = {"", "rot. X 90°", "rot. X -90°", "rot. Z 90°", "rot. Z -90°"};
  int flipMode = 1;
  bool showBorders = false;
  double verticalFOV = 78.0;
  double averageFactor = 0.0;
  double mipFactor = 0.0;
  double xOffsetFactor = 0.52;

  bool keyLeft = false;
  bool keyRight = false;
  bool keyUp = false;
  bool keyDown = false;
  bool keyComma = false;
  bool keyDot = false;

  bool showInfo = true;

  int numRenderingThreads = 5;
  std::vector<std::thread> renderingThreads;

  Matrix4d inverseCameraMatrix;
  double aspectRatio;

  bool useGpu = false;
  Texture *voxelGridTexture = NULL;
  Texture *minMaxVoxelValuesTexture = NULL;
  GlShader voxelTracerShader;
  Quadx quad;

  Scene3D(int w, int h);

  int pixelization = 2;

  //void render(int threadNumber);

  void startSaving();
  void stopSaving();
  void saveScene(bool sequence);

  void setVoxelGridSize(int w, int h, int d);
  void setUseGpu(bool useGpu);
  void reloadShaders();

  //void update(ImgWrapper img) {
  void update();
  void resetCanvases();

  void renderWithGpu(double time);

  void setReady(bool ready);
  bool isReady();

  void newFrame(bool insert);
  void applyFrame();

  void updateCamera(double dx, double dy, double dz, int mode);

  void updateView();
  void update(double time);

  void draw(int x, int y, int w, int h);
  void printInfo(TextGl &textRenderer, int x, int y, int w, int h, int verbality, bool left);

  void onKeyUp(Events &e);
  void onKeyDown(Events &e);
  void onMouseMotion(Events &e);
};
