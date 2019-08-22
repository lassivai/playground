#version 450 core

#define OCTAVES 5
#define MAX_WARP_LEVELS 10


uniform vec2 size;
uniform float time;

uniform float scale;
uniform float value, frequency, amplitude, lacunarity, gain;
uniform int warpLevels;
uniform float warpFreqs[MAX_WARP_LEVELS], warpAmounts[MAX_WARP_LEVELS];

uniform float warpTimes[MAX_WARP_LEVELS];

uniform int colorMode;
//uniform int colorMode;

in vec2 texCoord;
in vec4 color;
out vec4 colorOut;

float max3(float a, float b, float c) {
  return max(a, max(b, c));
}
float max4(float a, float b, float c, float d) {
  return max(a, max(b, max(c, d)));
}

float min3(float a, float b, float c) {
  return min(a, min(b, c));
}
float min4(float a, float b, float c, float d) {
  return min(a, min(b, min(c, d)));
}

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

/*vec3 mod289(vec3 x) {
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}
vec4 mod289(vec4 x) {
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

*/



float mod289(float x){return x - floor(x * (1.0 / 289.0)) * 289.0;}
vec4 mod289(vec4 x){return x - floor(x * (1.0 / 289.0)) * 289.0;}
vec4 perm(vec4 x){return mod289(((x * 34.0) + 1.0) * x);}
vec4 permute(vec4 x) {
     return mod289(((x*34.0)+1.0)*x);
}

float noise(vec3 p){
    vec3 a = floor(p);
    vec3 d = p - a;
    d = d * d * (3.0 - 2.0 * d);

    vec4 b = a.xxyy + vec4(0.0, 1.0, 0.0, 1.0);
    vec4 k1 = perm(b.xyxy);
    vec4 k2 = perm(k1.xyxy + b.zzww);

    vec4 c = k2 + a.zzzz;
    vec4 k3 = perm(c);
    vec4 k4 = perm(c + 1.0);

    vec4 o1 = fract(k3 * (1.0 / 41.0));
    vec4 o2 = fract(k4 * (1.0 / 41.0));

    vec4 o3 = o2 * d.z + o1 * (1.0 - d.z);
    vec2 o4 = o3.yw * d.x + o3.xz * (1.0 - d.x);

    return o4.y * d.y + o4.x * (1.0 - d.y);
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

vec4 taylorInvSqrt(vec4 r)
{
  return 1.79284291400159 - 0.85373472095314 * r;
}

float snoise(vec3 v)
  {
  const vec2  C = vec2(1.0/6.0, 1.0/3.0) ;
  const vec4  D = vec4(0.0, 0.5, 1.0, 2.0);

// First corner
  vec3 i  = floor(v + dot(v, C.yyy) );
  vec3 x0 =   v - i + dot(i, C.xxx) ;

// Other corners
  vec3 g = step(x0.yzx, x0.xyz);
  vec3 l = 1.0 - g;
  vec3 i1 = min( g.xyz, l.zxy );
  vec3 i2 = max( g.xyz, l.zxy );

  //   x0 = x0 - 0.0 + 0.0 * C.xxx;
  //   x1 = x0 - i1  + 1.0 * C.xxx;
  //   x2 = x0 - i2  + 2.0 * C.xxx;
  //   x3 = x0 - 1.0 + 3.0 * C.xxx;
  vec3 x1 = x0 - i1 + C.xxx;
  vec3 x2 = x0 - i2 + C.yyy; // 2.0*C.x = 1/3 = C.y
  vec3 x3 = x0 - D.yyy;      // -1.0+3.0*C.x = -0.5 = -D.y

// Permutations
  i = mod289(i);
  vec4 p = permute( permute( permute(
             i.z + vec4(0.0, i1.z, i2.z, 1.0 ))
           + i.y + vec4(0.0, i1.y, i2.y, 1.0 ))
           + i.x + vec4(0.0, i1.x, i2.x, 1.0 ));

// Gradients: 7x7 points over a square, mapped onto an octahedron.
// The ring size 17*17 = 289 is close to a multiple of 49 (49*6 = 294)
  float n_ = 0.142857142857; // 1.0/7.0
  vec3  ns = n_ * D.wyz - D.xzx;

  vec4 j = p - 49.0 * floor(p * ns.z * ns.z);  //  mod(p,7*7)

  vec4 x_ = floor(j * ns.z);
  vec4 y_ = floor(j - 7.0 * x_ );    // mod(j,N)

  vec4 x = x_ *ns.x + ns.yyyy;
  vec4 y = y_ *ns.x + ns.yyyy;
  vec4 h = 1.0 - abs(x) - abs(y);

  vec4 b0 = vec4( x.xy, y.xy );
  vec4 b1 = vec4( x.zw, y.zw );

  //vec4 s0 = vec4(lessThan(b0,0.0))*2.0 - 1.0;
  //vec4 s1 = vec4(lessThan(b1,0.0))*2.0 - 1.0;
  vec4 s0 = floor(b0)*2.0 + 1.0;
  vec4 s1 = floor(b1)*2.0 + 1.0;
  vec4 sh = -step(h, vec4(0.0));

  vec4 a0 = b0.xzyw + s0.xzyw*sh.xxyy ;
  vec4 a1 = b1.xzyw + s1.xzyw*sh.zzww ;

  vec3 p0 = vec3(a0.xy,h.x);
  vec3 p1 = vec3(a0.zw,h.y);
  vec3 p2 = vec3(a1.xy,h.z);
  vec3 p3 = vec3(a1.zw,h.w);

//Normalise gradients
  vec4 norm = taylorInvSqrt(vec4(dot(p0,p0), dot(p1,p1), dot(p2, p2), dot(p3,p3)));
  p0 *= norm.x;
  p1 *= norm.y;
  p2 *= norm.z;
  p3 *= norm.w;

// Mix final noise value
  vec4 m = max(0.6 - vec4(dot(x0,x0), dot(x1,x1), dot(x2,x2), dot(x3,x3)), 0.0);
  m = m * m;
  return 42.0 * dot( m*m, vec4( dot(p0,x0), dot(p1,x1),
                                dot(p2,x2), dot(p3,x3) ) );
  }



float fbm(vec2 st, float t, float value = -0.50, float frequency = 1.0,
          float amplitude = 0.5, float gain = 0.8, float lacunarity = 2.5) {
  /*gain = 0.5;
  value = -0.2;
  lacunarity = 3.;
  amplitude = 0.7;
  frequency = 0.5;*/

  for(int i=0; i<OCTAVES; i++) {
    //value += amplitude * (snoise(st * frequency + t)+1.0)*0.5;
    //float k = float(mod(i, 2))*2.0-1.0;
    //value += amplitude * noise((st+t*0.2) * frequency);
    value += amplitude * noise(st * frequency + t);
    //value += amplitude * (snoise(st * frequency + t)*0.5+0.5);
    frequency *= lacunarity;
    amplitude *= gain;
  }

  return value;
}



vec2 random2(int i) {
  return vec2(random(vec2(sin(float(i)*37542.73)*35.3565, sin(float(i)*46756.273)*15.54)),
              random(vec2(sin(float(i+5485)*5554.273)*25.66, sin(float(i+4573)*94326.273)*465.21)));
}



float fbmWarp(vec2 st, int index, float freq, int levels, float[MAX_WARP_LEVELS] amounts, float[MAX_WARP_LEVELS] times) {
  float p;
  for(int i=0; i<levels; i++) {
    p = fbm(vec2(p*amounts[i]) + random2(i+index) + st*freq, times[i]);
  }
  return p;
}

vec2 fbmWarp2(vec2 st, int index, float freq, int levels, float[MAX_WARP_LEVELS] amounts, float[MAX_WARP_LEVELS] times, float value = -0.50, float frequency = 1.0, float amplitude = 0.5, float gain = 0.8, float lacunarity = 2.5) {
  vec2 p;

  for(int i=levels-1; i>=0; i--) {
    p.x = fbm(p*amounts[i] + random2(i*2+index) + st*freq, times[i], value, frequency, amplitude, gain, lacunarity);
    p.y = fbm(p*amounts[i] + random2(i*2+1+index) + st*freq, times[i], value, frequency, amplitude, gain, lacunarity);
  }
  return p;
}

vec4 drawStar(float time, vec2 pixelPos, vec2 starPos, int starIndex) {
  float r = distance(pixelPos, starPos);
  return vec4(1.0, 1.0, 1.0, (1.0-smoothstep(0.0005, 0.001, r))*noise(random2(starIndex) * time));
}

void main()
{
  vec2 pos = texCoord;
  pos.x *= size.x/size.y;
  pos = pos*2.0 - vec2(1.0, 1.0);

  vec2 screenPos = texCoord.xy;

  float g = length(pos);

  pos += 10.;

  const int numStars = 100;
  vec2 starPositions[numStars];
  for(int i=0; i<numStars; i++) {
    starPositions[i] = random2(i);
  }


  vec2 f = fbmWarp2(pos, 0, scale, warpLevels, warpAmounts, warpTimes, value, frequency, amplitude, gain, lacunarity);
  vec2 f2 = fbmWarp2(pos, 10, scale, warpLevels, warpAmounts, warpTimes, value, frequency, amplitude, gain, lacunarity);
  vec2 f3 = fbmWarp2(pos, 30, scale, warpLevels, warpAmounts, warpTimes, value, frequency, amplitude, gain, lacunarity);

  if(colorMode == 1) {
    colorOut = vec4(f.x, f2.x, f3.x, 1.);
  }
  else if(colorMode == 2) {
    colorOut = vec4(max(f.y, max(f.x, max(f2.x, f3.x))),
                    min(f.y, min(f.x, min(f3.x, f2.x)))*0.7, 0, 1.);
  }
  else if(colorMode == 3) {
    colorOut = vec4(max3(f.x, f.y, f3.x), min4(f.x, f2.x, f.y, f3.x)*0.9*noise(vec2(time*0.33)), max3(f2.x, f2.y, f3.y), 1.);
  }
  else if(colorMode == 4) {
    colorOut = vec4(
      min4(f.x, f2.x, f.y, f3.x)*0.5*noise(vec2(time*0.33)),
      min4(f2.x, f2.y, f3.x, f2.x),
      max3(f2.x, f2.y, f3.y),
      1.0);
  }
  else if(colorMode == 5) {
    vec4 bg0 = vec4(0.1, 0.1, 0.4, 1.0);
    vec4 bg1 = vec4(0.4, 0.5, 1.0, 1.0);
    vec4 bg = mix(bg0, bg1, screenPos.y);
    vec4 fg = vec4(1, 1, 1, 1);
    float t = pow(f.x, 0.4); //max(f.x, f.y);
    colorOut = mix(bg, fg, clamp((t-0.2)*2.0, 0.0, 1.0));
  }
  else if(colorMode == 6) {
    vec4 bg0 = vec4(0.06, 0.11, 0.4, 1.0);
    vec4 bg1 = vec4(0.7, 0.8, 1.0, 1.0);
    vec4 bg2 = vec4(0.25, 0.2, 0.4, 1.0);
    vec4 bg3 = mix(bg0, bg1, screenPos.y*screenPos.y);
    vec4 bg = mix(bg0, bg1, screenPos.y*screenPos.y);
    vec4 fg = vec4(1, 1, 1, 1);
    float t = pow(f.x, 0.4);
    float k = max(0, screenPos.y - 0.5)*1.5;
    colorOut = mix(bg, fg, clamp((t-0.2-k)*2.0, 0.0, 1.0));
  }
  else if(colorMode == 7) {
    float k = max(0, screenPos.y - 0.5)*1.5;
    vec4 bg0 = vec4(0.02, 0.02, 0.2, 1.0);
    vec4 bg1 = vec4(0.3, 0.3, .4, 1.0);
    vec4 bg2 = vec4(0.2, 0.05, 0.1, 1.0);
    vec4 bg3 = mix(bg0, bg1, screenPos.y*screenPos.y);
    vec4 bg = mix(bg3, bg2, screenPos.y*screenPos.y*screenPos.y*screenPos.y*screenPos.y*screenPos.y);

    for(int i=0; i<numStars; i++) {
      vec4 starCol = drawStar(time, screenPos, starPositions[i], i);
      bg = mix(bg, starCol, max(0.0, starCol.a - k));
    }

    vec4 a = vec4(0, 0, 0, 1);
    vec4 b = vec4(0.3, 0.02, 0.04, 1);
    vec4 fg = mix(a, b, screenPos.y*screenPos.y);
    float t = pow(f.x, 0.4);
    colorOut = mix(bg, fg, clamp((t-0.2-k)*2.0, 0.0, 1.0));
  }
  else {
    colorOut = vec4(f.x, f.x, f.x, 1.);
    //colorOut = vec4(f.x, f.x, f.x, 1.);
    /*colorOut = vec4(max3(f2.x, f2.y, f3.y),
    max3(f2.x, f2.y, f3.y),
    max3(f2.x, f2.y, f3.y) + min4(f.x, f2.x, f.y, f3.x)*0.5*noise(vec2(time*0.33)), 1.);*/
    /*colorOut = vec4(min4(f2.x, f2.y, f3.y, f.x),
    min4(f2.x, f2.y, f3.y, f.x),
    max3(f2.x, f2.y, f3.y) + max4(f.x, f2.x, f.y, f3.x)*0.5*noise(vec2(time*0.33)), 1.);*/

    /*vec4 bg0 = vec4(0.1, 0.14, 0.4, 1.0);
    vec4 bg1 = vec4(0.6, 0.7, 1.0, 1.0);
    vec4 bg = mix(bg0, bg1, screenPos.y*screenPos.y);
    vec4 fg = vec4(1, 1, 1, 1);
    float t = pow(f.x, 0.4);
    colorOut = mix(bg, fg, clamp((t-0.2)*2.0, 0.0, 1.0));*/

  }

  //colorOut = vec4(max(f,f2), min(f, f3), f3, 1.);
  //colorOut = vec4(max(f3,f2), f2, f, 1.);
  //

  /*if(g < 0.8) {
    //colorOut = colorOut;
  }
  else {
    float t = (g - 0.8) * 10.0;
    //if(colorOut.r > 0.5*t && colorOut.g > 0.2*t) {
    if(f.x > f2.x*t ) {
    }
    else {
      colorOut = vec4(0.0);
    }
  }*/
}
