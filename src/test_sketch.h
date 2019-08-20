#pragma once
#include "engine/sketch.h"

struct TestSketch : public Sketch
{

  void onInit() {

    /*if(cliArgs.numValues("-file") > 0) {
      filename = cliArgs.getValues("-file")[0];
    }*/
/*
    std::string filename = "instrument.proto";

    InstrumentFileIO ifp;

    if(!ifp.open(filename)) {
      printf("Failed to open file %s\n", filename.c_str());
    }
    else if(!ifp.parse()) {
      printf("Invalid instrument file %s\n", filename.c_str());
    }
    else {
      ifp.print();
    }
*/



  }



  void onUpdate() {
    for(int i=0; i<commandQueue.commands.size(); i++) {
      Command *cmd = NULL;
    }
  }


  void onQuit() {
  }

  void onKeyUp() {
  }

  void onKeyDown() {
  }

  void onMouseWheel() {
  }

  void onMousePressed() {
  }

  void onMouseReleased() {
  }

  void onMouseMotion() {
  }




  void onDraw() {
    clear(0, 0, 0, 1);


    if(events.textInput.inputGrabbed) {
      console.render(sdlInterface);
    }



  }




};

/*
int findMatchingEndingBracket(const std::string &str, int start, char startingBracket = '{', char endingBracket = '}') {
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

std::vector<std::string> extractNumericParameter(const std::string &name, std::string &str) {
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

std::vector<std::string> extractStringParameter(const std::string &name, std::string &str) {
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

void printIntendation(int level, int numSpaces = 2) {
  for(int i=0; i<level*numSpaces; i++) {
    printf(" ");
  }
}

void printParameters(const std::vector<std::vector<std::string>> &parameters, int level) {
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

struct PostProcessingEffectString {
  std::vector<std::vector<std::string>> parameters;

  bool parse(std::string str) {
    //printf("(debugging) at PostProcessingEffectString.parse()\n");
    parameters.push_back(extractStringParameter("type", str));
    parameters.push_back(extractNumericParameter("gain", str));
    parameters.push_back(extractNumericParameter("wet", str));
    while(true) {
      std::vector<std::string> out = extractNumericParameter("out", str);
      if(out.size() > 1) parameters.push_back(out);
      else break;
    }
    while(true) {
      std::vector<std::string> in = extractNumericParameter("in", str);
      if(in.size() > 1) parameters.push_back(in);
      else break;
    }
    return true;
  }

  void print(int level) {
    printIntendation(level);
    printf("effect {\n");
    printParameters(parameters, level+1);
    printIntendation(level);
    printf("}\n");
  }
};

struct PartialsString {
  std::vector<std::vector<std::string>> parameters;

  bool parse(std::string str) {
    //printf("(debugging) at PartialsString.parse()\n");
    parameters.push_back(extractStringParameter("mode", str));
    parameters.push_back(extractNumericParameter("count", str));
    parameters.push_back(extractNumericParameter("frequencyParameters", str));
    parameters.push_back(extractNumericParameter("gainParameters", str));
    parameters.push_back(extractNumericParameter("frequencies", str));
    parameters.push_back(extractNumericParameter("gains", str));
    return true;
  }
  void print(int level) {
    printIntendation(level);
    printf("partials {\n");
    printParameters(parameters, level+1);
    printIntendation(level);
    printf("}\n");
  }

};

struct AmplitudeEnvelopeString {
  std::vector<std::vector<std::string>> parameters;

  bool parse(std::string str) {
    //printf("(debugging) at AmplitudeEnvelopeString.parse()\n");
    parameters.push_back(extractStringParameter("type", str));
    parameters.push_back(extractNumericParameter("parameters", str));
    return true;
  }
  void print(int level) {
    printIntendation(level);
    printf("amplitudeEnvelope {\n");
    printParameters(parameters, level+1);
    printIntendation(level);
    printf("}\n");
  }

};
struct FrequencyEnvelopeString {
  std::vector<std::vector<std::string>> parameters;

  bool parse(std::string str) {
    //printf("(debugging) at FrequencyEnvelopeString.parse()\n");
    parameters.push_back(extractStringParameter("type", str));
    parameters.push_back(extractNumericParameter("parameters", str));
    return true;
  }
  void print(int level) {
    printIntendation(level);
    printf("frequencyEnvelope {\n");
    printParameters(parameters, level+1);
    printIntendation(level);
    printf("}\n");
  }

};

struct WaveformString {
  std::vector<std::vector<std::string>> parameters;
  PartialsString partialsString;

  bool parse(std::string str) {
    //printf("(debugging) at WaveformString.parse()\n");
    int ind = 0, end = 0;

    if((ind = str.find("partials")) != std::string::npos) {
      end = findMatchingEndingBracket(str, ind, '{', '}');
      if(end == -1) {
        printf("Error at WaveformString.parse(): no ending bracket found for partials\n");
        return false;
      }
      if(!partialsString.parse(str.substr(ind, end-ind+1))) {
        return false;
      }
      str.erase(ind, end-ind+1);
    }

    parameters.push_back(extractStringParameter("type", str));
    parameters.push_back(extractStringParameter("wavetableMode", str));
    parameters.push_back(extractNumericParameter("waveformParameters", str));
    parameters.push_back(extractNumericParameter("phaseOffsets", str));
    parameters.push_back(extractNumericParameter("lockedStereoPhase", str));
    parameters.push_back(extractNumericParameter("numSteps", str));
    parameters.push_back(extractNumericParameter("gaussianSmoothing", str));

    return true;
  }

  void print(int level) {
    printIntendation(level);
    printf("waveform {\n");
    printParameters(parameters, level+1);
    partialsString.print(level+1);
    printIntendation(level);
    printf("}\n");
  }

};

struct VoiceString {
  std::vector<std::vector<std::string>> parameters;
  WaveformString waveformString;
  AmplitudeEnvelopeString amplitudeEnvelopeString;
  FrequencyEnvelopeString frequencyEnvelopeString;

  bool parse(std::string str) {
    //printf("(debugging) at VoiceString.parse()\n");
    int ind = 0, end = 0;

    if((ind = str.find("waveform")) != std::string::npos) {
      end = findMatchingEndingBracket(str, ind, '{', '}');
      if(end == -1) {
        printf("Error at VoiceString.parse(): no ending bracket found for waveform\n");
        return false;
      }
      if(!waveformString.parse(str.substr(ind, end-ind+1))) {
        return false;
      }
      str.erase(ind, end-ind+1);
    }
    if((ind = str.find("amplitudeEnvelope")) != std::string::npos) {
      end = findMatchingEndingBracket(str, ind, '{', '}');
      if(end == -1) {
        printf("Error at VoiceString.parse(): no ending bracket found for amplitudeEnvelope\n");
        return false;
      }
      if(!amplitudeEnvelopeString.parse(str.substr(ind, end-ind+1))) {
        return false;
      }
      str.erase(ind, end-ind+1);
    }
    if((ind = str.find("frequencyEnvelope")) != std::string::npos) {
      end = findMatchingEndingBracket(str, ind, '{', '}');
      if(end == -1) {
        printf("Error at VoiceString.parse(): no ending bracket found for frequencyEnvelope\n");
        return false;
      }
      if(!frequencyEnvelopeString.parse(str.substr(ind, end-ind+1))) {
        return false;
      }
      str.erase(ind, end-ind+1);
    }
    parameters.push_back(extractNumericParameter("volume", str));
    parameters.push_back(extractNumericParameter("tuning", str));
    parameters.push_back(extractNumericParameter("stereoPhaseOffset", str));
    parameters.push_back(extractNumericParameter("unison", str));
    parameters.push_back(extractNumericParameter("unisonDetuning", str));
    parameters.push_back(extractNumericParameter("unisonDetuningRandomSeeds", str));

    return true;
  }

  void print(int level) {
    printIntendation(level);
    printf("voice {\n");
    printParameters(parameters, level+1);
    waveformString.print(level+1);
    amplitudeEnvelopeString.print(level+1);
    frequencyEnvelopeString.print(level+1);
    printIntendation(level);
    printf("}\n");
  }
};
struct AmplitudeModulatorString {
  std::vector<std::vector<std::string>> parameters;
  WaveformString waveformString;
  AmplitudeEnvelopeString amplitudeEnvelopeString;

  bool parse(std::string str) {
    //printf("(debugging) at AmplitudeModulatorString.parse()\n");
    int ind = 0, end = 0;
    if((ind = str.find("waveform")) != std::string::npos) {
      end = findMatchingEndingBracket(str, ind, '{', '}');
      if(end == -1) {
        printf("Error at AmplitudeModulatorString.parse(): no ending bracket found for waveform\n");
        return false;
      }
      if(!waveformString.parse(str.substr(ind, end-ind+1))) {
        return false;
      }
      str.erase(ind, end-ind+1);
    }
    if((ind = str.find("amplitudeEnvelope")) != std::string::npos) {
      end = findMatchingEndingBracket(str, ind, '{', '}');
      if(end == -1) {
        printf("Error at AmplitudeModulatorString.parse(): no ending bracket found for amplitudeEnvelope\n");
        return false;
      }
      if(!amplitudeEnvelopeString.parse(str.substr(ind, end-ind+1))) {
        return false;
      }
      str.erase(ind, end-ind+1);
    }
    parameters.push_back(extractNumericParameter("depth", str));
    parameters.push_back(extractNumericParameter("frequency", str));
    parameters.push_back(extractNumericParameter("stereoPhaseOffset", str));
    parameters.push_back(extractNumericParameter("stereoPhaseDifference", str));
    parameters.push_back(extractNumericParameter("relativeMode", str));

    return true;
  }

  void print(int level) {
    printIntendation(level);
    printf("amplitudeModulator {\n");
    printParameters(parameters, level+1);
    waveformString.print(level+1);
    amplitudeEnvelopeString.print(level+1);
    printIntendation(level);
    printf("}\n");
  }
};

struct FrequencyModulatorString {
  std::vector<std::vector<std::string>> parameters;
  WaveformString waveformString;
  AmplitudeEnvelopeString amplitudeEnvelopeString;

  bool parse(std::string str) {
    //printf("(debugging) at FrequencyModulatorString.parse()\n");
    int ind = 0, end = 0;
    if((ind = str.find("waveform")) != std::string::npos) {
      end = findMatchingEndingBracket(str, ind, '{', '}');
      if(end == -1) {
        printf("Error at FrequencyModulatorString.parse(): no ending bracket found for waveform\n");
        return false;
      }
      if(!waveformString.parse(str.substr(ind, end-ind+1))) {
        return false;
      }
      str.erase(ind, end-ind+1);
    }
    if((ind = str.find("amplitudeEnvelope")) != std::string::npos) {
      end = findMatchingEndingBracket(str, ind, '{', '}');
      if(end == -1) {
        printf("Error at FrequencyModulatorString.parse(): no ending bracket found for amplitudeEnvelope\n");
        return false;
      }
      if(!amplitudeEnvelopeString.parse(str.substr(ind, end-ind+1))) {
        return false;
      }
      str.erase(ind, end-ind+1);
    }
    parameters.push_back(extractNumericParameter("depth", str));
    parameters.push_back(extractNumericParameter("frequency", str));
    parameters.push_back(extractNumericParameter("stereoPhaseOffset", str));
    parameters.push_back(extractNumericParameter("stereoPhaseDifference", str));

    return true;
  }

  void print(int level) {
    printIntendation(level);
    printf("frequencyModulator {\n");
    printParameters(parameters, level+1);
    waveformString.print(level+1);
    amplitudeEnvelopeString.print(level+1);
    printIntendation(level);
    printf("}\n");
  }
};

struct OscillatorSetString {
  std::vector<std::vector<std::string>> parameters;
  std::vector<VoiceString> voiceStrings;
  std::vector<FrequencyModulatorString> fmStrings;
  std::vector<AmplitudeModulatorString> amStrings;

  bool parse(std::string str) {
    //printf("(debugging) at OscillatorSetString.parse()\n%s\n", str.c_str());
    int ind = 0, end = 0;

    while((ind = str.find("voice")) != std::string::npos) {
      end = findMatchingEndingBracket(str, ind, '{', '}');
      if(end == -1) {
        printf("Error at OscillatorSetString.parse(): no ending bracket found for voice\n");
        return false;
      }
      VoiceString vs;
      if(!vs.parse(str.substr(ind, end-ind+1))) {
        return false;
      }
      voiceStrings.push_back(vs);
      str.erase(ind, end-ind+1);
    }
    end = 0;
    while((ind = str.find("amplitudeModulator")) != std::string::npos) {
      end = findMatchingEndingBracket(str, ind, '{', '}');
      if(end == -1) {
        printf("Error at OscillatorSetString.parse(): no ending bracket found for am\n");
        return false;
      }
      AmplitudeModulatorString as;
      if(!as.parse(str.substr(ind, end-ind+1))) {
        return false;
      }
      amStrings.push_back(as);
      str.erase(ind, end-ind+1);
    }
    end = 0;
    while((ind = str.find("frequencyModulator")) != std::string::npos) {
      end = findMatchingEndingBracket(str, ind, '{', '}');
      if(end == -1) {
        printf("Error at OscillatorSetString.parse(): no ending bracket found for fm\n");
        return false;
      }
      FrequencyModulatorString fs;
      if(!fs.parse(str.substr(ind, end-ind+1))) {
        return false;
      }
      fmStrings.push_back(fs);
      str.erase(ind, end-ind+1);
    }

    parameters.push_back(extractNumericParameter("volume", str));

    return true;
  }

  void print(int level) {
    printIntendation(level);
    printf("oscillatorSet {\n");
    printParameters(parameters, level+1);
    for(int i=0; i<voiceStrings.size(); i++) {
      voiceStrings[i].print(level+1);
    }
    for(int i=0; i<fmStrings.size(); i++) {
      fmStrings[i].print(level+1);
    }
    for(int i=0; i<amStrings.size(); i++) {
      amStrings[i].print(level+1);
    }
    printIntendation(level);
    printf("}\n");
  }
};




struct InstrumentString {
  std::vector<std::vector<std::string>> parameters;
  std::vector<PostProcessingEffectString> effectStrings;
  std::vector<OscillatorSetString> oscillatorSetStrings;

  bool parse(std::string str) {
    //printf("(debugging) at InstrumentString.parse()\n%s\n", str.c_str());
    int ind = 0, end = 0;

    while((ind = str.find("effect")) != std::string::npos) {
      end = findMatchingEndingBracket(str, ind, '{', '}');
      if(end == -1) {
        printf("Error at InstrumentString.parse(): no ending bracket found for effect\n");
        return false;
      }
      PostProcessingEffectString es;
      if(!es.parse(str.substr(ind, end-ind+1))) {
        return false;
      }
      effectStrings.push_back(es);
      str.erase(ind, end-ind+1);
      //printf("(debugging) at InstrumentString.parse()\n%s\n", str.c_str());
    }
    end = 0;
    while((ind = str.find("oscillatorSet")) != std::string::npos) {
      end = findMatchingEndingBracket(str, ind, '{', '}');
      if(end == -1) {
        printf("Error at InstrumentString.parse(): no ending bracket found for set\n");
        return false;
      }
      OscillatorSetString os;
      if(!os.parse(str.substr(ind, end-ind+1))) {
        return false;
      }
      oscillatorSetStrings.push_back(os);
      str.erase(ind, end-ind+1);
      //printf("(debugging) at InstrumentString.parse()\n%s\n", str.c_str());
    }

    parameters.push_back(extractStringParameter("name", str));
    parameters.push_back(extractNumericParameter("volume", str));

    return true;
  }

  void print() {
    printf("instrument {\n");
    printParameters(parameters, 1);
    for(int i=0; i<effectStrings.size(); i++) {
      effectStrings[i].print(1);
    }
    for(int i=0; i<oscillatorSetStrings.size(); i++) {
      oscillatorSetStrings[i].print(1);
    }
    printf("}\n");
  }
};







struct InstrumentFileParser
{
  std::string filename;
  std::string content;
  std::vector<InstrumentString> instrumentStrings;

  bool open(const std::string &filename) {
    this->filename = filename;
    return TextFile::getFileContent(filename, content);
  }


  bool parse() {
    //printf("(debugging) at InstrumentFileParser.parse()\n");

    if(content.size() == 0) {
      printf("Error at InstrumentFileParser.parse(): No content\n");
      return false;
    }
    int numOpeningCurlyBrackets = 0;
    int numClosingCurlyBrackets = 0;
    for(int i=0; i<content.size(); i++) {
      if(content[i] == '{') numOpeningCurlyBrackets++;
      if(content[i] == '}') numClosingCurlyBrackets++;
    }

    std::string str = content;
    int ind = 0, end = 0;
    while((ind = str.find("instrument")) != std::string::npos) {
      end = findMatchingEndingBracket(str, ind, '{', '}');
      if(end == -1) {
        printf("Error at InstrumentFileParser.parse(): no ending bracket found for instrument\n");
        return false;
      }
      //printf("(debugging) at InstrumentFileParser.parse(), %d, %d\n", ind, end);
      InstrumentString is;
      if(!is.parse(str.substr(ind, end-ind+1))) {
        return false;
      }
      instrumentStrings.push_back(is);
      str.erase(ind, end-ind+1);
    }

    return true;
  }

  void print() {
    printf("File %s contains %lu instruments:\n", filename.c_str(), instrumentStrings.size());

    for(int i=0; i<instrumentStrings.size(); i++) {
      instrumentStrings[i].print();
    }
  }

};
*/
