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

float rgbToAlpha(in vec3 col) {
  float minValue = min(col.r, min(col.g, col.b));
  float maxValue = max(col.r, max(col.g, col.b));
  return (minValue+maxValue) / 2.0;
}

//uniform vec2 screenSize;
//uniform vec2 mousePos;

layout (rgba32f) uniform image2DArray colorVoxels;

layout (r32ui) uniform uimage2DArray hitCounterVoxels;
layout (r32ui) uniform uimage2D minMaxHitCounterImage;

uniform float brightness;
uniform float colorArgs[3];

uniform ivec3 gridSize;

uniform float minValue, maxValue;

in vec2 texCoord;

layout(location=0) out vec4 colorOut;

void main()
{
  //ivec3 gridSize = imageSize(hitCounterVoxels).xyz;
  //float minValue = float(imageLoad(minMaxHitCounterImage, ivec2(0, 0)));
  //float maxValue = float(imageLoad(minMaxHitCounterImage, ivec2(1, 0)));
  //float minValue = 0, maxValue = 10000;
  float a = log(1.0+minValue);
  float b = log(1.0+maxValue);
  ivec3 pos = ivec3(texCoord.x*gridSize.x, texCoord.y*gridSize.y, 0);

  for(int i=0; i<gridSize.z; i++) {
    pos.z = i;
    float f = float(imageLoad(hitCounterVoxels, pos));
    float c = log(1.0+f);
    float k = (c-a) / (b - a);
    k = k * brightness;
    k = clamp(k, 0.0, 1.0);
    vec3 col = hslToRgb(colorArgs[0] + k * colorArgs[1], colorArgs[2], k);
    imageStore(colorVoxels, pos, vec4(col.r, col.g, col.b, rgbToAlpha(col)));
    /*vec4 col = vec4(0, 0, 1, 1);
    col.r = pos.x == pos.y ? 0.4 : 0;
    col.g = pos.z == pos.y ? 0.4 : 0;
    col.b = pos.x == pos.z ? 0.4 : 0;
    col.a = 0.4;
    imageStore(colorVoxels, pos, col);*/
  }
}
