#pragma once

#include "guielement.h"
#include "../texture.h"

static void p(double d) {
  printf("%f\n", d);
}
static void p(long l) {
  printf("%lu\n", l);
}

/* TODO, FIXME
 * - loaded textures into static buffer with shared pointers
 * - integer valued knobs turn faster CCW (seems like a rounding happening)
 * - unlimited knobs not tested
 */


template<class T>
struct RotaryKnob : public GuiElement
{
  // TODO shared pointer
  //static std::unordered_map<std::string, Texture*> buttonTextures;
  
  enum Mode { LimitedKnob, UnlimitedKnob, ListKnob };
  
  Mode mode = LimitedKnob;
  
  Vec4d defaultOverlayColor;
  Vec4d inactiveOverlayColor;
  Vec4d hoverOverlayColor;
  Vec4d pressedOverlayColor;
  
  Texture *texture = NULL;
  Texture *backgroundTexture = NULL;
  Texture *hoverTexture = NULL;

  Rect rect;
  
  T minValue, maxValue;
  
  double knobRotActual = 0, knobRotValue = 0, knobMin = 0, knobMax = 1;
  
  std::function<T(T minValue, T maxValue, double x, double min, double max)> valueMappingFunction;// = []() { return 1.0; };
  std::function<std::string()> getValueString; 
  
  double fadeDuration = 0.2;
  double hoverTimer = 0, pressedTimer = 0;
    
  std::vector<std::string> items;
  int numItemsPerRotation = 128;
  
  T valueBefore;
  bool isValueBeforeInitialized = false;
  
  bool isIntegerType = false;

  virtual ~RotaryKnob() {
    delete texture;
    delete hoverTexture;
  }

  RotaryKnob(const std::string &name, double x, double y, Mode mode) : GuiElement(name) {
    init(name, x, y, mode);
    setDefaultMapping(0, 1, 0, knobMin, knobMax);
    blockParentInputOnMouseHover = true;
  }
  
  RotaryKnob(const std::string &name, double x, double y, Mode mode, T minValue, T maxValue, T value) : GuiElement(name) {
    init(name, x, y, mode);
    double knobRot = map((double)value, (double)minValue, (double)maxValue, knobMin, knobMax);
    setDefaultMapping(minValue, maxValue, knobRot, knobMin, knobMax);
    blockParentInputOnMouseHover = true;
  }
  
  RotaryKnob(const std::string &name, double x, double y, Mode mode, T minValue, T maxValue, T value, const std::function<T(double)> &valueMappingFunction) : GuiElement(name) {
    init(name, x, y, mode);
    double knobRot = map((double)value, (double)minValue, (double)maxValue, knobMin, knobMax);
    setMapping(valueMappingFunction, minValue, maxValue, knobRot, knobMin, knobMax);
    blockParentInputOnMouseHover = true;
  }
  
  
  RotaryKnob(const std::string &name, RowColumnPlacer &placer, Mode mode) : GuiElement(name) {
    init(name, 1, 1, mode);
    setDefaultMapping(0, 1, 0, knobMin, knobMax);
    blockParentInputOnMouseHover = true;
    placer.setPosition(this);
  }
  
  RotaryKnob(const std::string &name, RowColumnPlacer &placer, Mode mode, T minValue, T maxValue, T value) : GuiElement(name) {
    init(name, 1, 1, mode);
    double knobRot = map((double)value, (double)minValue, (double)maxValue, knobMin, knobMax);
    setDefaultMapping(minValue, maxValue, knobRot, knobMin, knobMax);
    blockParentInputOnMouseHover = true;
    placer.setPosition(this);
  }
  
  RotaryKnob(const std::string &name, RowColumnPlacer &placer, Mode mode, T minValue, T maxValue, T value, const std::function<T(double)> &valueMappingFunction) : GuiElement(name) {
    init(name, 1, 1, mode);
    double knobRot = map((double)value, (double)minValue, (double)maxValue, knobMin, knobMax);
    setMapping(valueMappingFunction, minValue, maxValue, knobRot, knobMin, knobMax);
    blockParentInputOnMouseHover = true;
    placer.setPosition(this);
  }
  
  void init(const std::string &name, double x, double y, Mode mode) {
    this->pos.set(x, y);
    this->mode = mode;
    
    defaultOverlayColor.set(1, 1, 1, 1);
    inactiveOverlayColor.set(0, 0, 0, 0.3);
    hoverOverlayColor.set(1, 1, 1, 0.66);
    pressedOverlayColor.set(1, 1, 1, 0.75);
    
    loadTexture();
    
    isIntegerType = typeid(T) == typeid(long) || typeid(T) == typeid(int) || typeid(T) == typeid(unsigned long) || typeid(T) == typeid(unsigned int);
  }
  
  void setMapping(const std::function<T(double)> &valueMappingFunction, T minValue, T maxValue, double knobRot = 0, double knobMin = 0, double knobMax = 1) {
    if(knobMin >= knobMax) {
      printf("Error at RotaryKnob::setMapping(), invalid limits, name '%s'\n", name.c_str());
    }
    getValueString = [this]() {
      //printf("getValueString %s: ", name.c_str()); 
      T r = getValue();
      //printf("%s, ", std::to_string(r).c_str()); p((double)r);
      return std::to_string((double)r);
    };
    knobRot = clamp(knobRot, knobMin, knobMax);
    this->valueMappingFunction = valueMappingFunction;
    this->minValue = minValue;
    this->maxValue = maxValue;
    this->knobRotActual = this->knobRotValue = knobRot;
    this->knobMin = knobMin;
    this->knobMax = knobMax;
  }
  
  void setDefaultMapping(T minValue, T maxValue, double knobRot = 0, double knobMin = 0, double knobMax = 1) {
    if(knobMin >= knobMax) {
      printf("Error at RotaryKnob::setMapping(), invalid limits, name '%s'\n", name.c_str());
    }
    if(mode != ListKnob) {
      this->valueMappingFunction = [this](T minValue, T maxValue, double x, double min, double max) {
        if(isIntegerType) {
          //printf("(long) value mapping function %s, %f %f %f %f %f", name.c_str(), (double)minValue, (double)maxValue, x, min, max); 
          T r = (T)((long)clamp(round(lerp(minValue, maxValue, map(x, min, max, 0, 1))), minValue, maxValue));
          //p((double)r);
          return r;
        }
        return (T)lerp(minValue, maxValue, clamp(x / (max-min), 0, 1));
      };
      getValueString = [this]() {
        return std::to_string(getValue());
      };
    }
    else {
      this->valueMappingFunction = [this](T minValue, T maxValue, double x, double min, double max) -> T {
        if(items.size() == 0) return -1;
        return modulo((long)(lerp(0, this->numItemsPerRotation, knobRotActual)), (long)this->items.size());
      };
      this->getValueString = [this]() -> std::string {
        int i = valueMappingFunction(this->minValue, this->maxValue, this->knobRotActual, this->knobMin, this->knobMax);
        if(i < 0 || i >= items.size()) return "<error>";
        return items[i];
      };
    }

    this->minValue = minValue;
    this->maxValue = maxValue;
    this->knobRotActual = this->knobRotValue = clamp(knobRot, knobMin, knobMax);
    this->knobMin = knobMin;
    this->knobMax = knobMax;
  }


  void loadTexture() {
    texture = new Texture(24, 24);
    backgroundTexture = new Texture(28, 28);
    
    if(mode == LimitedKnob) {
      if(!texture->load("data/synth/textures/limited_knob.png", false)) {
        printf("Error at RotaryKnob::loadTexture(), name '%s', failed to load file '%s'!\n", name.c_str(), "data/synth/textures/limited_knob.png");
        texture->clear(1, 0, 1, 1);
      }
      if(!backgroundTexture->load("data/synth/textures/limited_knob_bg.png", false)) {
        printf("Error at RotaryKnob::loadTexture(), name '%s', failed to load file '%s'!\n", name.c_str(), "data/synth/textures/limited_knob_bg.png");
        texture->clear(1, 1, 0, 1);
      }
    }
    else {
      if(!texture->load("data/synth/textures/unlimited_knob.png", false)) {
        printf("Error at RotaryKnob::loadTexture(), name '%s', failed to load file '%s'!\n", name.c_str(), "data/synth/textures/unlimited_knob.png");
        texture->clear(1, 0, 1, 1);
      }
      if(!backgroundTexture->load("data/synth/textures/unlimited_knob_bg.png", false)) {
        printf("Error at RotaryKnob::loadTexture(), name '%s', failed to load file '%s'!\n", name.c_str(), "data/synth/textures/unlimited_knob_bg.png");
        texture->clear(1, 1, 0, 1);
      }
    }

    rect.set(texture->w, texture->h, texture->w*0.5, texture->h*0.5);
    this->setSize(texture->w, texture->h);
  }





  void onMousePressed(Events &events) { 
    GuiElement::onMousePressed(events);

    if(isPointWithin(events.m) && captureAndHideMouse()) {
      isInputGrabbed = true;
    }
  }

  void onMouseReleased(Events &events) {
    GuiElement::onMouseReleased(events);
    if(isInputGrabbed) {
      isInputGrabbed = false;
      releaseMouse();
    }
  }
  void onMouseMotion(Events &events) {
    GuiElement::onMouseMotion(events);
    
    if(isInputGrabbed) {
      double factor = 0;
      if(events.mouseDownL && events.mouseDownR && events.mouseDownM) factor = 1.0/(6*6*6*6*6*60.0);
      else if(events.mouseDownL && events.mouseDownR) factor = 1.0/(6*6*6*60.0);
      else if(events.mouseDownL && events.mouseDownM) factor = 1.0/(6*6*6*6*60.0);
      else if(events.mouseDownL) factor = 1.0/(6*60.0);
      else if(events.mouseDownM) factor = 1.0/(6*6*60.0);
      else if(events.mouseDownR) factor = 1.0/60.0;
      
      rotateKnob(events.md.x * factor);
      checkValueChanged();
    }
  }
  

  void onMouseWheel(Events &events) {
    GuiElement::onMouseWheel(events);

    if(isMouseHover && isReadyToGrabFocus())  {
      if(mode == ListKnob) {
        rotateKnob((double)events.mouseWheel / numItemsPerRotation);
        //long value = valueMappingFunction(minValue, maxValue, knobRotValue, knobMin, knobMax);
        //setValue(value + events.mouseWheel);
        checkValueChanged();
      }
      else {
        double factor = 0.1;
        if(events.lControlDown || events.rControlDown || events.mouseDownL && !events.mouseDownR) factor *= 0.1;
        if(events.lShiftDown || events.rShiftDown || (events.mouseDownL && events.mouseDownR)) factor *= 0.01;
        if(events.lAltDown || events.rAltDown || (!events.mouseDownL && events.mouseDownR)) factor *= 10.0;
        
        rotateKnob(events.mouseWheel * factor);
        checkValueChanged();
      }
    }
  }


  virtual void onUpdate(double time, double dt) {
    hoverTimer += isMouseHover || isInputGrabbed ? dt : -dt;
    hoverTimer = clamp(hoverTimer, 0, fadeDuration);
  }


  virtual void onPrepare(GeomRenderer &geomRenderer, TextGl &textRenderer) {
    //GuiElement::prepare(geomRenderer, textRenderer);

    if(!hoverTexture) {
      hoverTexture = createSpriteShadowTexture(geomRenderer, *texture, 15, 7, 2, Vec4d(1, 1, 1, 1), true);
    }
  }

  bool isReadyToGrabFocus() {
    if(hoverTimer > 0 || isMouseHover || isInputGrabbed) {
      return !doesThisApplyToAnyOtherElement([this](GuiElement *a, GuiElement *b) {
        return b->isVisible && b->isInputGrabbed;
      });
    }
    return false;
  }

  virtual void onRender(GeomRenderer &geomRenderer, TextGl &textRenderer) {
    
    bool readyToGrabFocus = isReadyToGrabFocus();

    double angle;
    
    if(mode == LimitedKnob) {
      angle = map(knobRotValue, knobMin, knobMax, -PI*0.75, PI*0.75);
    }
    if(mode == UnlimitedKnob || mode == ListKnob) {
      angle = knobRotActual * 2.0 * PI;
    }
    
    backgroundTexture->render(rect.pos+absolutePos, defaultOverlayColor);
    
    if(readyToGrabFocus) {
      Vec4d col = mix(Vec4d(1, 1, 1, 0), hoverOverlayColor * defaultOverlayColor, hoverTimer/fadeDuration);
      hoverTexture->render(rect.pos+absolutePos, angle, col);
    }
    
    texture->render(rect.pos+absolutePos, angle, defaultOverlayColor);
    

    if((isInputGrabbed || isMouseHover) && readyToGrabFocus) {
      geomRenderer.texture = NULL;
      geomRenderer.strokeType = 1;
      geomRenderer.strokeWidth = 1;
      geomRenderer.strokeColor.set(1, 1, 1, 0.5);
      geomRenderer.fillColor.set(0, 0, 0, 0.75);
      std::string eventStr;
      if(mode == ListKnob) {
        std::string str = getValueString();
        //printf("onRender ListKnob %s, %s\n", name.c_str(), str.c_str());
        eventStr = format("%s %lu/%lu: %s", name.c_str(), (long)getValue()+1, items.size(), str.c_str());
      }
      else {
        /*printf("onRender LimitedKnob %s, %f %f %f %f %f\n", name.c_str(), (double)minValue, (double)maxValue, knobRotValue, knobMin, knobMax); 
        if(!getValueString) {
          printf("!getValueString\n");
        }*/
        //printf("onRender LimitedKnob %s, %s\n", name.c_str(), str.c_str());
        eventStr = format("%s: %s", name.c_str(), getValueString().c_str());
      }
      Vec2d eventStrSize = textRenderer.getDimensions(eventStr, 10);
      Vec2d size = eventStrSize + Vec2d(10, 0);
      Vec2d p = rect.pos+absolutePos - Vec2d(size.x*0.5, size.y + texture->w*0.5 + 5);
      //Vec2d p = rect.pos+absolutePos + Vec2d(-size.x*0.5, texture->w*0.5 + 3);
      
      geomRenderer.drawRectCorner(size.x, size.y, p.x, p.y);
      textRenderer.setColor(1, 1, 1, 0.93);
      textRenderer.print(eventStr, p.x+5, p.y+5, 10);
    }   
    
  }
  



  

  void setItems(const std::vector<std::string> &items) {
    if(mode != ListKnob) printf("Warning at RotaryKnob.setItems(), name '%s', mode is not ListKnob\n", name.c_str());
    this->items = std::vector<std::string>(items);
    knobRotActual = knobRotValue = 0;
  }

  void addItems(const std::string item0, const std::string item1 = "", const std::string item2 = "", const std::string item3 = "", const std::string item4 = "", const std::string item5 = "", const std::string item6 = "", const std::string item7 = "", const std::string item8 = "", const std::string item9 = "", const std::string item10 = "", const std::string item11 = "") {
    if(mode != ListKnob) printf("Warning at RotaryKnob.addItems(), name '%s', mode is not ListKnob\n", name.c_str());
    items.push_back(item0);
    if(item1.size() > 0) items.push_back(item1);
    if(item2.size() > 0) items.push_back(item2);
    if(item3.size() > 0) items.push_back(item3);
    if(item4.size() > 0) items.push_back(item4);
    if(item5.size() > 0) items.push_back(item5);
    if(item6.size() > 0) items.push_back(item6);
    if(item7.size() > 0) items.push_back(item7);
    if(item8.size() > 0) items.push_back(item8);
    if(item9.size() > 0) items.push_back(item9);
    if(item10.size() > 0) items.push_back(item10);
    if(item11.size() > 0) items.push_back(item11);
    //inputText = items[activeItemIndex];
    //prerenderingNeeded = true;
    knobRotActual = knobRotValue = 0;
  }

  void setActiveItem(int item) {
    if(mode != ListKnob) printf("Warning at RotaryKnob.setActiveItem(), name '%s', mode is not ListKnob\n", name.c_str());
    if(item < 0 || item >= items.size()) {
      knobRotActual = knobRotValue = 0;
    }
    else {
      knobRotActual = knobRotValue = map(item, 0, items.size(), knobMin, knobMax);
    }
  }
  void clearItems() {
    if(mode != ListKnob) printf("Warning at RotaryKnob.clearItems(), name '%s', mode is not ListKnob\n", name.c_str());
    items.clear();
    knobRotActual = knobRotValue = 0;
  }


  void rotateKnob(double deltaRot) {
    knobRotActual += deltaRot;
    knobRotValue = clamp(knobRotValue+deltaRot, knobMin, knobMax);
  }

  void checkValueChanged() {
    if(!isValueBeforeInitialized || valueBefore != getValue()) {
      valueBefore = getValue();
      isValueBeforeInitialized = true;
      onValueChange(this);
    }
  }

  inline T getValue() {
    return valueMappingFunction(minValue, maxValue, knobRotValue, knobMin, knobMax);
  }
  
  void getValue(void *value) {
    //*(bool*)value = this->value;
    *(T*)value = getValue();
  }
  void getValue(T &value) {
    value = getValue();
  }
  
  void setRotation(double value) {
    value = clamp(value, knobMin, knobMax);
    knobRotValue = knobRotActual = value;
    prerenderingNeeded = true;
    //this->value = value;
  }
  
  void setValue(T value) {
    if(mode == ListKnob) {
      if(items.size() > 0) {
        value = modulo((long)value, (long)items.size());
        knobRotValue = knobRotActual = map((double)value, 0.0, numItemsPerRotation, knobMin, knobMax);
      }
    }
    else {
      value = clamp(value, minValue, maxValue);
      if((T)value != getValue()) {
        knobRotValue = knobRotActual = map((double)value, (double)minValue, (double)maxValue, knobMin, knobMax);
      }
    }
    prerenderingNeeded = true;
    //this->value = value;
  }
  
  
};
