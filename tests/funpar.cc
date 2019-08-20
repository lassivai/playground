#include <cstdio>
#include <string>
#include <vector>
#include <functional>

void print(std::string str) {
   printf("%s\n", str.c_str());
}

struct FibonacciCounter {
  int a = 1, b = 1;
  int count() {
    int c = a + b;
    a = b;
    b = c;
    return a;
  }
};

void execute(FibonacciCounter &fibonacciCounter, std::vector<std::function<void(FibonacciCounter &fibonacciCounter)>> &functions) {
  int i = 0;
  for(auto &f : functions) {
    f(fibonacciCounter);
  }
}

void normalFunction(FibonacciCounter &fibonacciCounter) {
  printf("This is normal function counting fibonacci sequence... %d\n", fibonacciCounter.count());
}

struct FunctorTest {
  int k = 0;
  void operator()(FibonacciCounter &fibonacciCounter) {
    printf("This is functor         counting fibonacci sequence... %d, for the %d. time...\n", fibonacciCounter.count(), ++k);
  }
};


int main() {
  print("Testing functions as parameters");

  FibonacciCounter fib;
  FunctorTest functorTest;

  std::vector<std::function<void(FibonacciCounter &fibonacciCounter)>> functions;
  functions.push_back(normalFunction);
  functions.push_back(functorTest);
  functions.push_back([](FibonacciCounter &fibonacciCounter) { printf("This is lambda          counting fibonacci sequence... %d\n", fibonacciCounter.count()); });
  
  execute(fib, functions);
  execute(fib, functions);
  execute(fib, functions);
  execute(fib, functions);

  return 0;
}