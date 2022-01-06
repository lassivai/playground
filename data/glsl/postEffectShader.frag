#version 450 core

uniform sampler2D texture1, texture2;
uniform vec2 screenSize;
uniform float time;

in vec2 texCoord;
in vec4 color;
out vec4 colorOut;


vec4 mod289(vec4 x) {
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec4 permute(vec4 x) {
  return mod289(((x*34.0)+1.0)*x);
}

vec4 taylorInvSqrt(vec4 r) {
  return 1.79284291400159 - 0.85373472095314 * r;
}

vec2 fade(vec2 t) {
  return t*t*t*(t*(t*6.0-15.0)+10.0);
}

// Classic Perlin noise
float cnoise(vec2 P) {
  vec4 Pi = floor(P.xyxy) + vec4(0.0, 0.0, 1.0, 1.0);
  vec4 Pf = fract(P.xyxy) - vec4(0.0, 0.0, 1.0, 1.0);
  Pi = mod289(Pi); // To avoid truncation effects in permutation
  vec4 ix = Pi.xzxz;
  vec4 iy = Pi.yyww;
  vec4 fx = Pf.xzxz;
  vec4 fy = Pf.yyww;

  vec4 i = permute(permute(ix) + iy);

  vec4 gx = fract(i * (1.0 / 41.0)) * 2.0 - 1.0 ;
  vec4 gy = abs(gx) - 0.5 ;
  vec4 tx = floor(gx + 0.5);
  gx = gx - tx;

  vec2 g00 = vec2(gx.x,gy.x);
  vec2 g10 = vec2(gx.y,gy.y);
  vec2 g01 = vec2(gx.z,gy.z);
  vec2 g11 = vec2(gx.w,gy.w);

  vec4 norm = taylorInvSqrt(vec4(dot(g00, g00), dot(g01, g01), dot(g10, g10), dot(g11, g11)));
  g00 *= norm.x;
  g01 *= norm.y;
  g10 *= norm.z;
  g11 *= norm.w;

  float n00 = dot(g00, vec2(fx.x, fy.x));
  float n10 = dot(g10, vec2(fx.y, fy.y));
  float n01 = dot(g01, vec2(fx.z, fy.z));
  float n11 = dot(g11, vec2(fx.w, fy.w));

  vec2 fade_xy = fade(Pf.xy);
  vec2 n_x = mix(vec2(n00, n01), vec2(n10, n11), fade_xy.x);
  float n_xy = mix(n_x.x, n_x.y, fade_xy.y);
  return 2.3 * n_xy;
}

float random(vec2 co) {
   return fract(sin(dot(co.xy,vec2(12.9898,78.233))) * 43758.5453);
}

float hash(vec2 v) {
  return fract(1e4 * sin(17.0 * v.x + v.y * 0.1) * (0.1 + abs(sin(v.y * 13.0 + v.x))));
}

/*float random2(float i) {
  return random(vec2(sin(float(i)*372.73)*5.3565, sin(float(i)*467.273)*5.54));
}*/


void main(){
  /*vec2 pix = 1.0 / screenSize;

  vec2 delta;
  delta.x = 25.0 * pix.x * cnoise(texCoord + vec2(time*0.2));
  delta.y = 25.0 * pix.y * cnoise(texCoord + vec2(134.567+time*0.2));

  delta.x += 5.0 * pix.x * cnoise(texCoord*vec2(10.0, 10000.0) + vec2(time*2.0));
  delta.y += 1.0 * pix.y * cnoise(texCoord*vec2(10.0, 10000.0) + vec2(134.567+time*2.0));*/

  vec4 color1 = texture(texture1, texCoord);
  vec4 color2 = texture(texture2, texCoord);
  
  int blendMode = 2;
  double blend = 0.9;

  if(blendMode == 1) {
    colorOut.r = color2.r - color1.r;
    colorOut.g = color2.g - color1.g;
    colorOut.b = color2.b - color1.b;
    colorOut.a = 1;
  }
  if(blendMode == 2) {
    colorOut.r = color1.r / (1-color2.r);
    colorOut.g = color1.g / (1-color2.g);
    colorOut.b = color1.b / (1-color2.b);
    colorOut.a = 1;
  }
  
  colorOut = vec4(mix(color1, colorOut, blend));
  
  //float k = random(gl_FragCoord.xy*144.5432*(1.535+sin(time))) * 0.2-0.1;
  //float k = random(vec2(cos(gl_FragCoord.y*5432)+3+sin(gl_FragCoord.x*12364*fract(time*0.000001)), gl_FragCoord.x*.54) + gl_FragCoord.xy*.05432*(.535+fract(time*0.00001)));
  
  float k = hash(gl_FragCoord.xy*(126+time*0.06));
  
  //float k = random2(gl_FragCoord.x+gl_FragCoord.y*screenSize.x);
  k = k*0.2 - 0.1;
  colorOut.r += k;
  colorOut.g += k;
  colorOut.b += k;
  /*colorOut.r = k;
  colorOut.g = k;
  colorOut.b = k;*/
  //colorOut = vec4(1, 0, 1, 1);

  /*vec4 b;
  for(int i=-8; i<9; i++) {
    for(int j=-8; j<9; j++) {
      b += texture(tex, fragTexCoord + 2.0*vec2(i, j) * pix) / (17.0*17.0);
    }
  }

  color = mix(color, b, 0.5);*/

}
