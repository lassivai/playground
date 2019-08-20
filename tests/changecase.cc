#include <cctype>
#include <string>
#include <cstdio>
#include <vector>

void makeTitleCase(std::string &str) {
    for(int i=0; i<str.size(); i++) {
        if(i == 0 || !isalnum(str[i-1])) {
            str[i] = toupper(str[i]);
        }
        else {
            str[i] = tolower(str[i]);
        }
    }
}

int main() {
    std::vector<std::string> strs = {"1. Midi Through:Midi Through Port-0 14:0",
                        "1. Launchpad Mini:Launchpad Mini MIDI 1 28:0",
"1. KOMPLETE KONTROL M32:KOMPLETE KONTROL M32 MIDI 1 32:0",
"1. Code 61:Code 61 MIDI 1 36:0",
"1. Code 61:Code 61 MIDI 2 36:1",
"1. Code 61:Code 61 MIDI 3 36:2",
"1. Code 61:Code 61 MIDI 4 36:3"};

    for(auto &s : strs) {
        makeTitleCase(s);
        printf("%s\n", s.c_str());
    }

    return 0;
}