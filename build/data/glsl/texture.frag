#version 450 core

uniform sampler2D tex;

in vec2 texCoord;
in vec4 color;
out vec4 colorOut;


void main(){
  //colorOut = texture(tex, fragTexCoord);
  colorOut = texture(tex, texCoord);
  //color = fragColor;
  //color = mix(vec4(0.6, 0, 0.3, 1.0), color, 0.5);
  //color = vec4(0.6, 0, 0.3, 1.0);
}
