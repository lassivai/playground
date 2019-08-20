#pragma once
//#include <sndfile.h>
//#include "recording.h"

#include <sndfile.h>
#include "recording.h"


struct AudioFileIo {
  
  static bool save(const std::string &filename, const AudioRecording &audioRecording);

  static bool save(const std::string filename, int sampleRate, const std::vector<float> &samples, long size = 0, AudioFileMetaData *audioFileMetaData = NULL);
  static bool open(const std::string &filename, AudioRecordingTrack &audioRecordingTrack);
  
  static bool open(const std::string &filename, AudioRecording &audioRecording, AudioFileMetaData *audioFileMetaData = NULL);
  
  static bool openMp3(const std::string &filename, AudioRecording &audioRecording, AudioFileMetaData *audioFileMetaData);
  
  /*static bool openMp4(const std::string &filename, AudioRecording &audioRecording, AudioFileMetaData *audioFileMetaData);*/

  static bool open(const std::string &filename, std::vector<Vec2d> &samples, SF_INFO &sfinfo, AudioFileMetaData *audioFileMetaData = NULL);
  
  static std::string getMajorFormatString(int majorFormat);


  static std::string getMinorFormatString(int minorFormat);
  
  static std::string getEndiannessString(int endianness);


  

};
