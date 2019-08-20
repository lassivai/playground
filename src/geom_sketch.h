#pragma once
#include "engine/sketch.h"
#include <vector>

double G = 10.0;

struct Particle
{
  Vec2d p, v;
  double m = 1;
  int spawnTime = 0;

  virtual ~Particle() {}

  virtual void update(double dt) {
    p.x += v.x * dt;
    p.y += v.y * dt;
  }

  void applyForce(Vec2d f, double dt) {
    double ax = f.x / m;
    double ay = f.y / m;
    v.x += ax * dt;
    v.y += ay * dt;
  }
  void mutualGravity(Particle &q, double dt) {
    Vec2d f = sub(q.p, p);
    double d = f.length();
    if(d > 10) {
      f.mul(G*q.m*m/(d*d*d));
      applyForce(f, dt);
      q.applyForce(f, -dt);
    }
  }
  void fixedLinearGravity(Particle q, double dt) {
    Vec2d f = sub(q.p, p);
    double d = f.length();
    f.mul(q.m*m/(d*d));
    applyForce(f, dt);
  }
  void springForce(Vec2d q, double k, double dt) {
    Vec2d f = sub(q, p);
    double d = f.length();
    f.mul(k);
    applyForce(f, dt);
  }

  virtual void render() {}
};

struct CircleParticle : public Particle
{
  double r = 10.0;
  double density = 1.0;

  CircleParticle() {
    m = r*PI*density;
  }

  virtual ~CircleParticle() {}

  CircleParticle(double r) {
    this->r = r;
    m = r*PI*density;
  }
  CircleParticle(double r, double density) {
    this->r = r;
    this->density = density;
    m = r*PI*density;
  }

  bool isHover(Vec2d h) {
    return (h.x-p.x)*(h.x-p.x) + (h.y-p.y)*(h.y-p.y) < r*r;
  }
};

struct Ship : public CircleParticle
{
  double direction = 0;
  double power = 100000;
  bool isThrottle = false;
  double rotateAmount = 0;

  Ship() {
    this->r = 20;
    this->m = 100;
  }

  void throttle(bool on) {
    isThrottle = on;
  }
  void rotate(double a) {
    rotateAmount = a;
  }

  void update(double dt) {
    CircleParticle::update(dt);
    if(isThrottle) {
      Vec2d f(power, direction+PI, true);
      this->applyForce(f, dt);
    }
    direction += rotateAmount * dt;
  }

  void render(SDLInterface *sdlInterface) {
    setColor(48.0/128, 10.0/128, 36.0/128, 1.0, sdlInterface);
    drawCircle(this->p, this->r, sdlInterface);
    double x1 = p.x + cos(direction) * r;
    double y1 = p.y + sin(direction) * r;
    drawCircle(x1, y1, 3, sdlInterface);
  }
};

void within(double x, double y, double r1, double r2, Vec2d &vOut) {
  double r = Random::getDouble(r1, r2);
  double a = Random::getDouble(0, TAU);
  vOut.x = x + r * cos(a);
  vOut.y = y + r * sin(a);
}

bool isMutualGravity = true;

struct GeomSketch : public Sketch
{
  std::vector<CircleParticle*> cs;
  std::vector<Particle> flame;
  Particle artificialGravity;
  CircleParticle *chosenCircle = NULL;

  Ship *spaceShip;

  bool isArtificialGravity = false;
  int collisionImpulse = 1;
  int counter = 0;
  int frozenStart = 0;

  void onInit() {
    int n = 100;
    if(cliArgs.numValues("-n") > 0) {
      int nin = atoi(cliArgs.getValues("-n")[0].c_str());
      if(nin > 0) n = nin;
    }
    double rMin = 10, rMax = 40;
    if(cliArgs.numValues("-r") == 1) {
      rMin = rMax = max(atof(cliArgs.getValues("-r")[0].c_str()), 1.0e-10);
    }
    if(cliArgs.numValues("-r") > 1) {
      rMin = max(atof(cliArgs.getValues("-r")[0].c_str()), 1.0e-10);
      rMax = max(atof(cliArgs.getValues("-r")[1].c_str()), 1.0e-10);
    }
    double density = 1;
    if(cliArgs.numValues("-d") > 0) {
      density = max(atof(cliArgs.getValues("-d")[0].c_str()), 1.0e-10);
    }
    if(cliArgs.numValues("-G") > 0) {
      G = max(atof(cliArgs.getValues("-G")[0].c_str()), 1.0e-10);
    }
    if(cliArgs.hasKey("-no_gravity")) {
      isMutualGravity = false;
    }
    if(cliArgs.numValues("-frozen_start") > 0) {
      frozenStart = atoi(cliArgs.getValues("-frozen_start")[0].c_str());
    }
    bool circleStart = false;
    double circleStartRadiusMin = 0.333 * screenH;
    double circleStartRadiusMax = 0.333 * screenH;
    if(cliArgs.hasKey("-circle_start")) {
      circleStart = true;
      if(cliArgs.numValues("-circle_start") == 1) {
        circleStartRadiusMin = circleStartRadiusMax = max(atof(cliArgs.getValues("-circle_start")[0].c_str()), 1.0);
      }
      if(cliArgs.numValues("-circle_start") > 1) {
        circleStartRadiusMin = max(atof(cliArgs.getValues("-circle_start")[0].c_str()), 1.0) * screenH;
        circleStartRadiusMax = max(atof(cliArgs.getValues("-circle_start")[1].c_str()), 1.0) * screenH;
      }
    }
    bool isStartingVelocity = false;
    double startingVelocityMin = 0, startingVelocityMax = 20;
    if(cliArgs.hasKey("-starting_velocity")) {
      isStartingVelocity = true;
      if(cliArgs.numValues("-starting_velocity") == 0) {
        startingVelocityMin = startingVelocityMax = max(atof(cliArgs.getValues("-starting_velocity")[0].c_str()), 1.0);
      }
      if(cliArgs.numValues("-starting_velocity") > 1) {
        startingVelocityMin = max(atof(cliArgs.getValues("-starting_velocity")[0].c_str()), 1.0);
        startingVelocityMax = max(atof(cliArgs.getValues("-starting_velocity")[1].c_str()), 1.0);
      }
    }
    if(cliArgs.numValues("-impulse") > 0) {
      collisionImpulse = atoi(cliArgs.getValues("-impulse")[0].c_str());
    }
    artificialGravity.m = 10000;
    if(cliArgs.numValues("-mouse_mass") > 0) {
      artificialGravity.m = atof(cliArgs.getValues("-mouse_mass")[0].c_str());
    }

    for(int i=0; i<n; i++) {
      CircleParticle *c = new CircleParticle(Random::getDouble(rMin, rMax), density);
      if(circleStart) {
        within(screenW/2, screenH/2, circleStartRadiusMin, circleStartRadiusMax, c->p);
      }
      else {
        c->p.randomize(0, screenW, 0, screenH);
      }
      if(isStartingVelocity) {
        c->v.randomizeLengthAndDirection(startingVelocityMin, startingVelocityMax);
      }
      cs.push_back(c);
    }
    spaceShip = new Ship();
    //cs.push_back(spaceShip);
    spaceShip->p.set(screenW/2, screenH/2);
  }

  void reset() {
    for(int i=0; i<cs.size(); i++) {
      delete cs.at(i);
    }
    cs.clear();
    onInit();
  }


  double impulseB(double a, double b, double v, double w) {
    double f = -b*b - b*a;
    double g = 2.0*b*b*w + 2.0*a*b*v;
    double h = -2.0*a*b*v*w + a*b*w*w - b*b*w*w;
    double s = (-g-sqrt(g*g-4.0*f*h))/(2.0*f);
    double t = (-g+sqrt(g*g-4.0*f*h))/(2.0*f);
    //return w > v ? s : t;
    return a > b ? s : t;
    //return t;
  }
  double impulseA(double a, double b, double v, double w, double impB) {
    return (a*v + b*w - b*impB)/a;
  }

  void onKeyDown() {
    if(events.sdlKeyCode == SDLK_r) {
      reset();
    }
    if(events.sdlKeyCode == SDLK_SPACE) {
      isArtificialGravity = true;
    }

    if(events.sdlKeyCode == SDLK_UP) {
      spaceShip->throttle(true);
    }
    if(events.sdlKeyCode == SDLK_LEFT) {
      spaceShip->rotate(-TAU);
    }
    if(events.sdlKeyCode == SDLK_RIGHT) {
      spaceShip->rotate(TAU);
    }
  }
  void onKeyUp() {
    if(events.sdlKeyCode == SDLK_SPACE) {
      isArtificialGravity = false;
    }
    if(events.sdlKeyCode == SDLK_UP) {
      spaceShip->throttle(false);
    }
    if(events.sdlKeyCode == SDLK_LEFT) {
      spaceShip->rotate(0);
    }
    if(events.sdlKeyCode == SDLK_RIGHT) {
      spaceShip->rotate(0);
    }
  }

  void onMousePressed() {
    for(int i=0; i<cs.size(); i++) {
      if(cs.at(i)->isHover(events.m)) {
        chosenCircle = cs.at(i);
      }
    }
  }
  void onMouseReleased() {
    chosenCircle = NULL;
  }

  void onUpdate()
  {
    counter++;
    isArtificialGravity = chosenCircle == NULL && events.mouseDownL;
    if(isArtificialGravity) {
      artificialGravity.p = events.m;
      //artificialGravity.p = spaceShip->p;
    }

    if(isArtificialGravity) {
      for(int i=0; i<cs.size(); i++) {
        cs.at(i)->fixedLinearGravity(artificialGravity, dt);
      }
    }
    if(chosenCircle != NULL) {
      chosenCircle->springForce(events.m, 1000, dt);
    }

    if(counter > frozenStart) {
      for(int i=0; i<cs.size(); i++) {
        cs.at(i)->update(dt);
      }
    }
    if(collisionImpulse != 0 || isMutualGravity) {
      for(int i=0; i<cs.size(); i++) {
        CircleParticle *a = cs.at(i);
        for(int k=i+1; k<cs.size(); k++) {
          CircleParticle *b = cs.at(k);
          Vec2d t = sub(a->p, b->p);
          double dist = t.length();
          if(dist < a->r + b->r) {
            double w = a->r + b->r - dist;
            t.normalize();
            Vec2d va = Vec2d(a->v), vb = Vec2d(b->v);
            double impB = impulseB(a->m, b->m, va.length(), vb.length());
            double impA = impulseA(a->m, b->m, va.length(), vb.length(), impB);
            //a->v.setLength(impA);
            //b->v.setLength(impB);
            if(counter > frozenStart) {
              if(collisionImpulse == 1) {
                a->v.add(mul(t, impA));
                b->v.add(mul(t, -impB));
              }
              else if(collisionImpulse == 2) {
                double d = a->m/(a->m+b->m);
                a->v.add(mul(t, -2.0*dot(t, a->v)*(1.0-d)));
                b->v.add(mul(t, -2.0*dot(t, b->v)*d));
              }
            }
            a->p.add(mul(t, w*0.55));
            b->p.add(mul(t, -w*0.55));
          }
          else if(counter > frozenStart && isMutualGravity) {
            a->mutualGravity(*b, dt);
          }
        }
      }
    }
  }


  void onDraw() {
    clear(0, 0, 0, 1);
    if(chosenCircle != NULL) {
      setColor(0.3, 0.05, 0.2, 1.0, sdlInterface);
      drawLine(chosenCircle->p, events.m, sdlInterface);
    }
    /*if(isArtificialGravity) {
      setColor(0.1, 0.1, 0.1, 1.0, sdlInterface);
      double cycleLength = 10.0;
      int n = 30;
      for(int i=0; i<n; i++) {
        //double t = modff((double)i*cycleLength/n+time, cycleLength);
        double t = fract((double)i/n+fract(time/cycleLength));
        double r = 2.0/(t*t*t+0.0035);
        if(r > 60)
        drawCircle(events.m, r, sdlInterface);
      }
    }*/
    for(int i=0; i<cs.size(); i++) {
      CircleParticle *c = cs.at(i);
      double f = 1.0-c->r*c->r/(40*40);
      //setColor(f*0.3, f*0.2, f*0.9, 1, sdlInterface);
      if(c == spaceShip) {
        spaceShip->render(sdlInterface);
      }
      else if(c == chosenCircle) {
        setColor(0.6, 0.1, 0.4, 1, sdlInterface);
      }
      else {
        setColor(0.6, 0.4, 1.0, 1, sdlInterface);
      }
      if(c-> r <= 1.0) {
        drawPoint(c->p.x, c->p.y, sdlInterface);
      }
      else {
        drawCircle(c->p, c->r, sdlInterface);
      }
      //fillRectCenter(c->p.x, c->p.y, 0.4*c->r, 0.4*c->r, sdlInterface);
      //drawPoint(c->p.x, c->p.y, sdlInterface);
    }
  }
};
