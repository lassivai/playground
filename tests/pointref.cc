#include <cstdio>

char *A = (char*)"abcdefg";
char *B = (char*)"hijklmn";
char *C = (char*)"opqrstu";

void testingPointerReference(char *&pointer) {
    if(pointer) {
        pointer = B;
    }
    else {
        pointer = C;
    }
}

int main() {
    char *chars = A;
    char *chars2 = NULL;

    printf("1: %s\n", chars);
    printf("2: %s\n", chars2);

    testingPointerReference(chars);
    testingPointerReference(NULL);

    printf("1: %s\n", chars);
    printf("2: %s\n", chars2);

    return 0;
}