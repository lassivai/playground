#include <cstdio>
#include <cstring>

int main() {
  int n = 4;
  int bytesPerSample = 8;
  char *bytes = new char[n * bytesPerSample];

  for(int i=0; i<n; i++) {
    double t = 1.0/(i+1);
    memcpy(&bytes[i*bytesPerSample], &t, sizeof(double));
  }
  for(int i=0; i<n; i++) {
    double t = *((double*)(&bytes[i*bytesPerSample]));
    printf("%f\n", t);
  }

  delete bytes;
  return 0;
}
