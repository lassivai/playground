#include <cstring>
#include <cstdio>

int main()
{
  int n = 20;
  float *arr = new float[n];
  for(int i=0; i<n; i++) {
   arr[i] = i;
   printf("%.0f ", arr[i]);
  }
  printf("\n");

  memmove(&arr[15], &arr[0], 5 * sizeof(float));

  for(int i=0; i<n; i++) {
   printf("%.0f ", arr[i]);
  }
  printf("\n");
 

  return 0;
}
