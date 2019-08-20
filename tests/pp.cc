#include <cstdio>

void f(double **p) {
  p[0][0] *= 5;
  p[1][0] += 123;
}

int main() {

  //double *arr[2] = new double[2];
  double **arr = new double*[2] {new double[1], new double[1]};
//  arr[0] = new double[1];
//  arr[1] = new double[1];
  arr[0][0] = 10;
  arr[1][0] = 1000;
  f(arr);
  printf("%f, %f\n", arr[0][0], arr[1][0]);


  return 0;
}
