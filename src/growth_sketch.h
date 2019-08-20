#pragma once
#include "engine/sketch.h"
#include <vector>

struct Wanderer
{
  double x = 0, y = 0;
  double direction = 0;
  double speed = 0;
  double spawnTime;
  double lifeSpan = 10;
  double rotationSpeed = 1;
  double nourishedLevel = 1.0;

  Wanderer(double time) {
    spawnTime = time;
  }

  Wanderer(double x, double y, double direction, double speed, double time) {
    this->x = x;
    this->y = y;
    this->direction = direction;
    this->speed = speed;
    spawnTime = time;
  }

  void update(double dt) {
    x += cos(direction) * speed * dt;
    y += sin(direction) * speed * dt;
    direction += Random::getDouble(
      -TAU*rotationSpeed*0.5*dt, TAU*rotationSpeed*0.5*dt);
  }

  Wanderer* getCopy(double time) {
    Wanderer *w = new Wanderer(time);
    w->x = this->x;
    w->y = this->y;
    w->direction = this->direction;
    w->speed = this->speed;
    w->lifeSpan = this->lifeSpan;
    w->nourishedLevel = 0.5 * this->nourishedLevel;
    this->nourishedLevel = 0.5 * this->nourishedLevel;
    return w;
  }

  void mutateSpeed(double probability, double maxFactor) {
    if(Random::getDouble() < probability) {
      speed *= Random::getDouble(1.0, maxFactor);
    }
  }
  void mutateRotationSpeed(double probability, double maxFactor) {
    if(Random::getDouble() < probability) {
      rotationSpeed *= Random::getDouble(1.0, maxFactor);
    }
  }
  /*void mutateLifeSpan(double probability, double maxFactor) {
    if(Random::getDouble() < probability) {
      if(Random::getDouble() < 0.5) {
        lifeSpan *= Random::getDouble(1.0, maxFactor);
      }
      else {
        lifeSpan /= Random::getDouble(1.0, maxFactor);
      }
    }
  }*/
  void mutateLifeSpan(double probability, double maxFactor) {
    if(Random::getDouble() < probability) {
      lifeSpan *= Random::getDouble(1.0, maxFactor);
      //lifeSpan = Random::getInt(5, 20);
    }
  }

};


struct GrowthSketch : public Sketch
{
  std::vector<Wanderer*> *ws;
  int *occupancyGridTmp, *occupancyGrid;
  bool showOccupancyGrid = false;

  void onInit() {
    ws = new std::vector<Wanderer*>();
    occupancyGridTmp = new int[screenW*screenH];
    occupancyGrid = new int[screenW*screenH];
    reset();
  }

  void reset() {
    for(int i=0; i<ws->size(); i++) {
      //Wanderer *w = ws->at(i);
      //ws->erase(ws->begin()+i);
      delete ws->at(i);
    }
    ws->clear();

    int n = 7;
    if(cliArgs.numValues("-n") > 0) {
      n = atoi(cliArgs.getValues("-n")[0].c_str());
    }
    bool randomStartingPos = cliArgs.hasKey("-randpos");

    for(int i=0; i<n; i++) {
      double x = randomStartingPos ? Random::getDouble(0, screenW) : screenW/2;
      double y = randomStartingPos ? Random::getDouble(0, screenH) : screenH/2;
      Wanderer *w = new Wanderer(x, y, Random::getDouble(0, TAU), Random::getDouble(1, 10), time);
      if(w == NULL) {
        printf("onInit null\n");
      }
      ws->push_back(w);
    }
    memset(occupancyGridTmp, 0, sizeof(int)*screenW*screenH);
    memset(occupancyGrid, 0, sizeof(int)*screenW*screenH);
    clear(0.0, 0.0, 0.0, 1.0);
    updateScreen(sdlInterface);
    clear(0.0, 0.0, 0.0, 1.0);
  }

  void onKeyDown() {
    if(events.sdlKeyCode = SDLK_r) {
      reset();
    }
    if(events.sdlKeyCode = SDLK_o) {
      showOccupancyGrid = !showOccupancyGrid;
    }
  }

  void onMouseMotion() {
    if(events.mouseDownL && !equals(events.m, events.mp)) {
      Wanderer *w = new Wanderer(events.mouseX, events.mouseY,
          Random::getDouble(0, TAU), Random::getDouble(1, 10), time);
      ws->push_back(w);
    }
    if(events.mouseDownR) {
      putValue(occupancyGridTmp, 100, events.mouseX, events.mouseY, screenW, screenH);
      putValue(occupancyGridTmp, 100, events.mouseX-1, events.mouseY, screenW, screenH);
      putValue(occupancyGridTmp, 100, events.mouseX+1, events.mouseY, screenW, screenH);
      putValue(occupancyGridTmp, 100, events.mouseX, events.mouseY-1, screenW, screenH);
      putValue(occupancyGridTmp, 100, events.mouseX, events.mouseY+1, screenW, screenH);
    }
  }
  void putValue(int *arr, int v, int x, int y, int w, int h) {
    if(x < 0 || x >= w || y < 0 || y >= h) {
      return;
    }
    int xy = x + y*w;
    arr[xy] = v;
  }

  void onUpdate() {
    //memset(occupancyGridTmp, 0, sizeof(int)*screenW*screenH);

    for(int i=0; i<ws->size(); i++) {
      Wanderer *w = ws->at(i);
      bool die = false;
      if(w == NULL) {
        printf("onUpdate null\n");
      }
      int xyPrev = (int)w->x + (int)w->y*screenW;
      w->update(dt);
      if(w->x < 0) w->x += screenW;
      if(w->x > screenW) w->x -= screenW;
      if(w->y < 0) w->y += screenH;
      if(w->y > screenH) w->y -= screenH;
      int xy = (int)(w->x) + (int)w->y*screenW;
      if(xy != xyPrev) {
        occupancyGridTmp[xy]++;
      }
      w->nourishedLevel += 1.0*dt/sqrt(w->speed);
      if(occupancyGrid[xy] > 1) {
        w->nourishedLevel *= 0.1;
        if(occupancyGrid[xy] > 10) {
          die = true;
        }
      }

      bool spawn = Random::getDouble() < 0.02*w->nourishedLevel;
      if(spawn && ws->size() < 10000) {
        Wanderer *spawned = w->getCopy(time);
        if(spawned == NULL) {
          printf("spawned null\n");
        }
        spawned->direction += Random::getDouble(-PI/4.0, PI/4.0);
        spawned->mutateSpeed(0.25, 0.8);
        spawned->mutateRotationSpeed(0.25, 0.9);
        spawned->mutateLifeSpan(0.25, 0.8);
        spawned->mutateSpeed(0.05, 1.5);
        spawned->mutateRotationSpeed(0.25, 1.5);
        spawned->mutateLifeSpan(0.05, 1.5);
        ws->push_back(spawned);
      }
      die = die || Random::getDouble() > w->lifeSpan/(time-w->spawnTime);
      die = die || (Random::getDouble()*w->nourishedLevel < 0.001*dt);
      if(die) {
        ws->erase(ws->begin()+i);
        delete w;
      }
    }
    memcpy(occupancyGrid, occupancyGridTmp, sizeof(int)*screenW*screenH);
  }

  void onDraw() {
    //clear(0, 0, 0, 1);
    /*if(showOccupancyGrid) {
      for(int i=0; i<screenW*screenH; i++) {
        double f = occupancyGrid[i];
        setColor(0.1*f, 0.1*f, 0.1*f, 1.0, sdlInterface);
        drawPoint(i%screenW, i/screenW, sdlInterface);
      }
    }*/
    //else {
      for(Wanderer *w : *ws) {
        if(w == NULL) {
          printf("onDraw null\n");
        }
        double f = w->nourishedLevel;
        setColor(0.3*f, 0.2*f, 0.9*f, 1.0, sdlInterface);
        //setColor(0.3, 0.2, 0.9, 1.0, sdlInterface);
        drawPoint(w->x, w->y, sdlInterface);
        //drawCircle(w->x, w->y, 2.0, sdlInterface);
      }
    }
  //}
};
