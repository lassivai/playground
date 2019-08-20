#include <cstdio>
#include <vector>

int noteOffMessage(unsigned char key, unsigned char velocity, unsigned char *message) {
  message[0] = (1<<7) | (8<<4) | 15;
  message[1] = key;
  message[2] = velocity;
  return 3;
}
int noteOnMessage(unsigned char key, unsigned char velocity, unsigned char *message) {
  message[0] = (1<<7) | (9<<4) | 15;
  message[1] = key;
  message[2] = velocity;
  return 3;
}
int programChangeMessage(unsigned char program, unsigned char *message) {
  message[0] = (1<<7) | (12<<4) | 15;
  message[1] = program;
  return 2;
}

int main()
{
  std::vector<unsigned char> messages;
  unsigned char message[3];
  int len = noteOnMessage(169, 77, message);
  messages.insert(messages.end(), message, message+len);
  len = noteOffMessage(169, 171, message);
  messages.insert(messages.end(), message, message+len);
  len = programChangeMessage(42, message);
  messages.insert(messages.end(), message, message+len);

  printf("Message queue length %lu chars\n", messages.size());

  for(int i=0; i<messages.size(); i++) {
    unsigned char c = messages[i];
    if(((c>>4) & 15) == 8) {
      printf("Note off, %u, %u\n", messages[i+1], messages[i+2]);
      i += 2;
    }
    if(((c>>4) & 15) == 9) {
      printf("Note on, %u, %u\n", messages[i+1], messages[i+2]);
      i += 2;
    }
    if(((c>>4) & 15) == 12) {
      printf("Program change, %u\n", messages[i+1]);
      i += 1;
    }
  }
}

void sendMessage() {
  unsigned char noteOff = 8;
  unsigned char noteOn = 9;
  unsigned char keyPressure = 10;
  unsigned char controlChange = 11;
  unsigned char programChange = 12;
  unsigned char channelPressure = 13;
  unsigned char pitchBend = 14;
}
