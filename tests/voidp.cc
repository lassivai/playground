#include <cstdio>
#include <string>

void fillValue(void *variable, std::string value,  std::string type) {
  if(type.compare("string") == 0) {
    std::string *str = static_cast<std::string*>(variable);
    *str = value;
  }
  if(type.compare("int") == 0) {
    int *i = static_cast<int *>(variable);
    *i = std::stoi(value);
  }
  if(type.compare("double") == 0) {
    double *d = static_cast<double *>(variable);
    *d = std::stod(value);
  }
}


int main()
{
  std::string A = "45ty6ujkm", B = "3546", C = "345.463";
  std::string a = "";
  int b = 0;
  double c = 0;

  fillValue(&a, A, "string");
  fillValue(&b, B, "int");
  fillValue(&c, C, "double");

  printf("a = %s\nb = %d\nc = %f\n", a.c_str(), b, c);

  return 0;
}
