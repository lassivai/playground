#version 450 core

uniform sampler2D tex;
uniform vec4 colorify;
uniform vec2 screenSize;
uniform int usePixelCoordinates = 0;
uniform int flipY = 1;

in vec2 texCoord;
in vec4 color;
out vec4 colorOut;


void main() {
  vec2 uv = texCoord;

  if(usePixelCoordinates == 1) {
    uv = gl_FragCoord.xy/screenSize.xy;
    if(flipY == 1) {
      uv.y = 1.0 - uv.y;
    }
  }

  colorOut = texture(tex, uv) * colorify;
}
