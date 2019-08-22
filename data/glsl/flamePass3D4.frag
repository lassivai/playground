#version 430

layout (r32ui) uniform uimage2DArray hitCounterVoxels;

uniform ivec3 gridSize;

in vec2 texCoord;

void main()
{
  //ivec3 gridSize = imageSize(hitCounterVoxels).xyz;

  for(int i=0; i<gridSize.z; i++) {
    imageStore(hitCounterVoxels, ivec3(texCoord.x*gridSize.x, texCoord.y*gridSize.y, i), uvec4(0));
  }
}
