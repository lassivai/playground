#include <cstdio>
#include <midifile/MidiFile.h>
#include <string>
#include <cstring>


void print(smf::MidiFile &mf, bool showEvents = false) {
  mf.doTimeAnalysis();

  printf("Midi file: %s\n", mf.getFilename());
  printf("Track count %d\n", mf.getTrackCount());
  //printf("Num tracks %d\n", mf.getNumTracks());
  //printf("size %d\n", mf.size());
  printf("Duration in ticks %d\n", mf.getFileDurationInTicks());
  printf("Duration in quarters %.4f\n", mf.getFileDurationInQuarters());
  printf("Duration in seconds %.4f\n", mf.getFileDurationInSeconds());
  printf("Ticks per quarter %d\n", mf.getTicksPerQuarterNote());

  for(int i=0; i<mf.getNumTracks(); i++) {
    printf("Track %d: events %d\n", i, mf.getNumEvents(i));
    if(showEvents) {
      for(int k=0; k<mf.getNumEvents(i); k++) {
	smf::MidiEvent me = mf.getEvent(i, k);
        printf("  - ");
        for(int p=0; p<me.size(); p++) {
	  printf("%d ", me[p]);
	}
	printf("\n");
      }
    }
  }
  printf("\n");

}


int main(int argn, char **argv) {

  if(argn < 2) {
    printf("usage: %s filename [-showevents]\n", argv[0]);
    return 0;
  }

  bool showEvents = false;
  std::string filename;

  for(int i=1; i<argn; i++) {
    if(strcmp(argv[i], "-showevents") == 0) {
      showEvents = true;
    }
    else {
      filename = argv[i];
    }
  }

  printf("Testing Midi File lib\n\n");

  smf::MidiFile mf;

  if(mf.read(filename)) {
    //printf("Midi file '%s' opened!\n", filename.c_str());
    print(mf, showEvents);
  }
  else {
    printf("Failed to load midi file '%s'\n", filename.c_str());
  }
}



