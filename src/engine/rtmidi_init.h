#pragma once
#include <rtmidi/RtMidi.h>
#include <string>
#include <vector>

static const std::vector<std::string> midiPortTypeNames = { "Midi Through", "Virtual", "Generic", "Launchpad", "Code 61" };

struct MidiMessage
{
  // Channel voice message status byte bits
  static constexpr unsigned char NoteOff = 8; //Key number, velocity
  static constexpr unsigned char NoteOn = 9; //Key number, velocity
  static constexpr unsigned char KeyPressure = 10; //Key number, pressure (polyphonic aftertouch)
  static constexpr unsigned char ControlChange = 11; // Conroller number, value
  static constexpr unsigned char ProgramChange = 12; // Program number
  static constexpr unsigned char ChannelPressure = 13; // Pressure (aftertouch)
  static constexpr unsigned char PitchBend = 14; // MSB, LSB

  // Control change message data bits
  static constexpr unsigned char BankSelectMSB = 0; // 0-127
  static constexpr unsigned char BankSelectLSB = 32; // 0-127
  static constexpr unsigned char ModulationWheel = 1; // 0-127
  static constexpr unsigned char FootController = 4; // 0-127
  static constexpr unsigned char PortamentoTime = 5; // 0-127
  static constexpr unsigned char ChannelVolume = 7; // 0-127
  static constexpr unsigned char Balance = 8; // 0-127
  static constexpr unsigned char Pan = 10; // 0-127
  static constexpr unsigned char ExpressionController = 11; // 0-127
  static constexpr unsigned char EffectControl1 = 12; // 0-127
  static constexpr unsigned char GeneralPurposeController1 = 16; // 0-127

  static constexpr unsigned char SustainPedal = 64; // 0-63/64-127 (off/on)
  static constexpr unsigned char Portamento = 65;
  static constexpr unsigned char Sostenuto = 66;
  static constexpr unsigned char SoftPedal = 67;
  static constexpr unsigned char LegatoFootswich = 68;


  // Channel mode message status byte bits
  static constexpr unsigned char ChannelModeMessage = 11;

  // Channel mode message data byte bits
  static constexpr unsigned char ResetControllers = 121;
  static constexpr unsigned char LocalControl = 122; // on/off (= 0/127)
  static constexpr unsigned char LocalControl_ValueOff = 0;
  static constexpr unsigned char LocalControl_ValueOn = 127;
  static constexpr unsigned char AllNotesOff = 123;
  static constexpr unsigned char OmniModeOff = 124;
  static constexpr unsigned char OmniModeOn = 125;
  static constexpr unsigned char MonoModeOn = 126; // number of channels
  static constexpr unsigned char PolyModeOn = 127;

  std::string toString() const {
    std::string msgStr = "";
    switch(statusByte >> 4) {
      case NoteOff:
        msgStr += "NoteOff, pitch " + std::to_string(dataByte1) + ", volume " + std::to_string(dataByte2);
        break;
      case NoteOn:
        msgStr += "NoteOn, pitch " + std::to_string(dataByte1) + ", volume " + std::to_string(dataByte2);
        break;
      case KeyPressure:
        msgStr += "KeyPressure";
        break;
      case ControlChange:
        msgStr += "ControlChange";
        if(dataByte1 == SustainPedal) {
          msgStr += ", sustain pedal " + std::to_string(dataByte2);
        }
        else if(dataByte1 == ModulationWheel) {
          msgStr += ", modulation wheel " + std::to_string(dataByte2);
        }
        else if(dataByte1 == ChannelVolume) {
          msgStr += ", channel volume " + std::to_string(dataByte2);
        }
        else if(dataByte1 == ExpressionController) {
          msgStr += ", expression controller " + std::to_string(dataByte2);
        }
        else if(dataByte1 == EffectControl1) {
          msgStr += ", effect control1 " + std::to_string(dataByte2);
        }
        else {
          msgStr += ", databyte_1 " + std::to_string(dataByte1) + ", databyte_2 " + std::to_string(dataByte2);
        }
        break;
      case ProgramChange:
        msgStr += "ProgramChange";
        msgStr += ", databyte_1 " + std::to_string(dataByte1) + ", databyte_2 " + std::to_string(dataByte2);
        break;
      case ChannelPressure:
        msgStr += "ChannelPressure";
        msgStr += ", databyte_1 " + std::to_string(dataByte1) + ", databyte_2 " + std::to_string(dataByte2);
        break;
      case PitchBend:
        msgStr += "PitchBend, MSB " + std::to_string(dataByte1) + ", LSB " + std::to_string(dataByte2);
        break;
      default:
        msgStr += "status byte high " + std::to_string(statusByte >> 4) + ", databyte_1 " + std::to_string(dataByte1) + ", databyte_2 " + std::to_string(dataByte2);
        break;
    }
    msgStr += ", channel " + std::to_string(statusByte & 4);
    return msgStr;
  }

  void print() const {
    printf("%s\n", toString().c_str());
  }
  
  MidiMessage &operator=(const MidiMessage &midiMessage) {
    statusByte = midiMessage.statusByte;
    dataByte1 = midiMessage.dataByte1;
    dataByte2 = midiMessage.dataByte2;
    controlChangeDirection = midiMessage.controlChangeDirection;
    stamp = midiMessage.stamp;
    midiPortIndex = midiMessage.midiPortIndex;
    return *this;
  }

  unsigned char statusByte = 0, dataByte1 = 0, dataByte2 = 0;
  char controlChangeDirection = 0;
  double stamp = 0;
  int midiPortIndex = 0;

  inline unsigned char getMessageChannel() const {
    return statusByte & 15;
  }
  inline unsigned char getMessageType() const {
    return (statusByte >> 4) & 15;
  }
  inline void setMessageType(unsigned char type) {
    statusByte = (statusByte & 15) | ((type & 15) << 4);
  }
  inline unsigned char getControlMessageType() const {
    return dataByte1;
  }
  inline unsigned char getChannelModeMessageType() const {
    return dataByte1;
  }

  
};

struct SysExMessage
{
  std::vector<unsigned char> bytes;
  
  static bool isSysExMessage(std::vector<unsigned char> bytes, int start = 0, int end = 0) {
    if(bytes.size() - start < 5) return false;
    bool isSysEx = bytes[start+0] == 0xF0;
    isSysEx = isSysEx && bytes[start+1] == 0x00;
    isSysEx = isSysEx && bytes[start+2] == 0x00;
    isSysEx = isSysEx && bytes[start+3] == 0x66;
    isSysEx = isSysEx && bytes[start+4] == 0x00;
    return isSysEx;
  }
  void beginMessage() {
    bytes = {0xF0, 0x00, 0x00, 0x66, 0x00};
  }
  void endMessage() {
    bytes.push_back(0xFC); // or F7
  }
  
  /* position (on a Mackie control LCD screen)
   *   From 00 to 37 (56 values) for the first line,
   *   From 38 to 6F (56 values) for the second line
   */
  void createLcdMessage(const std::string &str, unsigned char position = 0) {
    beginMessage();
    bytes.push_back(0x12); // LCD message id
    bytes.push_back(position);
    if(str.size() > 0) bytes.push_back(str[0]);
    if(str.size() > 1) bytes.push_back(str[1]);
    if(str.size() > 2) bytes.push_back(str[2]);
    if(str.size() > 3) bytes.push_back(str[3]);
    if(str.size() > 4) bytes.push_back(str[4]);
    if(str.size() > 5) bytes.push_back(str[5]);
    if(str.size() > 6) bytes.push_back(str[6]);
    endMessage();
  }
  
  const char hexDigits [16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
  std::string byteToHex(unsigned char byte) const {
    std::string str;
    int a = (byte >> 4) & 15;
    int b = byte & 15;
    return "0x" + hexDigits[a] + hexDigits[b];
  }
  
  std::string toString() const {
    std::string str;
    for(int i=0; i<bytes.size(); i++) {
      if(i > 0) str += " ";
      str += byteToHex(bytes[i]);
    }
    return str;
  }
};


struct HuiMessage
{
  enum Type { Unrecognised, DownPressed, LeftPressed, SetPressed, RightPressed, UpPressed, StopPressed, PlayPressed, RecordPressed };
  Type type = Unrecognised;
  int channel = 0;
  
  MidiMessage midiMessageA, midiMessageB;

  int decode(const std::vector<MidiMessage> &midiMessages, int i) {
    
    type = Unrecognised;
    channel = 0;
    if(i + 1 < midiMessages.size()) {
      if(midiMessages[i].getMessageType() == MidiMessage::ControlChange && midiMessages[i+1].getMessageType() == MidiMessage::ControlChange) {
        if(midiMessages[i].dataByte1 == 15 && midiMessages[i].dataByte2 == 13 && midiMessages[i+1].dataByte1 == 47) {
          if(midiMessages[i+1].dataByte2 == 64) {
            type = DownPressed;
          }
          if(midiMessages[i+1].dataByte2 == 65) {
            type = LeftPressed;
          }
          if(midiMessages[i+1].dataByte2 == 66) {
            type = SetPressed;
          }
          if(midiMessages[i+1].dataByte2 == 67) {
            type = RightPressed;
          }
          if(midiMessages[i+1].dataByte2 == 68) {
            type = UpPressed;
          }
        }
        if(midiMessages[i].dataByte1 == 15 && midiMessages[i].dataByte2 == 14 && midiMessages[i+1].dataByte1 == 47) {
          if(midiMessages[i+1].dataByte2 == 67) {
            type = StopPressed;
          }
          if(midiMessages[i+1].dataByte2 == 68) {
            type = PlayPressed;
          }
          if(midiMessages[i+1].dataByte2 == 69) {
            type = RecordPressed;
          }
        }
      }
      if(type != Unrecognised) {
        channel = midiMessages[i].statusByte & 4;
        return 1;
      }
    }
    return 0;
  }
  
  std::string toString() const {
    return "hui_message";
  }
};

enum MidiPortType { MidiThrough, Virtual, Generic, LaunchPad, Code61 /* etc. */ };

struct MidiMessageListener {
  virtual ~MidiMessageListener() {}
  //virtual void onMessages(std::vector<MidiMessage*> *midiMessages, MidiPortType midiPortType) = 0;
  virtual void onMessage(const MidiMessage &midiMessage, MidiPortType midiPortType) = 0;
};

struct MidiInterface
{
  //const std::vector<std::string> defaultInputPorts = {"Launchpad", "SWISSONIC EasyKeys61", "Keystation 88", "Keystation Mini 32"};
  //const std::vector<std::string> defaultOutputPorts = {"Launchpad"};
  
  struct MidiPort {

    std::vector<long> numNotesOn = std::vector<long>(128, 0), numNotesOff = std::vector<long>(128, 0);
    long totalNumMidiMessages = 0;
    
    int midiPortIndex = 0;
    MidiPortType type = Generic;
    std::string name, nameShort;
    std::vector<RtMidiIn *> midiIns;
    std::vector<RtMidiOut*> midiOuts;
    

    int numMidiMessagesOnPreviousUpdate = 0;
    std::vector<MidiMessage> previousProgramChanges = std::vector<MidiMessage>(16); // FIXME

    void update(std::vector<MidiMessageListener*> &listeners) {      
      numMidiMessagesOnPreviousUpdate = 0;
      for(int i=0; i<midiIns.size(); i++) {
        //MidiMessage *midiMessage = NULL;
        //while((midiMessage = getMessage(midiIns[i]))) {
        MidiMessage midiMessage;
        while(getMessage(midiIns[i], midiMessage) && numMidiMessagesOnPreviousUpdate < 20) {
          // Every once in a while the midi port receives exactly 101 midi messages which causes crash in MidiEventPanel
          numMidiMessagesOnPreviousUpdate++;
          
          for(int p=0; p<listeners.size(); p++) {
            listeners[p]->onMessage(midiMessage, type);
          }
        }
      }
    }
    bool getMessage(RtMidiIn *midiIn, MidiMessage &midiMessage) {
      std::vector<unsigned char> message;
      double stamp = midiIn->getMessage(&message);
      if(message.size() < 2) {
        return false;
      }
      
      midiMessage.statusByte = message[0];
      midiMessage.dataByte1 = message[1];
      midiMessage.dataByte2 = message.size() > 2 ? message[2] : 0;
      midiMessage.stamp = stamp;
      midiMessage.midiPortIndex = midiPortIndex;
      
      if(midiMessage.getMessageType() == MidiMessage::NoteOn && midiMessage.dataByte2 == 0) {
        midiMessage.setMessageType(MidiMessage::NoteOff);
      }
      
      if(midiMessage.getMessageType() == MidiMessage::NoteOn) {
        numNotesOn[midiMessage.dataByte1]++;
      }
      else if(midiMessage.getMessageType() == MidiMessage::NoteOff) {
        numNotesOff[midiMessage.dataByte1]++;
      }
      totalNumMidiMessages++;
            
      int channel = midiMessage.getMessageChannel();
      
      if(midiMessage.getMessageType() == MidiMessage::ProgramChange) {
        if(channel >= 0 && channel < 16) {
          char prevProgam = previousProgramChanges[channel].dataByte1;
          char currProgam = midiMessage.dataByte1;
          if(currProgam - prevProgam > 0 && currProgam - prevProgam < 64) {
            midiMessage.controlChangeDirection = 1;
          }
          else if(currProgam - prevProgam != 0) {
            midiMessage.controlChangeDirection = -1;
          }
        }
        previousProgramChanges[channel] = midiMessage;
      }

      return true;
    }
    /*MidiMessage *getMessage(RtMidiIn *midiIn, MidiMessage *midiMessage = NULL) {
      std::vector<unsigned char> message;
      double stamp = midiIn->getMessage(&message);
      if(message.size() < 2) {
        return NULL;
      }
      
      MidiMessage *midiMessage = new MidiMessage();
      
      midiMessage->statusByte = message[0];
      midiMessage->dataByte1 = message[1];
      midiMessage->dataByte2 = message.size() > 2 ? message[2] : 0;
      midiMessage->stamp = stamp;
      midiMessage->midiPortIndex = midiPortIndex;
      
      if(midiMessage->getMessageType() == MidiMessage::NoteOn && midiMessage->dataByte2 == 0) {
        midiMessage->setMessageType(MidiMessage::NoteOff);
      }
      
      if(midiMessage->getMessageType() == MidiMessage::NoteOn) {
        numNotesOn[midiMessage->dataByte1]++;
      }
      else if(midiMessage->getMessageType() == MidiMessage::NoteOff) {
        numNotesOff[midiMessage->dataByte1]++;
      }
      totalNumMidiMessages++;
      
      //if(midiMessage->getMessageType() == MidiMessage::NoteOn) {
        //midiLatencyTestTicToc.tic();
        //midiLatencyTestMidiTimeStamp = stamp;
        //midiLatencyTestActivated = true;
        //printf("midi latency test started at time stamp %f\n", midiLatencyTestMidiTimeStamp);
      //}
      
      int channel = midiMessage->getMessageChannel();
      
      if(midiMessage->getMessageType() == MidiMessage::ProgramChange) {
        if(channel >= 0 && channel < 16) {
          char prevProgam = previousProgramChanges[channel].dataByte1;
          char currProgam = midiMessage->dataByte1;
          if(currProgam - prevProgam > 0 && currProgam - prevProgam < 64) {
            midiMessage->controlChangeDirection = 1;
          }
          else if(currProgam - prevProgam != 0) {
            midiMessage->controlChangeDirection = -1;
          }
        }
        previousProgramChanges[channel] = *midiMessage;
      }

      return midiMessage;
    }*/
    
  };
  

  std::vector<MidiPort*> midiPorts;
  
  std::vector<MidiMessage> midiMessages;
  
  RtMidiIn *midiIn;
  RtMidiOut *midiOut;

  
  std::vector<MidiMessageListener*> midiMessageListeners;
  
  int updateIntervalNanoSecs = 1000*10;
  bool updateThreadStopRequested = false;
  bool isUpdateThreadRunning = false;
  std::thread updateThread;
  
  void startUpdateThread() {
    if(!isUpdateThreadRunning) {
      updateThreadStopRequested = false;
      updateThread = std::thread(midiUpdateThread, this);
    }
  }
  void stopUpdateThread() {
    if(isUpdateThreadRunning) {
      updateThreadStopRequested = true;
      updateThread.join();
    }
  }
  
  static void midiUpdateThread(MidiInterface *midiInterface) {
    midiInterface->isUpdateThreadRunning = true;
    while(!midiInterface->updateThreadStopRequested) {
      midiInterface->update();
      std::this_thread::sleep_for(std::chrono::nanoseconds(midiInterface->updateIntervalNanoSecs));
    }
    midiInterface->isUpdateThreadRunning = false;
  }
  
  std::vector<long> numNotesOn = std::vector<long>(128, 0), numNotesOff = std::vector<long>(128, 0);
  
  void update() {
    int mp = midiPorts.size();
    for(int i=0; i<mp; i++) {
      midiPorts[i]->update(midiMessageListeners);
      if(midiPorts[i]->numMidiMessagesOnPreviousUpdate > 0) {
        printf("Midi port '%s' received %d midi messages (total %lu)\n", midiPorts[i]->nameShort.c_str(), midiPorts[i]->numMidiMessagesOnPreviousUpdate, midiPorts[i]->totalNumMidiMessages);

        /*for(int r=0; r<128; r++) {
          if(midiPorts[i]->numNotesOn[r] > 0 || midiPorts[i]->numNotesOff[r] > 0) {
            printf("pitch %d, %lu/%lu\n", r, midiPorts[i]->numNotesOn[r], midiPorts[i]->numNotesOff[r]);
          }
        }*/
      }
    }
  }
  
  void addMidiMessageListener(MidiMessageListener *midiMessageListener) {
    midiMessageListeners.push_back(midiMessageListener);
  }
  void removeMidiMessageListener(MidiMessageListener *midiMessageListener) {
    for(int i=0; i<midiMessageListeners.size(); i++) {
      if(midiMessageListeners[i] == midiMessageListener) {
        midiMessageListeners.erase(midiMessageListeners.begin()+i);
      }
    }
    midiMessageListeners.push_back(midiMessageListener);
  }
  /*void update() {
    midiMessages.clear();
    for(int i=0; i<midiPorts.size(); i++) {
      midiPorts[i]->update();
      for(int k=0; k<midiPorts[i]->midiMessages.size(); k++)
      midiMessages.push_back(midiPorts[i]->midiMessages[k]);
    }
  }*/

  virtual ~MidiInterface() {
    for(int i=0; i<midiPorts.size(); i++) {
      delete midiPorts[i];
    }
  }

  int count(const std::string &str, char c) {
    int n = 0;
    for(int i=0; i<str.size(); i++) {
      if(str[i] == c) n++;
    }
    return n;
  }
  
  void splitString(const std::string &src, std::string &dstA, std::string &dstB, char c, bool lastOf = true) {
    int index = lastOf ? src.find_last_of(c) : src.find_first_of(c);
    dstA = src.substr(0, index);
    dstB = src.substr(index+1);
  }

  void createMidiPort(const std::string &portName, int portIndex) {
    MidiPort *midiPort = new MidiPort();
    if(appendMidiPort(midiPort, portIndex)) {
      splitString(portName, midiPort->name, midiPort->nameShort, ':');
      if(portIndex < 0) {
        midiPort->type = MidiPortType::Virtual;
      }
      else {
        for(int i=0; i<midiPortTypeNames.size(); i++) {
          if(midiPortTypeNames[i].compare(0, midiPortTypeNames[i].size(), midiPort->name, 0, midiPortTypeNames[i].size()) == 0) {
            midiPort->type = (MidiPortType)i;
            break;
          }
        }
      }
      midiPort->midiPortIndex = midiPorts.size();
      midiPorts.push_back(midiPort);
    }
  }
  
  bool appendMidiPort(MidiPort *midiPort, int portIndex) {
    RtMidiIn *midiIn = NULL;
    RtMidiOut *midiOut = NULL;
    try {
      midiIn = new RtMidiIn();
      midiOut = new RtMidiOut();
    }
    catch(RtMidiError &error) {
      error.printMessage();
      return false;
    }
    if(midiIn && midiOut) {
      if(portIndex >= 0) {
        midiIn->openPort(portIndex);
        midiOut->openPort(portIndex);
      }
      else {
        midiIn->openVirtualPort(midiPort->name);
        midiOut->openVirtualPort(midiPort->name);
      }
      midiPort->midiIns.push_back(midiIn);
      midiPort->midiOuts.push_back(midiOut);
      return true;
    }
    return false;
  }



  
  void setup() {
    RtMidiIn *midiIn = NULL;
            
    printf("Setting up RtMidi...\n");
    try {
      midiIn = new RtMidiIn();
    }
    catch(RtMidiError &error) {
      error.printMessage();
      return;
    }
    unsigned int numInputPorts = midiIn->getPortCount();

    std::string portName;
    printf("Available midi devices\n");
    for(int i=0; i<numInputPorts; i++) {
      try {
        portName = midiIn->getPortName(i);
        makeTitleCase(portName);
      }
      catch(RtMidiError &error) {
        error.printMessage();
        return;
      }
      printf("1. %s\n", portName.c_str());

      int portId = -1;
      if(count(portName, ':') >= 2) {
        std::string portIdStr = portName.substr(portName.find_last_of(':')+1);
        portName = portName.substr(0, portName.find_last_of(':'));
        toInt(portIdStr, portId);
      }
      if(portId == 0) {
        createMidiPort(portName, i);
      }
      else if(portId > 0) {
        MidiPort *midiPort = NULL;
        for(int k=0; k<midiPorts.size(); k++) {
          if(midiPorts[k]->name == portName) {
            midiPort = midiPorts[k];
            break;
          }
        }
        if(midiPort) {
          appendMidiPort(midiPort, i);
        }
      }
    }
    midiIn->closePort();
    delete midiIn;
    
    createMidiPort("Virtual port", -1);
    this->midiIn = midiPorts[0]->midiIns[0];
    this->midiOut = midiPorts[0]->midiOuts[0];


    printf("Midi connections:\n");
    
    for(int i=0; i<midiPorts.size(); i++) {
      if(midiPorts[i]->midiIns.size() > 1) {
        printf("%d. %s, type: %s, ports: %d)\n", i+1, midiPorts[i]->name.c_str(), midiPortTypeNames[midiPorts[i]->type].c_str(), (int)midiPorts[i]->midiIns.size());
      }
      else {
        printf("%d. %s, type: %s\n", i+1, midiPorts[i]->name.c_str(), midiPortTypeNames[midiPorts[i]->type].c_str());
      }
    }
    
    startUpdateThread();
  }



  void sendMessage(const MidiMessage &msg) {
    sendMessage(msg.statusByte, msg.dataByte1, msg.dataByte2);
  }

  void sendPortMessage(int midiPortIndex, unsigned char statusByte, unsigned char dataByte1 = 0, unsigned char dataByte2 = 0) {
    if(midiPortIndex < 0 || midiPortIndex >= midiPorts.size()) return;
    unsigned char message[] = {statusByte, dataByte1, dataByte2};
    unsigned char statusByteBits = (statusByte >> 4) & 15;
    
    for(int i=0; i<midiPorts[midiPortIndex]->midiOuts.size(); i++) {
      RtMidiOut *midiOut = midiPorts[midiPortIndex]->midiOuts[i];
      if(statusByteBits == MidiMessage::NoteOn) {
        midiOut->sendMessage(message, 3);
      }
      if(statusByteBits == MidiMessage::NoteOff) {
        midiOut->sendMessage(message, 3);
      }
      if(statusByteBits == MidiMessage::KeyPressure) {
        midiOut->sendMessage(message, 3);
      }
      if(statusByteBits == MidiMessage::ControlChange) {
        midiOut->sendMessage(message, 3);
      }
      if(statusByteBits == MidiMessage::ProgramChange) {
        midiOut->sendMessage(message, 2);
      }
      if(statusByteBits == MidiMessage::ChannelPressure) {
        midiOut->sendMessage(message, 2);
      }
      if(statusByteBits == MidiMessage::PitchBend) {
        midiOut->sendMessage(message, 3);
      }
    }
  }

  void sendMessage(unsigned char statusByte, unsigned char dataByte1 = 0, unsigned char dataByte2 = 0) {
    unsigned char message[] = {statusByte, dataByte1, dataByte2};
    unsigned char statusByteBits = (statusByte >> 4) & 15;
    if(statusByteBits == MidiMessage::NoteOn) {
      MidiMessage msg;
      midiOut->sendMessage(message, 3);
    }
    if(statusByteBits == MidiMessage::NoteOff) {
      midiOut->sendMessage(message, 3);
    }
    if(statusByteBits == MidiMessage::KeyPressure) {
      midiOut->sendMessage(message, 3);
    }
    if(statusByteBits == MidiMessage::ControlChange) {
      midiOut->sendMessage(message, 3);
    }
    if(statusByteBits == MidiMessage::ProgramChange) {
      midiOut->sendMessage(message, 2);
    }
    if(statusByteBits == MidiMessage::ChannelPressure) {
      midiOut->sendMessage(message, 2);
    }
    if(statusByteBits == MidiMessage::PitchBend) {
      midiOut->sendMessage(message, 3);
    }
  }

  void sendChannelMessage(unsigned char statusByteBits, unsigned char channel, unsigned char dataByte1 = 0, unsigned char dataByte2 = 0) {
    unsigned char message[] = {(unsigned char)(((statusByteBits&15) << 4) | channel), dataByte1, dataByte2};

    if(statusByteBits == MidiMessage::NoteOn) {
      midiOut->sendMessage(message, 3);
    }
    if(statusByteBits == MidiMessage::NoteOff) {
      midiOut->sendMessage(message, 3);
    }
    if(statusByteBits == MidiMessage::KeyPressure) {
      midiOut->sendMessage(message, 3);
    }
    if(statusByteBits == MidiMessage::ControlChange) {
      midiOut->sendMessage(message, 3);
    }
    if(statusByteBits == MidiMessage::ProgramChange) {
      midiOut->sendMessage(message, 2);
    }
    if(statusByteBits == MidiMessage::ChannelPressure) {
      midiOut->sendMessage(message, 2);
    }
    if(statusByteBits == MidiMessage::PitchBend) {
      midiOut->sendMessage(message, 3);
    }
  }

  void sendNoteOnMessage(unsigned char pitch, unsigned char volume, unsigned char channel = 0) {
    sendChannelMessage(MidiMessage::NoteOn, channel, pitch, volume);
  }
  void sendNoteOffMessage(unsigned char pitch, unsigned char volume, unsigned char channel = 0) {
    sendChannelMessage(MidiMessage::NoteOff, channel, pitch, volume);
  }


  void sendSysExMessage(const SysExMessage &sysExMessage, unsigned char channel = 0) {
    midiOut->sendMessage(sysExMessage.bytes.data(), sysExMessage.bytes.size());
  }


  void quit() {
    stopUpdateThread();
    if(midiIn != NULL) delete midiIn;
    if(midiOut != NULL) delete midiOut;
  }
};


/*struct MidiHub
{
  std::vector<MidiInterface> midiInterfaces;
  
  void checkConnections() {
    
  }
  
};*/


