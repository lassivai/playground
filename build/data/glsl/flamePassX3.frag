#version 430

const float PI = 3.14159265359;
const float TAU = 2.0*3.14159265359;


float hue2rgb(float p, float q, float t) {
  if(t < 0.0) t += 1.0;
  if(t > 1.0) t -= 1.0;
  if(t < 1.0/6.0) return p + (q - p) * 6.0 * t;
  if(t < 1.0/2.0) return q;
  if(t < 2.0/3.0) return p + (q - p) * (2.0/3.0 - t) * 6.0;
  return p;
}

vec3 hslToRgb(float h, float s, float v) {
  vec3 col;
  h = fract(h / TAU);
  //h = fract(modff(h, TAU));
  //h = fract(h);

  if(s == 0) {
    col = vec3(v, v, v);
  }
  else {
    float q = v < 0.5 ? v * (1.0 + s) : v + s - v * s;
    float p = 2.0 * v - q;

    col.x = hue2rgb(p, q, h + 1.0/3.0);
    col.y = hue2rgb(p, q, h);
    col.z = hue2rgb(p, q, h - 1.0/3.0);
  }

  return col;
}

//uniform vec2 screenSize;
//uniform vec2 mousePos;

layout (r32ui) uniform uimage2DArray hitCounterImageArray;
//layout (r32ui) uniform uimage2D minMaxHitCounterImages[4];

uniform int numHitCounterImages;
uniform vec2 hitCounterImageMinMaxs[512];
uniform float brightness;

uniform float colorArgs[3];

uniform float inverseColors;

in vec2 texCoord;

layout(location=0) out vec4 colorOut;

void main()
{
  //ivec2 ipos = ivec2(texCoord * isize);
  ivec2 ipos = ivec2(gl_FragCoord.xy);
  float minTotal = 0, maxTotal = 0;
  float valueTotal = 0;
  for(int i=0; i<numHitCounterImages; i++) {
    valueTotal += float(imageLoad(hitCounterImageArray, ivec3(ipos.x, ipos.y, i)));
    minTotal += hitCounterImageMinMaxs[i].x; // could be calculated before gragment shader
    maxTotal += hitCounterImageMinMaxs[i].y;
  }
  float a = log(1.0+minTotal);
  float b = log(1.0+maxTotal);

  float c = log(1.0+valueTotal);
  float value = (c-a) / (b - a);

  value = value * brightness;

  value = value * (1.0-inverseColors) + (1.0 - min(1.0, value)) * inverseColors;

  value = clamp(value, 0.0, 1.0);

  vec3 col = hslToRgb(colorArgs[0] + value * colorArgs[1], colorArgs[2], value);

  colorOut = vec4(col.x, col.y, col.z, 1.0);

}
