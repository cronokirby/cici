#include "stdlib.h"
#include "stdio.h"

// Exit the program with a given error message
void panic(char const* msg) {
    puts(msg);
    exit(-1);
}

int main() {
    puts("a");
    puts("Hello World");
}
