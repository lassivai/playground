#include "util.h"
#include <cstdio>
#include <string>
#include <cstring>

void writeTestData(char *filename) {
  TextFile file;
  file.openForWriting(filename);

  char str[5000];

  file.writeToFile("simple synth sequence\n");

  int numLoops = Random::getInt(4, 32);
  sprintf(str, "num loops %d\n", numLoops);
  file.writeToFile(str);

  int loopLength = Random::getInt(1, 8);
  sprintf(str, "loop duration %d\n", loopLength);
  file.writeToFile(str);

  int numRepeats = Random::getInt(1, 4);
  sprintf(str, "default num repeats %d\n", numRepeats);
  file.writeToFile(str);

  for(int i=0; i<numLoops; i++) {
    sprintf(str, "loop %d, num repeats %d\n", i, Random::getInt(1, 4));
    file.writeToFile(str);
    int numNotes = Random::getInt(5, 20);
    for(int k=0; k<numNotes; k++) {
      sprintf(str, "%f\t%f\t%f\n", round(Random::getFloat(20, 100)),
      Random::getFloat(0, 4), Random::getFloat(0, 1));
      file.writeToFile(str);
    }
  }
  file.finishWriting();
}


int readInt(std::string str, const char *start, const char *end, int &index)
{
  int i = 0, j = 0;
  int n = -1;
  i = str.find(start, index);
  if(i != -1) {
    j = str.find(end, i+strlen(start));
    //printf("%d, %d\n", i, j);
    //printf("%lu\n", strlen(str.substr(i+strlen(start), j-1).c_str()));
    try {
      n = std::stoi(str.substr(i+strlen(start), j-1));
    } catch(std::invalid_argument &e) {
      printf("Error at readInt: Can't read data between \'%s\' and \'%s\', exception: %s\n", start, end, e.what());
      n = -1;
    }
  }
  index = j;
  return n;
}
double readDouble(std::string str, const char *start, const char *end, int &index)
{
  int i = 0, j = 0;
  double n = -1;
  i = str.find(start, index);
  if(i != -1) {
    j = str.find(end, i+strlen(start));
    try {
      n = std::stod(str.substr(i+strlen(start), j-1));
    } catch(std::invalid_argument &e) {
      //printf("Error at readDouble: Can't read data between \'%s\' and \'%s\', exception: %s\n", start, end, e.what());
      n = -1;
    }
  }
  index = j;
  return n;
}
double readDouble(std::string str, const char *end, int &index)
{
  int j = 0;
  double n = -1;

  j = str.find(end, index);
  try {
    n = std::stod(str.substr(index, j-1));
  } catch(std::invalid_argument &e) {
    //printf("Error at readDouble: Can't read data between \'%s\' and \'%s\', exception: %s\n", start, end, e.what());
    n = -1;
  }
  index = j;
  return n;
}

void readTestData(const char *filename) {
  TextFile f;
  f.readContent(filename);
  if(f.status == File::CONTENT_READ) {
    //printf("testing...\n%s", f.content);
    std::string str(f.content);
    int index = 0, index2 = 0;
    int numLoops = readInt(str, "num loops", "\n", index);
    int defaultLoopDuration = readInt(str, "loop duration", "\n", index);
    int defaultNumRepeats = readInt(str, "default num repeats", "\n", index);

    while(true) {
      int loop = readInt(str, "loop", ",", index);
      index2 = str.find("loop", index) - 1;


      if(loop < 0) {
        break;
      }
      while(index < index2) {
        index++;
        float pitch = readDouble(str, "\t", index);
        index++;
        float time = readDouble(str, "\t", index);
        index++;
        float volume = readDouble(str, "\n", index);
        printf("%f - %f - %f\n", pitch, time, volume);
      }
    }
  }
}



int main(int argc, char *argv[]) {

  Random::init();

  if(argc > 2) {
    if(strcmp(argv[1], "read") == 0) {
      readTestData(argv[2]);
    }
    else if(strcmp(argv[1], "write") == 0) {
      writeTestData(argv[2]);
    }
  }
}
