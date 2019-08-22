
#version 430

//const float PI = 3.14159265359;
//const float TAU = 2.0*3.14159265359;

//uniform vec2 screenSize;
//uniform vec2 mousePos;

layout (binding = 0, r32ui) uniform uimage2D hitCounterImage;
layout (binding = 0, r32ui) uniform uimage2D minMaxHitCounterImage;

in vec2 texCoord;

layout(location=0) out vec4 colorOut;

void main()
{
  //ivec2 ipos = ivec2(texCoord * isize);
  ivec2 ipos = ivec2(gl_FragCoord.xy);

  float minValue = float(imageLoad(minMaxHitCounterImage, ivec2(0, 0)));
  float maxValue = float(imageLoad(minMaxHitCounterImage, ivec2(1, 0)));
  float f = float(imageLoad(hitCounterImage, ipos));
  //imageStore(hitCounterImage, ipos, uvec4(0));

  float j = log(100.0);

  float a = log(1.0+minValue);
  float b = log(1.0+maxValue);
  float c = log(1.0+f);
  float k = (c-a) / (b - a);
  colorOut = vec4(k, k, k, 1.0);
  //colorOut = vec4(0, 0, 0, 1);

  /*f = (f-minValue) / (maxValue-minValue);
  //f = f / 200;
  f = 1.0 - pow(1.0-f, 200.0);
  colorOut = vec4(f, f, f, 1.0);*/



  //if(f < 100) colorOut = vec4(0, 0, 0, 1.0);
  //else  colorOut = vec4(1, 1, 1, 1.0);
}
