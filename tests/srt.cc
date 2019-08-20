#include <cstdio>
#include <vector>
#include "../src/engine/alg.h"
#include "../src/engine/util.h"

int main()
{
  std::vector<double> a;
  for(int i=0; i<10; i++) {
    a.push_back(Random::getDouble());
  }
  sort(a);

  for(int i=0; i<a.size(); i++) printf("%.4f ", a[i]);
  printf("\n");
}
