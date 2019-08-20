#include <cstdio>
#include <boost/filesystem.hpp>
#include <string>

void createDirectories(std::string str) {
  int i = 0;
  while(true) {
    int j = str.find_first_of("/", i);
    if(j < 0) break;

    std::string path = str.substr(0, j);
    if(path.length() < 2) break;
    i = j+1;

    if(!boost::filesystem::is_directory(path)) {
      boost::filesystem::create_directory(path);
      if(boost::filesystem::is_directory(path)) {
        printf("Directory \'%s\' created...\n", path.c_str());
      }
      else {
        return;
      }
    }
  }
}

int main()
{
  std::string str("output/test/img.png");

  createDirectories(str);
}
