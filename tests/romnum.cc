#include <string>
#include <cstdio>

struct RomanNumeral
{
private:
  static std::string get1000(int number) {
    std::string str;
    if(number == 1) return "M";
    else if(number == 2) return "MM";
    else if(number == 3) return "MMM";
    else if(number > 3 && number < 10) return "(error)";
    return "";
  }

  static std::string get100(int number) {
    std::string str;
    if(number == 1) return "C";
    else if(number == 2) return "CC";
    else if(number == 3) return "CCC";
    else if(number == 4) return "CD";
    else if(number == 5) return "D";
    else if(number == 6) return "DC";
    else if(number == 7) return "DCC";
    else if(number == 8) return "DCCC";
    else if(number == 9) return "CM";
    return "";
  }
  
  static std::string get10(int number) {
    std::string str;
    if(number == 1) return "X";
    else if(number == 2) return "XX";
    else if(number == 3) return "XXX";
    else if(number == 4) return "XL";
    else if(number == 5) return "L";
    else if(number == 6) return "LX";
    else if(number == 7) return "LXX";
    else if(number == 8) return "LXXX";
    else if(number == 9) return "XC";
    return "";
  }
  
  static std::string get1(int number) {
    std::string str;
    if(number == 1) return "I";
    else if(number == 2) return "II";
    else if(number == 3) return "III";
    else if(number == 4) return "IV";
    else if(number == 5) return "V";
    else if(number == 6) return "VI";
    else if(number == 7) return "VII";
    else if(number == 8) return "VIII";
    else if(number == 9) return "IX";
    return "";
  }
  
public:
  static std::string get(int number) {
    if(number < 1 || number > 3000) {
      printf("Error at RomanNumeral::get(int), number %d out of range\n", number);
      return "";
    }
    int thousands = number / 1000;
    int hundreds = (number - thousands*1000) / 100;
    int tens = (number - thousands*1000 - hundreds*100) / 10;
    int ones = (number - thousands*1000 - hundreds*100 - tens*10);
    return get1000(thousands) + get100(hundreds) + get10(tens) + get1(ones);
  }
};


int main() {
    printf("Testing roman numerals...\n");

    for(int i=1; i<3001; i++) {
        printf("%d - %s\n", i, RomanNumeral::get(i).c_str());
    }
    return 0;
}