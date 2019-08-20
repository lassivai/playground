#pragma once

#include <vector>
#include "cereal_init.h"
#include "mathl.h"
#include "geometry.h"
#include "event.h"
#include "gfxwrap.h"



struct Curve
{
  std::vector<Vec2d> points;
  bool ring = false;

  Curve() {}

  Curve(const Curve &curve) {
    this->points = std::vector<Vec2d>(curve.points);
    this->ring = curve.ring;
  }

  int size() {
    return points.size();
  }

  void clear() {
    points.clear();
  }

  template<class Archive>
  void serialize(Archive &ar) {
    ar(points, ring);
  }
};

struct Shape
{
  static constexpr double selectDistance = 0.02;
  std::vector<Curve> curves;
  int activeCurveInd = -1;
  int activePointInd = -1;
  BoundingRect boundingRect;
  int numSegments = 100;

  int maxCurves = 1<<30;

  void print() {
    if(curves.size() == 0) return;
    Vec2d center = (boundingRect.a + boundingRect.b) * 0.5;
    for(int i=0; i<curves[0].size(); i++) {
      Vec2d p = curves[0].points[i] - center;
      printf("%.2f, %.2f\n", p.x*20, p.y*20);
    }
  }

  template<class Archive>
  void save(Archive &ar) const {
    ar(curves, activeCurveInd, activePointInd);
  }
  template<class Archive>
  void load(Archive &ar) {
    ar(curves, activeCurveInd, activePointInd);
    updateBoundingRect();
  }

  void updateBoundingRect() {
    if(curves.size() < 1 || curves[0].points.size() < 1) {
      boundingRect.clear();
      return;
    }
    boundingRect.set(curves[0].points);
    for(int i=1; i<curves.size(); i++) {
      boundingRect.expand(curves[i].points);
    }
  }

  Shape() {
  }


  void toggleRing() {
    if(curves.size() == 0 || activeCurveInd < 0) return;
    curves[activeCurveInd].ring = !curves[activeCurveInd].ring;
  }

  void addPoint(Vec2d point) {
    if(activeCurveInd < 0) return;
    if(activeCurveInd >= curves.size()) {
      newCurve();
    }
    curves[activeCurveInd].points.push_back(point);
    updateBoundingRect();
    activePointInd = curves[activeCurveInd].points.size() - 1;
  }

  void insertPoint(Vec2d point) {
    if(activeCurveInd < 0) return;

    curves[activeCurveInd].points.insert(curves[activeCurveInd].points.begin() + activePointInd, point);
    updateBoundingRect();
    //activePointInd = curves[activeCurveInd].points.size() - 1;
  }

  void removeLastPoint() {
    if(curves.size() == 0 || activeCurveInd < 0 || curves[activeCurveInd].size() == 0) {
      return;
    }
    curves[activeCurveInd].points.erase(curves[activeCurveInd].points.end());
    updateBoundingRect();
  }

  void removeActivePoint() {
    if(activeCurveInd >= curves.size() || activeCurveInd < 0 || activePointInd >= curves[activeCurveInd].size() || activePointInd < 0) {
      return;
    }
    curves[activeCurveInd].points.erase(curves[activeCurveInd].points.begin()+activePointInd);
    updateBoundingRect();
  }

  void newCurve() {
    if(curves.size() < maxCurves) {
      curves.push_back(Curve());
      activeCurveInd = curves.size() - 1;
    }
  }

  void selectPoint(const Vec2d &pos) {
    if(curves.size() == 0 || activeCurveInd < 0 || curves[activeCurveInd].size() == 0) return;
    int closestPointInd = -1;
    double closestPointDist = pow(2, 60);
    for(int i=0; i<curves[activeCurveInd].size(); i++) {
      double d = distance(pos, curves[activeCurveInd].points[i]);
      if(d < closestPointDist && d < selectDistance) {
        closestPointInd = i;
        closestPointDist = d;
      }
    }
    activePointInd = closestPointInd;
  }

  void removeCurve() {
    if(activeCurveInd < 0) return;
    if(activeCurveInd < curves.size()) {
      curves.erase(curves.begin()+activeCurveInd);
    }
    if(activeCurveInd > 0 && activeCurveInd == curves.size()) {
      activeCurveInd--;
    }
    updateBoundingRect();
  }


  void clear() {
    curves.clear();
    activeCurveInd = 0;
    activePointInd = -1;
    updateBoundingRect();
  }


  void selectNextCurve() {
    if(curves.size() == 0) return;
    activeCurveInd++;
    if(activeCurveInd >= curves.size()) {
      activeCurveInd = 0;
    }
  }

  void selectPreviousCurve() {
    if(curves.size() == 0) return;
    activeCurveInd--;
    if(activeCurveInd < 0) {
      activeCurveInd = curves.size()-1;
    }
  }

  void selectNextPoint() {
    if(curves.size() == 0 || curves[activeCurveInd].size() == 0) return;
    activePointInd++;
    if(activePointInd >= curves[activeCurveInd].size()) {
      activePointInd = 0;
    }
  }
  void selecPreviousPoint() {
    if(curves.size() == 0 || curves[activeCurveInd].size() == 0) return;
    activePointInd--;
    if(activePointInd < 0) {
      activePointInd = curves[activeCurveInd].size()-1;
    }
  }

  void translateActivePoint(const Vec2d &t) {
    if(curves.size() == 0 || activeCurveInd < 0 || curves[activeCurveInd].size() == 0 || activePointInd < 0) {
      return;
    }
    curves[activeCurveInd].points[activePointInd] += t;
    updateBoundingRect();
  }

  void translateCurve(const Vec2d &t) {
    if(curves.size() == 0 || activeCurveInd < 0 || curves[activeCurveInd].size() == 0) return;
    for(int i=0; i<curves[activeCurveInd].size(); i++) {
      curves[activeCurveInd].points[i] += t;
    }
    updateBoundingRect();
  }

  void translateShape(const Vec2d &t) {
    if(curves.size() == 0) return;
    for(int i=0; i<curves.size(); i++) {
      for(int p=0; p<curves[i].size(); p++) {
        curves[i].points[p] += t;
      }
    }
    updateBoundingRect();
  }

  void scaleShape(const double &s) {
    if(curves.size() == 0) return;
    for(int i=0; i<curves.size(); i++) {
      for(int p=0; p<curves[i].size(); p++) {
        curves[i].points[p] *= s;
      }
    }
    updateBoundingRect();
  }

  void duplicateCurve() {
    if(curves.size() == 0 || activeCurveInd < 0 || curves[activeCurveInd].size() == 0 || curves.size() >= maxCurves) return;
    int newCurveInd = activeCurveInd+1;
    curves.insert(curves.begin()+activeCurveInd+1, Curve());
    Vec2d t(0.1, 0.1);
    for(int i=0; i<curves[activeCurveInd].size(); i++) {
      curves[newCurveInd].points.push_back(curves[activeCurveInd].points[i] + t);
    }
    updateBoundingRect();
  }

  bool isPointSelected() {
    return curves.size() != 0 && activeCurveInd >= 0 && curves[activeCurveInd].size() != 0 && activePointInd >= 0;
  }

  void renderBoundingRect(Scene *scene, SDLInterface *sdlInterface) {
    setColor(0.3, 0.0, 0.1, 1.0, sdlInterface);
    boundingRect.render(scene, sdlInterface);
  }

  void renderCurves(SDLInterface *sdlInterface) {
    for(int i=0; i<curves.size(); i++) {
      if(i == activeCurveInd) {
        setColor(0.75, 0.0, 0.4, 1.0, sdlInterface);
      }
      else {
        setColor(0.6, 0.0, 0.3, 1.0, sdlInterface);
      }
      if(curves[i].size() > 1) {
        for(int p=1; p<curves[i].size(); p++) {
          drawLine(curves[i].points[p-1], curves[i].points[p], sdlInterface);
        }
      }
      if(curves[i].ring && curves[i].size() > 2) {
        drawLine(curves[i].points[0], curves[i].points[curves[i].size()-1], sdlInterface);
      }
    }
  }

  int dragMode = 0;


  void onKeyDown(Events &events, Scene *scene) {
    if(events.sdlKeyCode == SDLK_d && events.noModifiers) {
      activeCurveInd = -1;
    }
    if(events.sdlKeyCode == SDLK_c && events.noModifiers) {
      newCurve();
    }
    if(events.sdlKeyCode == SDLK_c && events.lShiftDown) {
      removeCurve();
    }
    if(events.sdlKeyCode == SDLK_c && events.lControlDown) {
      duplicateCurve();
    }
  }

  void onMouseWheel(Events &events, Scene *scene) {
    if(events.mouseWheel > 0 && events.noModifiers) {
      selectNextCurve();
    }
    if(events.mouseWheel < 0 && events.noModifiers) {
      selectPreviousCurve();
    }
  }


  void onMousePressed(Events &events, Scene *scene) {
    dragMode = 0;

    if(events.mouseDownL && events.noModifiers) {
      if(isPointSelected()) {
        dragMode = 1;
      }
      else {
        if(curves.size() == 0) newCurve();
        addPoint(scene->toWorld(events.m));
        dragMode = 1;
      }
    }
    if(events.mouseDownL && events.lAltDown && events.numModifiersDown == 1) {
      if(isPointSelected()) {
        insertPoint(scene->toWorld(events.m));
        dragMode = 1;
      }
    }
    if(events.mouseDownL && events.lControlDown && events.numModifiersDown == 1) {
      dragMode = 2;
    }
    if(events.mouseDownL && events.lShiftDown && events.numModifiersDown == 1) {
      newCurve();
      addPoint(scene->toWorld(events.m));
    }
    if(events.mouseDownM && events.lControlDown && events.lAltDown && events.numModifiersDown == 2) {
      removeCurve();
    }
    if(events.mouseDownL && events.lShiftDown && events.lAltDown && events.numModifiersDown == 2) {
      duplicateCurve();
    }
    if(events.mouseDownM && events.noModifiers) {
      removeActivePoint();
    }
    if(events.mouseDownM && events.lControlDown && events.numModifiersDown == 1) {
      removeLastPoint();
    }
    if(events.mouseDownR && events.noModifiers) {
      toggleRing();
    }
  }

  void onMouseReleased(Events &events, Scene *scene) {
    dragMode = 0;
  }

  void onMouseMotion(Events &events, Scene *scene) {
    if(dragMode == 0) {
      selectPoint(scene->toWorld(events.m));
    }

    if(dragMode == 1) {
      Vec2d t = scene->toWorld(events.m) - scene->toWorld(events.mp);
      translateActivePoint(t);
    }

    if(dragMode == 2) {
      Vec2d t = scene->toWorld(events.m) - scene->toWorld(events.mp);
      translateCurve(t);
    }
  }



  void renderPoints(Scene *scene, SDLInterface *sdlInterface) {
    for(int i=0; i<curves.size(); i++) {
      if(i == activeCurveInd) {
        setColor(0.75, 0.0, 0.4, 1.0, sdlInterface);
      }
      else {
        setColor(0.6, 0.0, 0.3, 1.0, sdlInterface);
      }
      for(int p=0; p<curves[i].size(); p++) {
        if(i == activeCurveInd && p == activePointInd) {
          setColor(1.0, 0.3, 0.8, 1.0, sdlInterface);
        }
        else {
        setColor(0.3, 0.0, 0.1, 1.0, sdlInterface);
      }
        //fillRect(curves[i][p].toScreen(sdlInterface), 6, sdlInterface);
        fillRect(scene->toScreen(curves[i].points[p]), 6, sdlInterface);
      }
    }
  }
  void renderCurvesSmooth(Scene *scene, SDLInterface *sdlInterface) {
    for(int i=0; i<curves.size(); i++) {
      if(i == activeCurveInd) {
        setColor(0.75, 0.0, 0.4, 1.0, sdlInterface);
      }
      else {
        setColor(0.6, 0.0, 0.3, 1.0, sdlInterface);
      }
      int n = curves[i].size();
      if(n > 1) {
        if(n > 2 && curves[i].ring) {
          for(int p=0; p<n; p++) {
            Vec2d p0 = curves[i].points[(p+n-1) % n];
            Vec2d p1 = curves[i].points[p];
            Vec2d p2 = curves[i].points[(p+1) % n];
            Vec2d p3 = curves[i].points[(p+2) % n];

            Vec2d pp = p1;
            for(int k=1; k<numSegments; k++) {
              double f = (double)k/(numSegments-1.0);
              Vec2d pn = catmullRom(f, 0.5, p0, p1, p2, p3);
              drawLine(scene->toScreen(pp), scene->toScreen(pn), sdlInterface);
              pp = pn;
            }
          }
        }
        else {
          for(int p=0; p<n-1; p++) {
            Vec2d p0 = curves[i].points[max(p-1, 0)];
            Vec2d p1 = curves[i].points[p];
            Vec2d p2 = curves[i].points[min(p+1, n-1)];
            Vec2d p3 = curves[i].points[min(p+2, n-1)];

            Vec2d pp = p1;
            for(int k=1; k<numSegments; k++) {
              double f = (double)k/(numSegments-1.0);
              Vec2d pn = catmullRom(f, 0.5, p0, p1, p2, p3);
              drawLine(scene->toScreen(pp), scene->toScreen(pn), sdlInterface);
              pp = pn;
            }
          }
        }
      }
    }
  }
};
