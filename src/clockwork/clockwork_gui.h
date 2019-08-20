#pragma once
#include "../engine/sketch.h"



struct EditingBodyPanel : public Panel
{
  //Panel *panel;
  TextBox *nameGui;
  NumberBox *elasticityGui, *frictionGui, *densityGui;
  TextBox *textureGui, *texture2Gui, *fragmentShaderGui;
  ColorBox *fillColorGui, *strokeColorGui;
  CheckBox *applyToAllBodypartsGui;

  Body *body = NULL;
  BodyPart *bodyPart = NULL;

  EditingBodyPanel() : Panel(250, 280, 30, 30) {
    init();
  }

  EditingBodyPanel(BodyPart *bodyPart) : Panel(250, 280, 30, 30) {
    init();
    set(bodyPart);
  }

  void init() {
    //panel = new Panel(400, 200, 30, 30);
    nameGui = new TextBox("name", "<name>", 10, 10);
    applyToAllBodypartsGui = new CheckBox("apply to whole body", true, 10, 40);
    elasticityGui = new NumberBox("elasticity", 0, NumberBox::FLOATING_POINT, 0, 1, 10, 70, 10);
    frictionGui = new NumberBox("friction", 0, NumberBox::FLOATING_POINT, 0, 1, 10, 100, 10);
    densityGui = new NumberBox("density", 0, NumberBox::FLOATING_POINT, 0, 1000000, 10, 130, 10);
    fillColorGui = new ColorBox("fill color", 0, 0, 0, 0, 10, 160);
    strokeColorGui = new ColorBox("stroke color", 0, 0, 0, 0, 10, 190);
    textureGui = new TextBox("texture ", "-", 10, 220);
    //texture2Gui = new TextBox("texture 2", "-", 10, 220);
    //fragmentShaderGui = new TextBox("fragment shader", "-", 10, 220);

    addChildElement(nameGui);
    addChildElement(applyToAllBodypartsGui);
    addChildElement(elasticityGui);
    addChildElement(frictionGui);
    addChildElement(densityGui);
    addChildElement(fillColorGui);
    addChildElement(strokeColorGui);
    addChildElement(textureGui);

    addGuiEventListener(new EventListener(this));
  }

  void set(BodyPart *bodyPart) {
    body = bodyPart->parentBody;
    this->bodyPart = bodyPart;

    nameGui->setValue(body->name);
    elasticityGui->setValue(bodyPart->getElasticity());
    frictionGui->setValue(bodyPart->getFriction());
    densityGui->setValue(bodyPart->getDensity());
    fillColorGui->setValue(bodyPart->renderingSettings.fillColor);
    strokeColorGui->setValue(bodyPart->renderingSettings.strokeColor);
    if(bodyPart->renderingSettings.texture) {
      textureGui->setValue(bodyPart->renderingSettings.texture->name);
    }
  }


  struct EventListener : public GuiEventListener {

    EditingBodyPanel *editingBodyPanel;
    EventListener(EditingBodyPanel *editingBodyPanel) {
      this->editingBodyPanel = editingBodyPanel;
    }

    void onAction(GuiElement *guiElement) {
      if(!editingBodyPanel->bodyPart || !editingBodyPanel->body) return;

      if(guiElement == editingBodyPanel->textureGui) {
        Texture *tex = new Texture();
        std::string filename;
        editingBodyPanel->textureGui->getValue((void*)&filename);

        if(tex->load(filename)) {
          printf("Texture loaded%s!\n", filename.c_str());
          editingBodyPanel->bodyPart->renderingSettings.texture = tex;
          // FIXME free the texture when not needed
        }
        else {
          printf("Failed to load texture %s\n", filename.c_str());
          delete tex;
        }
      }
    }

    void onValueChange(GuiElement *guiElement) {
      if(!editingBodyPanel->bodyPart || !editingBodyPanel->body) return;

      bool applyToAllBodyparts;
      editingBodyPanel->applyToAllBodypartsGui->getValue((void*)&applyToAllBodyparts);

      if(guiElement == editingBodyPanel->nameGui) {
        guiElement->getValue((void*)&editingBodyPanel->body->name);
      }
      if(guiElement == editingBodyPanel->elasticityGui) {
        double value;
        guiElement->getValue((void*)&value);
        if(applyToAllBodyparts) {
          editingBodyPanel->body->setElasticity(value);
          editingBodyPanel->body->elasticityDefault = value;
        }
        else editingBodyPanel->bodyPart->setElasticity(value);
      }
      if(guiElement == editingBodyPanel->frictionGui) {
        double value;
        guiElement->getValue((void*)&value);
        if(applyToAllBodyparts) {
          editingBodyPanel->body->setFriction(value);
          editingBodyPanel->body->frictionDefault = value;
        }
        else editingBodyPanel->bodyPart->setFriction(value);
      }
      if(guiElement == editingBodyPanel->densityGui) {
        double value;
        guiElement->getValue((void*)&value);
        if(applyToAllBodyparts) {
          editingBodyPanel->body->setDensity(value);
          editingBodyPanel->body->densityDefault = value;
        }
        else editingBodyPanel->bodyPart->setDensity(value);
      }
      if(guiElement == editingBodyPanel->fillColorGui) {
        Vec4d value;
        guiElement->getValue((void*)&value);
        if(applyToAllBodyparts) {
          editingBodyPanel->body->bodyPartRenderingSettingsDefault.fillColor.set(value);
          for(BodyPart *bp : editingBodyPanel->body->bodyParts) {
            bp->renderingSettings.fillColor.set(value);
          }
        }
        else editingBodyPanel->bodyPart->renderingSettings.fillColor.set(value);
      }
      if(guiElement == editingBodyPanel->strokeColorGui) {
        Vec4d value;
        guiElement->getValue((void*)&value);
        if(applyToAllBodyparts) {
          editingBodyPanel->body->bodyPartRenderingSettingsDefault.strokeColor.set(value);
          for(BodyPart *bp : editingBodyPanel->body->bodyParts) {
            bp->renderingSettings.strokeColor.set(value);
          }
        }
        else editingBodyPanel->bodyPart->renderingSettings.strokeColor.set(value);
      }
    }
  };
};

struct ConstraintPanel : public Panel
{
  //Panel *panel;
  TextBox *constraintTypeGui;
  NumberBox *maxForceGui, *errorBiasGui, *maxBiasGui;
  CheckBox *collideBodiesGui;

  NumberBox *motorRateGui; // simple motor

  NumberBox *minAngleGui, *maxAngleGui; // rotary limit

  NumberBox *angleGui, *speedGui; // constant angle
  CheckBox *squareRootedSpringGui;

  NumberBox *restLengthGui, *stiffnessGui, *dampingGui; // damped spring


  Constraint *constraint = NULL;

  ConstraintPanel() : Panel(250, 250, 30, 30) {
    init();
  }

  ConstraintPanel(Constraint *constraint) : Panel(250, 250, 30, 30) {
    this->constraint = constraint;
    init();
    set(constraint);
  }

  void init() {
    //panel = new Panel(400, 200, 30, 30);
    constraintTypeGui = new TextBox("type", "<type>", 10, 10);
    collideBodiesGui = new CheckBox("collide bodies", false, 10, 40);
    maxForceGui = new NumberBox("max force", 0, NumberBox::FLOATING_POINT, 0, 1e10, 10, 70, 10);
    errorBiasGui = new NumberBox("error bias", 0, NumberBox::FLOATING_POINT, 0, 1, 10, 100, 10);
    maxBiasGui = new NumberBox("max bias", 0, NumberBox::FLOATING_POINT, 0, 1e10, 10, 130, 10);


    addChildElement(constraintTypeGui);
    addChildElement(collideBodiesGui);
    addChildElement(maxForceGui);
    addChildElement(errorBiasGui);
    addChildElement(maxBiasGui);

    if(constraint) {
      if(constraint->type == Constraint::simpleMotor) {
        motorRateGui = new NumberBox("motor rate", 0, NumberBox::FLOATING_POINT, -1e10, 1e10, 10, 160, 10);
        addChildElement(motorRateGui);
      }
      if(constraint->type == Constraint::constantAngleConstraint) {
        angleGui = new NumberBox("angle", 0, NumberBox::FLOATING_POINT, -3600, 3600, 10, 160, 10);
        angleGui->incrementMode = NumberBox::IncrementMode::Linear;
        speedGui = new NumberBox("motor rate", 0, NumberBox::FLOATING_POINT, 0, 1e10, 10, 190, 10);
        squareRootedSpringGui = new CheckBox("squarerooted spring factor ", false, 10, 220);
        addChildElement(angleGui);
        addChildElement(speedGui);
        addChildElement(squareRootedSpringGui);
      }
      if(constraint->type == Constraint::rotaryLimitConstraint) {
        minAngleGui = new NumberBox("min angle", 0, NumberBox::FLOATING_POINT, -3600, 3600, 10, 160, 10);
        maxAngleGui = new NumberBox("max angle", 0, NumberBox::FLOATING_POINT, -3600, 3600, 10, 190, 10);
        minAngleGui->incrementMode = NumberBox::IncrementMode::Linear;
        maxAngleGui->incrementMode = NumberBox::IncrementMode::Linear;
        addChildElement(minAngleGui);
        addChildElement(maxAngleGui);
      }
      if(constraint->type == Constraint::dampedSpring) {
        restLengthGui = new NumberBox("rest length", 0, NumberBox::FLOATING_POINT, 0, 1e10, 10, 160, 10);
        stiffnessGui = new NumberBox("stiffness", 0, NumberBox::FLOATING_POINT, 0, 1e10, 10, 190, 10);
        dampingGui = new NumberBox("damping", 0, NumberBox::FLOATING_POINT, 0, 1e10, 10, 220, 10);
        addChildElement(restLengthGui);
        addChildElement(stiffnessGui);
        addChildElement(dampingGui);
      }
    }

    addGuiEventListener(new EventListener(this));
  }

  void set(Constraint *constraint) {
    this->constraint = constraint;

    constraintTypeGui->setValue(ConstraintNames[constraint->type-1]);
    maxForceGui->setValue(constraint->getMaxForce());
    errorBiasGui->setValue(constraint->getErrorBias());
    maxBiasGui->setValue(constraint->getMaxBias());
    collideBodiesGui->setValue(constraint->getCollideBodies());

    if(constraint->type == Constraint::simpleMotor) {
      motorRateGui->setValue(dynamic_cast<SimpleMotor*>(constraint)->getRate());
    }

    if(constraint->type == Constraint::rotaryLimitConstraint) {
      minAngleGui->setValue(dynamic_cast<RotaryLimitConstraint*>(constraint)->getMinAngle() * 180.0 / PI);
      maxAngleGui->setValue(dynamic_cast<RotaryLimitConstraint*>(constraint)->getMaxAngle() * 180.0 / PI);
    }

    if(constraint->type == Constraint::constantAngleConstraint) {
      angleGui->setValue(dynamic_cast<ConstantAngleConstraint*>(constraint)->angle * 180.0 / PI);
      speedGui->setValue(dynamic_cast<ConstantAngleConstraint*>(constraint)->speed);
      squareRootedSpringGui->setValue(dynamic_cast<ConstantAngleConstraint*>(constraint)->squareRootedSpringFactor);
    }

    if(constraint->type == Constraint::dampedSpring) {
      restLengthGui->setValue(dynamic_cast<DampedSpring*>(constraint)->restLength);
      stiffnessGui->setValue(dynamic_cast<DampedSpring*>(constraint)->stiffness);
      dampingGui->setValue(dynamic_cast<DampedSpring*>(constraint)->damping);
    }
  }


  struct EventListener : public GuiEventListener {

    ConstraintPanel *constraintPanel;
    EventListener(ConstraintPanel *constraintPanel) {
      this->constraintPanel = constraintPanel;
    }
    void onValueChange(GuiElement *guiElement) {
      if(!constraintPanel->constraint) return;


      if(guiElement == constraintPanel->maxForceGui) {
        double value;
        guiElement->getValue((void*)&value);
        constraintPanel->constraint->setMaxForce(value);
      }
      if(guiElement == constraintPanel->errorBiasGui) {
        double value;
        guiElement->getValue((void*)&value);
        constraintPanel->constraint->setErrorBias(value);
      }
      if(guiElement == constraintPanel->maxBiasGui) {
        double value;
        guiElement->getValue((void*)&value);
        constraintPanel->constraint->setMaxBias(value);
      }
      if(guiElement == constraintPanel->collideBodiesGui) {
        bool value;
        guiElement->getValue((void*)&value);
        constraintPanel->constraint->setCollideBodies(value);
      }

      if(guiElement == constraintPanel->motorRateGui) {
        double value;
        guiElement->getValue((void*)&value);
        dynamic_cast<SimpleMotor*>(constraintPanel->constraint)->setRate(value);
      }

      if(guiElement == constraintPanel->minAngleGui) {
        double value;
        guiElement->getValue((void*)&value);
        dynamic_cast<RotaryLimitConstraint*>(constraintPanel->constraint)->setMinAngle(value * PI / 180.0);
      }
      if(guiElement == constraintPanel->maxAngleGui) {
        double value;
        guiElement->getValue((void*)&value);
        dynamic_cast<RotaryLimitConstraint*>(constraintPanel->constraint)->setMaxAngle(value * PI / 180.0);
      }

      if(guiElement == constraintPanel->angleGui) {
        double value;
        guiElement->getValue((void*)&value);
        dynamic_cast<ConstantAngleConstraint*>(constraintPanel->constraint)->setAngle(value * PI / 180.0);
      }
      if(guiElement == constraintPanel->speedGui) {
        double value;
        guiElement->getValue((void*)&value);
        dynamic_cast<ConstantAngleConstraint*>(constraintPanel->constraint)->setSpeed(value);
      }
      if(guiElement == constraintPanel->squareRootedSpringGui) {
        bool value;
        guiElement->getValue((void*)&value);
        dynamic_cast<ConstantAngleConstraint*>(constraintPanel->constraint)->setSquareRootedSpringFactor(value);
      }

      if(guiElement == constraintPanel->restLengthGui) {
        double value;
        guiElement->getValue((void*)&value);
        dynamic_cast<DampedSpring*>(constraintPanel->constraint)->setRestLength(value);
      }
      if(guiElement == constraintPanel->stiffnessGui) {
        double value;
        guiElement->getValue((void*)&value);
        dynamic_cast<DampedSpring*>(constraintPanel->constraint)->setStiffness(value);
      }
      if(guiElement == constraintPanel->dampingGui) {
        double value;
        guiElement->getValue((void*)&value);
        dynamic_cast<DampedSpring*>(constraintPanel->constraint)->setDamping(value);
      }


    }
  };



};


struct CollisionCategorySelectionGUI
{
  std::vector<Rect> catRects, maskRects;
  NearestObjectQuery *nearestObjectQuery = NULL;
  Body **editingBody;
  BodyPart **lockedBodyPart;
  bool isHoveringCategory = false, isHoveringMask = false;

  unsigned int categoryBitsCopy = 0, maskBitsCopy = 0;
  unsigned int categoryBitsDefault = 0, maskBitsDefault = 0;


  void init(int screenW, int screenH, BodyPart **lockedBodyPart, Body **editingBody, NearestObjectQuery *nearestObjectQuery) {
   this->lockedBodyPart = lockedBodyPart;
   this->editingBody = editingBody;
   this->nearestObjectQuery = nearestObjectQuery;

   double w = 12, h = 12;
   double x = screenW-10, y = 80;

   for(int i=0; i<sizeof(unsigned int)*8; i++) {
     Rect catRect(w, h, x - w*1.5 * 2.0, y + h * 1.4 * i + int(i / 8)*6.0);
     Rect maskRect(w, h, x - w*1.5 * 1.0, y + h * 1.4 * i + int(i / 8)*6.0);
     catRects.push_back(catRect);
     maskRects.push_back(maskRect);
   }
  }

  bool onMousePressed(Events &events) {
   if(*editingBody == NULL) return false;
   Body *body = *editingBody;
   BodyPart *bodyPart = NULL;
   if(*lockedBodyPart) {
     bodyPart = *lockedBodyPart;
     body = NULL;
   }

   for(int i=0; i<catRects.size(); i++) {
     if(catRects[i].isPointWithin(events.m)) {
       if(body) {
         body->toggleCollisionCategory(i);
         return true;
       }
       else {
         bodyPart->toggleCollisionCategory(i);
         return true;
       }
     }
   }
   for(int i=0; i<maskRects.size(); i++) {
     if(maskRects[i].isPointWithin(events.m)) {
       if(body) {
         body->toggleCollisionMask(i);
         return true;
       }
       else {
         bodyPart->toggleCollisionMask(i);
         return true;
       }
     }
   }
   return false;
  }

  bool onMouseMotion(Events &events) {
   // TODO when hovering a particular category highlight all the objects that belong to it
   // TODO toggle when entering a cell
   isHoveringCategory = false;
   isHoveringMask = false;
   /*if(*editingBody == NULL) return false;
   Body *body = *editingBody;
   BodyPart *bodyPart = NULL;
   if(events.lControlDown && nearestObjectQuery->result != 0 && nearestObjectQuery->body == body) {
     bodyPart = nearestObjectQuery->bodyPart;
     body == NULL;
   }*/

   for(int i=0; i<catRects.size(); i++) {
     if(catRects[i].isPointWithin(events.m)) {
       /*if(body) {
         if(events.mouseDownL) body->toggleCollisionCategory(i);
       }
       else {
         if(events.mouseDownL) bodyPart->toggleCollisionCategory(i);
       }*/
       isHoveringCategory = true;
     }
   }
   for(int i=0; i<maskRects.size(); i++) {
     if(maskRects[i].isPointWithin(events.m)) {
       /*if(body) {
         if(events.mouseDownL) body->toggleCollisionMask(i);
       }
       else {
         if(events.mouseDownL) bodyPart->toggleCollisionMask(i);
       }*/
       isHoveringMask = true;
     }
   }
   return false;
  }


  bool onKeyDown(Events &events, Simulation &simulation) {
   if(*editingBody == NULL || (!isHoveringCategory && !isHoveringMask)) return false;

   Body *body = *editingBody;
   BodyPart *bodyPart = NULL;

   if(*lockedBodyPart) {
     bodyPart = *lockedBodyPart;
     body = NULL;
   }

   if(events.sdlKeyCode == SDLK_a) {
     if(isHoveringCategory) {
       if(body) {
         body->setCollisionCategories(allCategories);
       }
       else {
         bodyPart->setCollisionCategories(allCategories);
       }
     }
     if(isHoveringMask) {
       if(body) {
         body->setCollisionMask(allCategories);
       }
       else {
         bodyPart->setCollisionMask(allCategories);
       }
     }
   }

   if(events.sdlKeyCode == SDLK_n) {
     if(isHoveringCategory) {
       if(body) {
         body->setCollisionCategories(noCategories);
       }
       else {
         bodyPart->setCollisionCategories(noCategories);
       }
     }
     if(isHoveringMask) {
       if(body) {
         body->setCollisionMask(noCategories);
       }
       else {
         bodyPart->setCollisionMask(noCategories);
       }
     }
   }

   if(events.sdlKeyCode == SDLK_i) {
     if(isHoveringCategory) {
       if(body) {
         body->setCollisionCategories(~body->chipCollisionFilter.categories);
       }
       else {
         bodyPart->setCollisionCategories(~bodyPart->chipCollisionFilter.categories);
       }
     }
     if(isHoveringMask) {
       if(body) {
         body->setCollisionMask(~body->chipCollisionFilter.mask);
       }
       else {
         bodyPart->setCollisionMask(~bodyPart->chipCollisionFilter.mask);
       }
     }
   }

   if(events.sdlKeyCode == SDLK_c) {
     if(isHoveringCategory) {
       if(body) {
         categoryBitsCopy = body->chipCollisionFilter.categories;
       }
       else {
         categoryBitsCopy = bodyPart->chipCollisionFilter.categories;
       }
     }
     if(isHoveringMask) {
       if(body) {
         maskBitsCopy = body->chipCollisionFilter.mask;
       }
       else {
         maskBitsCopy = bodyPart->chipCollisionFilter.mask;
       }
     }
   }

   if(events.sdlKeyCode == SDLK_v) {
     if(isHoveringCategory) {
       if(body) {
         body->setCollisionCategories(categoryBitsCopy);
       }
       else {
         bodyPart->setCollisionCategories(categoryBitsCopy);
       }
     }
     if(isHoveringMask) {
       if(body) {
         body->setCollisionMask(maskBitsCopy);
       }
       else {
         bodyPart->setCollisionMask(maskBitsCopy);
       }
     }
   }

   if(events.sdlKeyCode == SDLK_d) {
     if(body) {
       simulation.chipCollisionFilterDefault.categories = body->chipCollisionFilter.categories;
       simulation.chipCollisionFilterDefault.mask = body->chipCollisionFilter.mask;
     }
     else {
       simulation.chipCollisionFilterDefault.categories = bodyPart->chipCollisionFilter.categories;
       simulation.chipCollisionFilterDefault.mask = bodyPart->chipCollisionFilter.mask;
     }
   }

   return true;
  }




  void render(GeomRenderer &geomRenderer, Events &events) {
   if(*editingBody == NULL) return;
   unsigned int categoryBits, maskBits;

   if(nearestObjectQuery->result != 0 && nearestObjectQuery->body == *editingBody) {
     categoryBits = nearestObjectQuery->bodyPart->chipCollisionFilter.categories;
     maskBits = nearestObjectQuery->bodyPart->chipCollisionFilter.mask;
   }
   else if(*lockedBodyPart && (*lockedBodyPart)->parentBody == *editingBody) {
     categoryBits = (*lockedBodyPart)->chipCollisionFilter.categories;
     maskBits = (*lockedBodyPart)->chipCollisionFilter.mask;
   }
   else {
     categoryBits = (*editingBody)->chipCollisionFilter.categories;
     maskBits = (*editingBody)->chipCollisionFilter.mask;
   }
   geomRenderer.strokeColor.set(0, 0, 0, 0.9);
   geomRenderer.strokeType = 1;
   geomRenderer.strokeWidth = 1;

   for(int i=0; i<catRects.size(); i++) {

     if(catRects[i].isPointWithin(events.m)) {
       geomRenderer.strokeColor.set(0.8, 0.8, 0.8, 0.9);
       geomRenderer.strokeWidth = 2;
     }
     else {
       geomRenderer.strokeColor.set(0.0, 0.0, 0.0, 0.9);
       geomRenderer.strokeWidth = 1;
     }
     if((categoryBits & (1<<i)) == (1<<i)) {
       geomRenderer.fillColor.set(0.6, 0.0, 0.2, 0.5);
     }
     else {
       geomRenderer.fillColor.set(0.2, 0.1, 0.1, 0.5);
     }
     geomRenderer.drawRect(catRects[i]);

     if(maskRects[i].isPointWithin(events.m)) {
       geomRenderer.strokeColor.set(0.8, 0.8, 0.8, 0.9);
       geomRenderer.strokeWidth = 2;
     }
     else {
       geomRenderer.strokeColor.set(0.0, 0.0, 0.0, 0.9);
       geomRenderer.strokeWidth = 1;
     }
     if((maskBits & (1<<i)) == (1<<i)) {
       geomRenderer.fillColor.set(0.6, 0.0, 0.2, 0.5);
     }
     else {
       geomRenderer.fillColor.set(0.2, 0.1, 0.1, 0.5);
     }
     geomRenderer.drawRect(maskRects[i]);
   }
  }
};
