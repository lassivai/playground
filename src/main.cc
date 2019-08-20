

#include "engine/sketch.h"

//#include "clockwork/clockwork_sketch.h"
#include "noise_sketch.h"
#include "colors_sketch.h"
#include "langton_sketch.h"
#include "growth_sketch.h"
#include "geom_sketch.h"
#include "geomtest_sketch.h"
#include "synth_sketch.h"
#include "interp_sketch.h"
#include "timer_sketch.h"
#include "shape_sketch.h"
#include "shader_sketch.h"
#include "shader2_sketch.h"
#include "post_sketch.h"
#include "test_sketch.h"
#include "fractal/fractal_sketch.h"
#include "integrate_sketch.h"
#include "volray_sketch.h"




int main(int argc, char* argv[])
{
  CommandLineArguments cliArgs;
  cliArgs.parse(argc, argv);

  Sketch *sketch = NULL;

  if(cliArgs.numValues("-x") > 0) {
    /*if(cliArgs.getValues("-x")[0].compare("clock") == 0) {
      sketch = new ClockworkSketch();
    }*/
    if(cliArgs.getValues("-x")[0].compare("noise") == 0) {
      sketch = new NoiseSketch();
    }
    if(cliArgs.getValues("-x")[0].compare("langton") == 0) {
      sketch = new LangtonSketch();
    }
    if(cliArgs.getValues("-x")[0].compare("colors") == 0) {
      sketch = new ColorsSketch();
    }
    if(cliArgs.getValues("-x")[0].compare("growth") == 0) {
      sketch = new GrowthSketch();
    }
    if(cliArgs.getValues("-x")[0].compare("geom") == 0) {
      sketch = new GeomSketch();
    }
    if(cliArgs.getValues("-x")[0].compare("geomtest") == 0) {
      sketch = new GeomTestSketch();
    }
    if(cliArgs.getValues("-x")[0].compare("synth") == 0) {
      sketch = new SynthSketch();
    }
    if(cliArgs.getValues("-x")[0].compare("fract") == 0) {
      sketch = new FractalSketch();
    }
    if(cliArgs.getValues("-x")[0].compare("interp") == 0) {
      sketch = new InterpSketch();
    }
    if(cliArgs.getValues("-x")[0].compare("timer") == 0) {
      sketch = new TimerSketch();
    }
    if(cliArgs.getValues("-x")[0].compare("shape") == 0) {
      sketch = new ShapeSketch();
    }
    if(cliArgs.getValues("-x")[0].compare("shader") == 0) {
      sketch = new ShaderSketch();
    }
    if(cliArgs.getValues("-x")[0].compare("shader2") == 0) {
      sketch = new Shader2Sketch();
    }
    if(cliArgs.getValues("-x")[0].compare("post") == 0) {
      sketch = new PostSketch();
    }
    if(cliArgs.getValues("-x")[0].compare("test") == 0) {
      sketch = new TestSketch();
    }
    if(cliArgs.getValues("-x")[0].compare("integ") == 0) {
      sketch = new IntegrateSketch();
    }
    if(cliArgs.getValues("-x")[0].compare("volray") == 0) {
      sketch = new VolraySketch();
    }
  }
  if(sketch == NULL) {
    return 0;
    //sketch = new TestSketch();
  }

  sketch->init(cliArgs);

  while(!sketch->events.quitRequested) {
    sketch->update();
  }

  sketch->quit();
  delete sketch;
}
