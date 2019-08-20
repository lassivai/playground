#version 430

const float PI = 3.14159265359;
const float TAU = 2.0*3.14159265359;


/* From the time I finished this ray tracer in summer 2016 I've been eagerly waiting to transform
 * this into GPU. There are quite a lot of texture reads, and I am not sure if those would
 * cause a bottleneck for this attempt. But if not, then I am a happy owner of my own hand made
 * fast voxel tracer. Quite a lot of optimization to be done anyway.
 *
 * I am not interested currently on the more commonly seen and more efficient raymarching raytracers or
 * such, but trying to make a 3D renderer from start to finish all by myself. This also  is quite different
 * affair than utilizing normal graphics hardware procedures to render 3D scene.
 *
 * I am already very happy, because I have already done it from start to finish. This GPU implementation
 * is an extra step to get possibly better peformance.
 *
 * If this doesn't yeild the expected performance benefit, then I'll utilize this tracer probably for
 * some heavyload offline rendering and turn my attention to those already proven to be fast methods.
 */


layout (rgba32f) uniform image2DArray voxels;
layout (r32ui) uniform uimage2D minMaxVoxelValuesImage;

uniform vec2 screenSize;
uniform float aspectRatio;
uniform mat4 inverseCameraMatrix;
uniform float cameraDistance, cameraDistanceFactorX; // why the latter?
//float cameraDistance = 1.0/tan(scene3D->verticalFOV*0.5*PI/180.0);
//scene3D->camDistance

uniform vec3 gridPos;
uniform int gridW, gridH, gridD;
uniform int gridMinX, gridMinY, gridMinZ, gridMaxX, gridMaxY, gridMaxZ;

uniform float attenuationAlphaFactor;
uniform float colorAlphaFactor;
uniform float colorRayStrengthFactor;

uniform float rayAttenuation;
uniform float deltaOffset;
uniform float previousMax, previousMin;
uniform vec4 colorFactors;
uniform int shadingMode;
uniform bool dynamicLevels;
uniform bool maxIntOrAlpha;
uniform bool showBorders;
uniform float mipFactor;
uniform float xOffsetFactor;

layout(location=0) out vec4 colorOut;



float map(float t, float a, float b, float x, float y) {
  if(b == a) return 0;
  return (t-a)/(b-a)*(y-x)+x;
}


void min2(inout vec2 v, float a) {
    if(v.x > a) {
      v.y = v.x;
      v.x = a;
    }
    else if(v.y > a) {
      v.y = a;
    }
}

vec3 intersectRayAAB(vec3 rayStart, vec3 rayDir)
{
  vec3 S = rayStart -  gridPos;
  vec3 D = rayDir;
  vec3 P;
  float x1 = floor( gridMinX) - gridW*0.5;
  float x2 = ceil( gridMaxX) - gridW*0.5;
  float y1 = floor( gridMinY) - gridH*0.5;
  float y2 = ceil( gridMaxY) - gridH*0.5;
  float z1 = floor( gridMinZ) - gridD*0.5;
  float z2 = ceil( gridMaxZ) - gridD*0.5;

  int numIntersections = 0;

  vec2 t = vec2(3.4e38f, 3.4e38f);

  float t1 = (x1-S.x)/D.x;
  P = vec3(x1, S.y + t1*D.y, S.z + t1*D.z);
  if(t1 > 0 && P.y < y2 && P.y >= y1 && P.z < z2 && P.z >= z1) {
    min2(t, t1);
  }

  float t2 = (x2-S.x)/D.x;
  P = vec3(x2, S.y + t2*D.y, S.z + t2*D.z);
  if(t2 > 0 && P.y < y2 && P.y >= y1 && P.z < z2 && P.z >= z1) {
    min2(t, t2);
  }

  float t3 = (y1-S.y)/D.y;
  P = vec3(S.x + t3*D.x, y1, S.z + t3*D.z);
  if(t3 > 0 && P.x < x2 && P.x >= x1 && P.z < z2 && P.z >= z1) {
    min2(t, t3);
  }

  float t4 = (y2-S.y)/D.y;
  P = vec3(S.x + t4*D.x, y2, S.z + t4*D.z);
  if(t4 > 0 && P.x < x2 && P.x >= x1 && P.z < z2 && P.z >= z1) {
    min2(t, t4);
  }

  float t5 = (z1-S.z)/D.z;
  P = vec3(S.x + t5*D.x, S.y + t5*D.y, z1);
  if(t5 > 0 && P.x < x2 && P.x >= x1 && P.y < y2 && P.y >= y1) {
    min2(t, t5);
  }

  float t6 = (z2-S.z)/D.z;
  P = vec3(S.x + t6*D.x, S.y + t6*D.y, z2);
  if(t6 > 0 && P.x < x2 && P.x >= x1 && P.y < y2 && P.y >= y1) {
    min2(t, t6);
  }

  if(t.x < 3e38f) numIntersections++;
  if(t.y < 3e38f) numIntersections++;

  vec3 ret = vec3(numIntersections, t.x, t.y);
  return ret;
};


// DDA traversal adapted from Amanatides and Woo, 1987
vec4 traverseGrid(vec3 start, vec3 end, vec3 dir)
{
  //dir = end - start;
  //normalize(dir);

  start = start - gridPos;
  vec3 currentPos = vec3(start.x+gridW/2-0.5, start.y+gridH/2-0.5, start.z+gridD/2-0.5);

  int nextVoxelX = dir.x > 0 ? 1 : -1;
  int nextVoxelY = dir.y > 0 ? 1 : -1;
  int nextVoxelZ = dir.z > 0 ? 1 : -1;

  int currentVoxelX = int(round(clamp(currentPos.x, 0, gridW-1)));
  int currentVoxelY = int(round(clamp(currentPos.y, 0, gridH-1)));
  int currentVoxelZ = int(round(clamp(currentPos.z, 0, gridD-1)));

  int voxelsVisited = 0;

  float tMaxX = (currentVoxelX + 0.5*nextVoxelX - currentPos.x)/dir.x;
  float tMaxY = (currentVoxelY + 0.5*nextVoxelY - currentPos.y)/dir.y;
  float tMaxZ = (currentVoxelZ + 0.5*nextVoxelZ - currentPos.z)/dir.z;

  float tStepX = 1.0/dir.x*nextVoxelX;
  float tStepY = 1.0/dir.y*nextVoxelY;
  float tStepZ = 1.0/dir.z*nextVoxelZ;

  float rayStrength = 1.0;
  vec4 rayColor;
  vec4 maxColors;

  bool border = false;

  if(((currentVoxelX ==  gridMinX || currentVoxelX ==  gridMaxX-1) ? 1 : 0) +
     ((currentVoxelY ==  gridMinY || currentVoxelY ==  gridMaxY-1) ? 1 : 0) +
     ((currentVoxelZ ==  gridMinZ || currentVoxelZ ==  gridMaxZ-1) ? 1 : 0) > 1) {
    border = true;
  }

  while(true)
  {
    voxelsVisited++;

    vec4 voxelColor = vec4(imageLoad(voxels, ivec3(currentVoxelX, currentVoxelY, currentVoxelZ)).xyza);
    //vec4 voxelColor = vec4(0.6/voxelsVisited, 0.1*voxelsVisited, 0.1, 0.1);
    voxelColor.r = voxelColor.r * colorFactors.r * colorFactors.a;
    voxelColor.g = voxelColor.g * colorFactors.g * colorFactors.a;
    voxelColor.b = voxelColor.b * colorFactors.b * colorFactors.a;


    if(shadingMode == 2) {
      rayColor = voxelColor;
      break;
    }
    else if(shadingMode == 1 && maxIntOrAlpha) {
      rayColor.r = max(rayColor.r, voxelColor.r);
      rayColor.g = max(rayColor.g, voxelColor.g);
      rayColor.b = max(rayColor.b, voxelColor.b);
      rayStrength -= rayAttenuation * rayAttenuation;
    }
    else if(shadingMode == 1 && !maxIntOrAlpha) {
      if(voxelColor.a > rayColor.a) {
        rayColor = voxelColor;
      }
      rayStrength -= rayAttenuation * rayAttenuation;
    }
    else {
      if(maxIntOrAlpha) {
          maxColors.r = max(maxColors.r, voxelColor.r);
          maxColors.g = max(maxColors.g, voxelColor.g);
          maxColors.b = max(maxColors.b, voxelColor.b);
      }
      else {
          if(voxelColor.a > maxColors.a) {
              maxColors = voxelColor;
          }
      }

      float q = (voxelColor.a*colorAlphaFactor + 1.0-colorAlphaFactor) * (rayStrength*colorRayStrengthFactor + 1.0-colorRayStrengthFactor);

      rayColor.rgb += voxelColor.rgb * q;
      rayStrength -= rayAttenuation * rayAttenuation * (voxelColor.a*attenuationAlphaFactor + 1.0-attenuationAlphaFactor);
    }


    if(rayStrength < 0) break;

    if(tMaxX < tMaxY && tMaxX < tMaxZ) {
      currentVoxelX += nextVoxelX;
      if(currentVoxelX <  gridMinX || currentVoxelX >  gridMaxX-1) {
        currentVoxelX -= nextVoxelX;
        break;
      }
      tMaxX += tStepX;
    }
    else if(tMaxY < tMaxZ) {
      currentVoxelY += nextVoxelY;
      if(currentVoxelY <  gridMinY || currentVoxelY >  gridMaxY-1) {
        currentVoxelY -= nextVoxelY;
        break;
      }
      tMaxY += tStepY;
    }
    else {
      currentVoxelZ += nextVoxelZ;
      if(currentVoxelZ <  gridMinZ || currentVoxelZ >  gridMaxZ-1) {
        currentVoxelZ -= nextVoxelZ;
        break;
      }
      tMaxZ += tStepZ;
    }
  }

  if(shadingMode < 2)
  {
    if(dynamicLevels) {
      //float avgR = r/voxelsVisited, avgG = g/voxelsVisited, avgB = b/voxelsVisited;
      //r = r*(1.0-averageFactor) + avgR*averageFactor;
      //g = g*(1.0-averageFactor) + avgG*averageFactor;
      //b = b*(1.0-averageFactor) + avgB*averageFactor;
      //previousMaxTmp = max(previousMaxTmp, max(r, max(g, b)));
      //previousMinTmp = min(previousMinTmp, min(r, min(g, b)));
      imageAtomicMin(minMaxVoxelValuesImage, ivec2(0, 0), uint(10000.0*min(rayColor.r, min(rayColor.g, rayColor.b))));
      imageAtomicMax(minMaxVoxelValuesImage, ivec2(1, 0), uint(10000.0*max(rayColor.r, max(rayColor.g, rayColor.b))));

      float mmax = previousMax;
      float mmin = previousMin;

      float fl = (1.0-deltaOffset)*mmin + deltaOffset*mmax;
      if(mmax > fl) {
      //if(true) {
        rayColor.r = map(rayColor.r, fl, mmax, 0, 1.0);
        rayColor.g = map(rayColor.g, fl, mmax, 0, 1.0);
        rayColor.b = map(rayColor.b, fl, mmax, 0, 1.0);
        rayColor.r = rayColor.r*(1.0-mipFactor) + maxColors.r*mipFactor;
        rayColor.g = rayColor.g*(1.0-mipFactor) + maxColors.g*mipFactor;
        rayColor.b = rayColor.b*(1.0-mipFactor) + maxColors.b*mipFactor;
      }
      else {
        rayColor.r = rayColor.g = rayColor.b = 0;
      }
    }
    else {
      float maxValue = max(1.0, max(rayColor.r, max(rayColor.g, rayColor.b)));
      float minValue = deltaOffset*min(rayColor.r, min(rayColor.g, rayColor.b));
      if(maxValue > minValue) {
        rayColor.r = map(rayColor.r, minValue, maxValue, 0, 1.0);
        rayColor.g = map(rayColor.g, minValue, maxValue, 0, 1.0);
        rayColor.b = map(rayColor.b, minValue, maxValue, 0, 1.0);
      }
      else {
        rayColor.r = rayColor.g = rayColor.b = 0;
      }
    }
  }

  if(((currentVoxelX == gridMinX || currentVoxelX == gridMaxX-1) ? 1 : 0) +
     ((currentVoxelY == gridMinY || currentVoxelY == gridMaxY-1) ? 1 : 0) +
     ((currentVoxelZ == gridMinZ || currentVoxelZ == gridMaxZ-1) ? 1 : 0) > 1) {
    border = true;
  }

  if(showBorders && border) {
    float minValue = min(rayColor.r, min(rayColor.g, rayColor.b));
    float maxValue = max(rayColor.r, max(rayColor.g, rayColor.b));
    float lum = (minValue+maxValue+0.4)/2.0/1.5;
    rayColor.r = rayColor.r*lum + 0.15 * (1.0-lum);
    rayColor.g = rayColor.g*lum + 0.15 * (1.0-lum);
    rayColor.b = rayColor.b*lum + 0.15 * (1.0-lum);
  }

    //rayColor.b = 0.5;


  // handle alpha too
  return vec4(rayColor.r, rayColor.g, rayColor.b, 1.0);
  //return vec4(1, 1, 1, 1);
}






void main()
{
  ivec2 ipos = ivec2(gl_FragCoord.xy);

  vec4 rayDir = vec4(map(ipos.x + screenSize.x/cameraDistanceFactorX*cameraDistance, 0, screenSize.x-1, -aspectRatio, aspectRatio), map(ipos.y, 0, screenSize.y-1, -1.0, 1.0), -cameraDistance, 0.0);
  normalize(rayDir);

  vec4 rayStart = vec4(0, 0, 0, 1);
  rayStart = inverseCameraMatrix * rayStart;
  rayDir = inverseCameraMatrix * rayDir;

  vec4 rayColor = vec4(0, 0, 0, 0);

  vec3 ret = intersectRayAAB(rayStart.xyz, rayDir.xyz);
    //, gridPos, gridW, gridH, gridD, gridMinX, gridMinY, grirMindZ, gridMaxX, gridMaxY, gridMaxZ);

  if(round(ret.x) > 0) {
    vec3 start, end;
    vec3 o = rayStart.xyz, d = rayDir.xyz;

    if(round(ret.x) == 1) {
      start = rayStart.xyz;
      end = vec3(o.x + d.x*ret.y, o.y + d.y*ret.y, o.z + d.z*ret.y);
    }
    else  {
      start = vec3(o.x + d.x*ret.y, o.y + d.y*ret.y, o.z + d.z*ret.y);
      end = vec3(o.x + d.x*ret.z, o.y + d.y*ret.z, o.z + d.z*ret.z);
    }

    rayColor = traverseGrid(start, end, rayDir.xyz);
    //gridW, gridH, gridD, rayAttenuation, deltaOffset, previousMax, previousMin, colorFactors, shadingMode, dynamicLevels);
  }

  colorOut = rayColor;
}
