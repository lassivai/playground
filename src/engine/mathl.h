#pragma once
#include <ctgmath>
#include "cereal_init.h"
#include "util.h"

inline int modff(int a, int b) {
  while(a < 0) a += b;
  while(a > b) a -= b;
  return a;
}

template<class T>
inline T modff(T a, T b, T c) {
  while(a < b) a += c-b;
  while(a > c) a -= c-b;
  return a;
}

inline double fract(double a) {
  return a-floor(a);
}

inline double modfff(double a, double b) {
  if(b == 0) return 0;
  return fract(a/b) * b;
}
inline double modfff(double a, double b, double c) {
  double t = c-b;
  if(t == 0) return 0;
  return fract((a-t)/b) * b + t;
}
// continuous % pattern into negative values
template<class T> inline T modulo(T a, T b) {
  return (b + a % b) % b;
}

// return value -PI - PI
inline double angleBetween(double a, double b) {
  double t = modfff(b - a, 2.0*PI);
  if(t >= -PI && t <= PI) return t;
  else if(t > PI) return 2.0*PI - t;
  else return -2.0*PI - t;
}



template<class T>
T sign(T value) {
  return value < 0 ? -1 : 1;
}

inline double triangleWave(double x, double skewness) {
  double d = max(skewness, 1.00000001);
  double half = step(1.0/d, fract(x));
  double slope1 = d*fract(x);
  double slope2 = 1.0/d == 1.0 ? 0 : 1.0/(1.0-1.0/d)*fract(x-1.0/d);
  return (slope2*half + (1.0-slope1)*(1.0-half))*2.0 - 1.0;
}
inline double triangleWave(double x) {
  double half = step(0.5, fract(x));
  double slope1 = 2.0*fract(x);
  double slope2 = 2.0*fract(x-0.5);
  return (slope2*half + (1.0-slope1)*(1.0-half))*2.0 - 1.0;
}

inline double squareWave(double x, double skewness) {
  double d = clamp(skewness, 1e-6, 1-1e-6);
  return 2.0*step(d, fract(x)) - 1.0;
}

inline double squareWave(double x) {
  return 2.0*step(0.5, fract(x)) - 1.0;
}

inline double sawWave(double x) {
  return 2.0*fract(x) - 1.0;
}


inline unsigned int toA(const unsigned int &rgba);
inline unsigned int toB(const unsigned int &rgba);
inline unsigned int toG(const unsigned int &rgba);
inline unsigned int toR(const unsigned int &rgba);
inline unsigned int toRGBA255(double r, double g, double b, double a);




struct Vec4d {

  static const Vec4d Zero;

  double x = 0, y = 0, z = 0, w = 0;

  template<class Archive>
  void serialize(Archive &ar) {
    ar(x, y, z, w);
  }

  Vec4d() {}

  Vec4d(double x, double y, double z, double w) {
    this->x = x;
    this->y = y;
    this->z = z;
    this->w = w;
  }
  Vec4d(const unsigned int &rgba) {
    this->x = toR(rgba);
    this->y = toG(rgba);
    this->z = toB(rgba);
    this->w = toA(rgba);
  }

  void set(const Vec4d &v) {
    this->x = v.x;
    this->y = v.y;
    this->z = v.z;
    this->w = v.w;
  }

  void set(double x, double y, double z, double w) {
    this->x = x;
    this->y = y;
    this->z = z;
    this->w = w;
  }

  inline unsigned int toColorRGBA() {
    return toRGBA255(x, y, z, w);
  }

  inline void operator=(const Vec4d &v) {
    this->x = v.x;
    this->y = v.y;
    this->z = v.z;
    this->w = v.w;
  }
  inline void operator+=(const Vec4d &v) {
    this->x += v.x;
    this->y += v.y;
    this->z += v.z;
    this->w += v.w;
  }
  inline void operator-=(const Vec4d &v) {
    this->x -= v.x;
    this->y -= v.y;
    this->z -= v.z;
    this->w -= v.w;
  }
  inline void operator*=(const Vec4d &v) {
    this->x *= v.x;
    this->y *= v.y;
    this->z *= v.z;
    this->w *= v.w;
  }
  inline void operator*=(double t) {
    this->x *= t;
    this->y *= t;
    this->z *= t;
    this->w *= t;
  }
  inline void operator/=(double t) {
    this->x /= t;
    this->y /= t;
    this->z /= t;
    this->w /= t;
  }
  inline Vec4d operator+(const Vec4d &v) const {
    return Vec4d(x + v.x, y + v.y, z + v.z, w + v.w);
  }
  inline Vec4d operator-(const Vec4d &v) const {
    return Vec4d(x - v.x, y - v.y, z - v.z, w - v.w);
  }
  inline Vec4d operator*(double t) const {
    return Vec4d(x * t, y * t, z * t, w *t);
  }
  inline Vec4d operator/(double t) const {
    return Vec4d(x / t, y / t, z / t, w / t);
  }
  inline Vec4d operator*(const Vec4d &v) const {
    return Vec4d(x * v.x, y * v.y, z * v.z, w * v.w);
  }
  inline Vec4d operator/(const Vec4d &v) const {
    return Vec4d(x / v.x, y / v.y, z / v.z, w / v.w);
  }
  
  inline bool operator==(const Vec4d &v) const {
    return x == v.x && y == v.y && z == v.z && w == v.w;
  }

  inline bool operator!=(const Vec4d &v) const {
    return x != v.x || y != v.y || z != v.z || w != v.w;
  }

};


struct Vec3i {
  int x, y, z;
  template<class Archive> void serialize(Archive &ar) {
    ar(x, y, z);
  }
  Vec3i() {}
  Vec3i(int x, int y, int z) {
    this->x = x;
    this->y = y;
    this->z = z;
  }
  inline void set(int x, int y, int z) {
    this->x = x;
    this->y = y;
    this->z = z;
  }
  inline bool operator==(const Vec3i &v) const {
    return x == v.x && y == v.y && z == v.z;
  }

  inline bool operator!=(const Vec3i &v) const {
    return x != v.x || y != v.y || z != v.z;
  }
};


struct Vec2i {
  int x, y;
  template<class Archive> void serialize(Archive &ar) {
    ar(x, y);
  }
  Vec2i() {}
  Vec2i(int x, int y) {
    this->x = x;
    this->y = y;
  }
  inline void set(int x, int y) {
    this->x = x;
    this->y = y;
  }
  inline bool operator==(const Vec2i &v) const {
    return x == v.x && y == v.y;
  }

  inline bool operator!=(const Vec2i &v) const {
    return x != v.x || y != v.y;
  }
};






struct Vec3d {
  static const Vec3d Zero;

  double x = 0, y = 0, z = 0;

  template<class Archive> void serialize(Archive &ar) {
    ar(x, y, z);
  }

  Vec3d() {}
  Vec3d(double x, double y, double z) {
    this->x = x;
    this->y = y;
    this->z = z;
  }
  Vec3d(double t) {
    this->x = t;
    this->y = t;
    this->z = t;
  }
  inline void operator=(const Vec3d &v) {
    this->x = v.x;
    this->y = v.y;
    this->z = v.z;
  }
  inline void operator+=(const Vec3d &v) {
    this->x += v.x;
    this->y += v.y;
    this->z += v.z;
  }
  inline void operator-=(const Vec3d &v) {
    this->x -= v.x;
    this->y -= v.y;
    this->z -= v.z;
  }
  inline void operator*=(double t) {
    this->x *= t;
    this->y *= t;
    this->z *= t;
  }
  inline void operator/=(double t) {
    this->x /= t;
    this->y /= t;
    this->z /= t;
  }
  inline Vec3d operator+(const Vec3d &v) {
    return Vec3d(x + v.x, y + v.y, z + v.z);
  }
  inline Vec3d operator-(const Vec3d &v) {
    return Vec3d(x - v.x, y - v.y, z - v.z);
  }
  inline Vec3d operator*(double t) {
    return Vec3d(x * t, y * t, z * t);
  }
  inline Vec3d operator/(double t) {
    return Vec3d(x / t, y / t, z / t);
  }

  inline void set(double x, double y, double z) {
    this->x = x;
    this->y = y;
    this->z = z;
  }
  inline void add(double x, double y, double z) {
    this->x += x;
    this->y += y;
    this->z += z;
  }
  inline void set(double t) {
    this->x = t;
    this->y = t;
    this->z = t;
  }
  inline void min(const Vec3d &v) {
    x = x < v.x ? x : v.x;
    y = y < v.x ? y : v.y;
    z = z < v.x ? z : v.z;
  }
  inline void max(const Vec3d &v) {
    x = x > v.x ? x : v.x;
    y = y > v.x ? y : v.y;
    z = z > v.x ? z : v.z;
  }
  inline double max() {
    return x > y ? (x > z ? x : z) : (y > z ? y : z);
  }
  inline double min() {
    return x < y ? (x < z ? x : z) : (y < z ? y : z);
  }
  inline void randomize() {
    x = Random::getDouble();
    y = Random::getDouble();
    z = Random::getDouble();
  }

  inline void normalize() {
    double d = sqrt(x*x + y*y * z*z);
    x = x / d;
    y = y / d;
    z = z / d;
  }

  inline double length() {
    return sqrt(x*x + y*y + z*z);
  }

  static Vec3d sub(const Vec3d &a, const Vec3d &b) {
    return Vec3d(a.x - b.x, a.y - b.y, a.z - b.z);
  }

};




/*struct Vec3ul {
  unsigned long a = 0, b = 0, c = 0;

  Vec3ul() {}
  Vec3ul(unsigned long a, unsigned long b, unsigned long c) {
    this->a = a;
    this->b = b;
    this->c = c;
  }
  Vec3ul(unsigned long t) {
    this->a = t;
    this->b = t;
    this->c = t;
  }
  inline void set(unsigned long a, unsigned long b, unsigned long c) {
    this->a = a;
    this->b = b;
    this->c = c;
  }
  inline void add(unsigned long a, unsigned long b, unsigned long c) {
    this->a += a;
    this->b += b;
    this->c += c;
  }
  inline void set(unsigned long t) {
    this->a = t;
    this->b = t;
    this->c = t;
  }
  inline void min(const Vec3ul &v) {
    a = a < v.a ? a : v.a;
    b = b < v.a ? b : v.b;
    c = c < v.a ? c : v.c;
  }
  inline void max(const Vec3ul &v) {
    a = a > v.a ? a : v.a;
    b = b > v.a ? b : v.b;
    c = c > v.a ? c : v.c;
  }
};*/

struct Vec2d
{
  static const Vec2d Zero;

  double x, y;

  template<class Archive> void serialize(Archive &ar) {
    ar(x, y);
  }

  Vec2d() {
    this->x = 0;
    this->y = 0;
  }
  Vec2d(double x, double y) {
    this->x = x;
    this->y = y;
  }

  Vec2d(const Vec2d &v) {
    this->x = v.x;
    this->y = v.y;
  }

  inline void operator=(const Vec2d &v) {
    this->x = v.x;
    this->y = v.y;
  }
  

  /*inline void operator=(double t) {
    this->x = t;
    this->y = t;
  }*/
  inline void operator+=(const Vec2d &v) {
    this->x += v.x;
    this->y += v.y;
  }
  inline void operator-=(const Vec2d &v) {
    this->x -= v.x;
    this->y -= v.y;
  }
  inline void operator*=(const Vec2d &v) {
    this->x *= v.x;
    this->y *= v.y;
  }
  inline void operator*=(double t) {
    this->x *= t;
    this->y *= t;
  }
  inline void operator/=(double t) {
    this->x /= t;
    this->y /= t;
  }
  inline void operator+=(double t) {
    this->x += t;
    this->y += t;
  }
  inline void operator-=(double t) {
    this->x -= t;
    this->y -= t;
  }

  inline Vec2d operator+(const Vec2d &v) const {
    return Vec2d(x + v.x, y + v.y);
  }
  inline Vec2d operator-(const Vec2d &v) const {
    return Vec2d(x - v.x, y - v.y);
  }
  inline Vec2d operator*(double t) const {
    return Vec2d(x * t, y * t);
  }
  inline Vec2d operator*(const Vec2d &v) const {
    return Vec2d(x * v.x, y * v.y);
  }
  inline Vec2d operator/(double t) const {
    return Vec2d(x / t, y / t);
  }

  inline bool operator==(const Vec2d &v) const {
    return x == v.x && y == v.y;
  }

  inline bool operator!=(const Vec2d &v) const {
    return x != v.x || y != v.y;
  }
  inline Vec2d operator-() const {
    return Vec2d(-x, -y);
  }


  inline void set(double x, double y) {
    this->x = x;
    this->y = y;
  }
  inline void set(const Vec2d &v) {
    this->x = v.x;
    this->y = v.y;
  }
  inline bool isZero() {
    return x == 0.0 && y == 0.0;
  }
  Vec2d(double a, double b, bool polarForm) {
    if(polarForm) {
      setPolar(a, b);
    }
    else {
      this->x = a;
      this->y = b;
    }
  }
  inline void setPolar(double r, double a) {
    this->x = r * cos(a);
    this->y = r * sin(a);
  }
  inline void add(const Vec2d &v) {
    x += v.x;
    y += v.y;
  }
  inline void add(double x, double y) {
    this->x += x;
    this->y += y;
  }
  inline void sub(const Vec2d &v) {
    x -= v.x;
    y -= v.y;
  }
  inline void sub(double x, double y) {
    this->x -= x;
    this->y -= y;
  }
  inline void mul(double t) {
    x *= t;
    y *= t;
  }
  inline double length() const {
    return sqrt(x*x + y*y);
  }
  inline void normalize() {
    double len = sqrt(x*x + y*y);
    x /= len;
    y /= len;
  }
  inline void setLength(double length) {
    double len = sqrt(x*x + y*y);
    x = x*length/len;
    y = y*length/len;
  }

  inline void rotate(double a) {
    double tmpX = cos(a) * x - sin(a) *y;
    y = sin(a) * x + cos(a) * y;
    x = tmpX;
  }

  inline void rotate(double sinA, double cosA) {
    double tmpX = cosA * x - sinA *y;
    y = sinA * x + cosA * y;
    x = tmpX;
  }

  inline void randomize(double x1, double x2, double y1, double y2) {
    this->x = Random::getDouble(x1, x2);
    this->y = Random::getDouble(y1, y2);
  }
  inline void randomizeLengthAndDirection(double minLength, double maxLength) {
    double len = Random::getDouble(minLength, maxLength);
    double a = Random::getDouble(0, TAU);
    this->x = len * cos(a);
    this->y = len * sin(a);
  }

  inline double getRandomDoubleFromTheRange() {
    return Random::getDouble(x, y);
  }
  /*inline Vec2d getRandomVec2dFromTheRange() {
    return Random::getDouble(x, y);
  }*/

  inline void complexPower(double power) {
    double a = atan2(y, x);
    double r = sqrt(x*x+y*y);
    r = pow(r, power);
    a = a * power;
    x = r * cos(a);
    y = r * sin(a);
  }

};

static inline Vec2d round(const Vec2d &v) {
  //return v;
  return Vec2d(round(v.x), round(v.y));
}

static inline Vec2d getRandomVec2d(double minLength, double maxLength) {
  double r = Random::getDouble(minLength, maxLength);
  double a = Random::getDouble(0, TAU);
  return Vec2d(r * cos(a), r * sin(a));
}

static inline bool equals(const Vec2d &a, const Vec2d &b) {
  return a.x == b.x && a.y == b.y;
}
inline Vec2d sub(const Vec2d &a, const Vec2d &b) {
  return Vec2d(a.x-b.x, a.y-b.y);
}
static inline Vec2d add(const Vec2d &a, const Vec2d &b) {
  return Vec2d(a.x+b.x, a.y+b.y);
}
static inline Vec2d mul(const Vec2d &a, double d) {
  return Vec2d(a.x*d, a.y*d);
}
static inline Vec2d mul(const Vec2d &a, double d, double f) {
  return Vec2d(a.x*d, a.y*f);
}
static inline Vec2d complexPower(const Vec2d &w, double power) {
  double a = atan2(w.y, w.x);
  double r = sqrt(w.x*w.x+w.y*w.y);
  r = pow(r, power);
  a = a * power;
  return Vec2d(r * cos(a), r * sin(a));
}
static inline double distance(const Vec2d &a, const Vec2d &b) {
  return sqrt((a.x-b.x)*(a.x-b.x) + (a.y-b.y)*(a.y-b.y));
}

static inline double dot(const Vec2d &a, const Vec2d &b) {
  return (a.x*b.x + a.y*b.y);
}

static inline double cross(const Vec2d &a, const Vec2d &b) {
  return (a.x*b.y - a.y*b.x);
}


static inline double angle(const Vec2d &a, const Vec2d &b) {
  double d = (a.length() * b.length());
  if(d == 0) return 0;
  return acos(dot(a, b) / d);
}
/*inline double angle(const Vec2d &a, const Vec2d &b) {
  return atan2(b.y-a.y, b.x-a.x);
}*/

static inline double dotNorm(const Vec2d &a, const Vec2d &b) {
  double d = (a.length() * b.length());
  if(d == 0) return 0;
  return dot(a, b) / d;
}

static inline double catmullRom(double t, double q, double p0, double p1, double p2, double p3) {
  double c0 = p1;
  double c1 = q*(p2-p0);
  double c2 = 3.0*(p2-p1) -  q*(p3-p1) - 2.0*q*(p2-p0);
  double c3 = -2.0*(p2-p1) +  q*(p3-p1) + q*(p2-p0);

  return c0 + t*c1 + t*t*c2 + t*t*t*c3;
}

static inline Vec2d catmullRom(double t, double q, const Vec2d &p0, const Vec2d &p1, const Vec2d &p2, const Vec2d &p3) {
  Vec2d v;
  v.x = catmullRom(t, q, p0.x, p1.x, p2.x, p3.x);
  v.y = catmullRom(t, q, p0.y, p1.y, p2.y, p3.y);
  return v;
}

static inline Vec3d catmullRom(double t, double q, const Vec3d &p0, const Vec3d &p1, const Vec3d &p2, const Vec3d &p3) {
  Vec3d v;
  v.x = catmullRom(t, q, p0.x, p1.x, p2.x, p3.x);
  v.y = catmullRom(t, q, p0.y, p1.y, p2.y, p3.y);
  v.z = catmullRom(t, q, p0.z, p1.z, p2.z, p3.z);
  return v;
}

static inline Vec4d catmullRom(double t, double q, const Vec4d &p0, const Vec4d &p1, const Vec4d &p2, const Vec4d &p3) {
  Vec4d v;
  v.x = catmullRom(t, q, p0.x, p1.x, p2.x, p3.x);
  v.y = catmullRom(t, q, p0.y, p1.y, p2.y, p3.y);
  v.z = catmullRom(t, q, p0.z, p1.z, p2.z, p3.z);
  v.w = catmullRom(t, q, p0.w, p1.w, p2.w, p3.w);
  return v;
}

static inline Vec4d biCatmullRom(double fx, double fy, double q, const Vec4d *p) {
  Vec4d v0 = catmullRom(fx, q, p[0], p[1], p[2], p[3]);
  Vec4d v1 = catmullRom(fx, q, p[4], p[5], p[6], p[7]);
  Vec4d v2 = catmullRom(fx, q, p[8], p[9], p[10], p[11]);
  Vec4d v3 = catmullRom(fx, q, p[12], p[13], p[14], p[15]);

  return catmullRom(fy, q, v0, v1, v2, v3);
}


/*struct Rect {
  Vec2d a, b;
  Rect() {
    a.set(0, 0);
    b.set(0, 0);
  }
};
*/


struct Matrix3d
{
  double m[9];

  inline void setIdentity() {
    m[0] = 1; m[1] = 0; m[2] = 0; m[3] = 0; m[4] = 1; m[5] = 0; m[6] = 0; m[7] = 0; m[8] = 1;
  }
  inline void setZeros() {
    for(int i=0; i<9; i++) {
      m[i] = 0;
    }
  }
  inline void operator=(const Matrix3d &w) {
    for(int i=0; i<9; i++) {
      m[i] = w.m[i];
    }
  }
  inline void operator+=(const Matrix3d &w) {
    for(int i=0; i<9; i++) {
      m[i] += w.m[i];
    }
  }

  inline void operator-=(const Matrix3d &w) {
    for(int i=0; i<9; i++) {
      m[i] -= w.m[i];
    }
  }

  // FIXME test
  void operator*=(const Matrix3d &w) {
    Matrix3d r;
    r.setZeros();
    for(int i=0; i<9; i++) {
      int rowA = (i/3) * 3, columnB = i % 3;
      for(int s=0; s<3; s++) {
        r.m[i] += m[rowA + s] * w.m[columnB + s*3];
      }
    }
    for(int i=0; i<9; i++) {
      m[i] = r.m[i];
    }
  }


  /*
    0   1   2
    3   4   5
    6   7   8

    1   0   dx
    0   1   dy
    0   0   1
  */

  double getDeterminant() {
    double det;
    det =  m[0] * (m[4]*m[8] - m[5]*m[7]);
    det -= m[1] * (m[3]*m[8] - m[5]*m[6]);
    det += m[2] * (m[3]*m[7] - m[4]*m[6]);

    det -= m[3] * (m[1]*m[8] - m[2]*m[7]);
    det += m[4] * (m[0]*m[8] - m[2]*m[6]);
    det -= m[5] * (m[0]*m[7] - m[1]*m[6]);

    det += m[6] * (m[1]*m[5] - m[2]*m[4]);
    det -= m[7] * (m[0]*m[5] - m[2]*m[3]);
    det += m[8] * (m[0]*m[4] - m[1]*m[3]);
    return det;
  }
  Matrix3d getInverse() {
    Matrix3d inv;
    double md0 = (m[4]*m[8] - m[5]*m[7]);
    double md1 = (m[3]*m[8] - m[5]*m[6]);
    double md2 = (m[3]*m[7] - m[4]*m[6]);

    double md3 = (m[1]*m[8] - m[2]*m[7]);
    double md4 = (m[0]*m[8] - m[2]*m[6]);
    double md5 = (m[0]*m[7] - m[1]*m[6]);

    double md6 = (m[1]*m[5] - m[2]*m[4]);
    double md7 = (m[0]*m[5] - m[2]*m[3]);
    double md8 = (m[0]*m[4] - m[1]*m[3]);

    double det = m[0]*md0 - m[1]*md1 + m[2]*md2 - m[3]*md3 + m[4]*md4 - m[5]*md5 + m[6]*md6 - m[7]*md7 + m[8]*md8;

    inv.m[0] = md0 / det;
    inv.m[1] = md3 / det;
    inv.m[2] = md6 / det;

    inv.m[3] = md1 / det;
    inv.m[4] = md4 / det;
    inv.m[5] = md7 / det;

    inv.m[6] = md2 / det;
    inv.m[7] = md5 / det;
    inv.m[8] = md8 / det;

    return inv;
  }


  inline Vec2d operator*(const Vec2d &v) {
    return Vec2d(m[0]*v.x + m[1]*v.y + m[2],  m[3]*v.x + m[4]*v.y + m[5]);
  }



  // TODO more optimized implementation for transformations
  inline void setTranslate(double dx, double dy) {
    setIdentity();
    m[2] = dx;
    m[5] = dy;
  }
  inline void setRotate(double da) {
    setIdentity();
    m[0] = cos(da);
    m[1] = -sin(da);
    m[3] = sin(da);
    m[4] = cos(da);
  }
  inline void setScale(double s) {
    setIdentity();
    m[0] = s;
    m[4] = s;
  }
  /*inline void transform(double x, double y, double rot, double scale) {
    m = getTranslate(x, y);
    m *= getRotate(rot);
    m *= getScale(scale);
  }*/
};

static inline double length(double a, double b, double c) {
  return sqrt(a*a + b*b + c*c);
}



struct CurvePoint : public Vec2d {
  //enum InterpolationMode { Linear, Cubic };
  //InterpolationMode interpolationMode = Cubic;
  double cubicInterpolationCurvature = 0;
  
  CurvePoint() {}
  CurvePoint(const Vec2d &v) : Vec2d(v) {}
  CurvePoint(double x, double y, double c) : Vec2d(x, y) {
    cubicInterpolationCurvature = c;
  }
  void set(double x, double y) {
    Vec2d::set(x, y);
  }
  void set(double x, double y, double c) {
    Vec2d::set(x, y);
    cubicInterpolationCurvature = c;
  }
  
  //bool snapToGrid;
};


static const char hexDigits[] = {"0123456789ABCDEF"};

static std::string byteToHex(unsigned char byte) {
  std::string hex = "00";
  hex[0] = hexDigits[(byte >> 4) & 15];
  hex[1] = hexDigits[byte & 15];
  return hex;
}



static std::string toHexadecimalColor(double r, double g, double b, double a, int formatting = 0) {
  r = clamp(r, 0.0, 1.0);
  g = clamp(g, 0.0, 1.0);
  b = clamp(b, 0.0, 1.0);
  a = clamp(a, 0.0, 1.0);

  unsigned char rByte = floor(255.999*r);
  unsigned char gByte = floor(255.999*g);
  unsigned char bByte = floor(255.999*b);
  unsigned char aByte = floor(255.999*a);
  
  std::string str;
  
  if(formatting == 1) {
    str = "#" + byteToHex(rByte) + byteToHex(gByte) + byteToHex(bByte) + byteToHex(aByte);
  }
  else if(formatting == 2) {
    str = byteToHex(rByte) + "." + byteToHex(gByte) + "." + byteToHex(bByte) + "." + byteToHex(aByte);
  }
  else {
    str = byteToHex(rByte) + byteToHex(gByte) + byteToHex(bByte) + byteToHex(aByte);
  }
  return str;
}

static std::string toHexadecimalColor(const Vec4d &color, int formatting = 0) {
  return toHexadecimalColor(color.x, color.y, color.z, color.w, formatting);
}

static inline Vec2d lerp(const Vec2d &a, const Vec2d &b, double t) {
  return Vec2d((b.x-a.x)*t + a.x, (b.y-a.y)*t + a.y);
}

/*Vec4d fromHexadecimalColor(std::string hex) {
  if(hex.size() != 7) return;

}*/
