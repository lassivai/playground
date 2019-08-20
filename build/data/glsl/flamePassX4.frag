#version 430

layout (r32ui) uniform uimage2DArray hitCounterImageArray;
uniform int activeHitCounterImageIndex;

void main()
{
  imageStore(hitCounterImageArray, ivec3(gl_FragCoord.x, gl_FragCoord.y, activeHitCounterImageIndex), uvec4(0));
}
