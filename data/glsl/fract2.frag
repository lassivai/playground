//varying vec3 normal;
//varying vec3 toLight;
//varying vec2 texCoord;

//vec3 normal1 = normalize(normal);
//vec3 toLight1 = normalize(toLight);
//float diff = clamp(dot(normal1, toLight1), 0.0, 1.0);
//gl_FragColor = texture2d(texture, texCoord);

#version 430

const float PI = 3.14159265359;
const float TAU = 2.0*3.14159265359;

uniform vec2 screenSize;
//uniform vec2 mousePos;

uniform float time;

uniform int pass;

uniform dvec2 location;
uniform double scale;
uniform double maxIter;
uniform dvec2 juliaPoint;
uniform int mandelbrotMode;
uniform int mappingMode;
uniform int colorMode;
uniform int postEffectMode;

uniform float spectrum[1000];
uniform int spectrumResolution;

uniform int multisamplingLevel;
uniform float multisamplingRange;
uniform float multisamplingRangePower;

uniform float previewIterationFactor;
uniform int previewActive;

uniform sampler2D tex, iterationTexture;

layout (binding = 0, r32f) uniform image2D iterationImage;
layout (binding = 0, r32ui) uniform uimage2D iterationMinMaxImage;
layout (binding = 0, r32ui) uniform uimage2D multisamplingRestrictionImage;

layout(location=0) out vec4 colorOut;


float random(in vec2 st) {
  return fract(sin(dot(st.xy, vec2(12.9898,78.233))) * 43758.5453123);
}
float random(in vec2 st, in float a, in float b) {
  return fract(sin(dot(st.xy, vec2(12.9898,78.233))) * 43758.5453123)*(b-a)+a;
}
double randomd(in vec2 st, in float a, in float b) {
  return double(fract(sin(dot(st.xy, vec2(12.9898,78.233))) * 43758.5453123)*(b-a)+a);
}

/*float noise(vec2 st) {
    vec2 i = floor(st);
    vec2 f = fract(st);
    vec2 u = f*f*(3.0-2.0*f);
    return mix( mix( random( i + vec2(0.0,0.0) ),
                     random( i + vec2(1.0,0.0) ), u.x),
                mix( random( i + vec2(0.0,1.0) ),
                     random( i + vec2(1.0,1.0) ), u.x), u.y);
}*/
void main()
{
  ivec2 ipos = ivec2(gl_FragCoord);
  vec2 uPos = gl_FragCoord.xy / screenSize.xy;

  if(previewActive == 1) {
    if(ipos.x % 4 != 0 && ipos.y % 4 != 0) discard;
  }

  float iterations = 0;
  double maxIterations = max(maxIter * (1.0-previewActive + previewIterationFactor * previewActive), 1.0);

  uint mark = 0;
  if(pass > 1) {
    mark = uint(imageLoad(multisamplingRestrictionImage, ipos));
    /*if(mark == 0) {
      colorOut = vec4(0, 0, 0, 1);
    }
    else {
      colorOut = vec4(1, 1, 1, 1);
    }*/
  }
  if(previewActive == 1 || (pass <= multisamplingLevel+1 && !(pass > 1 && mark == 1))) {
  //if(pass <= multisamplingLevel+1) {

    dvec2 d = dvec2(0, 0);
    /*if(pass > 1) {
      float w = multisamplingRange;
      d = dvec2(randomd(gl_FragCoord.xy*time, -w, w), randomd(gl_FragCoord.yx*time, -w, w));
    }*/

    if(previewActive != 1 && pass > 1) {
      float b = 1.0 - float(imageLoad(iterationImage, ipos)) / float(maxIter) * 0.8;
      //b = b * b;
      float r = pow(random(gl_FragCoord.xy*sin(float(time)*36.44365)*243.546, 0.0, 1.0), multisamplingRangePower) * multisamplingRange;
      float a = random(sin(time*5465.456)*(gl_FragCoord.yx*pass), -PI, PI);
      d = dvec2(cos(a)*r*b, sin(a)*r*b);
    }

    dvec2 pos = dvec2(d+gl_FragCoord.xy) / (screenSize.y*0.5) - dvec2(screenSize.x / screenSize.y, 1.0);
    //dvec2 pos = dvec2(dvec2(gl_FragCoord)) / (screenSize.y*0.5) - dvec2(screenSize.x / screenSize.y, 1.0);

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

    //float q = (noise(ipos*spectrum[250]*0.02)+1.0)*0.5;
    //q = q * q * q;
    //int bailout = int(q * maxIter);



    if(mandelbrotMode < 2) {
      while(w.x*w.x + w.y*w.y < 4.0 && iterations < maxIterations) {
        tmpW.x = w.x*w.x - w.y*w.y + c.x;
        tmpW.y = 2.0 * w.x * w.y + c.y;
        w.x = tmpW.x;
        w.y = tmpW.y;
        iterations++;
      }
    }
    if(mandelbrotMode > 1) {
      while(w.x*w.x + w.y*w.y < 40.0 && iterations < maxIterations) {
        tmpW.x = w.x*w.x - w.y*w.y + c.x;
        tmpW.y = 2.0 * w.x * w.y + c.y;
        double m = 1.0/(tmpW.x*tmpW.x + tmpW.y*tmpW.y);
        w.x = tmpW.x * m;
        w.y = -tmpW.y * m;
        iterations++;
      }
    }

    //colorOut = vec4(f, f, f, 1);

    if(previewActive != 1) {
      imageAtomicMin(iterationMinMaxImage, ivec2(0, 0), uint(iterations));
      imageAtomicMax(iterationMinMaxImage, ivec2(1, 0), uint(iterations));

      if(pass == 1) {
        //imageStore(iterationImage, ivec2(gl_FragCoord), uvec4(i));
        imageStore(iterationImage, ivec2(gl_FragCoord), vec4(iterations));
      }
      else {
        /*uint iters = uint(imageLoad(iterationImage, ivec2(gl_FragCoord)));
        imageStore(iterationImage, ivec2(gl_FragCoord), uvec4(i + iters));*/
        float iterationMean = float(imageLoad(iterationImage, ivec2(gl_FragCoord)));
        iterationMean = (iterations + iterationMean*(pass-1.0)) / float(pass);
        imageStore(iterationImage, ivec2(gl_FragCoord), vec4(iterationMean));
      }
    }
    else {
      imageStore(iterationImage, ivec2(gl_FragCoord), vec4(iterations));
    }
  }

  if(previewActive != 1 && pass > 1) {
    iterations = float(imageLoad(iterationImage, ivec2(gl_FragCoord)));
    //f = float(imageLoad(iterationImage, ivec2(gl_FragCoord)));
    //f = texture(iterationTexture, uPos).r;
  }

  float f = float(iterations/maxIterations);

  if(previewActive == 1) {
    if(iterations < maxIterations) {
      colorOut = vec4(0, 0, 0, 1);
    }
    else {
      colorOut = vec4(1, 1, 1, 1);
    }
  }
  else {
    if(pass > 1) {
      float minValue = float(imageLoad(iterationMinMaxImage, ivec2(0, 0))) / float(maxIterations);
      float maxValue = float(imageLoad(iterationMinMaxImage, ivec2(1, 0))) / float(maxIterations);
      f = (f - minValue) / (maxValue - minValue);
    }

    if(mappingMode == 1) {
      //d *= 0.5*0.5*0.5*0.5;
      float k = 1.0-f;
      float s = spectrum[int(clamp(spectrumResolution*k, 0, spectrumResolution-1))] * 0.4;
      s += spectrum[int(clamp(spectrumResolution*k+1, 0, spectrumResolution-1))] * 0.2;
      s += spectrum[int(clamp(spectrumResolution*k+2, 0, spectrumResolution-1))] * 0.1;
      s += spectrum[int(clamp(spectrumResolution*k-1, 0, spectrumResolution-1))] * 0.2;
      s += spectrum[int(clamp(spectrumResolution*k-2, 0, spectrumResolution-1))] * 0.1;
      if(colorMode == 0 || colorMode == 2) {
        f = max(0.0, s);
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
          float r = texture(tex, uPos+t+vec2(0.005, 0.0)).r / 105.0;
          float b = texture(tex, uPos+t+vec2(0.0, 0.005)).b / 105.0;
          float g = b * 0.3;
          texVal += vec4(r, g, b, 1.0/169.0);
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
}
