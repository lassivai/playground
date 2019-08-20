#include <cstdio>
#include <string>

int main() {
  std::string str =  "!\"#$%%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~ÄÅÖäåöΑΒΓΔΕΖΗΘΙΚΛΜΝΞΟΠΡ΢ΣΤΥΦΧΨΩαβγδεζηθικλμνξοπρςστυφχψω←↑→↓↔↕⇌∀∁∂∃∄∅∆∇∈∉∊∏∐∑∘∙∞∡∧∨∩∪∫≈≠≤≥⊂⊃⊄⊆⊈⋀⋁⋂⋃\n" ;

  std::wstring wstr(L"!\"#$%%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~ÄÅÖäåöΑΒΓΔΕΖΗΘΙΚΛΜΝΞΟΠΡ΢ΣΤΥΦΧΨΩαβγδεζηθικλμνξοπρςστυφχψω←↑→↓↔↕⇌∀∁∂∃∄∅∆∇∈∉∊∏∐∑∘∙∞∡∧∨∩∪∫≈≠≤≥⊂⊃⊄⊆⊈⋀⋁⋂⋃\n");

  printf("%lu\n", wstr.length());
  int n = 0;
  for(int i=0; i<wstr.length(); i++) {
    //if(str[i] >= 0)
    printf(" %d", wstr[i]);
    /*else {
      printf(" %d %d", str[i], str[i+1]);
      i++;
    }*/
    n++;
  }
  printf("\n");
  printf("%d\n", n);
}
