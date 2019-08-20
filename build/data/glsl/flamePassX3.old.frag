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

  float k = 0;
  for(int i=0; i<numHitCounterImages; i++) {
    //float minValue = float(imageLoad(minMaxHitCounterImages[i], ivec2(0, 0)));
    //float maxValue = float(imageLoad(minMaxHitCounterImages[i], ivec2(1, 0)));
    float f = float(imageLoad(hitCounterImageArray, ivec3(ipos.x, ipos.y, i)));


    float a = log(1.0+hitCounterImageMinMaxs[i].x);
    float b = log(1.0+hitCounterImageMinMaxs[i].y);
    //float d = a;

    //a = a*(1.0 - inverseColors) + b*inverseColors;
    //b = b*(1.0 - inverseColors) + d*inverseColors;

    //float a = 0, b = log(100);
    float c = log(1.0+f);
    k += (c-a) / (b - a);
  }

  k = k * brightness;

  k = k * (1.0-inverseColors) + (1.0 - min(1.0, k)) * inverseColors;

  k = clamp(k, 0.0, 1.0);

  vec3 col = hslToRgb(colorArgs[0] + k * colorArgs[1], colorArgs[2], k);

  colorOut = vec4(col.x, col.y, col.z, 1.0);

}
