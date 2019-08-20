#include <string>
#include <cstdio>

std::string toString(double a) {
  char buf[128];
  int n = std::sprintf(buf, "%.9f", a);
  bool d = false;
  int l = 1;
  for(int i=0; i<n; i++) {
    if(!d) {
      if(buf[i] == '.' || buf[i] == ',') {
        d = true;
        l = i-1;
      }
    }
    else {
      if(buf[i] != '0') {
        l = i;
      }
    }
  }
  buf[l+1] = '\0';
  return std::string(buf);
}

int main() {

  printf("%s\n", toString(1).c_str());
  printf("%s\n", toString(0).c_str());
  printf("%s\n", toString(123).c_str());
  printf("%s\n", toString(123.12).c_str());
  printf("%s\n", toString(123.102).c_str());
  printf("%s\n", toString(102.1003).c_str());
  printf("%s\n", toString(0.1).c_str());
  printf("%s\n", toString(0.100000001).c_str());
  printf("%s\n", toString(0.000000001).c_str());
  printf("%s\n", toString(1.000000001).c_str());
  printf("%s\n", toString(111000).c_str());
  printf("%s\n", toString(111000.00001).c_str());

  return 0;
}
