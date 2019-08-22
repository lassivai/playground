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




vec3 mod289(vec3 x) {
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec2 mod289(vec2 x) {
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec3 permute(vec3 x) {
  return mod289(((x*34.0)+1.0)*x);
}

float snoise(vec2 v)
  {
  const vec4 C = vec4(0.211324865405187,  // (3.0-sqrt(3.0))/6.0
                      0.366025403784439,  // 0.5*(sqrt(3.0)-1.0)
                     -0.577350269189626,  // -1.0 + 2.0 * C.x
                      0.024390243902439); // 1.0 / 41.0
// First corner
  vec2 i  = floor(v + dot(v, C.yy) );
  vec2 x0 = v -   i + dot(i, C.xx);

// Other corners
  vec2 i1;
  //i1.x = step( x0.y, x0.x ); // x0.x > x0.y ? 1.0 : 0.0
  //i1.y = 1.0 - i1.x;
  i1 = (x0.x > x0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
  // x0 = x0 - 0.0 + 0.0 * C.xx ;
  // x1 = x0 - i1 + 1.0 * C.xx ;
  // x2 = x0 - 1.0 + 2.0 * C.xx ;
  vec4 x12 = x0.xyxy + C.xxzz;
  x12.xy -= i1;

// Permutations
  i = mod289(i); // Avoid truncation effects in permutation
  vec3 p = permute( permute( i.y + vec3(0.0, i1.y, 1.0 ))
		+ i.x + vec3(0.0, i1.x, 1.0 ));

  vec3 m = max(0.5 - vec3(dot(x0,x0), dot(x12.xy,x12.xy), dot(x12.zw,x12.zw)), 0.0);
  m = m*m ;
  m = m*m ;

// Gradients: 41 points uniformly over a line, mapped onto a diamond.
// The ring size 17*17 = 289 is close to a multiple of 41 (41*7 = 287)

  vec3 x = 2.0 * fract(p * C.www) - 1.0;
  vec3 h = abs(x) - 0.5;
  vec3 ox = floor(x + 0.5);
  vec3 a0 = x - ox;

// Normalise gradients implicitly by scaling m
// Approximation of: m *= inversesqrt( a0*a0 + h*h );
  m *= 1.79284291400159 - 0.85373472095314 * ( a0*a0 + h*h );

// Compute final noise value at P
  vec3 g;
  g.x  = a0.x  * x0.x  + h.x  * x0.y;
  g.yz = a0.yz * x12.xz + h.yz * x12.yw;
  return 130.0 * dot(m, g);
}




#define OCTAVES 5

/*float fbm(vec2 st, float t) {
  float frequency = 1.0;
  float value = 0.0;
  float amplitude = 0.5;
  float gain = 0.5;
  float lacunarity = 4.0;

  for(int i=0; i<OCTAVES; i++) {
    //value += amplitude * (snoise(st * frequency + t)+1.0)*0.5;
    //float k = float(mod(i, 2))*2.0-1.0;
    value += amplitude * noise(st * frequency + t);
    frequency *= lacunarity;
    amplitude *= gain;
  }

  return value;
}*/

float fbm(vec2 st, float t, float value = -0.50, float frequency = 1.0,
          float amplitude = 0.5, float gain = 0.8, float lacunarity = 2.5) {
  gain = 0.5;
  value = -0.1;
  lacunarity = 3.3;
  amplitude = 0.5;
  frequency = 0.5;

  for(int i=0; i<OCTAVES; i++) {
    //value += amplitude * (snoise(st * frequency + t)+1.0)*0.5;
    //float k = float(mod(i, 2))*2.0-1.0;
    //value += amplitude * noise((st+t*0.2) * frequency);
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
 //float f = fract(r * 10.0);

 //float f = noise(pos*10.0);
 //float f = snoise(pos*10.0)*0.5+0.5;
 //float f = fbm(pos*10.0, time*0.2);
 float freq = 1.;


 float p = fbm((pos+vec2(-34.23, 425.54))*freq, time*0.05);

  float q = fbm(p + (pos+vec2(4134.23, -45.54))*freq, time*0.1);

 float t = fbm(q + (pos+vec2(134.23, -45.54))*freq, time*0.2);
 float h = fbm(t +(pos+vec2(1344.23, -945.54))*freq, time*0.4);
 float g = fbm(t+(pos+vec2(2134.23, -545.54))*freq, time*0.8);

 float f = fbm(vec2(g,h)*1. + pos*freq, time*1.4);
 float f2 = fbm(vec2(g,h) + vec2(32.43, -424.53) + pos*freq, time*1.4);
 float f3 = fbm(vec2(g,h) + vec2(5352.43, 4424.53) + pos*freq, time*1.4);

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

  //colorOut = vec4(max(f,f2), min(f, f3), f3, 1.);
  colorOut = vec4(max(f,f2), f*0.85, f3, 1.);
  //colorOut = vec4(f, f, f, 1.);
}
