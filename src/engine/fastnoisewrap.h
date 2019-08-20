#include "external/fastnoise/FastNoise.h"
#include "textfileparser.h"

struct FastNoise : public FastNoise_, public HierarchicalTextFileParser {
  static std::string getClassName() {
    return "fastNoise";
  }
  
  virtual std::string getBlockName() {
    return getClassName();
  }
  
  virtual void decodeParameters() {
  	
  	for(int i=0; i<doubleParameters.size(); i++) {
  		if(doubleParameters[i].values.size() == 0) continue;
  		
  		if(doubleParameters[i].name.compare("noiseType") == 0) {
  			SetNoiseType((FastNoise_::NoiseType)doubleParameters[i].values[0]);
  		}
  		else if(doubleParameters[i].name.compare("frequency") == 0) {
  			SetFrequency(doubleParameters[i].values[0]);
  		}
  		else if(doubleParameters[i].name.compare("seed") == 0) {
  			SetSeed(doubleParameters[i].values[0]);
  		}
  		else if(doubleParameters[i].name.compare("interpolation") == 0) {
  			SetInterp((FastNoise_::Interp)doubleParameters[i].values[0]);
  		}
  		else if(doubleParameters[i].name.compare("fractalType") == 0) {
  			SetFractalType((FastNoise_::FractalType)doubleParameters[i].values[0]);
  		}
  		else if(doubleParameters[i].name.compare("fractalOctaves") == 0) {
  			SetFractalOctaves(doubleParameters[i].values[0]);
  		}
  		else if(doubleParameters[i].name.compare("fractalLacunarity") == 0) {
  			SetFractalLacunarity(doubleParameters[i].values[0]);
  		}
  		else if(doubleParameters[i].name.compare("fractalGain") == 0) {
  			SetFractalGain(doubleParameters[i].values[0]);
  		}
  		else if(doubleParameters[i].name.compare("cellularDistanceFunction") == 0) {
  			SetCellularDistanceFunction((FastNoise_::CellularDistanceFunction)doubleParameters[i].values[0]);
  		}
  		else if(doubleParameters[i].name.compare("cellularReturnType") == 0) {
  			SetCellularReturnType((FastNoise_::CellularReturnType)doubleParameters[i].values[0]);
  		}
  	}
  }
  
  virtual void encodeParameters() {
    clearParameters();
  	putNumericParameter("noiseType", (double)GetNoiseType());
  	putNumericParameter("frequency", (double)GetFrequency());
  	putNumericParameter("seed", (double)GetSeed());
  	putNumericParameter("interpolation", (double)GetInterp());
  	putNumericParameter("fractalType", (double)GetFractalType());
  	putNumericParameter("fractalOctaves", (double)GetFractalOctaves());
  	putNumericParameter("fractalLacunarity", (double)GetFractalLacunarity());
  	putNumericParameter("fractalGain", (double)GetFractalGain());
  	putNumericParameter("cellularDistanceFunction", (double)GetCellularDistanceFunction());
  	putNumericParameter("cellularReturnType", (double)GetCellularReturnType());
  }
};