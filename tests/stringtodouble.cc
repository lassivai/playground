#include <string>
#include <cstdio>

int main()
{
  std::string a = "1.325", b = "r1.342w";

  double da, db;

  da = std::stod(a);

  try {
    db = std::stod(b);
  } catch(...) {
    printf("failed to convert\n");
  }

  printf("a = %f\nb = %f\n", da, db);

  return 0;
}
