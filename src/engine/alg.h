#pragma once
#include <cstring>

template <class T>
static void insertionSort(T *arr, int n) {
  for(int i=0; i<n; i++) {
    T a = arr[i];
    int j=i-1;
    for(; arr[j] > a && j >= 0; j--) {
      arr[j+1] = arr[j];
    }
    arr[j+1] = a;
  }
}

template <class T>
static void shellSort(T *arr, int n) {
  int gaps[] = { 1750, 701, 301, 132, 57, 23, 10, 4, 1 };
  for(int g=0; g<9; g++) {
    if(n/2 < gaps[g]) continue;
    for(int k=0; k<gaps[g]; k++) {
      for(int i=k; i<n; i+=gaps[g]) {
        T a = arr[i];
        int j = i-gaps[g];
        for(; j >= 0 && arr[j] > a; j-=gaps[g]) {
          arr[j+gaps[g]] = arr[j];
        }
        arr[j+gaps[g]] = a;
      }
    }
  }
}

template <class T>
static void merge(T *tmpArr, T *arr, int a, int b, int c) {
  memcpy(&tmpArr[a], &arr[a], (c-a)*sizeof(T));
  int iLeft = a, iRight = b;
  for(int i=a; i<c; i++) {
    if((tmpArr[iLeft] < tmpArr[iRight] && iLeft < b) || iRight == c) {
      arr[i] = tmpArr[iLeft];
      iLeft++;
    }
    else {
      arr[i] = tmpArr[iRight];
      iRight++;
    }
  }
}

template <class T>
static void split(T *tmpArr, T *arr, int a, int c) {
  int b = (a+c) / 2;
  if(c - a > 1) {
    split(tmpArr, arr, a, b);
    split(tmpArr, arr, b, c);
    merge(tmpArr, arr, a, b, c);
  }
}

template <class T>
inline void mergeSort(T *arr, int n) {
  T *tmpArr = new T[n];

  split(tmpArr, arr, 0, n);

  delete tmpArr;
}


template <class T, class P>
static void insertionSort(std::vector<T> &arr, std::vector<P> &arr2) {
  int n = arr.size();
  for(int i=0; i<n; i++) {
    T a = arr[i];
    P b = arr2[i];
    int j=i-1;
    for(; arr[j] > a && j >= 0; j--) {
      arr[j+1] = arr[j];
      arr2[j+1] = arr2[j];
    }
    arr[j+1] = a;
    arr2[j+1] = b;
  }
}

template <class T>
static void insertionSort(std::vector<T> &arr) {
  int n = arr.size();
  for(int i=0; i<n; i++) {
    T a = arr[i];
    int j=i-1;
    for(; a < arr[j] && j >= 0; j--) {
      arr[j+1] = arr[j];
    }
    arr[j+1] = a;
  }
}

template <class T>
static void shellSort(std::vector<T> &arr) {
  int gaps[] = { 1750, 701, 301, 132, 57, 23, 10, 4, 1 };
  int n = arr.size();
  for(int g=0; g<9; g++) {
    if(n/2 < gaps[g]) continue;
    for(int k=0; k<gaps[g]; k++) {
      for(int i=k; i<n; i+=gaps[g]) {
        T a = arr[i];
        int j = i-gaps[g];
        for(; j >= 0 && a < arr[j]; j-=gaps[g]) {
          arr[j+gaps[g]] = arr[j];
        }
        arr[j+gaps[g]] = a;
      }
    }
  }
}


template <class T>
static void merge(std::vector<T> &tmpArr, std::vector<T> &arr, int a, int b, int c) {
  memcpy(&tmpArr[a], &arr[a], (c-a)*sizeof(T));
  int iLeft = a, iRight = b;
  for(int i=a; i<c; i++) {
    if((tmpArr[iLeft] < tmpArr[iRight] && iLeft < b) || iRight == c) {
      arr[i] = tmpArr[iLeft];
      iLeft++;
    }
    else {
      arr[i] = tmpArr[iRight];
      iRight++;
    }
  }
}

template <class T>
static void split(std::vector<T> &tmpArr, std::vector<T> &arr, int a, int c) {
  int b = (a+c) / 2;
  if(c - a > 1) {
    split(tmpArr, arr, a, b);
    split(tmpArr, arr, b, c);
    merge(tmpArr, arr, a, b, c);
  }
}

template <class T>
inline void mergeSort(std::vector<T> &arr) {
  int n = arr.size();
  std::vector<T> tmpArr(n);
  split(tmpArr, arr, 0, n);
}

template <class T>
inline void mergeSort(std::vector<T> &arr, int n) {
  std::vector<T> tmpArr(n);
  split(tmpArr, arr, 0, n);
}


template <class T>
inline void sort(T *arr, int n) {
  if(n < 25) insertionSort(arr, n);
  else if(n < 300) shellSort(arr, n);
  else mergeSort(arr, n);
}

template <class T>
inline void sort(std::vector<T> arr) {
  int n = arr.size();
  if(n < 25) insertionSort(arr);
  else if(n < 300) shellSort(arr);
  else mergeSort(arr, n);
}
/*template <class T>
inline void sort(std::vector<T> &arr) {
  mergeSort(arr);
}*/





/*
template<class T>
struct Sortable {
  virtual T getValue() = 0;
};


template <class T>
void shellSort(std::vector<Sortable<T>> &arr) {
  int gaps[] = { 1750, 701, 301, 132, 57, 23, 10, 4, 1 };
  int n = arr.size();
  for(int g=0; g<9; g++) {
    if(n/2 < gaps[g]) continue;
    for(int k=0; k<gaps[g]; k++) {
      for(int i=k; i<n; i+=gaps[g]) {
        T a = arr[i].getValue();
        int j = i-gaps[g];
        for(; j >= 0 && arr[j].getValue() > a; j-=gaps[g]) {
          arr[j+gaps[g]] = arr[j];
        }
        arr[j+gaps[g]] = a;
      }
    }
  }
}

template <class T>
inline void sort(std::vector<Sortable<T>> &arr) {
  shellSort(arr);
}*/
