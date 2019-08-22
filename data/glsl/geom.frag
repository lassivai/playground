#version 450 core

uniform sampler2D tex, tex2;

uniform int textureType = 0;
uniform int geomType = 0, geomSubType = 0;
uniform vec4 fillColor = vec4(1.0, 1.0, 1.0, 1.0);
uniform vec4 strokeColor = vec4(0.0, 0.0, 0.0, 1.0);
uniform float strokeWidth = 2;
uniform vec2 screenSize, objectSize;
//uniform vec3 triangleA, triangleB, triangleC;
uniform int strokeType = 2;
uniform float innerRadius = 0.7;

uniform float scale = 1.0;

in vec2 texCoord;
in vec4 color;
in vec4 vertex;
out vec4 colorOut;

// TODO shader stroke for triangles


void main() {

  if(geomType > 0) {
    vec2 sizeXY = objectSize.xy * scale;
    float size = max(sizeXY.x, sizeXY.y);
    float pix = 1.0 / size, pixX = 1.0 / sizeXY.x, pixY = 1.0 / sizeXY.y;
    //float innerRadius = 0.7;
    //pix = pix / scale;
    //pixX = pixX / scale;
    //pixY = pixY / scale;
    vec4 faceColor = vec4(1, 1, 1, 1);

    if(textureType == 1) {
      faceColor = texture(tex, texCoord);
    }

    if(textureType == 2) {
    }

    float w2 = max(1.0, strokeWidth*scale), w = max(0.5, strokeWidth*scale*0.5);
    float u = min(w2*0.99, 4);
    vec2 cpos = texCoord * 2.0 - vec2(1.0, 1.0);
    float r = length(cpos);

    if(geomType == 1) {
      float q0 = 1.0 - w2*pix - pix*u * 2.0;
      float q1 = 1.0 - w2*pix - pix*u;
      float q2 = 1.0 - pix*u;
      float t = 1.0 - smoothstep(q1, q2, r);

      if(strokeType == 0) {
        colorOut = t * fillColor * faceColor;
      }

      if(strokeType > 0) {
        float s0 = smoothstep(q0, q1, r);
        float s1 = (1.0-smoothstep(q2, 1.0, r));
        float s = s0 * s1;

        if(strokeType > 1) {
          float s2 = 1.0 - smoothstep(w*pixX, (w+u)*pixX, abs(cpos.x));
          float s4 = smoothstep(-u*pix*0.5, u*pix*0.5, cpos.y);
          float s5 = 1.0-smoothstep(w*pix, (w+u)*pix, r);
          s = max(s, s2*s1*s4);
          s = max(s, s5);
        }

        colorOut = mix(t * fillColor * faceColor, strokeColor, s);
        //colorOut = fillColor;
      }
    }
    else if(geomType == 5) {
      float q0 = 1.0 - w2*pix - pix*u * 2.0;
      float q1 = 1.0 - w2*pix - pix*u;
      float q2 = 1.0 - pix*u;
      float t = 1.0 - smoothstep(q1, q2, r);

      float q0b = innerRadius - w2*pix - pix*u * 2.0;
      float q1b = innerRadius - w2*pix - pix*u;
      float q2b = innerRadius - pix*u;
      float tb = smoothstep(q1b, q2b, r);
      t = t * tb;

      if(strokeType == 0) {
        colorOut = t * fillColor * faceColor;
      }

      if(strokeType > 0) {
        float s0 = smoothstep(q0, q1, r);
        float s1 = (1.0-smoothstep(q2, 1.0, r));
        float s = s0 * s1;

        float s0b = smoothstep(q0b, q1b, r);
        float s1b = (1.0-smoothstep(q2b, innerRadius, r));
        float sb = s0b * s1b;
        s = max(s, sb);

        if(strokeType > 1) {
          float s2 = 1.0 - smoothstep(w*pixX, (w+u)*pixX, abs(cpos.x));
          float s4 = smoothstep(-u*pix*0.5, u*pix*0.5, cpos.y);
          float s5 = 1.0-smoothstep(w*pix, (w+u)*pix, r);
          s = max(s, s2*s1*s4);
          s = max(s, s5);
        }

        colorOut = mix(t * fillColor * faceColor, strokeColor, s);
      }
    }

    else if(geomType == 2) {
      float q0x = 1.0 - w2*pixX - pixX*u * 2.0;
      float q1x = 1.0 - w2*pixX - pixX*u;
      float q2x = 1.0 - pixX*u;
      float q0y = 1.0 - w2*pixY - pixY*u * 2.0;
      float q1y = 1.0 - w2*pixY - pixY*u;
      float q2y = 1.0 - pixY*u;

      float t1 = 1.0 - smoothstep(q1x, q2x, abs(cpos.x));
      float t2 = 1.0 - smoothstep(q1y, q2y, abs(cpos.y));
      float t = t1*t2;

      if(strokeType == 0) {
        colorOut = t * fillColor * faceColor;
        //colorOut = vec4(0.5, 0.1, 0.2, 1);
      }
      
      if(strokeType > 0) {
        float s0 = smoothstep(q0x, q1x, abs(cpos.x));
        float s1 = (1.0-smoothstep(q2x, 1.0, abs(cpos.x)));
        float s2 = smoothstep(q0y, q1y, abs(cpos.y));
        float s3 = (1.0-smoothstep(q2y, 1.0, abs(cpos.y)));
        float s = max(s0 * s1 * s3, s2 * s3 * s1);

        if(strokeType > 1) {
          float s4 = 1.0 - smoothstep(w*pixX, (w+u)*pixX, abs(cpos.x));
          float s5 = smoothstep(-u*pixY*0.5, u*pixY*0.5, cpos.y);
          s = max(s, s4*s3*s5);
        }

        colorOut = mix(t * fillColor * faceColor, strokeColor, s);
      }
    }
    else if(geomType == 3) {
      float u = min(w2*0.99, 4);
      float s0 = 1.0 - smoothstep(1.0-pixX*u, 1.0, abs(cpos.x)+(w2+u)*pixX);
      float s1 = 1.0 - smoothstep(w2*pixY, (w2+u)*pixY, abs(cpos.y));
      float s = s0 * s1;

      vec2 ppos = abs(cpos)/sizeXY.yx * sizeXY.x - vec2(sizeXY.x/sizeXY.y-(w2+u)*pixY, 0);
      float pr = length(ppos);
      float s2 = 1.0 - smoothstep(w2*pixY, (w2+u)*pixY, pr);

      s = max(s, s2);
      colorOut = s * strokeColor;// + (1.0-s) * vec4(1, 1, 1, 0.5);
      //colorOut = s * strokeColor + s2 * vec4(1, 1, 1, 0.5);
      //colorOut = s * strokeColor + (1-s)*vec4(0.5, 0.5, 0.5, 0.5);
      //colorOut = strokeColor;
    }
    else if(geomType == 4) {
      colorOut = faceColor * fillColor;
    }
  }

  else {
    colorOut = strokeColor;
  }

}
