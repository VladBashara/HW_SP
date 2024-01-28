#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <wait.h>

int main(int argc, char* argv[]) {
    int delay = atoi(argv[1]);
    printf("Inside child process\n");
    printf("wait %ds\n", delay);
    sleep(delay);
    exit(EXIT_SUCCESS);
}