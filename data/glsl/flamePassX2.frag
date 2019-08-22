
#version 430

//const float PI = 3.14159265359;
//const float TAU = 2.0*3.14159265359;

//uniform vec2 screenSize;
//uniform vec2 mousePos;

//layout (binding = 0, r32ui) uniform uimage2D hitCounterImage;
layout (r32ui) uniform uimage2DArray hitCounterImageArray;
layout (binding = 0, r32ui) uniform uimage2D minMaxHitCounterImage;

uniform int activeHitCounterImageIndex;

in vec2 texCoord;

layout(location=0) out vec4 colorOut;

void main()
{
  //ivec2 isize = imageSize(hitCounterImage);
  //ivec2 ipos = ivec2(texCoord * isize);
  ivec2 ipos = ivec2(gl_FragCoord.xy);

  uint hits = uint(imageLoad(hitCounterImageArray, ivec3(ipos.x, ipos.y, activeHitCounterImageIndex)));

  //if(hits > 0) {
    imageAtomicMin(minMaxHitCounterImage, ivec2(0, 0), uint(hits));
  //}
  imageAtomicMax(minMaxHitCounterImage, ivec2(1, 0), uint(hits));
}
