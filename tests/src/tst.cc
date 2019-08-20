#include <cstdio>
#include "util.h"
#include "mathl.h"

int main()
{
  int n = 15;
  for(int i=0; i<n; i++) {
    float d = map(i, 0.0, n, -3.0*PI, 3.0*PI);
    float f = fract(d/(2.0*PI))*2.0*PI;
    float m = modff(d, 2.0*PI);
    printf("%.2f, %.2f, %.2f\n", d, f, m);
  }

}
