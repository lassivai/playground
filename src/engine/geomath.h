#pragma once

#include "mathl.h"
//#include "geometry.h"



/*bool isIntersection(const Vec2d &a, const Vec2d &b, const Vec2d &c, const Vec2d &d) {
  Vec2d ab(b.x-a.x, b.y - a.y);
  Vec2d cd(d.x-c.x, d.y - c.y);
  double t = (c.y - (c.x - a.x) * ab.y / ab.x - a.y) / (cd.x * ab.y / ab.x - cd.y);
  double s = (c.x + t * cd.x - a.x) / ab.x;
  return s >= 0.0 && s <= 1.0 && t >= 0.0 && t <= 1.0;
}*/

// TODO find out is there more efficient way to check the winding order
inline bool isIntersection(const Vec2d &a, const Vec2d &b, const Vec2d &c, const Vec2d &d) {
  return cross(a-c, b-c)*cross(a-d, b-d)<0 && cross(c-a, d-a)*cross(c-b, d-b)<0;
}

inline bool isCCW(const Vec2d &a, const Vec2d &b, const Vec2d &c) {
  return cross(b-a, c-a) < 0;
}

static bool isPointWithinTriangle(const Vec2d &p, const Vec2d &a, const Vec2d &b, const Vec2d &c) {
  bool i = isCCW(a, b, p);
  bool j = isCCW(b, c, p);
  bool k = isCCW(c, a, p);
  return (i && j && k) || (!i && !j && !k);
  //return i;
}



static bool getIntersectionPoint(const Vec2d &a, const Vec2d &b, const Vec2d &c, const Vec2d &d, Vec2d &intersectionPoint) {
  Vec2d ab(b.x-a.x, b.y - a.y);
  Vec2d cd(d.x-c.x, d.y - c.y);
  double t = (c.y - (c.x - a.x) * ab.y / ab.x - a.y) / (cd.x * ab.y / ab.x - cd.y);
  double s = (c.x + t * cd.x - a.x) / ab.x;
  intersectionPoint.set(c.x + t * cd.x, c.y + t * cd.y);
  return s >= 0.0 && s <= 1.0 && t >= 0.0 && t <= 1.0;
}



static bool getIntersectionST(const Vec2d &a, const Vec2d &b, const Vec2d &c, const Vec2d &d, Vec2d &st) {
  Vec2d ab(b.x-a.x, b.y - a.y);
  Vec2d cd(d.x-c.x, d.y - c.y);
  ab.normalize();
  cd.normalize();

  st.y = (c.y - (c.x - a.x) * ab.y / ab.x - a.y) / (cd.x * ab.y / ab.x - cd.y);
  st.x = (c.x + st.y * cd.x - a.x) / ab.x;

  return st.x >= 0.0 && st.x <= 1.0 && st.y >= 0.0 && st.y <= 1.0;
}

static bool getIntersection(const Vec2d &a, const Vec2d &b, const Vec2d &c, const Vec2d &d, Vec2d &point, Vec2d &st) {
  Vec2d ab(b.x-a.x, b.y - a.y);
  Vec2d cd(d.x-c.x, d.y - c.y);
  ab.normalize();
  cd.normalize();

  st.y = (c.y - (c.x - a.x) * ab.y / ab.x - a.y) / (cd.x * ab.y / ab.x - cd.y);
  st.x = (c.x + st.y * cd.x - a.x) / ab.x;
  point.set(c.x + st.y * cd.x, c.y + st.y * cd.y);

  return st.x >= 0.0 && st.x <= 1.0 && st.y >= 0.0 && st.y <= 1.0;
}

/*
a + s * ab = c + t * cd


a.x + s * ab.x = c.x + t * cd.x
a.y + s * ab.y = c.y + t * cd.y

s = (c.x + t * cd.x - a.x) / ab.x

a.y + ((c.x + t * cd.x - a.x) / ab.x) * ab.y = c.y + t * cd.y

t  = (c.y - (c.x - a.x) * ab.y / ab.x - a.y) / (cd.x * ab.y / ab.x - cd.y)
*/
