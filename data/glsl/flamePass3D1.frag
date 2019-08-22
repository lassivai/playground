//varying vec3 normal;
//varying vec3 toLight;
//varying vec3 texCoord;

//vec3 normal1 = normalize(normal);
//vec3 toLight1 = normalize(toLight);
//float diff = clamp(dot(normal1, toLight1), 0.0, 1.0);
//gl_FragColor = texture2d(texture, texCoord);

#version 430

const float PI = 3.14159265359;
const float TAU = 2.0*3.14159265359;

uniform vec2 screenSize;
//uniform vec3 mousePos;
uniform float time;

uniform vec3 location;
uniform float scale;

uniform float maxIter;
uniform float minIter;

//uniform float dt;

uniform int variations[10];
uniform float parameters3D[10*20];
uniform int numVariations;

uniform ivec3 gridSize;

//uniform int numHitCounterImages;

layout (r32ui) uniform uimage2DArray hitCounterVoxels;

uniform vec2 xRange, yRange, zRange;


in vec2 texCoord;

float map(float t, float a, float b, float x, float y) {
  if(b == a) return 0;
  return (t-a)/(b-a)*(y-x)+x;
}


// FIXME 3D versions

#define XXX (vec2(1.0, 1.0)+st.xy)+(sin(43+time*4.5443)+vec2(25,55))*3.3645645+vec2(43.355, 55.243)
//layout(location=0) out vec4 colorOut;

float randomMinusOneToOne(in vec3 st) {
  return fract(sin(dot(XXX, vec2(127.9898, 78.233))) * 43.546477) * 2.0 - 1.0;
}
float randomZeroToOne(in vec3 st) {
  return fract(sin(dot(XXX, vec2(132.9898, 78.233))) * 48.5453126463);
}
float randomZeroOrOne(in vec3 st) {
  return floor(fract(sin(dot(XXX, vec2(127.9898, 7.233))) * 37.5453123)*2.0);
}
float randomMinusOneOrOne(in vec3 st) {
  return floor(fract(sin(dot(XXX, vec2(52.9898, 3.233))) * 43.5453123) * 2.0)*2.0 - 1.0;
}
float randomInteger(in vec3 st, float minVal, float maxVal) {
  return floor(fract(sin(dot(XXX, vec2(27.9898, 78.233))) * 4.5453123) * (1.0+round(maxVal)-round(minVal))) - round(minVal);
}
float randomIntegerWithoutZero(in vec3 st, float minVal, float maxVal) {
  switch(int(time*10000) % 2) {
    case 0:
      return floor(fract(sin(dot(XXX, vec2(27.9898, 78.233))) * 4.5453123) * (1.0+round(-1)-round(minVal))) - round(minVal);
    case 1:
      return floor(fract(sin(dot(XXX, vec2(27.9898, 78.233))) * 4.5453123) * (1.0+round(maxVal)-round(1))) - round(1);
  }
}
float random(in vec3 st, in float a, in float b) {
  return fract(sin(dot(XXX, vec2(182.9898,78.233))) * 43.5453123)*(b-a)+a;
}

void main()
{
  //ivec3 gridSize = imageSize(hitCounterVoxels).xyz;

  //ivec3 isize = imageSize(hitCounterVoxels).xy;

  vec3 w, q;

  //w = (gl_FragCoord.xy-isize.xy*0.5)*0.001;
  w.x = random(vec3(texCoord.x, texCoord.y, 34.54+sin(time*323.436+434.3)*34.45), -1, 1);
  w.y = random(vec3(texCoord.x, -texCoord.y, -5+sin(time*324.436)*24.45), -1, 1);
  w.z = random(vec3(-texCoord.x, texCoord.y, 44-sin(time*32643.436)*362.45), -1, 1);

  //w.x = random(vec3(5.46, 66.57)+0.0054654765*gl_FragCoord.yx*sin(float(54+time*545.44365)*.4365)*2.546, -1.0, 1.0);
  //w.y = random(vec3(7.86846, 6.58797)*sin(time*54.456)+(gl_FragCoord.yx+vec3(656.476854, -58)), -1.0, 1.0);

  for(int i=0; i<maxIter; i++) {
    for(int n=0; n<10; n++) {
      //w = vec3(randomMinusOneToOne(w.xy), randomMinusOneToOne(w.yx));

      if(variations[n] < 0) continue;

      q.x = parameters3D[n*20 + 1] * w.x +
            parameters3D[n*20 + 2] * w.y +
            parameters3D[n*20 + 3] * w.z +
            parameters3D[n*20 + 4];
      q.y = parameters3D[n*20 + 5] * w.x +
            parameters3D[n*20 + 6] * w.y +
            parameters3D[n*20 + 7] * w.z +
            parameters3D[n*20 + 8];
      q.z = parameters3D[n*20 + 9] * w.x +
            parameters3D[n*20 + 10] * w.y +
            parameters3D[n*20 + 11] * w.z +
            parameters3D[n*20 + 12];
      q *= parameters3D[n*20 + 0];

      w = vec3(0, 0, 0);
        //w += vec3(sin(q.x), sin(q.y));

      switch(variations[n])
      {
        case 0: { // linear
          w += q;
        } break;

        case 1: { // 3D sinusoidal
          w += vec3(sin(q.x), sin(q.y), sin(q.z));
        } break;

        case 2: { // 3D spherical
          float rr = q.x*q.x + q.y*q.y + q.z*q.z;
          w += vec3(q.x/rr, q.y/rr, q.z/rr);
        } break;

        case 3: { // 3D swirl
          float rr = q.x*q.x + q.y*q.y + q.z*q.z;
          w += vec3(q.x*sin(rr) - q.y*cos(rr) + q.z*sin(rr),
                    q.x*cos(rr) + q.y*sin(rr) + q.z*sin(rr),
                    q.x*sin(rr) - q.y*sin(rr) - q.z*cos(rr));
        } break;

        case 4: { // horseshoe
          float r = length(q);
          w + vec3(((q.x-q.y)*(q.x+q.y)) / r, 2.0*q.x*q.y / r, 0);
        } break;

        case 5: { // polar
          float r = length(q);
          float p = atan(q.y, q.x);
          w += vec3(p/PI, r-1.0, 0);
        } break;
        /*case 6: { // 3D handkerchief, pyramid
          float r = length(q);
          float p = atan(q.y, q.x);
          float t = parameters3D[n*20 + 15];
          w += vec3(r*sin(p+r), r*cos(p-r), -scale+r);
          w += vec3(randomMinusOneToOne(q), randomMinusOneToOne(q*2.0), randomMinusOneToOne(q*3.0)) * t;
        } break;*/

        case 6: { // 3D handkerchief, cube
          float r = length(q);
          float p = atan(q.y, q.x);
          float pz = atan(q.z, length(q.xy));
          float t = parameters3D[n*20 + 15];
          w += vec3(r*sin(p+r), r*cos(p-r), r*cos(pz+r));
          w += vec3(randomMinusOneToOne(q), randomMinusOneToOne(q*2.0), randomMinusOneToOne(q*3.0)) * t;
        } break;

        /*case 7: { // heart
          float r = length(q);
          float p = atan(q.y, q.x);
          w += vec3(r*sin(p*r), -r*cos(p*r), 0);
        } break;*/
        case 7: { // 3D heart
          float r = length(q);
          float p = atan(q.y, q.x);
          float pz = atan(q.z, length(q.xy));
          w += vec3(r*sin(p*r), -r*cos(p*r), r*sin(pz*r));
        } break;


        /*case 8: { // disc
          float r = length(q);
          float p = atan(q.y, q.x);
          w += vec3(p/PI*sin(PI*r), p/PI*cos(p*r), 0);
        } break;*/
        case 8: { // 3D disc
          float r = length(q);
          float p = atan(q.y, q.x);
          float pz = atan(q.z, length(q.xy));
          w += vec3(p/PI*sin(PI*r), p/PI*cos(p*r), pz/PI*sin(PI*r));
        } break;

        /*case 9: { // spiral
          float r = length(q);
          float p = atan(q.y, q.x);
          w += vec3((cos(p) + sin(r))/r, (sin(p) - cos(r))/r, 0);
        } break;*/
        case 9: { // spiral
          float r = length(q);
          float p = atan(q.y, q.x);
          float pz = atan(q.z, length(q.xy));
          w += vec3((cos(p) + sin(r))/r, (sin(p) - cos(r))/r, -scale/2 + (sin(pz) - cos(r))/r);
        } break;

        /*case 10: { // hyperbolic
          float r = length(q);
          float p = atan(q.y, q.x);
          w += vec3(sin(p)/r, cos(p)*r, 0);
        } break;*/
        case 10: { // hyperbolic
          float r = length(q);
          float p = atan(q.y, q.x);
          float pz = atan(q.z, length(q.xy));
          w += vec3(sin(p)/r, cos(p)*r, sin(pz)/r);
        }

        case 11: { // diamond
          float r = length(q);
          float p = atan(q.y, q.x);
          w += vec3(sin(p)*cos(r), cos(p)*sin(r), 0);
        } break;

        case 12: { // ex
          float r = length(q);
          float p = atan(q.y, q.x);
          float p0 = sin(p+r);
          float p1 = cos(p-r);
          w += vec3(p0*p0*p0 + p1*p1*p1, p0*p0*p0 - p1*p1*p1, -p0*p0*p0 + p1*p1*p1);
        } break;

        case 13: { // julia
          float sr = sqrt(length(q));
          float p = atan(q.y, q.x);
          float pz = atan(q.z, length(q.xy));
          float k = randomMinusOneOrOne(q) * PI * 0.5;
          w += vec3(sr*cos(p*0.5 + k), sr*sin(p*0.5 + k), sr*cos(pz*0.5 + k));
        } break;

        case 14: { // bent
          if(q.x >= q.y && q.y >= 0) {
            w += vec3(q.x, q.y, 0);
          }
          else if(q.x < q.y && q.y >= 0) {
            w += vec3(2.0*q.x, q.y, 0);
          }
          else if(q.x >= q.y && q.y < 0) {
            w += vec3(q.x, 0.5*q.y, 0);
          }
          else {
            w += vec3(2.0*q.x, 0.5*q.y, 0);
          }
        } break;

        case 15: { // waves
          float b = parameters3D[n*20 + 13];
          float c = max(parameters3D[n*20 + 14], 1e-8);
          float f = max(parameters3D[n*20 + 15], 1e-8);
          float e = parameters3D[n*20 + 16];
          w += vec3(q.x + b*sin(q.y/(c*c)), q.y + e*sin(q.x/(f*f)), 0);
        } break;

        case 16: { // fisheye
          float r = length(q);
          w += vec3(2*q.y/(r+1), 2*q.x/(r+1), 0);
        } break;

        case 17: { // popcorn
          float c = parameters3D[n*20 + 13];
          float f = parameters3D[n*20 + 14];
          w += vec3(q.x + c*sin(tan(3*q.y)), q.y + f*sin(tan(3*q.x)), 0);
        } break;

        case 18: { // exponential
          float r = length(q);
          float t = exp(q.x-1);
          w += vec3(t * cos(q.y*PI), t * sin(q.y*PI), 0);
        } break;

        case 19: { // power
          float r = length(q);
          float p = atan(q.y, q.x);
          float t = pow(r, sin(p));
          w += vec3(t * cos(p), t * sin(p), 0);
        } break;

        case 20: { // cosine
          float p = atan(q.y, q.x);
          w += vec3(cos(PI*q.x)*cosh(q.y), -sin(PI*q.x)*sinh(q.y), 0);
        } break;

        case 21: { // rings
          float r = length(q);
          float u = atan(q.y, q.x);
          float cc = parameters3D[n*20 + 13]*parameters3D[n*20 + 13];
          float t = mod(r+cc, 2*cc) - cc + r*(1-cc);
          w += vec3(t*cos(u), t*sin(u), 0);
        } break;

        case 22: { // fan
          float r = length(q);
          float u = atan(q.y, q.x);
          float c = parameters3D[n*20 + 13];
          float f = parameters3D[n*20 + 14];
          float t = PI*c*c;
          if(mod(u+f, t) > t/2) {
            w += vec3(r*cos(u - t/2), r*sin(u - t/2), 0);
          }
          else {
            w += vec3(r*cos(u + t/2), r*sin(u + t/2), 0);
          }
        } break;

        case 23: { // blob
          float r = length(q);
          float u = atan(q.y, q.x);
          float p1 = parameters3D[n*20 + 13];
          float p2 = parameters3D[n*20 + 14];
          float p3 = parameters3D[n*20 + 15];
          float t = r * (p2 + (p1-p2)/2*(sin(p3*u)+1));
          w += vec3(t*cos(u), t*sin(u), 0);
        } break;

        case 24: { // pdj
          float p1 = parameters3D[n*20 + 13];
          float p2 = parameters3D[n*20 + 14];
          float p3 = parameters3D[n*20 + 15];
          float p4 = parameters3D[n*20 + 16];
          w += vec3(sin(p1*q.y) - cos(p2*q.x), sin(p3*q.x) - cos(p4*q.y), 0);
        } break;

        case 25: { // fan2
          float r = length(q);
          float u = atan(q.y, q.x);
          float p1 = PI*parameters3D[n*20 + 13]*parameters3D[n*20 + 13];
          float p2 = parameters3D[n*20 + 14];
          float t = u + p2 - p1*trunc(2*u*p2/p1);
          if(t > p1/2) {
            w += vec3(r*(sin(u - p1/2)), r*cos(u - p1/2), 0);
          }
          else {
            w += vec3(r*(sin(u + p1/2)), r*cos(u + p1/2), 0);
          }
        } break;

        case 26: { // rings2
          float r = length(q);
          float u = atan(q.y, q.x);
          float p = parameters3D[n*20 + 13]*parameters3D[n*20 + 13];
          float t = r - 2*p*trunc((r+p)/(2*p)) + r*(1-p);
          w += vec3(t*sin(u), t*cos(u), 0);
        } break;

        case 27: { // eyefish
          float r = length(q);
          w += vec3(2.0*q.x/(r+1), 2.0*q.y/(r+1), 0);
        } break;

        case 28: { // bubble
          float rr = q.x*q.x + q.y*q.y;
          w += vec3(4.0*q.x/(rr+4), 4.0*q.y/(rr+4), 0);
        } break;

        case 29: { // cylinder
          w += vec3(sin(q.x), q.y, 0);
        } break;

        case 30: { // perspective
          float p1 = parameters3D[n*20 + 13];
          float p2 = parameters3D[n*20 + 14];
          float t = p2/(p2 - q.y*sin(p1));
          w += vec3(t*q.x, t*q.y*cos(p1), 0);
        } break;

        case 31: { // noise
          float t0 = randomZeroToOne(q);
          float t1 = randomZeroToOne(q);
          float t2 = randomZeroToOne(q);
          w += vec3(t0*q.x*cos(TAU*t1), t0*q.y*sin(TAU*t1), t2*q.z*sin(TAU*t2));
        } break;

        case 32: { // julian
          float r = length(q);
          float u = atan(q.y, q.x);
          float p1 = parameters3D[n*20 + 13];
          float p2 = parameters3D[n*20 + 14];
          //float h1 = randomZeroToOne(q);
          //float t1 = trunc(abs(p1)*h1);
          float t1 = randomInteger(q, 0, floor(abs(p1)));
          float t2 = (u + TAU*t1) / p1;
          float t3 = pow(r, p2/p1);
          w += vec3(t3*cos(t2), t3*sin(t2), 0);
        } break;

        case 33: { // juliascope
          float r = length(q);
          float u = atan(q.y, q.x);
          float uz = atan(q.z, length(q.xy));

          float p1 = parameters3D[n*20 + 14];
          float p2 = parameters3D[n*20 + 15];
          //float h1 = randomZeroToOne(q);
          float h1 = randomMinusOneOrOne(q);
          float h2 = randomMinusOneOrOne(q*2);
          float h3 = randomMinusOneOrOne(q*3.0);
          //float t1 = trunc(abs(p1)*h1);
          float tx1 = randomInteger(q, 0, floor(abs(p1)));
          float tx2 = (h1*u + TAU*tx1) / p1;

          float ty1 = randomInteger(q*2, 0, floor(abs(p1)));
          float ty2 = (h2*u + TAU*ty1) / p1;

          float tz1 = randomInteger(q*3, 0, floor(abs(p1)));
          float tz2 = (h3*uz + TAU*tz1) / p1;

          float t3 = pow(r, p2/p1);

          w += vec3(t3*cos(tx2),
                    t3*sin(ty2),
                    t3*cos(tz2) * length(q.xyz) / length(q.xy));
        } break;

        case 34: { // blur
          float t0 = randomZeroToOne(q);
          float t1 = randomZeroToOne(q*2.0);
          float t2 = randomZeroToOne(q*3.0);
          w.x = t0*cos(TAU*t1);
          w.y = t0*sin(TAU*t1);
          w.z = t0*tan(PI*t2) * length(w.xy);
        } break;

        case 35: { // gaussian
          float t0 = randomZeroToOne(q);
          float t1 = randomZeroToOne(q*2.0);
          float t2 = randomZeroToOne(q*3.0);
          float t3 = randomZeroToOne(q*4.0);
          float s = t0 + t1 + t2 + t3 - 2;
          float t4 = randomZeroToOne(q*5.0);
          w += vec3(s*cos(TAU*t4), s*sin(TAU*t4), length(q));
        } break;

        /*case 65: {
          float r = length(q);
          float p = atan(q.y, q.x);
          float t = parameters3D[n*20 + 15];
          w += vec3(r*sin(p+r), r*cos(p-r), r);
          w += vec3(randomMinusOneToOne(q), randomMinusOneToOne(q*2.0), randomMinusOneToOne(q*3.0)) * t;
        } break;*/

        case 36: { // radialblur
          float r = length(q);
          float u = atan(q.y, q.x);
          float uz = atan(q.z, length(q.xy));
          float p1 = parameters3D[n*20 + 13] * PI * 0.5;
          float h1 = randomZeroToOne(q);
          float h2 = randomZeroToOne(q*2.0);
          float h3 = randomZeroToOne(q*3.0);
          float h4 = randomZeroToOne(q*4.0);
          float t1 = h1 + h2 + h3 + h4 - 2;
          float t2 = u + t1 * sin(p1);
          float t2z = uz + t1 * sin(p1);
          float t3 = t1 * cos(p1 - 1);
          float v36 = parameters3D[n*20 + 0];
          w += v36*vec3(r*cos(t2) + t3*q.x,
                        r*sin(t2) + t3*q.y,
                        r*sin(t2z) + t3*q.z);
        } break;

        case 37: { // pie
          float p1 = parameters3D[n*20 + 13];
          float p2 = parameters3D[n*20 + 14];
          float p3 = parameters3D[n*20 + 15];
          float h1 = randomZeroToOne(q);
          float h2 = randomZeroToOne(q);
          float h3 = randomZeroToOne(q);
          float t1 = trunc(h1*p1 + 0.5);
          float t2 = p2 + TAU/p1 * (t1 + h2*p3);
          w += vec3(h3*cos(t2), sin(t2), 0);
        } break;

        case 38: { // ngon
          float r = length(q);
          float u = atan(q.y, q.x);
          float p1 = parameters3D[n*20 + 13];
          float p2 = TAU / parameters3D[n*20 + 14];
          float p3 = parameters3D[n*20 + 15];
          float p4 = parameters3D[n*20 + 16];
          float t1 = u - p2*floor(u/p2);
          float t2 = t1 > 0.5*p2 ? t1 : t1 - p2;
          float k = (p3 * (1/cos(t2)-1) + 4) / (pow(r, p1));
          w += vec3(k*q.x, k*q.y, 0);
        } break;

        case 39: { // curl
          float p1 = parameters3D[n*20 + 13];
          float p2 = parameters3D[n*20 + 14];
          float t1 = 1 + p1*q.x + p2*(q.x*q.x - q.y*q.y);
          float t2 = p1*q.y + 2*p2*q.x*q.y;
          float t3 = 1.0/(t1*t1 + t2*t2);
          w += vec3((q.x*t1 + q.y*t2) * t3, (q.y*t1 - q.x*t2) * t3, 0);
        } break;

        case 40: { // rectangles
          float p1 = parameters3D[n*20 + 13];
          float p2 = parameters3D[n*20 + 14];
          w += vec3((2*floor(q.x/p1)+1)*p1 - q.x, (2*floor(q.y/p2)+1)*p2 - q.y, 0);
        } break;

        case 41: { // arch (what's the point...)
          float t0 = randomZeroToOne(q);
          //float t1 = randomZeroToOne(q);
          //float t2 = randomZeroToOne(q);
          float v41 = parameters3D[n*20 + 0];
          float s = sin(t0*v41);
          w += vec3(sin(t0 * PI * v41), s*s / cos(t0 * PI * v41), 0);
        } break;

        case 42: { // tangent (what's the point...)
          w += vec3(sin(q.x)/cos(q.y), tan(q.y), 0);
        } break;

        case 43: { // square (what's the point...)
          float t0 = randomZeroToOne(q);
          float t1 = randomZeroToOne(q);
          w += vec3(t0-0.5, t1-0.5, 0);
        } break;

        case 44: { // rays (what's the point...)
          float rr = q.x*q.x + q.y*q.y;
          float v44 = parameters3D[n*20 + 0];
          float t = randomZeroToOne(q);
          float s = v44 * tan(t*PI*v44) / rr;
          w += vec3(s*cos(q.x), s*sin(q.y), 0);
        } break;

        case 45: { // blade (what's the point...)
          float r = length(q);
          float v45 = parameters3D[n*20 + 0];
          float t0 = randomZeroToOne(q);
          //float t1 = randomZeroToOne(q);
          //float t2 = randomZeroToOne(q);
          //float t3 = randomZeroToOne(q);
          w += vec3(q.x * (cos(t0 * r * v45) + sin(t0 * r * v45)),
                q.x * (cos(t0 * r * v45) - sin(t0 * r * v45)), 0);
        } break;

        case 46: { // secant
          float r = length(q);
          float v46 = parameters3D[n*20 + 0];
          w += vec3(q.x, 1.0 / (v46*cos(v46*r)), 0);
        } break;

        case 47: { // twintrian (what's the point...)
          float r = length(q);
          float v47 = parameters3D[n*20 + 0];
          float h0 = randomZeroToOne(q);
          //float h1 = randomZeroToOne(q);
          //float h2 = randomZeroToOne(q);
          float s = sin(h0*r*v47);
          float t = log(s*s) / log(10.0) + cos(h0*r*v47);
          w += vec3(q.x*t, q.x*(t - PI*sin(h0*r*v47)), 0);
        } break;

        case 48: { // cross (what's the point...)
          float t = sqrt(1.0 / (q.x*q.x - q.y*q.y));
          w += vec3(t*q.x, t*q.y, 0);
        } break;

        case 49: { //
          float t = randomZeroOrOne(q);
          float r = length(q);
          r = t * sqrt(r) + (1.0-t) * r;
          w += vec3(q.x/r, q.y/r, 0);
        } break;

        case 50: { //
          float p = randomMinusOneOrOne(q) * PI * 0.5;
          w += vec3(sin(q.x + p), sin(q.y + p), 0);
        } break;

        case 51: { //
          float rr = q.x*q.x + q.y*q.y;
          w += vec3(q.x/rr, q.y/rr, 0);
        } break;

        case 52: { //
          float r = length(q);
          w += vec3(q.x*sin(r) - q.y*cos(r), q.x*cos(r) + q.y*sin(r), 0);
        } break;

        case 53: { //
          float r = sqrt(length(q));
          w += vec3(((q.x-q.y)*(q.x+q.y)) / r, 2.0*q.x*q.y / r, 0);
        } break;

        case 54: { //
          float r = length(q);
          float p = atan(q.y, q.x);
          w += vec3(p/PI, q.x/r, 0);
        } break;

        case 55: { //
          float r = length(q);
          float p = atan(q.y, q.x);
          w += vec3(r*sin(p+r), r*cos(p-r), 0);
        } break;

        case 56: { //
          float r = sqrt(length(q));
          float p = atan(q.y, q.x);
          w += vec3(r*sin(p+r), r*cos(p-r), 0);
        } break;

        case 57: { // seems to be usable
          float t = randomZeroOrOne(q);
          float r = length(q);
          r = t * sqrt(r) + (1.0-t) * r;
          float p = atan(q.y, q.x);
          w += vec3(r*sin(p+r), r*cos(p-r), 0);
        } break;

        case 58: { // quite good
          float r = length(q);
          float p = atan(q.y, q.x);
          p += randomMinusOneOrOne(q) * PI * 0.5;
          w += vec3(r*sin(p+r), r*cos(p-r), 0);
        } break;

        case 59: { // quite good
          float r = sqrt(length(q));
          float p = atan(q.y, q.x);
          p += randomMinusOneOrOne(q) * PI * 0.5;
          w += vec3(r*sin(p+r), r*cos(p-r), 0);
        } break;

        case 60: { // quite good
          float t = randomZeroOrOne(q);
          float r = length(q);
          r = t * sqrt(r) + (1.0-t) * r;
          float p = atan(q.y, q.x);
          float pz = atan(q.z, length(q.xy));
          p += randomMinusOneOrOne(q) * PI * 0.5;
          w += vec3(r*sin(p+r), r*cos(p-r), r*sin(pz+r));
        } break;

        case 61: { // usable, sparse, quite rich
          float r = length(q);
          float p = atan(q.y, q.x);
          w += vec3(r*sin(p+r+randomMinusOneOrOne(q) * PI * 0.5), r*cos(p-r+randomMinusOneOrOne(q) * PI * 0.5), 0);
        } break;

        case 62: { // usable, dense, not very rich
          float r = sqrt(length(q));
          float p = atan(q.y, q.x);
          w += vec3(r*sin(p+r+randomMinusOneOrOne(q) * PI * 0.5), r*cos(p-r+randomMinusOneOrOne(q) * PI * 0.5), 0);
        } break;

        case 63: { //
          float sr = sqrt(length(q));
          float p = atan(q.y, q.x);
          float k1 = randomMinusOneOrOne(q) * PI * 0.5;
          float k2 = randomMinusOneOrOne(q) * PI * 0.5;
          w += vec3(sr*cos(p*0.5 + k1), sr*sin(p*0.5 + k2), 0);
        } break;

        case 64: { //
          float sr = sqrt(length(q));
          float p = atan(q.y, q.x);
          float k1 = randomMinusOneOrOne(q) * PI * 0.5;
          float k2 = randomMinusOneOrOne(q) * PI * 0.5;
          w += vec3(sr*cos(p*0.5 + k1)*q.x, sr*sin(p*0.5 + k2)*q.y, 0);
        } break;

        case 65: { //
          float sr = length(q);
          float p = atan(q.y, q.x);
          float pz = atan(q.z, length(q.xy));
          float t = max(floor(abs(parameters3D[n*20 + 14])), 1.0);
          float u = parameters3D[n*20 + 15] * 0.5;
          float s1, s2, s3;
          s1 = randomInteger(q, -t, t+1);
          s2 = randomInteger(q*2.0, -t, t+1);
          s3 = randomInteger(q*3.0, -t, t+1);
          float k1 = s1 * PI / (1.0+t);
          float k2 = s2 * PI / (1.0+t);
          float k3 = s3 * PI / (1.0+t);
          w += vec3(1.0/sr*cos(p*u + k1) * length(q.xy) / length(q.xyz),
                    1.0/sr*sin(p*u + k2) * length(q.xy) / length(q.xyz),
                    1.0/sr*cos(pz*u + k3));
        } break;


        // I noticed that in that julia formula, which I was so fond of, the orbit was based on deciding on every iteration
        // whether to turn 90 degress (or some other definite amount) left or right.

        // Here I applied vector cross product to have similar decision in 3D.
        // Quite nice, but maybe too old school 3D "imposter" effect this one


        /*case XX: { // 3D cone
          float sr = length(q);
          vec3 v = vec3(randomMinusOneToOne(q), randomMinusOneToOne(q*2.0), randomMinusOneToOne(q*3.0));
          vec3 a = cross(q, v);
          vec3 na = normalize(a) * t;
          vec3 nq = normalize(q) * (1-t);
          vec3 tt = na + nq;
          tt = normalize(tt);
          w += tt * random(q*6.0, 0, sr);
        } break;*/

        /*case XX: { // 3D julia planes
          float sr = length(q);
          vec3 v = vec3(randomMinusOneOrOne(q), randomMinusOneOrOne(q*2.0), randomMinusOneOrOne(q*3.0));
          vec3 a = cross(q, v);
          w += a / length(a) / sr;
        } break; */

        /*case XX: { // 3D hollow sphere
          vec3 v = vec3(random(q, -1, 1), random(q*2.0, -1, 1), random(q*3.0, -1, 1));
          vec3 a = cross(q, v);
          w += a / length(a);
        } break;*/
        /* case XX: { // 3D hollow blob
          float sr = sqrt(length(q));
          vec3 v = vec3(random(q, -1, 1), random(q*2.0, -1, 1), random(q*3.0, -1, 1));
          vec3 a = cross(q, v);
          w += sr * a / length(a);
        } break;*/
        /*case XX: { // 3D ring clouds
          float sr = length(q);
          vec3 v = vec3(random(q, -1, 1), random(q*2.0, -1, 1), random(q*3.0, -1, 1));
          vec3 a = cross(q, v);
          w += a / length(a) / sr;
          //w += vec3(sr*cos(p*u + k1), sr*sin(p*u + k2), 0);
        } break;*/


        case 66: { // 3D better blob
          float sr = sqrt(length(q));
          float t = parameters3D[n*20 + 15];
          vec3 v = vec3(randomMinusOneToOne(q), randomMinusOneToOne(q*2.0), randomMinusOneToOne(q*3.0));
          vec3 a = cross(q, v);
          vec3 na = normalize(a) * t;
          vec3 nq = normalize(q) * (1-t);
          vec3 tt = na + nq;
          tt = normalize(tt);
          w += tt / sr;
        } break;

/* ...in my afterlife:
 *  "Everyone wanting to work at research and development section apply now"
 *  Me running already one mile away.
 */

        /*case 66: { //
          float sr = sqrt(length(q));
          float p = atan(q.y, q.x);
          float k1 = randomMinusOneOrOne(q) * PI * 0.5;
          float k2 = randomMinusOneOrOne(q) * PI * 0.5;
          w += vec3(sr*cos(p*0.5 + k1)*q.x, sr*sin(p*0.5 + k2)*q.y, 0);
        } break;*/

        /*case 66: { // julia
          float sr = sqrt(length(q));
          float p = atan(q.y, q.x);
          float pz = atan(q.z, length(q.xy));
          float k = randomMinusOneOrOne(q) * PI * 0.5 * parameters3D[n*20 + 14];
          float t = parameters3D[n*20 + 13] * 0.5;
          w += vec3(sr*cos(p*t + k), sr*sin(p*t + k), sr*cos(pz*t + k));
        } break;*/

      }
    }
    /*if(i >= minIter) {
      int px = int(random(gl_FragCoord.xy*time*325+vec3(-3455+i*45,463), 0, isize.x-1));
      if(px >= 0 && px < isize.x) {
        int py = int(random(vec3(124, 645)+gl_FragCoord.xy*time*24.53*i, 0, isize.y-1));
        if(py >= 0 && py < isize.y) {
          imageAtomicAdd(hitCounterImage, ivec3(px, py), 1);
        }
      }
    }*/
    /*int getPixelIndex(vec3d &v) {
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
      int px = int(map(w.x-location.x, xRange.x*scale, xRange.y*scale, 0, gridSize.x-1));
      if(px >= 0 && px < gridSize.x) {
        int py = int(map(w.y-location.y, yRange.x*scale, yRange.y*scale, 0, gridSize.y-1));
        if(py >= 0 && py < gridSize.y) {
          int pz = int(map(w.z-location.z, zRange.x*scale, zRange.y*scale, 0, gridSize.z-1));
          if(pz >= 0 && pz < gridSize.z) {
            imageAtomicAdd(hitCounterVoxels, ivec3(px, py, pz), 1);
          }
        }
      }
    }

  }



}
