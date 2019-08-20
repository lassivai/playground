#version 430


layout (r32ui) uniform uimage2DArray hitCounterVoxels;
layout (r32ui) uniform uimage2D minMaxHitCounterImage;

uniform ivec3 gridSize;

in vec2 texCoord;

//layout(location=0) out vec4 colorOut;

void main()
{
  //ivec3 gridSize = imageSize(hitCounterVoxels).xyz;
  ivec2 ipos = ivec2(texCoord.x*gridSize.x, texCoord.y*gridSize.y);

  for(int i=0; i<gridSize.z; i++) {
    uint hits = uint(imageLoad(hitCounterVoxels, ivec3(ipos.x, ipos.y, i)));
    imageAtomicMin(minMaxHitCounterImage, ivec2(0, 0), uint(hits));
    imageAtomicMax(minMaxHitCounterImage, ivec2(1, 0), uint(hits));
  }
}
