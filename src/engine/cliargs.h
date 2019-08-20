#pragma once
#include <string>
#include <vector>

struct InputProperty
{
  std::string key;
  std::vector<std::string> values;
};

struct CommandLineArguments
{
  std::vector<InputProperty> inputProperties;

  bool hasKey(std::string key) {
    for(int i=0; i<inputProperties.size(); i++) {
      if(inputProperties[i].key.compare(key) == 0) {
        return true;
      }
    }
    return false;
  }

  int numValues(std::string key) {
    for(int i=0; i<inputProperties.size(); i++) {
      if(inputProperties[i].key.compare(key) == 0) {
        return inputProperties[i].values.size();
      }
    }
    return 0;
  }

  std::vector<std::string> getValues(std::string key) {
    for(int i=0; i<inputProperties.size(); i++) {
      if(inputProperties[i].key.compare(key) == 0) {
        return inputProperties[i].values;
      }
    }
    return std::vector<std::string>();
  }

  void parse(int argc, char* argv[])
  {
    bool keyFound = false;
    for(int i=0; i<argc; i++) {
      char str[256];
      strcpy(str, argv[i]);
      if(strlen(str) > 0) {
        if(str[0] == '-') {
          inputProperties.push_back(InputProperty());
          inputProperties[inputProperties.size()-1].key.assign(str);
          keyFound = true;
        }
        else if(keyFound) {
          inputProperties[inputProperties.size()-1].values.push_back(std::string(str));
        }
      }
    }
  }
  void print() {
    for(InputProperty ip : inputProperties) {
      printf("%s:", ip.key.c_str());
      for(std::string str : ip.values) {
        printf(" %s", str.c_str());
      }
      printf("\n");
    }
  }
};
