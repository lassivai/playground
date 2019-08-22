#version 450 core

uniform sampler2D tex, tex2;

uniform vec2 pos;
uniform vec2 size;
uniform vec3 lightPos;

uniform vec4 lightColor;
uniform float brightness;
uniform float decay;
uniform int lightOn;

in vec2 texCoord;
in vec4 color;
out vec4 colorOut;


void main(){

  if(lightOn != 0) {
    vec3 normal = texture(tex2, texCoord).xyz;
    normal.x = -normal.x*2.0 + 1.0;
    normal.y = -normal.y*2.0 + 1.0;

    vec2 point = pos + (texCoord - vec2(0.5, 0.5)) * size;
    vec3 toLight = lightPos - vec3(point.xy, 0.0);

    float diffuse = max(dot(normal, normalize(toLight)), 0.0);

    float d = max(1.0, length(toLight)*decay);
    diffuse = brightness * diffuse / (d*d);

    colorOut = texture(tex, texCoord) * vec4(diffuse, diffuse, diffuse, 1.0) * lightColor;
  }
  else {
    colorOut = texture(tex, texCoord);
  }
  //colorOut = vec4(diffuse, diffuse, diffuse, 1.0);


}
