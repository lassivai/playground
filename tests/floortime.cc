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

  std::vector<double> array = {1, 2, 3, 4, 5, 6, 7, 8};
  int n = array.size();

  std::vector<std::vector<double>> array2 = {{1, 2}, {3, 4}, {5, 6}, {7, 8}};
  std::vector<std::vector<double>> array3;
  int a = 1, b = 128;
;
  array3.resize(a);
  for(int i=0; i<a; i++) array3[i].assign(b, 123);

  double w = 0;
  auto t0 = std::chrono::high_resolution_clock::now();
  for(int i=0; i<reps; i++) {
//    w += array2[i%4][i%2];
      w += array3[i%a][i%b];
  }
  auto t1 = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(t1-t0);
  double dt = duration.count();

  printf("time per operation %e s\n", dt/reps);
}
