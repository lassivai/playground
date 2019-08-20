#include <cstdio>

void printBits(unsigned int bits) {
  int n = sizeof(bits)*8;
  for(int i=n-1; i>=0; i--) {
    printf("%d%s", bits>>i & 1, i == 0 ? "\n" : (i % 8 == 0 ? " - " : " "));
  }
}


int main() {

  printBits(~15);

  printBits((255 & 15));
  printBits((255)/16);

  return 0;
}
