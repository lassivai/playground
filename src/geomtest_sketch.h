#pragma once
#include "engine/sketch.h"



struct GeomTestSketch : public Sketch
{
  FastNoise fastNoise;

  GlShader shader, shaderPost;
  Texture texture, texture2, texture3;

  Quadx quadx, quadScreen;

  GlShader shaderGeom;
  Quadx quadGeom;
  Texture texture4;

  Trianglex trianglex;

  double scale = 1.0;
  double rotation = 0.0;

  //Polygon poly;

  GeomRenderer geomRenderer;

  Shape shape;

  void onInit() {

    scaleBind->active = true;
    positionBind->active = true;

    Events::InputVariableBind<double> *scaleBind = new Events::InputVariableBind<double>(&scale, Events::dragModeExponential, 0.02, Events::buttonRight, 0);
    Events::InputVariableBind<double> *rotationBind = new Events::InputVariableBind<double>(&rotation, Events::dragModeLinear, 0.2, Events::buttonMiddle, 0);

    events.inputBoundDoubles.push_back(scaleBind);
    events.inputBoundDoubles.push_back(rotationBind);

    glSetup(sdlInterface);

    fastNoise.SetSeed(64573);
    fastNoise.SetNoiseType(FastNoise::SimplexFractal);


    shaderGeom.create("data/glsl/texture.vert", "data/glsl/geom.frag");
    if(!shaderGeom.readyToUse) events.quitRequested = true;

    quadGeom.create(1, 1, false);

    texture4.load("data/losing_my_religion2.png");

    shader.create("data/glsl/texture.vert", "data/glsl/texture2.frag");
    shaderPost.create("data/glsl/texture.vert", "data/glsl/texture3.frag");

    texture.load("data/my_new_religion.png");
    texture2.load("data/textures/0001.png");
    /*texture.create(256, 256);
    for(int i=0; i<texture.w; i++) {
      for(int j=0; j<texture.h; j++) {
        double x = map(i, 0, texture.w, -1.0, 1.0);
        double y = map(j, 0, texture.h, -1.0, 1.0);
        double r = 1.0 - clamp(sqrt(x*x + y*y), 0.0, 1.0);
        //double r = (double)i/texture.w;
        //double r = 1.0;
        texture.setPixel(i, j, r, r, r, 1.0);
      }
    }
    texture.applyPixels();*/
    //img.load("data/losing_my_religion2.png", sdlInterface);

    //poly.create(9, 50, 150);

    trianglex.create(0, 0, 0, 0, 0, 0);

    geomRenderer.create();

    quadx.create(texture.w, texture.h, false);

    texture3.create(screenW, screenH);
    quadScreen.create(screenW, screenH);


    clear(0, 0, 0, 1);
    updateScreen(sdlInterface);

  }



  void onUpdate() {
    for(int i=0; i<commandQueue.commands.size(); i++) {
      Command *cmd = NULL;
    }
  }


  void onQuit() {
  }

  void onKeyUp() {
  }

  void onKeyDown() {
    shape.onKeyDown(events, scene);
  }

  void onMouseWheel() {
    shape.onMouseWheel(events, scene);
  }

  void onMousePressed() {
    shape.onMousePressed(events, scene);
    //poly.create(map(events.mouseX, 0 , screenW-1, 3, 20), 40, 200);
  }

  void onMouseReleased() {
    shape.onMouseReleased(events, scene);
  }

  void onMouseMotion() {
    shape.onMouseMotion(events, scene);
  }

  void onReloadShaders() {
    shaderGeom.create("data/glsl/texture.vert", "data/glsl/geom.frag");
    shader.create("data/glsl/texture.vert", "data/glsl/texture2.frag");
    shaderPost.create("data/glsl/texture.vert", "data/glsl/texture3.frag");
  }


  void onDraw() {
    //texture3.setRenderTarget();

    clear(0.18, 0, 0.05, 1);

    glLoadIdentity();

    geomRenderer.strokeWidth = 5;
    geomRenderer.strokeType = 1;
    geomRenderer.strokeColor.set(0, 0, 0, 0.25);
    geomRenderer.texture = &texture4;

    int n = 60;
    for(int i=0; i<n; i++) {
      double x = map(i, 0, n-1, 10, screenW-10);
      double y = map(fastNoise.GetNoise(i*1054.435, time*1), -1.0, 1.0, 10, screenH-10);
      double w = 250.0 * (fastNoise.GetNoise(i*654.4375, time*10)*0.5+0.5);
      double h = 250.0 * (fastNoise.GetNoise(i*164.4375, time*10)*0.5+0.5);
      double rot = fastNoise.GetNoise(i*1054.435, time*2.0) * 10.0;
      geomRenderer.fillColor.set(fastNoise.GetNoise(-i*105.6435, time*10) * 0.5 + 0.5,
                                fastNoise.GetNoise(i*16.4375, time*10) * 0.5 + 0.5,
                                fastNoise.GetNoise(i*634.35, time*10) * 0.5 + 0.5, 0.25);
      if(i % 2 == 0) geomRenderer.drawRect(w, h, x, y, rot);
      else geomRenderer.drawCircle(w*0.5, x, y, rot);
    }


    Line line1(noisetn(5, 5)*screenW, noisetn(5, 6)*screenH, noisetn(5, 7)*screenW, noisetn(5, 8)*screenH);
    Line line2(noisetn(5, 9)*screenW, noisetn(5, 10)*screenH, noisetn(5, 11)*screenW, noisetn(5, 12)*screenH);

    Vec2d intersectionPoint;
    bool intersects = getIntersectionPoint(line1, line2, intersectionPoint);

    geomRenderer.texture = NULL;
    geomRenderer.strokeColor.set(0, 0, 0, 0.4);
    geomRenderer.strokeWidth = 5;
    geomRenderer.strokeType = 1;
    geomRenderer.fillColor.set(0, 0.7, 0, 0.25);
    geomRenderer.drawCircle(20, line1.a);
    geomRenderer.drawCircle(15, line1.b);
    geomRenderer.drawCircle(15, line2.a);
    geomRenderer.drawCircle(15, line2.b);

    geomRenderer.strokeWidth = isCCW(line1.a, line1.b, events.m) ? 10 : 5;
    geomRenderer.drawLine(line1);

    geomRenderer.strokeWidth = 5;
    geomRenderer.drawLine(line2);

    Vec2d normal = line1.b - line1.a;
    normal = Vec2d(-normal.y, normal.x);
    normal.setLength(120);
    geomRenderer.strokeColor.set(0, 0, 0.6, 0.4);
    geomRenderer.drawLine(line1.a, line1.a+normal);



    geomRenderer.strokeColor.set(0, 0, 0, 0.4);

    if(intersects) geomRenderer.fillColor.set(0.7, 0, 0, 0.25);
    else geomRenderer.fillColor.set(0, 0, 0.8, 0.25);


    geomRenderer.drawCircle(15, intersectionPoint);


    Triangle tri(noiset(10, 1)*500, noiset(10, 2)*500, noiset(10, 3)*500, noiset(10, 4)*500, noiset(10, 5)*500, noiset(10, 6)*500);
    //geomRenderer.fillColor.set(1, 1, 1, 1);
    geomRenderer.strokeType = 0;
    geomRenderer.fillColor.set(noisetn(10, 10), noisetn(10, 11), noisetn(10, 12), 0.5);
    //glTranslated(0, 0, -10);
    //geomRenderer.setZLayer(-10);
    geomRenderer.drawTriangle(tri, screenW/2, screenH/2, time);
    //geomRenderer.resetZLayer();
    //glTranslated(0, 0, 10);

    tri.set(noiset(5, 20)*500, noiset(5, 21)*500, noiset(5, 22)*500, noiset(5, 23)*500, noiset(5, 24)*500, noiset(5, 25)*500);
    //geomRenderer.fillColor.set(1, 1, 1, 1);
    Vec2d triPos(screenW/2, screenH/2);
    geomRenderer.fillColor.set(noisetn(10, 11), noisetn(10, 12), noisetn(10, 13), 0.5);
    geomRenderer.strokeType = 1;
    geomRenderer.strokeWidth = isPointWithinTriangle(events.m-triPos, tri.a, tri.b, tri.c) ? 15 : 5;
    geomRenderer.strokeColor.set(0, 0, 0, 1);
    //glTranslated(0, 0, 10);
    //geomRenderer.setZLayer(10);
    geomRenderer.drawTriangle(tri, triPos.x, triPos.y, 0);
    //geomRenderer.resetZLayer();
    //glTranslated(0, 0, -10);

    geomRenderer.drawCircle(10, triPos + tri.getCenterOfMass());

    Vec2d abm = tri.a + (tri.b-tri.a) * 0.5;
    Vec2d bcm = tri.b + (tri.c-tri.b) * 0.5;
    Vec2d cam = tri.c + (tri.a-tri.c) * 0.5;
    geomRenderer.strokeWidth = 1.5;
    geomRenderer.drawLine(triPos + abm, triPos + tri.c);
    geomRenderer.drawLine(triPos + bcm, triPos + tri.a);
    geomRenderer.drawLine(triPos + cam, triPos + tri.b);


    Polygon poly;
    if(shape.curves.size() > 0) {
      for(int p=0; p<shape.curves[0].points.size(); p++) {
        poly.vertices.push_back(scene->toScreen(shape.curves[0].points[p]));
      }
      geomRenderer.strokeWidth = 5;
      geomRenderer.fillColor.set(noisetn(20, 101), noisetn(20, 102), noisetn(20, 103), 0.5);
      geomRenderer.drawPolygon(poly);

      geomRenderer.strokeWidth = 3;
      for(int  p=0; p<poly.vertices.size(); p++) {
        geomRenderer.drawCircle(6, poly.vertices[p]);
      }

      /*for(int p=0; p<poly.selfIntersectionPointsDebug.size(); p++) {
        geomRenderer.strokeWidth = 2;
        geomRenderer.drawCircle(6, poly.selfIntersectionPointsDebug[p]);
      }*/
    }
    /*
      shape.renderPoints(scene, sdlInterface);
      shape.renderCurvesSmooth(scene, sdlInterface);
    */

    geomRenderer.strokeColor.set(0, 0, 0.3, 1);
    geomRenderer.strokeWidth = 2;
    Line lineR(0, 0, 100, 0);
    lineR.b.rotate(time);
    lineR.translate(events.m);
    geomRenderer.drawLine(lineR);

    //trianglex.set(tri);
    //trianglex.render(screenW/2, screenH/2, time, 400);

    //texture3.inactivate(0);

    /*shader.activate();
    shader.setUniform2f("screenSize", texture2.w, texture2.h);
    texture.activate(shader, "tex1", 0);
    texture2.activate(shader, "tex2", 1);
    quadx.render((double)events.mouseX, (double)events.mouseY, rotation, scale);
    shader.deActivate();
    texture.inactivate(0);
    texture2.inactivate(1);*/


    /*texture3.unsetRenderTarget();
    shaderPost.activate();
    shaderPost.setUniform2f("screenSize", screenW, screenH);
    shaderPost.setUniform1f("time", time);
    texture3.activate(shaderPost, "tex", 0);
    quadScreen.render(screenW/2, screenH/2);
    shaderPost.deActivate();
    texture3.inactivate(0);*/

    if(events.textInput.inputGrabbed) {
      console.render(sdlInterface);
    }

    //drawLine(200, 450, 500, 20, sdlInterface);

    wchar_t buf[256];
    double y = 20;

    if(shape.curves.size() > 0 && shape.curves[0].points.size() >= 3) {
      const Vec2d &a = shape.curves[0].points[0];
      const Vec2d &b = shape.curves[0].points[1];
      const Vec2d &c = shape.curves[0].points[2];
      double t = cross(b-a, c-a);
      double s = angle(a-b, c-b);
      std::swprintf(buf, 256, L"cross(b-a, c-a) = %.4f\nangle(a-b, c-b) = %.4f", t, s);
      y += textRenderer.print(buf, 10, y, 12, 1).y;

      std::swprintf(buf, 256, L"vertices = %d\ntriangles = %d", poly.vertices.size(), poly.triangles.size());
      y += textRenderer.print(buf, 10, y, 12, 1).y;
    }
    double area = tri.getArea()*sdlInterface->pixelWidthMeters*sdlInterface->pixelHeightMeters*100*100;
    std::swprintf(buf, 256, L"tri area = %.4f cm^2", area);
    y += textRenderer.print(buf, 10, y, 12, 1).y;

  }




};
