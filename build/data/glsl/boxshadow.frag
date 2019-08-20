#version 450 core

//uniform sampler2D tex;

uniform ivec2 boxSize;//, displacement;
uniform int shadowWidth;
uniform float shadowAlpha, shadowPower;
//uniform vec4 backgroundColor; // solve the render to texture transparency issue next time
//uniform int onlyExterior;

in vec2 texCoord;
in vec4 color;
out vec4 colorOut;

void main() {
  vec2 size = boxSize + vec2(shadowWidth*2, shadowWidth*2);
  vec2 pos = texCoord * size;
  
  vec2 tpos = pos - vec2(shadowWidth, shadowWidth);
    
  double dist = 0;
  
  if(tpos.y >= 0 && tpos.y < boxSize.y) {
    if(tpos.x < 0) {
      dist = -tpos.x;
    }
    else {
      dist = tpos.x-boxSize.x;
    }
  }
  else if(tpos.x >= 0 && tpos.x < boxSize.x) {
    if(tpos.y < 0) {
      dist = -tpos.y;
    }
    else {
      dist = tpos.y-boxSize.y;
    }
  }
  else if(tpos.x < 0 && tpos.y < 0) {
    dist = distance(vec2(0, 0), tpos);
  }
  else if(tpos.x < 0 && tpos.y >= boxSize.y) {
    dist = distance(vec2(0, boxSize.y), tpos);
  }
  else if(tpos.x >= boxSize.x && tpos.y < 0) {
    dist = distance(vec2(boxSize.x, 0), tpos);
  }
  else if(tpos.x >= boxSize.x && tpos.y >= boxSize.y) {
    dist = distance(vec2(boxSize.x, boxSize.y), tpos);
  }
  if(dist > 0) {
    dist = clamp(dist/shadowWidth, 0.0, 1.0);
    double r = pow(1.0-float(dist), shadowPower);
    colorOut = vec4(0.0, 0.0, 0.0, r*shadowAlpha);
  }
  else {
    colorOut = vec4(0.0, 0.0, 0.0, shadowAlpha);
  }
}
