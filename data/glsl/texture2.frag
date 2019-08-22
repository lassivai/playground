#version 450 core

uniform sampler2D tex1, tex2;
uniform vec2 screenSize;

in vec2 texCoord;
in vec4 color;
out vec4 colorOut;


void main(){
  //color = mix(texture(tex1, fragTexCoord), texture(tex2, fragTexCoord), 0.5);
  vec4 a = texture(tex1, texCoord);
  vec4 b = texture(tex2, gl_FragCoord.xy/screenSize.xy);

  colorOut = a * b;
  //color = fragColor;
  //color = mix(vec4(0.6, 0, 0.3, 1.0), color, 0.5);
  //color = vec4(0.6, 0, 0.3, 1.0);
}
