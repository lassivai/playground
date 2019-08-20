#pragma once
#include "util.h"


struct Message {
  std::wstring str;
  double timeStamp = 0;

  Message() {}

  Message(const std::wstring &str) {
    set(str);
  }

  void set(const std::wstring &str) {
    this->str = L"" + str;
    timeStamp = (double)SDL_GetTicks() * 0.001;
  }
};

struct MessageQueue
{
  double tmpQueueTime = 3;
  std::vector<Message*> permQueue;
  std::vector<Message*> tmpQueue;


  ~MessageQueue() {
    for(int i=0; i<permQueue.size(); i++) {
      delete permQueue[i];
    }
  }

  void addMessage(const std::string &str) {
    addMessage(to_wstring(str));
  }

  void addMessage(const std::wstring &str) {
    Message *msg = new Message(str);
    permQueue.push_back(msg);
    tmpQueue.push_back(msg);
  }

  void update() {
    double now = (double)SDL_GetTicks() * 0.001;
    for(int i=tmpQueue.size()-1; i >= 0; i--) {
      if(now - tmpQueue[i]->timeStamp >= tmpQueueTime) {
        tmpQueue.erase(tmpQueue.begin()+i);
      }
    }
  }
};

static MessageQueue messageQueue; // FIXME
