#pragma once

#include "../sketch.h"


struct Scene3D;

/* TODO
 * - inherit from canvas shader
 * - progress parameters
 *   - continuously
 *   - within a waveform such as sin (DONE)
 * - spectrum threshold level
 * - spectrum averaging only without progression
 *
 */

struct FlameVisualizer
{
  const std::string filePath = "data/flameVisualizer/";
  const std::string fileExtension = ".flv";

  void reset();



  GlShader flameShaderPass1, flameShaderPass2, flameShaderPass3, flameShaderPass4;
  GlShader flameShader3DPass1, flameShader3DPass2, flameShader3DPass3, flameShader3DPass4;
  Quadx quad;

  //Texture hitCounterImage;
  Texture hitCounterVoxels;
  Texture minMaxHitCounterImage;
  Texture minMaxHitCounterVoxels;
  Texture hitCounterImageArray;

  //std::vector<Texture*> minMaxHitCounterImages;
  static const int maxNumHitCounterImages = 64;
  int numHitCounterImages = 1;
  int activeHitCounterImageIndex = 0;

  std::vector<Vec2d> hitCounterImageMinMaxs = std::vector<Vec2d>(maxNumHitCounterImages);
  bool resetAllHitCounterImages = false;

  Texture bufferImage;

  int w, h;

  FractalParameters fractPars;

  static const int numFlameVariations = 10;
  static const int numFlameParameters2D = 14;
  static const int numFlameParameters3D = 20;
  static const int maxNumFlameParameters = 20;

  static const int totalNumFlameParameters = numFlameVariations*maxNumFlameParameters;

  std::vector<int> flameParameterSpectrumAssociation;
  std::vector<double> flameParameterSpectrumAssociationStrength;
  std::vector<double> flameParameterProgressAmount;
  std::vector<double> flameParameterProgression;

  // TODO additional texture image for recording min/max value
  std::vector<double> spectrum;
  CircularBuffer<std::vector<double>> spectrums;
  int spectrumSize = 7;
  int spectrumAveraging = 5;
  std::vector<double> spectrumScalings;

  bool isInputActive = false;

  int clearInterval = 0;
  unsigned long currentClearStep = 0;

  double spectrumScaling = 100.0;
  double spectrumMinimumThreshold = 0.0;

  double brightness = 1.0;

  bool showInfo = true;

  Scene3D *scene3D = NULL;
  Vec3i requestedVoxelGridSize;
  bool view3dActive = false;
  const int maxVoxelGridWidth = 512, maxVoxelGridHeight = 512, maxVoxelGridDepth = 512;

  bool useVoxelFlame = false;

  void setFlame3D(bool value);

  virtual ~FlameVisualizer();

  virtual void init(Events &events, int w, int h, GuiElement *parentGuiElement);
  virtual void loadShader();

  void onKeyUp(Events &events);
  void onKeyDown(Events &events);
  void onMouseWheel(Events &events);
  void onMouseMotion(Events &events);

  void update(const std::vector<double> &spectrum, double volume);
  void render(TextGl &textRenderer, double w, double h, double time, double dt, const Events &events, bool isDarkGuiColors);

  void setActive(bool isActive);


  struct FlameParameterPanel : public Panel {
    Label *variationLabelGui = NULL;
    NumberBox *variationGui = NULL;
    NumberBox *spectrumScalingGui = NULL;
    std::vector<NumberBox*> parametersGui;
    std::vector<NumberBox*> spectrumPositionsGui;
    std::vector<NumberBox*> spectrumStrengthsGui;
    std::vector<NumberBox*> progressAmountsGui;

    FlameVisualizer *flameVisualizer = NULL;
    int variationIndex = 0;

    virtual ~FlameParameterPanel();

    FlameParameterPanel(FlameVisualizer *flameVisualizer, int variationIndex);
    void init(FlameVisualizer *flameVisualizer, int variationIndex);


    void update();

    struct FlameParameterPanelListener : public GuiEventListener {
      FlameParameterPanel *flameParameterPanel;
      FlameParameterPanelListener(FlameParameterPanel *flameParameterPanel);
      void onValueChange(GuiElement *guiElement);
    };
  };

  Panel *panel = NULL;
  NumberBox *scaleGui = NULL;
  NumberBox *locationXGui = NULL, *locationYGui = NULL;
  NumberBox *minIterationGui = NULL;
  NumberBox *maxIterationGui = NULL;
  NumberBox *clearIntervalGui = NULL;
  NumberBox *spectrumScalingGui = NULL;
  NumberBox *spectrumMinimumThresholdGui = NULL;
  NumberBox *spectrumResolutionGui = NULL;
  NumberBox *spectrumAveragingGui = NULL;
  NumberBox *hitCounterTrailLevelGui = NULL;
  NumberBox *brightnessGui = NULL;
  CheckBox *showInfoGui = NULL;
  CheckBox *useVoxelFlameGui = NULL;
  NumberBox *voxelGridWidthGui = NULL, *voxelGridHeightGui = NULL, *voxelGridDepthGui = NULL;

  GraphPanel *spectrumPanel = NULL;
  std::vector<FlameParameterPanel*> flameParameterPanels;


  Panel *addPanel(GuiElement *parentElement, const std::string &title = "");

  void removePanel(GuiElement *parentElement);
  void updatePanel();

  Panel *getPanel();
  struct FlameVisualizerPanelListener : public GuiEventListener {
    FlameVisualizer *flameVisualizer;
    FlameVisualizerPanelListener(FlameVisualizer *flameVisualizer);
    void onValueChange(GuiElement *guiElement);
  };



  void saveToFile(std::string filename);

  void loadFromFile(const std::string &filename);



  template<class Archive>
  void serialize(Archive &ar, const std::uint32_t version);





};
