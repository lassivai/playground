#include <cereal/archives/binary.hpp>
#include <cereal/types/memory.hpp>
//#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/unordered_map.hpp>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <unordered_map>

struct Thing {
  std::string name;
  double x, y, z;

  Thing() {}
  Thing(std::string name, double x, double y, double z) {
    this->name = name;
    this->x = x;
    this->y = y;
    this->z = z;
  }

  template <class Archive>
  void save(Archive &ar) const
  {
    ar(name, x, y);
  }

  template <class Archive>
  void load( Archive & ar )
  {
    ar(name, x, y);
    z = 123;
  }

};
std::ostream& operator<<(std::ostream &os, const Thing &t) {
    os << "name: " << t.name << ", x: " << t.x << ", y: " << t.y << ", z: " << t.z;
    return os;
}

struct TestClass
{
  int a;
  double b;
  char c[10];
  std::string str;
  std::vector<std::string> vec;
  std::unordered_map<int, Thing> umap;

  template<class Archive>
  void serialize(Archive &ar) {
    ar(a, b, c, str, vec, umap);
  }

  /*template <class Archive>
  void save( Archive & ar ) const
  {
    ar( data );
  }

  template <class Archive>
  void load( Archive & ar )
  {
    static int32_t idGen = 0;
    id = idGen++;
    ar( data );
  }*/

};

template<class T>
int saveCerealBinary(T &t, const char *filename) {
  std::ofstream os(filename, std::ios::binary);
  cereal::BinaryOutputArchive archive(os);
  archive(t);
}

template<class T>
int loadCerealBinary(T &t, const char *filename) {
  std::ifstream os(filename, std::ios::binary);
  cereal::BinaryInputArchive archive(os);
  archive(t);
}


int main()
{
  TestClass tstOut = {87654, 354.46363, "abcdefghi", "ewsfgeh"};
  tstOut.vec.push_back("3243fsdg45");
  tstOut.vec.push_back("yhh65i856");
  tstOut.vec.push_back("537ujgte");
  tstOut.umap[587] = Thing("rwer", 24, 56, 42);
  tstOut.umap[32456] = Thing("657fr", 64, 7876, 85.654367);
  tstOut.umap[43] = Thing("r5367", 35.624, 5664, 4657);
  saveCerealBinary(tstOut, "tst.bin");

  TestClass tstIn;
  loadCerealBinary(tstIn, "tst.bin");

  std::cout << "a: " << tstIn.a << "\n";
  std::cout << "b: " << tstIn.b << "\n";
  std::cout << "c: " << tstIn.c << "\n";
  std::cout << "str: " << tstIn.str << "\n";
  for(std::string s : tstIn.vec) {
    std::cout << "vec: " << s << "\n";
  }
  for(auto &intThingPair : tstIn.umap) {
    std::cout << "umap: " << intThingPair.first << " -> " << intThingPair.second << "\n";
  }

  return 0;
}
