#version 450 core

uniform sampler2D brushTexture, canvasTexture;
uniform vec2 canvasTextureSize;
uniform int useCanvasTexture;
uniform vec4 brushColor;

in vec2 texCoord;
in vec4 color;
out vec4 colorOut;

// TODO other blending equations for different sorts of paints, brushes and materials

void main(){
  vec4 brush = texture(brushTexture, texCoord);
  brush *= brushColor;

  if(useCanvasTexture == 1) {
    vec2 uv = gl_FragCoord.xy/canvasTextureSize.xy;
    //uv.y = 1.0 - uv.y;
    //uv = fragTexCoord;
    vec4 canvas = texture(canvasTexture, uv);
    //brush = mix(brush*canvas, brush, 0.2);
    //brush = min(brush*canvas+0.14*brush, brush);
    brush = min(brush*canvas+0.14*brush, brush);

  }

  colorOut = brush;
}


/*
Whatever. My challenge was to make use of canvas texture as blending factor for the brush. As I also wanted the final color to be closer and closer to the actual paint color the more paint was applied I needed to feedback the actual rendered image for this shader. I don't know if there would be more appropriate way to do it. At least this is not quite complete yet. With certain initial canvas images the result is really close to the one I had in mind. Where there are really dark areas in the initial canvas the result is not quite what is should be, yet. There might be still that texture coordinate problem haunting (probably not). Apparently one problem is insufficient precision of byte values to do the stuff I tried to do. Some weird blocks are apparent on areas of constant color (looks like compression artifacts)... The whole feedback thing isn't officially supported, apparently.

Update: Changed the approach to copy the rendered image to another texture which is then feeded back for the shader. Everything seems to work now.

But really slowly if multiple steps involved.

Update: now copying only the part that changed. Still quite slow with multistep brushes.

Maybe satisfactory results could yield only one render target swap per primary brush stroke. Let's try

That pencil doesn't even need this feedback based blending, just testing the speed.

Update: After few other set backs this might now work, finally. Maybe.
*/
