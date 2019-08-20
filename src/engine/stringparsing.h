#pragma once
#include <vector>
#include <string>


static int findMatchingEndingBracket(const std::string &str, int start, char startingBracket = '{', char endingBracket = '}') {
  ////printf("(debugging) at findMatchingEndingBracket(), start = %d\n%s\n", start, str.c_str());
  //printf("(debugging) at findMatchingEndingBracket(), start = %d\n", start);
  int numOpeningBrackets = 0, numClosingBrackets = 0;
  for(int i=start; i<str.size(); i++) {
    if(str[i] == startingBracket) numOpeningBrackets++;
    if(str[i] == endingBracket) numClosingBrackets++;
    if(numOpeningBrackets > 0 && numOpeningBrackets == numClosingBrackets) {
      return i;
    }
  }
  return -1;
}

static std::string fetchBlock(const std::string &srcStr, const std::string &blockName, int startIndex = 0) {
  //printf("(debugging) at fetchBlock() '%s'\n", blockName.c_str());
  int ind = startIndex, end = startIndex;

  if((ind = srcStr.find(blockName, end)) != std::string::npos) {
    end = findMatchingEndingBracket(srcStr, ind, '{', '}');
    if(end == -1) {
      printf("Error at fetchBlock(): no ending bracket found for '%s'\n", blockName.c_str());
      return "";
    }
    return srcStr.substr(ind, end-ind+1);
  }
  return "";
}

static std::vector<std::string> fetchBlocks(const std::string &srcStr, const std::string &blockName, int startIndex = 0) {
  //printf("(debugging) at fetchBlocks() '%s'\n", blockName.c_str());
  int ind = startIndex, end = startIndex;
  std::vector<std::string> blocks;
  while((ind = srcStr.find(blockName, end)) != std::string::npos) {
    end = findMatchingEndingBracket(srcStr, ind, '{', '}');
    if(end == -1) {
      printf("Error at fetchBlocks(): no ending bracket found for '%s'\n", blockName.c_str());
      return blocks;
    }
    blocks.push_back(srcStr.substr(ind, end-ind+1));
  }
  return blocks;
}


static std::vector<std::string> fetchNumericParameter(const std::string &name, const std::string &str, int &ind) {
  //printf("(debugging) at fetchNumericParameter(), %s\n%s\n", name.c_str(), str.c_str());
  std::vector<std::string> values;
  values.push_back(name);
  while(true) {
    ind = str.find(name, ind);
    if(ind != std::string::npos) {
      if(str[ind+name.size()] == ' ' || str[ind+name.size()] == ':') {
        break;
      }
    }
    else {
      break;
    }
    ind += name.size();
  }
  if(ind != std::string::npos) {
    int start = str.find(":", ind);
    int end = findMatchingEndingBracket(str, start, ':', '\n');
    if(end == -1) {
      printf("Error at fetchNumericParameter(): for \"%s\" no matching ':' and ';' found\n", name.c_str());
      return values;
    }
    std::string valueString = str.substr(start+1, end-start);
    int i = 0;
    while((i = valueString.find_first_of("-1234567890.", i)) != std::string::npos) {
      int p = valueString.find_first_of(",();\n ", i);
      values.push_back(valueString.substr(i, p-i));
      if(p == std::string::npos || valueString[p] == ')' || valueString[p] == ';') break;
      i = p;
    }
    ind = end;
  }

  return values;
}
static std::vector<std::vector<std::string>> fetchNumericParameters(const std::string &name, const std::string &str) {
  int ind = 0;
  std::vector<std::vector<std::string>> parameters;
  while(true) {
    std::vector<std::string> out = fetchNumericParameter(name, str, ind);
    if(out.size() > 1) parameters.push_back(out);
    else break;
  }
  return parameters;
}

static std::vector<std::string> fetchNumericParameter(const std::string &name, const std::string &str) {
  int ind = 0;
  return fetchNumericParameter(name, str, ind);
}


static std::vector<std::string> fetchStringParameter(const std::string &name, const std::string &str) {
  //printf("(debugging) at fetchStringParameter(), %s\n%s\n", name.c_str(), str.c_str());

  std::vector<std::string> values;
  values.push_back(name);
  int ind = str.find(name);
  if(ind != std::string::npos) {
    int start = str.find(":", ind);
    int end = findMatchingEndingBracket(str, start, ':', '\n');
    if(end == -1) {
      printf("Error at fetchStringParameter(): for \"%s\" no matching ':' and ';' found\n", name.c_str());
      return values;
    }
    std::string valueString = str.substr(start+1, end-start);
    int i = 0;
    //while(true) {
      int valueStartInd = -1, valueEndInd = -1;
      for(int i=0; i<valueString.size(); i++) {
        if(valueStartInd == -1 && valueString[i] != ':' && valueString[i] != ' ' && valueString[i] != '(') {
          valueStartInd = i;
        }
        if(valueStartInd != -1 && (valueString[i] == ';' || valueString[i] == '\n')) {
          valueEndInd = i;
        }
      }
      if(valueStartInd > -1) {
        if(valueEndInd == -1) valueEndInd = valueString.size() - 1;
        values.push_back(valueString.substr(valueStartInd, valueEndInd-valueStartInd));
      }
    }
  //}
  return values;
}


static std::vector<std::string> extractNumericParameter(const std::string &name, std::string &str) {
  //printf("(debugging) at extractNumericParameter(), %s\n%s\n", name.c_str(), str.c_str());
  std::vector<std::string> values;
  values.push_back(name);
  int ind = 0;
  while(true) {
    ind = str.find(name, ind);
    if(ind != std::string::npos) {
      if(str[ind+name.size()] == ' ' || str[ind+name.size()] == ':') {
        break;
      }
    }
    else {
      break;
    }
    ind += name.size();
  }
  if(ind != std::string::npos) {
    int start = str.find(":", ind);
    int end = findMatchingEndingBracket(str, start, ':', '\n');
    if(end == -1) {
      printf("Error at extractParameter(): for \"%s\" no matching ':' and ';' found\n", name.c_str());
      return values;
    }
    std::string valueString = str.substr(start+1, end-start);
    int i = 0;
    while((i = valueString.find_first_of("-1234567890.", i)) != std::string::npos) {
      int p = valueString.find_first_of(",();\n ", i);
      values.push_back(valueString.substr(i, p-i));
      if(p == std::string::npos || valueString[p] == ')' || valueString[p] == ';') break;
      i = p;
    }
    str.erase(ind, end-ind+1);
  }
  return values;
}

static std::vector<std::string> extractStringParameter(const std::string &name, std::string &str) {
  //printf("(debugging) at extractStringParameter(), %s\n%s\n", name.c_str(), str.c_str());
  std::vector<std::string> values;
  values.push_back(name);
  int ind = str.find(name);
  if(ind != std::string::npos) {
    int start = str.find(":", ind);
    int end = findMatchingEndingBracket(str, start, ':', '\n');
    if(end == -1) {
      printf("Error at extractStringParameter(): for \"%s\" no matching ':' and ';' found\n", name.c_str());
      return values;
    }
    std::string valueString = str.substr(start+1, end-start);
    int i = 0;
    //while(true) {
      int valueStartInd = -1, valueEndInd = -1;
      for(int i=0; i<valueString.size(); i++) {
        if(valueStartInd == -1 && valueString[i] != ':' && valueString[i] != ' ' && valueString[i] != '(') {
          valueStartInd = i;
        }
        if(valueStartInd != -1 && (valueString[i] == ';' || valueString[i] == '\n')) {
          valueEndInd = i;
        }
      }
      if(valueStartInd > -1) {
        if(valueEndInd == -1) valueEndInd = valueString.size() - 1;
        values.push_back(valueString.substr(valueStartInd, valueEndInd-valueStartInd));
      }
      str.erase(ind, end-ind+1);
    }
  //}
  return values;
}


static void printParameters(const std::vector<std::vector<std::string>> &parameters, int level) {
  ////printf("(debugging) at printParameters()\n");
  for(int i=0; i<parameters.size(); i++) {
    if(parameters[i].size() > 1) {
      printIntendation(level);
      printf("%s: ", parameters[i][0].c_str());
      for(int k=1; k<parameters[i].size(); k++) {
        printf("%s%s", parameters[i][k].c_str(), k+1<parameters[i].size() ? ", ": "\n");
      }
    }
  }
}
static void printParameters(std::string &str, const std::vector<std::vector<std::string>> &parameters, int level) {
  ////printf("(debugging) at printParameters()\n");
  for(int i=0; i<parameters.size(); i++) {
    if(parameters[i].size() > 1) {
      printIntendation(str, level);
      str += parameters[i][0] + ": ";
      for(int k=1; k<parameters[i].size(); k++) {
        str += parameters[i][k];
        if(k+1 < parameters[i].size()) str += ", ";
        else str += "\n";
      }
    }
  }
}



static std::string getParameter(const std::string &name, const std::vector<std::vector<std::string>> &parameters) {
  for(int i=0; i<parameters.size(); i++) {
    if(parameters[i].size() > 1 && parameters[i][0].compare(name) == 0) {
      return parameters[i][1];
    }
  }
  return "";
}
