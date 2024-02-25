#include <cstdlib>
#include <stdio.h>


int main() {
    printf("OS %s\n", std::getenv("OS"));
    return 0;
}