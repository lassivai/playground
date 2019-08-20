#include <cstdio>
#include <cstdlib>
#include <string>


int main() {

  std::string str = "1 345  5  678,901,    234 ,567";
  char *endPtr = NULL;
  int ind = 0;
  do {
    endPtr = NULL;
    double d = strtod(&str[ind], &endPtr);
    printf("%f, %p, %p, %lu, %d\n", d, (void*)&str[ind], (void*)endPtr, endPtr - &str[ind], ind);
    if(endPtr == &str[ind]) break;
    if(endPtr) {
       ind += endPtr - &str[ind];
    }
  } while(endPtr);

  return 0;
}
