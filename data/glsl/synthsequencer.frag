#version 400

out vec4 fragColor;

const float PI = 3.14159265359;
const float TAU = 2.0*3.14159265359;

const int MAX_NOTES = 96;

uniform vec2 screenSize;
uniform vec2 mousePos;
uniform double time;
uniform float notes[MAX_NOTES];
uniform float noteVolumes[MAX_NOTES];
uniform float noteStartTimes[MAX_NOTES];
uniform int isKeyHoldings[MAX_NOTES];
uniform int numNotes;
uniform int latestNoteIndex;
uniform int keyBaseNote;
uniform int showMeasures;
uniform int screenKeyboardMinNote, screenKeyboardMaxNote;
uniform int isBackgroundGraphVisible;

uniform int measuresPerLooperTrack, subdivisionsPerMeasure;

uniform vec2 timeWindow;
uniform float trackLenght;

const float SIGMA = pow(2.0, 1.0/12.0);

uniform bool useGlobalOverlayTexture = true;

float map(float x, float a, float b, float c, float d) {
  return (x-a) / (b-a) * (d-c) + c;
}


void main() {
  vec2 pos = vec2(gl_FragCoord.xy) / (screenSize.y*0.5)
        - vec2(screenSize.x / screenSize.y, 1.0);



  int numNotes = screenKeyboardMaxNote - screenKeyboardMinNote;
  float ff = floor(gl_FragCoord.y/screenSize.y*numNotes);
  float mf1 = floor((screenSize.y-mousePos.y)/screenSize.y*numNotes);
  float t = mod(ff-keyBaseNote+screenKeyboardMinNote, 12.0)/11.0;

  vec4 mc = vec4(0.5+t*0.5);
  
  if(useGlobalOverlayTexture) {
    mc = vec4(0.1+t*0.6);
  }
  /*if(ff >= mf1 && ff < mf1+1.0) {
    mc = vec4(0.25);
  }*/

  for(int n=0; n<numNotes; n++) {
    float dt = float(time) - noteStartTimes[n];
    if(dt >= 0.0) {
      float mf2 = (notes[n]-screenKeyboardMinNote);
      if(ff >= mf2 && ff < mf2+1.0) {
        if(isKeyHoldings[n] != 0) {
          mc = vec4(0.2+0.7*(1.0-t), 0.0, 0.2+0.7*t, 1.0);
        }
        else {
          mc = mix(mc, vec4(0.2+0.7*(1.0-t), 0.0, 0.2+0.7*t, 1.0), exp(-4.0*dt));
        }
      }
    }
  }

  // TODO deal with variable beat amount, and divisions
  if(showMeasures == 1) {
    
    float x = map(gl_FragCoord.x, 0, screenSize.x-1, timeWindow.x, timeWindow.y);
    
    int i = int(x/trackLenght * measuresPerLooperTrack*subdivisionsPerMeasure);
    
    
    vec4 measuCol = vec4(0.4, 0.4, 0.4, 1.0);
    if(useGlobalOverlayTexture) {
      measuCol = vec4(0.15, 0.15, 0.15, 1.0);
    }
    //vec4 measuCol = vec4(0.8, 0.8, 0.8, 1.0);
    float measu = 0.5;
    
    if(mod(i, 2) == 1) {
      if(useGlobalOverlayTexture) {
        measu *= 0.65;
      }
      else {
        measu *= 0.88;
      }
    }
    //if(mod(i/4, 2) == 1) {
    if(mod(i/subdivisionsPerMeasure, 2) == 1) {
      if(useGlobalOverlayTexture) {
        measuCol = vec4(0.06, 0.06, 0.06, 1.0);
      }
      else {
        measuCol = vec4(0.1, 0.1, 0.1, 1.0);
      }
      //measuCol = vec4(0.6, 0.6, 0.6, 1.0);
    }
    mc = mix(mc, measuCol, measu);
  }
  else {
    if(useGlobalOverlayTexture) {
      mc = mc*0.55 + 0.1;
    }
    else {
      mc = mc*0.65 + 0.25;
    }
  }

  if(ff >= mf1 && ff < mf1+1.0) {
    mc = vec4(0.3);
  }
  //fragColor = mix(mix(synthVis, synthVis2, 0.5),  mc, 0.45);
  fragColor = mc;//mix(synthVis,  mc, 0.65);
  fragColor.a = 1;
  

  /*float f = gl_FragCoord.y/screenSize.y*72.0;
  float ff = floor(f);
  float q = f-ff;
  float mf1 = round((screenSize.y-mousePos.y)/screenSize.y*72.0);
  float t = mod(ff-keyBaseNote, 12.0)/11.0;
  vec4 mc = vec4(vec3(step(0.9, q)*0.1), 1.0);
  if(ff >= mf1 && ff < mf1+1.0) {
    mc = max(mc, vec4(0.2+0.5*(1.0-t), 0.0, 0.2+0.7*t, 1.0));
  }


  fragColor = mc;*/
}
