#include <cstdio>
#include <vector>

template<class A> struct CircularBuffer : public std::vector<A> {
  unsigned int currentPosition = 0;

  CircularBuffer(int size) : std::vector<A>(size) {
    currentPosition = 0;
  }
  CircularBuffer(int size, const A &initialValue) : std::vector<A>(size, initialValue) {
    currentPosition = 0;
  }

  void put(const A &value) {
    this->at(currentPosition) = value;
    currentPosition++;
    if(currentPosition >= this->size()) {
      currentPosition = 0;
    }
  }

  void print() {
    for(int i=0; i<this->size(); i++) {
      printf("%d%s", (int)this->at(i), i < this->size()-1 ? ", " : "\n");
    }
  }
};

template<class A>
double getAverage(const std::vector<A> &arr) {
  double total = 0;
  for(int i=0; i<arr.size(); i++) {
    total += arr[i];
  }
  return total / arr.size();
}


int main() {

  CircularBuffer<int> cb(5, -1);

  cb.print();

  cb.put(4);
  cb.put(8);
  cb.put(16);
  cb.put(32);
  cb.print();
  cb.put(63);
  cb.put(127);
  cb.put(255);
  cb.put(511);
  cb.print();
  return 0;
}
