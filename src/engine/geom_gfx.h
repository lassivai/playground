#pragma once
#include "geometry.h"
#include "glwrap.h"
#include "shape.h"
#include "camera2d.h"

struct GeomRenderer
{
  enum RendererType { None = -1, Basic = 0, FastStrokeRenderer = 1 };

  GlShader shaderGeom, fastStrokeShader;
  Quadx quadGeom;
  Trianglex triangleGeom;

  int screenW, screenH;
  Vec4d fillColor;
  Vec4d strokeColor;
  double strokeWidth = 2;
  int strokeType = 2;
  Texture *texture = NULL;

  Camera2D *camera = NULL;

  struct RenderingSettings {
    Vec4d fillColor = { 1, 1, 1, 0.5 };
    Vec4d strokeColor = { 0, 0, 0, 1.0 };
    double strokeWidth = 2;
    int strokeType = 2;
    Texture *texture = NULL;
  };

  void setSettings(const RenderingSettings &settings) {
    fillColor = settings.fillColor;
    strokeColor = settings.strokeColor;
    strokeWidth = settings.strokeWidth;
    strokeType = settings.strokeType;
    texture = settings.texture;
  }

private:
  int geomType = 0, geomTypePrev = -1;
  double w = 0, h = 0, wPrev = -1, hPrev = -1;
  double innerRadius = 0, innerRadiusPrev = -1;
  Vec4d fillColorPrev;
  Vec4d strokeColorPrev;
  Vec4d fastStrokeColorPrev;
  double strokeWidthPrev = 0;
  int strokeTypePrev = -1;
  double scalePrev = -1;
  //bool shaderActivatedByForce = false;

  Vec4d strokeColorFastPrev;

  RendererType rendererActivated = None;

  inline void initRenderer() {
    if(geomType < 0) {
      if(rendererActivated == None) {
        fastStrokeShader.activate();
      }
      if(strokeColor != fastStrokeColorPrev) {
        fastStrokeShader.setUniform4f("strokeColor", strokeColor);
        fastStrokeColorPrev = strokeColor;
      }
    }
    else {
      if(rendererActivated == None) {
        shaderGeom.activate();
      }

      if(texture != NULL && (geomType == 1 || geomType == 2)) {
        texture->activate(shaderGeom, "tex", 0);
        shaderGeom.setUniform1i("textureType", 1);
      }
      else {
        shaderGeom.setUniform1i("textureType", 0);
      }
      if(geomType != geomTypePrev) {
        shaderGeom.setUniform1i("geomType", geomType);
        geomTypePrev = geomType;
      }
      if(strokeWidth != strokeWidthPrev) {
        double t = strokeWidth;
        /*if(camera) {
          t = max(1.0, strokeWidth * camera->scale);
        }*/
        shaderGeom.setUniform1f("strokeWidth", t);
        strokeWidthPrev = strokeWidth;
      }
      if(strokeType != strokeTypePrev) {
        shaderGeom.setUniform1i("strokeType", strokeType);
        strokeTypePrev = strokeType;
      }
      if(innerRadius != innerRadiusPrev) {
        shaderGeom.setUniform1f("innerRadius", innerRadius);
        innerRadiusPrev = innerRadius;
      }
      if(camera && camera->pushed) {
        if(camera->scale != scalePrev) {
          shaderGeom.setUniform1f("scale", camera->scale);
          scalePrev = camera->scale;
        }
      }
      else if(scalePrev != 1.0) {
        shaderGeom.setUniform1f("scale", 1.0);
        scalePrev = 1.0;
      }
      if(strokeColor != strokeColorPrev) {
        shaderGeom.setUniform4f("strokeColor", strokeColor);
        strokeColorPrev = strokeColor;
      }
      if(fillColor != fillColorPrev) {
        shaderGeom.setUniform4f("fillColor", fillColor);
        fillColorPrev = fillColor;
      }

      if(w != wPrev || h != hPrev) {
        /*if(camera) {
          shaderGeom.setUniform2f("objectSize", w*camera->scale, h*camera->scale);
        }
        else {
        }*/
        shaderGeom.setUniform2f("objectSize", w, h);

        quadGeom.setSize(w, h);
        wPrev = w;
        hPrev = h;
      }
    }
  }

  inline void finalizeRenderer() {
    if(texture != NULL && (geomType == 1 || geomType == 2)) {
      texture->inactivate(0);
    }
    if(rendererActivated == None) {
      if(geomType < 0) {
        fastStrokeShader.deActivate();
      }
      else  {
        shaderGeom.deActivate();
      }
    }
  }

public:

  inline void startRendering(RendererType rendererType = Basic) {
    rendererActivated = rendererType;

    if(rendererType == Basic) {
      shaderGeom.activate();
    }
    if(rendererType == FastStrokeRenderer) {
      fastStrokeShader.activate();
      /*fastStrokeShader.setUniform4f("strokeColor", strokeColor);
      strokeColorPrev = strokeColor;*/
    }
  }

  inline void endRendering() {
    if(rendererActivated == Basic) {
      shaderGeom.deActivate();
    }
    if(rendererActivated == FastStrokeRenderer) {
      fastStrokeShader.deActivate();
    }
    rendererActivated = None;
  }


  bool create() {
    fillColorPrev.set(-1, -1, -1, -1);
    strokeColorPrev.set(-1, -1, -1, -1);
    fillColor.set(0, 0, 0, 0.5);
    strokeColor.set(1, 1, 1, 0.5);
    shaderGeom.create("data/glsl/texture.vert", "data/glsl/geom.frag");
    fastStrokeShader.create("data/glsl/texture.vert", "data/glsl/stroke.frag");
    quadGeom.create(1, 1, false);
    triangleGeom.create(0, 0, 0, 0, 0, 0, false);
    return shaderGeom.readyToUse;
  }

  void reloadShaders() {
    shaderGeom.create("data/glsl/texture.vert", "data/glsl/geom.frag");
  }


  void draw(GeomObject *geomObject) {
    if(geomObject->type == GeomObject::circle) {
      drawCircle(*dynamic_cast<Circle*>(geomObject));
    }
    if(geomObject->type == GeomObject::line) {
      drawLine(*dynamic_cast<Line*>(geomObject));
    }
    if(geomObject->type == GeomObject::triangle) {
      drawTriangle(*dynamic_cast<Triangle*>(geomObject));
    }
    if(geomObject->type == GeomObject::rect) {
      drawRect(*dynamic_cast<Rect*>(geomObject));
    }
    if(geomObject->type == GeomObject::polygon) {
      drawPolygon(*dynamic_cast<Polygon*>(geomObject));
    }
    if(geomObject->type == GeomObject::hoop) {
      drawHoop(*dynamic_cast<Hoop*>(geomObject));
    }
  }

  inline void drawCircle(double r, const Vec2d &pos, double rot = 0) {
    drawCircle(Circle(r, pos.x, pos.y, rot));
  }
  inline void drawCircle(double r, double x = 0, double y = 0, double rot = 0) {
    drawCircle(Circle(r, x, y, rot));
  }
  void drawCircle(const Circle &circle, double dx = 0, double dy = 0, double rot = 0) {
    geomType = 1;
    w =  circle.r*2.0;
    if(camera && camera->pushed) {
      w += 2.0 / camera->scale;
      if(w * camera->scale < 2.0) w = 2.0 / camera->scale;
    }
    h = w;

    initRenderer();
    quadGeom.render(circle.pos.x + dx, circle.pos.y + dy, circle.rot + rot);
    finalizeRenderer();
  }

  inline void drawHoop(double r, double r2, const Vec2d &pos, double rot = 0) {
    drawHoop(Hoop(r, r2, pos.x, pos.y, rot));
  }
  inline void drawHoop(double r, double r2, double x = 0, double y = 0, double rot = 0) {
    drawHoop(Hoop(r, r2, x, y, rot));
  }
  void drawHoop(const Hoop &hoop, double dx = 0, double dy = 0, double rot = 0) {
    geomType = 5;
    w =  hoop.r*2.0;
    if(camera && camera->pushed) {
      w += 2.0 / camera->scale;
      if(w * camera->scale < 2.0) w = 2.0 / camera->scale;
    }
    h = w;
    innerRadius = hoop.r2 / hoop.r;

    initRenderer();
    quadGeom.render(hoop.pos.x + dx, hoop.pos.y + dy, hoop.rot + rot);
    finalizeRenderer();
  }

  inline void drawRect(const BoundingRect &br, double x = 0, double y = 0, double rot = 0) {
    double w = br.b.x - br.a.x;
    double h = br.b.y - br.a.y;
    x = x + br.a.x + 0.5*w;
    y = y + br.a.y + 0.5*h;
    drawRect(Rect(w, h, x, y, rot));
  }
  inline void drawRect(const Vec2d &size, const Vec2d &pos, double rot = 0) {
    drawRect(Rect(size.x, size.y, pos.x, pos.y, rot));
  }
  inline void drawRect(double w, double h, const Vec2d &pos, double rot = 0) {
    drawRect(Rect(w, h, pos.x, pos.y, rot));
  }
  inline void drawRect(double w, double h, double x = 0, double y = 0, double rot = 0) {
    drawRect(Rect(w, h, x, y, rot));
  }

  inline void drawRectCorners(double x0, double y0, double x1, double y1) {
    double w = fabs(x1 - x0);
    double h = fabs(y1 - y0);
    double x = min(x0, x1) + w * 0.5;
    double y = min(y0, y1) + h * 0.5;
    drawRect(Rect(w, h, x, y));
  }
  void drawRectCorner(const Vec2d &size, const Vec2d &pos, double rot = 0) {
    drawRect(Rect(size.x, size.y, pos.x + size.x/2, pos.y + size.y/2, rot));
  }
  void drawRectCorner(const Rect &rect, const Vec2d &d, double rot = 0) {
    drawRect(rect, d.x + rect.w/2, d.y + rect.h/2, rot);
  }
  
  void drawRectCorner(const Rect &rect, double dx = 0, double dy = 0, double rot = 0) {
    drawRect(rect, dx + rect.w/2, dy + rect.h/2, rot);
  }
  void drawRectCorner(double w, double h, double x, double y, double rot = 0, double scale = 1) {
    drawRect(Rect(w, h), x + w/2, y + h/2, rot, scale);
  }
  void drawRect(const Rect &rect, const Vec2d &d, double rot = 0, double scale = 1) {
    drawRect(rect, d.x, d.y, rot, scale);
  }
  void drawRect(const Rect &rect, double dx = 0, double dy = 0, double rot = 0, double scale = 1) {
    geomType = 2;
    w = rect.w * scale;
    h = rect.h * scale;
    if(camera && camera->pushed) {
      w += 2.0 / camera->scale;
      h += 2.0 / camera->scale;
      if(w * camera->scale < 2.0) w = 2.0 / camera->scale;
      if(h * camera->scale < 2.0) h = 2.0 / camera->scale;
    }

    initRenderer();
    quadGeom.render(rect.pos.x + dx, rect.pos.y + dy, rect.rot + rot);
    finalizeRenderer();
  }
  
  //double zLayer = 0;
  
  /*void setZLayer(double z) {
    zLayer = z;
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslated(0, 0, zLayer);
    //glDepthFunc(GL_LESS); 
  }
  void resetZLayer() {
    zLayer = 0;
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    //glDepthFunc(GL_ALWAYS); 
  }*/
  /*void drawRectZCorner(const Vec2d &size, const Vec3d &pos, double rot = 0) {
    drawRect(Rect(size.x, size.y, pos.x + size.x/2, pos.y + size.y/2, pos.z rot));
  }
  void drawRectZCorner(const Vec2d &size, const Vec3d &pos, double rot = 0) {
    drawRect(Rect(size.x, size.y, pos.x + size.x/2, pos.y + size.y/2, pos.z rot));
  }
  void drawRectZ(const Rect &rect, double dx = 0, double dy = 0, double dz = 0, double rot = 0, double scale = 1) {
    geomType = 2;
    w = rect.w * scale;
    h = rect.h * scale;
    if(camera && camera->pushed) {
      w += 2.0 / camera->scale;
      h += 2.0 / camera->scale;
      if(w * camera->scale < 2.0) w = 2.0 / camera->scale;
      if(h * camera->scale < 2.0) h = 2.0 / camera->scale;
    }

    initRenderer();
    quadGeom.render(rect.pos.x + dx, rect.pos.y + dy, rect.rot + rot);
    finalizeRenderer();
  }*/
  
  inline void drawRectNonAntialized(const Vec2d &size, const Vec2d &pos, double rot = 0) {
    drawRect(Rect(size.x, size.y, pos.x, pos.y, rot));
  }
  void drawRectNonAntialized(const Rect &rect, double dx = 0, double dy = 0, double rot = 0, double scale = 1) {
    geomType = 4;
    w = rect.w * scale;
    h = rect.h * scale;
    if(camera && camera->pushed) {
      w += 2.0 / camera->scale;
      h += 2.0 / camera->scale;
      if(w * camera->scale < 2.0) w = 2.0 / camera->scale;
      if(h * camera->scale < 2.0) h = 2.0 / camera->scale;
    }

    initRenderer();
    quadGeom.render(rect.pos.x + dx, rect.pos.y + dy, rect.rot + rot);
    finalizeRenderer();
  }

  inline void drawLine(const Vec2d &a, const Vec2d &b, const Vec2d &d, double rot = 0) {
    if(rendererActivated == 1) {
      drawLineFast(Line(a, b), d.x, d.y, rot);
    }
    else {
      drawLine(Line(a, b), d.x, d.y, rot);
    }
  }

  inline void drawLine(const Vec2d &a, const Vec2d &b, double dx = 0, double dy = 0, double rot = 0) {
    if(rendererActivated == 1) {
      drawLineFast(Line(a, b), dx, dy, rot);
    }
    else {
      drawLine(Line(a, b), dx, dy, rot);
    }
  }

  inline void drawLine(double x0, double y0, double x1, double y1, const Vec2d &d, double rot = 0) {
    if(rendererActivated == 1) {
      drawLineFast(Line(x0, y0, x1, y1), d.x, d.y, rot);
    }
    else {
      drawLine(Line(x0, y0, x1, y1), d.x, d.y, rot);
    }
  }

  inline void drawLine(double x0, double y0, double x1, double y1, double dx = 0, double dy = 0, double rot = 0) {
    if(rendererActivated == 1) {
      drawLineFast(Line(x0, y0, x1, y1), dx, dy, rot);
    }
    else {
      drawLine(Line(x0, y0, x1, y1), dx, dy, rot);
    }
  }

  void drawLine(const Line &line, double dx = 0, double dy = 0, double rot = 0) {
    Circle cl = line.getAlignedCircle();
    geomType = 3;
    w = cl.r*2.0 + strokeWidth;
    h = strokeWidth * 2.0 + 4.0;

    if(camera && camera->pushed) { // FIXME 4 would be enough perhaps
      if(w * camera->scale < 6.0) w = 6.0 / camera->scale;
      if(h * camera->scale < 6.0) h = 6.0 / camera->scale;
    }

    initRenderer();
    quadGeom.render(cl.pos.x+dx, cl.pos.y+dy, cl.rot+rot);
    finalizeRenderer();
  }


  inline void drawLineFast(const Vec2d &a, const Vec2d &b, double dx = 0, double dy = 0, double rot = 0) {
    drawLineFast(Line(a, b), dx, dy, rot);
  }

  inline void drawLineFast(double x0, double y0, double x1, double y1, double dx = 0, double dy = 0, double rot = 0) {
    drawLineFast(Line(x0, y0, x1, y1), dx, dy, rot);
  }
  void drawLineFast(const Line &line, double dx = 0, double dy = 0, double rot = 0) {
    Circle cl = line.getAlignedCircle();
    geomType = -1;
    w = cl.r*2.0;
    h = strokeWidth;

    /*if(camera && camera->pushed) { // FIXME 4 would be enough perhaps
      if(w * camera->scale < 6.0) w = 6.0 / camera->scale;
      if(h * camera->scale < 6.0) h = 6.0 / camera->scale;
    }*/
    quadGeom.setSize(w, h);

    initRenderer();
    quadGeom.render(cl.pos.x+dx, cl.pos.y+dy, cl.rot+rot);
    finalizeRenderer();
  }



  // FIXME get rid of these
  inline void drawLine2(const Vec2d &a, const Vec2d &b) {
    drawLine2(Line(a, b));
  }

  inline void drawLine2(double x0, double y0, double x1, double y1) {
    drawLine2(Line(x0, y0, x1, y1));
  }
  void drawLine2(const Line &line, double dx = 0, double dy = 0, double rot = 0) {
    Circle cl = line.getAlignedCircle();
    geomType = 3;
    w = cl.r*2.0 + strokeWidth*0.5;
    h = strokeWidth*2 + 4.0;
    if(camera && camera->pushed) {
      if(w * camera->scale < 6.0) w = 6.0 / camera->scale;
      if(h * camera->scale < 6.0) h = 6.0 / camera->scale;
    }

    initRenderer();

    shaderGeom.setUniform1f("strokeWidth", strokeWidth*0.5);
    strokeWidthPrev = strokeWidth*0.5;
    quadGeom.render(cl.pos.x+dx, cl.pos.y+dy, cl.rot+rot);
    finalizeRenderer();
  }



  void drawTriangle(const Triangle &t, double x = 0, double y = 0, double rot = 0) {
    triangleGeom.set(t);
    geomType = 4;
    initRenderer();

    glPushMatrix();
    glTranslated(x, y, 0);
    glRotatef(rot*180.0/PI, 0, 0, 1);
    //glScalef(scaleX, scaleY, scaleZ);
    triangleGeom.render(0, 0, 0);

    if(strokeType > 0) {
      drawLine2(t.a, t.b);
      drawLine2(t.b, t.c);
      drawLine2(t.c, t.a);
    }

    glPopMatrix();

    finalizeRenderer();
  }

  void drawPolygon(std::vector<Vec2d> &v, double x = 0, double y = 0, double rot = 0) {
    Polygon poly;
    poly.vertices = v;
    drawPolygon(poly, x, y, rot);
  }

  void drawPolygon(Shape &s, double x = 0, double y = 0, double rot = 0) {
    Polygon poly;
    for(int i=0; i<s.curves.size(); i++) {
      poly.vertices = s.curves[i].points;
      drawPolygon(poly, x, y, rot);
    }
  }


  void drawPolygon(Polygon &p, double x = 0, double y = 0, double rot = 0) {
    std::vector<Triangle> tris = p.triangulateWithHoles();
    if(tris.size() == 0) return;

    geomType = 4;

    glPushMatrix();
    glTranslated(x, y, 0);
    glRotatef(rot*180.0/PI, 0, 0, 1);

    if(!debugTriangulation) initRenderer();


    for(int i=0; i<tris.size(); i++) {
      Triangle &t = tris[i];
      triangleGeom.set(t);
      triangleGeom.render(0, 0, 0);
    }

    if(debugTriangulation) initRenderer();

    if(strokeType > 0) {
      for(int i=0; i<p.vertices.size(); i++) {
        Vec2d a = p.vertices[i];
        Vec2d b = p.vertices[(i+1) % p.vertices.size()];
        drawLine2(a, b);
      }
      for(int h=0; h<p.holes.size(); h++) {
        for(int i=0; i<p.holes[h].size(); i++) {
          Vec2d a = p.holes[h][i];
          Vec2d b = p.holes[h][(i+1) % p.holes[h].size()];
          drawLine2(a, b);
        }
      }
    }

    glPopMatrix();

    finalizeRenderer();
  }



};
