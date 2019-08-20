#include <cstdio>
#include <string>
#include <locale>


int main()
{

  std::locale::global(std::locale(""));

  std::string str = std::to_string(3.1415926535);
  printf("%s\n", str.c_str());

  std::locale::global(std::locale("en_US.UTF8"));
  str = std::to_string(3.1415926535);
  printf("%s\n", str.c_str());


  return 0;
}
