#include <string>
#include <cstdio>

void compare(std::string a, std::string b) {
  printf("'%s' x '%s' = %d\n", a.c_str(), b.c_str(), a.compare(0, a.size(), b, 0, a.size()));
}

int main() {
  std::string a = "Launchpad Mini:Launchpad Mini MIDI 1 32:0";
  std::string a2 = "SWISSONIC EasyKeys61:SWISSONIC EasyKeys61 MIDI 1 28:0";
  std::string a3 = "Keystation 88:Keystation 88 MIDI 1 36:0";
  std::string b = "Launchpad Mini";
  std::string c = "Launchpad";

  compare(c, a);
  compare(c, a2);
  compare(c, a3);

  return 0;
}
