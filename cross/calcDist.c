#include <stdio.h>

void calcDist(int* N, int* M, int* M_prcs, int* M_last_prcs) {
    if ((*N) > (*M)/2) {
        (*N) = (*M)/2;
        printf("Warning: N = M//2\n");
    }
    *M_prcs = (*M) / (*N);
    *M_last_prcs = (*M) - ((*N)-1)*((*M) / (*N));

    printf("M - data size: %d\n", *M);
    printf("N - child processes: %d\n", *N);
    printf("M_prcs: %d\n", *M_prcs);
    printf("M_last_prcs: %d\n", *M_last_prcs);
}