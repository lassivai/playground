
#version 400

const float PI = 3.14159265359;
const float TAU = 2.0*3.14159265359;

uniform vec2 screenSize;
uniform vec2 mousePos;

uniform double time;

//uniform int colorMode;
//uniform int postEffectMode;

uniform sampler2D tex;

layout(location=0) out vec4 colorOut;


void main()
{
  vec2 uPos = gl_FragCoord.xy / screenSize.xy;
  dvec2 pos = dvec2(gl_FragCoord.xy) / (screenSize.y*0.5)
        - dvec2(screenSize.x / screenSize.y, 1.0);

  postEffectMode = 1;

  if(postEffectMode == 1) {
    for(int i=-6; i<7; i++) {
      for(int j=-6; j<7; j++) {
        vec2 t = vec2(i/screenSize.x, j/screenSize.y);
        texVal += texture(tex, uPos+t) / 100.0;
      }
    }
  }

  if(postEffectMode == 2) {
    for(int i=-6; i<7; i++) {
      for(int j=-6; j<7; j++) {
        vec2 t = vec2(i/screenSize.x, j/screenSize.y);
        float r = texture(tex, uPos+t+vec2(0.005, 0.0)).r / 100.0;
        float b = texture(tex, uPos+t+vec2(0.0, 0.005)).b / 100.0;
        texVal += vec4(r, 0.0, b, 1.0/169.0);
      }
    }
  }

 colorOut = mix(texVal, texture(tex, uPos), 0.5);

}
