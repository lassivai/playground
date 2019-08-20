#pragma once
#include "mathl.h"


inline Uint8 toByte(float f) {
  return Uint8(clamp(f * 255.9999999, 0.0, 255.0));
}
inline Uint8 toByte(double f) {
  return Uint8(clamp(f * 255.9999999, 0.0, 255.0));
}
inline Uint8 toByte255(double f) {
  return Uint8(clamp(f, 0.0, 255.0));
}

inline unsigned int toRGBA(double r, double g, double b, double a) {
  unsigned char rByte = toByte(r);
  unsigned char gByte = toByte(g);
  unsigned char bByte = toByte(b);
  unsigned char aByte = toByte(a);
  return aByte | (bByte<<8) | (gByte<<16) | (rByte<<24);
}

inline unsigned int toRGBA255(double r, double g, double b, double a) {
  unsigned char rByte = toByte255(r);
  unsigned char gByte = toByte255(g);
  unsigned char bByte = toByte255(b);
  unsigned char aByte = toByte255(a);
  return aByte | (bByte<<8) | (gByte<<16) | (rByte<<24);
}

inline unsigned int toRGBA(const Vec3d &col) {
  unsigned char rByte = toByte(col.x);
  unsigned char gByte = toByte(col.y);
  unsigned char bByte = toByte(col.z);
  return 255 | (bByte<<8) | (gByte<<16) | (rByte<<24);
}

inline unsigned int toRGBA(const Vec4d &col) {
  unsigned char rByte = toByte(col.x);
  unsigned char gByte = toByte(col.y);
  unsigned char bByte = toByte(col.z);
  unsigned char aByte = toByte(col.w);
  return aByte | (bByte<<8) | (gByte<<16) | (rByte<<24);
}

inline unsigned int toRGBA(const Vec3d &col, double a) {
  unsigned char rByte = toByte(col.x);
  unsigned char gByte = toByte(col.y);
  unsigned char bByte = toByte(col.z);
  unsigned char aByte = toByte(a);
  return aByte | (bByte<<8) | (gByte<<16) | (rByte<<24);
}

inline unsigned int toA(const unsigned int &rgba) {
  return rgba & 255;
}
inline unsigned int toB(const unsigned int &rgba) {
  return (rgba & (255<<8)) >> 8;
}
inline unsigned int toG(const unsigned int &rgba) {
  return (rgba & (255<<16)) >> 16;
}
inline unsigned int toR(const unsigned int &rgba) {
  return (rgba & (255<<24)) >> 24;
}

inline double rgbToAlpha(double r, double g, double b) {
  double minValue = min(r, min(g, b));
  double maxValue = max(r, max(g, b));
  return (minValue+maxValue) / 2.0;
}


static const unsigned int Black = 0x000000FF;
static const unsigned int White = 0xFFFFFFFF;
static const unsigned int Gray50 = 0x7F7F7FFF;
static const unsigned int Purple = 0x7A005FFF;

inline double hue2rgb(double p, double q, double t) {
  if(t < 0.0) t += 1.0;
  if(t > 1.0) t -= 1.0;
  if(t < 1.0/6.0) return p + (q - p) * 6.0 * t;
  if(t < 1.0/2.0) return q;
  if(t < 2.0/3.0) return p + (q - p) * (2.0/3.0 - t) * 6.0;
  return p;
}

static Vec3d hslToRgb(double h, double s, double v) {
  Vec3d col;
  h = fract(h / TAU);
  //h = fract(modff(h, TAU));
  //h = fract(h);

  if(s == 0) {
    col = Vec3d(v, v, v);
  }
  else {
    double q = v < 0.5 ? v * (1.0 + s) : v + s - v * s;
    double p = 2.0 * v - q;

    col.x = hue2rgb(p, q, h + 1.0/3.0);
    col.y = hue2rgb(p, q, h);
    col.z = hue2rgb(p, q, h - 1.0/3.0);
  }

  return col;
}


static Vec3d rgbToHsl(double r, double g, double b)
{
    Vec3d hsv;
    double minv = min(r, min(g, b));
    double maxv = max(r, max(g, b));
    double delta = maxv - minv;

    hsv.z = (maxv + minv) * 0.5;

    if (delta < 0.00001) {
        hsv.y = 0;
        hsv.x = 0;
        return hsv;
    }
    if(maxv > 0.0) {
        hsv.y = (hsv.z <= 0.5) ? (delta / (maxv + minv)) : (delta / (2 - maxv - minv));
    }
    else {
        hsv.y = 0.0;
        hsv.x = 0.0;
        return hsv;
    }
    if(r >= maxv) {
      hsv.x = (g - b) / delta;
    }
    else if(g >= maxv) {
      hsv.x = 2.0 + (b - r) / delta;
    }
    else {
      hsv.x = 4.0 + (r - g) / delta;
    }
    //hsv.x = modff(hsv.x, 6.0);
    hsv.x *= TAU/6.0;


    return hsv;
}

static Vec3d rgbToHsv(double r, double g, double b)
{
    Vec3d hsv;
    double minv = min(r, min(g, b));
    double maxv = max(r, max(g, b));
    double delta = maxv - minv;

    hsv.z = maxv;

    if (delta < 0.00001) {
        hsv.y = 0;
        hsv.x = 0;
        return hsv;
    }
    if(maxv > 0.0) {
        hsv.y = delta / maxv;
    }
    else {
        hsv.y = 0.0;
        hsv.x = 0.0;
        return hsv;
    }
    if(r >= maxv) {
      hsv.x = (g - b) / delta;
    }
    else if(g >= maxv) {
      hsv.x = 2.0 + (b - r) / delta;
    }
    else {
      hsv.x = 4.0 + (r - g) / delta;
    }
    hsv.x *= TAU/6.0;

    return hsv;
}


static Vec3d hsvToRgb(double h, double s, double v) {
  h = modff(h, TAU);
  while(h < 0) h += TAU;
  //while(h > TAU) h += TAU;

  s = clamp(s, 0.0, 1.0);
  v = clamp(v, 0.0, 1.0);

  float k = h/TAU*6.0;
  int i = int(k);

  float ff = k - i;
  float p = v * (1.0 - s);
  float q = v * (1.0 - (s * ff));
  float t = v * (1.0 - (s * (1.0 - ff)));

  Vec3d col;
  if(i == 0) {
    col.set(v, t, p);
  }
  else if(i == 1) {
    col.set(q, v, p);
  }
  else if(i == 2) {
    col.set(p, v, t);
  }
  else if(i == 3) {
    col.set(p, q, v);
  }
  else if(i == 4) {
    col.set(t, p, v);
  }
  else {
    col.set(v, p, q);
  }
  return col;
}

/*inline Vec4d hsvToRgb(const Vec4d &hsv) {
  Vec3d rgb = hsvToRgb(hsv.x, hsv.y, hsv.z);
  return Vec4(rgb.x, rgb.y, rgb.z, hsv.w);
}
inline Vec4d hslToRgb(const Vec4d &hsl) {
  Vec3d rgb = hslToRgb(hsl.x, hsl.y, hsl.z);
  return Vec4(rgb.x, rgb.y, rgb.z, hsl.w);
}
inline Vec4d rgbToHsv(const Vec4d &rgb) {
  Vec3d hsv = rgbToHsv(rgb.x, rgb.y, rgb.z);
  return Vec4(hsv.x, hsv.y, hsv.z, rgb.w);
}
inline Vec4d rgbToHsl(const Vec4d &rgb) {
  Vec3d hsl = rgbToHsl(rgb.x, rgb.y, rgb.z);
  return Vec4(hsl.x, hsl.y, hsl.z, rgb.w);
}*/