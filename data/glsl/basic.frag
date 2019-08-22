//varying vec3 normal;
//varying vec3 toLight;
//varying vec2 texCoord;

//vec3 normal1 = normalize(normal);
//vec3 toLight1 = normalize(toLight);
//float diff = clamp(dot(normal1, toLight1), 0.0, 1.0);
//gl_FragColor = texture2d(texture, texCoord);

#version 400

const float PI = 3.14159265359;
const float TAU = 2.0*3.14159265359;

uniform vec2 screenSize;
uniform vec2 mousePos;
uniform float time;

out vec4 colorOut;



void main()
{
  vec2 pos = vec2(gl_FragCoord.xy) / (screenSize.y*0.5)
        - vec2(screenSize.x / screenSize.y, 1.0);

  float d = distance(vec2(0.0, 0.0), pos);

  colorOut = vec4(0.2 + 0.4*d, 0.0, 0.6-0.2*d, 1.0);
}
