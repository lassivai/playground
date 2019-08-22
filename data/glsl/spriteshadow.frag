#version 450 core

uniform sampler2D sprite;

uniform ivec2 spriteSize;
uniform int shadowWidth;
uniform float shadowPower;
uniform vec4 shadowColor;
uniform float shadowFactor;

uniform bool inverseTextureCoordinates = false; // FIXME

in vec2 texCoord;
in vec4 color;
out vec4 colorOut;


void main() {
  vec2 size = spriteSize + vec2(shadowWidth*2, shadowWidth*2);
  vec2 pos = texCoord * size;
  
  int n = shadowWidth*shadowWidth;
  double t = 0;
  double total = 0;
  
  for(int dx=-shadowWidth; dx<shadowWidth-1; dx++) {
    for(int dy=-shadowWidth; dy<shadowWidth-1; dy++) {
      vec2 dpos = vec2(pos.x+dx-shadowWidth, pos.y+dy-shadowWidth);
      /*if(dpos.x < 0 || dpos.y < 0 || dpos.x >= spriteSize.x || dpos.y >= spriteSize.y) {
        continue;
      }*/
      //n++;
      //double dist = clamp(distance(pos, dpos+vec2(shadowWidth, shadowWidth))/size.x, 0.0, 1.0);
      double dist = clamp(distance(vec2(0, 0), vec2(dx, dy))/shadowWidth, 0.0, 1.0);
      //double dist = 0;
      double r = pow(1.0-float(dist), 5);
      total += r;
      //double r = 1.0-float(dist);
      //t += texture(sprite, dpos/spriteSize).a;
      vec2 v;
      
      if(!inverseTextureCoordinates) {
        v = dpos/spriteSize;
      }
      else {
        v.x = dpos.x/spriteSize.x;
        v.y = 1 - dpos.y/spriteSize.y;
      }
      
      if(v.x >= 0 && v.y >= 0 && v.x < 1.0 && v.y < 1.0) t += r * texture(sprite, v).a;
    }
  }
  //t = clamp(t/double(n)*2.0, 0, 1);
  //t /= total;
  t = clamp(pow(float(t / total * shadowFactor), 0.5f), 0.0f, 1.0f);
  //t = t/double(n);
  
  /*vec2 v = vec2(1, 1) - (pos-vec2(shadowWidth, shadowWidth))/spriteSize;
  if(v.x >= 0 && v.y >= 0 && v.x < 1.0 && v.y < 1.0) t = texture(sprite, v).a;
  else t = 0;*/
  
  colorOut = vec4(shadowColor.r, shadowColor.g, shadowColor.b, shadowColor.a * t);
  //colorOut = vec4(0.6, 0.2, 0.1, t);
}
