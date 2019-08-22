#version 450 core

//uniform sampler2D tex, tex2;

//uniform int textureType = 0;
//uniform int geomType = 0, geomSubType = 0;
//uniform vec4 fillColor = vec4(1.0, 1.0, 1.0, 1.0);
uniform vec4 strokeColor = vec4(0.0, 0.0, 0.0, 1.0);
/*uniform float strokeWidth = 2;
uniform vec2 screenSize, objectSize;
//uniform vec3 triangleA, triangleB, triangleC;
uniform int strokeType = 2;
uniform float innerRadius = 0.7;

uniform float scale = 1.0;*/

in vec2 texCoord;
in vec4 color;
in vec4 vertex;
out vec4 colorOut;


void main() {
  colorOut = strokeColor;
}
