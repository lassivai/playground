#include <vector>
#include <cstdio>
#include <chrono>
#include <iostream>

using namespace std::chrono;

int main() {

  time_point<high_resolution_clock> tic, toc;

  int reps = 1;
  long initN = 10000;
  long extN = 1000;
  long extCount = 0;
  long totalN = 44100 * 60 * 60;

  std::vector<double> arr(initN);
  tic = high_resolution_clock::now();

  for(long l=0; l<totalN; l++) {
    if(l >= arr.size()) {
      arr.resize(arr.size() + extN);
      extCount++;
    }
    arr[l] = 123;
  }

  toc = high_resolution_clock::now();
  double dt = duration_cast<duration<double>>(toc-tic).count();

  std::vector<double> arr2(totalN);
  tic = high_resolution_clock::now();
  for(long l=0; l<totalN; l++) {
    arr2[l] = 123;
  }
  toc = high_resolution_clock::now();
  double dt0 = duration_cast<duration<double>>(toc-tic).count();

  unsigned long total = 0;
  for(long l=0; l<totalN; l++) {
    total += arr[l] + arr2[l];
  }


  printf("total n %lu, num re allocs %lu, dt %f, dt0 %f, dt/dt0 %f, avg %f\n", totalN, extCount, dt, dt0, dt/dt0, (double)total / (2.0 * totalN));

  return 0;
}
