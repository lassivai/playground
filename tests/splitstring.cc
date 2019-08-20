#include <string>
#include <vector>
#include <cstdio>

std::vector<std::string> split(std::string str) {
  std::vector<std::string> words;

  int i = 0;

  while(i < str.size()) {
    int j = str.find_first_of(" \n\t", i);
    if(j == std::string::npos) j = str.size();
    if(j-i > 0) {
      std::string word = str.substr(i, j-i);
      words.push_back(word);
    }
    i = j+1;
  }

  return words;
}


int main() {
  std::string str = "ewa\trfg  3k 45 kl\nm 4äm ä4 äemrf";
  std::vector<std::string> words = split(str);

  for(std::string &s : words) {
    printf("%s\n", s.c_str());
  }

  return 0;
}
