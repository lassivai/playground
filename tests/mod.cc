#include <cstdio>


int main() {
  int k = 5;
  for(int i=-20; i<21; i++) {
    printf("%d %% %d = %d\n", i, k, i%k);
  }
  for(int i=-20; i<21; i++) {
    printf("%d XXX %d = %d\n", i, k, (k+i%k)%k);
  }

}
