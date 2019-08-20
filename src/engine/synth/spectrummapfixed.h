#pragma once


struct SpectrumMapFixed
{
  Texture spectrumImg;

  double spectrumMapScaling = 44;

  Vec3d gradientParameters = Vec3d(-0.45, 2.0, 0);

  std::vector<std::vector<double>> spectrums;

  FFTW3Interface waveFormFFTW;
  
  int sampleRate = 1;

  int transformSize = 1 << 15;
  
  int width = 1;
  int height = 1;
  
  Vec2d spectrumLimits = Vec2d(1, 10000);
  bool spectrumAsNotes = false;
  int volumeUnit = 2;
  
  bool textureRefreshed = false;
  
  bool horizontal = false;
  
  SpectrumMapFixed() {
    spectrumImg.create(32, 32);
    spectrumImg.clear(1, 0, 1, 0.6);
  }
  
  void init(int sampleRate, int width, int height, bool horizontal = false) {
    this->sampleRate = sampleRate;
    this->horizontal = horizontal;

    this->width = width;
    this->height = height;

    spectrumImg.create(width, height);
    spectrumImg.clear(1, 0, 1, 0.6);

    waveFormFFTW.initialize(transformSize, false);
    
    if(horizontal) {
      spectrums.resize(this->width);
      for(int i=0; i<this->width; i++) {
        spectrums[i].resize(this->height, 0);
      }
    }
    else {
      spectrums.resize(this->height);
      for(int i=0; i<this->height; i++) {
        spectrums[i].resize(this->width, 0);
      }
    }
  }

  void render(int x, int y, const Vec4d &overlayColor = Vec4d(1, 1, 1, 1)) {
    refreshTexture();
    spectrumImg.renderCenter(x, y, overlayColor);
  }
  
  bool updatingPixels = false;
  bool pixelUpdateBlocked = false;


  void update(std::vector<double> &waveForm) {
    if(waveForm.size() < waveFormFFTW.getSize()) {
      printf("Warning at SpectrumMapFixed.update(), waveForm.size() < waveFormFFTW.getSize(), %lu < %lu\n", waveForm.size(), waveFormFFTW.getSize());
    }
    for(int x=0; x<spectrums.size(); x++) {
      long startIndex = map(x, 0, spectrums.size()-1, 0, waveForm.size()-transformSize);
      
      if(startIndex + transformSize <= waveForm.size()) {
        for(long k=0; k<transformSize; k++) {
          waveFormFFTW.input[k] = waveForm[startIndex + k];
        }
        waveFormFFTW.transform();
        
        if(spectrumAsNotes) {
          double minNote = freqToNote(spectrumLimits.x);
          double maxNote = freqToNote(spectrumLimits.y);

          for(int i=0; i<spectrums[x].size(); i++) {
            double noteBegin = map(i, 0.0, spectrums[x].size()-1, minNote, maxNote);
            double noteEnd = map(i+1, 0.0, spectrums[x].size()-1, minNote, maxNote);
            double frequencyMin = noteToFreq(noteBegin);
            double frequencyMax = noteToFreq(noteEnd);
            int f0 = (int)map(frequencyMin, 0.0, sampleRate/2.0, 0, waveFormFFTW.output.size()/2);
            int f1 = (int)map(frequencyMax, 0.0, sampleRate/2.0, 0, waveFormFFTW.output.size()/2);
            double t = 0;
            for(int k=f0; k<=f1; k++) {
              if(k < 0 || k >= waveFormFFTW.output.size()) {
                printf("Error at SpectrumMapFixed::update(), invalid frequency at spectrum at notes conversion, %d\n", k);
              }
              else {
                t += waveFormFFTW.output[k];
              }
            }
            t = t / (1.0 + f1 - f0);
            spectrums[x][i] = t;
          }
        }
        else {
          int f0 = (int)map(spectrumLimits.x, 0.0, sampleRate/2.0, 0, waveFormFFTW.output.size()/2);
          int f1 = (int)map(spectrumLimits.y, 0.0, sampleRate/2.0, 0, waveFormFFTW.output.size()/2);
          
          resizeArray(waveFormFFTW.output, spectrums[x], f0, f1);
        }
        scaleArray(spectrums[x], spectrumMapScaling);

        for(int i=0; i<spectrums[x].size(); i++) {
          if(std::isnan(spectrums[x][i])) {
            spectrums[x][i] = 0.0;
          }
          if(volumeUnit == 1) {
            if(spectrums[x][i] <= 1.0e-5) {
              spectrums[x][i] = 0;
            }
            else {
              spectrums[x][i] = log10(spectrums[x][i]);
              spectrums[x][i] = (spectrums[x][i] + 5.0) / 5.0;
            }
          }
          else if(volumeUnit == 2) {
            spectrums[x][i] = pow(spectrums[x][i], 0.5);
          }
          else if(volumeUnit == 3) {
            spectrums[x][i] = (pow(0.13, spectrums[x][i]) - 1.0) / (0.13 - 1.0);
          }
        }
      }
      else {
        std::fill(spectrums[x].begin(), spectrums[x].end(), 0);
        /*for(int i=0; i<spectrums[x].size(); i++) {
          spectrums[x][i] = 0;
        }*/
      }
    }
    if(horizontal) {
      for(int x=0; x<spectrums.size(); x++) {
        for(int y=0; y<spectrums[x].size(); y++) {
          double v = clamp(spectrums[x][y], 0, 1);
          spectrumImg.setPixel(x, y, hslToRgb(gradientParameters.x + gradientParameters.y*v, gradientParameters.z, v));
        }
      }
    }
    else {
      for(int x=0; x<spectrums.size(); x++) {
        for(int y=0; y<spectrums[x].size(); y++) {
          double v = clamp(spectrums[x][y], 0, 1);
          spectrumImg.setPixel(y, x, hslToRgb(gradientParameters.x + gradientParameters.y*v, gradientParameters.z, v));
        }
      }
    }
    
    textureRefreshed = false;

  }

  void refreshTexture() {    
    if(!textureRefreshed) {
      spectrumImg.applyPixels();
    }
  }



};
