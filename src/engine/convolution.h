#pragma once

#include <vector>





struct Convolution {
  virtual ~Convolution() {}
  virtual void apply(const std::vector<double> &input, std::vector<double> &output, long maxIndex = -1) {}
  virtual long getFilterMaxSize() { return 0; }
};


struct BoxConvolution : public Convolution {
  //std::vector<double> filter;
  int size = 0;

  virtual ~BoxConvolution() {}
  BoxConvolution() {}
  BoxConvolution(int size) {
    init(size);
  }

  void init(int size) {
    this->size = size;
    //filter.assign(size, 1.0/size);
  }
  virtual long getFilterMaxSize() { return size; }
  
  virtual void apply(const std::vector<double> &input, std::vector<double> &output, long maxIndex = -1) override {
    output.resize(input.size());
    long lowerHalf = size/2;
    long upperHalf = size - lowerHalf;
    long a = 0, b = 0, n = 0;
    
    long num = maxIndex >= 0 ? min(maxIndex, input.size()) : input.size();
    
    for(long i=0; i<num; i++) {
      a = (i < lowerHalf) ? 0 : i - lowerHalf;
      b = (i+upperHalf >= input.size()) ? b = input.size() : i + upperHalf;
      int n = b - a;
      double t = 1.0/n;
      output[i] = 0;
      for(long k=a; k<b; k++) {
        output[i] += input[k] * t;
      }
    }
  }
};

struct GaussianConvolution : public Convolution {
  std::vector<double> filter;
  int size = 0;

  virtual ~GaussianConvolution() {}
  GaussianConvolution() {}
  GaussianConvolution(int size) {
    init(size);
  }

  virtual long getFilterMaxSize() { return size; }
  
  void init(int size) {
    this->size = size;
    filter.resize(size);
    double total = 0;
    
    for(int i=0; i<size; i++) {
      double x = map(i, 0, size-1, -2.6, 2.6);
      filter[i] = exp(-x*x / 2.0);
      total += filter[i];
    }
    for(int i=0; i<size; i++) {
      filter[i] /= total;
    }
  }  
  virtual void apply(const std::vector<double> &input, std::vector<double> &output, long maxIndex = -1) override {
    //output.resize(input.size());
    long n = min(input.size(), output.size());
    n = maxIndex >= 0 ? min(maxIndex, n) : n;

    long lowerHalf = size/2;
    //long upperHalf = size - lowerHalf;

    for(long i=0; i<n; i++) {
      output[i] = 0;
      for(long k=0; k<size; k++) {
        long p = i + k - lowerHalf;
        if(p >= 0 && p < input.size()) {
            output[i] += input[p] * filter[k];
        }
      }
    }
  }
};


struct VariableGaussianConvolution : public Convolution {
  std::vector<double> filterMain;
  std::function<double(double t)> filterSizeMapper;
  int sizeStart = 0, sizeEnd = 0;
  int sizeMain = 0;

  std::vector<std::vector<double>> filters;
  int numFilters = 0;

  virtual ~VariableGaussianConvolution() {}
  VariableGaussianConvolution() {}
  VariableGaussianConvolution(int sizeStart, int sizeEnd) {
    init(sizeStart, sizeEnd);
  }
  VariableGaussianConvolution(int sizeStart, int sizeEnd, const std::function<double(double t)> &filterSizeMapper) {
    init(sizeStart, sizeEnd, filterSizeMapper);
  }
  
  virtual long getFilterMaxSize() { return sizeMain; }
  
  void init(int sizeStart, int sizeEnd) {
    init(sizeStart, sizeEnd, [](double t) { return t; });
  }
  void init(int sizeStart, int sizeEnd, const std::function<double(double t)> &filterSizeMapper) {
    this->filterSizeMapper = filterSizeMapper;

    this->sizeStart = sizeStart;
    this->sizeEnd = sizeEnd;
    sizeMain = max(sizeStart, sizeEnd);
    long sizeSub = min(sizeStart, sizeEnd);

    filterMain.resize(sizeMain);
    double total = 0;
    
    for(int i=0; i<sizeMain; i++) {
      double x = map(i, 0, sizeMain-1, -2.6, 2.6);
      filterMain[i] = exp(-x*x / 2.0);
      total += filterMain[i];
    }
    for(int i=0; i<sizeMain; i++) {
      filterMain[i] /= total;
    }

    numFilters = (sizeMain - sizeSub) / 2 + 1;
    filters.resize(numFilters);
    for(int i=0; i<numFilters; i++) {
        double t = numFilters == 1 ? 0 : (double)i/(numFilters-1);
        int n = clamp(map(t, 0, 1, sizeSub, sizeMain), sizeSub, sizeMain);
        filters[i].resize(n);
        //printf("¤ %d %d ¤\n", i, n);
        //resizeArray(filterMain, filters[i]);
        double total = 0;
        
        for(int k=0; k<filters[i].size(); k++) {
          double x = map(k, 0, filters[i].size()-1, -2.6, 2.6);
          filters[i][k] = exp(-x*x / 2.0);
          total += filters[i][k];
        }
        for(int k=0; k<filters[i].size(); k++) {
          filters[i][k] /= total;
        }

    }
  }

  virtual void apply(const std::vector<double> &input, std::vector<double> &output, long maxIndex = -1) override {
    //output.resize(input.size());
    long n = min(input.size(), output.size());
    n = maxIndex >= 0 ? min(maxIndex, n) : n;
    
    for(long i=0; i<n; i++) {
      //long filterIndex = i * filters.size() / input.size();
      long filterIndex = (long)(clamp(filterSizeMapper((double)i / (input.size()-1)), 0.0, 1.0) *  (filters.size()-1));
      //printf("filter index i: %lu = %lu/%lu\n", i, filterIndex+1, filters.size());
      
      long size = filters[filterIndex].size();
      if(size < 2) {
        output[i] = input[i];
      }
      long lowerHalf = size/2;
      output[i] = 0;
      for(long k=0; k<size; k++) {
        long p = i + k - lowerHalf;
        if(p >= 0 && p < input.size()) {
            output[i] += input[p] * filters[filterIndex][k];
        }
      }
    }
  }
};