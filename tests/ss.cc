#include <cstdio>
#include <string>

int main() {

  std::string str = "Code 61:Code 61 MIDI 1 36:123";
  std::string id = str.substr(str.find_last_of(':')+1);
  str = str.substr(0, str.find_last_of(':'));
  printf("%s\n", str.c_str());
  printf("%s\n", id.c_str());

  return 0;
}
