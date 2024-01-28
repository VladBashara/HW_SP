#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include "itoa.c"

void print_error_msg(char* add_msg) {
    printf("%d : %s (%s)\n", errno, strerror(errno), add_msg);
}

void check_if_argc_equal_to(int argc, int n) {
    if (argc != n) {
        printf("ERROR: argc != %d\n", n);
        exit(EXIT_FAILURE);
    }
}

FILE* check_if_file_exists(char* argv_1) {
    FILE* fp = fopen(argv_1, "rb");
    if (fp == NULL) {
        print_error_msg("file doesn`t exist");
        exit(EXIT_FAILURE);
    }
    return fp;
}

long get_file_size(FILE* fp) {
    fseek(fp, 0, SEEK_CUR);
    long offset = ftell(fp);
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, offset, SEEK_SET);
    return size;
}

long check_file_size(FILE* fp) {
    long file_size = get_file_size(fp);
    if (file_size < 2) {
        printf("ERROR: file_size < 2\n");
        exit(EXIT_FAILURE);
    }
    return file_size;
}

void calc(int M, int* N, int* M_prcs, int* M_last_prcs) {
    if (*N > M/2) {
        *N = M/2;
        printf("Warning: N = M//2\n");
    }
    *M_prcs = M / *N;
    *M_last_prcs = M - (*N-1)*(M / *N);
}

void write_slices_to_files(char* buf, int N, int M_prcs, int M_last_prcs) {
    FILE* fp;
    for (int i = 0; i < N-1; i++) { // writing slices of array to new files for N-1 proccesses
        char folder[] = "splitted_arrays/";
        fp = fopen(strcat(folder, i_to_a(i+1)), "wb");
        fwrite(&buf[i*M_prcs], 1, M_prcs, fp);
        fclose(fp);
    }
    char folder[] = "splitted_arrays/";
    fp = fopen(strcat(folder, i_to_a(N)) , "wb"); // writing slices of array to new files for N (last) proccess
    fwrite(&buf[(N-1)*M_prcs], 1, M_last_prcs, fp);
    fclose(fp);
}

void waiting_all_proccesses(int N, char* delay_str) {
    int ret_code;
    printf("parent sleep(%d)\n", 4*atoi(delay_str));
    sleep(4*atoi(delay_str));
    for (int j = 0; j < N; j++) { // waiting until all proccesses will ended
        pid_t ret_wait = wait(&ret_code);
    }
}

void calling_proccesses(int N, char* delay_str) {
    for (int i = 0; i < N; i++) {
        pid_t ret_fork = fork();
        if (ret_fork == 0) {
            char* argv[3] = {i_to_a(i+1), delay_str, NULL};
            execve("./child", argv, NULL);
        }
    }
}

int main(int argc, char* argv[]) {

    check_if_argc_equal_to(argc, 4);
    FILE* fp = check_if_file_exists(argv[1]);
    char* delay_str = argv[3];
    long M = check_file_size(fp);
    int N = atoi(argv[2]);

    char* buf = malloc(M * sizeof(char));
    fread(buf, 1, M, fp); // read file_size bytes from file to buffer
    fclose(fp);
    
    int M_prcs;
    int M_last_prcs;
    calc(M, &N, &M_prcs, &M_last_prcs);
    printf("M - data: %ld\n", M);
    printf("N - subproccesses: %d\n\n", N);
    
    write_slices_to_files(buf, N, M_prcs, M_last_prcs);
    free(buf);
    calling_proccesses(N, delay_str);
    waiting_all_proccesses(N, delay_str);

    int result = 0;
    for (int i = 0; i < N; i++)  { // summation
        char folder[] = "summed_arrays/";
        FILE* fp = fopen(strcat(folder, i_to_a(i+1)), "rb");
        long file_size = get_file_size(fp);
        buf = malloc(file_size * sizeof(char));
        fread(buf, 1, file_size, fp);
        result += atoi(buf);
        free(buf);
        fclose(fp);
    }
    printf("\nResult = %d\n", result);

    exit(EXIT_SUCCESS);
}