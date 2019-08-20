#pragma once
#include "util.h"
#include "gfxwrap.h"
#include "mathl.h"
#include "geomath.h"
#include "alg.h"
#include "scene.h"
#include <vector>
#include <unordered_map>

// TODO is point within the object for every item here (some matrix stuff needed with rectangles)



// FIXME
static bool debugTriangulation = false;


struct GeomObject {
  static const int none = -1, circle = 0, line = 1, triangle = 2, rect = 3, polygon = 4, hoop = 5;
  const int type;

  GeomObject() : type(none) {}

  GeomObject(int type) : type(type) {}

  GeomObject(const GeomObject &geomObject) : type(geomObject.type) {}

  virtual ~GeomObject() {}

  virtual double getArea() {
    return 1;
  }

  virtual Vec2d getCenterOfMass() {
    return Vec2d(0, 0);
  }

  virtual const std::string getName() {
    return "<geom>";
  }

  // FIXME
  /*virtual bool isPointWithin(Vec2d p) {
    return false;
  }*/

  virtual void translate(const Vec2d &t) {}
  virtual void rotate(double w) {}

  virtual void setPosition(const Vec2d &p) {}
};


struct Triangle : public GeomObject {
  Vec2d a, b, c;

  Triangle() : GeomObject(GeomObject::triangle) {
    a.set(0, 0);
    b.set(0, 0);
    c.set(0, 0);
  }

  Triangle(const Triangle &t) : GeomObject(GeomObject::triangle) {
    this->a = t.a;
    this->b = t.b;
    this->c = t.c;
  }

  Triangle(const Vec2d &a, const Vec2d &b, const Vec2d &c) : GeomObject(GeomObject::triangle)  {
    this->a = a;
    this->b = b;
    this->c = c;
  }


  Triangle(const double &x0, const double &y0, const double &x1, const double &y1, const double &x2, const double &y2) : GeomObject(GeomObject::triangle)  {
    this->a.set(x0, y0);
    this->b.set(x1, y1);
    this->c.set(x2, y2);
  }

  inline void set(const Vec2d &a, const Vec2d &b, const Vec2d &c) {
    this->a = a;
    this->b = b;
    this->c = c;
  }

  inline void set(const Triangle &t) {
    this->a = t.a;
    this->b = t.b;
    this->c = t.c;
  }

  void set(const double &x0, const double &y0, const double &x1, const double &y1, const double &x2, const double &y2) {
    this->a.set(x0, y0);
    this->b.set(x1, y1);
    this->c.set(x2, y2);
  }

  double getArea() {
    return fabs(cross(b-a, c-a) * 0.5);
  }

  Vec2d getCenterOfMass() {
    Vec2d abm = a + (b-a) * 0.5;
    Vec2d bcm = b + (c-b) * 0.5;
    Vec2d ip;
    getIntersectionPoint(abm, c, bcm, a, ip);
    return ip;
  }

  const std::string getName() {
    return "triangle";
  }

  void translate(const Vec2d &t) {
    a += t;
    b += t;
    c += t;
  }

  void rotate(double w) {
    double sinA = sin(w), cosA = cos(w);
    a.rotate(sinA, cosA);
    b.rotate(sinA, cosA);
    c.rotate(sinA, cosA);
  }

  void rotate(double sinA, double cosA) {
    a.rotate(sinA, cosA);
    b.rotate(sinA, cosA);
    c.rotate(sinA, cosA);
  }
};


struct Circle : public GeomObject
{
  Vec2d pos;
  double rot;
  double r;

  Circle() : GeomObject(GeomObject::circle)  {
    pos.set(0, 0);
    rot = 0;
    r = 1.0;
  }

  Circle(const Circle &circle) : GeomObject(GeomObject::circle) {
    this->pos = circle.pos;
    this->r = circle.r;
    this->rot = circle.rot;
  }

  Circle(double r) : GeomObject(GeomObject::circle) {
    pos.set(0, 0);
    rot = 0;
    this->r = r;
  }
  Circle(Vec2d pos, double r) : GeomObject(GeomObject::circle) {
    this->pos = pos;
    rot = 0;
    this->r = r;
  }
  Circle(double r, double x, double y) : GeomObject(GeomObject::circle) {
    pos.set(x, y);
    rot = 0;
    this->r = r;
  }
  Circle(double r, double x, double y, double rot) : GeomObject(GeomObject::circle) {
    pos.set(x, y);
    this->r = r;
    this->rot = rot;
  }

  double getArea() {
    return PI * r * r;
  }

  Vec2d getCenterOfMass() {
    return pos;
  }

  const std::string getName() {
    return "circle";
  }

  void translate(const Vec2d &t) {
    pos += t;
  }

  void rotate(double w) {
    rot += w;
  }
};

struct Hoop : public GeomObject
{
  Vec2d pos;
  double rot;
  double r, r2;

  Hoop() : GeomObject(GeomObject::hoop)  {
    pos.set(0, 0);
    rot = 0;
    r = 1.0;
    r2 = 0.7;
  }

  Hoop(const Hoop &hoop) : GeomObject(GeomObject::hoop) {
    this->pos = hoop.pos;
    this->r = hoop.r;
    this->r2 = hoop.r2;
    this->rot = hoop.rot;
  }

  Hoop(double r, double r2) : GeomObject(GeomObject::hoop) {
    pos.set(0, 0);
    rot = 0;
    this->r = r;
    this->r2 = r2;
  }
  Hoop(Vec2d pos, double r, double r2) : GeomObject(GeomObject::hoop) {
    this->pos = pos;
    rot = 0;
    this->r = r;
    this->r2 = r2;
  }
  Hoop(double r, double r2, double x, double y) : GeomObject(GeomObject::hoop) {
    pos.set(x, y);
    rot = 0;
    this->r = r;
    this->r2 = r2;
  }
  Hoop(double r, double r2, double x, double y, double rot) : GeomObject(GeomObject::hoop) {
    pos.set(x, y);
    this->r = r;
    this->r2 = r2;
    this->rot = rot;
  }

  double getArea() {
    return PI*r*r - PI*r2*r2;
  }
  void translate(const Vec2d &t) {
    pos += t;
  }

  void rotate(double w) {
    rot += w;
  }
  Vec2d getCenterOfMass() {
    return pos;
  }

  const std::string getName() {
    return "hoop";
  }
};


struct Line : public GeomObject
{
  Vec2d a, b;
  double bevelRadius = 1;

  Line() : GeomObject(GeomObject::line) {
    a.set(0, 0);
    b.set(0, 0);
  }

  Line(const Line &line) : GeomObject(GeomObject::line) {
    this->a = line.a;
    this->b = line.b;
    bevelRadius = line.bevelRadius;
  }

  Line(double x0, double y0, double x1, double y1, double r = 1) : GeomObject(GeomObject::line) {
    a.set(x0, y0);
    b.set(x1, y1);
    bevelRadius = r;
  }

  Line(const Vec2d &a, const Vec2d &b, double r = 1) : GeomObject(GeomObject::line) {
    this->a = a;
    this->b = b;
    bevelRadius = r;
  }

  void set(double x0, double y0, double x1, double y1, double r = 1) {
    a.set(x0, y0);
    b.set(x1, y1);
    bevelRadius = r;
  }

  void set(const Vec2d &a, const Vec2d &b, double r = 1) {
    this->a = a;
    this->b = b;
    bevelRadius = r;
  }

  Circle getAlignedCircle() const {
    Circle c;
    c.pos.set((a.x+b.x)*0.5, (a.y+b.y)*0.5);
    c.r = distance(a, b) * 0.5;
    double dx = b.x - a.x, dy = b.y - a.y;
    c.rot = atan2(dy, dx);
    return c;
  }

  Vec2d getNormal() {
    Vec2d n = b - a;
    n.set(-n.y, n.x);
    n.normalize();
    return n;
  }

  double getLength() {
    return distance(a, b);
  }

  double getArea() {
    return distance(a, b) * 2.0 * bevelRadius;
  }

  Vec2d getCenterOfMass() {
    return Vec2d((a.x+b.x)*0.5, (a.y+b.y)*0.5);
  }

  const std::string getName() {
    return "line";
  }

  void translate(const Vec2d &t) {
    a += t;
    b += t;
  }

  void rotate(double w) {
    double sinA = sin(w), cosA = cos(w);
    a.rotate(sinA, cosA);
    b.rotate(sinA, cosA);
  }

};





struct Rect : public GeomObject  {
  Vec2d pos;
  double rot;
  double w, h;

  Rect() : GeomObject(GeomObject::rect) {
    pos.set(0, 0);
    rot = 0;
    w = 1.0;
    h = 1.0;
  }

  Rect(const Rect &rect) : GeomObject(GeomObject::rect) {
    this->pos = rect.pos;
    this->w = rect.w;
    this->h = rect.h;
    this->rot = rect.rot;
  }

  Rect(double w, double h) : GeomObject(GeomObject::rect) {
    pos.set(0, 0);
    rot = 0;
    this->w = w;
    this->h = h;
  }
  Rect(double w, double h, double x, double y) : GeomObject(GeomObject::rect) {
    pos.set(x, y);
    rot = 0;
    this->w = w;
    this->h = h;
  }
  Rect(const Vec2d &size, const Vec2d &pos) : GeomObject(GeomObject::rect) {
    this->pos = pos;
    rot = 0;
    this->w = size.x;
    this->h = size.y;
  }
  Rect(double w, double h, double x, double y, double rot) : GeomObject(GeomObject::rect) {
    pos.set(x, y);
    this->w = w;
    this->h = h;
    this->rot = rot;
  }
  void set(double w, double h, double x, double y) {
    pos.set(x, y);
    rot = 0;
    this->w = w;
    this->h = h;
  }

  Rect &operator=(const Rect &rect) {
    this->pos = rect.pos;
    this->w = rect.w;
    this->h = rect.h;
    this->rot = rect.rot;
    return *this;
  }
  
  double getArea() {
    return w*h;
  }

  Vec2d getCenterOfMass() {
    return pos;
  }

  const std::string getName() {
    return "rect";
  }

  void translate(const Vec2d &t) {
    pos += t;
  }

  void rotate(double w) {
    rot += w;
  }

  // FIXME rotation not concidered currently
  virtual bool isPointWithin(const Vec2d &p) {
    return p.x >= pos.x - w*0.5 && p.x <= pos.x + w*0.5 && p.y >= pos.y - h*0.5 && p.y <= pos.y + h*0.5;
  }
};



struct Polygon : public GeomObject
{
  std::vector<Vec2d> vertices;
  std::vector<Triangle> triangles;
  std::vector<std::vector<Vec2d>> holes;
  bool useCachedTriangles = false;;
  double area = 0;
  Vec2d centerOfMass = {0, 0};

  Polygon() : GeomObject(GeomObject::polygon) {}

  Polygon(const Polygon &polygon) : GeomObject(GeomObject::polygon) {
    this->vertices = std::vector<Vec2d>(polygon.vertices);
    this->triangles = std::vector<Triangle>(polygon.triangles);
    this->holes = std::vector<std::vector<Vec2d>>(polygon.holes);
    this->area = polygon.area;
    this->centerOfMass = polygon.centerOfMass;
  }


  Polygon(int numVertices, double minR, double maxR) : GeomObject(GeomObject::polygon) {
    create(numVertices, minR, maxR);
  }

  void addVertex(double x, double y) {
    vertices.push_back(Vec2d(x, y));
  }
  void addVertex(Vec2d p) {
    vertices.push_back(p);
  }

  void clear() {
    vertices.clear();
    triangles.clear();
    holes.clear();
    area = 0;
    centerOfMass.set(0, 0);
  }

  void create(int numVertices, double minR, double maxR) {
    double angles[numVertices];
    vertices.resize(numVertices);

    for(int i=0; i<numVertices; i++) {
      angles[i] = Random::getDouble(0, TAU);
    }
    sort(angles, numVertices);

    for(int i=0; i<numVertices; i++) {
      double r = Random::getDouble(minR, maxR);
      vertices[i] = Vec2d(r*cos(angles[i]), r*sin(angles[i]));
    }
  }

  void createRegular(int numVertices, double r) {
    vertices.resize(numVertices);
    for(int i=0; i<numVertices; i++) {
      double a = 2.0 * PI * i / numVertices;
      vertices[i] = Vec2d(r*cos(a), r*sin(a));
    }
  }

  // FIXME get rid of this
  void render(double x, double y, SDLInterface *sdlInterface) {
    int n = vertices.size();
    for(int i=0; i<n+1; i++) {
      int a = i % n;
      int b = (i+1) % n;
      drawLine(x+vertices[a].x, y+vertices[a].y, x+vertices[b].x, y+vertices[b].y, sdlInterface);
    }
  }


  std::vector<Triangle> triangulateConvex() {
    triangles.clear();
    int n = vertices.size();
    if(n < 3) return triangles;
    int a = 0, b = 1, c = 2;
    for(int i=0; i<n-2; i++) {
      triangles.push_back(Triangle(vertices[a], vertices[b], vertices[c]));
      b++;
      c++;
    }
    return triangles;
  }

  bool isSelfIntersection(const Vec2d &a, const Vec2d &b, const std::vector<Vec2d> &v)  {
    for(int i=0; i<v.size()+1; i++) {
      const Vec2d &q = v[i%v.size()];
      const Vec2d &p = v[(i+1)%v.size()];
      if(a == q || a == p || b == p || b == q) continue;
      Vec2d ip;
      bool intersects = getIntersectionPoint(a, b, q, p, ip);
      if(intersects) {
        return true;
      }
    }
    return false;
  }
  bool isHoleIntersection(const Vec2d &a, const Vec2d &b, const std::vector<std::vector<Vec2d>> &holesTmp)  {
    for(int h=0; h<holesTmp.size(); h++) {
      const std::vector<Vec2d> &v = holesTmp[h];
      for(int i=0; i<v.size()+1; i++) {
        const Vec2d &q = v[i%v.size()];
        const Vec2d &p = v[(i+1)%v.size()];
        if(a == q || a == p || b == p || b == q) continue;
        Vec2d ip;
        bool intersects = getIntersectionPoint(a, b, q, p, ip);
        if(intersects) {
          return true;
        }
      }
    }
    return false;
  }

  bool isSelfIntersectionTri(const Vec2d &a, const Vec2d &b, const Vec2d &c, const std::vector<Vec2d> &v)  {
    for(int i=0; i<v.size(); i++) {
      const Vec2d &p = v[i];
      if(p == a || p == b || p == c) continue;
      bool intercects = isPointWithinTriangle(p, a, b, c);
      if(intercects) {
        return true;
      }
    }
    return false;
  }


  bool isHoleIntersectionTri(const Vec2d &a, const Vec2d &b, const Vec2d &c, const std::vector<std::vector<Vec2d>> &holesTmp, int &holeIndexWithClosestPoint, int &nearestPointIndexOfIntersectingHole)  {
    holeIndexWithClosestPoint = -1;
    nearestPointIndexOfIntersectingHole = -1;
    double nearestPointDistanceOfIntersectingHole = 1e100;

    for(int h=0; h<holesTmp.size(); h++) {
      const std::vector<Vec2d> &v = holesTmp[h];

      for(int i=0; i<v.size(); i++) {
        const Vec2d &p = v[i];
        bool intersects = isPointWithinTriangle(p, a, b, c);
        if(intersects) {
          double dist = distance(a, p);
          if(dist < nearestPointDistanceOfIntersectingHole) {
            nearestPointDistanceOfIntersectingHole = dist;
            nearestPointIndexOfIntersectingHole = i;
            holeIndexWithClosestPoint = h;
          }
        }
      }
    }
    return holeIndexWithClosestPoint >= 0;
  }







  // TODO detect and report invalid polygon instead of giving messy stuff
  std::vector<Triangle> triangulate(int windingDirection = 1) {
    if(useCachedTriangles) return triangles;

    //selfIntersectionPointsDebug.clear();
    //anglesDebug.clear();
    triangles.clear();
    if(vertices.size() < 3) return triangles;

    std::vector<Vec2d> v(vertices);

    bool counterClockwiseWinding = false;
    if(windingDirection < 0) counterClockwiseWinding = true;

    // FIXME this could be calculated exactly instead of this approximation, which sucks
    if(windingDirection == 0) {
      int numCCW = 0, numCW = 0;
      int n = v.size();
      for(int i=0; i<n; i++) {
        bool ccw = isCCW(v[i], v[(i+1)%n], v[(i+2)%n]);
        if(ccw) numCCW++;
        else numCW++;
      }
      bool counterClockwiseWinding = numCCW >= numCW;
    }

    int a = 0;
    int k = 0;

    while(v.size() > 2) {
      while(v.size() > 2) {
        int b = (a+1) % v.size(), c = (a+2) % v.size();
        bool ccw = isCCW(v[a], v[b], v[c]);
        bool isInnerAngle = counterClockwiseWinding ? ccw : !ccw;
        if(isInnerAngle) {
          if(!isSelfIntersectionTri(v[a], v[b], v[c], vertices)) {
            triangles.push_back(Triangle(v[a], v[b], v[c]));
            v.erase(v.begin()+b);
            if(b < a) a--;
            k = 0;
          }
        }
        break;
      }
      while(v.size() > 2) {
        int b = (a-1+v.size()) % v.size(), c = (a-2+v.size()) % v.size();
        bool ccw = isCCW(v[a], v[b], v[c]);
        bool isInnerAngle = counterClockwiseWinding ? !ccw : ccw;
        if(isInnerAngle) {
          if(!isSelfIntersectionTri(v[a], v[b], v[c], vertices)) {
            triangles.push_back(Triangle(v[a], v[b], v[c]));
            v.erase(v.begin()+b);
            if(b < a) a--;
            k = 0;
          }
        }
        break;
      }
      a = (a+1) % v.size();
      k++;
      if(k > v.size()) break;
    }

    calculateMassAndCenter();

    return triangles;
  }

  void print(std::vector<char> chars) {
    for(int i=0; i<chars.size(); i++) {
      printf("%c%s", chars[i], i < chars.size()-1 ? ", " : "\n");
    }
  }

  std::vector<Line> dbgHoleConnections;




  // This will be a great accomplishment for me if I actually finish this thing...
  // It shouldn't be too much different in the end than the previous one, with the logic I tried to
  // visualise (for myself). Starting with the outermost polygon first, and whenever there's intersecting
  // shape (that hole) it will be added to the vertex list of that polygon. In addition to that there seems
  // to be only one  kind of extra logic to be dealed with - the vertex in the "hole" that is first added to the
  // original polygon has two places in the vertex list... I'm not going to deal with shapes within shapes within...
  // There will be extra troubles too if the winding direction is different that that of the outer polygon...
  // DONE (almost)
  std::vector<Triangle> triangulateWithHoles() {
    if(useCachedTriangles) return triangles;

    dbgHoleConnections.clear();
    //selfIntersectionPointsDebug.clear();
    //anglesDebug.clear();
    triangles.clear();
    if(vertices.size() < 3) return triangles;

    bool counterClockwiseWinding = false;
    int holeIndexWithClosestPoint, nearestPointIndexOfIntersectingHole;
    std::vector<Vec2d> v(vertices);
    std::vector<std::vector<Vec2d>> holesTmp(holes);

    std::vector<Vec2d> verticesTotal(vertices);
    for(int i=0; i<holesTmp.size(); i++) {
      for(int p=0; p<holesTmp[i].size(); p++) {
        verticesTotal.push_back(holesTmp[i][p]);
      }
    }

    std::vector<char> vertexNames;
    char currentLetter = 'a';
    for(int i=0; i<v.size(); i++) {
      vertexNames.push_back(currentLetter++);
    }
    int counter = 0;
    int a = 0;
    int k = 0;
    while(v.size() > 2) {
      while(v.size() > 2) {
        int b = (a+1) % v.size(), c = (a+2) % v.size();
        //printf("FORWARD %d\n%c - %c - %c\n", counter, vertexNames[a], vertexNames[b], vertexNames[c]);
        //print(vertexNames);
        bool ccw = isCCW(v[a], v[b], v[c]);
        bool isInnerAngle = counterClockwiseWinding ? ccw : !ccw;
        if(isInnerAngle) {
          if(holesTmp.size() > 0 && isHoleIntersectionTri(v[a], v[b], v[c], holesTmp, holeIndexWithClosestPoint, nearestPointIndexOfIntersectingHole)) {
            /*if(v.size() == 4) {
              getClosestHolePoint(v[a], holesTmp, holeIndexWithClosestPoint, nearestPointIndexOfIntersectingHole);
            }*/
            Vec2d p = holesTmp[holeIndexWithClosestPoint][nearestPointIndexOfIntersectingHole];
            if(!isSelfIntersection(v[a], p, vertices) && !isSelfIntersection(v[a], p, v) && !isHoleIntersection(v[a], p, holes)) {
              dbgHoleConnections.push_back(Line(v[a], p));
              for(int i=0; i<holesTmp[holeIndexWithClosestPoint].size()+1; i++) {
                int t = (i + nearestPointIndexOfIntersectingHole) % holesTmp[holeIndexWithClosestPoint].size();
                v.insert(v.begin() + a+1+i, holesTmp[holeIndexWithClosestPoint][t]);
                vertexNames.insert(vertexNames.begin() + a+1+i, currentLetter++);
              }
              v.insert(v.begin() + a+2+holesTmp[holeIndexWithClosestPoint].size(), v[a]);
              vertexNames.insert(vertexNames.begin() + a+2+holesTmp[holeIndexWithClosestPoint].size(), vertexNames[a]);
              holesTmp.erase(holesTmp.begin() + holeIndexWithClosestPoint);
              k = 0;
              //printf("hole added\n");
              //print(vertexNames);
            }
          }
          else if(!isSelfIntersectionTri(v[a], v[b], v[c], verticesTotal) && !isSelfIntersection(v[a], v[c], vertices) && !isSelfIntersection(v[a], v[c], v) && !isHoleIntersection(v[a], v[c], holes)) {
            triangles.push_back(Triangle(v[a], v[b], v[c]));
            v.erase(v.begin()+b);
            vertexNames.erase(vertexNames.begin()+b);
            if(b < a) a--;
            k = 0;
            //printf("triangulated\n");
            //print(vertexNames);
          }
        }
        break;
      }
      while(v.size() > 2) {
        int b = (a-1+v.size()) % v.size(), c = (a-2+v.size()) % v.size();
        //printf("BACKWARD %d\n%c - %c - %c\n", counter, vertexNames[a], vertexNames[b], vertexNames[c]);
        //print(vertexNames);
        bool ccw = isCCW(v[a], v[b], v[c]);
        bool isInnerAngle = counterClockwiseWinding ? !ccw : ccw;
        if(isInnerAngle) {
          if(holesTmp.size() > 0 && isHoleIntersectionTri(v[a], v[b], v[c], holesTmp, holeIndexWithClosestPoint, nearestPointIndexOfIntersectingHole)) {
            /*if(v.size() == 4) {
              getClosestHolePoint(v[a], holesTmp, holeIndexWithClosestPoint, nearestPointIndexOfIntersectingHole);
            }*/
            Vec2d p = holesTmp[holeIndexWithClosestPoint][nearestPointIndexOfIntersectingHole];
            if(!isSelfIntersection(v[a], p, vertices) && !isSelfIntersection(v[a], p, v) && !isHoleIntersection(v[a], p, holes)) {
              dbgHoleConnections.push_back(Line(v[a], p));
              for(int i=0; i<holesTmp[holeIndexWithClosestPoint].size()+1; i++) {
                int t = (i + nearestPointIndexOfIntersectingHole) % holesTmp[holeIndexWithClosestPoint].size();
                v.insert(v.begin() + a+1+i, holesTmp[holeIndexWithClosestPoint][t]);
                vertexNames.insert(vertexNames.begin() + a+1+i, currentLetter++);
              }
              v.insert(v.begin() + a+2+holesTmp[holeIndexWithClosestPoint].size(), v[a]);
              vertexNames.insert(vertexNames.begin() + a+2+holesTmp[holeIndexWithClosestPoint].size(), vertexNames[a]);
              holesTmp.erase(holesTmp.begin() + holeIndexWithClosestPoint);
              k = 0;
              //printf("hole added\n");
              //print(vertexNames);
            }
          }
          else if(!isSelfIntersectionTri(v[a], v[b], v[c], verticesTotal) && !isSelfIntersection(v[a], v[c], vertices) && !isSelfIntersection(v[a], v[c], v) && !isHoleIntersection(v[a], v[c], holes)) {
            triangles.push_back(Triangle(v[a], v[b], v[c]));
            v.erase(v.begin()+b);
            vertexNames.erase(vertexNames.begin()+b);
            if(b < a) a--;
            k = 0;
            //printf("triangulated\n");
            //print(vertexNames);
          }
        }
        break;
      }
      a = (a+1) % v.size();
      k++;
      if(k > v.size()) break;
      counter++;
    }

    calculateMassAndCenter();

    return triangles;
  }


  void calculateMassAndCenter() {
    area = 0;
    centerOfMass.set(0, 0);

    for(Triangle &t : triangles) {
      double a = t.getArea();
      Vec2d pos = t.getCenterOfMass();
      area += a;
      centerOfMass += pos * a;
    }

    centerOfMass = centerOfMass / area;
  }

  double getArea() {
    return area;
  }

  Vec2d getCenterOfMass() {
    return centerOfMass;
  }

  const std::string getName() {
    return std::to_string((int)vertices.size()) + "-gon";
  }

  void translate(const Vec2d &t) {
    for(int i=0; i<vertices.size(); i++) {
      vertices[i] += t;
    }
    for(int i=0; i<triangles.size(); i++) {
      triangles[i].translate(t);
    }
  }

  void rotate(double w) {
    double sinA = sin(w), cosA = cos(w);
    for(int i=0; i<vertices.size(); i++) {
      vertices[i].rotate(sinA, cosA);
    }
    for(int i=0; i<triangles.size(); i++) {
      triangles[i].rotate(sinA, cosA);
    }
  }

};




// TODO FIXME looks too messy to be the most optimal way...
static GeomObject *copyGeomObject(GeomObject *geomObject) {
  GeomObject *copy = NULL;
  if(geomObject->type == GeomObject::circle) {
    copy = new Circle(*dynamic_cast<Circle*>(geomObject));
  }
  if(geomObject->type == GeomObject::hoop) {
    copy = new Hoop(*dynamic_cast<Hoop*>(geomObject));
  }
  if(geomObject->type == GeomObject::line) {
    copy = new Line(*dynamic_cast<Line*>(geomObject));
  }
  if(geomObject->type == GeomObject::triangle) {
    copy = new Triangle(*dynamic_cast<Triangle*>(geomObject));
  }
  if(geomObject->type == GeomObject::rect) {
    copy = new Rect(*dynamic_cast<Rect*>(geomObject));
  }
  if(geomObject->type == GeomObject::polygon) {
    copy = new Polygon(*dynamic_cast<Polygon*>(geomObject));
  }
  return copy;
}



struct BoundingRect {
  Vec2d a, b;

  BoundingRect() {}

  BoundingRect(double x, double y, double w = 0, double h = 0) {
    a.x = x - w*0.5;
    a.y = y - h*0.5;
    b.x = x + w*0.5;
    b.y = y + h*0.5;
  }

  BoundingRect(const Vec2d &p, double w = 0, double h = 0) {
    a.x = p.x - w*0.5;
    a.y = p.y - h*0.5;
    b.x = p.x + w*0.5;
    b.y = p.y + h*0.5;
  }

  BoundingRect(const std::vector<Vec2d> &points) {
    set(points);
  }

  void clear() {
    a.set(0, 0);
    b.set(0, 0);
  }

  double getCenterX() {
    return (a.x+b.x) * 0.5;
  }
  double getCenterY() {
    return (a.y+b.y) * 0.5;
  }

  double getWidth() {
    return b.x-a.x;
  }
  double getHeight() {
    return b.y-a.y;
  }

  void set(double x, double y, double w = 0, double h = 0) {
    a.x = x - w*0.5;
    a.y = y - h*0.5;
    b.x = x + w*0.5;
    b.y = y + h*0.5;
  }
  void expand(double x, double y, double w, double h) {
    a.x = min(a.x, x - w*0.5);
    a.y = min(a.y, y - h*0.5);
    b.x = max(b.x, x + w*0.5);
    b.y = max(b.y, y + h*0.5);
  }

  void expand(const BoundingRect &r) {
    a.x = min(a.x, r.a.x);
    a.y = min(a.y, r.a.y);
    b.x = max(b.x, r.b.x);
    b.y = max(b.y, r.b.y);
  }

  // TODO check if coordinate directions match with chipmunk...
  void setMinsMaxs(const double &left, const double &bottom, const double &right, const double &top) {
    a.x = left;
    a.y = bottom;
    b.x = right;
    b.y = top;
  }
  void expandMinsMaxs(const double &left, const double &bottom, const double &right, const double &top) {
    a.x = min(a.x, left);
    a.y = min(a.y, bottom);
    b.x = max(b.x, right);
    b.y = max(b.y, top);
  }

  void set(const std::vector<Vec2d> &points) {
    if(points.size() < 1) {
      a.set(0, 0);
      b.set(0, 0);
      return;
    }
    a = points[0];
    b = points[0];
    for(int i=1; i<points.size(); i++) {
      const Vec2d &p = points[i];
      if(p.x < a.x) a.x = p.x;
      if(p.x > b.x) b.x = p.x;
      if(p.y < a.y) a.y = p.y;
      if(p.y > b.y) b.y = p.y;
    }
  }

  bool isVisible(SDLInterface *sdlInterface) {
    if(a.x > sdlInterface->screenW) return false;
    if(a.y > sdlInterface->screenH) return false;
    if(b.x < 0) return false;
    if(b.y < 0) return false;
    return true;
  }
  bool isVisible(Scene *scene, SDLInterface *sdlInterface) {
    if(scene->toScreenX(a.x) > sdlInterface->screenW) return false;
    if(scene->toScreenY(a.y) > sdlInterface->screenH) return false;
    if(scene->toScreenX(b.x) < 0) return false;
    if(scene->toScreenY(b.y) < 0) return false;
    return true;
  }

  void expand(const std::vector<Vec2d> &points) {
    if(points.size() < 1) {
      return;
    }
    for(int i=0; i<points.size(); i++) {
      const Vec2d &p = points[i];
      if(p.x < a.x) a.x = p.x;
      if(p.x > b.x) b.x = p.x;
      if(p.y < a.y) a.y = p.y;
      if(p.y > b.y) b.y = p.y;
    }
  }


  void render(SDLInterface *sdlInterface) {
    drawRect(a, b, sdlInterface);
  }
  void render(Scene *scene, SDLInterface *sdlInterface) {
    drawRect(scene->toScreen(a), scene->toScreen(b), sdlInterface);
  }



};




inline bool getIntersectionPoint(const Line &line1, const Line &line2, Vec2d &intersectionPoint) {
  return getIntersectionPoint(line1.a, line1.b, line2.a, line2.b, intersectionPoint);
}

inline bool isIntersection(const Line &line1, const Line &line2) {
  return isIntersection(line1.a, line1.b, line2.a, line2.b);
}
