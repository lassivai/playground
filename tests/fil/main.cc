#include "file.h"
#include <string>
#include <vector>
#include <cstdio>


static void printIntendatedString(const std::string &str, int level, int numSpaces = 2) {
  for(int i=0; i<level*numSpaces; i++) {
    printf(" ");
  }
  printf("%s\n", str.c_str());
}



int main(int argc, char **argv)
{
    if(argc < 2) return 0;
    TextFile file;
    file.readContent(argv[1]);
    int ind = 0;
    int nameStartInd = -1;
    int nameEndInd = 0;

    int blockLevel = 0; // for testing
    enum ReadingValues {Not, Yes, DoubleValues, StringValues};
    ReadingValues readingValues = Not;
    std::vector<double> doubleValues;
    std::vector<std::string> stringValues;
    std::string variableName;

    for(int ind = 0; ind < file.content.size(); ind++) {
        char c = file.content[ind];
        switch(c) {
            case ' ':
                break;
            case ',': {
                if(readingValues == StringValues && nameStartInd > 0 && nameEndInd+1 > nameStartInd) {
                    stringValues.push_back(file.content.substr(nameStartInd, nameEndInd - nameStartInd + 1));
                    nameStartInd = -1;
                }
            }   break;

            case '\n': {
                if(readingValues == StringValues && nameStartInd > 0 && nameEndInd+1 > nameStartInd) {
                    stringValues.push_back(file.content.substr(nameStartInd, nameEndInd - nameStartInd + 1));
                    nameStartInd = -1;
                    std::string str = ": ";
                    for(int i=0; i<stringValues.size(); i++) {
                        str += stringValues[i];
                        if(i < stringValues.size()-1) str += ", ";
                    }
                    printIntendatedString(variableName + str, blockLevel);
                }
                if(readingValues == DoubleValues) {
                    std::string str = ": ";
                    for(int i=0; i<doubleValues.size(); i++) {
                        str += std::to_string(doubleValues[i]);
                        if(i < doubleValues.size()-1) str += ", ";
                    }
                    printIntendatedString(variableName + str, blockLevel);
                }
                readingValues = Not;
            }   break;

            case '1':case '2':case '3':case '4':case '5':case '6':case '7':case '8':case '9':case '0':case '.':case '-': {
                if(readingValues == Yes) readingValues = DoubleValues;
                if(readingValues == DoubleValues) {
                    char *endPtr = NULL;
                    double d = strtod(&file.content[ind], &endPtr);
                    if(endPtr && endPtr - &file.content[ind] > 0) {
                        ind += endPtr - &file.content[ind] - 1;
                        doubleValues.push_back(d);
                    }
                }
                if(readingValues == StringValues) {
                    nameEndInd = ind;
                }
            }   break;

            case ':': {
                variableName = file.content.substr(nameStartInd, nameEndInd - nameStartInd + 1);
                nameStartInd = -1;
                readingValues = Yes;
                doubleValues.clear();
                stringValues.clear();
            }   break;

            case '{': {
                std::string blockName = file.content.substr(nameStartInd, nameEndInd - nameStartInd + 1);
                nameStartInd = -1;
                printIntendatedString(blockName + " {", blockLevel);
                blockLevel++;
            }   break;

            case '}':
                blockLevel--;
                printIntendatedString("}", blockLevel);
                break;

            default: {
                if(nameStartInd == -1) nameStartInd = ind;
                nameEndInd = ind;
                if(readingValues == Yes) readingValues = StringValues;
                if(readingValues == StringValues) {
                    
                }
            }   break;
        }
    }
    
    return 0;
}



