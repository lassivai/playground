#pragma once
#include <SDL2/SDL.h>
#include "../geom_gfx.h"
#include "../glwrap.h"
#include "../textgl.h"
#include "../event.h"
#include "../mathl.h"
#include <vector>
#include <string>


/** TODO
  * - onUpdate
  * - automatic bounding box for each element
  * - pre-rendering of text and other elements hierarchically
  * - force text color
  * - tighter fitting text boxes
  * - restrict text box input to fit into the field
  * - nicer check box
  * - more general solution for text rendering
  * - concider the paradigm of making stuff invisible and visible instead of removing and adding them
  *
  * - gui element value type with a template
  * - minimize/maximize panels to title bar size
  *
  */
  

  
static double _a = 0, _b = 0, _c = 1, _d = 1;
static bool _rel = true;
static GlShader boxShadowShader;
static bool boxShadowShaderInitialized = false;

static void initBoxShadowShader(bool reload = false) {
  if(!boxShadowShaderInitialized || reload) {
    boxShadowShader.create("data/glsl/texture.vert", "data/glsl/boxshadow.frag");
    boxShadowShaderInitialized = true;
  }
}

static Texture *createBoxShadowTexture(GeomRenderer &geomRenderer, int w, int h, int shadowWidth = 10, double shadowAlpha = 0.3, double shadowPower = 2.0) {
  initBoxShadowShader();
  int shadowTextureWidth = w + shadowWidth*2, shadowTextureHeight = h + shadowWidth*2;
  Texture *boxShadowTexture = new Texture(shadowTextureWidth, shadowTextureHeight);
  //boxShadowTexture->clear(0, 0, 0, 0);
  
  Quadx quad(shadowTextureWidth, shadowTextureHeight);
  
  Vec2d d(0, max(shadowTextureHeight, geomRenderer.screenH) - shadowTextureHeight);
  
  boxShadowShader.activate();
  boxShadowShader.setUniform2i("boxSize", w, h);
  boxShadowShader.setUniform1i("shadowWidth", shadowWidth);
  boxShadowShader.setUniform1f("shadowAlpha", shadowAlpha);
  boxShadowShader.setUniform1f("shadowPower", shadowPower);
  //boxShadowShader.setUniform4d("backgroundColor", backgroundColor);
  //boxShadowShader.setUniform2i("displacement", displacementX, displacementY);
  //boxShadowShader.setUniform1i("onlyExterior", onlyExterior);
  
  //boxShadowTexture.activate(boxShadowShader, "tex", 0);
  boxShadowTexture->setRenderTarget();
  //glViewport(0, 0, 1400, 1000);
  glLoadIdentity();
  //clear(0, 0, 0, 1);
  quad.render(d.x+shadowTextureWidth*0.5, d.y+shadowTextureHeight*0.5);
  //if(rememberIterations && !previewActive) currentPass++;
  //mandelbrotShader.setUniform1i("pass", rememberIterations && !previewActive ? currentPass : 0);
  //quad.render();
  boxShadowTexture->unsetRenderTarget();
  
  boxShadowShader.deActivate();
  //boxShadowTexture.inactivate(0);
  
  return boxShadowTexture;
}



static GlShader spriteShadowShader;
static bool spriteShadowShaderInitialized = false;

static void initSpriteShadowShader(bool reload = false) {
  if(!spriteShadowShaderInitialized || reload) {
    spriteShadowShader.create("data/glsl/texture.vert", "data/glsl/spriteshadow.frag");
    spriteShadowShaderInitialized = true;
  }
}

static Texture *createSpriteShadowTexture(GeomRenderer &geomRenderer, Texture &spriteTexture, int shadowWidth = 15, double shadowPower = 7.0, double shadowFactor = 2.0, const Vec4d &shadowColor = Vec4d(0, 0, 0, 1), bool inverseTextureCoordinates = false) {
  initSpriteShadowShader();
  int shadowTextureWidth = spriteTexture.w + shadowWidth*2, shadowTextureHeight = spriteTexture.h + shadowWidth*2;
  Texture *spriteShadowTexture = new Texture(shadowTextureWidth, shadowTextureHeight);
  //boxShadowTexture->clear(0, 0, 0, 0);
  
  Quadx quad(shadowTextureWidth, shadowTextureHeight);
  
  Vec2d d(0, max(shadowTextureHeight, geomRenderer.screenH) - shadowTextureHeight);
  
  spriteShadowShader.activate();
  spriteTexture.activate(spriteShadowShader, "sprite", 0);
  spriteShadowShader.setUniform2i("spriteSize", spriteTexture.w, spriteTexture.h);
  spriteShadowShader.setUniform1i("shadowWidth", shadowWidth);
  spriteShadowShader.setUniform1i("inverseTextureCoordinates", inverseTextureCoordinates);
  spriteShadowShader.setUniform1f("shadowPower", shadowPower);
  spriteShadowShader.setUniform1f("shadowFactor", shadowFactor);
  spriteShadowShader.setUniform4f("shadowColor", shadowColor);

  spriteShadowTexture->setRenderTarget();

  glLoadIdentity();

  quad.render(d.x+shadowTextureWidth*0.5, d.y+shadowTextureHeight*0.5);

  spriteShadowTexture->unsetRenderTarget();
  spriteTexture.inactivate(0);
  spriteShadowShader.deActivate();

  
  return spriteShadowTexture;
}


static Texture *createLabelTexture(GeomRenderer &geomRenderer, TextGl &textRenderer, const std::string &text, int textSize, const Vec4d &textColor = Vec4d(1, 1, 1, 1)) {
  
  Vec2d dim = textRenderer.getDimensions(text, textSize);
  textRenderer.setColor(textColor);
  
  Texture *labelTexture = new Texture(dim.x, dim.y);
  
  Vec2d d(0, max(dim.y, geomRenderer.screenH) - dim.y);
  
  labelTexture->setRenderTarget();
  clear(1, 1, 1, 0);
  glLoadIdentity();
  textRenderer.print(text, d.x, d.y, textSize);
  labelTexture->unsetRenderTarget();
  
  return labelTexture;
}



struct GuiElement;

struct GuiEventListener {
  virtual ~GuiEventListener() {}
  virtual void onMessage(GuiElement *guiElement, const std::string &message) {}
  virtual void onMessage(GuiElement *guiElement, const std::vector<std::string> &message) {}
  virtual void onMessage(GuiElement *guiElement, const std::string &message, void *userData) {}
  virtual void onValueChange(GuiElement *guiElement) {}
  virtual void onAction(GuiElement *guiElement) {}
  virtual void onKeyUp(GuiElement *guiElement, Events &events) {}
  virtual void onKeyDown(GuiElement *guiElement, Events &events) {}
  virtual void onMouseWheel(GuiElement *guiElement, Events &events) {}
  virtual void onMousePressed(GuiElement *guiElement, Events &events) {}
  virtual void onMouseReleased(GuiElement *guiElement, Events &events) {}
  virtual void onMouseMotion(GuiElement *guiElement, Events &events) {}
  virtual void onMouseArrived(GuiElement *guiElement, Events &events) {}
  virtual void onMouseDeparted(GuiElement *guiElement, Events &events) {}
  virtual void onTextInput(GuiElement *guiElement, Events &events) {}
};




struct GuiElement
{
  Texture *shadowTexture = NULL;
  bool drawShadow = false;
  int shadowWidth = 10;
  Vec2d shadowDisplacement = Vec2d(3, 3);
  Vec2d shadowPosition;
  
  Texture prerenderedGuiElement;
  bool prerenderingNeeded = true;

  std::string name;
  Vec2d pos, absolutePos;
  double zLayer = 0, absoluteZLayer = 0;
  Vec2d size;
  bool drawBackground = false, drawBorder = false;
  Vec4d backgroundColor;
  Vec4d borderColor;
  double borderWidth = 1;

  std::vector<GuiElement*> children;
  GuiElement *parent = NULL;

  std::vector<GuiEventListener*> guiEventListeners;

  bool isInputGrabbed = false;
  bool isInputGrabbedInChildren = false;
  bool isInputGrabbedAnywhere = false;
  bool isMouseHover = false;

  bool isVisible = true;

  Vec4d overlayColor;
  
  // TODO different sorts of fade in/fade out effects utilizing these timers
  TicToc mouseArrivedTicToc, mouseDepartedTicToc, mousePressedTicToc, mouseReleasedTicToc;
  
  GuiElement *guiElementCapturingMouse = NULL;
  int mousePosX = 0, mousePosY = 0;
  
  bool mouseCapturedForChildren = false;
  bool mouseHidden = false;
  
  bool closeRequested = false;
  
  int readyToReceiveEvents = false;
  
  
  bool captureMouseForChildren() {
    GuiElement *root = this;
    while(root->parent) {
      root = root->parent;
    }
    if(root->guiElementCapturingMouse) {
      root->guiElementCapturingMouse->releaseMouse();
    }
    //if(root->guiElementCapturingMouse == NULL) {
      root->guiElementCapturingMouse = guiElementCapturingMouse = this;
      mouseCapturedForChildren = true;
      //applyToChildren([](GuiElement *parent, GuiElement *child) { child->mouseCapturedForChildren = true; });
      
      //root->deactivateMouseInput();
      
      return true;
    //}
    //return false;
  }
  void deactivateMouseInput() {
    if(mouseCapturedForChildren) return;
    
    isInputGrabbed = false;
    isMouseHover = false;
    
    for(int i=0; i<children.size(); i++) {
      children[i]->deactivateMouseInput();
    }
  }
  
  
  bool captureAndHideMouse() {
    GuiElement *root = this;
    while(root->parent) {
      root = root->parent;
    }
    if(root->guiElementCapturingMouse == NULL) {
      root->guiElementCapturingMouse = guiElementCapturingMouse = this;
      mouseHidden = true;
      SDL_GetGlobalMouseState(&mousePosX, &mousePosY);
      SDL_SetRelativeMouseMode(SDL_TRUE);
      return true;
    }
    return false;
  }
  
  bool releaseMouse() {
    if(guiElementCapturingMouse) {
      GuiElement *root = this;
      while(root->parent) {
        root = root->parent;
      }
      if(root->guiElementCapturingMouse == this) {
        if(mouseHidden) {
          SDL_SetRelativeMouseMode(SDL_FALSE);
          SDL_WarpMouseGlobal(mousePosX, mousePosY);
          mouseHidden = false;
        }
        if(mouseCapturedForChildren) {
          //applyToChildren([](GuiElement *parent, GuiElement *child) { child->mouseCapturedForChildren = false; });
          mouseCapturedForChildren = false;
        }
        root->guiElementCapturingMouse = guiElementCapturingMouse = NULL;
        
        return true;
      }
    }
    return false;
  }

  GuiElement(std::string name) {
    this->name = name;
    backgroundColor.set(0, 0, 0, 0.85);
    borderColor.set(0.5, 0.5, 0.5, 0.5);
    overlayColor.set(1, 1, 1, 1);
    //printf("Gui element '%s', %lu created\n", name.c_str(), (long)this);
  }

  GuiElement() {
    GuiElement("unnamed");
    overlayColor.set(1, 1, 1, 1);
  }

  virtual ~GuiElement() {
    //printf("Gui element '%s', %lu removed\n", name.c_str(), (long)this);
  }
  
  bool blockParentInputOnMouseHover = false;
  
  bool isInputBlockedInChildren() {
    for(int i=0; i<children.size(); i++) {
      if(children[i]->isVisible) {
        if(children[i]->isInputGrabbed) {
          return true;
        }
        if(children[i]->blockParentInputOnMouseHover && children[i]->isMouseHover) {
          return true;
        }
        if(children[i]->isInputBlockedInChildren()) {
          return true;
        }
      }
    }
    
    return false;
  }

  virtual void disableInputGrab() {
    for(int i=0; i<children.size(); i++) {
      children[i]->disableInputGrab();
    }
    isInputGrabbed = false;
    isInputGrabbedAnywhere = false;
    isInputGrabbedInChildren = false;
  }

  virtual void setInputGrabbed(bool isInputGrabbed) {
    if(this->isInputGrabbed != isInputGrabbed) {
      this->isInputGrabbed = isInputGrabbed;
      if(!isInputGrabbed) {
        disableInputGrab();
      }
      prerenderingNeeded = true;
    }
  }

  void update(double time, double dt) {
    onUpdate(time, dt);
    for(int i=0; i<children.size(); i++) {
      children[i]->update(time, dt);
    }
  }
  virtual void onUpdate(double time, double dt) {}

  void setSize(int w, int h) {
    this->size.set(w, h);
    prerenderingNeeded = true;
    onSetSize();
  }
  inline void setSize(const Vec2d &size) {
    setSize(size.x, size.y);
  }

  virtual void onSetSize() {}

  virtual void setPosition(const Vec2d &pos) {
    this->pos = pos;
  }
  virtual void setPosition(double x, double y) {
    this->pos.set(x, y);
  }

  virtual void setVisible(bool isVisible) {
    if(isVisible && !this->isVisible) {
      prerenderingNeeded = true;
    }
    if(this->isVisible != isVisible) {
      this->isVisible = isVisible;
      onVisibilityChanged(isVisible);
    }
    if(isInputGrabbed && !isVisible) {
      isInputGrabbed = false;
    }
  }

  virtual void toggleVisibility() {
    isVisible = !isVisible;
    if(isVisible) {
      prerenderingNeeded = true;
    }
    if(isInputGrabbed && !isVisible) {
      isInputGrabbed = false;
    }
    onVisibilityChanged(isVisible);
  }
  
  /*
  virtual void setVisible(bool isVisible) {

    if(this->isVisible != isVisible) {
      if(isVisible) {
        prerenderingNeeded = true;
      }
      //onVisibilityChanged(isVisible);
    }
    
    this->isVisible = isVisible;
    
    if(isInputGrabbed && !isVisible) {
      isInputGrabbed = false;
    }
    
    for(int i=0; i<children.size(); i++) {
      children[i]->setVisible(isVisible);
    }
  }*/
  virtual void onVisibilityChanged(bool toVisible) {}

  /*virtual void toggleVisibility() {
    isVisible = !isVisible;
    
    if(isVisible) {
      prerenderingNeeded = true;
    }
    //onVisibilityChanged(isVisible);
  }*/
  

  virtual void addChildElement(GuiElement *guiElement) {
    children.push_back(guiElement);
    guiElement->parent = this;
  }

  virtual void deleteChildElement(GuiElement *guiElement) {
    for(int i=0; i<children.size(); i++) {
      if(children[i] == guiElement) {
        guiElement->finalize();
        delete guiElement;
        children.erase(children.begin() + i);
        i--;
      }
    }
  }
  
  virtual bool hasChildElement(GuiElement *guiElement) {
    for(int i=0; i<children.size(); i++) {
      if(children[i] == guiElement) {
        return true;
      }
    }
    return false;
  }

  void print(int level = 0) {
    for(int i=0; i<level; i++) {
      printf("  ");
    }
    printf("'%s'\n", name.c_str());
    if(children.size() > 0) {
      for(int i=0; i<children.size(); i++) {
        children[i]->print(level + 1);
      }
    }
  }

  virtual void deleteChildElements() {
    for(int i=0; i<children.size(); i++) {
      children[i]->finalize();
      delete children[i];
    }
    children.clear();
  }
  
  virtual void clearChildElements() {
    for(int i=0; i<children.size(); i++) {
      children[i]->parent = NULL;
    }
    children.clear();
  }

  void finalize() {
    //if(parent) printf("(debugging) at GuiElement.finalize, name = %s, parent = %s\n", name.c_str(), parent->name.c_str());
    //else printf("(debugging) at GuiElement.finalize, name = %s\n", name.c_str());
    for(GuiElement *childGuiElement : children) {
      childGuiElement->finalize();
      delete childGuiElement;
    }
    children.clear();

    /*for(GuiEventListener *guiEventListener : guiEventListeners) {
      delete guiEventListener;
    }*/
    guiEventListeners.clear();
  }

  bool checkInputGrab() {
    if(!isVisible) return false;
    
    isInputGrabbedAnywhere = false;
    isInputGrabbedInChildren = false;
    
    for(GuiElement *childGuiElement : children) {
      if(childGuiElement->checkInputGrab()) {
        isInputGrabbedInChildren = true;
        //return true;
      }
    }
    
    isInputGrabbedAnywhere = isInputGrabbed || isInputGrabbedInChildren;
    
    return isInputGrabbedAnywhere;
  }

  void addGuiEventListener(GuiEventListener *guiEventListener) {
    guiEventListeners.push_back(guiEventListener);
  }

  void deleteGuiEventListener(int index) {
    if(index >= 0 && index < guiEventListeners.size()) {
      //delete guiEventListeners[index];
      guiEventListeners.erase(guiEventListeners.begin() + index);
    }
  }

  void onValueChange(GuiElement *guiElement) {
    if(!isVisible) return;
    
    //printf("onValueChange %s\n", name.c_str());
    
    for(GuiEventListener *guiEventListener : guiEventListeners) {
      guiEventListener->onValueChange(guiElement);
    }
    if(parent) {
      parent->onValueChange(guiElement);
    }
    prerenderingNeeded = true;
  }
  void onAction() {
    onAction(this);
  }
  
  void onAction(GuiElement *guiElement) {
    if(!isVisible) return;

    for(GuiEventListener *guiEventListener : guiEventListeners) {
      guiEventListener->onAction(guiElement);
    }
    if(parent) {
      parent->onAction(guiElement);
    }
    prerenderingNeeded = true;
  }
  
  template<class T>
  void getValue(T &value) {
    getTypedValue(value);
  }
  virtual void getTypedValue(double &value) {  }
  virtual void getTypedValue(bool &value) {  }
  virtual void getTypedValue(int &value) {  }
  virtual void getTypedValue(Vec4d &value) {  }
  virtual void getTypedValue(std::string &value) {  }
  

  virtual void getValue(void *value) { printf("Error GuiElement::getValue(void*) not implemented! name: '%s'", name.c_str()); }

  virtual void setValue(std::string value) {}

  GuiElement *getRoot() {
    GuiElement *root = this;
    while(root->parent) {
      root = root->parent;
    }
    return root;
  }

  bool doesThisApplyToAnyOtherElement(const std::function<bool(GuiElement *a, GuiElement *b)> &conditionFunction) {
    GuiElement *root = this;
    while(root->parent) {
      root = root->parent;
    }
    return root->doesThisApplyToAnyOtherElement(conditionFunction, this);
  }
  
  void applyToChildren(const std::function<void(GuiElement *parent, GuiElement *child)> &applyFunction, GuiElement *invoker = NULL) {
    if(!invoker) invoker = this;
    for(int i=0; i<children.size(); i++) {
      applyFunction(invoker, children[i]);
      children[i]->applyToChildren(applyFunction, invoker);
    }
  }
  
private:
  
  bool doesThisApplyToAnyOtherElement(const std::function<bool(GuiElement *a, GuiElement *b)> &conditionFunction, GuiElement *enquiringElement) {
    for(int i=0; i<children.size(); i++) {
      if(children[i] != enquiringElement && conditionFunction(enquiringElement, children[i])) {
        return true;
      }
      if(children[i]->doesThisApplyToAnyOtherElement(conditionFunction, enquiringElement)) {
        return true;
      }
    }
    return false;
  }
  
public:
  

  // FIXME this should be called only when things change
  virtual void prepare(GeomRenderer &geomRenderer, TextGl &textRenderer) {
    absolutePos = parent ? pos + parent->absolutePos : pos;
    absoluteZLayer = parent ? zLayer + parent->absoluteZLayer : zLayer;
    //printf("%f, %f\n", pos.x, absolutePos.x);
    
    onPrepare(geomRenderer, textRenderer);

    onPrepareShadowTexture(geomRenderer);

    prerender(geomRenderer, textRenderer);
    
    for(GuiElement *childGuiElement : children) {
      childGuiElement->prepare(geomRenderer, textRenderer);
    }
    
    readyToReceiveEvents = true;
  }
  virtual void onPrepare(GeomRenderer &geomRenderer, TextGl &textRenderer) {}

  virtual void prerender(GeomRenderer &geomRenderer, TextGl &textRenderer) {
    //if(!isVisible) return;

    if(isVisible && prerenderingNeeded && size.x > 0 && size.y > 0) {

      if(prerenderedGuiElement.w != size.x || prerenderedGuiElement.h != size.y) {
        prerenderedGuiElement.createRenderTarget(size.x, size.y);
        prerenderedGuiElement.enableFiltering(false);
      }

      prerenderedGuiElement.setRenderTarget();
      
      // TODO put this stuff into Texture::setRenderTarget()
      double w = max(size.x, geomRenderer.screenW);
      double h = max(size.y, geomRenderer.screenH);
      glViewport(0, 0, w, h);
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      glOrtho(0, w, h, 0, -100, 100);
      glMatrixMode(GL_MODELVIEW);
      clear(1, 1, 1, 0);
      Vec2d d(0, max(size.y, geomRenderer.screenH) - prerenderedGuiElement.h);
      
      //geomRenderer.setZLayer(absoluteZLayer);

      if(drawBackground || drawBorder) {
        geomRenderer.texture = NULL;
        geomRenderer.fillColor = drawBackground ? backgroundColor : Vec4d::Zero;
        geomRenderer.strokeColor = borderColor;
        geomRenderer.strokeType = drawBorder ? 1 : 0;
        geomRenderer.strokeWidth = borderWidth;
        geomRenderer.drawRect(size, d+size*0.5);
      }

      onPrerender(geomRenderer, textRenderer);
      onPrerender(geomRenderer, textRenderer, d);

      //geomRenderer.resetZLayer();
      
      prerenderedGuiElement.unsetRenderTarget();
      
      // TODO put this stuff into Texture::unsetRenderTarget()
      glViewport(0, 0, geomRenderer.screenW, geomRenderer.screenH);
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      glOrtho(0, geomRenderer.screenW, geomRenderer.screenH, 0, -100, 100);
      glMatrixMode(GL_MODELVIEW);

      prerenderingNeeded = false;
    }

    /*for(GuiElement *childGuiElement : children) {
      childGuiElement->prerender(geomRenderer, textRenderer);
    }*/
  }

  virtual void onPrerender(GeomRenderer &geomRenderer, TextGl &textRenderer) {}

  virtual void onPrerender(GeomRenderer &geomRenderer, TextGl &textRenderer, const Vec2d &displacement) {}

  virtual void onPrepareShadowTexture(GeomRenderer &geomRenderer) {}
  
  // not the fastest to excecute single shadow stretched for all boxes case yet, bu this is probably fast enough for now, it only needs to be
  // rendered once per each gui element at initialization, so not too critical

  virtual void renderShadow() {
    if(!isVisible) return;
    if(drawShadow && shadowTexture) {
      shadowTexture->render(absolutePos+shadowPosition+shadowDisplacement-Vec2d(shadowWidth, shadowWidth));
    }
  }


  virtual void render(GeomRenderer &geomRenderer, TextGl &textRenderer, double currentZLayer, double &nextZLayer) {
    if(!isVisible) return;
    if(absoluteZLayer > currentZLayer) {
      if(currentZLayer == nextZLayer) {
        nextZLayer = absoluteZLayer;
      }
      else {
        nextZLayer = min(nextZLayer, absoluteZLayer);
      }
    }
    if(absoluteZLayer == currentZLayer) {
      if(prerenderedGuiElement.w > 0 && prerenderedGuiElement.h > 0) {
        prerenderedGuiElement.render(absolutePos+size*0.5, overlayColor);
      }
      onRender(geomRenderer, textRenderer);
    }
    for(GuiElement *childGuiElement : children) {
      if(childGuiElement->absoluteZLayer == currentZLayer) {
        childGuiElement->renderShadow();
      }
    }
    
    for(GuiElement *childGuiElement : children) {
      childGuiElement->render(geomRenderer, textRenderer, currentZLayer, nextZLayer);
    }
  }


  
  
  
  virtual void onRender(GeomRenderer &geomRenderer, TextGl &textRenderer) {}
  

  virtual bool isPointWithin(const Vec2d &p) {
    return isVisible && p.x >= absolutePos.x && p.x <= absolutePos.x + size.x && p.y >= absolutePos.y && p.y <= absolutePos.y + size.y;
  }
  bool isPointWithinChildElements(const Vec2d &p) {
    if(!isVisible) return false;

    for(GuiElement *child : children) {
      if(child->isPointWithin(p)) return true;
      if(child->isPointWithinChildElements(p)) return true;
    }
    return false;
  }

  virtual void onKeyUp(Events &events) {
    if(!isVisible || !readyToReceiveEvents) return;

    for(int i=0; i<children.size(); i++) {
      children[i]->onKeyUp(events);
    }
    if(isInputGrabbed) {
      onKeyUpNotify(events, this);
    }
  }
  virtual void onKeyDown(Events &events) {
    if(!isVisible || !readyToReceiveEvents) return;

    for(int i=0; i<children.size(); i++) {
      children[i]->onKeyDown(events);
    }
    if(isInputGrabbed) {
      onKeyDownNotify(events, this);
    }
  }
  virtual void onMouseWheel(Events &events) {
    if(!isVisible || !readyToReceiveEvents) return;
    
    if(guiElementCapturingMouse && guiElementCapturingMouse != this) {
      guiElementCapturingMouse->onMouseWheel(events);
      return;
    }
    if(!guiElementCapturingMouse || mouseCapturedForChildren) {
      for(int i=0; i<children.size(); i++) {
        children[i]->onMouseWheel(events);
      }
    }
    //if(isInputGrabbed) {
    //if(isPointWithin(events.m)) {
      onMouseWheelNotify(events, this);
    //}
  }
  virtual void onMousePressed(Events &events) {
    if(!isVisible || !readyToReceiveEvents) return;
    
    if(guiElementCapturingMouse && guiElementCapturingMouse != this) {
      guiElementCapturingMouse->onMousePressed(events);
      return;
    }
    if(!guiElementCapturingMouse || mouseCapturedForChildren) {
      for(int i=0; i<children.size(); i++) {
        children[i]->onMousePressed(events);
      }
    }
    
    //if(isInputGrabbed) {
    if(isPointWithin(events.m)) {
      mousePressedTicToc.tic();
    }
    // FIXME
    onMousePressedNotify(events, this);
  }
  virtual void onMouseReleased(Events &events) {
    if(!isVisible || !readyToReceiveEvents) return;

    if(guiElementCapturingMouse && guiElementCapturingMouse != this) {
      guiElementCapturingMouse->onMouseReleased(events);
      return;
    }
    if(!guiElementCapturingMouse || mouseCapturedForChildren) {
      for(int i=0; i<children.size(); i++) {
        children[i]->onMouseReleased(events);
      }
    }
    
    if(isInputGrabbed) {
      mouseReleasedTicToc.tic();
    }
    //if(isPointWithin(events.m)) {
    
      onMouseReleasedNotify(events, this);
    //}
  }
  virtual void onMouseMotion(Events &events) {
    if(!isVisible || !readyToReceiveEvents) return;

    if(guiElementCapturingMouse && guiElementCapturingMouse != this) {
      guiElementCapturingMouse->onMouseMotion(events);
      return;
    }
    if(!guiElementCapturingMouse || mouseCapturedForChildren) {
      for(int i=0; i<children.size(); i++) {
        children[i]->onMouseMotion(events);
      }
    }
    //if(isInputGrabbed) {
    if(isPointWithin(events.m)) {
      if(!isMouseHover) {
        mouseArrivedTicToc.tic();
        onMouseArrivedNotify(events, this);
      }
      isMouseHover = true;
      //onMouseMotionNotify(events, this);
    }
    else {
      if(isMouseHover) {
        mouseDepartedTicToc.tic();
        onMouseDepartedNotify(events, this);
      }
      isMouseHover = false;
    }
    onMouseMotionNotify(events, this);
  }
  virtual void onTextInput(Events &events) {
    if(!isVisible || !readyToReceiveEvents) return;

    for(int i=0; i<children.size(); i++) {
      children[i]->onTextInput(events);
    }
    if(isInputGrabbed) {
      onTextInputNotify(events, this);
    }
  }



  // TODO Find out simpler way
  void onKeyUpNotify(Events &events, GuiElement *guiElement = NULL) {
    if(guiElement == NULL) guiElement = this;
    for(GuiEventListener *guiEventListener : guiEventListeners) {
      guiEventListener->onKeyUp(guiElement, events);
    }
    if(parent) {
      parent->onKeyUpNotify(events, guiElement);
    }
  }

  void onKeyDownNotify(Events &events, GuiElement *guiElement = NULL) {
    if(guiElement == NULL) guiElement = this;
    for(GuiEventListener *guiEventListener : guiEventListeners) {
      guiEventListener->onKeyDown(guiElement, events);
    }
    if(parent) {
      parent->onKeyDownNotify(events, guiElement);
    }
  }
  void onMouseWheelNotify(Events &events, GuiElement *guiElement = NULL) {
    if(guiElement == NULL) guiElement = this;
    for(GuiEventListener *guiEventListener : guiEventListeners) {
      guiEventListener->onMouseWheel(guiElement, events);
    }
    if(parent) {
      parent->onMouseWheelNotify(events, guiElement);
    }
  }
  void onMousePressedNotify(Events &events, GuiElement *guiElement = NULL) {
    if(guiElement == NULL) guiElement = this;
    for(GuiEventListener *guiEventListener : guiEventListeners) {
      guiEventListener->onMousePressed(guiElement, events);
    }
    if(parent) {
      parent->onMousePressedNotify(events, guiElement);
    }
  }
  void onMouseReleasedNotify(Events &events, GuiElement *guiElement = NULL) {
    if(guiElement == NULL) guiElement = this;
    for(GuiEventListener *guiEventListener : guiEventListeners) {
      guiEventListener->onMouseReleased(guiElement, events);
    }
    if(parent) {
      parent->onMouseReleasedNotify(events, guiElement);
    }
  }
  void onMouseMotionNotify(Events &events, GuiElement *guiElement = NULL) {
    if(guiElement == NULL) guiElement = this;
    for(GuiEventListener *guiEventListener : guiEventListeners) {
      guiEventListener->onMouseMotion(guiElement, events);
    }
    if(parent) {
      parent->onMouseMotionNotify(events, guiElement);
    }
  }
  void onMouseArrivedNotify(Events &events, GuiElement *guiElement = NULL) {
    if(guiElement == NULL) guiElement = this;
    for(GuiEventListener *guiEventListener : guiEventListeners) {
      guiEventListener->onMouseArrived(guiElement, events);
    }
    if(parent) {
      parent->onMouseArrivedNotify(events, guiElement);
    }
  }
  void onMouseDepartedNotify(Events &events, GuiElement *guiElement = NULL) {
    if(guiElement == NULL) guiElement = this;
    for(GuiEventListener *guiEventListener : guiEventListeners) {
      guiEventListener->onMouseDeparted(guiElement, events);
    }
    if(parent) {
      parent->onMouseDepartedNotify(events, guiElement);
    }
  }
  
  
  void onTextInputNotify(Events &events, GuiElement *guiElement = NULL) {
    if(guiElement == NULL) guiElement = this;
    for(GuiEventListener *guiEventListener : guiEventListeners) {
      guiEventListener->onTextInput(guiElement, events);
    }
    if(parent) {
      parent->onTextInputNotify(events, guiElement);
    }
  }
  
  /*void onValueChangeNotify(Events &events, GuiElement *guiElement = NULL) {
    if(guiElement == NULL) guiElement = this;
    for(GuiEventListener *guiEventListener : guiEventListeners) {
      guiEventListener->onValueChange(guiElement, events);
    }
    if(parent) {
      parent->onValueChangeNotify(events, guiElement);
    }
  }
  void onActionNotify(Events &events, GuiElement *guiElement = NULL) {
    if(guiElement == NULL) guiElement = this;
    for(GuiEventListener *guiEventListener : guiEventListeners) {
      guiEventListener->onAction(guiElement, events);
    }
    if(parent) {
      parent->onActionNotify(events, guiElement);
    }
  }*/

  virtual void sendMessage(const std::string &message, void *userData, GuiElement *guiElement = NULL) {
    if(guiElement == NULL) guiElement = this;
    for(GuiEventListener *guiEventListener : guiEventListeners) {
      guiEventListener->onMessage(guiElement, message, userData);
    }
    if(parent) {
      parent->sendMessage(message, userData, guiElement);
    }
  }

  virtual void sendMessage(const std::string &message, GuiElement *guiElement = NULL) {
    if(guiElement == NULL) guiElement = this;
    for(GuiEventListener *guiEventListener : guiEventListeners) {
      guiEventListener->onMessage(guiElement, message);
    }
    if(parent) {
      parent->sendMessage(message, guiElement);
    }
  }
  
  virtual void sendMessage(const std::vector<std::string> &message, GuiElement *guiElement = NULL) {
    if(guiElement == NULL) guiElement = this;
    for(GuiEventListener *guiEventListener : guiEventListeners) {
      guiEventListener->onMessage(guiElement, message);
    }
    if(parent) {
      parent->sendMessage(message, guiElement);
    }
  }


  /*void gatherChildElementsSortedByAscendingZLayer() {
    std::vector<GuiElement*> allChildren;
    gatherChildElementsByZLayer(allChildren);
  }*/
  
  std::vector<GuiElement*> sortChildrenByZLayer() {
    std::vector<GuiElement*> sorted;
    
    for(int i=0; i<children.size(); i++) {
      bool childAdded = false;
      for(int k=0; k<sorted.size(); k++) {
        if(sorted[k]->absoluteZLayer > children[i]->absoluteZLayer) {
          sorted.insert(sorted.begin()+k, children[i]);
          childAdded = true;
          break;
        }
      }
      if(!childAdded) {
        sorted.push_back(children[i]);
      }
    }
    /*printf("%s children sorted by z layer:\n", name.c_str());
    for(int i=0; i<sorted.size(); i++) {
      printf("  %d. %s %f\n", i+1, sorted[i]->name.c_str(), sorted[i]->absoluteZLayer);
    }*/
    return sorted;
  }
  /*void gatherChildElementsByZLayer(std::vector<GuiElement*> &allChildren) {
    for(int i=0; i<children.size(); i++) {
      bool childAdded = false;
      for(int k=0; k<allChildren.size(); k++) {
        if(allChildren[k]->absoluteZLayer > children[i]->absoluteZLayer) {
          allChildren.insert(allChildren.begin()+k, children[i]);
          childAdded = true;
          break;
        }
      }
      if(!childAdded) {
        allChildren.push_back(children[i]);
      }
      children[i]->gatherChildElementsByZLayer(allChildren);
    }
  }*/


};

struct LayoutPlacer {
  virtual ~LayoutPlacer() {}
  virtual void setPosition(GuiElement *guiElement) {}
};

struct RowColumnPlacer : public LayoutPlacer {

  static const int Left = 0, Middle = 1, Right = 2, Top = 3, Bottom = 4;
  int horizontalAlignment = Left;
  int verticalAlignment = Middle;
  
  double progressDeltaX = 0;
  double alignmentDeltaX = 0;
  double alignmentDeltaY = 0;
  
  virtual ~RowColumnPlacer() {}
  
  void setPosition(GuiElement *guiElement) override {
    double x, y;
    
    if(horizontalAlignment == Left) {
      x = getX();
    }
    else if(horizontalAlignment == Middle) {
      x = getX() - guiElement->size.x*0.5;
    }
    if(horizontalAlignment == Right) {
      x = getX() - guiElement->size.x;
    }
    
    if(verticalAlignment == Top) {
      y = getY();
    }
    else if(verticalAlignment == Middle) {
      y = getY() + knobSize*0.5 - guiElement->size.y*0.5;
    }
    else if(verticalAlignment == Bottom) {
      y = getY() + knobSize - guiElement->size.y;
    }
    
    progressX(guiElement->size.x + progressDeltaX);
    x += alignmentDeltaX;
    y += alignmentDeltaY;
    
    guiElement->setPosition(x, y);
  }
  
  inline void setDeltas(double alignmentDeltaX = 0, double alignmentDeltaY = 0) {
    this->alignmentDeltaX = alignmentDeltaX;
    this->alignmentDeltaY = alignmentDeltaY;
  }
  

  RowColumnPlacer &alignLeftTop(double alignmentDeltaX = 0, double alignmentDeltaY = 0) {
    setDeltas(alignmentDeltaX, alignmentDeltaY);
    horizontalAlignment = Left;
    verticalAlignment = Top;
    return *this;
  }
  RowColumnPlacer &alignLeftMiddle(double alignmentDeltaX = 0, double alignmentDeltaY = 0) {
    setDeltas(alignmentDeltaX, alignmentDeltaY);
    horizontalAlignment = Left;
    verticalAlignment = Middle;
    return *this;
  }
  RowColumnPlacer &alignLeftBottom(double alignmentDeltaX = 0, double alignmentDeltaY = 0) {
    setDeltas(alignmentDeltaX, alignmentDeltaY);
    horizontalAlignment = Left;
    verticalAlignment = Bottom;
    return *this;
  }
  RowColumnPlacer &alignRightTop(double alignmentDeltaX = 0, double alignmentDeltaY = 0) {
    setDeltas(alignmentDeltaX, alignmentDeltaY);
    horizontalAlignment = Right;
    verticalAlignment = Top;
    return *this;
  }
  RowColumnPlacer &alignRightMiddle(double alignmentDeltaX = 0, double alignmentDeltaY = 0) {
    setDeltas(alignmentDeltaX, alignmentDeltaY);
    horizontalAlignment = Right;
    verticalAlignment = Middle;
    return *this;
  }
  RowColumnPlacer &alignRightBottom(double alignmentDeltaX = 0, double alignmentDeltaY = 0) {
    setDeltas(alignmentDeltaX, alignmentDeltaY);
    horizontalAlignment = Right;
    verticalAlignment = Bottom;
    return *this;
  }
  
  void alignLeftTop(GuiElement *guiElement, double alignmentDeltaX = 0, double alignmentDeltaY = 0) {
    setDeltas(alignmentDeltaX, alignmentDeltaY);
    horizontalAlignment = Left;
    verticalAlignment = Top;
    setPosition(guiElement);
  }
  void alignLeftMiddle(GuiElement *guiElement, double alignmentDeltaX = 0, double alignmentDeltaY = 0) {
    setDeltas(alignmentDeltaX, alignmentDeltaY);
    horizontalAlignment = Left;
    verticalAlignment = Middle;
    setPosition(guiElement);
  }
  void alignLeftBottom(GuiElement *guiElement, double alignmentDeltaX = 0, double alignmentDeltaY = 0) {
    setDeltas(alignmentDeltaX, alignmentDeltaY);
    horizontalAlignment = Left;
    verticalAlignment = Bottom;
    setPosition(guiElement);
  }
  void alignRightTop(GuiElement *guiElement, double alignmentDeltaX = 0, double alignmentDeltaY = 0) {
    setDeltas(alignmentDeltaX, alignmentDeltaY);
    horizontalAlignment = Right;
    verticalAlignment = Top;
    setPosition(guiElement);
  }
  void alignRightMiddle(GuiElement *guiElement, double alignmentDeltaX = 0, double alignmentDeltaY = 0) {
    setDeltas(alignmentDeltaX, alignmentDeltaY);
    horizontalAlignment = Right;
    verticalAlignment = Middle;
    setPosition(guiElement);
  }
  void alignRightBottom(GuiElement *guiElement, double alignmentDeltaX = 0, double alignmentDeltaY = 0) {
    setDeltas(alignmentDeltaX, alignmentDeltaY);
    horizontalAlignment = Right;
    verticalAlignment = Bottom;
    setPosition(guiElement);
  }
  
  double width = 300;
  double line = 0;
  double lineStart = 0;
  double column = 10;
  double columnStart = 10;
  double knobSize = 32; // rename to height
  
  bool rightToLeft = false;
  
  //double defaultDeltaX = 0;
  
  RowColumnPlacer() {}
  RowColumnPlacer(double width) : width(width) {}
  RowColumnPlacer(GuiElement *parentElement) : width(parentElement->size.x) {}
  RowColumnPlacer(double width, double columnStart, double lineStart) {
    this->width = width;
    this->column = this->columnStart = columnStart;
    this->line = this->lineStart = lineStart;
  }
  RowColumnPlacer(GuiElement *parentElement, double columnStart, double lineStart) {
    width = parentElement->size.x;
    this->column = this->columnStart = columnStart;
    this->line = this->lineStart = lineStart;
  }
  void init(double width, double height, double progressDeltaX, double columnStart, double lineStart, bool rightToLeft = false) {
    this->width = width;
    this->progressDeltaX = progressDeltaX;
    this->column = this->columnStart = columnStart;
    this->line = this->lineStart = lineStart;
    this->rightToLeft = rightToLeft;
    this->knobSize = height;
  }
  
  void reset() {
    line = lineStart;
    column = columnStart;
  }
  void reset(double columnStart) {
    line = lineStart;
    this->column = columnStart;
  }
  void reset(double columnStart, double lineStart) {
    this->column = columnStart;
    this->line = lineStart;
  }
  double progressX() {
    return progressX(knobSize);
  }
  double progressX(double deltaX) {
    double r = column;
    column += deltaX;
    
    if(column >= width - columnStart) {
      column = columnStart;
      r = column;
      line += knobSize;
      column += deltaX;
    }
    return rightToLeft ? width - r : r;
  }
  
  double getX() {
    return rightToLeft ? width - column : column;
  }

  double getY() {
    return line;
  }
};


struct ConstantWidthColumnPlacer : public LayoutPlacer {
  double x = 0, y = 0;

  double width = 300;
  double verticalSpacing = 2;
  double verticalBorder = 6;
  double horizontalBorder = 6;
  
  virtual ~ConstantWidthColumnPlacer() {}
  ConstantWidthColumnPlacer() {}
  ConstantWidthColumnPlacer(double width, double verticalSpacing = 2, double horizontalBorder = 6, double verticalBorder = 6) {
    this->width = width;
    this->verticalSpacing = verticalSpacing;
    this->horizontalBorder = horizontalBorder;
    this->verticalBorder = verticalBorder;
    x = horizontalBorder;
    y = verticalBorder;
  }
  void init(double width, double verticalSpacing = 2, double horizontalBorder = 6, double verticalBorder = 6) {
    this->width = width;
    this->verticalSpacing = verticalSpacing;
    this->verticalSpacing = verticalSpacing;
    this->verticalBorder = verticalBorder;
    x = horizontalBorder;
    y = verticalBorder;
  }

  void setPosition(GuiElement *guiElement) override {
    guiElement->setPosition(x, y);
    guiElement->setSize(width, guiElement->size.y);
    y += guiElement->size.y + verticalSpacing;
  }
  
  ConstantWidthColumnPlacer &align() {
    return *this;
  }

  double getX() {
    return x;
  }

  double getY() {
    return y;
  }
};

