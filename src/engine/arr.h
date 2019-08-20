#pragma once
#include "util.h"
#include "mathl.h"

template<class A> struct CircularBuffer : public std::vector<A> {
  unsigned int currentPosition = 0;

  virtual ~CircularBuffer() {}
  CircularBuffer() {}

  CircularBuffer(int size) : std::vector<A>(size) {
    currentPosition = 0;
  }
  CircularBuffer(int size, const A &initialValue) : std::vector<A>(size, initialValue) {
    currentPosition = 0;
  }

  void resize(unsigned int size) {
    std::vector<A>::resize(size);
    currentPosition = 0;
  }
  void clear() {
    std::vector<A>::clear();
    currentPosition = 0;
  }
  void put(const A &value) {
    this->at(currentPosition) = value;
    currentPosition++;
    if(currentPosition >= this->size()) {
      currentPosition = 0;
    }
  }

};

template<class A>
static double getAverage(const std::vector<A> &arr) {
  double total = 0;
  for(int i=0; i<arr.size(); i++) {
    total += arr[i];
  }
  return total / arr.size();
}

template<class A>
static A getArrayMin(const std::vector<A> &arr) {
  if(arr.size() == 0) {
    printf("Error at getArrayMin(), empty array\n");
    return 0;
  }
  A minValue = arr[0];
  for(int i=1; i<arr.size(); i++) {
    if(arr[i] < minValue) {
      minValue = arr[i];
    }
  }
  return minValue;
}

template<class A>
static A getArrayMax(const std::vector<A> &arr) {
  if(arr.size() == 0) {
    printf("Error at getArrayMax(), empty array\n");
    return 0;
  }
  A maxValue = arr[0];
  for(int i=1; i<arr.size(); i++) {
    if(arr[i] > maxValue) {
      maxValue = arr[i];
    }
  }
  return maxValue;
}








static void getAverageArray(const std::vector<std::vector<double>> &arrArr, std::vector<double> &avgArr) {
  avgArr.assign(avgArr.size(), 0);
  for(int i=0; i<arrArr.size(); i++) {
    if(arrArr[i].size() != avgArr.size()) {
      printf("Error at getAverageArray(), mismatching array sizes %d and %d\n", (int)arrArr[i].size(), (int)avgArr.size());
      return;
    }
    for(int k=0; k<avgArr.size(); k++) {
      avgArr[k] += arrArr[i][k];
    }
  }
  for(int k=0; k<avgArr.size(); k++) {
    avgArr[k] /= arrArr.size();
  }
}

/*template<class A, class B>
void resizeArray(const std::vector<A> &src, std::vector<B> &dst, int srcStart = -1, int srcEnd = -1, bool useCubicInterpolation = false) {
  resizeArray(src, dst, srcStart, srcEnd, useCubicInterpolation);
}*/


static void resizeArray(const std::vector<double> &src, std::vector<double> &dst, long srcStart = -1, long srcEnd = -1, long dstStart = -1, long dstEnd = -1, bool useCubicInterpolation = false) {
  if(src.size() == 0 || dst.size() == 0) return;
  srcStart = srcStart >= 0 && srcStart < src.size() ? srcStart : 0;
  srcEnd = srcEnd >= srcStart && srcEnd < src.size() ? srcEnd : src.size();

  dstStart = dstStart >= 0 && dstStart < dst.size() ? dstStart : 0;
  dstEnd = dstEnd >= dstStart && dstEnd < dst.size() ? dstEnd : dst.size();

  long srcSize = srcEnd - srcStart;
  long dstSize = dstEnd - dstStart;

  if(srcSize >= dstSize) {
    
    for(long i=0; i<dstSize; i++) {
      double srcA = map(i, 0.0, dstSize, srcStart, srcEnd-1.0);
      double srcB = map(i+1, 0.0, dstSize, srcStart, srcEnd-1.0);
      long a = (long)srcA;
      long b = (long)srcB;
      double d = 0;
      
      d += src[a] * (1.0-(srcA-a));
      d += src[b] * (srcB-b);
      for(long k=a+1; k<b; k++) {
        d += src[k];
      }
      dst[i + dstStart] = d / (srcB-srcA);
    }
  }
  else {
    for(int i=0; i<dstSize; i++) {
      double f = i * srcSize / dstSize;
      int a = floor(f) + srcStart;
      f = fract(f);
      if(useCubicInterpolation) {
        int g = max(0, a-1);
        int b = min(srcSize-1, a+1);
        int h = min(srcSize-1, a+2);
        dst[i] = catmullRom(f, 0.5, src[g], src[a], src[b], src[h]);
      }
      else {
        long b = a+1;
        f = fract(f);
        if(b < srcSize) {
          dst[i+dstStart] = lerp(src[a], src[b], f);
        }
        else {
          dst[i+dstStart] = src[a];
        }
      }
    }
  }
}

static void resizeArray(const std::vector<Vec2d> &src, std::vector<Vec2d> &dst, long srcStart = -1, long srcEnd = -1, long dstStart = -1, long dstEnd = -1, bool useCubicInterpolation = false) {
  if(src.size() == 0 || dst.size() == 0) return;
  srcStart = srcStart >= 0 && srcStart < src.size() ? srcStart : 0;
  srcEnd = srcEnd >= srcStart && srcEnd < src.size() ? srcEnd : src.size();

  dstStart = dstStart >= 0 && dstStart < dst.size() ? dstStart : 0;
  dstEnd = dstEnd >= dstStart && dstEnd < dst.size() ? dstEnd : dst.size();

  long srcSize = srcEnd - srcStart;
  long dstSize = dstEnd - dstStart;

  if(srcSize >= dstSize) {
    
    for(long i=0; i<dstSize; i++) {
      double srcA = map(i, 0.0, dstSize, srcStart, srcEnd-1.0);
      double srcB = map(i+1, 0.0, dstSize, srcStart, srcEnd-1.0);
      long a = (long)srcA;
      long b = (long)srcB;
      Vec2d d;
      d.x += src[a].x * (1.0-(srcA-a));
      d.y += src[a].y * (1.0-(srcA-a));
      d.x += src[b].x * (srcB-b);
      d.y += src[b].y * (srcB-b);
      for(int k=a+1; k<b; k++) {
        d.x += src[k].x;
        d.y += src[k].y;
      }
      dst[i + dstStart].x = d.x / (srcB-srcA);
      dst[i + dstStart].y = d.y / (srcB-srcA);
    }
  }
  else {
    for(long i=0; i<dstSize; i++) {
      double f = i * srcSize / dstSize;
      long a = floor(f) + srcStart;
      f = fract(f);
      if(useCubicInterpolation) {
        long g = max(0, a-1);
        long b = min(srcSize-1, a+1);
        long h = min(srcSize-1, a+2);
        dst[i] = catmullRom(f, 0.5, src[g], src[a], src[b], src[h]);
      }
      else {
        long b = a+1;
        f = fract(f);
        if(b < srcSize) {
          dst[i+dstStart] = lerp(src[a], src[b], f);
        }
        else {
          dst[i+dstStart] = src[a];
        }
      }
    }
  }
}

static void resizeArrayBicubic(const std::vector<Vec2d> &src, std::vector<Vec2d> &dst) {
  resizeArray(src, dst, -1, -1, -1, -1, true);
}


template<class A>
static void normalizeArray(std::vector<A> &arr) {
  if(arr.size() < 1) return;
  A maxVal = arr[0];
  for(int i=1; i<arr.size(); i++) {
    if(arr[i] > maxVal) {
      maxVal = arr[i];
    }
  }
  for(int i=0; i<arr.size(); i++) {
    arr[i] /= maxVal;
  }
}

template<class A, class B>
static void scaleArray(std::vector<A> &arr, const B &t) {
  if(arr.size() < 1) return;
  for(int i=0; i<arr.size(); i++) {
    arr[i] *= t;
  }
}

template<class A, class B>
static void fillArray(std::vector<A> &arr, const B &t) {
  if(arr.size() < 1) return;
  for(int i=0; i<arr.size(); i++) {
    arr[i] = t;
  }
}
