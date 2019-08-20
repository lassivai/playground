#pragma once
#include "util.h"
#include <vector>
#include <unordered_map>
#include <type_traits>
#include <typeinfo>
#include "mathl.h"

struct HierarchicalTextFileParser
{
  enum ReadingValues {Not, Yes, ReadingDoubleParameter, ReadingStringParameter};
  //enum ParameterType {DoubleParameter, StringParameter};
  
protected:
  int nameStartInd = -1;
  int nameEndInd = 0;
  
  int blockLevel = 0; // for testing
  ReadingValues readingValues = Not;
  //std::vector<double> doubleValues;
  //std::vector<std::string> stringValues;
  std::string parameterName;
  std::string *content = NULL;
  
  int blockStartInd = 0;
  int blockEndInd = 0;
  
public:
  
  //std::string fileExtension;
  virtual std::string getFileExtension() {
    return "";
  }
  
  std::vector<DoubleParameter> doubleParameters;
  std::vector<StringParameter> stringParameters;
  std::unordered_map<std::string, int> doubleParameterIndices, stringParameterIndices;
  
  virtual ~HierarchicalTextFileParser() {}
  
  HierarchicalTextFileParser() {
    //blockName = getBlockName();
  }
  
  
  // return the block size
  virtual int onNewBlock(const std::string &blockName, int blockStartInd, int blockLevel) {
    return blockEndInd - blockStartInd;
  }
  
  int parse(std::string *content, int blockStartInd, int blockLevel) {
    this->content = content;
    this->blockStartInd = blockStartInd;
    this->blockLevel = blockLevel;
    blockEndInd = content->size();
    
    return parse();
  }
  
  int parse() {
    doubleParameters.clear();
    stringParameters.clear();
    doubleParameterIndices.clear();
    stringParameterIndices.clear();
    onParse();
    //printf("(debugging) at HierarchicalTextFileParser::parse(), 0, size = %d, start = %d, end = %d, nameStartInd = %d, nameEndInd = %d, reading = %d\n", (int)content->size(), blockStartInd, blockEndInd, nameStartInd, nameEndInd, (int)readingValues);
    
    for(int ind = blockStartInd; ind < blockEndInd; ind++) {
      char c = content->at(ind);
      switch(c) {
        case ' ':
          break;
        case ',': {
          //printf("(debugging) at HierarchicalTextFileParser::parse(), 1, ind = %d, size = %d, start = %d, end = %d, nameStartInd = %d, nameEndInd = %d, reading = %d\n", ind, (int)content->size(), blockStartInd, blockEndInd, nameStartInd, nameEndInd, (int)readingValues);
          if(readingValues == ReadingStringParameter && nameStartInd > 0 && nameEndInd+1 > nameStartInd) {
            stringParameters[stringParameters.size()-1].values.push_back(content->substr(nameStartInd, nameEndInd - nameStartInd + 1));
            nameStartInd = -1;
          }
          //printf("(debugging) at HierarchicalTextFileParser::parse(), 2, ind = %d, size = %d, start = %d, end = %d, nameStartInd = %d, nameEndInd = %d, reading = %d\n", ind, (int)content->size(), blockStartInd, blockEndInd, nameStartInd, nameEndInd, (int)readingValues);
        }   break;
        
        case '\n': {
          //printf("(debugging) at HierarchicalTextFileParser::parse(), 3, ind = %d, size = %d, start = %d, end = %d, nameStartInd = %d, nameEndInd = %d, reading = %d\n", ind, (int)content->size(), blockStartInd, blockEndInd, nameStartInd, nameEndInd, (int)readingValues);
          if(readingValues == ReadingStringParameter && nameStartInd > 0 && nameEndInd+1 > nameStartInd) {
            stringParameters[stringParameters.size()-1].values.push_back(content->substr(nameStartInd, nameEndInd - nameStartInd + 1));
            nameStartInd = -1;
            /*std::string str = ": ";
            for(int i=0; i<stringParameters[stringParameters.size()-1].values.size(); i++) {
              str += stringParameters[stringParameters.size()-1].values[i];
              if(i < stringParameters[stringParameters.size()-1].values.size()-1) str += ", ";
            }*/
            //printf("(debugging) at HierarchicalTextFileParser::parse(), 4, ind = %d, size = %d, start = %d, end = %d, nameStartInd = %d, nameEndInd = %d, reading = %d\n", ind, (int)content->size(), blockStartInd, blockEndInd, nameStartInd, nameEndInd, (int)readingValues);
            //printIntendatedString(parameterName + str, blockLevel); // for debugging
            //onParameterDecoded(parameterName, stringValues);
          }
          /*if(readingValues == ReadingDoubleParameter) {
            //printf("(debugging) at HierarchicalTextFileParser::parse(), 5, ind = %d, size = %d, start = %d, end = %d, nameStartInd = %d, nameEndInd = %d, reading = %d\n", ind, (int)content->size(), blockStartInd, blockEndInd, nameStartInd, nameEndInd, (int)readingValues);
            std::string str = ": ";
            for(int i=0; i<doubleParameters[doubleParameters.size()-1].values.size(); i++) {
              str += std::to_string(doubleParameters[doubleParameters.size()-1].values[i]);
              if(i < doubleParameters[doubleParameters.size()-1].values.size()-1) str += ", ";
            }
            //printf("(debugging) at HierarchicalTextFileParser::parse(), 6, ind = %d, size = %d, start = %d, end = %d, nameStartInd = %d, nameEndInd = %d, reading = %d\n", ind, (int)content->size(), blockStartInd, blockEndInd, nameStartInd, nameEndInd, (int)readingValues);
            //printIntendatedString(parameterName + str, blockLevel); // for debugging
            //onParameterDecoded(parameterName, doubleValues);
          }*/
          readingValues = Not;
        }   break;
        
        case '1':case '2':case '3':case '4':case '5':case '6':case '7':case '8':case '9':case '0':case '.':case '-': {
          //printf("(debugging) at HierarchicalTextFileParser::parse(), 7, ind = %d, size = %d, start = %d, end = %d, nameStartInd = %d, nameEndInd = %d, reading = %d\n", ind, (int)content->size(), blockStartInd, blockEndInd, nameStartInd, nameEndInd, (int)readingValues);
          if(readingValues == Yes) {
            readingValues = ReadingDoubleParameter;
            doubleParameters.push_back(DoubleParameter(parameterName));
            doubleParameterIndices[parameterName] = doubleParameters.size() - 1;
          }
          if(readingValues == ReadingDoubleParameter) {
            char *endPtr = NULL;
            double d = strtod(&content->at(ind), &endPtr);
            if(endPtr && endPtr - &content->at(ind) > 0) {
              ind += endPtr - &content->at(ind) - 1;
              doubleParameters[doubleParameters.size()-1].values.push_back(d);
            }
          }
          //if(readingValues == ReadingStringParameter) {
          else {
            nameEndInd = ind;
          }
          //printf("(debugging) at HierarchicalTextFileParser::parse(), 8, ind = %d, size = %d, start = %d, end = %d, nameStartInd = %d, nameEndInd = %d, reading = %d\n", ind, (int)content->size(), blockStartInd, blockEndInd, nameStartInd, nameEndInd, (int)readingValues);
        }   break;
        
        case ':': {
          //printf("(debugging) at HierarchicalTextFileParser::parse(), 9, ind = %d, size = %d, start = %d, end = %d, nameStartInd = %d, nameEndInd = %d, reading = %d\n", ind, (int)content->size(), blockStartInd, blockEndInd, nameStartInd, nameEndInd, (int)readingValues);
          parameterName = content->substr(nameStartInd, nameEndInd - nameStartInd + 1);
          nameStartInd = -1;
          readingValues = Yes;
          //printf("(debugging) at HierarchicalTextFileParser::parse(), 10, ind = %d, size = %d, start = %d, end = %d, nameStartInd = %d, nameEndInd = %d, reading = %d\n", ind, (int)content->size(), blockStartInd, blockEndInd, nameStartInd, nameEndInd, (int)readingValues);
        }   break;
        
        case '{': {
          //printf("(debugging) at HierarchicalTextFileParser::parse(), 11, ind = %d, size = %d, start = %d, end = %d, nameStartInd = %d, nameEndInd = %d, reading = %d\n", ind, (int)content->size(), blockStartInd, blockEndInd, nameStartInd, nameEndInd, (int)readingValues);
          if(nameStartInd < 0 || nameEndInd - nameStartInd + 1 < 1) {
            printf("Error at HierarchicalTextFileParser::parse(), unnamed block\n");
          }
          ind += onNewBlock(content->substr(nameStartInd, nameEndInd - nameStartInd + 1), ind+1, blockLevel) + 1;
          nameStartInd = -1;
          //printIntendatedString(blockName + " {", blockLevel); // for debugging
          blockLevel++;
          //printf("(debugging) at HierarchicalTextFileParser::parse(), 12, ind = %d, size = %d, start = %d, end = %d, nameStartInd = %d, nameEndInd = %d, reading = %d\n", ind, (int)content->size(), blockStartInd, blockEndInd, nameStartInd, nameEndInd, (int)readingValues);
        }   break;
        
        case '}':
        blockLevel--;
        blockEndInd = ind;
        //printIntendatedString("}", blockLevel); // for debugging
        break;
        
        default: {
          //printf("(debugging) at HierarchicalTextFileParser::parse(), 13, ind = %d, size = %d, start = %d, end = %d, nameStartInd = %d, nameEndInd = %d, reading = %d\n", ind, (int)content->size(), blockStartInd, blockEndInd, nameStartInd, nameEndInd, (int)readingValues);
          if(nameStartInd == -1) nameStartInd = ind;
          nameEndInd = ind;
          if(readingValues == Yes) {
            readingValues = ReadingStringParameter;
            stringParameters.push_back(StringParameter(parameterName));
            stringParameterIndices[parameterName] = stringParameters.size() - 1;
          }
          if(readingValues == ReadingStringParameter) {
            
          }
          //printf("(debugging) at HierarchicalTextFileParser::parse(), 14, ind = %d, size = %d, start = %d, end = %d, nameStartInd = %d, nameEndInd = %d, reading = %d\n", ind, (int)content->size(), blockStartInd, blockEndInd, nameStartInd, nameEndInd, (int)readingValues);
        }   break;
      }
    }
    onParsed();
    decodeParameters();
    
    return blockEndInd - blockStartInd;
  }
  
  virtual void onParsed() {}
  
  virtual void decodeParameters() {}
  virtual void encodeParameters() {}

  
  //std::string blockName;
  
  std::string filename = "";
  std::string fileContent = "";
  
  virtual std::string getDefaultFileDirectory() {
    return "";
  }
    
  virtual std::string getBlockName() {
    return "unnamed";
  }
  
  virtual void onParse() {}
  
  bool open(const std::string &filename) {
    this->filename = filename;
    
    if(!TextFile::Read(filename, fileContent)) {
      printf("Warning at HierarchicalTextFileParser.parse(): Couldn't open file '%s'\n", filename.c_str());
      return false;
    }
    
    if(fileContent.size() == 0) {
      printf("Warning at HierarchicalTextFileParser.parse(): No content in file '%s'\n", filename.c_str());
      return false;
    }
    
    int numOpeningCurlyBrackets = 0;
    int numClosingCurlyBrackets = 0;
    for(int i=0; i<fileContent.size(); i++) {
      if(fileContent.at(i) == '{') numOpeningCurlyBrackets++;
      if(fileContent.at(i) == '}') numClosingCurlyBrackets++;
    }
    if(numOpeningCurlyBrackets != numClosingCurlyBrackets) {
      printf("Warning at HierarchicalTextFileParser.parse(): Mismatching number of opening and closing brackets in file '%s'\n", filename.c_str());
      return false;
    }
    
    parse(&fileContent, 0, 0);

    return true;
  }

  void clearParameters() {
    doubleParameters.clear();
    stringParameters.clear();
  }

  bool save(const std::string &filename) {
    this->filename = filename;
    encodeParameters();
    fileContent = "";
    printParameters(fileContent);
    if(TextFile::Write(filename, fileContent)) {
      return true;
    }
    return false;
  }

  virtual void printToTerminal(int level = 0) {
    printIntendation(level);
    printf("%s {\n", getBlockName().c_str());
    for(int i=0; i<stringParameters.size(); i++) {
      stringParameters[i].print(1);
    }
    for(int i=0; i<doubleParameters.size(); i++) {
      doubleParameters[i].print(1);
    }
    printIntendation(level);
    printf("}\n");
  }

  virtual void printParameters(std::string &content, int level = 0) {
    printIntendation(content, level);
    content += getBlockName() + " {\n";
    for(int i=0; i<stringParameters.size(); i++) {
      stringParameters[i].print(content, level+1);
    }
    for(int i=0; i<doubleParameters.size(); i++) {
      doubleParameters[i].print(content, level+1);
    }
    onPrintParameters(content, level+1);
    printIntendation(content, level);
    content += "}\n";
  }

  virtual void onPrintParameters(std::string &content, int level = 0) {}

  /*virtual int onNewBlock(const std::string &blockName, int blockStartInd, int blockLevel) {
    if(blockName == "effect") {
      effectStrings.push_back(new EffectString());
      return effectStrings[effectStrings.size()-1]->parse(content, blockStartInd, blockLevel);
    }
    return 0;
  }*/

  /*bool isMatchingDoubleParameter(int index, const std::string &name, int minAmountOfValues) {
    return index >= 0 && index < doubleParameters.size() && doubleParameters[index].values.size() >= minAmountOfValues && doubleParameters[index].name.compare(name) == 0;
  }
  bool isMatchingStringParameter(int index, const std::string &name, int minAmountOfValues) {
    return index >= 0 && index < stringParameters.size() && stringParameters[index].values.size() >= minAmountOfValues && stringParameters[index].name.compare(name) == 0;
  }*/
  
  void putStringParameter(const std::string &name, const std::string &value) {
    stringParameters.push_back(StringParameter(name, value));
  }
  void putStringParameter(const std::string &name, const std::vector<std::string> &values, unsigned int maxCount = -1) {
    stringParameters.push_back(StringParameter(name, values, maxCount));
  }
  template<class T>
  void putNumericParameter(const std::string &name, T value) {
    if(!std::is_fundamental<T>::value) {
      printf("Error at HierarchicalTextFileParser::putNumericParameter(), block '%s', name '%s', invalid numeric type\n", getBlockName().c_str(), name.c_str());
      return;
    }
    doubleParameters.push_back(DoubleParameter(name, value));
  }
  /*template<class T>
  void putNumericParameter(const std::string &name, const std::vector<T> &values) {
    if(!std::is_fundamental<T>::value) {
      printf("Error at HierarchicalTextFileParser::putNumericParameter(), block '%s', name '%s', invalid numeric type\n", blockName.c_str(), name.c_str());
      return;
    }
    doubleParameters.push_back(DoubleParameter(name, values));
  }*/

  void putNumericParameter(const std::string &name, const std::vector<double> &values, unsigned int maxCount = -1) {
    doubleParameters.push_back(DoubleParameter(name, values, maxCount));
  }
  void putVectorParameter(const std::string &name, const Vec2d &value) {
    doubleParameters.push_back(DoubleParameter(name, {value.x, value.y}));
  }
  void putVectorParameter(const std::string &name, const std::vector<Vec2d> &values, unsigned int maxCount = -1) {
    doubleParameters.push_back(DoubleParameter(name));
    for(int i=0; i<values.size() && i < maxCount; i++) {
      doubleParameters[doubleParameters.size()-1].values.push_back(values[i].x);
      doubleParameters[doubleParameters.size()-1].values.push_back(values[i].y);
    }
  }
  void putVectorParameter(const std::string &name, const Vec3d &value) {
    doubleParameters.push_back(DoubleParameter(name, {value.x, value.y, value.z}));
  }
  void putVectorParameter(const std::string &name, const std::vector<Vec3d> &values, unsigned int maxCount = -1) {
    doubleParameters.push_back(DoubleParameter(name));
    for(int i=0; i<values.size() && i < maxCount; i++) {
      doubleParameters[doubleParameters.size()-1].values.push_back(values[i].x);
      doubleParameters[doubleParameters.size()-1].values.push_back(values[i].y);
      doubleParameters[doubleParameters.size()-1].values.push_back(values[i].z);
    }
  }
  void putVectorParameter(const std::string &name, const CurvePoint &value) {
    doubleParameters.push_back(DoubleParameter(name, {value.x, value.y, value.cubicInterpolationCurvature}));
  }
  void putVectorParameter(const std::string &name, const std::vector<CurvePoint> &values, unsigned int maxCount = -1) {
    doubleParameters.push_back(DoubleParameter(name));
    for(int i=0; i<values.size() && i < maxCount; i++) {
      doubleParameters[doubleParameters.size()-1].values.push_back(values[i].x);
      doubleParameters[doubleParameters.size()-1].values.push_back(values[i].y);
      doubleParameters[doubleParameters.size()-1].values.push_back(values[i].cubicInterpolationCurvature);
    }
  }
  void putVectorParameter(const std::string &name, const Vec4d &value) {
    doubleParameters.push_back(DoubleParameter(name, {value.x, value.y, value.z, value.w}));
  }
  void putVectorParameter(const std::string &name, const std::vector<Vec4d> &values, unsigned int maxCount = -1) {
    doubleParameters.push_back(DoubleParameter(name));
    for(int i=0; i<values.size() && i < maxCount; i++) {
      doubleParameters[doubleParameters.size()-1].values.push_back(values[i].x);
      doubleParameters[doubleParameters.size()-1].values.push_back(values[i].y);
      doubleParameters[doubleParameters.size()-1].values.push_back(values[i].z);
      doubleParameters[doubleParameters.size()-1].values.push_back(values[i].w);
    }
  }

  
  int getStringParameter(const std::string &name, std::string &value) {
    if(stringParameterIndices.count(name) > 0) {
      int i = stringParameterIndices[name];
      int n = stringParameters[i].values.size();
      if(i >= 0 && i < stringParameters.size() && n > 0) {
        value = stringParameters[i].values[0];
        return n;
      }
    }
    return 0;
  }
  
  int getStringParameter(const std::string &name, std::vector<std::string> &values) {
    if(stringParameterIndices.count(name) > 0) {
      int i = stringParameterIndices[name];
      int n = stringParameters[i].values.size();
      if(i >= 0 && i < stringParameters.size() && n > 0) {
        values.resize(n);
        for(int k=0; k<n; k++) {
          values[k] = stringParameters[i].values[k];
        }
        return n;
      }
    }
    return 0;
  }
  
  
  template<class T>
  int getNumericParameter(const std::string &name, T &value, int index = 0) {
    if(!std::is_fundamental<T>::value) {
      printf("Error at HierarchicalTextFileParser::getNumericParameter(), block '%s', name '%s', invalid numeric type\n", getBlockName().c_str(), name.c_str());
      return 0;
    }
    if(doubleParameterIndices.count(name) > 0) {
      int i = doubleParameterIndices[name];
      int n = doubleParameters[i].values.size();
      if(i >= 0 && i < doubleParameters.size() && n > index) {
        value = (T)doubleParameters[i].values[index];
        return n;
      }
    }
    return 0;
  }
  
  template<class T>
  int getNumericParameter(const std::string &name, std::vector<T> &values, bool allowResize = true) {
    if(!std::is_fundamental<T>::value) {
      printf("Error at HierarchicalTextFileParser::getNumericParameter(), block '%s', name '%s', invalid numeric type\n", getBlockName().c_str(), name.c_str());
      return 0;
    }
    if(doubleParameterIndices.count(name) > 0) {
      int i = doubleParameterIndices[name];
      int n = allowResize ? doubleParameters[i].values.size() : min(values.size(), doubleParameters[i].values.size());
      if(i >= 0 && i < doubleParameters.size() && n > 0) {
        if(allowResize) {
          values.resize(n);
        }
        for(int k=0; k<n; k++) {
          values[k] = (T)doubleParameters[i].values[k];
        }
        return n;
      }
    }
    return 0;
  }

  int getVectorParameter(const std::string &name, Vec2d &value) {
    if(doubleParameterIndices.count(name) > 0) {
      int i = doubleParameterIndices[name];
      int n = doubleParameters[i].values.size();
      if(i >= 0 && i < doubleParameters.size() && n > 1) {
        value.set(doubleParameters[i].values[0], doubleParameters[i].values[1]);
        return n;
      }
    }
    return 0;
  }
  
  int getVectorParameter(const std::string &name, std::vector<Vec2d> &values) {
    if(doubleParameterIndices.count(name) > 0) {
      int i = doubleParameterIndices[name];
      int n = doubleParameters[i].values.size();
      if(i >= 0 && i < doubleParameters.size() && n > 1) {
        values.resize(n/2);
        for(int k=0; k<n/2; k++) {
          values[k].set(doubleParameters[i].values[k*2], doubleParameters[i].values[k*2+1]);
        }
        return n;
      }
    }
    return 0;
  }
  
  int getVectorParameter(const std::string &name, CurvePoint &value) {
    if(doubleParameterIndices.count(name) > 0) {
      int i = doubleParameterIndices[name];
      int n = doubleParameters[i].values.size();
      if(i >= 0 && i < doubleParameters.size() && n > 2) {
        value.set(doubleParameters[i].values[0], doubleParameters[i].values[1], doubleParameters[i].values[2]);
        return n;
      }
    }
    return 0;
  }
  
  int getVectorParameter(const std::string &name, std::vector<CurvePoint> &values) {
    if(doubleParameterIndices.count(name) > 0) {
      int i = doubleParameterIndices[name];
      int n = doubleParameters[i].values.size();
      if(i >= 0 && i < doubleParameters.size() && n > 2) {
        values.resize(n/3);
        for(int k=0; k<n/3; k++) {
          values[k].set(doubleParameters[i].values[k*3], doubleParameters[i].values[k*3+1], doubleParameters[i].values[k*3+2]);
        }
        return n;
      }
    }
    return 0;
  }
  
  int getVectorParameter(const std::string &name, Vec3d &value) {
    if(doubleParameterIndices.count(name) > 0) {
      int i = doubleParameterIndices[name];
      int n = doubleParameters[i].values.size();
      if(i >= 0 && i < doubleParameters.size() && n > 2) {
        value.set(doubleParameters[i].values[0], doubleParameters[i].values[1], doubleParameters[i].values[2]);
        return n;
      }
    }
    return 0;
  }
  
  int getVectorParameter(const std::string &name, std::vector<Vec3d> &values) {
    if(doubleParameterIndices.count(name) > 0) {
      int i = doubleParameterIndices[name];
      int n = doubleParameters[i].values.size();
      if(i >= 0 && i < doubleParameters.size() && n > 2) {
        values.resize(n/3);
        for(int k=0; k<n/3; k++) {
          values[k].set(doubleParameters[i].values[k*3], doubleParameters[i].values[k*3+1], doubleParameters[i].values[k*3+2]);
        }
        return n;
      }
    }
    return 0;
  }

  int getVectorParameter(const std::string &name, Vec4d &value) {
    if(doubleParameterIndices.count(name) > 0) {
      int i = doubleParameterIndices[name];
      int n = doubleParameters[i].values.size();
      if(i >= 0 && i < doubleParameters.size() && n > 3) {
        value.set(doubleParameters[i].values[0], doubleParameters[i].values[1], doubleParameters[i].values[2], doubleParameters[i].values[3]);
        return n;
      }
    }
    return 0;
  }
  
  int getVectorParameter(const std::string &name, std::vector<Vec4d> &values) {
    if(doubleParameterIndices.count(name) > 0) {
      int i = doubleParameterIndices[name];
      int n = doubleParameters[i].values.size();
      if(i >= 0 && i < doubleParameters.size() && n > 3) {
        values.resize(n/4);
        for(int k=0; k<n/4; k++) {
          values[k].set(doubleParameters[i].values[k*4], doubleParameters[i].values[k*4+1], doubleParameters[i].values[k*4+2], doubleParameters[i].values[k*4+3]);
        }
        return n;
      }
    }
    return 0;
  }

  int getVectorParameter(const std::string &name, Vec2i &value) {
    if(doubleParameterIndices.count(name) > 0) {
      int i = doubleParameterIndices[name];
      int n = doubleParameters[i].values.size();
      if(i >= 0 && i < doubleParameters.size() && n > 1) {
        value.set((int)doubleParameters[i].values[0], (int)doubleParameters[i].values[1]);
        return n;
      }
    }
    return 0;
  }
  
  int getVectorParameter(const std::string &name, std::vector<Vec2i> &values) {
    if(doubleParameterIndices.count(name) > 0) {
      int i = doubleParameterIndices[name];
      int n = doubleParameters[i].values.size();
      if(i >= 0 && i < doubleParameters.size() && n > 1) {
        values.resize(n/2);
        for(int k=0; k<n/2; k++) {
          values[k].set((int)doubleParameters[i].values[k*2], (int)doubleParameters[i].values[k*2+1]);
        }
        return n;
      }
    }
    return 0;
  }
  
};



static int getParameterIndex(const std::string &name, const std::vector<StringParameter> &parameters) {
  for(int i=0; i<parameters.size(); i++) {
    if(parameters[i].name == name) {
      return i;
    }
  }
  return -1;
}
static int getParameterIndex(const std::string &name, const std::vector<DoubleParameter> &parameters) {
  for(int i=0; i<parameters.size(); i++) {
    if(parameters[i].name == name) {
      return i;
    }
  }
  return -1;
}