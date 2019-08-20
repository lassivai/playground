//varying vec3 normal;
//varying vec3 toLight;
//varying vec2 texCoord;

//vec3 normal1 = normalize(normal);
//vec3 toLight1 = normalize(toLight);
//float diff = clamp(dot(normal1, toLight1), 0.0, 1.0);
//gl_FragColor = texture2d(texture, texCoord);

#version 430

const float PI = 3.14159265359;
const float TAU = 2.0*3.14159265359;

uniform vec2 screenSize;
//uniform vec2 mousePos;
uniform float time;

uniform vec2 location;
uniform float scale;
uniform float maxIter;

uniform float minIter;

uniform float dt;

uniform int variations[10];
uniform float parameters[10*14];
uniform int numVariations;

layout (binding = 0, r32ui) uniform uimage2D hitCounterImage;

in vec2 texCoord;

float map(float t, float a, float b, float x, float y) {
  if(b == a) return 0;
  return (t-a)/(b-a)*(y-x)+x;
}

#define XXX (vec2(1.0, 1.0)+st.xy)*gl_FragCoord.xy*(sin(time*43.5443)+vec2(25,55))*33.345+vec2(43.355, 55.243)
//layout(location=0) out vec4 colorOut;

float randomMinusOneToOne(in vec2 st) {
  return fract(sin(dot(XXX, vec2(127.9898, 78.233))) * 43.546477) * 2.0 - 1.0;
}
float randomZeroToOne(in vec2 st) {
  return fract(sin(dot(XXX, vec2(132.9898, 78.233))) * 438.5453126463);
}
float randomZeroOrOne(in vec2 st) {
  return floor(fract(sin(dot(XXX, vec2(127.9898, 78.233))) * 43758.5453123)*2.0);
}
float randomMinusOneOrOne(in vec2 st) {
  return floor(fract(sin(dot(XXX, vec2(152.9898, 3.233))) * 43.5453123) * 2.0)*2.0 - 1.0;
}
float randomInteger(in vec2 st, float minVal, float maxVal) {
  return floor(fract(sin(dot(XXX, vec2(27.9898, 78.233))) * 4.5453123) * (1.0+round(maxVal)-round(minVal))) - round(minVal);
}
float random(in vec2 st, in float a, in float b) {
  return fract(sin(dot(XXX, vec2(182.9898,78.233))) * 43.5453123)*(b-a)+a;
}

void main()
{
  ivec2 isize = imageSize(hitCounterImage);
  float aspectRatio = float(isize.x)/isize.y;

  vec2 w, q;

  w = gl_FragCoord.xy*0.001;
  //w.x = random(vec2(5.46, 66.57)+0.0054654765*gl_FragCoord.yx*sin(float(54+time*545.44365)*.4365)*2.546, -1.0, 1.0);
  //w.y = random(vec2(7.86846, 6.58797)*sin(time*54.456)+(gl_FragCoord.yx+vec2(656.476854, -58)), -1.0, 1.0);

  for(int i=0; i<maxIter; i++) {
    for(int n=0; n<10; n++) {
      //w = vec2(randomMinusOneToOne(w.xy), randomMinusOneToOne(w.yx));

      if(variations[n] < 0) continue;

      q.x = parameters[n*14 + 1] * w.x +
            parameters[n*14 + 2] * w.y +
            parameters[n*14 + 3];
      q.y = parameters[n*14 + 4] * w.x +
            parameters[n*14 + 5] * w.y +
            parameters[n*14 + 6];
      q *= parameters[n*14 + 0];

      w = vec2(0, 0);
        //w += vec2(sin(q.x), sin(q.y));

      switch(variations[n])
      {
        case 0: { // linear
          w += q;
        } break;

        case 1: { // sinusoidal
          w += vec2(sin(q.x), sin(q.y));
        } break;

        case 2: { // spherical
          float rr = q.x*q.x + q.y*q.y;
          w += vec2(q.x/rr, q.y/rr);
        } break;

        case 3: { // swirl
          float rr = q.x*q.x + q.y*q.y;
          w += vec2(q.x*sin(rr) - q.y*cos(rr), q.x*cos(rr) + q.y*sin(rr));
        } break;

        case 4: { // horseshoe
          float r = length(q);
          w + vec2(((q.x-q.y)*(q.x+q.y)) / r, 2.0*q.x*q.y / r);
        } break;

        case 5: { // polar
          float r = length(q);
          float p = atan(q.y, q.x);
          w += vec2(p/PI, r-1.0);
        } break;

        case 6: { // handkerchief
          float r = length(q);
          float p = atan(q.y, q.x);
          w += vec2(r*sin(p+r), r*cos(p-r));
        } break;

        case 7: { // heart
          float r = length(q);
          float p = atan(q.y, q.x);
          w += vec2(r*sin(p*r), -r*cos(p*r));
        } break;

        case 8: { // disc
          float r = length(q);
          float p = atan(q.y, q.x);
          w += vec2(p/PI*sin(PI*r), p/PI*cos(p*r));
        } break;

        case 9: { // spiral
          float r = length(q);
          float p = atan(q.y, q.x);
          w += vec2((cos(p) + sin(r))/r, (sin(p) - cos(r))/r);
        } break;

        case 10: { // hyperbolic
          float r = length(q);
          float p = atan(q.y, q.x);
          w += vec2(sin(p)/r, cos(p)*r);
        } break;

        case 11: { // diamond
          float r = length(q);
          float p = atan(q.y, q.x);
          w += vec2(sin(p)*cos(r), cos(p)*sin(r));
        }break;

        case 12: { // ex
          float r = length(q);
          float p = atan(q.y, q.x);
          float p0 = sin(p+r);
          float p1 = cos(p-r);
          w += vec2(p0*p0*p0 + p1*p1*p1, p0*p0*p0 - p1*p1*p1);
        }break;

        case 13: { // julia
          float sr = sqrt(length(q));
          float p = atan(q.y, q.x);
          float k = randomMinusOneOrOne(q) * PI * 0.5;
          w += vec2(sr*cos(p*0.5 + k), sr*sin(p*0.5 + k));
        } break;

        case 14: { // bent
          if(q.x >= q.y && q.y >= 0) {
            w += vec2(q.x, q.y);
          }
          else if(q.x < q.y && q.y >= 0) {
            w += vec2(2.0*q.x, q.y);
          }
          else if(q.x >= q.y && q.y < 0) {
            w += vec2(q.x, 0.5*q.y);
          }
          else {
            w += vec2(2.0*q.x, 0.5*q.y);
          }
        } break;

        case 15: { // waves
          float b = parameters[n*14 + 7];
          float c = parameters[n*14 + 8];
          float f = parameters[n*14 + 9];
          float e = parameters[n*14 + 10];
          w += vec2(q.x + b*sin(q.y/(c*c)), q.y + e*sin(q.x/(f*f)));
        } break;

        case 16: { // fisheye
          float r = length(q);
          w += vec2(2*q.y/(r+1), 2*q.x/(r+1));
        } break;

        case 17: { // popcorn
          float c = parameters[n*14 + 7];
          float f = parameters[n*14 + 8];
          w += vec2(q.x + c*sin(tan(3*q.y)), q.y + f*sin(tan(3*q.x)));
        } break;

        case 18: { // exponential
          float r = length(q);
          float t = exp(q.x-1);
          w += vec2(t * cos(q.y*PI), t * sin(q.y*PI));
        } break;

        case 19: { // power
          float r = length(q);
          float p = atan(q.y, q.x);
          float t = pow(r, sin(p));
          w += vec2(t * cos(p), t * sin(p));
        } break;

        case 20: { // cosine
          float p = atan(q.y, q.x);
          w += vec2(cos(PI*q.x)*cosh(q.y), -sin(PI*q.x)*sinh(q.y));
        } break;

        case 21: { // rings
          float r = length(q);
          float u = atan(q.y, q.x);
          float cc = parameters[n*14 + 7]*parameters[n*14 + 7];
          float t = mod(r+cc, 2*cc) - cc + r*(1-cc);
          w += vec2(t*cos(u), t*sin(u));
        } break;

        case 22: { // fan
          float r = length(q);
          float u = atan(q.y, q.x);
          float c = parameters[n*14 + 7];
          float f = parameters[n*14 + 8];
          float t = PI*c*c;
          if(mod(u+f, t) > t/2) {
            w += vec2(r*cos(u - t/2), r*sin(u - t/2));
          }
          else {
            w += vec2(r*cos(u + t/2), r*sin(u + t/2));
          }
        } break;

        case 23: { // blob
          float r = length(q);
          float u = atan(q.y, q.x);
          float p1 = parameters[n*14 + 7];
          float p2 = parameters[n*14 + 8];
          float p3 = parameters[n*14 + 9];
          float t = r * (p2 + (p1-p2)/2*(sin(p3*u)+1));
          w += vec2(t*cos(u), t*sin(u));
        } break;

        case 24: { // pdj
          float p1 = parameters[n*14 + 7];
          float p2 = parameters[n*14 + 8];
          float p3 = parameters[n*14 + 9];
          float p4 = parameters[n*14 + 10];
          w += vec2(sin(p1*q.y) - cos(p2*q.x), sin(p3*q.x) - cos(p4*q.y));
        } break;

        case 25: { // fan2
          float r = length(q);
          float u = atan(q.y, q.x);
          float p1 = PI*parameters[n*14 + 7]*parameters[n*14 + 7];
          float p2 = parameters[n*14 + 8];
          float t = u + p2 - p1*trunc(2*u*p2/p1);
          if(t > p1/2) {
            w += vec2(r*(sin(u - p1/2)), r*cos(u - p1/2));
          }
          else {
            w += vec2(r*(sin(u + p1/2)), r*cos(u + p1/2));
          }
        } break;

        case 26: { // rings2
          float r = length(q);
          float u = atan(q.y, q.x);
          float p = parameters[n*14 + 7]*parameters[n*14 + 7];
          float t = r - 2*p*trunc((r+p)/(2*p)) + r*(1-p);
          w += vec2(t*sin(u), t*cos(u));
        } break;

        case 27: { // eyefish
          float r = length(q);
          w += vec2(2.0*q.x/(r+1), 2.0*q.y/(r+1));
        } break;

        case 28: { // bubble
          float rr = q.x*q.x + q.y*q.y;
          w += vec2(4.0*q.x/(rr+4), 4.0*q.y/(rr+4));
        } break;

        case 29: { // cylinder
          w += vec2(sin(q.x), q.y);
        }break;

        case 30: { // perspective
          float p1 = parameters[n*14 + 7];
          float p2 = parameters[n*14 + 8];
          float t = p2/(p2 - q.y*sin(p1));
          w += vec2(t*q.x, t*q.y*cos(p1));
        } break;

        case 31: { // noise
          float t0 = randomZeroToOne(q);
          float t1 = randomZeroToOne(q);
          w += vec2(t0*q.x*cos(TAU*t1), t0*q.y*sin(TAU*t1));
        } break;

        case 32: { // julian
          float r = length(q);
          float u = atan(q.y, q.x);
          float p1 = parameters[n*14 + 7];
          float p2 = parameters[n*14 + 8];
          //float h1 = randomZeroToOne(q);
          //float t1 = trunc(abs(p1)*h1);
          float t1 = randomInteger(q, 0, floor(abs(p1)));
          float t2 = (u + TAU*t1) / p1;
          float t3 = pow(r, p2/p1);
          w += vec2(t3*cos(t2), t3*sin(t2));
        } break;

        case 33: { // juliascope
          float r = length(q);
          float u = atan(q.y, q.x);
          float p1 = parameters[n*14 + 7];
          float p2 = parameters[n*14 + 8];
          //float h1 = randomZeroToOne(q);
          float h2 = randomMinusOneOrOne(q);
          //float t1 = trunc(abs(p1)*h1);
          float t1 = randomInteger(q, 0, floor(abs(p1)));
          float t2 = (h2*u + TAU*t1) / p1;
          float t3 = pow(r, p2/p1);
          w += vec2(t3*cos(t2), t3*sin(t2));
        } break;

        case 34: { // blur
          float t0 = randomZeroToOne(q);
          float t1 = randomZeroToOne(q);
          w += vec2(t0*cos(TAU*t1), t0*sin(TAU*t1));
        } break;

        case 35: { // gaussian
          float t0 = randomZeroToOne(q);
          float t1 = randomZeroToOne(q);
          float t2 = randomZeroToOne(q);
          float t3 = randomZeroToOne(q);
          float s = t0 + t1 + t2 + t3 - 2;
          float t4 = randomZeroToOne(q);
          w += vec2(s*cos(TAU*t4), s*sin(TAU*t4));
        } break;

        case 36: { // radialblur
          float r = length(q);
          float u = atan(q.y, q.x);
          float p1 = parameters[n*14 + 7] * PI * 0.5;
          float h1 = randomZeroToOne(q);
          float h2 = randomZeroToOne(q);
          float h3 = randomZeroToOne(q);
          float h4 = randomZeroToOne(q);
          float t1 = h1 + h2 + h3 + h4 - 2;
          float t2 = u + t1 * sin(p1);
          float t3 = t1 * cos(p1 - 1);
          float v36 = parameters[n*14 + 0];
          w += vec2(v36*r*cos(t2) + t3*q.x, r*sin(t2) + t3*q.y);
        } break;

        case 37: { // pie
          float p1 = parameters[n*14 + 7];
          float p2 = parameters[n*14 + 8];
          float p3 = parameters[n*14 + 9];
          float h1 = randomZeroToOne(q);
          float h2 = randomZeroToOne(q);
          float h3 = randomZeroToOne(q);
          float t1 = trunc(h1*p1 + 0.5);
          float t2 = p2 + TAU/p1 * (t1 + h2*p3);
          w += vec2(h3*cos(t2), sin(t2));
        } break;

        case 38: { // ngon
          float r = length(q);
          float u = atan(q.y, q.x);
          float p1 = parameters[n*14 + 7];
          float p2 = TAU / parameters[n*14 + 8];
          float p3 = parameters[n*14 + 9];
          float p4 = parameters[n*14 + 10];
          float t1 = u - p2*floor(u/p2);
          float t2 = t1 > 0.5*p2 ? t1 : t1 - p2;
          float k = (p3 * (1/cos(t2)-1) + 4) / (pow(r, p1));
          w += vec2(k*q.x, k*q.y);
        } break;

        case 39: { // curl
          float p1 = parameters[n*14 + 7];
          float p2 = parameters[n*14 + 8];
          float t1 = 1 + p1*q.x + p2*(q.x*q.x - q.y*q.y);
          float t2 = p1*q.y + 2*p2*q.x*q.y;
          float t3 = 1.0/(t1*t1 + t2*t2);
          w += vec2((q.x*t1 + q.y*t2) * t3, (q.y*t1 - q.x*t2) * t3);
        } break;

        case 40: { // rectangles
          float p1 = parameters[n*14 + 7];
          float p2 = parameters[n*14 + 8];
          w += vec2((2*floor(q.x/p1)+1)*p1 - q.x, (2*floor(q.y/p2)+1)*p2 - q.y);
        } break;

        case 41: { // arch (what's the point...)
          float t0 = randomZeroToOne(q);
          //float t1 = randomZeroToOne(q);
          //float t2 = randomZeroToOne(q);
          float v41 = parameters[n*14 + 0];
          float s = sin(t0*v41);
          w += vec2(sin(t0 * PI * v41), s*s / cos(t0 * PI * v41));
        } break;

        case 42: { // tangent (what's the point...)
          w += vec2(sin(q.x)/cos(q.y), tan(q.y));
        } break;

        case 43: { // square (what's the point...)
          float t0 = randomZeroToOne(q);
          float t1 = randomZeroToOne(q);
          w += vec2(t0-0.5, t1-0.5);
        } break;

        case 44: { // rays (what's the point...)
          float rr = q.x*q.x + q.y*q.y;
          float v44 = parameters[n*14 + 0];
          float t = randomZeroToOne(q);
          float s = v44 * tan(t*PI*v44) / rr;
          w += vec2(s*cos(q.x), s*sin(q.y));
        } break;

        case 45: { // blade (what's the point...)
          float r = length(q);
          float v45 = parameters[n*14 + 0];
          float t0 = randomZeroToOne(q);
          //float t1 = randomZeroToOne(q);
          //float t2 = randomZeroToOne(q);
          //float t3 = randomZeroToOne(q);
          w += vec2(q.x * (cos(t0 * r * v45) + sin(t0 * r * v45)),
                q.x * (cos(t0 * r * v45) - sin(t0 * r * v45)));
        } break;

        case 46: { // secant
          float r = length(q);
          float v46 = parameters[n*14 + 0];
          w += vec2(q.x, 1.0 / (v46*cos(v46*r)));
        } break;

        case 47: { // twintrian (what's the point...)
          float r = length(q);
          float v47 = parameters[n*14 + 0];
          float h0 = randomZeroToOne(q);
          //float h1 = randomZeroToOne(q);
          //float h2 = randomZeroToOne(q);
          float s = sin(h0*r*v47);
          float t = log(s*s) / log(10.0) + cos(h0*r*v47);
          w += vec2(q.x*t, q.x*(t - PI*sin(h0*r*v47)));
        } break;

        case 48: { // cross (what's the point...)
          float t = sqrt(1.0 / (q.x*q.x - q.y*q.y));
          w += vec2(t*q.x, t*q.y);
        } break;

        case 49: { //
          float t = randomZeroOrOne(q);
          float r = length(q);
          r = t * sqrt(r) + (1.0-t) * r;
          w += vec2(q.x/r, q.y/r);
        } break;

        case 50: { //
          float p = randomMinusOneOrOne(q) * PI * 0.5;
          w += vec2(sin(q.x + p), sin(q.y + p));
        } break;

        case 51: { //
          float rr = q.x*q.x + q.y*q.y;
          w += vec2(q.x/rr, q.y/rr);
        } break;

        case 52: { //
          float r = length(q);
          w += vec2(q.x*sin(r) - q.y*cos(r), q.x*cos(r) + q.y*sin(r));
        } break;

        case 53: { //
          float r = sqrt(length(q));
          w += vec2(((q.x-q.y)*(q.x+q.y)) / r, 2.0*q.x*q.y / r);
        } break;

        case 54: { //
          float r = length(q);
          float p = atan(q.y, q.x);
          w += vec2(p/PI, q.x/r);
        } break;

        case 55: { //
          float r = length(q);
          float p = atan(q.y, q.x);
          w += vec2(r*sin(p+r), r*cos(p-r));
        } break;

        case 56: { //
          float r = sqrt(length(q));
          float p = atan(q.y, q.x);
          w += vec2(r*sin(p+r), r*cos(p-r));
        } break;

        case 57: { // seems to be usable
          float t = randomZeroOrOne(q);
          float r = length(q);
          r = t * sqrt(r) + (1.0-t) * r;
          float p = atan(q.y, q.x);
          w += vec2(r*sin(p+r), r*cos(p-r));
        } break;

        case 58: { // quite good
          float r = length(q);
          float p = atan(q.y, q.x);
          p += randomMinusOneOrOne(q) * PI * 0.5;
          w += vec2(r*sin(p+r), r*cos(p-r));
        } break;

        case 59: { // quite good
          float r = sqrt(length(q));
          float p = atan(q.y, q.x);
          p += randomMinusOneOrOne(q) * PI * 0.5;
          w += vec2(r*sin(p+r), r*cos(p-r));
        } break;

        case 60: { // quite good
          float t = randomZeroOrOne(q);
          float r = length(q);
          r = t * sqrt(r) + (1.0-t) * r;
          float p = atan(q.y, q.x);
          p += randomMinusOneOrOne(q) * PI * 0.5;
          w += vec2(r*sin(p+r), r*cos(p-r));
        } break;

        case 61: { // usable, sparse, quite rich
          float r = length(q);
          float p = atan(q.y, q.x);
          w += vec2(r*sin(p+r+randomMinusOneOrOne(q) * PI * 0.5), r*cos(p-r+randomMinusOneOrOne(q) * PI * 0.5));
        } break;

        case 62: { // usable, dense, not very rich
          float r = sqrt(length(q));
          float p = atan(q.y, q.x);
          w += vec2(r*sin(p+r+randomMinusOneOrOne(q) * PI * 0.5), r*cos(p-r+randomMinusOneOrOne(q) * PI * 0.5));
        } break;

        case 63: { //
          float sr = sqrt(length(q));
          float p = atan(q.y, q.x);
          float k1 = randomMinusOneOrOne(q) * PI * 0.5;
          float k2 = randomMinusOneOrOne(q) * PI * 0.5;
          w += vec2(sr*cos(p*0.5 + k1), sr*sin(p*0.5 + k2));
        } break;

        case 64: { //
          float sr = sqrt(length(q));
          float p = atan(q.y, q.x);
          float k1 = randomMinusOneOrOne(q) * PI * 0.5;
          float k2 = randomMinusOneOrOne(q) * PI * 0.5;
          w += vec2(sr*cos(p*0.5 + k1)*q.x, sr*sin(p*0.5 + k2)*q.y);
        } break;

        case 65: { //
          float sr = sqrt(length(q));
          float p = atan(q.y, q.x);
          float t = max(floor(abs(parameters[n*14 + 7])), 1.0);
          float s1 = 0, s2 = 0;
          while((s1 = randomInteger(q, -t, t)) < 0.00001) {}
          //while((s2 = Random::getInt(-t, t)) == 0) {}
          float k1 = s1 * PI / (1.0+t);
          float k2 = s1 * PI / (1.0+t);
          w += vec2(sr*cos(p*0.5 + k1), sr*sin(p*0.5 + k2));
        } break;

        case 66: { //
          float sr = sqrt(length(q));
          float p = atan(q.y, q.x);
          float k1 = randomMinusOneOrOne(q) * PI * 0.5;
          float k2 = randomMinusOneOrOne(q) * PI * 0.5;
          w += vec2(sr*cos(p*0.5 + k1)*q.x, sr*sin(p*0.5 + k2)*q.y);
        } break;

      }
    }
    /*if(i >= minIter) {
      int px = int(random(gl_FragCoord.xy*time*325+vec2(-3455+i*45,463), 0, isize.x-1));
      if(px >= 0 && px < isize.x) {
        int py = int(random(vec2(124, 645)+gl_FragCoord.xy*time*24.53*i, 0, isize.y-1));
        if(py >= 0 && py < isize.y) {
          imageAtomicAdd(hitCounterImage, ivec2(px, py), 1);
        }
      }
    }*/
    /*int getPixelIndex(Vec2d &v) {
      int px = int(map(v.x-location.x, -aspectRatio*scale, aspectRatio*scale, 0, w-1));
      if(px >= 0 && px < w) {
        int py = int(map(v.y+location.y, -scale, scale, 0, h-1));
        if(py >= 0 && py < h ) {
          return px + py*w;
        }
      }
      return -1;
    }*/

    if(i >= minIter) {
      int px = int(map(w.x-location.x, -aspectRatio*scale, aspectRatio*scale, 0, isize.x-1));
      if(px >= 0 && px < isize.x) {
        int py = int(map(w.y+location.y, -scale, scale, 0, isize.y-1));
        if(py >= 0 && py < isize.y) {
          imageAtomicAdd(hitCounterImage, ivec2(px, py), 1);
        }
      }
    }

  }



}
