#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <wait.h>
#include "itoa.c" // https://opensource.apple.com/source/groff/groff-40/groff/src/libs/libgroff/itoa.c.auto.html

int main(int argc, char* argv[]) {
    if (argc != 2) { printf("\
    Use next command:\n\
        For Linux or MacOS (UNIX): gcc child.c -o child && gcc parent.c -o parent && ./parent <delay_sec>\n\
        For Windows: gcc child.c -o child.exe && gcc parent.c -o parent.exe && parent.exe <delay_sec>\n\
    "); }
    int delay = atoi(argv[1]);
    
    pid_t ret_fork = fork();
    
    switch (ret_fork) {
    case 0:
        printf("ret_fork for child == %d\n", ret_fork);
        printf("PID of child process == %d\n", getpid());
        printf("wait %ds\n", delay);
        sleep(delay);
        char* argv[] = {"child", i_to_a(delay), NULL};
        if (execve("./child", argv, NULL) == -1) { printf("ERROR: DONT GO HERE\n"); }
        printf("wait %ds\n", delay);
        sleep(delay);
        exit(EXIT_SUCCESS);

    case -1:
        printf("ret_fork == -1\n");
        printf("Error: can`t fork");
        break;
    
    default:
        printf("ret_fork for parent == %d\n", ret_fork);
        printf("PID of parent process == %d\n", getpid());
        int ret_code = 0;
        pid_t ret_pid = wait(&ret_code);
        printf("ret_code == %d\n", ret_code);
        printf("ret_pid == %d\n", ret_pid);
        break;
    }
    printf("wait %ds\n", delay);
    sleep(delay);
    exit(EXIT_SUCCESS);
}