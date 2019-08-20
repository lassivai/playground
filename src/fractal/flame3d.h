#pragma once
#include "../engine/sketch.h"
#include "fructs.h"






static int fractalFlameIterator3D(const Vec3d &p, std::vector<unsigned long> &voxels, FractalParameters *fractPars) {

  if(fractPars->updateIterationRoute) {
    fractPars->numIterationRoutePoints = 0;
  }

  int maxIter = int(fractPars->maxIter);

  int i = 0;

  Vec3d w = p;
  Vec3d q(0, 0, 0);

  while(i < maxIter) {
    for(int n=0; n<fractPars->numActiveVariations; n++) {
      if(fractPars->flamePars[n].variation < 0 || fractPars->flamePars[n].variation >= FractalParameters::numFlameVariations) continue;
      q.x = fractPars->flamePars[n].p3d[1] * w.x +
            fractPars->flamePars[n].p3d[2] * w.y +
            fractPars->flamePars[n].p3d[3] * w.z +
            fractPars->flamePars[n].p3d[4];
      q.y = fractPars->flamePars[n].p3d[5] * w.x +
            fractPars->flamePars[n].p3d[6] * w.y +
            fractPars->flamePars[n].p3d[7] * w.z +
            fractPars->flamePars[n].p3d[8];
      q.z = fractPars->flamePars[n].p3d[9] * w.x +
            fractPars->flamePars[n].p3d[10] * w.y +
            fractPars->flamePars[n].p3d[11] * w.z +
            fractPars->flamePars[n].p3d[12];
      q *= fractPars->flamePars[n].p3d[0];

      w.set(0, 0, 0);
      switch(fractPars->flamePars[n].variation)
      {
        case 0: { // linear
          w.add(q.x, q.y, q.z);
        } break;

        case 1: { // sinusoidal
          w.add(sin(q.x), sin(q.y), sin(q.z));
        } break;

        case 2: { // spherical
          double rr = q.x*q.x + q.y*q.y;
          w.add(q.x/rr, q.y/rr, q.z/rr);
        } break;

        case 3: { // swirl
          double rr = q.x*q.x + q.y*q.y;
          w.add(q.x*sin(rr) - q.y*cos(rr), q.x*cos(rr) + q.y*sin(rr), 0);
        } break;

        case 4: { // horseshoe
          double r = q.length();
          w.add(((q.x-q.y)*(q.x+q.y)) / r, 2.0*q.x*q.y / r, 0);
        } break;

        case 5: { // polar
          double r = q.length();
          double p = atan2(q.y, q.x);
          w.add(p/PI, r-1.0, 0);
        } break;

        case 6: { // handkerchief
          double r = q.length();
          double p = atan2(q.y, q.x);
          w.add(r*sin(p+r), r*cos(p-r), 0);
        } break;

        case 7: { // heart
          double r = q.length();
          double p = atan2(q.y, q.x);
          w.add(r*sin(p*r), -r*cos(p*r), 0);
        } break;

        case 8: { // disc
          double r = q.length();
          double p = atan2(q.y, q.x);
          w.add(p/PI*sin(PI*r), p/PI*cos(p*r), 0);
        } break;

        case 9: { // spiral
          double r = q.length();
          double p = atan2(q.y, q.x);
          w.add((cos(p) + sin(r))/r, (sin(p) - cos(r))/r, 0);
        } break;

        case 10: { // hyperbolic
          double r = q.length();
          double p = atan2(q.y, q.x);
          w.add(sin(p)/r, cos(p)*r, 0);
        } break;

        case 11: { // diamond
          double r = q.length();
          double p = atan2(q.y, q.x);
          w.add(sin(p)*cos(r), cos(p)*sin(r), 0);
        }break;

        case 12: { // ex
          double r = q.length();
          double p = atan2(q.y, q.x);
          double p0 = sin(p+r);
          double p1 = cos(p-r);
          w.add(p0*p0*p0 + p1*p1*p1, p0*p0*p0 - p1*p1*p1, 0);
        }break;

        case 13: { // julia
          double sr = sqrt(q.length());
          double p = atan2(q.y, q.x);
          double k = Random::get(-1, 1) * PI * 0.5;
          w.add(sr*cos(p*0.5 + k), sr*sin(p*0.5 + k), 0);
        } break;

        case 14: { // bent
          if(q.x >= q.y && q.y >= 0) {
            w.add(q.x, q.y, 0);
          }
          else if(q.x < q.y && q.y >= 0) {
            w.add(2.0*q.x, q.y, 0);
          }
          else if(q.x >= q.y && q.y < 0) {
            w.add(q.x, 0.5*q.y, 0);
          }
          else {
            w.add(2.0*q.x, 0.5*q.y, 0);
          }
        } break;

        case 15: { // waves
          double b = fractPars->flamePars[n].p3d[13];
          double c = fractPars->flamePars[n].p3d[14];
          double f = fractPars->flamePars[n].p3d[15];
          double e = fractPars->flamePars[n].p3d[16];
          w.add(q.x + b*sin(q.y/(c*c)), q.y + e*sin(q.x/(f*f)), 0);
        } break;

        case 16: { // fisheye
          double r = q.length();
          w.add(2*q.y/(r+1), 2*q.x/(r+1), 0);
        } break;

        case 17: { // popcorn
          double c = fractPars->flamePars[n].p3d[13];
          double f = fractPars->flamePars[n].p3d[14];
          w.add(q.x + c*sin(tan(3*q.y)), q.y + f*sin(tan(3*q.x)), 0);
        } break;

        case 18: { // exponential
          double r = q.length();
          double t = exp(q.x-1);
          w.add(t * cos(q.y*PI), t * sin(q.y*PI), 0);
        } break;

        case 19: { // power
          double r = q.length();
          double p = atan2(q.y, q.x);
          double t = pow(r, sin(p));
          w.add(t * cos(p), t * sin(p), 0);
        } break;

        case 20: { // cosine
          double p = atan2(q.y, q.x);
          w.add(cos(PI*q.x)*cosh(q.y), -sin(PI*q.x)*sinh(q.y), 0);
        } break;

        case 21: { // rings
          double r = q.length();
          double u = atan2(q.y, q.x);
          double cc = fractPars->flamePars[n].p3d[13]*fractPars->flamePars[n].p3d[13];
          double t = modfff(r+cc, 2*cc) - cc + r*(1-cc);
          w.add(t*cos(u), t*sin(u), 0);
        } break;

        case 22: { // fan
          double r = q.length();
          double u = atan2(q.y, q.x);
          double c = fractPars->flamePars[n].p3d[13];
          double f = fractPars->flamePars[n].p3d[14];
          double t = PI*c*c;
          if(modfff(u+f, t) > t/2) {
            w.add(r*cos(u - t/2), r*sin(u - t/2), 0);
          }
          else {
            w.add(r*cos(u + t/2), r*sin(u + t/2), 0);
          }
        } break;

        case 23: { // blob
          double r = q.length();
          double u = atan2(q.y, q.x);
          double p1 = fractPars->flamePars[n].p3d[13];
          double p2 = fractPars->flamePars[n].p3d[14];
          double p3 = fractPars->flamePars[n].p3d[15];
          double t = r * (p2 + (p1-p2)/2*(sin(p3*u)+1));
          w.add(t*cos(u), t*sin(u), 0);
        } break;

        case 24: { // pdj
          double p1 = fractPars->flamePars[n].p3d[13];
          double p2 = fractPars->flamePars[n].p3d[14];
          double p3 = fractPars->flamePars[n].p3d[15];
          double p4 = fractPars->flamePars[n].p3d[16];
          w.add(sin(p1*q.y) - cos(p2*q.x), sin(p3*q.x) - cos(p4*q.y), 0);
        } break;

        case 25: { // fan2
          double r = q.length();
          double u = atan2(q.y, q.x);
          double p1 = PI*fractPars->flamePars[n].p3d[13]*fractPars->flamePars[n].p3d[13];
          double p2 = fractPars->flamePars[n].p3d[14];
          double t = u + p2 - p1*trunc(2*u*p2/p1);
          if(t > p1/2) {
            w.add(r*(sin(u - p1/2)), r*cos(u - p1/2), 0);
          }
          else {
            w.add(r*(sin(u + p1/2)), r*cos(u + p1/2), 0);
          }
        } break;

        case 26: { // rings2
          double r = q.length();
          double u = atan2(q.y, q.x);
          double p = fractPars->flamePars[n].p3d[13]*fractPars->flamePars[n].p3d[13];
          double t = r - 2*p*trunc((r+p)/(2*p)) + r*(1-p);
          w.add(t*sin(u), t*cos(u), 0);
        } break;

        case 27: { // eyefish
          double r = q.length();
          w.add(2.0*q.x/(r+1), 2.0*q.y/(r+1), 0);
        } break;

        case 28: { // bubble
          double rr = q.x*q.x + q.y*q.y;
          w.add(4.0*q.x/(rr+4), 4.0*q.y/(rr+4), 0);
        } break;

        case 29: { // cylinder
          w.add(sin(q.x), q.y, 0);
        }break;

        case 30: { // perspective
          double p1 = fractPars->flamePars[n].p3d[13];
          double p2 = fractPars->flamePars[n].p3d[14];
          double t = p2/(p2 - q.y*sin(p1));
          w.add(t*q.x, t*q.y*cos(p1), 0);
        } break;

        case 31: { // noise
          double t0 = Random::getDouble(0, 1);
          double t1 = Random::getDouble(0, 1);
          w.add(t0*q.x*cos(TAU*t1), t0*q.y*sin(TAU*t1), 0);
        } break;

        case 32: { // julian
          double r = q.length();
          double u = atan2(q.y, q.x);
          double p1 = fractPars->flamePars[n].p3d[13];
          double p2 = fractPars->flamePars[n].p3d[14];
          //double h1 = Random::getDouble(0, 1);
          //double t1 = trunc(abs(p1)*h1);
          double t1 = Random::getInt(0, floor(abs(p1)));
          double t2 = (u + TAU*t1) / p1;
          double t3 = pow(r, p2/p1);
          w.add(t3*cos(t2), t3*sin(t2), 0);
        } break;

        case 33: { // juliascope
          double r = q.length();
          double u = atan2(q.y, q.x);
          double p1 = fractPars->flamePars[n].p3d[13];
          double p2 = fractPars->flamePars[n].p3d[14];
          //double h1 = Random::getDouble(0, 1);
          double h2 = Random::get(-1, 1);
          //double t1 = trunc(abs(p1)*h1);
          double t1 = Random::getInt(0, floor(abs(p1)));
          double t2 = (h2*u + TAU*t1) / p1;
          double t3 = pow(r, p2/p1);
          w.add(t3*cos(t2), t3*sin(t2), 0);
        } break;

        case 34: { // blur
          double t0 = Random::getDouble(0, 1);
          double t1 = Random::getDouble(0, 1);
          w.add(t0*cos(TAU*t1), t0*sin(TAU*t1), 0);
        } break;

        case 35: { // gaussian
          double t0 = Random::getDouble(0, 1);
          double t1 = Random::getDouble(0, 1);
          double t2 = Random::getDouble(0, 1);
          double t3 = Random::getDouble(0, 1);
          double s = t0 + t1 + t2 + t3 - 2;
          double t4 = Random::getDouble(0, 1);
          w.add(s*cos(TAU*t4), s*sin(TAU*t4), 0);
        } break;

        case 36: { // radialblur
          double r = q.length();
          double u = atan2(q.y, q.x);
          double p1 = fractPars->flamePars[n].p3d[13] * PI * 0.5;
          double h1 = Random::getDouble(0, 1);
          double h2 = Random::getDouble(0, 1);
          double h3 = Random::getDouble(0, 1);
          double h4 = Random::getDouble(0, 1);
          double t1 = h1 + h2 + h3 + h4 - 2;
          double t2 = u + t1 * sin(p1);
          double t3 = t1 * cos(p1 - 1);
          double v36 = fractPars->flamePars[n].p3d[0];
          w.add(v36*r*cos(t2) + t3*q.x, r*sin(t2) + t3*q.y, 0);
        } break;

        case 37: { // pie
          double p1 = fractPars->flamePars[n].p3d[13];
          double p2 = fractPars->flamePars[n].p3d[14];
          double p3 = fractPars->flamePars[n].p3d[15];
          double h1 = Random::getDouble(0, 1);
          double h2 = Random::getDouble(0, 1);
          double h3 = Random::getDouble(0, 1);
          double t1 = trunc(h1*p1 + 0.5);
          double t2 = p2 + TAU/p1 * (t1 + h2*p3);
          w.add(h3*cos(t2), sin(t2), 0);
        } break;

        case 38: { // ngon
          double r = q.length();
          double u = atan2(q.y, q.x);
          double p1 = fractPars->flamePars[n].p3d[13];
          double p2 = TAU / fractPars->flamePars[n].p3d[14];
          double p3 = fractPars->flamePars[n].p3d[15];
          double p4 = fractPars->flamePars[n].p3d[16];
          double t1 = u - p2*floor(u/p2);
          double t2 = t1 > 0.5*p2 ? t1 : t1 - p2;
          double k = (p3 * (1/cos(t2)-1) + 4) / (pow(r, p1));
          w.add(k*q.x, k*q.y, 0);
        } break;

        case 39: { // curl
          double p1 = fractPars->flamePars[n].p3d[13];
          double p2 = fractPars->flamePars[n].p3d[14];
          double t1 = 1 + p1*q.x + p2*(q.x*q.x - q.y*q.y);
          double t2 = p1*q.y + 2*p2*q.x*q.y;
          double t3 = 1.0/(t1*t1 + t2*t2);
          w.add((q.x*t1 + q.y*t2) * t3, (q.y*t1 - q.x*t2) * t3, 0);
        } break;

        case 40: { // rectangles
          double p1 = fractPars->flamePars[n].p3d[13];
          double p2 = fractPars->flamePars[n].p3d[14];
          w.add((2*floor(q.x/p1)+1)*p1 - q.x, (2*floor(q.y/p2)+1)*p2 - q.y, 0);
        } break;

        case 41: { // arch (what's the point...)
          double t0 = Random::getDouble(0, 1);
          //double t1 = Random::getDouble(0, 1);
          //double t2 = Random::getDouble(0, 1);
          double v41 = fractPars->flamePars[n].p3d[0];
          double s = sin(t0*v41);
          w.add(sin(t0 * PI * v41), s*s / cos(t0 * PI * v41), 0);
        } break;

        case 42: { // tangent (what's the point...)
          w.add(sin(q.x)/cos(q.y), tan(q.y), 0);
        } break;

        case 43: { // square (what's the point...)
          double t0 = Random::getDouble(0, 1);
          double t1 = Random::getDouble(0, 1);
          w.add(t0-0.5, t1-0.5, 0);
        } break;

        case 44: { // rays (what's the point...)
          double rr = q.x*q.x + q.y*q.y;
          double v44 = fractPars->flamePars[n].p3d[0];
          double t = Random::getDouble(0, 1);
          double s = v44 * tan(t*PI*v44) / rr;
          w.add(s*cos(q.x), s*sin(q.y), 0);
        } break;

        case 45: { // blade (what's the point...)
          double r = q.length();
          double v45 = fractPars->flamePars[n].p3d[0];
          double t0 = Random::getDouble(0, 1);
          //double t1 = Random::getDouble(0, 1);
          //double t2 = Random::getDouble(0, 1);
          //double t3 = Random::getDouble(0, 1);
          w.add(q.x * (cos(t0 * r * v45) + sin(t0 * r * v45)),
                q.y * (cos(t0 * r * v45) - sin(t0 * r * v45)), 0);
        } break;

        case 46: { // secant
          double r = q.length();
          double v46 = fractPars->flamePars[n].p3d[0];
          w.add(q.x, 1.0 / (v46*cos(v46*r)), 0);
        } break;

        case 47: { // twintrian (what's the point...)
          double r = q.length();
          double v47 = fractPars->flamePars[n].p3d[0];
          double h0 = Random::getDouble(0, 1);
          //double h1 = Random::getDouble(0, 1);
          //double h2 = Random::getDouble(0, 1);
          double s = sin(h0*r*v47);
          double t = log10(s*s) + cos(h0*r*v47);
          w.add(q.x*t, q.x*(t - PI*sin(h0*r*v47)), 0);
        } break;

        case 48: { // cross (what's the point...)
          double t = sqrt(1.0 / (q.x*q.x - q.y*q.y));
          w.add(t*q.x, t*q.y, 0);
        } break;

        case 49: { //
          double r = q.length();
          if(Random::get(false, true)) r = sqrt(r);
          w.add(q.x/r, q.y/r, 0);
        } break;

        case 50: { //
          double p = Random::get(-PI, PI) * 0.5;
          w.add(sin(q.x + p), sin(q.y + p), 0);
        } break;

        case 51: { //
          double rr = q.x*q.x + q.y*q.y;
          w.add(q.x/rr, q.y/rr, 0);
        } break;

        case 52: { //
          double r = q.length();
          w.add(q.x*sin(r) - q.y*cos(r), q.x*cos(r) + q.y*sin(r), 0);
        } break;

        case 53: { //
          double r = sqrt(q.length());
          w.add(((q.x-q.y)*(q.x+q.y)) / r, 2.0*q.x*q.y / r, 0);
        } break;

        case 54: { //
          double r = q.length();
          double p = atan2(q.y, q.x);
          w.add(p/PI, q.x/r, 0);
        } break;

        case 55: { //
          double r = q.length();
          double p = atan2(q.y, q.x);
          w.add(r*sin(p+r), r*cos(p-r), 0);
        } break;

        case 56: { //
          double r = sqrt(q.length());
          double p = atan2(q.y, q.x);
          w.add(r*sin(p+r), r*cos(p-r), 0);
        } break;

        case 57: { // seems to be usable
          double r = q.length();
          if(Random::get(true, false)) r = sqrt(r);
          double p = atan2(q.y, q.x);
          w.add(r*sin(p+r), r*cos(p-r), 0);
        } break;

        case 58: { // quite good
          double r = q.length();
          double p = atan2(q.y, q.x);
          p += Random::get(-PI, PI) * 0.5;
          w.add(r*sin(p+r), r*cos(p-r), 0);
        } break;

        case 59: { // quite good
          double r = sqrt(q.length());
          double p = atan2(q.y, q.x);
          p += Random::get(-PI, PI) * 0.5;
          w.add(r*sin(p+r), r*cos(p-r), 0);
        } break;

        case 60: { // quite good
          double r = q.length();
          if(Random::get(true, false)) r = sqrt(r);
          double p = atan2(q.y, q.x);
          p += Random::get(-PI, PI) * 0.5;
          w.add(r*sin(p+r), r*cos(p-r), 0);
        } break;

        case 61: { // usable, sparse, quite rich
          double r = q.length();
          double p = atan2(q.y, q.x);
          w.add(r*sin(p+r+Random::get(-PI, PI) * 0.5), r*cos(p-r+Random::get(-PI, PI) * 0.5), 0);
        } break;

        case 62: { // usable, dense, not very rich
          double r = sqrt(q.length());
          double p = atan2(q.y, q.x);
          w.add(r*sin(p+r+Random::get(-PI, PI) * 0.5), r*cos(p-r+Random::get(-PI, PI) * 0.5), 0);
        } break;

        case 63: { //
          double sr = sqrt(q.length());
          double p = atan2(q.y, q.x);
          double k1 = Random::get(-1, 1) * PI * 0.5;
          double k2 = Random::get(-1, 1) * PI * 0.5;
          w.add(sr*cos(p*0.5 + k1), sr*sin(p*0.5 + k2), 0);
        } break;

        case 64: { //
          double sr = sqrt(q.length());
          double p = atan2(q.y, q.x);
          double k1 = Random::get(-1, 1) * PI * 0.5;
          double k2 = Random::get(-1, 1) * PI * 0.5;
          w.add(sr*cos(p*0.5 + k1)*q.x, sr*sin(p*0.5 + k2)*q.y, 0);
        } break;

        case 65: { //
          double sr = sqrt(q.length());
          double p = atan2(q.y, q.x);
          double t = max(floor(abs(fractPars->flamePars[n].p3d[13])), 1.0);
          int s1 = 0, s2 = 0;
          while((s1 = Random::getInt(-t, t)) == 0) {}
          //while((s2 = Random::getInt(-t, t)) == 0) {}
          double k1 = s1 * PI / (1.0+t);
          double k2 = s1 * PI / (1.0+t);
          w.add(sr*cos(p*0.5 + k1), sr*sin(p*0.5 + k2), 0);
        } break;

        case 66: { //
          double sr = sqrt(q.length());
          double p = atan2(q.y, q.x);
          double k1 = Random::get(-1, 1) * PI * 0.5;
          double k2 = Random::get(-1, 1) * PI * 0.5;
          w.add(sr*cos(p*0.5 + k1)*q.x, sr*sin(p*0.5 + k2)*q.y, 0);
        } break;

      }
    }

    i++;

    if(i >= fractPars->minIter && !fractPars->updateIterationRoute) {
      int ind = fractPars->getVoxelIndex(w);
      if(ind >= 0) {
        voxels[ind]++;
      }
    }
    /*if(fractPars->updateIterationRoute) {
      if(fractPars->numIterationRoutePoints+1 < fractPars->maxIterationRoutePoints) {
        fractPars->iterationRoute[fractPars->numIterationRoutePoints].set(w);
        fractPars->numIterationRoutePoints++;
      }
    }*/
  }

  fractPars->updateIterationRoute = false;

  return i;
}
