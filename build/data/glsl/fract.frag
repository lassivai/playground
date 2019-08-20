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

uniform double time;

uniform dvec2 location;
uniform double scale;
uniform double maxIter;
uniform dvec2 juliaPoint;
uniform int mandelbrotMode;
uniform int mappingMode;
uniform int colorMode;
uniform int postEffectMode;


const int MAX_NOTES = 96;
uniform float notes[MAX_NOTES];
uniform float noteVolumes[MAX_NOTES];
uniform float noteStartTimes[MAX_NOTES];
uniform int numNotes;
uniform int latestNoteIndex;
//uniform int keyBaseNote;
//uniform int showMeasures;

uniform sampler2D tex;

layout(location=0) out vec4 colorOut;



const float SIGMA = pow(2.0, 1.0/12.0);
float noteToFreq(float note) {
  return pow(SIGMA, note-69.0) * 440.0;
}
float synth(float d) {
  float ret = 0.0;
  for(int n=0; n<numNotes; n++)
  {
    float dt = d - noteStartTimes[n];
    if(dt >= 0.0 && dt < 10.0) {
      float t1 = 0.05;
      float t2 = 0.4;
      float a = smoothstep(0.0f, t1, dt) * (1.0-step(t2, dt));
      a += step(t2, dt) * exp(-3.0*(dt-t2));;

      float dn = dt * noteToFreq(notes[n]);

      ret += 0.2*a*noteVolumes[n]*sin(dn*TAU);
      //ret += 0.2*an*(2.0*fract(dn)-1.0);
    }
  }
  return ret;
}

void main()
{
  vec2 uPos = gl_FragCoord.xy / screenSize.xy;
  dvec2 pos = dvec2(gl_FragCoord.xy) / (screenSize.y*0.5)
        - dvec2(screenSize.x / screenSize.y, 1.0);

  pos *= scale * dvec2(1.0, -1.0);
  pos += location;


  dvec2 c = pos;
  dvec2 w = dvec2(pos), tmpW = dvec2(0, 0);

  if(mandelbrotMode == 1) {
    w = dvec2(0,0);
  }
  if(juliaPoint.x != 0.0 || juliaPoint.y != 0) {
    c = juliaPoint;
  }
  if(mod(mandelbrotMode, 2) == 1) {
    double m = 1.0/(c.x*c.x + c.y*c.y);
    c = dvec2(c.x*m, -c.y*m);
  }


  int i = 0;

  if(mandelbrotMode < 2) {
    while(w.x*w.x + w.y*w.y < 4.0 && i < int(maxIter)) {
      tmpW.x = w.x*w.x - w.y*w.y + c.x;
      tmpW.y = 2.0 * w.x * w.y + c.y;
      w.x = tmpW.x;
      w.y = tmpW.y;
      i++;
    }
  }
  if(mandelbrotMode > 1) {
    while(w.x*w.x + w.y*w.y < 40.0 && i < int(maxIter)) {
      tmpW.x = w.x*w.x - w.y*w.y + c.x;
      tmpW.y = 2.0 * w.x * w.y + c.y;
      double m = 1.0/(tmpW.x*tmpW.x + tmpW.y*tmpW.y);
      w.x = tmpW.x * m;
      w.y = -tmpW.y * m;
      i++;
    }
  }

  double d = double(i)/maxIter;
  float f = float(d);

  if(mappingMode == 1) {
    //d *= 0.5*0.5*0.5*0.5;
    float s = synth(float(time+d));
    if(colorMode == 0 || colorMode == 2) {
      f = abs(s);
    }
    else {
      f = 0.5 + 0.5*s;
    }
  }

  vec4 texVal = vec4(0);

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

  if(colorMode == 0) {
    colorOut = vec4(f, f, f, 1.0);
  }
  if(colorMode == 1) {
    colorOut = vec4(f, f, f, 1.0);
  }
  if(colorMode == 2) {
    colorOut = vec4(0.2+0.8*f, 0, 0.1-0.2*f, 1.0);
  }
  if(colorMode == 3) {
    colorOut = vec4(f, 0, 1.0-f, 1.0);
  }
  if(postEffectMode > 0) {
    colorOut = mix(colorOut, texVal, 0.5);
  }
}
