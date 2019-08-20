#include <string>
#include <cstdio>
#include <cwchar>
#include <iostream>


int main()
{
  std::wcout.sync_with_stdio(false);
  std::wcout.imbue(std::locale("fi_FI.utf8"));

  std::wcout << "initial locale: " << setlocale(LC_ALL, NULL) << "\n";
  setlocale(LC_ALL, "fi_FI.utf8");
  std::wcout << "locale now: " << setlocale(LC_ALL, NULL) << "\n";

  wchar_t wc = L'ä';

  std::wcout << wc << " " << (int)wc << L"\n";


  std::string str = "ä";

  wchar_t wcs[128];
  mbstowcs(wcs, str.c_str(), 1);

  std::wstring wstr(wcs);


  std::wcout << "wstr: " << wstr << "\n";

  for(int i=0; i<wstr.size(); i++) {
    std::wcout << "wstr[" << i << "]: " << int(wstr[i]) << "\n";
  }


  return 0;
}
