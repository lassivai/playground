#include "../sketch.h"
#include "sndfileio.h"
#include "../util.h"

#define MINIMP3_IMPLEMENTATION
#include "../external/minimp3_ex.h"
static mp3dec_t mp3d;
static bool isMinimp3Initialized = false;

//#include <bento4/Ap4.h>
//#include <bento4/Bento4C.h>


static const long maxSamplesRead = 96000*2*60*20;

//struct AudioFileIo {
  
   bool AudioFileIo::save(const std::string &filename, const AudioRecording &audioRecording) {
    long numSamples = audioRecording.seeker;
    std::vector<float> samples = std::vector<float>(numSamples * 2);
    double max = 1.0;
    if(audioRecording.normalize) {
      max = 0.00001;
      for(int i=0; i<numSamples; i++) {
        if(audioRecording.samples[i].x > max) max = audioRecording.samples[i].x;
        if(audioRecording.samples[i].y > max) max = audioRecording.samples[i].y;
      }
    }
    for(long i=0; i<numSamples; i++) {
      samples[i*2+0] = audioRecording.samples[i].x / max;
      samples[i*2+1] = audioRecording.samples[i].y / max;
    }
    return save(filename, audioRecording.sampleRate, samples, numSamples*2);
  }

  bool AudioFileIo::save(const std::string filename, int sampleRate, const std::vector<float> &samples, long size, AudioFileMetaData *audioFileMetaData) {
    std::string fileExtension = extractFileExtension(filename);

    int fileMajorType, fileSubType;
    int endian = SF_ENDIAN_FILE;

    if(fileExtension.compare("wav") == 0) {
      fileMajorType = SF_FORMAT_WAV;
      fileSubType = SF_FORMAT_FLOAT;
    }
    else if(fileExtension.compare("aiff") == 0) {
      fileMajorType = SF_FORMAT_AIFF;
      fileSubType = SF_FORMAT_FLOAT;
    }
    else if(fileExtension.compare("ogg") == 0) {
      fileMajorType = SF_FORMAT_OGG;
      fileSubType = SF_FORMAT_VORBIS;
    }
    else if(fileExtension.compare("flac") == 0) {
      fileMajorType = SF_FORMAT_FLAC;
      fileSubType = SF_FORMAT_PCM_24; //SF_FORMAT_ALAC_24, SF_FORMAT_ALAC_32
    }
    else if(fileExtension.compare("") == 0) {
      fileMajorType = SF_FORMAT_RAW;
      fileSubType = SF_FORMAT_FLOAT;
    }
    else {
      printf("Error at AudioFileIo.save: unsupported fileformat %s", filename.c_str());
      return false;
    }

    long numSamples = (size > 0 ? min(samples.size(), size) : samples.size()) / 2;

    SF_INFO sfinfo;
    memset(&sfinfo, 0, sizeof(sfinfo));
    sfinfo.format = fileMajorType | fileSubType;
    sfinfo.samplerate = sampleRate;
    sfinfo.channels = 2;
    sfinfo.frames = numSamples;

    SNDFILE *sndfile;

    if((sndfile = sf_open(filename.c_str(), SFM_WRITE, &sfinfo)) == NULL) {
      printf("Error at AudioFileIo.save: unable to open file %s for writing. %s\n", filename.c_str(), sf_strerror(NULL));
      return false;
    }

    for(long l=0; l<numSamples*2; l+=sampleRate*2) {
      long n = min(sampleRate*2, numSamples*2-l);
      if(sf_write_float(sndfile, &samples[l], n) != n) {
        printf("Error at AudioFileIo.save: unable to write to file %s. %s\n", filename.c_str(), sf_strerror(sndfile));
        return false;
      }
    }
    /*if(sf_write_float(sndfile, samples.data(), numSamples * 2) != numSamples * 2) {
      printf("Error at AudioFileIo.save: unable to write to file %s. %s\n", filename.c_str(), sf_strerror(sndfile));
      return false;
    }*/

    if(audioFileMetaData) {
      sf_set_string(sndfile, SF_STR_TITLE, audioFileMetaData->title.c_str());
      sf_set_string(sndfile, SF_STR_COPYRIGHT, audioFileMetaData->copyright.c_str());
      sf_set_string(sndfile, SF_STR_SOFTWARE, audioFileMetaData->software.c_str());
      sf_set_string(sndfile, SF_STR_ARTIST, audioFileMetaData->artist.c_str());
      sf_set_string(sndfile, SF_STR_COMMENT, audioFileMetaData->comment.c_str());
      sf_set_string(sndfile, SF_STR_DATE, audioFileMetaData->date.c_str());
      sf_set_string(sndfile, SF_STR_ALBUM, audioFileMetaData->album.c_str());
      sf_set_string(sndfile, SF_STR_LICENSE, audioFileMetaData->license.c_str());
      sf_set_string(sndfile, SF_STR_TRACKNUMBER, audioFileMetaData->trackNumber.c_str());
      sf_set_string(sndfile, SF_STR_GENRE, audioFileMetaData->genre.c_str());
    }
    
    sf_write_sync(sndfile);

    sf_close(sndfile);

    return true;
  }

  bool AudioFileIo::open(const std::string &filename, AudioRecordingTrack &audioRecordingTrack) {
    return open(filename, audioRecordingTrack.audioRecording, &audioRecordingTrack.audioFileMetaData);
  }
  // FIXME to AudioRecording class
  
  /*static bool open(const std::string &filename, AudioRecording &audioRecording, AudioFileMetaData *audioFileMetaData = NULL) {
    std::vector<float> samples;
    SF_INFO sfinfo;
    if(open(filename, samples, sfinfo, audioFileMetaData) && samples.size() > 0) {
      audioRecording.samples.resize(samples.size()/2);
      for(int i=0; i*2+1<samples.size(); i++) {
        audioRecording.samples[i].x = samples[i*2];
        audioRecording.samples[i].y = samples[i*2+1];
      }
      audioRecording.numVariableSamples = audioRecording.samples.size();
      return true;
    }
    return false;
  }*/
  
  bool AudioFileIo::open(const std::string &filename, AudioRecording &audioRecording, AudioFileMetaData *audioFileMetaData) {
    std::string fileExtension = extractFileExtension(filename);
    if(fileExtension == "mp3") {
      return openMp3(filename, audioRecording, audioFileMetaData);
    }
    /*else if(fileExtension == "mp4" || fileExtension == "m4a") {
      return openMp4(filename, audioRecording, audioFileMetaData);
    }*/

    std::vector<Vec2d> samples;
    SF_INFO sfinfo;
    if(open(filename, samples, sfinfo, audioFileMetaData) && samples.size() > 0) {
      long numSamplesDst = (long)((double)samples.size() * audioRecording.sampleRate / sfinfo.samplerate);
      // FIXME
      if(numSamplesDst == samples.size()) {
        audioRecording.samples = samples;
      }
      else if(numSamplesDst != 0 && samples.size() != 0) {
        audioRecording.samples.resize(numSamplesDst);
        resizeArray(samples, audioRecording.samples);
        printf("samplerate: %d -> %d\n", (int)sfinfo.samplerate, (int)audioRecording.sampleRate);
      }
      /*audioRecording.samples.resize(numSamplesDst);
      for(int i=0; i*2+1<samples.size(); i++) {
        audioRecording.samples[i].x = samples[i*2];
        audioRecording.samples[i].y = samples[i*2+1];
      }*/
      audioRecording.numVariableSamples = audioRecording.samples.size();
      return true;
    }
    return false;
  }

    /*bool AudioFileIo::openMp4(const std::string &filename, AudioRecording &audioRecording, AudioFileMetaData *audioFileMetaData) {
        
        
    }*/

  bool AudioFileIo::openMp3(const std::string &filename, AudioRecording &audioRecording, AudioFileMetaData *audioFileMetaData) {
    if(!isMinimp3Initialized) {
      mp3dec_init(&mp3d);
      isMinimp3Initialized = true;
    }

    mp3dec_file_info_t info;
    
    
    if(!mp3dec_load(&mp3d, filename.c_str(), &info, NULL, NULL)) {
      printf("MP3 '%s' loaded, samples = %lu, channels = %d, hz = %d, layer = %d, avg_bitrate_kbps = %d\n", filename.c_str(), info.samples, info.channels, info.hz, info.layer, info.avg_bitrate_kbps);

      std::vector<Vec2d> samples(info.samples/2);
      double maxVal = 0;
      int k = info.channels > 1 ? 1 : 0;
      for(long i=0; i<info.samples/2; i++) {
        samples[i].set(info.buffer[i*info.channels], info.buffer[i*info.channels+k]);
        maxVal = max(maxVal, max(fabs(samples[i].x), fabs(samples[i].y)));
      }
      if(maxVal != 0) {
        for(long i=0; i<info.samples/2; i++) {
          samples[i] /= maxVal;
        }
      }

      long numSamplesDst = (long)((double)samples.size() * audioRecording.sampleRate / info.hz);

      if(numSamplesDst == samples.size()) {
        audioRecording.samples = samples;
      }
      else if(numSamplesDst != 0 && samples.size() != 0) {
        audioRecording.samples.resize(numSamplesDst);
        resizeArray(samples, audioRecording.samples);
        printf("samplerate: %d -> %d\n", (int)info.hz, (int)audioRecording.sampleRate);
      }

      audioRecording.numVariableSamples = audioRecording.samples.size();

      return true;
    }
    else {
        printf("Failed to load MP3 '%s', samples = %lu, channels = %d, hz = %d, layer = %d, avg_bitrate_kbps = %d\n", filename.c_str(), info.samples, info.channels, info.hz, info.layer, info.avg_bitrate_kbps);
      return false;
    }
  }
const std::vector<std::string> trackTypes = { "TYPE_UNKNOWN", "TYPE_AUDIO", "TYPE_VIDEO", "TYPE_SYSTEM", "TYPE_HINT", "TYPE_TEXT", "TYPE_JPEG", "TYPE_RTP"};




  bool AudioFileIo::open(const std::string &filename, std::vector<Vec2d> &samples, SF_INFO &sfinfo, AudioFileMetaData *audioFileMetaData) {
    //static short buffer [numBufferItems] ;
  	SNDFILE *sndfile;
  	int	k, audio_device, numItemsRead, majorFormat, subFormat;
    const int numBufferItems = 1000;

    memset(&sfinfo, 0, sizeof(sfinfo)) ;

  	if(!(sndfile = sf_open(filename.c_str(), SFM_READ, &sfinfo))) {
      printf("At AudioFileIo.open: failed to open file '%s' - %s\n", filename.c_str(), sf_strerror(NULL));
      return false;
  	}

    if(sfinfo.channels < 1 || sfinfo.channels > 2) {
      printf("At AudioFileIo.open: file %s contains invalid number of channels \n", filename.c_str());
      return false;
  	}

    majorFormat = sfinfo.format & SF_FORMAT_TYPEMASK;
    subFormat = sfinfo.format & SF_FORMAT_SUBMASK;

    printf("File %s opened\n", filename.c_str());
    printf(" -frames %lu\n", sfinfo.frames);
    printf(" -samplerate %d\n", sfinfo.samplerate);
    printf(" -channels %d\n", sfinfo.channels);
    printf(" -format %d = %s, %s\n", sfinfo.format, getMajorFormatString(majorFormat).c_str(), getMinorFormatString(subFormat).c_str());
    printf(" -sections %d\n", sfinfo.sections);
    printf(" -seekable %d\n", sfinfo.seekable);

    long sampleCount = min(sfinfo.channels * sfinfo.frames, maxSamplesRead);
    samples.resize(sampleCount/2);
    int n = 0;
    float maxValue = 1.0;
    
    double buffer[numBufferItems];
	while((numItemsRead = sf_read_double(sndfile, buffer, numBufferItems)) > 0) {
      if(samples.size() < n+numItemsRead/2) samples.resize(n+numItemsRead/2);
        for(int i=0; i+1<numItemsRead && n < maxSamplesRead; i+=2) {
          samples[n].x = buffer[i];
          samples[n].y = buffer[i+1];
          maxValue = max(maxValue, samples[n].x);
          maxValue = max(maxValue, samples[n].y);
          n++;
      }
    }
    
    if(maxValue != 1.0) {
      printf("At AudioFileIo::open(), maxValue = %f\n", maxValue);
      for(int i=0; i<samples.size(); i++) {
        samples[i].x = samples[i].x / maxValue;
        samples[i].y = samples[i].y / maxValue;
      }
    }
    //if(subFormat == SF_FORMAT_FLOAT) {
  	//}
    /*else if(subFormat == SF_FORMAT_DOUBLE) {
      double buffer[numBufferItems*sfinfo.channels];
      while((numItemsRead = sf_readf_double(sndfile, buffer, numBufferItems))) {
        if(samples.size() < n+numItemsRead) samples.resize(n+numItemsRead);
        for(int i=0; i<numItemsRead; i++) {
          samples[n] = scale * buffer[i];
          n++;
        }
      }
    }
  	else if(subFormat == SF_FORMAT_SHORT) {
      short buffer[numBufferItems*sfinfo.channels];
  		while((numItemsRead = sf_readf_short(sndfile, buffer, numBufferItems))) {
        if(samples.size() < n+numItemsRead) samples.resize(n+numItemsRead);
        for(int i=0; i<numItemsRead; i++) {
          samples[n] = scale * buffer[i];
          n++;
        }
  		}
  	}
    else if(subFormat == SF_FORMAT_INT) {
      int buffer[numBufferItems*sfinfo.channels];
  		while((numItemsRead = sf_readf_int(sndfile, buffer, numBufferItems))) {
        if(samples.size() < n+numItemsRead) samples.resize(n+numItemsRead);
        for(int i=0; i<numItemsRead; i++) {
          samples[n] = scale * buffer[i];
          n++;
        }
  		}
  	}*/

    if(audioFileMetaData) {
      audioFileMetaData->title = sf_get_string(sndfile, SF_STR_TITLE) ? sf_get_string(sndfile, SF_STR_TITLE) : "";
      audioFileMetaData->copyright = sf_get_string(sndfile, SF_STR_COPYRIGHT) ? sf_get_string(sndfile, SF_STR_COPYRIGHT) : "";
      audioFileMetaData->software = sf_get_string(sndfile, SF_STR_SOFTWARE) ? sf_get_string(sndfile, SF_STR_SOFTWARE) : "";
      audioFileMetaData->artist = sf_get_string(sndfile, SF_STR_ARTIST) ? sf_get_string(sndfile, SF_STR_ARTIST) : "";
      audioFileMetaData->comment = sf_get_string(sndfile, SF_STR_COMMENT) ? sf_get_string(sndfile, SF_STR_COMMENT) : "";
      audioFileMetaData->date = sf_get_string(sndfile, SF_STR_DATE) ? sf_get_string(sndfile, SF_STR_DATE) : "";
      audioFileMetaData->album = sf_get_string(sndfile, SF_STR_ALBUM) ? sf_get_string(sndfile, SF_STR_ALBUM) : "";
      audioFileMetaData->license = sf_get_string(sndfile, SF_STR_LICENSE) ? sf_get_string(sndfile, SF_STR_LICENSE) : "";
      audioFileMetaData->trackNumber = sf_get_string(sndfile, SF_STR_TRACKNUMBER) ? sf_get_string(sndfile, SF_STR_TRACKNUMBER) : "";
      audioFileMetaData->genre = sf_get_string(sndfile, SF_STR_GENRE) ? sf_get_string(sndfile, SF_STR_GENRE) : "";
      audioFileMetaData->print();
    }
  	sf_close(sndfile);

  	return true;
  }
  
  
  std::string AudioFileIo::getMajorFormatString(int majorFormat) {
    switch (majorFormat) {
      case SF_FORMAT_WAV: return "wav"; /* Microsoft WAV format (little endian). */
      case SF_FORMAT_AIFF: return "aiff";     /* Apple/SGI AIFF format (big endian). */
      case SF_FORMAT_AU: return "au";     /* Sun/NeXT AU format (big endian). */
      case SF_FORMAT_RAW: return "raw";     /* RAW PCM data. */
      case SF_FORMAT_PAF: return "paf";     /* Ensoniq PARIS file format. */
      case SF_FORMAT_SVX: return "svx";     /* Amiga IFF / SVX8 / SV16 format. */
      case SF_FORMAT_NIST: return "nist";     /* Sphere NIST format. */
      case SF_FORMAT_VOC: return "voc";     /* VOC files. */
      case SF_FORMAT_IRCAM: return "ircam";    /* Berkeley/IRCAM/CARL */
      case SF_FORMAT_W64: return "w64";     /* Sonic Foundry's 64 bit RIFF/WAV */
      case SF_FORMAT_MAT4: return "mat4";     /* Matlab (tm) V4.2 / GNU Octave 2.0 */
      case SF_FORMAT_MAT5: return "mat5";     /* Matlab (tm) V5.0 / GNU Octave 2.1 */
      case SF_FORMAT_PVF: return "pvf";     /* Portable Voice Format */
      case SF_FORMAT_XI: return "xi";     /* Fasttracker 2 Extended Instrument */
      case SF_FORMAT_HTK: return "htk";     /* HMM Tool Kit format */
      case SF_FORMAT_SDS: return "sds";     /* Midi Sample Dump Standard */
      case SF_FORMAT_AVR: return "avr";     /* Audio Visual Research */
      case SF_FORMAT_WAVEX: return "wavex";     /* MS WAVE with WAVEFORMATEX */
      case SF_FORMAT_SD2: return "sd2";     /* Sound Designer 2 */
      case SF_FORMAT_FLAC: return "flac";     /* FLAC lossless file format */
      case SF_FORMAT_CAF: return "caf";     /* Core Audio File format */
      case SF_FORMAT_WVE: return "wve";     /* Psion WVE format */
      case SF_FORMAT_OGG: return "ogg";     /* Xiph OGG container */
      case SF_FORMAT_MPC2K: return "mpc2k";     /* Akai MPC 2000 sampler */
      case SF_FORMAT_RF64: return "rf64";     /* RF64 WAV file */
    }
    return "unsupported_major_format";
  }

  std::string AudioFileIo::getMinorFormatString(int minorFormat) {
    switch (minorFormat) {
      case SF_FORMAT_PCM_S8: return "pcm_s8";       /* Signed 8 bit data */
      case SF_FORMAT_PCM_16: return "pcm_16";       /* Signed 16 bit data */
      case SF_FORMAT_PCM_24: return "pcm_24";       /* Signed 24 bit data */
      case SF_FORMAT_PCM_32: return "pcm_32";       /* Signed 32 bit data */
      case SF_FORMAT_PCM_U8: return "pcm_u8";       /* Unsigned 8 bit data (WAV and RAW only) */
      case SF_FORMAT_FLOAT: return "float";       /* 32 bit float data */
      case SF_FORMAT_DOUBLE: return "double";       /* 64 bit float data */
      case SF_FORMAT_ULAW: return "ulaw";       /* U-Law encoded. */
      case SF_FORMAT_ALAW: return "alaw";       /* A-Law encoded. */
      case SF_FORMAT_IMA_ADPCM: return "ima_adpcm";       /* IMA ADPCM. */
      case SF_FORMAT_MS_ADPCM: return "ms_adpcm";       /* Microsoft ADPCM. */
      case SF_FORMAT_GSM610: return "gsm610";       /* GSM 6.10 encoding. */
      case SF_FORMAT_VOX_ADPCM: return "vox_adpcm";       /* Oki Dialogic ADPCM encoding. */
      case SF_FORMAT_G721_32: return "g721_32";       /* 32kbs G721 ADPCM encoding. */
      case SF_FORMAT_G723_40: return "g723_40";       /* 40kbs G723 ADPCM encoding. */
      case SF_FORMAT_G723_24: return "g723_24";       /* 24kbs G723 ADPCM encoding. */
      case SF_FORMAT_DWVW_12: return "dwvw_12";       /* 12 bit Delta Width Variable Word encoding. */
      case SF_FORMAT_DWVW_16: return "dwvw_16";       /* 16 bit Delta Width Variable Word encoding. */
      case SF_FORMAT_DWVW_24: return "dwvw_24";       /* 24 bit Delta Width Variable Word encoding. */
      case SF_FORMAT_DWVW_N: return "dwvw_n";       /* N bit Delta Width Variable Word encoding. */
      case SF_FORMAT_DPCM_8: return "dpcm_8";       /* 8 bit differential PCM (XI only) */
      case SF_FORMAT_DPCM_16: return "dpcm_16";       /* 16 bit differential PCM (XI only) */
      case SF_FORMAT_VORBIS: return "vorbis";       /* Xiph Vorbis encoding. */
    }
    return "unsupported_minor_format";
  }
  
  std::string AudioFileIo::getEndiannessString(int endianness) {
    switch (endianness) {
      case SF_ENDIAN_FILE: "default_file_endianness";  /* Default file endian-ness. */
      case SF_ENDIAN_LITTLE: "little_endian";   /* Force little endian-ness. */
      case SF_ENDIAN_BIG: "big_endian";   /* Force big endian-ness. */
      case SF_ENDIAN_CPU: "cpu_endian";   /* Force CPU endian-ness. */
    }
    return "unsupported_endianness";
  }


  

//};
