#include <cstdio>

int main()
{
  int a = (1<<31)-1, b = 1<<31, c = (1<<31)+1;
  int x = (1<<31)-2;
  printf("%d, %d, %d, %d\n", x, a, b, c);
  return 0;
}
