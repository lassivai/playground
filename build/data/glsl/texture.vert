#version 450 core

uniform mat4 gl_ModelViewMatrix;
uniform mat4 gl_ProjectionMatrix;

layout(location = 0) in vec3 vertexIn;
layout(location = 1) in vec3 normalIn;
layout(location = 2) in vec2 texCoordIn;
layout(location = 3) in vec4 colorIn;

out vec4 vertex;
out vec3 normal;
out vec2 texCoord;
out vec4 color;

void main()
{
  vertex = gl_ModelViewMatrix * vec4(vertexIn, 1.0);
  texCoord = texCoordIn;
  color = colorIn;
  normal = normalIn; // FIXME shouldn't this be multiplied by some normal matrix...


  gl_Position = gl_ProjectionMatrix * vertex;
}
