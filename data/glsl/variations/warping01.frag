#version 450 core

float random (in vec2 st) {
    return fract(sin(dot(st.xy,
                         vec2(12.9898,78.233)))
                * 43758.5453123);
}

// Value noise by Inigo Quilez - iq/2013
// https://www.shadertoy.com/view/lsf3WH
float noise(vec2 st) {
    vec2 i = floor(st);
    vec2 f = fract(st);
    vec2 u = f*f*(3.0-2.0*f);
    return mix( mix( random( i + vec2(0.0,0.0) ),
                     random( i + vec2(1.0,0.0) ), u.x),
                mix( random( i + vec2(0.0,1.0) ),
                     random( i + vec2(1.0,1.0) ), u.x), u.y);
}





#define OCTAVES 5


float fbm(vec2 st, float t, float value = -0.50, float frequency = 1.0,
          float amplitude = 0.5, float gain = 0.8, float lacunarity = 2.5) {
  gain = 0.5;
  value = -0.1;
  lacunarity = 3.;
  amplitude = 0.7;
  frequency = 2.0;

  for(int i=0; i<OCTAVES; i++) {

    value += amplitude * noise(st * frequency + t);
    frequency *= lacunarity;
    amplitude *= gain;
  }

  return value;
}


uniform vec2 size;
uniform float time;

/*uniform sampler2D tex, tex2;

uniform vec2 pos;

uniform vec3 lightPos;

uniform vec4 lightColor;
uniform float brightness;
uniform float decay;
uniform int lightOn;*/

in vec2 texCoord;
in vec4 color;
out vec4 colorOut;


void main()
{
 vec2 pos = texCoord;
 pos.x *= size.x/size.y;
 pos = pos*2.0 - vec2(1.0, 1.0);
 pos += 10.;
 float r = length(pos);
 //float f = fract(r * 10.0);

 //float f = noise(pos*10.0);
 //float f = snoise(pos*10.0)*0.5+0.5;
 //float f = fbm(pos*10.0, time*0.2);
 float freq = 3.0;

 float t = fbm((pos+vec2(134.23, -45.54))*freq, time*0.2);
 float h = fbm(t +(pos+vec2(1344.23, -945.54))*freq, time*0.5);
 float g = fbm(t+(pos+vec2(2134.23, -545.54))*freq, time*0.8);

 float f = fbm(vec2(g,h) + pos*freq, time*0.2);
 float f2 = fbm(vec2(g,h) + vec2(32.43, -424.53) + pos*freq, time*0.2);
 float f3 = fbm(vec2(g,h) + vec2(5352.43, 4424.53) + pos*freq, time*0.2);

 /*float levels = 4;
 f = float(int(f*levels)/levels);
 f2 = float(int(f2*levels)/levels);
 f3 = float(int(f3*levels)/levels);*/
 //f = smoothstep(0.5, 0.52, f);
  /*if(lightOn != 0) {
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
  }*/
  //colorOut = vec4(diffuse, diffuse, diffuse, 1.0);

  colorOut = vec4(max(f,f2), min(f, f3), f3, 1.);
}
