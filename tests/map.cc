#include <unordered_map>
#include <cstdio>
#include <string>

int getClosest(int size, auto &smap) {
  int below = -1, above = 9999;

  for(const auto &keyValuePair : smap) {
    int key = keyValuePair.first;
    if(key == size) return key;
    if(key < size && key > below) below = key;
    if(key > size && key < above) above = key;
  }
  if(above != 9999) return above;
  if(below != -1) return below;
}

int main()
{
  std::unordered_map<int, std::string> smap;

  smap[10] = "rgtearg";
  smap[17] = "grsgesah";
  smap[32] = "43hytr";
  smap[65] = "trujh5r";

/*  for(const auto &keyValuePair : smap) {
    int key = keyValuePair.first;
    std::string value = keyValuePair.second;
    printf("%d -> %s\n", key, value.c_str());
  }*/

  for(int i=6; i<70; i++) {
    printf("%d, %d\n", i, getClosest(i, smap));
  }

  return 0;
}
