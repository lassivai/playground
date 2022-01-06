#pragma once
#include "../engine/sketch.h"
#include "clockwork_gui.h"
#include "clockwork_help.h"

/* DEBUG
 *
 * - sometimes crashes when removing a body. Observed with long chains. Possibly
 *   something to do with constraints not being handled appropriately. SOLVED probably
 *
 * - another rarer crash observed (had a lot of gears)
 */

/* TODO
 *
 * - target camera to follow specific body
 * - replicate "whole object" based on links made with constraints (bodies + constraints)
 * - not so simple motor (thing that can rotate faster than the constant rate etc.)
 * - slighly different positions for constraints placed at the exact same places in editing view
 * - different colors for different constraints in editing mode
 * - more pleasent constraint rendering, optionally, when not in editing mode (especially for springs)
 * - constraint to limit rotation to an arbitrary sector (modification of constant angle constraint maybe)
 * - sometimes things get messy and weird (incredients: pivot joints, constant angle constraints, motors,
 *   different collision categories; especially the last thing should be investigated more)
 * - loadable sprites for objects
 * - something wrong with rendering settings, some similar strokes seem smoother than others
 * - triangulation of polygon with holes doesn't work in all cases (some of the most simplest ones), which
 *   causes other unexpected bugs here and there
 * - way to set parameters based on object name
 * - choosable default collision categories and mask (is this done? check)
 * - preserve some collision bits for internal stuff only (it might be ok now even though user can do weird things)
 *    - one category for objects that don't collide with anything but visible to nearest object finding
 *    - the locked category
 * - prebuilt composite objects
 *    - springs
 *    - it would be nice to have prebuilt testing vehicles (bike, motorcycle, unicycle, tank, etc.)
 *    - testing character with all the input already set
 * - concidering the previous cases, a nice feature would be a conversion of objects in to commands that could be copy-
 *   pasted into the code after refining the thing with the graphical editor
 * - "steampunk" style line conversion (+ breakable stuff)
 * - save/load simulation state (quite easy to do, but a lot of problems arise if a lot of changes still happen what
 *   would be saved/loaded)
 *
 *
 * If you can't yet see where I am heading: I can basically create totally different kind of game scenarios with
 * this thing. I thought it would be nice challenge to recreate e.g. the original Super Mario with this thing, at
 * first... Not quite there yet, however.
 *
 * One of my big nerdy dreams has been to first create this kind of physics simulator and then do evolutionary
 * programming with it. Basically see if I can teach a stick figure to walk or jump or something. I have never
 * done anything like that but I think I'm now ready to start experimenting. With a much more basic setup than any
 * neural network or such, at first.
 *
 *
 * DEBUG
 *
 * - removed the line object, created two gears and collision stuff was messed up
 * - apparently objects not removed from space properly (the old crash when collision in editing mode...),
 *   probably solved
 *
 *
 */






struct ClockworkSketch : public Sketch
{
  FastNoise fastNoise;

  GlShader shaderPost;
  Texture texture3;

  Quadx quadScreen;

  //GeomRenderer geomRenderer;

  Shapex shape;

  Simulation simulation;

  //bool editingShape = false;

  CommandTemplate setGravityCmdTmpl, setDampingCmdTmpl, setNumIterationsCmdTmpl;
  CommandTemplate setTimeStepCmdTmpl;

  CommandTemplate addBodiesCmdTmpl;
  CommandTemplate setBoundingRectsVisibleCmdTmpl, setContactPointsVisibleCmdTmpl;
  CommandTemplate clearBodiesCmdTmpl;
  CommandTemplate addScreenWallsCmdTmpl;
  CommandTemplate setDefaultDensityCmdTmpl, setDefaultElasticityCmdTmpl, setDefaultFrictionCmdTmpl;
  CommandTemplate setDensityCmdTmpl, setElasticityCmdTmpl, setFrictionCmdTmpl;
  CommandTemplate setObjectNameCmdTmpl, setObjectNameDefaultCmdTmpl;

  CommandTemplate setConstraintCmdTmpl;
  CommandTemplate setSimpleMotorCmdTmpl, setConstantAngleConstraintCmdTmpl;
  CommandTemplate setConstantAngleConstraintAngleCmdTmpl;
  CommandTemplate setDampedSpringCmdTmpl;
  CommandTemplate setFaceEachOtherConstraintCmdTmpl;
  CommandTemplate setRotaryLimitConstraintMinCmdTmpl, setRotaryLimitConstraintMaxCmdTmpl;
  CommandTemplate setRotaryLimitConstraintCmdTmpl;

  CommandTemplate createObjectCmdTmpl;
  CommandTemplate replicateGridCmdTmpl, replicateRingCmdTmpl;

  CommandTemplate setNumExtraIterationsCmdTmpl;

  CommandTemplate setDragForceCmdTmpl;

  CommandTemplate setSimpleMotorControllerCmdTmpl;
  CommandTemplate setSimpleCharacterControllerCmdTmpl;
  CommandTemplate setInputInterfaceInfoVisibleCmdTmpl;

  CommandTemplate printInputCmdTmpl;

  Body *bodyMiddle = NULL;

  NearestObjectQuery nearestObjectQuery;
  BodyPart *lockedBodyPart = NULL;
  int nearestObjectDraggingMode = 0;
  Vec2d nearestObjectBindingPointLocal;
  double dragForce = 10;

  Composite *compositeSelection = NULL;

  //GenericSimpleMotorController simpleMotorController;



  static const int toolModeDefault = 0, toolModeEditObject = 1;
  int toolMode = 0;


  static const int editingModeNewCirclePos = 1, editingModeNewCircleScale = 2;
  static const int editingModeNewLineA = 3, editingModeNewLineB = 4;
  static const int editingModeNewRectPos = 5, editingModeNewRectSize = 6;
  static const int editingModeNewPolygon = 7;
  static const int editingModeMoveBody = 8, editingModeMoveBodyPart = 9;
  static const int editingModeNewPivotJointPos = 10;
  static const int editingModeMoveConstraint = 12, editingModeMoveBodyB = 13;
  static const int editingModeRotateBody = 14, editingModeRotateBodyPart = 15;
  static const int editingModeRotateBodyB = 16;
  static const int editingModeNewGrooveJointPosA = 17, editingModeNewGrooveJointPosB = 18;
  static const int editingModeNewDampedSpringPosA = 19, editingModeNewDampedSpringPosB = 20;
  static const int editingModeNewHoopPos = 21, editingModeNewHoopScale = 22, editingModeNewHoopInnerRadius = 23;


  //static const int editingModeGatherCollisionGroup = 17;

  //std::vector<Body*> newCollisionGroup;

  int editingMode = 0;
  Body *editingBody = NULL, *editingBodyB = NULL;
  Constraint *editingConstraint = NULL;
  GeomObject *tmpEditingGeomObject = NULL;
  Vec2d newConstraintPosA, newConstraintPosB;
  bool editingConstraintNearerThanBodyPart = false;

  bool lockEditingConstraint = false;

  Circle newCircle;
  Line newLine;
  Rect newRect;
  Polygon newPolygon;
  Hoop newHoop;

  double maxGravity = 1000;
  Polygon gravityArrow;

  Camera2D camera;

  bool showInputInterfaces = false;

  bool guiInputCapture = false;
  CollisionCategorySelectionGUI collisionCategorySelectionGUI;


  EditingBodyPanel *editingBodyPanel = NULL;
  ConstraintPanel *constraintPanel = NULL;


  Vec2d rd2(double min, double max) {
    return Vec2d(Random::getDouble(min, max), Random::getDouble(min, max));
  }
  double rd(double min, double max) {
    return Random::getDouble(min, max);
  }
  int ri(int min, int max) {
    return Random::getInt(min, max);
  }
  void onInit() {

    /*printf("%d, %d\n", SDLK_1, SDL_GetKeyFromName("1"));
    printf("%d, %d\n", SDLK_DELETE, SDL_GetKeyFromName("delete"));
    printf("%d, %d\n", SDLK_a, SDL_GetKeyFromName("a"));
    printf("%d, %d\n", SDLK_UP, SDL_GetKeyFromName("up"));
    printf("%d, %d\n", SDLK_BACKSPACE, SDL_GetKeyFromName("backspace"));
    printf("%d, %d\n", SDLK_RSHIFT, SDL_GetKeyFromName("right shift"));
    printf("%d, %d\n", SDLK_KP_PLUS, SDL_GetKeyFromName("keypad +"));
    printf("%d, %d\n", SDLK_KP_1, SDL_GetKeyFromName("keypad 1"));*/

    //scaleBind->active = true;
    //positionBind->active = true;

    camera.init(screenW, screenH, events, sdlInterface);

    printInputCmdTmpl.finishInitialization("printInput");
    commandQueue.addCommandTemplate(&printInputCmdTmpl);

    setObjectNameCmdTmpl.addArgument("name", STR_STRING);
    setObjectNameCmdTmpl.finishInitialization("setObjectName");
    commandQueue.addCommandTemplate(&setObjectNameCmdTmpl);

    setObjectNameDefaultCmdTmpl.addArgument("name", STR_STRING);
    setObjectNameDefaultCmdTmpl.finishInitialization("setObjectNameDefault");
    commandQueue.addCommandTemplate(&setObjectNameDefaultCmdTmpl);


    setDragForceCmdTmpl.addArgument("force", STR_DOUBLE);
    setDragForceCmdTmpl.finishInitialization("setDragForce");
    commandQueue.addCommandTemplate(&setDragForceCmdTmpl);

    setTimeStepCmdTmpl.addArgument("dt", STR_DOUBLE);
    setTimeStepCmdTmpl.finishInitialization("setTimeStep");
    commandQueue.addCommandTemplate(&setTimeStepCmdTmpl);

    setGravityCmdTmpl.addArgument("x", STR_DOUBLE);
    setGravityCmdTmpl.addArgument("y", STR_DOUBLE);
    setGravityCmdTmpl.finishInitialization("setGravity");
    commandQueue.addCommandTemplate(&setGravityCmdTmpl);

    setDampingCmdTmpl.addArgument("value", STR_DOUBLE);
    setDampingCmdTmpl.finishInitialization("setDamping");
    commandQueue.addCommandTemplate(&setDampingCmdTmpl);

    setNumIterationsCmdTmpl.addArgument("value", STR_INT);
    setNumIterationsCmdTmpl.finishInitialization("setNumIterations");
    commandQueue.addCommandTemplate(&setNumIterationsCmdTmpl);

    setNumExtraIterationsCmdTmpl.addArgument("value", STR_INT);
    setNumExtraIterationsCmdTmpl.finishInitialization("setNumExtraIterations");
    commandQueue.addCommandTemplate(&setNumExtraIterationsCmdTmpl);

    addBodiesCmdTmpl.addArgument("n", STR_INT);
    addBodiesCmdTmpl.addArgument("rMin", STR_DOUBLE);
    addBodiesCmdTmpl.addArgument("rMax", STR_DOUBLE);
    addBodiesCmdTmpl.addArgument("type1", STR_INT, "0");
    addBodiesCmdTmpl.addArgument("type2", STR_INT, "0");
    addBodiesCmdTmpl.addArgument("type3", STR_INT, "0");
    addBodiesCmdTmpl.addArgument("type4", STR_INT, "0");
    addBodiesCmdTmpl.addArgument("type5", STR_INT, "0");
    addBodiesCmdTmpl.finishInitialization("addBodies");
    commandQueue.addCommandTemplate(&addBodiesCmdTmpl);

    setBoundingRectsVisibleCmdTmpl.addArgument("value", STR_BOOL);
    setBoundingRectsVisibleCmdTmpl.finishInitialization("setBoundingRectsVisible");
    commandQueue.addCommandTemplate(&setBoundingRectsVisibleCmdTmpl);
    setContactPointsVisibleCmdTmpl.addArgument("value", STR_BOOL);
    setContactPointsVisibleCmdTmpl.finishInitialization("setContactPointsVisible");
    commandQueue.addCommandTemplate(&setContactPointsVisibleCmdTmpl);

    addScreenWallsCmdTmpl.addArgument("thickness", STR_DOUBLE, "3");
    addScreenWallsCmdTmpl.finishInitialization("addScreenWalls");
    commandQueue.addCommandTemplate(&addScreenWallsCmdTmpl);

    clearBodiesCmdTmpl.addArgument("bodyType1", STR_STRING, "random");
    clearBodiesCmdTmpl.addArgument("bodyType2", STR_STRING, "nothing");
    clearBodiesCmdTmpl.addArgument("bodyType3", STR_STRING, "nothing");
    clearBodiesCmdTmpl.addArgument("bodyType4", STR_STRING, "nothing");
    clearBodiesCmdTmpl.addArgument("bodyType6", STR_STRING, "nothing");
    clearBodiesCmdTmpl.addArgument("bodyType7", STR_STRING, "nothing");
    clearBodiesCmdTmpl.finishInitialization("clearBodies");
    commandQueue.addCommandTemplate(&clearBodiesCmdTmpl);

    setDefaultDensityCmdTmpl.addArgument("value", STR_DOUBLE, "0.001");
    setDefaultDensityCmdTmpl.finishInitialization("setDefaultDensity");
    commandQueue.addCommandTemplate(&setDefaultDensityCmdTmpl);
    setDefaultElasticityCmdTmpl.addArgument("value", STR_DOUBLE, "0.5");
    setDefaultElasticityCmdTmpl.finishInitialization("setDefaultElasticity");
    commandQueue.addCommandTemplate(&setDefaultElasticityCmdTmpl);
    setDefaultFrictionCmdTmpl.addArgument("value", STR_DOUBLE, "0.5");
    setDefaultFrictionCmdTmpl.finishInitialization("setDefaultFriction");
    commandQueue.addCommandTemplate(&setDefaultFrictionCmdTmpl);

    setDensityCmdTmpl.addArgument("value", STR_DOUBLE, "0.001");
    setDensityCmdTmpl.addArgument("bodyPartIndex", STR_INT, "-1");
    setDensityCmdTmpl.finishInitialization("setDensity");
    commandQueue.addCommandTemplate(&setDensityCmdTmpl);

    setElasticityCmdTmpl.addArgument("value", STR_DOUBLE, "0.5");
    setElasticityCmdTmpl.addArgument("bodyPartIndex", STR_INT, "-1");
    setElasticityCmdTmpl.finishInitialization("setElasticity");
    commandQueue.addCommandTemplate(&setElasticityCmdTmpl);

    setFrictionCmdTmpl.addArgument("value", STR_DOUBLE, "0.5");
    setFrictionCmdTmpl.addArgument("bodyPartIndex", STR_INT, "-1");
    setFrictionCmdTmpl.finishInitialization("setFriction");
    commandQueue.addCommandTemplate(&setFrictionCmdTmpl);

    setConstraintCmdTmpl.addArgument("collideBodies", STR_BOOL, "0");
    setConstraintCmdTmpl.addArgument("maxForce", STR_DOUBLE, "-1");
    setConstraintCmdTmpl.addArgument("maxBias", STR_DOUBLE, "-1");
    setConstraintCmdTmpl.addArgument("errorBias", STR_DOUBLE, "0.1");
    setConstraintCmdTmpl.finishInitialization("setConstraint");
    commandQueue.addCommandTemplate(&setConstraintCmdTmpl);

    //setSimpleMotorCmdTmpl.addArgument("isActive", STR_BOOL, "1");
    setSimpleMotorCmdTmpl.addArgument("rate", STR_DOUBLE, "0");
    setSimpleMotorCmdTmpl.addArgument("maxForce", STR_DOUBLE, "1");
    setSimpleMotorCmdTmpl.finishInitialization("setSimpleMotor");
    commandQueue.addCommandTemplate(&setSimpleMotorCmdTmpl);

    //setConstantAngleConstraintCmdTmpl.addArgument("isActive", STR_BOOL, "1");
    setConstantAngleConstraintCmdTmpl.addArgument("speed", STR_DOUBLE, "1");
    setConstantAngleConstraintCmdTmpl.addArgument("maxForce", STR_DOUBLE, "1");
    setConstantAngleConstraintCmdTmpl.finishInitialization("setConstantAngleConstraint");
    commandQueue.addCommandTemplate(&setConstantAngleConstraintCmdTmpl);

    setConstantAngleConstraintAngleCmdTmpl.finishInitialization("setConstantAngleConstraintAngle");
    commandQueue.addCommandTemplate(&setConstantAngleConstraintAngleCmdTmpl);

    setRotaryLimitConstraintMinCmdTmpl.finishInitialization("setRotaryLimitConstraintMin");
    commandQueue.addCommandTemplate(&setRotaryLimitConstraintMinCmdTmpl);
    setRotaryLimitConstraintMaxCmdTmpl.finishInitialization("setRotaryLimitConstraintMax");
    commandQueue.addCommandTemplate(&setRotaryLimitConstraintMaxCmdTmpl);
    setRotaryLimitConstraintCmdTmpl.addArgument("min", STR_DOUBLE, "0");
    setRotaryLimitConstraintCmdTmpl.addArgument("max", STR_DOUBLE, "0");
    setRotaryLimitConstraintCmdTmpl.finishInitialization("setRotaryLimitConstraint");
    commandQueue.addCommandTemplate(&setRotaryLimitConstraintCmdTmpl);


    //setFaceEachOtherConstraintCmdTmpl.addArgument("isActive", STR_BOOL, "1");
    setFaceEachOtherConstraintCmdTmpl.addArgument("speed", STR_DOUBLE, "10");
    setFaceEachOtherConstraintCmdTmpl.addArgument("maxForce", STR_DOUBLE, "1");
    setFaceEachOtherConstraintCmdTmpl.addArgument("bodyBActive", STR_BOOL, "1");
    setFaceEachOtherConstraintCmdTmpl.finishInitialization("setFaceEachOtherConstraint");
    commandQueue.addCommandTemplate(&setFaceEachOtherConstraintCmdTmpl);


    setDampedSpringCmdTmpl.addArgument("restLength", STR_STRING, "x");
    setDampedSpringCmdTmpl.addArgument("stiffness", STR_STRING, "1000");
    setDampedSpringCmdTmpl.addArgument("damping", STR_STRING, "100");
    setDampedSpringCmdTmpl.finishInitialization("setDampedSpring");
    commandQueue.addCommandTemplate(&setDampedSpringCmdTmpl);


    createObjectCmdTmpl.addArgument("type", STR_STRING, "");
    createObjectCmdTmpl.addArgument("arg0", STR_DOUBLE, "0");
    createObjectCmdTmpl.addArgument("arg1", STR_DOUBLE, "0");
    createObjectCmdTmpl.addArgument("arg2", STR_DOUBLE, "0");
    createObjectCmdTmpl.addArgument("arg3", STR_DOUBLE, "0");
    createObjectCmdTmpl.addArgument("arg4", STR_DOUBLE, "0");
    createObjectCmdTmpl.addArgument("arg5", STR_DOUBLE, "0");
    createObjectCmdTmpl.addArgument("arg6", STR_DOUBLE, "0");
    createObjectCmdTmpl.addArgument("arg7", STR_DOUBLE, "0");
    createObjectCmdTmpl.addArgument("arg8", STR_DOUBLE, "0");
    createObjectCmdTmpl.addArgument("arg9", STR_DOUBLE, "0");
    createObjectCmdTmpl.addArgument("arg10", STR_DOUBLE, "0");
    createObjectCmdTmpl.finishInitialization("createObject");
    commandQueue.addCommandTemplate(&createObjectCmdTmpl);


    replicateGridCmdTmpl.addArgument("nx", STR_INT, "1");
    replicateGridCmdTmpl.addArgument("ny", STR_INT, "1");
    replicateGridCmdTmpl.addArgument("dx", STR_DOUBLE, "0");
    replicateGridCmdTmpl.addArgument("dy", STR_DOUBLE, "0");
    replicateGridCmdTmpl.finishInitialization("replicateGrid");
    commandQueue.addCommandTemplate(&replicateGridCmdTmpl);

    replicateRingCmdTmpl.addArgument("n", STR_INT, "1");
    replicateRingCmdTmpl.addArgument("r", STR_DOUBLE, "100");
    replicateRingCmdTmpl.addArgument("da", STR_DOUBLE, "0.1");
    replicateRingCmdTmpl.addArgument("fixedAngle", STR_BOOL, "1");
    replicateRingCmdTmpl.finishInitialization("replicateRing");
    commandQueue.addCommandTemplate(&replicateRingCmdTmpl);

    setSimpleMotorControllerCmdTmpl.addArgument("index", STR_INT, "1");
    setSimpleMotorControllerCmdTmpl.addArgument("on", STR_BOOL, "1");
    setSimpleMotorControllerCmdTmpl.addArgument("reverseKey", STR_STRING, "left");
    setSimpleMotorControllerCmdTmpl.addArgument("forwardKey", STR_STRING, "right");
    setSimpleMotorControllerCmdTmpl.finishInitialization("setSimpleMotorController");
    commandQueue.addCommandTemplate(&setSimpleMotorControllerCmdTmpl);


    setSimpleCharacterControllerCmdTmpl.addArgument("index", STR_INT, "1");
    setSimpleCharacterControllerCmdTmpl.addArgument("on", STR_BOOL, "1");
    setSimpleCharacterControllerCmdTmpl.addArgument("maxLateralSpeed", STR_DOUBLE, "10");
    setSimpleCharacterControllerCmdTmpl.addArgument("lateralForce", STR_DOUBLE, "10");
    setSimpleCharacterControllerCmdTmpl.addArgument("jumpForce", STR_DOUBLE, "100");
    setSimpleCharacterControllerCmdTmpl.addArgument("leftKey", STR_STRING, "left");
    setSimpleCharacterControllerCmdTmpl.addArgument("rightKey", STR_STRING, "right");
    setSimpleCharacterControllerCmdTmpl.addArgument("jumpKey", STR_STRING, "up");
    setSimpleCharacterControllerCmdTmpl.finishInitialization("setSimpleCharacterController");
    commandQueue.addCommandTemplate(&setSimpleCharacterControllerCmdTmpl);

    setInputInterfaceInfoVisibleCmdTmpl.addArgument("value", STR_BOOL, "1");
    setInputInterfaceInfoVisibleCmdTmpl.finishInitialization("setInputInterfaceInfoVisible");
    commandQueue.addCommandTemplate(&setInputInterfaceInfoVisibleCmdTmpl);


    fastNoise.SetSeed(64573);
    fastNoise.SetNoiseType(FastNoise::SimplexFractal);

    shaderPost.create("data/glsl/texture.vert", "data/glsl/texture3.frag");

    //geomRenderer.create();
    geomRenderer.camera = &camera;

    texture3.create(screenW, screenH);
    quadScreen.create(screenW, screenH);


    simulation.init(160);


    bodyMiddle = simulation.addBody(Body::dynamicBody);
    double a = 0, r = 100;
    int n = 16, k = 13;
    double d = 2.0*PI*k/n;
    for(int i=0; i<n; i++) {
      bodyMiddle->addLine(r*cos(a), r*sin(a), r*cos(a+d), r*sin(a+d));
      a += d;
    }
    bodyMiddle->setPosition(screenW*0.5, screenH*0.5);

    addScreenWalls(5);

    //shape.maxCurves = 1;

    collisionCategorySelectionGUI.init(screenW, screenH, &lockedBodyPart, &editingBody, &nearestObjectQuery);

    gravityArrow.addVertex(-1.08, 3.63);
    gravityArrow.addVertex(-0.58, 0.37);
    gravityArrow.addVertex(-2.23, 0.53);
    gravityArrow.addVertex(0.23, -3.82);
    gravityArrow.addVertex(2.23, 0.53);
    gravityArrow.addVertex(0.52, 0.37);
    gravityArrow.addVertex(0.73, 3.82);
    gravityArrow.triangulate();
    gravityArrow.useCachedTriangles = true;
  }

  void addScreenWalls(double r) {
    Line topWall(-10-r, 0-r, screenW+10+r, -r, r);
    Line bottomWall(-10-r, screenH+r, screenW+10+r, screenH+r, r);
    Line leftWall(0-r, -10-r, 0-r, screenH+10+r, r);
    Line rightWall(screenW+r, -10-r, screenW+r, screenH+10+r, r);
    Body *body = simulation.addBody(Body::staticBody, &topWall, 1, 1, 1);
    body = simulation.addBody(Body::staticBody, &bottomWall, 1, 1, 1);
    body = simulation.addBody(Body::staticBody, &leftWall, 1, 1, 1);
    body = simulation.addBody(Body::staticBody, &rightWall, 1, 1, 1);
  }





  void onUpdate() {
    for(int i=0; i<commandQueue.commands.size(); i++) {
      Command *cmd = NULL;

      if(cmd = commandQueue.popCommand(&printInputCmdTmpl)) {
        for(int i=0; ;) {
          int j = clockInputHelpString.find("\n", i);
          if(j == -1 || i == j) break;
          messageQueue.addMessage(clockInputHelpString.substr(i, j-i));
          i = j+1;
        }

        delete cmd;
      }
      if(cmd = commandQueue.popCommand(&addBodiesCmdTmpl)) {
        int n;
        double rMin, rMax;
        addBodiesCmdTmpl.fillValues(cmd, &n, &rMin, &rMax);
        std::vector<int> geomTypes;
        for(int i=0; i<cmd->getNumArguments()-3; i++) {
          int v;
          addBodiesCmdTmpl.fillValueInd(cmd, &v, 3+i);
          geomTypes.push_back(max(v, 0));
        }
        if(geomTypes.size() == 0) {
           geomTypes.push_back(0);
         }
         for(int i=0; i<n; i++) {
           int g = Random::get(geomTypes);
           GeomObject *geom = NULL;
           if(g == 0) geom = new Circle(Random::getDouble(rMin, rMax));
           else if(g == 1) geom = new Line(rd2(-rMax, rMax), rd2(-rMin, rMax), 1);
           else if(g == 2) geom = new Rect(rd(rMin, rMax)*2.0, rd(rMin, rMax)*2.0);
           else if(g == 3) geom = new Triangle(rd2(-rMax, rMax), rd2(-rMax, rMax), rd2(-rMax, rMax));
           else {
              Polygon *poly = new Polygon(g, rMin, rMax);
              geom = poly;
           }
           Body *body = simulation.addBody(Body::dynamicBody, geom);
           body->name = "random";
           body->setPosition(Random::getDouble(50, screenW-50), Random::getDouble(50, screenH-50));
           if(geom) delete geom;
         }
         delete cmd;
      }
      if(cmd = commandQueue.popCommand(&setDragForceCmdTmpl)) {
        setDragForceCmdTmpl.fillValues(cmd, &dragForce);
        delete cmd;
      }
      if(cmd = commandQueue.popCommand(&setBoundingRectsVisibleCmdTmpl)) {
        bool value;
        setBoundingRectsVisibleCmdTmpl.fillValues(cmd, &value);
        simulation.renderBoundingRects = value;
        delete cmd;
      }
      if(cmd = commandQueue.popCommand(&setContactPointsVisibleCmdTmpl)) {
        bool value;
        setContactPointsVisibleCmdTmpl.fillValues(cmd, &value);
        simulation.renderContactInfo = value;
        delete cmd;
      }
      if(cmd = commandQueue.popCommand(&setInputInterfaceInfoVisibleCmdTmpl)) {
        bool value;
        setContactPointsVisibleCmdTmpl.fillValues(cmd, &value);
        showInputInterfaces = value;
        delete cmd;
      }
      if(cmd = commandQueue.popCommand(&clearBodiesCmdTmpl)) {
        int n = max(1, cmd->getNumArguments());
        for(int i=0; i<n; i++) {
          std::string bodyType;
          clearBodiesCmdTmpl.fillValueInd(cmd, &bodyType, i);
          if(bodyType.compare("all") == 0) simulation.clearBodies(-1);
          else if(bodyType.compare("dynamic") == 0) simulation.clearBodies(Body::dynamicBody);
          else if(bodyType.compare("kinematic") == 0) simulation.clearBodies(Body::kinematicBody);
          else if(bodyType.compare("static") == 0) simulation.clearBodies(Body::staticBody);
          else if(bodyType.compare("nothing") == 0) {}
          else simulation.clearBodies(bodyType);
        }
        lockedBodyPart = NULL;
        nearestObjectQuery.result = 0;
        editingBody = NULL;
        toolMode = 0;
        editingMode = 0;
        if(tmpEditingGeomObject) {
          delete tmpEditingGeomObject;
          tmpEditingGeomObject = NULL;
        }
        delete cmd;
      }
      if(cmd = commandQueue.popCommand(&setTimeStepCmdTmpl)) {
        double value;
        setTimeStepCmdTmpl.fillValues(cmd, &value);
        simulation.setTimeStep(value);
        delete cmd;
      }
      if(cmd = commandQueue.popCommand(&setGravityCmdTmpl)) {
        double x, y;
        setGravityCmdTmpl.fillValues(cmd, &x, &y);
        simulation.setGravity(x, y);
        delete cmd;
      }
      if(cmd = commandQueue.popCommand(&setDampingCmdTmpl)) {
        double value;
        setDampingCmdTmpl.fillValues(cmd, &value);
        simulation.setDamping(value);
        delete cmd;
      }
      if(cmd = commandQueue.popCommand(&setNumIterationsCmdTmpl)) {
        int value;
        setNumIterationsCmdTmpl.fillValues(cmd, &value);
        simulation.setNumIterations(value);
        delete cmd;
      }
      if(cmd = commandQueue.popCommand(&setNumExtraIterationsCmdTmpl)) {
        int value;
        setNumExtraIterationsCmdTmpl.fillValues(cmd, &value);
        simulation.numExtraIterations = max(value, 0);
        delete cmd;
      }
      if(cmd = commandQueue.popCommand(&addScreenWallsCmdTmpl)) {
        double thickness;
        addScreenWallsCmdTmpl.fillValues(cmd, &thickness);
        addScreenWalls(thickness);
        delete cmd;
      }
      if(cmd = commandQueue.popCommand(&setDefaultDensityCmdTmpl)) {
        setDefaultDensityCmdTmpl.fillValues(cmd, &simulation.densityDefault);
        delete cmd;
      }
      if(cmd = commandQueue.popCommand(&setDefaultElasticityCmdTmpl)) {
        setDefaultElasticityCmdTmpl.fillValues(cmd, &simulation.elasticityDefault);
        delete cmd;
      }
      if(cmd = commandQueue.popCommand(&setDefaultFrictionCmdTmpl)) {
        setDefaultFrictionCmdTmpl.fillValues(cmd, &simulation.frictionDefault);
        delete cmd;
      }
      if(cmd = commandQueue.popCommand(&setObjectNameCmdTmpl)) {
        std::string name;
        setObjectNameCmdTmpl.fillValues(cmd, &name);
        if(lockedBodyPart) {
          lockedBodyPart->parentBody->name = name;
        }
        else if(editingBody) {
          editingBody->name = name;
        }
        delete cmd;
      }
      if(cmd = commandQueue.popCommand(&setObjectNameDefaultCmdTmpl)) {
        setObjectNameDefaultCmdTmpl.fillValues(cmd, &simulation.objectNameDefault);
        delete cmd;
      }
      if(cmd = commandQueue.popCommand(&replicateGridCmdTmpl)) {
        if(editingBody || lockedBodyPart || (compositeSelection && contains(simulation.composites, compositeSelection))) {
          double dx, dy;
          int nx, ny;
          replicateGridCmdTmpl.fillValues(cmd, &nx, &ny, &dx, &dy);

          if(compositeSelection && contains(simulation.composites, compositeSelection)) {
            simulation.replicateCompositeGrid(compositeSelection, nx, ny, Vec2d(dx, dy));
          }
          else if(!lockedBodyPart) {
            simulation.replicateBodyGrid(editingBody, nx, ny, events.mw, Vec2d(dx, dy));
          }
          else {
            simulation.replicateBodyGrid(lockedBodyPart->parentBody, nx, ny, events.mw, Vec2d(dx, dy));
          }
        }
        delete cmd;
      }
      if(cmd = commandQueue.popCommand(&replicateRingCmdTmpl)) {
        if(editingBody || lockedBodyPart) {
          double r, da;
          int n;
          bool fixedAngle;
          replicateRingCmdTmpl.fillValues(cmd, &n, &r, &da, &fixedAngle);
          if(!lockedBodyPart) {
            simulation.replicateBodyRing(editingBody, n, events.mw, r, da*PI/180.0, fixedAngle);
          }
          else {
            simulation.replicateBodyRing(lockedBodyPart->parentBody, n, events.mw, r, da*PI/180.0, fixedAngle);
          }
        }
        delete cmd;
      }
      if(cmd = commandQueue.popCommand(&setDensityCmdTmpl)) {
        if(editingBody || lockedBodyPart) {
          double value;
          int index;
          setDensityCmdTmpl.fillValues(cmd, &value, &index);
          if(!lockedBodyPart) {
            editingBody->setDensity(value);
          }
          else {
            if(index == 0) {
              lockedBodyPart->setDensity(value);
            }
            else if(index < 0) {
              lockedBodyPart->parentBody->setDensity(value);
            }
            else {
              index = clamp(index-1, 0, (int)lockedBodyPart->parentBody->bodyParts.size()-1);
              lockedBodyPart->parentBody->bodyParts[index]->setDensity(value);
            }
          }
        }
        delete cmd;
      }
      if(cmd = commandQueue.popCommand(&setElasticityCmdTmpl)) {
        if(editingBody || lockedBodyPart) {
          double value;
          int index;
          setElasticityCmdTmpl.fillValues(cmd, &value, &index);
          if(!lockedBodyPart) {
            editingBody->setElasticity(value);
          }
          else {
            if(index == 0) {
              lockedBodyPart->setElasticity(value);
            }
            else if(index < 0) {
              lockedBodyPart->parentBody->setElasticity(value);
            }
            else {
              index = clamp(index-1, 0, (int)lockedBodyPart->parentBody->bodyParts.size()-1);
              lockedBodyPart->parentBody->bodyParts[index]->setElasticity(value);
            }
          }
        }
        delete cmd;
      }
      if(cmd = commandQueue.popCommand(&setFrictionCmdTmpl)) {
        if(editingBody || lockedBodyPart) {
          double value;
          int index;
          setFrictionCmdTmpl.fillValues(cmd, &value, &index);
          if(!lockedBodyPart) {
            editingBody->setFriction(value);
          }
          else {
            if(index == 0) {
              lockedBodyPart->setFriction(value);
            }
            else if(index < 0) {
              lockedBodyPart->parentBody->setFriction(value);
            }
            else {
              index = clamp(index-1, 0, (int)lockedBodyPart->parentBody->bodyParts.size()-1);
              lockedBodyPart->parentBody->bodyParts[index]->setFriction(value);
            }
          }
        }
        delete cmd;
      }

      if(cmd = commandQueue.popCommand(&setConstraintCmdTmpl)) {
        if(editingConstraint) {
          double maxForce, maxBias, errorBias;
          bool collideBodies;
          setConstraintCmdTmpl.fillValues(cmd, &collideBodies, &maxForce, &maxBias, &errorBias);
          editingConstraint->setCollideBodies(collideBodies);
          if(cmd->getNumArguments() >= 1) editingConstraint->setMaxForce(maxForce*1.0e6);
          if(cmd->getNumArguments() >= 2) editingConstraint->setMaxBias(maxBias);
          if(cmd->getNumArguments() >= 3) editingConstraint->setErrorBias(errorBias);
        }
        delete cmd;
      }
      if(cmd = commandQueue.popCommand(&setSimpleMotorCmdTmpl)) {
        if(editingConstraint && editingConstraint->type == Constraint::simpleMotor) {
          double rate, maxForce;
          bool isActive;
          //setSimpleMotorCmdTmpl.fillValues(cmd, &isActive, &rate, &maxForce);
          setSimpleMotorCmdTmpl.fillValues(cmd, &rate, &maxForce);
          SimpleMotor *simpleMotor = dynamic_cast<SimpleMotor*>(editingConstraint);
          //simpleMotor->setActive(isActive);
          if(cmd->getNumArguments() >= 1) simpleMotor->setRate(rate);
          if(cmd->getNumArguments() >= 2) simpleMotor->setMaxForce(maxForce*1.0e6);
        }
        delete cmd;
      }
      if(cmd = commandQueue.popCommand(&setConstantAngleConstraintCmdTmpl)) {
        if(editingConstraint && editingConstraint->type == Constraint::constantAngleConstraint) {
          double speed, maxForce;
          bool isActive;
          //setConstantAngleConstraintCmdTmpl.fillValues(cmd, &isActive, &speed, &maxForce);
          setConstantAngleConstraintCmdTmpl.fillValues(cmd, &speed, &maxForce);
          ConstantAngleConstraint *c = dynamic_cast<ConstantAngleConstraint*>(editingConstraint);
          //c->setActive(isActive);
          if(cmd->getNumArguments() >= 1) c->setSpeed(speed);
          if(cmd->getNumArguments() >= 2) c->setMaxForce(maxForce*1.0e6);
        }
        delete cmd;
      }
      if(cmd = commandQueue.popCommand(&setConstantAngleConstraintAngleCmdTmpl)) {
        if(editingConstraint && editingConstraint->type == Constraint::constantAngleConstraint) {
          ConstantAngleConstraint *c = dynamic_cast<ConstantAngleConstraint*>(editingConstraint);
          c->updateAngle();
        }
        delete cmd;
      }
      if(cmd = commandQueue.popCommand(&setRotaryLimitConstraintMinCmdTmpl)) {
        if(editingConstraint && editingConstraint->type == Constraint::rotaryLimitConstraint) {
          RotaryLimitConstraint *r = dynamic_cast<RotaryLimitConstraint*>(editingConstraint);
          r->setMinAngle(r->bodyB->getAngle() - r->bodyA->getAngle());
        }
        delete cmd;
      }
      if(cmd = commandQueue.popCommand(&setRotaryLimitConstraintMaxCmdTmpl)) {
        if(editingConstraint && editingConstraint->type == Constraint::rotaryLimitConstraint) {
          RotaryLimitConstraint *r = dynamic_cast<RotaryLimitConstraint*>(editingConstraint);
          r->setMaxAngle(r->bodyB->getAngle() - r->bodyA->getAngle());
        }
        delete cmd;
      }
      if(cmd = commandQueue.popCommand(&setRotaryLimitConstraintCmdTmpl)) {
        if(editingConstraint && editingConstraint->type == Constraint::rotaryLimitConstraint) {
          RotaryLimitConstraint *r = dynamic_cast<RotaryLimitConstraint*>(editingConstraint);
          double min, max;
          setRotaryLimitConstraintCmdTmpl.fillValues(cmd, &min, &max);
          r->setAngles(min*PI/180.0, max*PI/180.0);
        }
        delete cmd;
      }

      if(cmd = commandQueue.popCommand(&setFaceEachOtherConstraintCmdTmpl)) {
        if(editingConstraint && editingConstraint->type == Constraint::faceEachOtherConstraint) {
          double speed, maxForce;
          bool bodyBActive;
          setFaceEachOtherConstraintCmdTmpl.fillValues(cmd, &speed, &maxForce, &bodyBActive);
          FaceEachOtherConstraint *f = dynamic_cast<FaceEachOtherConstraint*>(editingConstraint);
          if(cmd->getNumArguments() >= 1) f->setSpeed(speed);
          if(cmd->getNumArguments() >= 2) f->setMaxForce(maxForce*1.0e6);
          if(cmd->getNumArguments() >= 3) f->setBodyBDisabled(!bodyBActive);
        }
        delete cmd;
      }
      if(cmd = commandQueue.popCommand(&setDampedSpringCmdTmpl)) {
        if(editingConstraint && editingConstraint->type == Constraint::dampedSpring) {
          std::string restLengthStr, stiffnessStr, dampingStr;
          double restLength, stiffness, damping;
          setDampedSpringCmdTmpl.fillValues(cmd, &restLengthStr, &stiffnessStr, &dampingStr);

          DampedSpring *d = dynamic_cast<DampedSpring*>(editingConstraint);

          if(restLengthStr.compare("x") == 0) {
            restLength = distance(d->bodyA->getPosition(), d->bodyB->getPosition());
          }
          else if(!toDouble(restLengthStr, restLength)) {
            restLength = d->restLength;
          }
          d->setRestLength(restLength);

          if(cmd->getNumArguments() >= 2) {
            if(!toDouble(stiffnessStr, stiffness)) {
              stiffness = d->stiffness;
            }
            d->setStiffness(stiffness);
          }
          if(cmd->getNumArguments() >= 3) {
            if(!toDouble(dampingStr, damping)) {
              damping = d->damping;
            }
            d->setDamping(damping);
          }
        }
        delete cmd;
      }

      if(cmd = commandQueue.popCommand(&setSimpleMotorControllerCmdTmpl)) {
        int index;
        bool on;
        std::string forwardKeyStr, reverseKeyStr;
        setSimpleMotorControllerCmdTmpl.fillValues(cmd, &index, &on, &forwardKeyStr, &reverseKeyStr);
        int forwardKey = 0, reverseKey = 0;
        if(on && editingConstraint) {
          forwardKey = SDL_GetKeyFromName(forwardKeyStr.c_str());
          if(forwardKey == 0) messageQueue.addMessage("Invalid key name " + forwardKeyStr);
          reverseKey = SDL_GetKeyFromName(reverseKeyStr.c_str());
          if(reverseKey == 0) messageQueue.addMessage("Invalid key name " + reverseKeyStr);
        }
        simulation.setSimpleMotorController(index, on, editingConstraint, reverseKey, forwardKey);
        delete cmd;
      }
      if(cmd = commandQueue.popCommand(&setSimpleCharacterControllerCmdTmpl)) {
        int index;
        double maxLateralSpeed, lateralForce, jumpForce;
        bool on;
        std::string leftKeyStr, rightKeyStr, jumpKeyStr;
        setSimpleCharacterControllerCmdTmpl.fillValues(cmd, &index, &on, &maxLateralSpeed, &lateralForce, &jumpForce, &leftKeyStr, &rightKeyStr, &jumpKeyStr);
        int leftKey = 0, rightKey = 0, jumpKey = 0;

        Body *body = NULL;
        BodyPart *foot = NULL;

        if(on && (editingBody || lockedBodyPart)) {
          body = editingBody ? editingBody : lockedBodyPart->parentBody;
          foot = lockedBodyPart ? lockedBodyPart : editingBody->bodyParts[0];

          leftKey = SDL_GetKeyFromName(leftKeyStr.c_str());
          if(leftKey == 0) messageQueue.addMessage("Invalid key name " + leftKeyStr);
          rightKey = SDL_GetKeyFromName(rightKeyStr.c_str());
          if(rightKey == 0) messageQueue.addMessage("Invalid key name " + rightKeyStr);
          jumpKey = SDL_GetKeyFromName(jumpKeyStr.c_str());
          if(jumpKey == 0) messageQueue.addMessage("Invalid key name " + jumpKeyStr);
        }
        simulation.setSimpleCharacterController(index, on, body, foot, maxLateralSpeed, lateralForce, jumpForce, leftKey, rightKey, jumpKey);
        delete cmd;
      }

      if(cmd = commandQueue.popCommand(&createObjectCmdTmpl)) {
        double arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10;
        std::string type;
        createObjectCmdTmpl.fillValues(cmd, &type, &arg0, &arg1, &arg2, &arg3, &arg4, &arg5, &arg6, &arg7, &arg8, &arg9, &arg10);

        // arguments: n teeth, n holes, r1, r2 [, w1, w2, thickness h, thickness w]
        if(type.compare("gear") == 0) {
          int n = max((int)arg0, 1);
          int numHoles =  max(0, (int)arg1);
          double r1 = max(2.0, arg2);
          double r2 = max(r1, arg3);
          Body *body = simulation.addBody(Body::dynamicBody);
          body->name = "gear";
          Polygon poly;
          double a = 0;
          double u = 2.0 * PI / n;
          double bottomW = cmd->getNumArguments() >= 6 ? arg4 : 0.4;
          double topW = cmd->getNumArguments() >= 7 ? arg5 : 0.3;
          double obliqueW = (1.0 - bottomW - topW) * 0.5;
          for(int i=0; i<n; i++) {
            poly.addVertex(r1 * cos(a), r1 * sin(a));
            a += u * bottomW;
            poly.addVertex(r1 * cos(a), r1 * sin(a));
            a += u * obliqueW;
            poly.addVertex(r2 * cos(a), r2 * sin(a));
            a += u * topW;
            poly.addVertex(r2 * cos(a), r2 * sin(a));
            a += u * obliqueW;
          }

          double h = cmd->getNumArguments() >= 8 ? arg6 : min(r2-r1, r1*0.333);
          double w = cmd->getNumArguments() >= 9 ? arg7 : h;
          double q = atan(w*0.5 / (r1-h));
          double e = 2.0*PI/numHoles - q * 2.0;
          double re0 = sqrt(2.0*0.25*w*w);
          double ae0 = q + e * 0.5;
          int m = max(3, n/4 - 1);

          for(int i=0; i<numHoles; i++) {
            a = 2.0 * PI * i / numHoles;
            poly.holes.push_back(std::vector<Vec2d>());
            poly.holes[i].push_back(Vec2d(re0 * cos(a+ae0), re0 * sin(a+ae0)));
            for(int k=0; k<m; k++) {
              poly.holes[i].push_back(Vec2d((r1-h) * cos(a + q + e*(m-k-1)/(m-1)), (r1-h) * sin(a + q + e*(m-k-1)/(m-1))));
            }
          }

          poly.triangulateWithHoles();
          poly.useCachedTriangles = true;

          body->addBodyPart(&poly);
          body->setPosition(events.mw);
        }



        if(type.compare("gear2") == 0) {
          int n = max((int)arg0, 1);
          double r1 = max(2.0, arg1);
          double r2 = arg2;
          double w = max(0.5, arg3);
          Body *body = simulation.addBody(Body::dynamicBody);
          body->name = "gear2";
          for(int i=0; i<n; i++) {
            double a = 2.0*PI * i / n;
            body->addLine(0, 0, r2 * cos(a), r2 * sin(a), w);
          }
          if(r2 >= 0.5) {
            body->addCircle(r1, 0, 0, 0);
          }
          body->setPosition(events.mw);
        }

        // TODO ring option
        if(type.compare("chain") == 0 || type.compare("chainC") == 0) {
          bool useRects = type.compare("chain") == 0;
          int n = max((int)abs(arg0), 1);
          double w = max(1.0, arg1);
          double h = max(1.0, arg2);
          double overlap = arg3;
          bool collisionGroup = arg4 != 0.0;

          std::vector<Body*> bodies;
          bool vertical = h > w;
          Vec2d pivotPos = vertical ? Vec2d(w*0.5, overlap*0.5-h*0.5) : Vec2d(overlap*0.5-w*0.5, h*0.5);
          Vec2d pos(0, 0);
          for(int i=0; i<n; i++) {
            Body *body = simulation.addBody(Body::dynamicBody);
            if(useRects) {
              body->name = "chain";
              body->addRect(w, h, 0, 0, 0);
            }
            else {
              body->name = "chainC";
              body->addCircle(w, 0, 0, 0);
            }
            if(vertical) pos.y += h - overlap;
            else pos.x += w - overlap;
            bodies.push_back(body);
            body->setPosition(events.mw + pos);
            if(i > 0) {
              PivotJoint *pivotJoint = simulation.addPivotJoint(body, bodies[i-1], events.mw + pos + pivotPos);
              if(cmd->getNumArguments() >= 6) {
                pivotJoint->setMaxForce(arg5);
              }
              if(cmd->getNumArguments() >= 7) {
                pivotJoint->setMaxBias(arg6);
              }
              if(cmd->getNumArguments() >= 8) {
                pivotJoint->setErrorBias(arg7);
              }
            }
          }
          if(collisionGroup) {
            simulation.addCollisionGroup(bodies);
          }
        }

        delete cmd;
      }
    }

    Constraint *previousEditingConstraint = editingConstraint ? editingConstraint : NULL;

    if(nearestObjectDraggingMode == 0) {
      if(lockedBodyPart) {
        simulation.getNearestObject(events.mw, nearestObjectQuery, collisionFilterFindLocked);
      }
      else {
        simulation.getNearestObject(events.mw, nearestObjectQuery, collisionFilterDefault);
      }
      if(toolMode == toolModeEditObject) {
        if(!editingConstraint || !lockEditingConstraint) {
          double distConstraint = 1e100;
          editingConstraint = NULL;
          for(Constraint *constraint : simulation.constraints) {
            if(editingBody == constraint->bodyA || editingBody == constraint->bodyB) {
              double dist = distance(constraint->getPosition(), events.mw);
              if(dist < distConstraint) {
                distConstraint = dist;
                editingConstraint = constraint;
              }
            }
          }
          editingConstraintNearerThanBodyPart = max(0.0, distConstraint-10) < fabs(nearestObjectQuery.distance);
        }
      }
    }
    else {
      if(toolMode == toolModeDefault) {
        Vec2d p = nearestObjectQuery.body->localToWorld(nearestObjectBindingPointLocal);
        Vec2d f = events.mw - p;
        nearestObjectQuery.body->applyForceAtWorldPoint(f*dragForce, p);
      }
    }


    if(editingConstraint && previousEditingConstraint != editingConstraint) {
      closeConstraintPanel();
      showConstraintPanel();
    }

    if(!editingConstraint && constraintPanel) {
      closeConstraintPanel();
    }



    simulation.update(dt);
  }


  void initEditing(Events &events) {
    toolMode = toolModeEditObject;
    if(!editingBody) {
      if(lockedBodyPart) {
        editingBody = lockedBodyPart->parentBody;
      }
      else {
        if(events.lControlDown && events.numModifiersDown == 1) {
          editingBody = simulation.addBody(Body::staticBody);
        }
        else if(events.lShiftDown && events.numModifiersDown == 1) {
          editingBody = simulation.addBody(Body::kinematicBody);
        }
        else {
          editingBody = simulation.addBody(Body::dynamicBody);
        }
        editingBody->setPosition(events.mw);

      }
    }
    simulation.removeEverythingFromSpace();
    simulation.renderConstraints = true;
    simulation.setSimulationPaused(true);

    editingBody->isBoundingRectVisible = true;
  }


  void finishEditing() {
    if(editingBody) {
      if(editingMode == editingModeNewPolygon) {
        finishNewPolygon();
      }
      if(editingBody->bodyParts.size() == 0) {
        simulation.removeBody(editingBody);
      }
      unsetEditingBody();
    }
    if(editingBodyB) {
      unsetEditingBodyB();
    }
    simulation.putEverythingBackToSpace();
    simulation.renderConstraints = false;
    simulation.setSimulationPaused(false);
    toolMode = toolModeDefault;
  }

  void finishNewPolygon() {
    editingMode = 0;
    newPolygon.clear();
    if(shape.curves.size() > 0 && shape.curves[0].points.size() > 2) {
      for(int p=0; p<shape.curves[0].points.size(); p++) {
        newPolygon.vertices.push_back(editingBody->worldToLocal(shape.curves[0].points[p]));
      }

      if(shape.curves.size() > 1) {
        int numHoles = 0;
        for(int h=1; h<shape.curves.size(); h++) {
          if(shape.curves[h].size() >= 3) {
            newPolygon.holes.push_back(std::vector<Vec2d>());
            for(int p=0; p<shape.curves[h].points.size(); p++) {
              // FIXME it would be nice for the triangulator if the winding direction would be forced here
              //       to be the same as that of the outer polygon (actually the opposite direction)
              newPolygon.holes[numHoles].push_back(editingBody->worldToLocal(shape.curves[h].points[p]));
            }
            numHoles++;
          }
        }
      }

      newPolygon.useCachedTriangles = false;
      newPolygon.triangulateWithHoles();
      newPolygon.useCachedTriangles = true;

      editingBody->addBodyPart(&newPolygon);
      simulation.onBodyUpdate();

      // FIXME not sure if that mathematical truth (apparently), applies to the holed polygon in any way...
      /*if(newPolygon.vertices.size() == newPolygon.triangles.size()+2) {
        editingBody->addBodyPart(&newPolygon, densityDefault, elasticityDefault, frictionDefault);
        simulation.onBodyUpdate();
      }
      else {
        printf("Failed to create new polygon bodypart: number of vertices (%lu) and triangles (%lu) don't agree\n", newPolygon.vertices.size(), newPolygon.triangles.size());
      }*/
    }
  }

  void lockNearestBodyPart() {
    lockedBodyPart = nearestObjectQuery.bodyPart;
    //lockedBodyPart->parentBody->setCollisionFilter(collisionFilterLocked);
    lockedBodyPart->chipCollisionFilter.categories = lockedBodyPart->chipCollisionFilter.categories | (1<<31);
    lockedBodyPart->setCollisionFilter(lockedBodyPart->chipCollisionFilter);
  }
  void unlockNearestBodyPart() {
    if(lockedBodyPart) {
      //lockedBodyPart->parentBody->setCollisionFilter(collisionFilterDefault);
      lockedBodyPart->chipCollisionFilter.categories = lockedBodyPart->chipCollisionFilter.categories & ~(1<<31);
      lockedBodyPart->setCollisionFilter(lockedBodyPart->chipCollisionFilter);
      lockedBodyPart = NULL;
    }
  }

  void showEditingBodyPanel() {
    editingBodyPanel = new EditingBodyPanel(lockedBodyPart ? lockedBodyPart : editingBody->bodyParts[0]);
    editingBodyPanel->pos.set(screenW - editingBodyPanel->size.x - 60, 10);
    guiRoot.addChildElement(editingBodyPanel);
  }

  void showConstraintPanel() {
    if(editingConstraint && !constraintPanel) {
      constraintPanel = new ConstraintPanel(editingConstraint);
      constraintPanel->pos.set(screenW - constraintPanel->size.x - 60, screenH - constraintPanel->size.y - 10);
      guiRoot.addChildElement(constraintPanel);
    }
  }

  void closeConstraintPanel() {
    if(constraintPanel) {
      guiRoot.deleteChildElement(constraintPanel);
      constraintPanel = NULL;
    }
  }


  void setEditingBody(Body *body) {
    if(editingBody) unsetEditingBody();
    editingBody = body;
    editingBody->isBoundingRectVisible = true;
    editingBody->setVelocity(Vec2d::Zero);
    editingBody->setAnglularVelocity(0);

    showEditingBodyPanel();
    showConstraintPanel();
  }

  void unsetEditingBody(bool shutGuis = false) {
    //if(editingBody) {
    editingBody->isBoundingRectVisible = false;
    editingBody = NULL;
    //}

    if(editingBodyPanel) {
      guiRoot.deleteChildElement(editingBodyPanel);
      editingBodyPanel = NULL;
    }

    if(shutGuis || !(editingConstraint && lockEditingConstraint)) {
      closeConstraintPanel();
    }
  }

  void setEditingBodyB(Body *body) {
    if(editingBodyB) unsetEditingBodyB();
    editingBodyB = body;
    editingBodyB->renderingSettingsBoundingRect.fillColor.set(0.07, 0, 0.4, 0.1);
    editingBodyB->renderingSettingsBoundingRect.strokeColor.set(0.07, 0, 0.4, 0.3);
    editingBodyB->isBoundingRectVisible = true;
    editingBodyB->setVelocity(Vec2d::Zero);
    editingBodyB->setAnglularVelocity(0);
  }
  void unsetEditingBodyB() {
    if(editingBodyB) {
      editingBodyB->setDefaultRenderingSettings();
      editingBodyB->isBoundingRectVisible = false;
      editingBodyB = NULL;
    }
  }
  void swapEditingBodies() {
    Body *tmp = editingBody, *tmpB = editingBodyB;
    unsetEditingBody();
    unsetEditingBodyB();
    setEditingBody(tmpB);
    setEditingBodyB(tmp);
  }


  void onQuit() {
    simulation.finalize();
  }

  void onKeyUp() {
    simulation.onKeyUp(events);
  }

  void onKeyDown() {
    simulation.onKeyDown(events);



    guiInputCapture = guiInputCapture || collisionCategorySelectionGUI.onKeyDown(events, simulation);

    if(guiInputCapture) return;
    /*if(events.sdlKeyCode == SDLK_SPACE) {
      shape.print();
    }*/



    if(events.sdlKeyCode == SDLK_d && events.lControlDown) {
      debugTriangulation = !debugTriangulation;
    }

    if(events.sdlKeyCode == SDLK_DELETE) {
      if(editingConstraint && editingConstraintNearerThanBodyPart) {
        simulation.removeConstraint(editingConstraint);
        editingConstraint = NULL;
      }
      else if(editingBody) {
        simulation.removeBody(editingBody);
        //editingBody = NULL;
        unsetEditingBody(true);
        unsetEditingBodyB();
        finishEditing();
        editingConstraint = NULL;
      }
      else if(lockedBodyPart) {
        if(lockedBodyPart->parentBody == editingBody) {
          //editingBody = NULL;
          unsetEditingBody(true);
          unsetEditingBodyB();
          finishEditing();
        }
        else if(lockedBodyPart->parentBody == editingBodyB){
          editingBodyB = NULL;
        }
        simulation.removeBody(lockedBodyPart->parentBody);
        editingConstraint = NULL;
        lockedBodyPart = NULL;
        nearestObjectQuery.result = 0;
      }
      else if(nearestObjectDraggingMode > 0 && nearestObjectQuery.result > 0) {
        if(nearestObjectQuery.body == editingBody) {
          //editingBody = NULL;
          unsetEditingBody(true);
          unsetEditingBodyB();
          finishEditing();
        }
        else if(nearestObjectQuery.body == editingBodyB){
          editingBodyB = NULL;
        }
        simulation.removeBody(nearestObjectQuery.body);
        nearestObjectQuery.result = 0;
        nearestObjectDraggingMode = 0;
        editingConstraint = NULL;
      }
    }

    if(events.sdlKeyCode == SDLK_e) {
      if(toolMode != toolModeEditObject) {
        if(lockedBodyPart || nearestObjectQuery.result != 0) {
          toolMode = toolModeEditObject;
          simulation.removeEverythingFromSpace();
          simulation.renderConstraints = true;
          simulation.setSimulationPaused(true);
          setEditingBody(lockedBodyPart ? lockedBodyPart->parentBody : nearestObjectQuery.body);
        }
      }
      else if(!tmpEditingGeomObject) {
        finishEditing();
      }

    }

    if(events.sdlKeyCode == SDLK_c) {
      initEditing(events);
      editingMode = editingModeNewCirclePos;
      newCircle.pos = events.mw;
      newCircle.r = 10;
    }
    if(events.sdlKeyCode == SDLK_h) {
      initEditing(events);
      editingMode = editingModeNewHoopPos;
      newHoop.pos = events.mw;
      newHoop.r = 10;
      newHoop.r2 = 7;
    }
    if(events.sdlKeyCode == SDLK_l) {
      initEditing(events);
      editingMode = editingModeNewLineA;
      newLine.a = events.mw;
      newLine.b = events.mw;
    }
    if(events.sdlKeyCode == SDLK_r) {
      initEditing(events);
      editingMode = editingModeNewRectPos;
      newRect.pos = events.mw;
      newRect.w = 10;
      newRect.h = 10;
    }


    if(events.sdlKeyCode == SDLK_s) {
      if(editingMode == editingModeNewPolygon) {
        finishNewPolygon();
      }
      else {
        initEditing(events);
        editingMode = editingModeNewPolygon;
      }
    }
    if(toolMode == toolModeEditObject && editingBody) {
      if(editingMode == editingModeNewPolygon) {
        shape.onKeyDown(events);
      }
      if(editingMode == 0) {
        if(events.sdlKeyCode == SDLK_a && editingBodyB) {
          simulation.combineBodies(editingBody, editingBodyB);
          editingBodyB = NULL;
        }
        if(events.sdlKeyCode == SDLK_q) {
          if(nearestObjectQuery.result != 0 && nearestObjectQuery.body == editingBody && editingBody->bodyParts.size() > 1) {
            Body *body = simulation.seperateBodyPartToNewBody(editingBody, nearestObjectQuery.bodyPart);
            //setEditingBodyB(body);
          }
        }
        if(events.sdlKeyCode == SDLK_v) {
          if(compositeSelection) {
            if(editingBodyB && simulation.addToComposite(compositeSelection, editingBodyB)) {}
            else {
              simulation.removeComposite(compositeSelection);
              compositeSelection = NULL;
            }
          }
          else {
            compositeSelection = simulation.createComposite(editingBody);
          }
        }
        if(events.sdlKeyCode == SDLK_1) {
          editingMode = editingModeNewPivotJointPos;
        }
        if(events.sdlKeyCode == SDLK_2) {
          simulation.addSimpleMotor(editingBody, editingBodyB);
        }
        if(events.sdlKeyCode == SDLK_3) {
          simulation.addConstantAngleConstraint(editingBody, editingBodyB);
        }
        if(events.sdlKeyCode == SDLK_4 && editingBodyB) {
          simulation.addFaceEachOtherConstraint(editingBody, editingBodyB);
        }
        if(events.sdlKeyCode == SDLK_5) {
          simulation.addRotaryLimitConstraint(editingBody, editingBodyB);
        }
        if(events.sdlKeyCode == SDLK_6) {
          editingMode = editingModeNewGrooveJointPosA;
          newConstraintPosA = events.mw;
          newConstraintPosB = events.mw;
        }
        if(events.sdlKeyCode == SDLK_7) {
          editingMode = editingModeNewDampedSpringPosA;
          newConstraintPosA = events.mw;
          newConstraintPosB = events.mw;
        }
        if(events.sdlKeyCode == SDLK_g) {
          if(nearestObjectQuery.result != 0) {
            CollisionGroup *collisionGroup = simulation.getCollisionGroup(editingBody);
            if(editingBody == nearestObjectQuery.body && !collisionGroup) {
              simulation.addCollisionGroup(editingBody);
            }
            else if(editingBody == nearestObjectQuery.body && collisionGroup) {
              if(collisionGroup->isOwner(editingBody)) {
                simulation.removeCollisionGroup(collisionGroup);
              }
              else {
                collisionGroup->removeBody(editingBody);
              }
            }
            else if(editingBody != nearestObjectQuery.body && collisionGroup) {
              if(simulation.getCollisionGroup(nearestObjectQuery.body) == collisionGroup) {
                if(collisionGroup->isOwner(nearestObjectQuery.body)) {
                  simulation.removeCollisionGroup(collisionGroup);
                }
                else {
                  collisionGroup->removeBody(nearestObjectQuery.body);
                }
              }
              else {
                collisionGroup->addBody(nearestObjectQuery.body);
              }
            }
            if(collisionGroup && collisionGroup->isEmpty()) {
              simulation.removeCollisionGroup(collisionGroup);
            }
          }
        }
      }
    }
  }


  void onMouseWheel() {
    if(guiInputCapture) return;

    if(toolMode == toolModeEditObject) {
      if(editingMode == editingModeNewPolygon) {
        shape.onMouseWheel(events);
      }
    }
  }

  void onMousePressed() {

    guiInputCapture = guiInputCapture || collisionCategorySelectionGUI.onMousePressed(events);

    if(guiInputCapture) {
      return;
    }

    if(editingMode == editingModeNewPolygon) {
      shape.onMousePressed(events);
      return;
    }

    if(events.mouseDownM) {
      /*if(lockedBodyPart && events.numModifiersDown == 0) {
        //unlockNearestBodyPart();
      }*/
      if(editingConstraint && editingConstraintNearerThanBodyPart && events.numModifiersDown == 0) {
        lockEditingConstraint = !lockEditingConstraint;
      }
      else if(nearestObjectQuery.result != 0) {
        if(events.lControlDown && events.lShiftDown && events.numModifiersDown == 2) {
          if(editingBody == nearestObjectQuery.body) {
            //editingBody = NULL;
            unsetEditingBody(true);
            unsetEditingBodyB();
            finishEditing();
          }
          if(editingBodyB == nearestObjectQuery.body) {
            editingBodyB = NULL;
          }
          simulation.removeBody(nearestObjectQuery.body);
          nearestObjectQuery.result = 0;
          lockedBodyPart = NULL;
          editingConstraint = NULL;
        }
        else if(events.lControlDown && events.numModifiersDown == 1) {
          // FIXME this shouldn't happen here
          for(auto &intInputInterfacePair : simulation.inputInterfaces) {
            intInputInterfacePair.second->onBodyPartRemoval(nearestObjectQuery.bodyPart);
          }
          bool isBodyPartActiveInGui = editingBodyPanel && editingBodyPanel->bodyPart == nearestObjectQuery.bodyPart;
          bool isBodyPartsLeft = nearestObjectQuery.body->removeBodyPart(nearestObjectQuery.bodyPart);

          if(isBodyPartsLeft && isBodyPartActiveInGui) {
            editingBodyPanel->set(editingBody->bodyParts[0]);
          }

          if(!isBodyPartsLeft) {
            if(editingBody == nearestObjectQuery.body) {
              //editingBody = NULL;
              unsetEditingBody(true);
              unsetEditingBodyB();
              finishEditing();
            }
            if(editingBodyB == nearestObjectQuery.body) {
              editingBodyB = NULL;
            }
            simulation.removeBody(nearestObjectQuery.body);
            editingConstraint = NULL;
          }
          nearestObjectQuery.result = 0;
          lockedBodyPart = NULL;
        }
        else  {
          //lockNearestBodyPart();
        }
      }
    }
    if(toolMode == toolModeDefault) {
      if(events.mouseDownL && events.lControlDown && events.lShiftDown && events.lAltDown && events.numModifiersDown == 3) {
        Vec2d g = events.mw - camera.toCameraSpace(Vec2d(screenW*0.5, screenH * 0.5));
        g.setLength(map(g.length()*camera.scale, 0, screenW*0.5, 0, maxGravity));
        simulation.setGravity(g.x, g.y);
      }
      if(events.mouseDownL && events.numModifiersDown == 0) {
        if(nearestObjectQuery.result != NearestObjectQuery::nothingFound) {
          nearestObjectDraggingMode = 1;
          nearestObjectBindingPointLocal = nearestObjectQuery.body->worldToLocal(nearestObjectQuery.closestPoint);
        }
      }
    }

    if(!(editingConstraint && editingConstraintNearerThanBodyPart && editingBody) && events.mouseDownM && events.numModifiersDown == 0) {
      if(nearestObjectQuery.result != NearestObjectQuery::nothingFound) {
        //if(toolMode == 0 || (toolMode == toolModeEditObject && editingBody == nearestObjectQuery.body)) {
          if(lockedBodyPart == nearestObjectQuery.bodyPart) {
            unlockNearestBodyPart();
          }
          else {
            lockNearestBodyPart();

            if(editingBodyPanel && lockedBodyPart->parentBody == editingBody) {
              editingBodyPanel->set(lockedBodyPart);
            }
          }
        //}
      }
    }

    if(toolMode == toolModeEditObject) {
      if(editingMode == 0) {
        if(events.mouseDownM && events.numModifiersDown == 0) {
          if(nearestObjectQuery.result != NearestObjectQuery::nothingFound) {
            if(editingBody != nearestObjectQuery.body && editingBodyB != nearestObjectQuery.body) {
              setEditingBodyB(nearestObjectQuery.body);
            }
            else if(editingBodyB == nearestObjectQuery.body) {
              swapEditingBodies();
            }
            else if(editingBodyB) {
              unsetEditingBodyB();
            }
          }
        }

        if(events.mouseDownL) {
          if(editingConstraint && editingConstraintNearerThanBodyPart) {
            editingMode = editingModeMoveConstraint;
          }
          else {
            if(events.lShiftDown && events.numModifiersDown == 1 && nearestObjectQuery.result != 0)  {
              if(nearestObjectQuery.body == editingBody) {
                editingMode = editingModeMoveBodyPart;
                tmpEditingGeomObject = copyGeomObject(nearestObjectQuery.bodyPart->geomObject);
                // FIXME this shouldn't happen here
                for(auto &intInputInterfacePair : simulation.inputInterfaces) {
                  intInputInterfacePair.second->onBodyPartRemoval(nearestObjectQuery.bodyPart);
                }
                editingBody->removeBodyPart(nearestObjectQuery.bodyPart);
                nearestObjectQuery.result = 0;
                lockedBodyPart = NULL;
              }
            }
            if(events.lControlDown && events.lShiftDown && events.numModifiersDown == 2 && nearestObjectQuery.result != 0)  {
              if(nearestObjectQuery.body == editingBody) {
                editingMode = editingModeRotateBodyPart;
                tmpEditingGeomObject = copyGeomObject(nearestObjectQuery.bodyPart->geomObject);
                // FIXME this shouldn't happen here
                for(auto &intInputInterfacePair : simulation.inputInterfaces) {
                  intInputInterfacePair.second->onBodyPartRemoval(nearestObjectQuery.bodyPart);
                }
                editingBody->removeBodyPart(nearestObjectQuery.bodyPart);
                nearestObjectQuery.result = 0;
                lockedBodyPart = NULL;
                //printf("...\n");
              }
            }
            if(events.numModifiersDown == 0) {
              if(editingBodyB) {
                editingMode = editingModeMoveBodyB;
              }
              else {
                editingMode = editingModeMoveBody;
              }
            }
            if(events.lControlDown && events.numModifiersDown == 1) {
              if(editingBodyB) {
                editingMode = editingModeRotateBodyB;
              }
              else {
                editingMode = editingModeRotateBody;
              }
            }
          }
        }
      }
      if(editingMode == editingModeNewPivotJointPos) {
        simulation.addPivotJoint(editingBody, editingBodyB, newConstraintPosA);
        editingMode = 0;
      }
      if(editingMode == editingModeNewGrooveJointPosA) {
        editingMode = editingModeNewGrooveJointPosB;
      }
      if(editingMode == editingModeNewDampedSpringPosA) {
        editingMode = editingModeNewDampedSpringPosB;
      }
      if(editingMode == editingModeNewCirclePos) {
        editingMode = editingModeNewCircleScale;
      }
      if(editingMode == editingModeNewHoopPos) {
        editingMode = editingModeNewHoopScale;
      }
      if(editingMode == editingModeNewRectPos) {
        editingMode = editingModeNewRectSize;
      }
      if(editingMode == editingModeNewLineA) {
        editingMode = editingModeNewLineB;
      }
      /*if(editingMode == editingModeNewPolygon) {
        shape.onMousePressed(events);
      }*/
    }
  }



  void onMouseReleased() {
    if(guiInputCapture) return;

    if(events.mouseUpL) {
      nearestObjectDraggingMode = 0;
    }

    if(editingMode == editingModeNewGrooveJointPosB) {
      simulation.addGrooveJoint(editingBody, editingBodyB, newConstraintPosA, newConstraintPosB);
    }
    if(editingMode == editingModeNewDampedSpringPosB) {
      simulation.addDampedSpring(editingBody, editingBodyB, newConstraintPosA, newConstraintPosB);
    }

    if(toolMode == toolModeEditObject && editingBody) {
      bool bodyPartCreated = false;

      if(editingMode == editingModeNewCircleScale) {
        newCircle.r = max(3.0, newCircle.r);
        newCircle.pos = editingBody->worldToLocal(newCircle.pos);
        editingBody->addBodyPart(&newCircle);
        simulation.onBodyUpdate();
        bodyPartCreated = true;
      }
      if(editingMode == editingModeNewHoopInnerRadius) {
        //newHoop.r2 = min(newHoop.r, newHoop.r2);
        newHoop.pos = editingBody->worldToLocal(newHoop.pos);
        editingBody->addBodyPart(&newHoop);
        simulation.onBodyUpdate();
        bodyPartCreated = true;
        editingMode = 0;
      }
      if(editingMode == editingModeNewHoopScale) {
        newHoop.r = max(3.0, newHoop.r);
        editingMode = editingModeNewHoopInnerRadius;
      }
      if(editingMode == editingModeNewRectSize) {
        newRect.w = max(3.0, newRect.w);
        newRect.h = max(3.0, newRect.h);
        newRect.pos = editingBody->worldToLocal(newRect.pos);
        editingBody->addBodyPart(&newRect);
        simulation.onBodyUpdate();
        bodyPartCreated = true;
      }
      if(editingMode == editingModeNewLineB) {
        newLine.a = editingBody->worldToLocal(newLine.a);
        newLine.b = editingBody->worldToLocal(newLine.b);
        editingBody->addBodyPart(&newLine);
        simulation.onBodyUpdate();
        bodyPartCreated = true;
      }
      if(editingMode == editingModeNewPolygon) {
        shape.onMouseReleased(events);
      }
      if((editingMode == editingModeMoveBodyPart || editingMode == editingModeRotateBodyPart) && tmpEditingGeomObject) {
        // FIXME copy other parameters too
        //tmpEditingGeomObject->translate(editingBody->getPosition() * (-1));
        editingBody->addBodyPart(tmpEditingGeomObject);
        delete tmpEditingGeomObject;
        tmpEditingGeomObject = NULL;
        simulation.onBodyUpdate();
      }
      if(bodyPartCreated && !editingBodyPanel) {
        showEditingBodyPanel();
      }
    }

    if(editingMode != editingModeNewPolygon && editingMode != editingModeNewHoopInnerRadius) {
      editingMode = 0;
    }

  }

  void onMouseMotion() {
    guiInputCapture = guiInputCapture || collisionCategorySelectionGUI.onMouseMotion(events);

    if(guiInputCapture) return;

    if(toolMode == toolModeDefault) {
      if(events.mouseDownL && events.lControlDown && events.lShiftDown && events.lAltDown && events.numModifiersDown == 3) {
        Vec2d g = events.mw - camera.toCameraSpace(Vec2d(screenW*0.5, screenH * 0.5));
        g.setLength(map(g.length()*camera.scale, 0, screenW*0.5, 0, maxGravity));
        simulation.setGravity(g.x, g.y);
      }
    }
    if(toolMode == toolModeEditObject && editingBody) {
      if(editingMode == editingModeNewPivotJointPos) {
        newConstraintPosA = events.mw;
      }
      if(editingMode == editingModeNewGrooveJointPosA) {
        newConstraintPosA = events.mw;
        newConstraintPosB = events.mw;
      }
      if(editingMode == editingModeNewGrooveJointPosB) {
        newConstraintPosB = events.mw;
      }
      if(editingMode == editingModeNewDampedSpringPosA) {
        newConstraintPosA = events.mw;
        newConstraintPosB = events.mw;
      }
      if(editingMode == editingModeNewDampedSpringPosB) {
        newConstraintPosB = events.mw;
      }


      if(editingMode == editingModeNewCirclePos) {
        newCircle.pos = events.mw;
      }
      if(editingMode == editingModeNewHoopPos) {
        newHoop.pos = events.mw;
      }
      if(editingMode == editingModeNewCircleScale) {
        newCircle.r = distance(newCircle.pos, events.mw);
      }
      if(editingMode == editingModeNewHoopScale) {
        newHoop.r = distance(newHoop.pos, events.mw);
      }
      if(editingMode == editingModeNewHoopInnerRadius) {
        newHoop.r2 = distance(newHoop.pos, events.mw);
        newHoop.r2 = min(newHoop.r2, newHoop.r - 1.0);
      }
      if(editingMode == editingModeNewRectPos) {
        newRect.pos = events.mw;
      }
      if(editingMode == editingModeNewRectSize) {
        newRect.w = fabs(newRect.pos.x - events.mw.x) * 2.0;
        newRect.h = fabs(newRect.pos.y - events.mw.y) * 2.0;
      }
      if(editingMode == editingModeNewLineA) {
        newLine.a = events.mw;
        newLine.b = events.mw;
      }
      if(editingMode == editingModeNewLineB) {
        newLine.b = events.mw;
      }
      if(editingMode == editingModeNewPolygon) {
        shape.onMouseMotion(events, &camera);
      }
      if(editingMode == editingModeRotateBody) {
        double da = -(events.m.x - events.mp.x) * 2.0*PI / (360.0 * 4.0);
        editingBody->rotate(da);
        simulation.onBodyUpdate();
      }
      if(editingMode == editingModeRotateBodyB) {
        double da = -(events.m.x - events.mp.x) * 2.0*PI / (360.0 * 4.0);
        editingBodyB->rotate(da);
        simulation.onBodyUpdate();
      }
      if(editingMode == editingModeMoveBody) {
        if(compositeSelection && contains(simulation.composites, compositeSelection)) {
          compositeSelection->translate(events.mw - events.mwp);
        }
        else {
          editingBody->setPosition(editingBody->getPosition() + events.mw - events.mwp);
        }
        simulation.onBodyUpdate();
      }
      if(editingMode == editingModeMoveBodyB && editingBodyB) {
        editingBodyB->setPosition(editingBodyB->getPosition() + events.mw - events.mwp);
        simulation.onBodyUpdate();
      }
      if(editingMode == editingModeMoveBodyPart && tmpEditingGeomObject) {
        Vec2d t = events.mw - events.mwp;
        t.rotate(-editingBody->getAngle());
        tmpEditingGeomObject->translate(t);
        simulation.onBodyUpdate();
      }
      if(editingMode == editingModeRotateBodyPart && tmpEditingGeomObject) {
        double da = -(events.m.x - events.mp.x) * 2.0*PI / (360.0 * 4.0);
        tmpEditingGeomObject->rotate(da);
        simulation.onBodyUpdate();
      }
      if(editingMode == editingModeMoveConstraint && editingConstraint) {
        editingConstraint->translate(events.mw - events.mwp);
      }
    }
  }

  void onReloadShaders() {
    geomRenderer.reloadShaders();
  }




  void onDraw() {
    //texture3.setRenderTarget();

    clear(0.18, 0, 0.05, 1);
    //clear(0, 0, 0, 1);
    
    geomRenderer.sdlInterface->glmMatrixStack.loadIdentity();
    //glLoadIdentity();


    camera.pushMatrix();

    CollisionGroup *collisionGroup = simulation.getCollisionGroup(editingBody);
    if(toolMode == toolModeEditObject && editingBody && collisionGroup) {
      collisionGroup->updateBoundingRect();
      collisionGroup->render(geomRenderer);
    }

    simulation.render(geomRenderer);

    Triangle *dbgTriangle = NULL;
    int dbgTriangleIndex = -1;

    if(toolMode == toolModeEditObject && editingBody) {
      geomRenderer.fillColor.set(0, 0, 0, 0.7);
      geomRenderer.strokeColor.set(0, 0, 0, 1);
      geomRenderer.strokeWidth = 2;
      geomRenderer.strokeType = 1;

      if(editingMode == editingModeNewCirclePos || editingMode == editingModeNewCircleScale) {
        geomRenderer.drawCircle(newCircle);
      }
      if(editingMode == editingModeNewHoopPos || editingMode == editingModeNewHoopScale || editingMode == editingModeNewHoopInnerRadius) {
        geomRenderer.drawHoop(newHoop);
      }
      if(editingMode == editingModeNewRectPos || editingMode == editingModeNewRectSize) {
        geomRenderer.drawRect(newRect);
      }
      if(editingMode == editingModeNewLineA || editingMode == editingModeNewLineB) {
        geomRenderer.drawLine(newLine);
      }
      if(editingMode == editingModeNewPolygon && shape.curves.size() > 0 && shape.curves.size() > 0) {

        if(shape.changed) {
          newPolygon.clear();
          for(int p=0; p<shape.curves[0].points.size(); p++) {
            newPolygon.vertices.push_back(shape.curves[0].points[p]);
          }

          if(shape.curves.size() > 1) {
            int numHoles = 0;
            for(int h=1; h<shape.curves.size(); h++) {
              if(shape.curves[h].size() >= 3) {
                newPolygon.holes.push_back(std::vector<Vec2d>());
                for(int p=0; p<shape.curves[h].points.size(); p++) {
                  newPolygon.holes[numHoles].push_back(shape.curves[h].points[p]);
                }
                numHoles++;
              }
            }
          }
          newPolygon.useCachedTriangles = false;
          newPolygon.triangulateWithHoles();
          newPolygon.useCachedTriangles = true;
          shape.changed = false;
        }

        geomRenderer.drawPolygon(newPolygon);


        if(debugTriangulation) {
          for(int i=0; i<newPolygon.triangles.size(); i++) {
            Triangle &t = newPolygon.triangles[i];
            if(isPointWithinTriangle(events.mw, t.a, t.b, t.c)) {
              dbgTriangleIndex = i;
              dbgTriangle = &newPolygon.triangles[i];
              geomRenderer.strokeType = 1;
              geomRenderer.strokeWidth = 6;
              geomRenderer.fillColor.set(0, 0, 0, 0);
              geomRenderer.strokeColor.set(1, 1, 1, 0.75);
              geomRenderer.drawTriangle(t);
            }
          }
        }

        for(int i=0; i<shape.curves.size(); i++) {
          for(int  p=0; p<shape.curves[i].size(); p++) {
            double r = 4;
            if(i == shape.activeCurveInd && p == shape.activePointInd) {
              geomRenderer.fillColor.set(0.5, 0.15, 0.15, 0.7);
              r = 6;
            }
            else {
              geomRenderer.fillColor.set(0, 0, 0, 0.7);
            }
            geomRenderer.drawCircle(r, shape.curves[i].points[p]);
          }
        }
        if(debugTriangulation) {
          for(int i=0; i<newPolygon.dbgHoleConnections.size(); i++) {
            double o = (double)i/(newPolygon.dbgHoleConnections.size()-1.0);
            geomRenderer.strokeType = 1;
            geomRenderer.strokeWidth = 6;
            geomRenderer.fillColor.set(0, 0, 0, 0);
            geomRenderer.strokeColor.set(o, o, o, 0.75);
            geomRenderer.drawLine(newPolygon.dbgHoleConnections[i]);
          }
        }
      }

      if(tmpEditingGeomObject) {
        sdlInterface->glmMatrixStack.pushMatrix();
        //glPushMatrix();
        Vec2d p = editingBody->getPosition();
        double a = editingBody->getAngle();
        sdlInterface->glmMatrixStack.translate(p.x, p.y, 0);
        sdlInterface->glmMatrixStack.rotate(a*180.0/PI, 0, 0, 1);
        //glTranslated(p.x, p.y, 0);
        //glRotatef(a*180.0/PI, 0, 0, 1);
        geomRenderer.draw(tmpEditingGeomObject);
        sdlInterface->glmMatrixStack.popMatrix();
        //glPopMatrix();
      }
      if(editingMode == editingModeNewPivotJointPos) {
        PivotJoint::render(geomRenderer, newConstraintPosA);
      }
      if(editingMode == editingModeNewGrooveJointPosA || editingMode == editingModeNewGrooveJointPosB) {
        GrooveJoint::render(geomRenderer, newConstraintPosA, newConstraintPosB);
      }
      if(editingMode == editingModeNewDampedSpringPosA || editingMode == editingModeNewDampedSpringPosB) {
        DampedSpring::render(geomRenderer, newConstraintPosA, newConstraintPosB);
      }
      if(editingConstraint && editingConstraintNearerThanBodyPart) {
        editingConstraint->render(geomRenderer, 1);
      }
      if(editingConstraint && lockEditingConstraint) {
        editingConstraint->render(geomRenderer, 2);
      }
    }

    if(nearestObjectDraggingMode == 0) {
      if(nearestObjectQuery.result != NearestObjectQuery::nothingFound) {
        if(nearestObjectQuery.distance < screenW) {
          //double d = clamp(1.0 - nearestObjectQuery.distance / screenW, 0.0, 1.0);
          geomRenderer.strokeColor.set(0, 0, 0, 0.9);
          if(lockedBodyPart)
            geomRenderer.fillColor.set(0.4, 0, 0.1, 1.0);
          else
            geomRenderer.fillColor.set(0, 0, 0, 1.0);
          geomRenderer.strokeType = 1;
          geomRenderer.strokeWidth = 1;
          Vec2d p = nearestObjectQuery.closestPoint;
          geomRenderer.drawLine(events.mw, p);
          geomRenderer.strokeWidth = 2;
          double r = 7.0/sqrt(camera.scale);
          geomRenderer.drawCircle(r, p);
        }
      }
    }
    else {
      Vec2d p = nearestObjectQuery.body->localToWorld(nearestObjectBindingPointLocal);
      double dist = distance(events.mw, p);
      //double d = clamp(1.0 - dist / screenW, 0.0, 1.0);
      geomRenderer.strokeColor.set(0, 0, 0, 0.9);
      geomRenderer.fillColor.set(0.5, 0.5, 0.5, 1.0);
      geomRenderer.strokeType = 1;
      geomRenderer.strokeWidth = 1;
      geomRenderer.drawLine(events.mw, p);
      geomRenderer.strokeWidth = 2;
      double r = 7.0/sqrt(camera.scale);
      geomRenderer.drawCircle(r, p);
    }


    camera.popMatrix();



    collisionCategorySelectionGUI.render(geomRenderer, events);

    if(editingBody) {
      geomRenderer.strokeColor.set(0, 0, 0, 0.5);
      geomRenderer.strokeWidth = 1;

      geomRenderer.drawLine(-75, 0, 75, 0, screenW-85, screenH-85);
      geomRenderer.drawLine(0, -75, 0, 75, screenW-85, screenH-85);

      geomRenderer.strokeColor.set(0.65, 0, 0, 0.6);
      geomRenderer.strokeWidth = 2;
      double a = editingBody->getAngle();
      geomRenderer.drawLine(0, 0, 65.0 * cos(a), 65.0 * sin(a), screenW-85, screenH-85);
      if(editingBodyB){
        geomRenderer.strokeColor.set(0.1, 0.1, 0.65, 0.6);
        a = editingBodyB->getAngle();
        geomRenderer.drawLine(0, 0, 65.0 * cos(a), 65.0 * sin(a), screenW-85, screenH-85);
      }
    }


    {
      Vec2d g = simulation.getGravity();
      double a = atan2(g.y, g.x) + PI*0.5 + camera.rotation;
      double s = map(g.length(), 0, maxGravity, 1, 8);
      sdlInterface->glmMatrixStack.pushMatrix();
      sdlInterface->glmMatrixStack.translate(screenW - 30, 30, 0);
      sdlInterface->glmMatrixStack.rotate(a * 180.0 / PI, 0, 0, 1);
      sdlInterface->glmMatrixStack.scale(s, s, 1);
      //glPushMatrix();
      //glTranslated(screenW - 30, 30, 0);
      //glRotated(a * 180.0 / PI, 0, 0, 1);
      //glScaled(s, s, 1);
      geomRenderer.fillColor.set(0.7, 0.7, 0.7, 0.4);
      geomRenderer.strokeType = 0;
      geomRenderer.drawPolygon(gravityArrow);
      sdlInterface->glmMatrixStack.popMatrix();
      //glPopMatrix();
    }

    /*texture3.unsetRenderTarget();
    shaderPost.activate();
    shaderPost.setUniform2f("screenSize", screenW, screenH);
    shaderPost.setUniform1f("time", time);
    texture3.activate(shaderPost, "tex", 0);
    quadScreen.render(screenW/2, screenH/2);
    shaderPost.deActivate();
    texture3.inactivate(0);*/



    wchar_t buf[256];
    double y = 10;
    std::swprintf(buf, 256, L"fps %.0f", round(timer.fps));
    y += textRenderer.print(buf, 10, y, 10, 1.0, sdlInterface).y;
    std::swprintf(buf, 256, L"scale %.4g", camera.scale);
    y += textRenderer.print(buf, 10, y, 10, 1.0, sdlInterface).y;

    if(debugTriangulation && dbgTriangle) {
      std::swprintf(buf, 256, L"triangle %d/%d", dbgTriangleIndex+1, (int)newPolygon.triangles.size());
      y += textRenderer.print(buf, 10, y, 10, 1.0, sdlInterface).y;
    }

    if(editingBody) {
      textRenderer.setColor(1, 1, 1, 0.2);
      Body *body = editingBody ? editingBody : lockedBodyPart->parentBody;
      y += textRenderer.print(body->getInfoW(), 10, y, 10, 1.0, sdlInterface).y;
      textRenderer.setColor(1, 1, 1, 1);
    }

    if(simulation.collisionGroups.size() > 0) {
      y += textRenderer.print(L"Collision groups, " + std::to_wstring(simulation.collisionGroups.size()), 10, y, 10, 1.0, sdlInterface).y;
      for(int i = 0; i < simulation.collisionGroups.size() && i < 10; i++) {
        std::swprintf(buf, 256, L"%d, %d", simulation.collisionGroups[i]->id, simulation.collisionGroups[i]->bodies.size());
        y += textRenderer.print(buf, 10, y, 10, 1.0, sdlInterface).y;
      }
    }

    if(showInputInterfaces) {
      for(auto &intInputInterfacePair : simulation.inputInterfaces) {
        std::swprintf(buf, 256, L"%d: %s", intInputInterfacePair.first, intInputInterfacePair.second->toString().c_str());
        y += textRenderer.print(buf, 10, y, 10, 1.0, sdlInterface).y;
      }
    }

    if(console.inputGrabbed) {
      console.render(sdlInterface);
    }


    /*for(Body &b : simulation.bodies) {
      Vec2d pos = b.getPosition();
      Vec2d v = b.getVelocity();
      std::swprintf(buf, 256, L"pos = (%.4f, %.4f)\nvel = (%.4f, %.4f)", pos.x, pos.y, v.x, v.y);
      y += text.print(buf, 10, y, 12, 1).y;
    }*/
    guiInputCapture = false;
  }




};
