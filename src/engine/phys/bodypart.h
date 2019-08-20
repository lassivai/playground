#pragma once
#include "chipphys.h"
#include "../geometry.h"
#include "../geom_gfx.h"

/* TODO
 * - sprite rendering, animated sprites, procedural textures/animations
 *
 */

struct Body;

struct Impulse {
  Vec2d impulse;
  double impulseMagnitude = 0;
  double kineticEnergyLost = 0;

  virtual ~Impulse() {}

  void add(const Impulse &imp) {
    impulse += imp.impulse;
    impulseMagnitude += imp.impulseMagnitude;
    kineticEnergyLost += imp.kineticEnergyLost;
  }

  void clear() {
    impulse.set(0, 0);
    impulseMagnitude = 0;
    kineticEnergyLost = 0;
  }
};

struct BodyPart
{
  GeomObject *geomObject;
  std::vector<cpShape*> chipShapes;
  cpSpace *chipSpace;
  GeomRenderer::RenderingSettings renderingSettings;
  double density = 1.0;
  Body *parentBody = NULL;
  cpShapeFilter chipCollisionFilter;
  //std::vector<Impulse> currentImpulses;
  Impulse currentImpulse;

  //Texture *spriteTexture = NULL;
  //Quadx *spriteQuad = NULL;

  Texture *texture = NULL;

  virtual ~BodyPart() {}

  void clearCurrentImpulses() {
    //currentImpulses.clear();
    currentImpulse.clear();
  }

  void setDefaultRenderingSettings() {
    //renderingSettings.fillColor.set(1, 1, 1, 0.5);
    //renderingSettings.strokeColor.set(1, 1, 1, 1);
    renderingSettings.fillColor.set(0, 0, 0, 0.5);
    renderingSettings.strokeColor.set(0, 0, 0, 1);
    renderingSettings.strokeWidth = 3;
    renderingSettings.strokeType = 1;
    renderingSettings.texture = NULL;
  }

  BodyPart() {
    setDefaultRenderingSettings();
  }

  void set(GeomObject *geomObject, cpShape *chipShape, cpSpace *chipSpace, Body *parentBody, double density = 0.001) {
    setGeomObject(geomObject);
    chipShapes.push_back(chipShape);
    this->chipSpace = chipSpace;
    this->parentBody = parentBody;
    this->density = density;
    initShapes();
  }

  void set(GeomObject *geomObject, std::vector<cpShape*> chipShapes, cpSpace *chipSpace, Body *parentBody, double density = 0.001) {
    setGeomObject(geomObject);
    this->chipShapes = chipShapes;
    this->chipSpace = chipSpace;
    this->parentBody = parentBody;
    this->density = density;
    initShapes();
  }

  void initShapes() {
    for(cpShape *chipShape : chipShapes) {
      cpShapeSetUserData(chipShape, (cpDataPointer)this);
      cpShapeSetFilter(chipShape, collisionFilterDefault);
      cpShapeSetCollisionType(chipShape, 1);
    }
    updateMass();
  }
  //cpShapeSetUserData(cpShape *shape, cpDataPointer value)
  //cpDataPointer cpShapeGetUserData(const cpShape *shape)



  // TODO find out if there would be simpler way of doing that...
  void setGeomObject(GeomObject *geomObject) {
    if(geomObject->type == GeomObject::circle) {
      this->geomObject = new Circle(*dynamic_cast<Circle*>(geomObject));
    }
    if(geomObject->type == GeomObject::hoop) {
      this->geomObject = new Hoop(*dynamic_cast<Hoop*>(geomObject));
    }
    if(geomObject->type == GeomObject::line) {
      this->geomObject = new Line(*dynamic_cast<Line*>(geomObject));
    }
    if(geomObject->type == GeomObject::triangle) {
      this->geomObject = new Triangle(*dynamic_cast<Triangle*>(geomObject));
    }
    if(geomObject->type == GeomObject::rect) {
      this->geomObject = new Rect(*dynamic_cast<Rect*>(geomObject));
    }
    if(geomObject->type == GeomObject::polygon) {
      Polygon *polygon = new Polygon(*dynamic_cast<Polygon*>(geomObject));
      this->geomObject = polygon;
      polygon->useCachedTriangles = true;
    }
  }

  void remove() {
    for(cpShape *chipShape : chipShapes) {
      if(cpSpaceContainsShape(chipSpace, chipShape)) {
        cpSpaceRemoveShape(chipSpace, chipShape);
      }
      cpShapeFree(chipShape);
    }
    chipShapes.clear();

    if(geomObject) delete geomObject;
  }

  inline void setElasticity(double elasticity) {
    for(cpShape *chipShape : chipShapes) {
      cpShapeSetElasticity(chipShape, elasticity);
    }
  }

  inline void setFriction(double friction) {
    for(cpShape *chipShape : chipShapes) {
      cpShapeSetFriction(chipShape, friction);
    }
  }

  inline void setDensity(double density) {
    this->density = density;
    updateMass();
  }

  inline double getDensity() {
    return density;
  }

  inline double getMass() {
    return getArea() * density;
  }

  inline double getElasticity() {
    return chipShapes.size() == 0 ? 0 : cpShapeGetElasticity(chipShapes[0]);
  }
  inline double getFriction() {
    return chipShapes.size() == 0 ? 0 : cpShapeGetFriction(chipShapes[0]);
  }

  double getArea() {
    return geomObject->getArea();
  }

  Vec2d getCenterOfMass() {
    return geomObject->getCenterOfMass();
  }


  void checkCatMask() {
		if(chipCollisionFilter.categories == 0 || chipCollisionFilter.categories == (1<<31)) {
			chipCollisionFilter.categories = chipCollisionFilter.categories | 1;
		}
		if(chipCollisionFilter.mask == 0 || chipCollisionFilter.mask == (1<<31)) {
			chipCollisionFilter.mask = chipCollisionFilter.mask | 1;
		}
	}

  void setCollisionFilter(cpShapeFilter filter) {
    chipCollisionFilter = filter;
    checkCatMask();
    for(cpShape *chipShape : chipShapes) {
      cpShapeSetFilter(chipShape, filter);
    }
  }
  void setCollisionGroup(unsigned int group) {
    chipCollisionFilter.group = group;
    for(cpShape *chipShape : chipShapes) {
      cpShapeSetFilter(chipShape, chipCollisionFilter);
    }
  }
  void setCollisionCategories(unsigned int categories) {
    chipCollisionFilter.categories = categories;
    checkCatMask();
    for(cpShape *chipShape : chipShapes) {
      cpShapeSetFilter(chipShape, chipCollisionFilter);
    }
  }
  void setCollisionMask(unsigned int mask) {
		chipCollisionFilter.mask = mask;
    checkCatMask();
    for(cpShape *chipShape : chipShapes) {
      cpShapeSetFilter(chipShape, chipCollisionFilter);
    }
  }

  void toggleCollisionCategory(int index) {
    unsigned int q = chipCollisionFilter.categories;
    chipCollisionFilter.categories = (q & ~(1<<index)) | (~q & (1<<index));
    checkCatMask();
    for(cpShape *chipShape : chipShapes) {
      cpShapeSetFilter(chipShape, chipCollisionFilter);
    }
  }
  void toggleCollisionMask(int index) {
    unsigned int q = chipCollisionFilter.mask;
    chipCollisionFilter.mask = (q & ~(1<<index)) | (~q & (1<<index));
    checkCatMask();
    for(cpShape *chipShape : chipShapes) {
      cpShapeSetFilter(chipShape, chipCollisionFilter);
    }
  }


  void updateMass() {
    if(geomObject->type == GeomObject::polygon) {
      Polygon *p = dynamic_cast<Polygon*>(geomObject);
      if(p->triangles.size() != chipShapes.size()) {
        printf("Error at bodyPart->updateMass(): p->triangles.size() != chipShapes.size(), %lu != %lu", p->triangles.size(), chipShapes.size());
      }
      else {
        for(int i=0; i<chipShapes.size(); i++) {
          cpShapeSetMass(chipShapes[i], p->triangles[i].getArea() * density);
        }
      }
    }
    else {
      if(chipShapes.size() != 1) {
        printf("Error at bodyPart->updateMass(): chipShapes.size() != 1, %lu != 1", chipShapes.size());
      }
      else {
        cpShapeSetMass(chipShapes[0], getArea() * density);
      }
    }
  }

  int getNumShapesInSpace() {
		int n = 0;
		for(cpShape *chipShape : chipShapes) {
      if(cpSpaceContainsShape(chipSpace, chipShape)) {
        n++;
      }
		}
		return n;
	}

  BoundingRect getBoundingRect() {
    if(chipShapes.size() == 0) return BoundingRect();
    cpShapeCacheBB(chipShapes[0]);
    cpBB chipBB = cpShapeGetBB(chipShapes[0]);
    BoundingRect br;
    br.setMinsMaxs(chipBB.l, chipBB.b, chipBB.r, chipBB.t);
    for(int i=1; i<chipShapes.size(); i++) {
      //cpShapeCacheBB(chipShapes[i]);
      chipBB = cpShapeGetBB(chipShapes[i]);
      br.expandMinsMaxs(chipBB.l, chipBB.b, chipBB.r, chipBB.t);
    }
    return br;
  }

  /*void setCollisionFilter(TYPE filter) {
    this->filter = filter;
    cpShapeSetFilter(chipShape, filter);
  }*/

  void render(GeomRenderer &geomRenderer) {
    geomRenderer.setSettings(renderingSettings);
    /*if(!cpSpaceContainsBody(chipSpace, cpShapeGetBody(chipShapes[0]))) {
      geomRenderer.fillColor.set(0, 0, 0, 0.3);
      geomRenderer.strokeColor.set(0, 0, 0, 0.7);
    }*/
    if(texture) {
      geomRenderer.texture = texture;
    }

    //if(renderingSettings.strokeWidth > 3 && renderingSettings.strokeWidth < 10)
    //printf("w %.4f\n", renderingSettings.strokeWidth);

    if(geomObject->type == GeomObject::circle) {
      geomRenderer.drawCircle(*dynamic_cast<Circle*>(geomObject));
    }
    if(geomObject->type == GeomObject::hoop) {
      geomRenderer.drawHoop(*dynamic_cast<Hoop*>(geomObject));
    }
    if(geomObject->type == GeomObject::line) {
      Line *line = dynamic_cast<Line*>(geomObject);
      renderingSettings.strokeWidth = line->bevelRadius*2.0;
      geomRenderer.drawLine(*line);
    }
    if(geomObject->type == GeomObject::triangle) {
      geomRenderer.drawTriangle(*dynamic_cast<Triangle*>(geomObject));
    }
    if(geomObject->type == GeomObject::rect) {
      geomRenderer.drawRect(*dynamic_cast<Rect*>(geomObject));
    }
    if(geomObject->type == GeomObject::polygon) {
      geomRenderer.drawPolygon(*dynamic_cast<Polygon*>(geomObject));
    }

    geomRenderer.texture = NULL;
  }

};
