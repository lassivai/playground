#include <cstdio>
#include <ctgmath>

int main() {
  for(int i=-10; i<20; i++) {
    double a =  log(pow(10.0, i)) / log(10);
    printf("%f\n", a);
  }
  return 0;
}
