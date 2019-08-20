#include <cstdio>
#include <vector>
#include <ctgmath>
#include <cstring>

struct Primes {
  std::vector<unsigned long> primes;
  unsigned long maxComposite = 100000;
  unsigned long maxPrime = 0;
  unsigned long maxAmountOfPrimes = 0;
  unsigned long numPrimes = 0;

  Primes(bool initAtCreation = true) {
    if(initAtCreation) {
      init();
    }
  }
  
  unsigned long getNthPrime(unsigned long n) {
    if(n >= 0 && n < primes.size()) {
      return primes[n];
    }
    return 0;
  }
  
  void init() {
    unsigned long maxPrimeTmp = 0;
    if(maxComposite < 2 && maxPrime < 2 && maxAmountOfPrimes < 1) {
      maxAmountOfPrimes = 100000;
    }
if(maxAmountOfPrimes > 0) {
    numPrimes = 0;
    primes.resize(maxAmountOfPrimes);
    for(unsigned long i=2; ; i++) {
      unsigned long m = (unsigned long)sqrt(i);
      bool isPrime = true;
      for(unsigned long k=0; k<numPrimes && k <= m; k++) {
        if(i % primes[k] == 0) {
          isPrime = false;
          break;
        }
      }
      if(isPrime) {
        primes[numPrimes] = i;
	maxPrimeTmp = i;
        numPrimes++;
        if(numPrimes >= maxAmountOfPrimes) {
             break;
        }
      }
    }
    }
else {
    for(unsigned long i=2; i<=maxComposite || maxPrimeTmp < maxPrime; i++) {
      unsigned long m = (unsigned long)sqrt(i);
      bool isPrime = true;
      for(unsigned long k=0; k<primes.size() && k <= m; k++) {
        if(i % primes[k] == 0) {
          isPrime = false;
          break;
        }
      }
      if(isPrime) {
        primes.push_back(i);
	maxPrimeTmp = i;
      }
    }
}
    maxPrime = primes[primes.size()-1];
    numPrimes = primes.size();

    maxComposite = maxComposite > maxPrime ? maxComposite : maxPrime;
  }
  
  unsigned long getClosestPrime(unsigned long number) {
    if(number <= primes[0]) return primes[0];
    if(number >= maxPrime) return maxPrime;
    
    unsigned long closestPrime = primes[0];
    
    for(unsigned long i=0; i<primes.size(); i++) {
      if(number < primes[i]) {
        if(number == primes[i]) {
          return number;
        }
        if(number - primes[i-1] < primes[i] - number) {
          return primes[i-1];
        }
        return primes[i];
      }
    }
    
    return 0;
  }

    unsigned long getClosestPrime(unsigned long number, unsigned long &i) {
    if(number <= primes[0]) return primes[0];
    if(number >= maxPrime) return maxPrime;
    
    unsigned long closestPrime = primes[0];
    
    for(i=0; i<primes.size(); i++) {
      if(number - primes[i] <= 0) {
        if(number == primes[i]) {
          return number;
        }
        if(number - primes[i-1] < primes[i] - number) {
          return primes[i-1];
        }
        return primes[i];
      }
    }
    
    return 0;
  }
  
  
  std::vector<unsigned long> getPrimeFactorization(unsigned long number) {
    std::vector<unsigned long> primeFactors;
    if(number > maxPrime*maxPrime) {
      printf("Error at Primes::getPrimeFactorization(), too big number %lu, primes initialized up to %lu\n", number, maxComposite);
      return primeFactors;
    }
    
    for(unsigned long i=0; i<primes.size(); i++) {
      if(number == 1) {
        break;
      }
      if(number % primes[i] == 0) {
        primeFactors.push_back(primes[i]);
        number /= primes[i];
        i--;
      }
    }
    if(primeFactors.size() == 0) {
       primeFactors.push_back(number);
    }

    return primeFactors;
  }


};

void printUsage(const char *command) {
  printf("usage: %s [-n <number>] [-c <numPrimesPrinted>] [-maxPrime <maxPrime> -maxComposite<maxComposite>] \n", command);
}

int main(int argc, char **argv) {

  Primes p(false);
  p.maxComposite = 0;
  p.maxPrime = 0;
  p.maxAmountOfPrimes = 0;

  int numPrimesPrinted = 0;
  unsigned long composite = 0;

  if(argc == 1) {
    printUsage(argv[0]);
    return 0;
  }
  for(int i=1; i<argc; i++) {
    if(strcmp(argv[i], "-c") == 0 && argc > i+1) {
      numPrimesPrinted = atoi(argv[i+1]);
    }
    if(strcmp(argv[i], "-n") == 0 && argc > i+1) {
      composite = atoi(argv[i+1]);
    }
    if(strcmp(argv[i], "-maxPrime") == 0 && argc > i+1) {
      p.maxPrime = atoi(argv[i+1]);
    }
    if(strcmp(argv[i], "-maxComposite") == 0 && argc > i+1) {
      p.maxComposite = atoi(argv[i+1]);
    }
    if(strcmp(argv[i], "-numPrimes") == 0 && argc > i+1) {
      p.maxAmountOfPrimes = atoi(argv[i+1]);
    }
  }

  p.init();

  if(numPrimesPrinted > 0) {
  numPrimesPrinted = numPrimesPrinted < p.primes.size() ? numPrimesPrinted : p.primes.size();
  printf("%d first primes are ", numPrimesPrinted);
  for(int i=0; i<numPrimesPrinted; i++) {
    printf("%lu%s", p.primes[i], i<numPrimesPrinted-1 ? ", " : "\n\n");
  }
  }

if(composite > 0) {
  printf("prime factorization of %lu is ", composite);

  std::vector<unsigned long> pf = p.getPrimeFactorization(composite);
  for(int i=0; i<pf.size(); i++) {
    printf("%lu%s", pf[i], i < pf.size()-1 ? ", " : "\n");
  }

  printf("closest prime to %lu is %lu\n", composite, p.getClosestPrime(composite));
}

  return 0;
}
