#pragma once
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>
#include <boost/filesystem.hpp>
#include <cstdlib>
#include <ctime>
#include <string>
#include <fstream>
#include <cwchar>
#include <ctgmath>
#include <cstring>
#include <chrono>
#include <locale>
#include <codecvt>
#include <climits>
#include <vector>
#include <initializer_list>

#include "external/glm/glm.hpp"
#include "external/glm/ext.hpp"

static bool checkGlError2(const std::string &place, const std::string &glCall) {
  bool allGood = true;
  GLenum error;
  while((error = glGetError()) != GL_NO_ERROR) {
    printf("OpenGl error at %s, %s: %s\n", gluErrorString(error), place.c_str(), glCall.c_str());
    allGood = false;
  }
  return allGood;
}


struct GlmMatrixStack {
  std::vector<glm::mat4> modelViewStack, projectionStack;
  enum MatrixMode {ModelView, Projection};
  MatrixMode matrixMode = MatrixMode::ModelView;
  
  //std::vector<GLuint> shaderPrograms;
  
  GlmMatrixStack() {
    modelViewStack.push_back(glm::mat4(1.0));
    projectionStack.push_back(glm::mat4(1.0));
  }
  
  /*void addShaderProgram(GLuint program) {
    shaderPrograms.push_back(program);
  }*/
  
  void updateShader(GLuint program) {

    int modelViewLoc = glGetUniformLocation(program, "modelView");
    checkGlError2("GlmMatrixStack.updateShader()", "glGetUniformLocation(shaderPrograms[i], \"modelView\")");
    
    glUniformMatrix4fv(modelViewLoc, 1, GL_FALSE, glm::value_ptr(modelViewStack.back()));
    checkGlError2("GlmMatrixStack.updateShader()", "glUniformMatrix4fv(modelViewLoc, ...)");
    //printf("modelViewLoc %i\n", modelViewLoc);
    
    int projectionLoc = glGetUniformLocation(program, "projection");
    checkGlError2("GlmMatrixStack.updateShader()", "glGetUniformLocation(shaderPrograms[i], \"projection\")");
    
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projectionStack.back()));
    checkGlError2("GlmMatrixStack.updateShader()", "glUniformMatrix4fv(projectionLoc, ...)");
    //printf("projectionLoc %i\n", projectionLoc);
  }
  
  /*void updateShaders() {
    for(int i=0; i<shaderPrograms.size(); i++) {
      int modelViewLoc = glGetUniformLocation(shaderPrograms[i], "modelView");
      checkGlError2("GlmMatrixStack.updateShaders()", "glGetUniformLocation(shaderPrograms[i], \"modelView\")");
      
      glUniformMatrix4fv(modelViewLoc, 1, GL_FALSE, glm::value_ptr(modelViewStack.back()));
      checkGlError2("GlmMatrixStack.updateShaders()", "glUniformMatrix4fv(modelViewLoc, ...)");
      
      int projectionLoc = glGetUniformLocation(shaderPrograms[i], "projection");
      checkGlError2("GlmMatrixStack.updateShaders()", "glGetUniformLocation(shaderPrograms[i], \"projection\")");
      
      glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projectionStack.back()));
      checkGlError2("GlmMatrixStack.updateShaders()", "glUniformMatrix4fv(projectionLoc, ...)");
    }
  }*/
  
  void setMatrixMode(MatrixMode matrixMode) {
    this->matrixMode = matrixMode;
    if(matrixMode == ModelView) {
      glMatrixMode(GL_MODELVIEW);
    }
    else {
      glMatrixMode(GL_PROJECTION);
    }
  }
  
  void pushMatrix() {
    if(matrixMode == MatrixMode::ModelView) {
      modelViewStack.push_back(modelViewStack.back());
    }
    else {
      projectionStack.push_back(projectionStack.back());
    }
    glPushMatrix();
  }
  void popMatrix() {
    if(matrixMode == MatrixMode::ModelView) {
      modelViewStack.pop_back();
    }
    else {
      projectionStack.pop_back();
    }
    glPopMatrix();
  }

  
  void ortho(double left, double right, double bottom, double top, double near, double far) {
    if(matrixMode == MatrixMode::ModelView) {
      modelViewStack[modelViewStack.size()-1] = glm::ortho(left, right, bottom, top, near, far);
    }
    else {
      projectionStack[projectionStack.size()-1] = glm::ortho(left, right, bottom, top, near, far);
    }
    glOrtho(left, right, bottom, top, near, far);
  }
  
  void perspective(double fow, double aspectRatio, double nearClip, double farClip) {
    if(matrixMode == MatrixMode::ModelView) {
      modelViewStack[modelViewStack.size()-1] = glm::perspective(glm::radians(fow), aspectRatio, nearClip, farClip);
    }
    else {
      projectionStack[projectionStack.size()-1] = glm::perspective(glm::radians(fow), aspectRatio, nearClip, farClip);
    }
  }
  
  void lookAt(double px, double py, double pz, double tx, double ty, double tz, double dx, double dy, double dz) {
    if(matrixMode == MatrixMode::ModelView) {
      modelViewStack[modelViewStack.size()-1] = glm::lookAt(glm::vec3(px, py, pz), glm::vec3(tx, ty, tz), glm::vec3(dx, dy, dz));
    }
    else {
      projectionStack[projectionStack.size()-1] = glm::lookAt(glm::vec3(px, py, pz), glm::vec3(tx, ty, tz), glm::vec3(dx, dy, dz));
    }
  }
  
  void loadIdentity() {
    if(matrixMode == MatrixMode::ModelView) {
      modelViewStack[modelViewStack.size()-1] = glm::mat4(1);
    }
    else {
      projectionStack[projectionStack.size()-1] = glm::mat4(1);
    }
    glLoadIdentity();
  }
  
  void translate(double x, double y, double z) {
    if(matrixMode == MatrixMode::ModelView) {
      modelViewStack[modelViewStack.size()-1] = glm::translate(modelViewStack.back(), glm::vec3(x, y, z));
    }
    else {
      projectionStack[projectionStack.size()-1] = glm::translate(projectionStack.back(), glm::vec3(x, y, z));
    }
    glTranslated(x, y, z);
  }
  void scale(double x, double y, double z) {
    if(matrixMode == MatrixMode::ModelView) {
      modelViewStack[modelViewStack.size()-1] = glm::scale(modelViewStack.back(), glm::vec3(x, y, z));
    }
    else {
      projectionStack[projectionStack.size()-1] = glm::scale(projectionStack.back(), glm::vec3(x, y, z));
    }
    glScalef(x, y, z);
  }
  void rotate(double angle, double x, double y, double z) {
    if(matrixMode == MatrixMode::ModelView) {
      modelViewStack[modelViewStack.size()-1] = glm::rotate(modelViewStack.back(), (float)angle, glm::vec3(x, y, z));
    }
    else {
      projectionStack[projectionStack.size()-1] = glm::rotate(projectionStack.back(), (float)angle, glm::vec3(x, y, z));
    }
    glRotatef(angle, x, y, z);
  }


};





/* TODO
 * - wrap everything within classes
 * - divide to individual source files
 */

 static const std::vector<std::string> fastNoiseTypeNames = { "Value", "ValueFractal", "Perlin", "PerlinFractal", "Simplex", "SimplexFractal", "Cellular", "WhiteNoise", "Cubic", "CubicFractal" };
 static const std::vector<std::string> fastNoiseInterpNames = { "Linear", "Hermite", "Quintic" };
 static const std::vector<std::string> fastNoiseFractalTypeNames = { "FBM", "Billow", "RigidMulti" };
 static const std::vector<std::string> fastNoiseCellularDistanceFunctionNames = { "Euclidean", "Manhattan", "Natural" };
 static const std::vector<std::string> fastNoiseCellularReturnTypeNames = { "CellValue", "NoiseLookup", "Distance", "Distance2", "Distance2Add", "Distance2Sub", "Distance2Mul", "Distance2Div" };


static const double PI = 3.14159265359;
static const double TAU = 2.0*3.14159265359;


static std::chrono::time_point<std::chrono::high_resolution_clock> ticTime, tocTime;
static double ticTocDuration = 0;

static inline void tic() {
  ticTime = std::chrono::high_resolution_clock::now();
}

static inline double toc() {
  tocTime = std::chrono::high_resolution_clock::now();
  ticTocDuration = std::chrono::duration_cast<std::chrono::duration<double>>(tocTime-ticTime).count();
  return ticTocDuration;
}

struct TicToc {
  std::chrono::time_point<std::chrono::high_resolution_clock> ticTime, tocTime;
  double duration = 0;

  TicToc() {
    tic();
  }

  inline void tic() {
    ticTime = std::chrono::high_resolution_clock::now();
  }

  inline double toc() {
    tocTime = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::duration<double>>(tocTime-ticTime).count();
    return duration;
  }

};

static TicToc midiLatencyTestTicToc;
static bool midiLatencyTestActivated = false;
static double midiLatencyTestMidiTimeStamp = 0;




static void makeTitleCase(std::string &str) {
  for(int i=0; i<str.size(); i++) {
    if(i == 0 || !isalnum(str[i-1])) {
      str[i] = toupper(str[i]);
    }
    else {
      str[i] = tolower(str[i]);
    }
  }
}

static void toUpperCase(std::string &str) {
  for(int i=0; i<str.size(); i++) {
    str[i] = toupper(str[i]);
  }
}

static void toLowerCase(std::string &str) {
  for(int i=0; i<str.size(); i++) {
    str[i] = tolower(str[i]);
  }
}

struct RomanNumeral
{
private:
  static std::string get1000(int number) {
    std::string str;
    if(number == 1) return "M";
    else if(number == 2) return "MM";
    else if(number == 3) return "MMM";
    else if(number > 3 && number < 10) return "(error)";
    return "";
  }

  static std::string get100(int number) {
    std::string str;
    if(number == 1) return "C";
    else if(number == 2) return "CC";
    else if(number == 3) return "CCC";
    else if(number == 4) return "CD";
    else if(number == 5) return "D";
    else if(number == 6) return "DC";
    else if(number == 7) return "DCC";
    else if(number == 8) return "DCCC";
    else if(number == 9) return "CM";
    return "";
  }
  
  static std::string get10(int number) {
    std::string str;
    if(number == 1) return "X";
    else if(number == 2) return "XX";
    else if(number == 3) return "XXX";
    else if(number == 4) return "XL";
    else if(number == 5) return "L";
    else if(number == 6) return "LX";
    else if(number == 7) return "LXX";
    else if(number == 8) return "LXXX";
    else if(number == 9) return "XC";
    return "";
  }
  
  static std::string get1(int number) {
    std::string str;
    if(number == 1) return "I";
    else if(number == 2) return "II";
    else if(number == 3) return "III";
    else if(number == 4) return "IV";
    else if(number == 5) return "V";
    else if(number == 6) return "VI";
    else if(number == 7) return "VII";
    else if(number == 8) return "VIII";
    else if(number == 9) return "IX";
    return "";
  }
  
public:
  static std::string get(int number) {
    if(number < 1 || number > 3000) {
      printf("Error at RomanNumeral::get(int), number %d out of range\n", number);
      return "";
    }
    int thousands = number / 1000;
    int hundreds = (number - thousands*1000) / 100;
    int tens = (number - thousands*1000 - hundreds*100) / 10;
    int ones = (number - thousands*1000 - hundreds*100 - tens*10);
    return get1000(thousands) + get100(hundreds) + get10(tens) + get1(ones);
  }
};



static std::string toString(double a) {
  char buf[128];
  int n = std::sprintf(buf, "%.9f", a);
  bool d = false;
  int l = 0;
  
  for(int i=0; i<n; i++) {
    if(!d) {
      if(buf[i] == '.' || buf[i] == ',') {
        d = true;
        l = i-1;
      }
    }
    else {
      if(buf[i] != '0') {
        l = i;
      }
    }
  }
  buf[l+1] = '\0';

  return std::string(buf);
}


static void printIntendation(int level, int numSpaces = 2) {
  for(int i=0; i<level*numSpaces; i++) {
    printf(" ");
  }
}

static void printIntendation(std::string &str, int level, int numSpaces = 2) {
  for(int i=0; i<level*numSpaces; i++) {
    str += " ";
  }
}



struct DoubleParameter {
  std::string name;
  std::vector<double> values;
  
  DoubleParameter() {}
  DoubleParameter(const std::string &name) {
    this->name = name;
  }
  
  template<class T>
  DoubleParameter(const std::string &name, const T &value) {
    if(!std::is_fundamental<T>::value) {
      printf("Error at DoubleParameter(), name '%s', invalid numeric type\n", name.c_str());
    }
    else {
      this->name = name;
      double d = (double)value;
      this->values = {d};
    }
  }
  
  DoubleParameter(const std::string &name, const std::vector<double> &values, unsigned int maxCount = -1) {
    this->name = name;
    if(maxCount >= values.size()) {
      this->values = values;
    }
    else {
      this->values.resize(maxCount);
      for(int i=0; i<maxCount; i++) {
        this->values[i] = values[i];
      }
    }
  }
    
  void print(std::string &str, int level = 0) {
    printIntendation(str, level);
    str += name + ": ";
    for(int k=0; k<values.size(); k++) {
      str += toString(values[k]);
      if(k+1 < values.size()) str += ", ";
    }
    str += "\n";
  }
  void print(int level = 0) {
    printIntendation(level);
    printf("%s: ", name.c_str());
    for(int k=0; k<values.size(); k++) {
      printf("%f", values[k]);
      if(k+1 < values.size()) printf(", ");;
    }
    printf("\n");
  }
};


struct StringParameter {
  std::string name;
  std::vector<std::string> values;
  
  StringParameter() {}
  StringParameter(const std::string &name) {
    this->name = name;
  }
  StringParameter(const std::string &name, const std::string &value) {
    this->name = name;
    this->values = {value};
  }
  StringParameter(const std::string &name, const std::vector<std::string> &values, unsigned int maxCount = -1) {
    this->name = name;
    
    if(maxCount >= values.size()) {
      this->values = values;
    }
    else {
      this->values.resize(maxCount);
      for(int i=0; i<maxCount; i++) {
        this->values[i] = values[i];
      }
    }
  }
  
  void print(std::string &str, int level = 0) {
    printIntendation(str, level);
    str += name + ": ";
    for(int k=0; k<values.size(); k++) {
      str += values[k];
      if(k+1 < values.size()) str += ", ";
    }
    str += "\n";
  }
  void print(int level = 0) {
    printIntendation(level);
    printf("%s: ", name.c_str());
    for(int k=0; k<values.size(); k++) {
      printf("%s", values[k].c_str());
      if(k+1 < values.size()) printf(", ");;
    }
    printf("\n");
  }
};



struct Primes {
  std::vector<unsigned long> primes;
  unsigned long maxComposite = 0;
  unsigned long maxPrime = 100000;
  unsigned long maxAmountOfPrimes = 0;
  unsigned long numPrimes = 0;

  Primes(bool initAtCreation = true) {
    if(initAtCreation) {
      init();
    }
  }
  
  unsigned long getNthPrime(unsigned long n) {
    if(n >= 0 && n < primes.size()) {
      return primes[n];
    }
    return 0;
  }
  // TODO check if this works
  void ensureMaxPrime(int maxPrime) {
    if(this->maxPrime < maxPrime) {
      unsigned long maxPrimeTmp = this->maxPrime;
      this->maxPrime = maxPrime;
      maxAmountOfPrimes = 0;
      
      for(unsigned long i=maxPrimeTmp+1; i < maxPrime; i++) {
        unsigned long m = (unsigned long)sqrt(i);
        bool isPrime = true;
        for(unsigned long k=0; k<primes.size() && k <= m; k++) {
          if(i % primes[k] == 0) {
            isPrime = false;
            break;
          }
        }
        if(isPrime) {
          primes.push_back(i);
  	      maxPrimeTmp = i;
        }
      }

      maxPrime = primes[primes.size()-1];
      numPrimes = primes.size();

      maxComposite = maxComposite > maxPrime ? maxComposite : maxPrime;
    }
  }
  
  void init() {
    unsigned long maxPrimeTmp = 0;
    if(maxComposite < 2 && maxPrime < 2 && maxAmountOfPrimes < 1) {
      maxAmountOfPrimes = 100000;
    }
    if(maxAmountOfPrimes > 0) {
      numPrimes = 0;
      primes.resize(maxAmountOfPrimes);
      for(unsigned long i=2; ; i++) {
        unsigned long m = (unsigned long)sqrt(i);
        bool isPrime = true;
        for(unsigned long k=0; k<numPrimes && k <= m; k++) {
          if(i % primes[k] == 0) {
            isPrime = false;
            break;
          }
        }
        if(isPrime) {
          primes[numPrimes] = i;
  	       maxPrimeTmp = i;
          numPrimes++;
          if(numPrimes >= maxAmountOfPrimes) {
            break;
          }
        }
      }
    }
    else {
      for(unsigned long i=2; i<=maxComposite || maxPrimeTmp < maxPrime; i++) {
        unsigned long m = (unsigned long)sqrt(i);
        bool isPrime = true;
        for(unsigned long k=0; k<primes.size() && k <= m; k++) {
          if(i % primes[k] == 0) {
            isPrime = false;
            break;
          }
        }
        if(isPrime) {
          primes.push_back(i);
  	      maxPrimeTmp = i;
        }
      }
    }
    maxPrime = primes[primes.size()-1];
    numPrimes = primes.size();

    maxComposite = maxComposite > maxPrime ? maxComposite : maxPrime;
  }
  
  unsigned long getClosestPrime(unsigned long number) {
    if(number <= primes[0]) return primes[0];
    if(number >= maxPrime) return maxPrime;
    
    unsigned long closestPrime = primes[0];
    
    for(unsigned long i=0; i<primes.size(); i++) {
      if(number < primes[i]) {
        if(number == primes[i]) {
          return number;
        }
        if(number - primes[i-1] < primes[i] - number) {
          return primes[i-1];
        }
        return primes[i];
      }
    }
    
    return 0;
  }

    unsigned long getClosestPrime(unsigned long number, unsigned long &i) {
    if(number <= primes[0]) return primes[0];
    if(number >= maxPrime) return maxPrime;
    
    unsigned long closestPrime = primes[0];
    
    for(i=0; i<primes.size(); i++) {
      if(number - primes[i] <= 0) {
        if(number == primes[i]) {
          return number;
        }
        if(number - primes[i-1] < primes[i] - number) {
          return primes[i-1];
        }
        return primes[i];
      }
    }
    
    return 0;
  }
  
  
  std::vector<unsigned long> getPrimeFactorization(unsigned long number) {
    std::vector<unsigned long> primeFactors;
    if(number > maxComposite) {
      printf("Error at Primes::getPrimeFactorization(), too big number %lu, primes initialized up to %lu\n", number, maxComposite);
    }
    
    for(unsigned long i=0; i<primes.size(); i++) {
      if(number == 1) {
        break;
      }
      if(number % primes[i] == 0) {
        primeFactors.push_back(primes[i]);
        number /= primes[i];
        i--;
      }
    }
    return primeFactors;
  }


};
static Primes primes;



template<class T>
static bool contains(const std::vector<T> &v, const T &t) {
  for(int i=0; i<v.size(); i++) {
    if(v[i] == t) return true;
  }
  return false;
}

template<class T, class S>
static inline T min(const T &a, const S &b) {
  return a < b ? a : b;
}

template<class T, class S>
static inline T max(const T &a, const S &b) {
  return a > b ? a : b;
}

template<class A, class B, class C>
static inline A clamp(A a, B b, C c) {
  return a > c ? c : (a < b ? b : a);
}

static inline float step(float threshold, float a) {
  return a >= threshold ? 1.0f : 0.0f;
}
static inline float smoothstep(float edge1, float edge2, float a) {
  if(edge2 == edge1) return 0;

  float x = clamp((a-edge1)/(edge2-edge1), 0.0f, 1.0f);
  return x*x*(3.0f - 2.0f*x);
}

static inline double step(double threshold, double a) {
  return a >= threshold ? 1.0 : 0.0;
}
static inline double smoothstep(double edge1, double edge2, double a) {
  if(edge2 == edge1) return 0;

  double x = clamp((a-edge1)/(edge2-edge1), 0.0, 1.0);
  return x*x*(3.0 - 2.0*x);
}
static inline double box(double threshold1, double threshold2, double a) {
  return a > threshold1 && a < threshold2 ? 1.0 : 0.0;
}
static inline double map(double t, double a, double b, double x, double y) {
  if(b == a) return 0;
  return (t-a)/(b-a)*(y-x)+x;
}
static inline int roundClamp(double t, double min, double max) {
  int i = (int)round(t);
  return i > max ? max : (i < min ? min : i);
}
static inline double mapRoundClamp(double t, double a, double b, double x, double y) {
  if(b == a) return 0;
  int i = (int)round((t-a)/(b-a)*(y-x)+x);
  return i > y ? y : (i < x ? x : i);
}

static inline double lerp(double a, double b, double t) {
  return (b-a)*t + a;
}

/*template<class T> static inline T lerp(const T &a, const T &b, double t) {
  return (b-a)*t + a;
}*/
template<class T> static inline T mix(const T &a, const T &b, double t) {
  t = clamp(t, 0, 1);
  return a * (1.0-t) + b * t;
}


static void printBits(unsigned int bits) {
  int n = sizeof(bits)*8;
  for(int i=n-1; i>=0; i--) {
    printf("%d%s", bits>>i & 1, i == 0 ? "\n" : (i % 8 == 0 ? " - " : " "));
  }
}


struct AudioFileMetaData {
  std::string title;
  std::string copyright;
  std::string software;
  std::string artist;
  std::string comment;
  std::string date;
  std::string album;
  std::string license;
  std::string trackNumber;
  std::string genre;
  
  void print() {
    printf("Audiofile metadata, title = '%s', copyright = '%s', software = '%s', artist = '%s', comment = '%s', date = '%s', album = '%s', license = '%s', trackNumber = '%s', genre = '%s'\n", title.c_str(), copyright.c_str(), software.c_str(), artist.c_str(), comment.c_str(), date.c_str(), album.c_str(), license.c_str(), trackNumber.c_str(), genre.c_str());
  }
};


/*struct LookupMath
{
  double expLookupTableLength = 1000;
  long expLookupTableSize = 1<<20;
  std::vector<double> expLookupTable, expMinusLookupTable;
  LookupMath() {
    prepareLookupTables();
  }
  void prepareLookupTables() {
    if(expLookupTable.size() != expLookupTableSize) {
      expLookupTable.resize(expLookupTableSize);
      expMinusLookupTable.resize(expLookupTableSize);
    }
    for(int i=0; i<expLookupTableSize; i++) {
      double t = map(i, 0, expLookupTableSize-1, 0.0, expLookupTableLength);
      expLookupTable[i] = std::exp(t);
      expMinusLookupTable[i] = std::exp(-t);
    }
  }
  inline double exp(double t) {
    int i = int(t / expLookupTableLength * expLookupTableSize);
    if(i * i >= expLookupTableSize*expLookupTableSize) return 0;
    if(i > 0) return expLookupTable[i];
    if(i < 0) return expMinusLookupTable[-i];
    return 0;
  }
};*/

/*typedef std::codecvt<wchar_t,char,std::mbstate_t> facet_type;
std::locale mylocale;
const facet_type& myfacet = std::use_facet<facet_type>(mylocale);
std::mbstate_t mystate = std::mbstate_t();

std::wstring to_wstring(std::string str) {
  const char *chars = str.c_str();

  wchar_t pwstr[str.length()];
  const char* pc;
  wchar_t* pwc;

  facet_type::result result = myfacet.in (mystate,
    chars, chars+str.length(), pc,
    pwstr, pwstr+str.length(), pwc);

  return std::wstring(pwstr);
}
std::string to_string(std::wstring wstr) {
  const wchar_t *wchars = wstr.c_str();

  char str[wstr.length()];
  char* pcc;
  const wchar_t* pwcc;
  facet_type::result result = myfacet.out(mystate, wchars, wchars+wstr.length(), pwcc, str, str+wstr.length(), pcc);
  str[wstr.length()] = '\0';
  return std::string(str);
}*/


template<typename ... Args>
static std::string format(const std::string &format, Args ... args) {
  size_t size = snprintf(nullptr, 0, format.c_str(), args ...) + 1; // Extra space for '\0'
  std::unique_ptr<char[]> buf(new char[size]);
  snprintf(buf.get(), size, format.c_str(), args ...);

  return std::string(buf.get(), buf.get() + size - 1);
}

static std::string formatTime(double seconds) {
  if(seconds < 60*60) {
    int mins = (int)seconds/60;
    int secs = (int)seconds - mins*60;
    if(secs < 1) return format("%d:00", mins);
    else if(secs < 10) return format("%d:0%d", mins, secs);
    else return format("%d:%d", mins, secs);
  }
  else {
    int hours = (int)seconds/(60*60);
    int mins = (int)seconds/60 - hours*60;
    int secs = (int)seconds - hours*60*60 - mins*60;
    std::string hoursStr = std::to_string(hours);
    std::string minsStr = mins < 1 ? "00" : (mins < 10 ? "0"+std::to_string(mins) : std::to_string(mins));
    std::string secsStr = secs < 1 ? "00" : (secs < 10 ? "0"+std::to_string(secs) : std::to_string(secs));
    return hoursStr + ":" + minsStr + ":" + secsStr;
  }
}


static std::wstring to_wstring(const std::string &str) {
  wchar_t wcs[str.size()+1];
  if(mbstowcs(wcs, str.c_str(), (int)str.size()+1) == (size_t)-1) {
    printf("Error at to_wstring(): Invalid character in str '%s'\n", str.c_str());
    return std::wstring(L"|.|.|");
  }
  return std::wstring(wcs);
}
static std::string to_string(const std::wstring &wstr) {
  char mbs[wstr.size()*4+1];
  wcstombs(mbs, wstr.c_str(), wstr.size());
  return std::string(mbs);
}
/*std::wstring to_wchar(const std::string &str) {
  wchar_t wcs[str.size()];
  mbstowcs(wcs, str.c_str(), (int)str.size());
  return std::wstring(wcs);
}
std::string to_char(const std::wstring &wstr) {
  char mbs[wstr.size()*4];
  wcstombs(mbs, wstr.c_str(), wstr.size());
  return std::string(mbs);
}*/


static std::string toNumbers(std::string str) {
  std::string strOut = "";
  for(int i=0; i<str.size(); i++) {
    strOut += std::to_string((int)str[i]);
    if(i < str.size()-1) strOut += ", ";
  }
  return strOut;
}

static std::wstring toNumbers(std::wstring wstr) {
  std::wstring wstrOut = L"";
  for(int i=0; i<wstr.size(); i++) {
    wstrOut += std::to_wstring((int)wstr[i]);
    if(i < wstr.size()-1) wstrOut += L", ";
  }
  return wstrOut;
}

static std::vector<std::string> split(std::string str) {
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


static std::vector<std::wstring> split(std::wstring str) {
  std::vector<std::wstring> words;
  int i = 0;
  while(i < str.size()) {
    int j = str.find_first_of(L" \n\t", i);
    if(j == std::wstring::npos) j = str.size();
    if(j-i > 0) {
      std::wstring word = str.substr(i, j-i);
      words.push_back(word);
    }
    i = j+1;
  }
  return words;
}


static void replace(std::string &str, const std::string &from, const std::string &to) {
  int i = 0;
  while((i = str.find(from, i)) != std::string::npos) {
    str.replace(i, from.size(), to);
  }
}

static void replace(std::wstring &str, const std::wstring &from, const std::wstring &to) {
  int i = 0;
  while((i = str.find(from, i)) != std::string::npos) {
    str.replace(i, from.size(), to);
  }
}


/*static int getClosestKeyAboveOrBelow(int i, auto &smap) {
  int below = INT_MIN, above = INT_MAX;

  for(const auto &keyValuePair : smap) {
    int key = keyValuePair.first;
    if(key == i) return key;
    if(key < i && key > below) below = key;
    if(key > i && key < above) above = key;
  }
  if(above != INT_MAX) return above;
  if(below != INT_MIN) return below;
  return -1;
}*/
template<class T>
static int getClosestKeyAboveOrBelow(int i, T &smap) {
  int below = INT_MIN, above = INT_MAX;

  for(const auto &keyValuePair : smap) {
    int key = keyValuePair.first;
    if(key == i) return key;
    if(key < i && key > below) below = key;
    if(key > i && key < above) above = key;
  }
  if(above != INT_MAX) return above;
  if(below != INT_MIN) return below;
  return -1;
}


static bool toDouble(const std::string &str, double &d) {
  bool r = true;
  try {
    d = std::stod(str);
  } catch(...) {
    r = false;
  }
  return r;
}

static bool toInt(const std::string &str, int &i) {
  bool r = true;
  try {
    i = std::stoi(str);
  } catch(...) {
    r = false;
  }
  return r;
}

static bool toLong(const std::string &str, long &l) {
  bool r = true;
  try {
    l = std::stol(str);
  } catch(...) {
    r = false;
  }
  return r;
}


static const std::string STR_DOUBLE = "double";
static const std::string STR_INT = "int";
static const std::string STR_STRING = "string";
static const std::string STR_WSTRING = "wstring";
static const std::string STR_BOOL = "bool";

static void fillValue(void *variable, const std::string &value, const std::string &type) {
  if(type.compare(STR_STRING) == 0) {
    std::string *str = static_cast<std::string*>(variable);
    *str = value;
  }
  if(type.compare(STR_WSTRING) == 0) {
    std::wstring *wstr = static_cast<std::wstring*>(variable);
    *wstr = to_wstring(value);
  }
  if(type.compare(STR_INT) == 0) {
    int *i = static_cast<int *>(variable);
    *i = std::stoi(value);
  }
  if(type.compare(STR_DOUBLE) == 0) {
    double *d = static_cast<double *>(variable);
    *d = std::stod(value);
  }
  if(type.compare(STR_BOOL) == 0) {
    bool *b = static_cast<bool *>(variable);
    *b = value.compare("true") == 0 || value.compare("1") == 0 || value.compare("yes") == 0 || value.compare("on") == 0;
  }
}

// FIXME check if variable types actually match (currently crash if not)
static void fillValue(void *variable, const std::wstring &value, const std::string &type) {
  if(type.compare(STR_STRING) == 0) {
    std::string *str = static_cast<std::string*>(variable);
    *str = to_string(value);
  }
  if(type.compare(STR_WSTRING) == 0) {
    std::wstring *wstr = static_cast<std::wstring*>(variable);
    *wstr = value;
  }
  if(type.compare(STR_INT) == 0) {
    int *i = static_cast<int *>(variable);
    *i = std::stoi(value);
  }
  if(type.compare(STR_DOUBLE) == 0) {
    double *d = static_cast<double *>(variable);
    *d = std::stod(value);
  }
  if(type.compare(STR_BOOL) == 0) {
    bool *b = static_cast<bool *>(variable);
    *b = value.compare(L"true") == 0 || value.compare(L"1") == 0 || value.compare(L"yes") == 0 || value.compare(L"on") == 0;
  }
}




static std::string getTimeString(const char *format = "%F_%T") {
  auto tp = std::chrono::system_clock::now();
  time_t tt = std::chrono::system_clock::to_time_t(tp);
  tm *ti = localtime(&tt);
  char buf[256];
  strftime(buf, 256, "%F_%T", ti);
  //printf("%s\n", buf);
  return std::string(buf);
}




static bool compareStrings(const std::string &a, const std::string &b) {
  return a.compare(b) < 0;
}

static void sortAlphabetically(std::vector<std::string> &v) {
  std::sort(v.begin(), v.end(), compareStrings);
}


static std::string extractFilename(const std::string &path) {
  int i = path.find_last_of("/");
  if(i != std::string::npos) {
    return path.substr(i+1);
  }
  return path;
}
static std::string extractDirectoryname(const std::string &path) {
  int i = path.find_last_of("/", path.size()-2);
  if(i != std::string::npos) {
    return path.substr(i+1);
  }
  return path;
}




/*std::string extractDirectoryName(const std::string &path) {
  int i = path.find_last_of("/");
  if(i != std::string::npos) {
    return path.substr(0, i);
  }
  return path;
}*/

static std::string extractFileExtension(const std::string &filename) {
  int i = filename.find_last_of(".");
  if(i != std::string::npos) {
    return filename.substr(i+1);
  }
  return "";
}



static inline bool doesFileExist(const std::string &path) {
  bool ret = false;
  try {
    ret = boost::filesystem::exists(path);
  }
  catch(const boost::filesystem::filesystem_error& ex) {
    printf("%s\n", ex.what());;
  }
  return ret;
}

static inline bool isDirectory(const std::string &path) {
  bool ret = false;
  try {
    ret = boost::filesystem::is_directory(path);
  }
  catch(const boost::filesystem::filesystem_error& ex) {
    printf("%s\n", ex.what());;
  }
  return ret;
}

static inline double getFileSize(const std::string &path) {
  bool ret = false;
  try {
    ret = boost::filesystem::file_size(path);
  }
  catch(const boost::filesystem::filesystem_error& ex) {
    printf("%s\n", ex.what());;
  }
  return ret;
}

static std::vector<std::string> getDirectoryContent(const std::string &path, bool getFiles = true) {
  std::vector<std::string> files;
  bool validDirectory = false;
  try {
    if(doesFileExist(path) && isDirectory(path)) {
      validDirectory = true;
      boost::filesystem::path directory(path);
      for(boost::filesystem::directory_entry &de : boost::filesystem::directory_iterator(directory)) {
        bool isFile = isDirectory(de.path().string());
        if((getFiles && isFile) || (!getFiles && !isFile)) {
          files.push_back(de.path().string());
        }
      }
    }
  }
  catch(const boost::filesystem::filesystem_error& ex) {
    if(validDirectory) {
      // FIXME
      files.push_back("(can't access the directory)");
    }
    printf("%s\n", ex.what());;
  }
  return files;
}


struct Directory {
  std::vector<Directory> directories;
  std::vector<std::string> files;
  std::vector<std::string> fileNames, directoryNames, directoryPaths;
  std::string path;
  
  bool includeHiddenFiles = false;

  Directory() {}
  Directory(const std::string &path) {
    this->path = path;
    getContent();
  }
  void getContent(const std::string &path, bool getDirectoriesRecursively = true) {
    this->path = path;
    getContent(getDirectoriesRecursively);
  }

  void getContent(bool getDirectoriesRecursively = true) {
    files = getDirectoryContent(path, false);
    directoryPaths = getDirectoryContent(path, true);
    if(getDirectoriesRecursively) {
      for(int i=0; i<directoryPaths.size(); i++) {
        directories.push_back(Directory(directoryPaths[i]));
      }
    }
    sortAlphabetically(files);
    sortAlphabetically(directoryPaths);
    
    fileNames.resize(files.size());
    for(int i=0; i<files.size(); i++) {
      fileNames[i] = extractFilename(files[i]);
    }
    directoryNames.resize(directoryPaths.size());
    for(int i=0; i<directoryPaths.size(); i++) {
      directoryNames[i] = extractFilename(directoryPaths[i]);
    }
    /*if(!includeHiddenFiles) {
      for(int i=0; i<fileNames.size(); i++) {
        if(fileNames[i].size() == 0 || fileNames[i].at(0) == '.') {
          fileNames.erase(fileNames.begin() + i);
          files.erase(files.begin() + i);
          i--;
        }
      }
      for(int i=0; i<directoryNames.size(); i++) {
        if(directoryNames[i].size() == 0 || directoryNames[i].at(0) == '.') {
          directoryNames.erase(directoryNames.begin() + i);
          directoryPaths.erase(directoryPaths.begin() + i);
          directories.erase(directories.begin() + i);
          i--;
        }
      }
    }*/
  }

  void getFilesRecursively(std::vector<std::string> &files) {
    files.insert(files.begin(), this->files.begin(), this->files.end());
    for(int i=0; i<directories.size(); i++) {
      directories[i].getFilesRecursively(files);
    }
    fileNames.resize(files.size());
    for(int i=0; i<files.size(); i++) {
      fileNames[i] = extractFilename(files[i]);
    }
  }

  void print(int mode = 0, int level = 0) {
    printIntendation(level);
    printf("| %s |\n", path.c_str());
    for(int i=0; i<files.size(); i++) {
      printIntendation(level + 1);
      printf("%s\n", files[i].c_str());
    }
    if(mode == 1) {
      for(int i=0; i<directories.size(); i++) {
        printIntendation(level + 1);
        printf("| %s |\n", directories[i].path.c_str());
      }
    }
    if(mode == 2) {
      for(int i=0; i<directories.size(); i++) {
        directories[i].print(mode, level + 1);
      }
    }
  }
};




static void createDirectories(std::string str) {
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



struct File
{
  static const int UNINITIALIZED = 0, CONTENT_READ = 1, OPEN_FAILED = -1, OPENED_FOR_WRITING = 2;
  int status = UNINITIALIZED;
  virtual ~File() {}
  virtual void open(std::string filename) {}
  virtual void save(std::string filename) {}
};


struct TextFile : public File
{
  std::string content;
  std::filebuf *buf = NULL;
  char *contentCstr = NULL;

  static bool Read(const std::string &filename, std::string &content) {
    if(filename.size() > 0) {
      TextFile file;
      file.readContent(filename);
      if(file.status == File::CONTENT_READ) {
        content = std::string(file.content);
        return true;
      }
    }
    return false;
  }
  static bool Write(const std::string &filename, const std::string &content) {
    if(filename.size() > 0) {
      TextFile file;
      file.openForWriting(filename);
      if(file.status == File::OPENED_FOR_WRITING) {
        file.writeToFile(content.c_str());
        file.finishWriting();
        return true;
      }
    }
    return false;
  }

  TextFile() {}

  TextFile(const std::string &filename) {
    readContent(filename);
  }

  ~TextFile() {
    if(contentCstr) {
      delete [] contentCstr;
    }
    if(buf != NULL) {
      delete buf;
    }
  }

  void writeToFile(const std::string &filename, const std::string &content) {
    openForWriting(filename);
    writeToFile(content);
    finishWriting();
  }

  void openForWriting(const std::string &filename) {
    buf = new std::filebuf();
    if(buf == NULL || buf->open(filename.c_str(), std::ios_base::out) == NULL) {
      printf("Error. Can't access file %s\n", filename.c_str());
      return;
    }
    status = OPENED_FOR_WRITING;
  }

  void writeToFile(const std::string &str) {
    if(status != OPENED_FOR_WRITING) {
      printf("Error. File not opened for writing.\n");
      return;
    }
    buf->sputn(str.c_str(), str.size());
  }

  void finishWriting() {
    buf->close();
    status = File::UNINITIALIZED;
  }

  void readContent(const std::string &filename) {
    buf = new std::filebuf();
    if(buf->open(filename.c_str(), std::ios_base::in) == NULL) {
      printf("Error. Can't open file %s\n", filename.c_str());
      status = OPEN_FAILED;
    }

    if(status != OPEN_FAILED) {
      std::streamsize size = buf->in_avail();
      contentCstr = new char[size+1];
      buf->sgetn(contentCstr, size);
      contentCstr[size] = '\0';
      content = std::string(contentCstr);
      buf->close();
      status = File::CONTENT_READ;
    }
  }

  void print() {
    printf("%s", content.c_str());
  }
};



struct Random
{
  static void init() {
    srand(time(0));
  }
  static int getInt() {
    return rand();
  }
  static int getInt(int min, int max) {
    return rand()%(1+max-min)+min;
  }
  static float getFloat(float min, float max) {
    return (float)rand()/RAND_MAX*(max-min)+min;
  }
  static float getFloat() {
    return (float)rand()/RAND_MAX;
  }
  static double getDouble(double min, double max) {
    return (double)rand()/RAND_MAX*(max-min)+min;
  }
  static double getDouble() {
    return (double)rand()/RAND_MAX;
  }
  static double getDouble(double min, double max, long seed) {
    srand(seed);
    return (double)rand()/RAND_MAX*(max-min)+min;
  }
  static double getDouble(long seed) {
    srand(seed);
    return (double)rand()/RAND_MAX;
  }

  static double getTrue(double p) {
    return (double)rand()/RAND_MAX < p;
  }

  template<class T>
  static T get(const std::vector<T> &v) {
    return v.size() == 0 ? 0 : v[Random::getInt(0, v.size()-1)];
  }

  template<class T>
  static T get(T a, T b) {
    return rand() % 2 == 0 ? a : b;
  }
  template<class T>
  static T get(T a, T b, T c) {
    long k = rand();
    return k % 3 == 0 ? a : (k % 3 == 1 ? b : c);
  }
  template<class T>
  static T get(T a, T b, T c, T d) {
    long k = rand();
    return k % 4 == 0 ? a : (k % 4 == 1 ? b :  (k % 4 == 2 ? c : d));
  }

};

/* Based on http://xoroshiro.di.unimi.it/xoroshiro128plus.c */
class Xoroshiro128plus
{
public:
  static constexpr double MAX = pow(2.0, 64.0);

  static void init() {
    jump();
  }

  static int getInt(int min, int max) {
    return next()%(1+max-min)+min;
  }

  static float getFloat(float min, float max) {
    return (float)next()/MAX*(max-min)+min;
  }

  static float getFloat() {
    return (float)next()/MAX;
  }

  static double getDouble(double min, double max) {
    return (double)next()/MAX*(max-min)+min;
  }

  static double getDouble() {
    return (double)next()/MAX;
  }

//private:
  static uint64_t s[2];

  static inline uint64_t rotl(const uint64_t x, int k) {
  	return (x << k) | (x >> (64 - k));
  }

  static uint64_t next() {
  	const uint64_t s0 = s[0];
  	uint64_t s1 = s[1];
  	const uint64_t result = s0 + s1;

  	s1 ^= s0;
  	s[0] = rotl(s0, 55) ^ s1 ^ (s1 << 14); // a, b
  	s[1] = rotl(s1, 36); // c

  	return result;
  }

  static void jump() {
  	static const uint64_t JUMP[] = { 0xbeac0467eba5facb, 0xd86b048b86aa9922 };

  	uint64_t s0 = 0;
  	uint64_t s1 = 0;
  	for(int i = 0; i < sizeof JUMP / sizeof *JUMP; i++)
  		for(int b = 0; b < 64; b++) {
  			if (JUMP[i] & UINT64_C(1) << b) {
  				s0 ^= s[0];
  				s1 ^= s[1];
  			}
  			next();
  		}

  	s[0] = s0;
  	s[1] = s1;
  }
};




/* Based on http://xoroshiro.di.unimi.it/xoroshiro128plus.c */
class Xorshift128plus
{
public:
  static constexpr double MAX = pow(2.0, 64.0);

  static void init() {
    jump();
  }

  static int getInt(int min, int max) {
    return next()%(1+max-min)+min;
  }

  static float getFloat(float min, float max) {
    return (float)next()/MAX*(max-min)+min;
  }

  static float getFloat() {
    return (float)next()/MAX;
  }

  static double getDouble(double min, double max) {
    return (double)next()/MAX*(max-min)+min;
  }

  static double getDouble() {
    return (double)next()/MAX;
  }

//private:
  static uint64_t s[2];

  static uint64_t next() {
    uint64_t s1 = s[0];
  	const uint64_t s0 = s[1];
  	const uint64_t result = s0 + s1;
  	s[0] = s0;
  	s1 ^= s1 << 23; // a
  	s[1] = s1 ^ s0 ^ (s1 >> 18) ^ (s0 >> 5); // b, c
  	return result;
  }

  static void jump() {
    static const uint64_t JUMP[] = { 0x8a5cd789635d2dff, 0x121fd2155c472f96 };

  	uint64_t s0 = 0;
  	uint64_t s1 = 0;
  	for(int i = 0; i < sizeof JUMP / sizeof *JUMP; i++)
  		for(int b = 0; b < 64; b++) {
  			if (JUMP[i] & UINT64_C(1) << b) {
  				s0 ^= s[0];
  				s1 ^= s[1];
  			}
  			next();
  		}

  	s[0] = s0;
  	s[1] = s1;
  }
};
