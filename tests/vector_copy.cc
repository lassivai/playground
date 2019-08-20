#include <cstdio>
#include <vector>
#include <string>

void print(std::string name, const std::vector<int> &v) {
  printf("%s ", name.c_str());
  for(int i=0; i<v.size(); i++) {
    printf("%d%s", v[i], i < v.size()-1 ? ", " : "\n");
  }
}

int main() {
  std::vector<int> a = {1, 2, 3, 4, 5};
  std::vector<int> b;
  b = a;
  print("a", a);
  print("b", b);
  a[0] = 56;
  a[2] = 945;
  print("a'", a);
  print("b'", b);

  return 0;
}
