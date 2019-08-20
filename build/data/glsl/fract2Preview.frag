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

uniform int pass;

uniform dvec2 location;
uniform double scale;
uniform double maxIter;
uniform dvec2 juliaPoint;
uniform int mandelbrotMode;
uniform int mappingMode;
uniform int colorMode;
uniform int postEffectMode;

uniform float spectrum[1000];
uniform int spectrumResolution;

uniform int multisamplingLevel;
uniform float multisamplingRange;
uniform float multisamplingRangePower;

uniform float previewIterationFactor;
uniform int previewActive;

uniform sampler2D tex, iterationTexture;

layout (binding = 0, r32f) uniform image2D iterationImage;
/*layout (binding = 0, r32ui) uniform uimage2D iterationMinMaxImage;
layout (binding = 0, r32ui) uniform uimage2D multisamplingRestrictionImage;*/

layout(location=0) out vec4 colorOut;


float random(in vec2 st) {
  return fract(sin(dot(st.xy, vec2(12.9898,78.233))) * 43758.5453123);
}
float random(in vec2 st, in float a, in float b) {
  return fract(sin(dot(st.xy, vec2(12.9898,78.233))) * 43758.5453123)*(b-a)+a;
}
double randomd(in vec2 st, in float a, in float b) {
  return double(fract(sin(dot(st.xy, vec2(12.9898,78.233))) * 43758.5453123)*(b-a)+a);
}

/*float noise(vec2 st) {
    vec2 i = floor(st);
    vec2 f = fract(st);
    vec2 u = f*f*(3.0-2.0*f);
    return mix( mix( random( i + vec2(0.0,0.0) ),
                     random( i + vec2(1.0,0.0) ), u.x),
                mix( random( i + vec2(0.0,1.0) ),
                     random( i + vec2(1.0,1.0) ), u.x), u.y);
}*/
void main()
{
  ivec2 ipos = ivec2(gl_FragCoord);
  double maxIterations = max(maxIter * (1.0-previewActive + previewIterationFactor * previewActive), 1.0);

  if(ipos.x % 16 != 0 || ipos.y % 16 != 0) {
    float f = float(imageLoad(iterationImage, ipos/16*16));
    if(f < maxIterations) {
      colorOut = vec4(0, 0, 0, 1);
    }
    else {
      colorOut = vec4(1, 1, 1, 1);
    }
  }

  else {

    vec2 uPos = gl_FragCoord.xy / screenSize.xy;
    float iterations = 0;

    dvec2 pos = dvec2(gl_FragCoord.xy) / (screenSize.y*0.5) - dvec2(screenSize.x / screenSize.y, 1.0);

    pos *= scale * dvec2(1.0, -1.0);
    pos += location;


    dvec2 c = pos;
    dvec2 w = dvec2(pos), tmpW = dvec2(0, 0);

    if(mandelbrotMode == 1) {
      w = dvec2(0,0);
    }
    if(juliaPoint.x != 0.0 || juliaPoint.y != 0) {
      c = juliaPoint;
    }
    if(mod(mandelbrotMode, 2) == 1) {
      double m = 1.0/(c.x*c.x + c.y*c.y);
      c = dvec2(c.x*m, -c.y*m);
    }


    if(mandelbrotMode < 2) {
      while(w.x*w.x + w.y*w.y < 4.0 && iterations < maxIterations) {
        tmpW.x = w.x*w.x - w.y*w.y + c.x;
        tmpW.y = 2.0 * w.x * w.y + c.y;
        w.x = tmpW.x;
        w.y = tmpW.y;
        iterations++;
      }
    }
    if(mandelbrotMode > 1) {
      while(w.x*w.x + w.y*w.y < 40.0 && iterations < maxIterations) {
        tmpW.x = w.x*w.x - w.y*w.y + c.x;
        tmpW.y = 2.0 * w.x * w.y + c.y;
        double m = 1.0/(tmpW.x*tmpW.x + tmpW.y*tmpW.y);
        w.x = tmpW.x * m;
        w.y = -tmpW.y * m;
        iterations++;
      }
    }


    imageStore(iterationImage, ivec2(gl_FragCoord), vec4(iterations));


    //float f = float(iterations/maxIterations);

    if(iterations < maxIterations) {
      colorOut = vec4(0, 0, 0, 1);
    }
    else {
      colorOut = vec4(1, 1, 1, 1);
    }
  }
}
