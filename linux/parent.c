#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include "itoa.c"
#include <ctype.h>
#include "module.c"

#define ARGC 4

int main(int argc, char* argv[]) {

    if (argc != ARGC) {
        printf("ERROR: argc != %d\n", ARGC);
        exit(EXIT_FAILURE);
    }

    FILE* fp = fopen(argv[1], "rb");
    if (fp == NULL) {
        print_error_msg("file doesn`t exist");
        exit(EXIT_FAILURE);
    }

    char* delay_str = argv[3];
    long file_size = get_file_size(fp);
    int N = atoi(argv[2]);

    char* buf = malloc(file_size * sizeof(char));
    fread(buf, 1, file_size, fp);
    if (ferror(fp)) {
        free(buf);
        print_error_msg("Can`t read the file");
        exit(EXIT_FAILURE);
    }
    if (fclose(fp)) {
        free(buf);
        print_error_msg("Can`t close the file");
        exit(EXIT_FAILURE);
    }
    long M = count_str(buf, '.');
    if (M < 2) { free(buf); printf("ERROR: M < 2\n"); exit(EXIT_FAILURE); }
    
    
    int M_prcs;
    int M_last_prcs;
    if (N > M/2) {
        N = M/2;
        printf("Warning: N = M//2\n");
    }
    M_prcs = M / N;
    M_last_prcs = M - (N-1)*(M / N);

    printf("M - data size: %ld\n", M);
    printf("N - child processes: %d\n", N);
    
    if (write_slices_to_files(buf, file_size, N, M_prcs, M_last_prcs)) {
        free(buf);
        print_error_msg("write_slices error");
        exit(EXIT_FAILURE);
    }
    free(buf);
    calling_proccesses(N, delay_str);
    waiting_all_proccesses(N, delay_str);
    double result = 0;
    for (int i = 0; i < N; i++)  {
        char folder[] = "summed_arrays/";
        FILE* fp = fopen(strcat(folder, i_to_a(i+1)), "rb");
        if (fp == NULL) {
            print_error_msg("");
            exit(EXIT_FAILURE);
        }
        long file_size = get_file_size(fp);
        buf = malloc(file_size * sizeof(char));
        fread(buf, 1, file_size, fp);
        if (ferror(fp)) {
            free(buf);
            print_error_msg("Can`t read from file");
            exit(EXIT_FAILURE);
        }
        char* result_str = malloc(file_size * sizeof(char));
        strncpy(result_str, buf, file_size);
        char** end = NULL;
        result += atof(result_str);
        free(buf);
        free(result_str);
        if (fclose(fp)) {
            print_error_msg("Can`t close the file");
            exit(EXIT_FAILURE);
        }
    }
    printf("\nResult = %f\n", result);

    exit(EXIT_SUCCESS);
}