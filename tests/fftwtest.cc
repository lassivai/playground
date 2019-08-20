#include <vector>
#include <cstdio>
#include <ctgmath>
#include <functional>
#include "fftw3interface.h"


double map(double x, double a, double b, double c, double d) {
    return (x-a) / (b-a) * (d-c) + c;
}
double min(double a, double b) {
    return a < b ? a : b;
}
double max(double a, double b) {
    return a > b ? a : b;
}



struct Convolution {
  virtual void apply(const std::vector<double> &input, std::vector<double> &output) {}
};


struct BoxConvolution : public Convolution {
  //std::vector<double> filter;
  int size = 0;

  BoxConvolution() {}
  BoxConvolution(int size) {
    init(size);
  }

  void init(int size) {
    this->size = size;
    //filter.assign(size, 1.0/size);
  }
  
  virtual void apply(const std::vector<double> &input, std::vector<double> &output) override {
    output.resize(input.size());
    long lowerHalf = size/2;
    long upperHalf = size - lowerHalf;
    long a = 0, b = 0, n = 0;
    for(long i=0; i<input.size(); i++) {
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

  GaussianConvolution() {}
  GaussianConvolution(int size) {
    init(size);
  }

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
  virtual void apply(const std::vector<double> &input, std::vector<double> &output) override {
    output.resize(input.size());
    long lowerHalf = size/2;
    //long upperHalf = size - lowerHalf;

    for(long i=0; i<input.size(); i++) {
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

  VariableGaussianConvolution() {}
  VariableGaussianConvolution(int sizeStart, int sizeEnd) {
    init(sizeStart, sizeEnd);
  }

  void init(int sizeStart, int sizeEnd) {
    filterSizeMapper = [](double t) { return t; };

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

    numFilters = (sizeMain - min(sizeStart, sizeEnd)) / 2 + 1;
    filters.resize(numFilters);
    for(int i=0; i<numFilters; i++) {
        double t = numFilters == 1 ? 0 : (double)i/(numFilters-1);
        int n = map(filterSizeMapper(t), 0, 1, sizeSub, sizeMain);
        filters[i].resize(n);
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

  virtual void apply(const std::vector<double> &input, std::vector<double> &output) override {
    output.resize(input.size());

    for(long i=0; i<input.size(); i++) {
      long filterIndex = i * filters.size() / input.size();
      long size = filters[filterIndex].size();
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



double drawColumns(const std::vector<double> &v, double maxValue = 0, int maxColumn = 30) {
    //double minValue = 1e10, maxValue = -1e10;
    if(maxValue == 0) {
        for(long i=0; i<v.size(); i++) {
            maxValue = max(v[i], fabs(maxValue));
        }
    }
    for(long i=0; i<v.size(); i++) {
        printf("%06lu:", i);
        int n = map(v[i], 0, maxValue, 0, maxColumn);
        for(int k=-maxColumn; k<maxColumn+1; k++) {
            if(n < 0 && k < 0 && k >= n) printf("#");
            else if(k == 0) printf("|");
            else if(n > 0 && k <= n && k > 0) printf("#");
            else printf(" ");
        }
        printf("  %f\n", v[i]);
    }
    return maxValue;
}

double drawColumns(const std::vector<Vec2d> &v, double maxValue = 0, double maxValue2 = 0,int maxColumn = 30) {
    if(maxValue == 0) {
        for(long i=0; i<v.size(); i++) {
            maxValue = max(v[i].x, fabs(maxValue));
        }
    }

    if(maxValue2 == 0) {
        for(long i=0; i<v.size(); i++) {
            maxValue2 = max(v[i].y, fabs(maxValue2));
        }
    }


    for(long i=0; i<v.size(); i++) {
        int n = map(v[i].x, 0, maxValue, 0, maxColumn);
        printf("%06lu:", i);
        for(int k=-maxColumn; k<maxColumn+1; k++) {
            if(n < 0 && k < 0 && k >= n) printf("#");
            else if(k == 0) printf("|");
            else if(n > 0 && k <= n && k > 0) printf("#");
            else printf(" ");
        }
        printf("  %f\n", v[i].x);

        printf("%06lu:", i);
        int n2 = map(v[i].y, 0, maxValue2, 0, maxColumn);
        for(int k=-maxColumn; k<maxColumn+1; k++) {
            if(n2 < 0 && k < 0 && k >= n2) printf("'");
            else if(k == 0) printf("|");
            else if(n2 > 0 && k <= n2 && k > 0) printf("'");
            else printf(" ");
        }
        printf("  %f\n", v[i].y);
    }
    return maxValue;
}



void addSinWave(std::vector<double> &v, double freq, double amp) {
   for(int i=0; i<v.size(); i++) {
     double t = freq * 2.0*3.1415926535 * i / (v.size()-1);
     v[i] += amp * sin(t);
   }
}

/* Testing how to do forward fftw + inverse fftw and get the original array... */

int main() {
    int n = 1<<10;
    std::vector<double> in(n, 0);// = {1, 0, 0, 0, 1, 0, 1, 0, 0, 0};
//    std::vector<double> out(n, 0);
    std::vector<Vec2d> out(n);

    FFTW3Interface fftw;
    fftw.initialize(n, true, FFTW3Interface::ForwardAndReverse);

//    addSinWave(in, 5, 1);
    addSinWave(in, 50, 1);

    for(int i=0; i<n; i++) {
      fftw.input[i] = in[i];
    }
    fftw.transform();

    for(int i=0; i<n; i++) {
      //fftw.reverseInput[i] = fftw.output2[i];
      fftw.reverseInput[i].x = 0;
      fftw.reverseInput[i].y = 0;
    }
    fftw.reverseInput[51].x = 1;

    fftw.transformReverse();

    printf("original input:\n");
    drawColumns(in);
    printf("input after forward+inverse fftw:\n");
    drawColumns(fftw.reverseOutput);


/*    for(int i=0; i<n; i++) {
       out[i].x
    }*/

    printf("transform:\n");
    drawColumns(fftw.output2);
//    printf("input after forward+inverse fftw:\n");
//    drawColumns(fftw.reverseOutput);



    /*GaussianConvolution conv(10);
    VariableGaussianConvolution conv(4, 30);
    conv.apply(in, out);

    printf("in:\n");
    double m = drawColumns(in);

    printf("out:\n");
    drawColumns(out, m);*/

    return 0;
}
