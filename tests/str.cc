#include <cwchar>
#include <string>
#include <iostream>
#include <locale>
#include <codecvt>

typedef std::codecvt<wchar_t,char,std::mbstate_t> facet_type;
std::locale mylocale;
const facet_type& myfacet = std::use_facet<facet_type>(mylocale);
std::mbstate_t mystate = std::mbstate_t();

std::wstring to_wstring(std::string str) {
  const char *chars = str.c_str();

  wchar_t pwstr[str.length()];
  const char* pc;
  wchar_t* pwc;

  facet_type::result result = myfacet.in (mystate,
    chars, chars+str.length(), pc,
    pwstr, pwstr+str.length(), pwc);

  return std::wstring(pwstr);
}

std::string to_string(std::wstring wstr) {
  const wchar_t *wchars = wstr.c_str();

  char str[wstr.length()];
  char* pcc;
  const wchar_t* pwcc;
  facet_type::result result = myfacet.out(mystate, wchars, wchars+wstr.length(), pwcc, str, str+wstr.length(), pcc);
  str[wstr.length()] = '\0';
  return std::string(str);
}

int main()
{
  std::string str = "asdfghjkl";

  std::wstring wstr(to_wstring(str));

  std::string str2(to_string(wstr));


  //std::wcout << "wstr: " << wstr << "\n";
  std::cout << "str: " << str2 << "\n";

}
