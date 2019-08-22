
#version 400

const float PI = 3.14159265359;

uniform vec2 screenSize;
uniform vec2 mousePos;
uniform double time;
uniform vec4 col;

uniform sampler2D texture;


varying vec2 texCoord;


layout(location=0) out vec4 colorOut;


void main()
{
  /*vec2 uPos = gl_FragCoord.xy / screenSize.xy;
  dvec2 pos = dvec2(gl_FragCoord.xy) / (screenSize.y*0.5)
        - dvec2(screenSize.x / screenSize.y, 1.0);*/


  //fragColor = texture2D(texture, texCoord);
  colorOut = vec4(texCoord.x, 0, texCoord.y, 1.0) * col;
}
