#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctgmath>
#include <vector>

int main(int argc, char *argv[])
{
  int reps = 1;
  for(int i=1; i<argc; i++) {
    if(strcmp(argv[i], "-reps") == 0 && i+1 < argc) {
       reps = atoi(argv[i+1]);
    }
  }
  int n = 1000000;
  std::vector<double> vec(n);
  for(int i=0; i<n; i++) {
    vec[i] = pow((double)i/n, 20.0);
  }


  auto t0 = std::chrono::high_resolution_clock::now();
  for(int i=0; i<reps; i++) {
    double s = 0;
    for(int j=0; j<n; j++) {
      //s += pow((double)j/n, 20.0);
      int q = j * n / 10000;
      if(q >= 0 && q < n) s += vec[q];
      else s += 0;
    }
  }
  auto t1 = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(t1-t0);
  double dt = duration.count();

  printf("time per operation %e s\n", dt/reps);
}
