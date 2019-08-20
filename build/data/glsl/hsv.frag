

#version 400

const float PI = 3.14159265359;
const float TAU = 2.0*3.14159265359;

uniform vec2 screenSize;
uniform vec2 mousePos;
uniform float time;

out vec4 fragColor;


vec3 hsvToRgb(float h, float s, float v) {
  h = mod(h, TAU);
  while(h < 0) h += TAU;
  //while(h > TAU) h += TAU;

  s = clamp(s, 0.0, 1.0);
  v = clamp(v, 0.0, 1.0);

  float k = h/TAU*6.0;
  int i = int(k);

  float ff = k - i;
  float p = v * (1.0 - s);
  float q = v * (1.0 - (s * ff));
  float t = v * (1.0 - (s * (1.0 - ff)));

  vec3 col;
  if(i == 0) {
    col = vec3(v, t, p);
  }
  else if(i == 1) {
    col = vec3(q, v, p);
  }
  else if(i == 2) {
    col = vec3(p, v, t);
  }
  else if(i == 3) {
    col = vec3(p, q, v);
  }
  else if(i == 4) {
    col = vec3(t, p, v);
  }
  else {
    col = vec3(v, p, q);
  }
  return col;

}

vec3 hsvToRgb2(float h, float s, float v) {
  h = mod(h, TAU);
  while(h < 0) h += TAU;
  //while(h > TAU) h += TAU;

  s = clamp(s, 0.0, 1.0);
  v = clamp(v, 0.0, 1.0);

  float k = h/TAU*6.0;

  float c = (1.0 - abs(2.0*v - 1.0)) * s;
  float x = c * (1.0 - abs(mod(k, 2.0) - 1.0));
  float m = v - c*0.5;

  int i = int(k);

  vec3 col;
  if(i == 0) {
    col = vec3(c, x, 0.0);
  }
  else if(i == 1) {
    col = vec3(x, c, 0.0);
  }
  else if(i == 2) {
    col = vec3(0.0, c, x);
  }
  else if(i == 3) {
    col = vec3(0.0, x, c);
  }
  else if(i == 4) {
    col = vec3(x, 0.0, c);
  }
  else {
    col = vec3(c, 0.0, x);
  }
  return vec3(col.r+m, col.g+m, col.b+m);
}

float hue2rgb(float p, float q, float t) {
  if(t < 0.0) t += 1.0;
  if(t > 1.0) t -= 1.0;
  if(t < 1.0/6.0) return p + (q - p) * 6.0 * t;
  if(t < 1.0/2.0) return q;
  if(t < 2.0/3.0) return p + (q - p) * (2.0/3.0 - t) * 6.0;
  return p;
}

vec3 hsvToRgb3(float h, float s, float v) {
  vec3 col;
  h = fract(h / TAU);

  if(s == 0) {
    col = vec3(v, v, v);
  }
  else {
    float q = v < 0.5 ? v * (1.0 + s) : v + s - v * s;
    float p = 2.0 * v - q;

    col.r = hue2rgb(p, q, h + 1.0/3.0);
    col.g = hue2rgb(p, q, h);
    col.b = hue2rgb(p, q, h - 1.0/3.0);
  }

  return col;
}


void main()
{
  float aspectRatio = screenSize.x / screenSize.y;
  vec2 pos = vec2(gl_FragCoord.xy) / (screenSize.y*0.5)
        - vec2(screenSize.x / screenSize.y, 1.0);

  float d = distance(vec2(0.0, 0.0), pos);

  float mx = mousePos.x / screenSize.x;
  float my = mousePos.y / screenSize.y;

  float h = atan(pos.y, pos.x) + PI;
  h = h*(my*0.95+0.05) + mx*TAU;
  //float t = 1.0-step(1.0, d);
  float v = 1.0-d;
  //float s = mousePos.y / screenSize.y;
  float s = 0.6;

  //float h = pos.x / aspectRatio * PI + PI;
  //float v = pos.y/2.0+0.5;

  int t = int(time);

  vec3 col;

  col = hsvToRgb2(h, s, v);
  
  /*if(mod(t, 3) == 0) {
    float r = distance(pos, vec2(-0.5, 0.5));
    float p = step(0.01, r);
    col = p*hsvToRgb(h, s, v);
  }
  else if(mod(t, 3) == 1) {
    float r = distance(pos, vec2(0.0, 0.5));
    float p = step(0.01, r);
    col = p*hsvToRgb2(h, s, v);
  }
  else {
    float r = distance(pos, vec2(0.5, 0.5));
    float p = step(0.01, r);
    col = p * hsvToRgb3(h, s, v);
  }*/

  fragColor = vec4(col, 1.0);
}
