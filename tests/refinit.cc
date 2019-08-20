#include <cstdio>

struct ReferenceTest {
    int &reference;
    ReferenceTest(int &reference) : reference(reference) {}
};


int main() {
    int number = 123;

    printf("%d\n", number);

    ReferenceTest rt(number);

    rt.reference = 456;

    printf("%d\n", number);

    return 0;
}