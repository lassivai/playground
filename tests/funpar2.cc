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

void execute(FibonacciCounter &fibonacciCounter, std::vector<std::function<int(FibonacciCounter &fibonacciCounter)>> &functions) {
  int i = 0;
  for(auto &f : functions) {
    int ret = f(fibonacciCounter);
    if(ret == 0) print("...called from normal function");
    if(ret == 1) print("...called from functor");
    if(ret == 2) print("...called from lambda");
  }
}

int normalFunction(FibonacciCounter &fibonacciCounter) {
  printf("fibonacci sequence... %d\n", fibonacciCounter.count());
  return 0;
}

struct FunctorTest {
  int k = 0;
  int operator()(FibonacciCounter &fibonacciCounter) {
    printf("fibonacci sequence... %d, for the %d. time...\n", fibonacciCounter.count(), ++k);
    return 1;
  }
};


int main() {
  print("Testing functions as parameters");

  FibonacciCounter fib;
  FunctorTest functorTest;

  std::vector<std::function<int(FibonacciCounter &fibonacciCounter)>> functions;
  functions.push_back(normalFunction);
  functions.push_back(functorTest);
  functions.push_back([](FibonacciCounter &fibonacciCounter) { printf("fibonacci sequence... %d\n", fibonacciCounter.count()); return 2; });
  
  execute(fib, functions);
  execute(fib, functions);
  execute(fib, functions);
  execute(fib, functions);

  return 0;
}