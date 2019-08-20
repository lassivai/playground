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


uniform int maxIter;
/*
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

uniform sampler2D tex, iterationTexture;
*/
layout (binding = 0, r32f) uniform image2D iterationImage;
layout (binding = 0, r32ui) uniform uimage2D multisamplingRestrictionImage;

//layout(location=0) out vec4 colorOut;


float random(in vec2 st) {
  return fract(sin(dot(st.xy, vec2(12.9898,78.233))) * 43758.5453123);
}
float random(in vec2 st, in float a, in float b) {
  return fract(sin(dot(st.xy, vec2(12.9898,78.233))) * 43758.5453123)*(b-a)+a;
}
double randomd(in vec2 st, in float a, in float b) {
  return double(fract(sin(dot(st.xy, vec2(12.9898,78.233))) * 43758.5453123)*(b-a)+a);
}


void main()
{
  ivec2 ipos = ivec2(gl_FragCoord.xy);

  for(int i=-1; i<2; i++) {
    for(int j=-1; j<2; j++) {
      float b = float(imageLoad(iterationImage, ipos + ivec2(i, j)));
      if(b < maxIter) {
        imageStore(multisamplingRestrictionImage, ipos, uvec4(0));
        discard;
      }
    }
  }

  imageStore(multisamplingRestrictionImage, ipos, uvec4(1));

}
