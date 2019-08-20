#pragma once


struct FractalAnim
{
  std::vector<Texture*> renderedFrames;

  std::vector<FractalParameters> fractParsKeyFrames;
  int numKeyFrames = 0;
  int keyFrame = 0;
  double frameFraction = 0;
  int frameFractionCounter = 0;
  bool animationPlaying = false;
  int numFractionalFramesPerSec = 30;
  int numFractionalFrames = 0;
  double animationStartedTime = 0;
  double animationStartedFrameTime = 0;
  bool recordingStarted = false;

  int swapFrame = -1;

  static const int AnimationRecordModeNone = 0, AnimationRecordModeMemoryImages = 1, AnimationRecordModeToFile = 2;
  int animationRecordMode = 0;

  static const int AnimationProgressionModeBasic = 0, AnimationProgressionModeRenderingTime = 1, AnimationProgressionModeNumSamples = 2;
  int animationProgressionMode = 1;
  double animationProgressionRenderingTime = 1.0/10.0;
  unsigned long animationProgressionNumSamples = 1000000;

  bool interpolateColorsByRGB = false;

  std::string saveFilenameBase;



  template<class Archive>
  void serialize(Archive &ar, const std::uint32_t version) {
    ar(
      fractParsKeyFrames,
//      numKeyFrames,
      keyFrame,
      frameFraction,
      frameFractionCounter,
//      animationPlaying,
      numFractionalFramesPerSec,
      numFractionalFrames,
//      animationStartedTime,
//      animationStartedFrameTime,
//      recordingStarted,
      swapFrame,
      animationRecordMode,
      animationProgressionMode,
      animationProgressionRenderingTime,
      animationProgressionNumSamples,
      interpolateColorsByRGB
    );
    numKeyFrames = fractParsKeyFrames.size();
    animationPlaying = false;
    animationStartedTime = 0;
    animationStartedFrameTime = 0;
    recordingStarted = 0;
  }

  ~FractalAnim() {}

  void clear() {
    fractParsKeyFrames.clear();
    numKeyFrames = 0;
    numFractionalFrames = 0;
    resetSettings();
  }
  void resetSettings() {
    keyFrame = 0;
    frameFraction = 0;
    frameFractionCounter = 0;
    animationPlaying = false;
    numFractionalFramesPerSec = 30;
    animationStartedTime = 0;
    animationStartedFrameTime = 0;
    recordingStarted = false;
    swapFrame = -1;
    animationRecordMode = 0;
    animationProgressionMode = 1;
    animationProgressionRenderingTime = 1.0/10.0;
    animationProgressionNumSamples = 1000000;

    interpolateColorsByRGB = false;
  }


  double getFrameTime() {
    if(numKeyFrames < 1) return 0;
    double t = 0;
    for(int i=0; i<keyFrame; i++) {
      t += fractParsKeyFrames[i].frameDuration;
    }
    t += frameFraction * fractParsKeyFrames[keyFrame].frameDuration;
    return t;
  }
  double getFrame() {
    if(numKeyFrames < 1) return 0;
    int t = 0;
    for(int i=0; i<keyFrame; i++) {
      t += int(fractParsKeyFrames[i].frameDuration * numFractionalFramesPerSec);
    }
    t += frameFractionCounter;
    return t;
  }

  void setAnimationStartTimes() {
    if(numKeyFrames < 1) return;
    animationStartedTime = (double)SDL_GetTicks() * 0.001;
    animationStartedFrameTime = getFrameTime();
  }

  void updateFrameFraction() {
    if(numKeyFrames < 1) return;
    int numFractionalFrames = int(fractParsKeyFrames[keyFrame].frameDuration * numFractionalFramesPerSec);
    frameFraction = (double)frameFractionCounter/numFractionalFrames;
  }


  void startRecording(FractalParameters &fractPars) {
    keyFrame = 0;
    frameFraction = 0;
    frameFractionCounter = 0;
    animationPlaying = true;
    animationRecordMode = AnimationRecordModeToFile;

    std::string typeStr("flame");
    for(int i=0; i<fractPars.numActiveVariations; i++) {
      if(fractPars.flamePars[i].variation >= 0) {
        typeStr += "-" + std::to_string(fractPars.flamePars[i].variation);
      }
    }
    saveFilenameBase = "output/sequences/" + typeStr + "_" + getTimeString() + "/";


    for(int i=0; i<renderedFrames.size(); i++) {
      delete renderedFrames[i];
    }
    renderedFrames.clear();
    fractPars.viewChanged = true;
    fractPars.renderingStartedMillis = SDL_GetTicks(); // FIXME
    previousFrame(fractPars);
    recordingStarted = false;
  }

  void stopRecording() {
    animationRecordMode = 0;
    animationPlaying = false;
    recordingStarted = false;
  }

  void readyToRecord() {
    recordingStarted = true;
  }

  void togglePlay() {
    if(numKeyFrames < 1) return;
    if(animationPlaying && animationRecordMode > 0) {
      stopRecording();
    }
    else {
      animationPlaying = !animationPlaying;
    }
  }
  void addKeyFrame(FractalParameters fractPars) {
    numKeyFrames++;
    if(fractParsKeyFrames.size() < numKeyFrames) {
      fractParsKeyFrames.push_back(fractPars);
    }
    if(numKeyFrames == 1) {
      numFractionalFrames = int(fractParsKeyFrames[keyFrame].frameDuration * numFractionalFramesPerSec);
    }
  }

  void insertKeyFrame(FractalParameters fractPars) {
    fractParsKeyFrames.insert(fractParsKeyFrames.begin()+keyFrame, fractPars);
    numKeyFrames++;
    if(numKeyFrames == 1) {
      numFractionalFrames = int(fractParsKeyFrames[keyFrame].frameDuration * numFractionalFramesPerSec);
    }
  }

  void applyKeyFrame(FractalParameters &fractPars) {
    if(numKeyFrames < 1) return;
    fractParsKeyFrames[keyFrame].set(fractPars);
  }

  void markSwapFrame() {
    swapFrame = keyFrame;
  }

  void swapKeyFrames(FractalParameters &fractPars) {
    if(swapFrame < 0 || swapFrame >= numKeyFrames || swapFrame == keyFrame) return;

    std::swap(fractParsKeyFrames[keyFrame],
              fractParsKeyFrames[swapFrame]);
    fractPars.set(fractParsKeyFrames[keyFrame]);
    fractPars.viewChanged = true;
  }

  void removeCurrentKeyFrame(FractalParameters &fractPars) {
    if(numKeyFrames < 1) return;
    numKeyFrames = max(0, numKeyFrames-1);
    fractParsKeyFrames.erase(fractParsKeyFrames.begin()+keyFrame);
    if(keyFrame >= numKeyFrames) {
      keyFrame = numKeyFrames-1;
    }
    fractPars.set(fractParsKeyFrames[keyFrame]);
    fractPars.viewChanged = true;
    numFractionalFrames = int(fractParsKeyFrames[keyFrame].frameDuration * numFractionalFramesPerSec);
  }

  void removeKeyFrame(FractalParameters &fractPars) {
    if(numKeyFrames < 2) return;
    numKeyFrames = max(0, numKeyFrames-1);
    if(keyFrame >= numKeyFrames) {
      keyFrame = numKeyFrames-1;
      fractPars.set(fractParsKeyFrames[keyFrame]);
      fractPars.viewChanged = true;
      numFractionalFrames = int(fractParsKeyFrames[keyFrame].frameDuration * numFractionalFramesPerSec);
    }
  }
  void getCurrentKeyFrame(FractalParameters &fractPars) {
    if(numKeyFrames < 1) return;
    fractPars.set(fractParsKeyFrames[keyFrame]);
    fractPars.viewChanged = true;
  }
  void nextKeyFrame(FractalParameters &fractPars) {
    if(numKeyFrames < 1) return;
    keyFrame = (keyFrame+1) % numKeyFrames;
    frameFraction = 0;
    frameFractionCounter = 0;
    fractPars.set(fractParsKeyFrames[keyFrame]);
    fractPars.viewChanged = true;

    if(keyFrame == 0 && animationPlaying) {
      setAnimationStartTimes();
    }
    numFractionalFrames = int(fractParsKeyFrames[keyFrame].frameDuration * numFractionalFramesPerSec);
  }
  void previousKeyFrame(FractalParameters &fractPars) {
    if(numKeyFrames < 1) return;
    keyFrame = (keyFrame+numKeyFrames-1) % numKeyFrames;
    frameFraction = 0;
    frameFractionCounter = 0;
    fractPars.set(fractParsKeyFrames[keyFrame]);
    fractPars.viewChanged = true;
    numFractionalFrames = int(fractParsKeyFrames[keyFrame].frameDuration * numFractionalFramesPerSec);
  }
  void nextFrame(FractalParameters &fractPars) {
    if(numKeyFrames < 1) return;
    frameFractionCounter++;

    if(frameFractionCounter >= numFractionalFrames) {
      nextKeyFrame(fractPars);
    }
    else {
      frameFraction = (double)frameFractionCounter/numFractionalFrames;
      interpolate(fractPars);
    }
    fractPars.viewChanged = true;
  }
  void previousFrame(FractalParameters &fractPars) {
    if(numKeyFrames < 1) return;
    frameFractionCounter--;
    if(frameFractionCounter < 0) {
      previousKeyFrame(fractPars);
      frameFractionCounter = numFractionalFrames - 1;
    }
    frameFraction = (double)frameFractionCounter/numFractionalFrames;
    interpolate(fractPars);
    fractPars.viewChanged = true;
  }

  int getNumFractionalFrames(FractalParameters &fractParsframe) {
    return int(fractParsframe.frameDuration * numFractionalFramesPerSec);
  }

  FractalParameters &getCurrentKeyFrame() {
    return fractParsKeyFrames[keyFrame];
  }

  const char *getProgressionModeStr() {
    if(animationProgressionMode == AnimationProgressionModeNumSamples) {
      return "num samples";
    }
    if(animationProgressionMode == AnimationProgressionModeRenderingTime) {
      return "render time";
    }
  }

  // FIXME
  void update(FractalParameters &fractPars, Texture *renderedImage) {
    if(numKeyFrames < 1) return;
    numFractionalFrames = getNumFractionalFrames(fractParsKeyFrames[keyFrame]);
    bool frameChanged = false;

    if(animationPlaying && recordingStarted) {
      if(animationProgressionMode == AnimationProgressionModeBasic) {
        double frameTime = 1.0/numFractionalFramesPerSec;
        double d = (double)SDL_GetTicks() * 0.001 - animationStartedTime + animationStartedFrameTime;
        if(d >= getFrameTime() + frameTime) {
          nextFrame(fractPars);
          frameChanged = true;
        }
      }
      if(animationProgressionMode == AnimationProgressionModeRenderingTime) {
        double d = ((double)SDL_GetTicks() - fractPars.renderingStartedMillis) * 0.001;
        if(d >= animationProgressionRenderingTime) {
          nextFrame(fractPars);
          fractPars.renderingStartedMillis = SDL_GetTicks(); // FIXME
          frameChanged = true;
        }
      }
      if(animationProgressionMode == AnimationProgressionModeNumSamples) {
        if(fractPars.numSamples >= animationProgressionNumSamples) {
          nextFrame(fractPars);
          frameChanged = true;
        }
      }
    }

    if(frameChanged) {
      if(animationRecordMode == AnimationRecordModeMemoryImages) {
        if(renderedImage) {
          renderedFrames.push_back(renderedImage->getCopy());
        }
      }
      if(animationRecordMode == AnimationRecordModeToFile) {
        if(renderedImage) {
          std::string filename;
          filename += std::to_string(keyFrame+1) + "_of_" + std::to_string(numKeyFrames);
          filename += "_(" + std::to_string(frameFractionCounter+1);
          filename += "_of_" + std::to_string(numFractionalFrames) + ")";
          filename += ".png";

          renderedImage->saveToFile(saveFilenameBase+filename);
        }
      }
      if(animationRecordMode > 0) {
        if(keyFrame == numKeyFrames - 1 && frameFractionCounter == getNumFractionalFrames(fractParsKeyFrames[numKeyFrames-1]) - 1) {
          stopRecording();
        }
      }
    }

  }

  void saveRecordedFrames() {
    for(int i=0; i<renderedFrames.size(); i++) {
      std::string filename = "output/test/frame_";
      filename += std::to_string(i+1);
      filename += "_of_" + std::to_string(renderedFrames.size());
      filename += "_" + getTimeString("%F_%T") + ".png";

      renderedFrames[i]->saveToFile(filename);
    }
  }

  void interpolate(FractalParameters &fractPars);
  void interpolateRGB(Vec3d &col, double v);
};
CEREAL_CLASS_VERSION(FractalAnim, 1);



void FractalAnim::interpolate(FractalParameters &fractPars) {

  double curv = 0.75;
  double f = frameFraction;
  int n = numKeyFrames;
  if(n < 1) return;

  FractalParameters *p0 = &fractParsKeyFrames[(keyFrame+n-1) % n];
  FractalParameters *p1 = &fractParsKeyFrames[keyFrame];
  FractalParameters *p2 = &fractParsKeyFrames[(keyFrame+1) % n];
  FractalParameters *p3 = &fractParsKeyFrames[(keyFrame+2) % n];

  fractPars.location.set(catmullRom(f, curv, p0->location, p1->location, p2->location, p3->location));
  fractPars.juliaPoint.set(catmullRom(f, curv, p0->juliaPoint, p1->juliaPoint, p2->juliaPoint, p3->juliaPoint));
  fractPars.scale = catmullRom(f, curv, p0->scale, p1->scale, p2->scale, p3->scale);
  fractPars.maxIter = catmullRom(f, curv, p0->maxIter, p1->maxIter, p2->maxIter, p3->maxIter);
  fractPars.minIter = catmullRom(f, curv, p0->minIter, p1->minIter, p2->minIter, p3->minIter);
  fractPars.mandelbrotPower = catmullRom(f, curv, p0->mandelbrotPower, p1->mandelbrotPower, p2->mandelbrotPower, p3->mandelbrotPower);

  for(int n=0; n<FractalParameters::numActiveVariations; n++) {
    for(int i=0; i<FlameParameters::numPars; i++) {
      fractPars.flamePars[n].p[i] = catmullRom(f, curv, p0->flamePars[n].p[i], p1->flamePars[n].p[i], p2->flamePars[n].p[i], p3->flamePars[n].p[i]);
    }
  }
  if(!interpolateColorsByRGB) {
    for(int i=0; i<FractalParameters::numColorArgs; i++) {
      fractPars.colorArg[i] = catmullRom(f, curv, p0->colorArg[i], p1->colorArg[i], p2->colorArg[i], p3->colorArg[i]);
    }
  }
  /*for(int i=0; i<FractalParameters::numArgs; i++) {
    fractPars.arg[i] = catmullRom(f, curv, p0->arg[i], p1->arg[i], p2->arg[i], p3->arg[i]);
  }*/

}


void FractalAnim::interpolateRGB(Vec3d &col, double v) {
  int n = numKeyFrames;
  if(n < 1) return;

  FractalParameters *p0 = &fractParsKeyFrames[(keyFrame+n-1) % n];
  FractalParameters *p1 = &fractParsKeyFrames[keyFrame];
  FractalParameters *p2 = &fractParsKeyFrames[(keyFrame+1) % n];
  FractalParameters *p3 = &fractParsKeyFrames[(keyFrame+2) % n];

  Vec3d col0 = p0->getColor(v);
  Vec3d col1 = p1->getColor(v);
  Vec3d col2 = p2->getColor(v);
  Vec3d col3 = p3->getColor(v);

  col = catmullRom(frameFraction, 0.2, col0, col1, col2, col3);
}
