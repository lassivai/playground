#include <string>
#include <cstdio>

void replace(std::string &str, std::string from, std::string to) {
  int i = 0;
  while((i = str.find(from, i)) != std::string::npos) {
    str.replace(i, from.size(), to);
  }
}

int main()
{
  std::string str = "testing<_>replacement<_>123<.>abc";
  replace(str, "<_>", " ");
  printf("%s\n", str.c_str());
  return 0;
}
