#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
// #include "main.c"

int main(int argc, char* argv[]) {
    // for (int i = 0; i < 20; i++) {
    //     main1(argc, argv);
    //     printf("\n\n\n");
    // }
    int N = 5;
    pthread_t* id = malloc(N * sizeof(pthread_t));
    for (int i = 0; i < N; i++) {
        id[i] = i;
    }
    for (int i = 0; i < N; i++) {
        printf("id[%d] %ld\n", i, id[i]);
    }
    free(id);
    return 0;
}