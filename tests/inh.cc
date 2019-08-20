#include <iostream>


template<class T> struct Base {
  virtual void f() {
    std::cout << "Base.f()\n";
  }
};

template<class T> struct Inherited : public Base {
  void f() {
    std::cout << "Inherited.f()\n";
  }
};



int main()
{
  Base b;
  return 0;
}
