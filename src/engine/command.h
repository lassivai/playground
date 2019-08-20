#pragma once
#include "util.h"
#include "message.h"




struct Command
{
  std::vector<std::string> command;
  std::string str;
  double timeStamp = 0;

  Command() {}

  Command(const std::string &str) {
    set(str);
  }

  int getNumArguments() {
    return max(0, (int)command.size() - 1);
  }

  void set(const std::string &str) {
    command = split(str);
    timeStamp = (double)SDL_GetTicks() * 0.001;

    for(std::string &str : command) {
      replace(str, "<_>", " ");
    }

    if(command.size() > 0) {
      this->str = command[0];
      this->str += "(";
      for(int i=1; i<command.size(); i++) {
        this->str += command[i];
        if(i < command.size()-1) {
          this->str += ", ";
        }
      }
      this->str += ")";
    }
  }


};

struct CommandTemplate
{
  std::string commandName;
  std::vector<std::string> arguments;
  std::vector<std::string> argumentTypes;
  std::vector<std::string> argumentDefaultValues;
  std::string strTemplate = "";
  int numRequiredArguments = 0;

  void addArgument(std::string name, std::string type, std::string defaultValue = "") {
    arguments.push_back(name);
    argumentTypes.push_back(type);
    argumentDefaultValues.push_back(defaultValue);
  }

  void finishInitialization(std::string commandName) {
    this->commandName = commandName;
    numRequiredArguments = 0;
    strTemplate = commandName;
    strTemplate += "(";
    for(int i=0; i<arguments.size(); i++) {
      strTemplate += argumentTypes[i] + " " + arguments[i];
      if(argumentDefaultValues[i].size() > 0) {
        if(argumentTypes[i].compare(STR_STRING) == 0) {
          strTemplate += " = \"" + argumentDefaultValues[i] + "\"";
        }
        else {
          strTemplate += " = " + argumentDefaultValues[i];
        }
      }
      else {
        numRequiredArguments++;
      }
      if(i < arguments.size()-1) {
        strTemplate += ", ";
      }
    }
    strTemplate += ")";

  }

  int match(Command *cmd) {
    if(cmd->command.size() == 0) return -1;
    if(!cmd->command[0].compare(commandName) == 0) return -1;
    if(cmd->command.size()-1 < numRequiredArguments) {
      //printf("%s, %d, %d\n", cmd->str.c_str(), (int)cmd->command.size()-1, numRequiredArguments);
      return 0;
    }
    return 1;
  }

  int getNumArguments() {
    return arguments.size();
  }

  void fillValueInd(Command *cmd, void *a, int argumentIndex = 0) {
    int numTmplArgs = arguments.size();
    int numCmdArgs = cmd->command.size()-1;

    if(numTmplArgs-1 >= argumentIndex && a) {
      std::string strValue;
      if(numCmdArgs-1 >= argumentIndex && cmd->command[argumentIndex+1].size() > 0) {
        strValue = cmd->command[argumentIndex+1];
      }
      else {
        strValue = argumentDefaultValues[argumentIndex];
      }
      fillValue(a, strValue, argumentTypes[argumentIndex]);
    }
  }

  void fillValues(Command *cmd, void *a, void *b = NULL, void *c = NULL, void *d = NULL, void *e = NULL, void *f = NULL, void *g = NULL, void *h = NULL, void *i = NULL, void *j = NULL, void *k = NULL, void *l = NULL, void *m = NULL, void *n = NULL, void *o = NULL, void *p = NULL, void *q = NULL, void *r = NULL, void *s = NULL, void *t = NULL, void *u = NULL, void *v = NULL, void *w = NULL, void *x = NULL, void *y = NULL, void *z = NULL) {
    int nt = arguments.size();
    int nc = cmd->command.size()-1;

    if(nt > 0 && a) fillValue(a, nc > 0 && cmd->command[1].size() > 0 ? cmd->command[1] : argumentDefaultValues[0], argumentTypes[0]);
    if(nt > 1 && b) fillValue(b, nc > 1 && cmd->command[2].size() > 0 ? cmd->command[2] : argumentDefaultValues[1], argumentTypes[1]);
    if(nt > 2 && c) fillValue(c, nc > 2 && cmd->command[3].size() > 0 ? cmd->command[3] : argumentDefaultValues[2], argumentTypes[2]);
    if(nt > 3 && d) fillValue(d, nc > 3 && cmd->command[4].size() > 0 ? cmd->command[4] : argumentDefaultValues[3], argumentTypes[3]);
    if(nt > 4 && e) fillValue(e, nc > 4 && cmd->command[5].size() > 0 ? cmd->command[5] : argumentDefaultValues[4], argumentTypes[4]);
    if(nt > 5 && f) fillValue(f, nc > 5 && cmd->command[6].size() > 0 ? cmd->command[6] : argumentDefaultValues[5], argumentTypes[5]);
    if(nt > 6 && g) fillValue(g, nc > 6 && cmd->command[7].size() > 0 ? cmd->command[7] : argumentDefaultValues[6], argumentTypes[6]);
    if(nt > 7 && h) fillValue(h, nc > 7 && cmd->command[8].size() > 0 ? cmd->command[8] : argumentDefaultValues[7], argumentTypes[7]);
    if(nt > 8 && i) fillValue(i, nc > 8 && cmd->command[9].size() > 0 ? cmd->command[9] : argumentDefaultValues[8], argumentTypes[8]);
    if(nt > 9 && j) fillValue(j, nc > 9 && cmd->command[10].size() > 0 ? cmd->command[10] : argumentDefaultValues[9], argumentTypes[9]);
    if(nt > 10 && k) fillValue(k, nc > 10 && cmd->command[11].size() > 0 ? cmd->command[11] : argumentDefaultValues[10], argumentTypes[10]);
    if(nt > 11 && l) fillValue(l, nc > 11 && cmd->command[12].size() > 0 ? cmd->command[12] : argumentDefaultValues[11], argumentTypes[11]);
    if(nt > 12 && m) fillValue(m, nc > 12 && cmd->command[13].size() > 0 ? cmd->command[13] : argumentDefaultValues[12], argumentTypes[12]);
    if(nt > 13 && n) fillValue(n, nc > 13 && cmd->command[14].size() > 0 ? cmd->command[14] : argumentDefaultValues[13], argumentTypes[13]);
    if(nt > 14 && o) fillValue(o, nc > 14 && cmd->command[15].size() > 0 ? cmd->command[15] : argumentDefaultValues[14], argumentTypes[14]);
    if(nt > 15 && p) fillValue(p, nc > 15 && cmd->command[16].size() > 0 ? cmd->command[16] : argumentDefaultValues[15], argumentTypes[15]);
    if(nt > 16 && q) fillValue(q, nc > 16 && cmd->command[17].size() > 0 ? cmd->command[17] : argumentDefaultValues[16], argumentTypes[16]);
    if(nt > 17 && r) fillValue(r, nc > 17 && cmd->command[18].size() > 0 ? cmd->command[18] : argumentDefaultValues[17], argumentTypes[17]);
    if(nt > 18 && s) fillValue(s, nc > 18 && cmd->command[19].size() > 0 ? cmd->command[19] : argumentDefaultValues[18], argumentTypes[18]);
    if(nt > 19 && t) fillValue(t, nc > 19 && cmd->command[20].size() > 0 ? cmd->command[20] : argumentDefaultValues[19], argumentTypes[19]);
    if(nt > 20 && u) fillValue(u, nc > 20 && cmd->command[21].size() > 0 ? cmd->command[21] : argumentDefaultValues[20], argumentTypes[20]);
    if(nt > 21 && v) fillValue(v, nc > 21 && cmd->command[22].size() > 0 ? cmd->command[22] : argumentDefaultValues[21], argumentTypes[21]);
    if(nt > 22 && w) fillValue(w, nc > 22 && cmd->command[23].size() > 0 ? cmd->command[23] : argumentDefaultValues[22], argumentTypes[22]);
    if(nt > 23 && x) fillValue(x, nc > 23 && cmd->command[24].size() > 0 ? cmd->command[24] : argumentDefaultValues[23], argumentTypes[23]);
    if(nt > 24 && y) fillValue(y, nc > 24 && cmd->command[25].size() > 0 ? cmd->command[25] : argumentDefaultValues[24], argumentTypes[24]);
    if(nt > 25 && z) fillValue(z, nc > 25 && cmd->command[26].size() > 0 ? cmd->command[26] : argumentDefaultValues[25], argumentTypes[25]);
  }
};


struct CommandQueue
{
  std::vector<CommandTemplate*> commandTemplates;
  std::vector<Command*> commands;
  std::vector<std::string> inputHistory;

  ~CommandQueue() {
    for(int i=0; i<commands.size(); i++) {
      delete commands[i];
    }
    // the user side has the deleting responsibility of command templates
  }

  bool addCommandTemplate(CommandTemplate *commandTemplate) {
    for(int i=0; i<commandTemplates.size(); i++) {
      if(commandTemplates[i]->commandName.compare(commandTemplate->commandName) == 0) {
        printf("Error at CommandQueue.addCommandTemplate: Command \"%s\" already registered\n", commandTemplate->commandName.c_str());
        return false;
      }
    }
    commandTemplates.push_back(commandTemplate);
    return true;
  }

  Command *popCommand(const CommandTemplate *commandTemplate) {
    for(int i=0; i<commands.size(); i++) {
      if(commandTemplate->commandName.compare(commands[i]->command[0]) == 0) {
        Command *cmd = commands[i];
        commands.erase(commands.begin()+i);
        return cmd;
      }
    }

    return NULL;
  }

  void addCommand(const std::wstring &str) {
    addCommand(to_string(str));
  }

  void addCommand(const std::string &str) {
    if(str.size() == 0) return;

    inputHistory.push_back(str);

    Command *cmd = new Command(str);

    if(cmd->command.size() > 0) {
      int match = -1;
      std::string strTemplate;

      for(int i=0; i<commandTemplates.size(); i++) {
        int ret = commandTemplates[i]->match(cmd);
        if(ret == 1) {
          match = 1;
          break;
        }
        if(ret == 0) {
          match = 0;
          strTemplate = commandTemplates[i]->strTemplate;
          break;
        }
      }
      if(match == -1) {
        messageQueue.addMessage("Command \"" + cmd->command[0] + "\" doesn't exist.");
      }
      if(match == 0) {
        messageQueue.addMessage("Usage: " + strTemplate);
      }
      if(match == 1) {
        messageQueue.addMessage(cmd->str);
        commands.push_back(cmd);
        return;
      }
    }
    delete cmd;
  }

};

static CommandQueue commandQueue; // FIXME
