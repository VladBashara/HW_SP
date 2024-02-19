#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include <ctype.h>
#include <pthread.h>
#include "itoa.c"
#include "module.c"

#define ARGC 4
double sum = 0;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void* th_main(void* substrs_void) {
    char* substrs = (char*)substrs_void;
    int len = 0;
    long* size_list = NULL;
    char** pos_list = NULL;
    printf("asdasdasdasdad\n");
    split_str(substrs, strlen(substrs), &pos_list, &size_list, &len);
    printf("len: %d\n", len);
    for (int i = 0; i < len; i++) {
        pthread_mutex_lock(&mutex);
        char* val = malloc(size_list[i]);
        strncpy(val, pos_list[i], size_list[i]);
        printf("val: %s\n", val);
        sum += atof(val);
        free(val);
        pthread_mutex_unlock(&mutex);
    }
    int* r = malloc(sizeof(int));
    *r = 12;
    pthread_exit(r);
}

int main(int argc, char* argv[]) {

    if (argc != ARGC) {
        printf("ERROR: argc != %d\n", ARGC);
        return 1;
    }

    FILE* fp = fopen(argv[1], "rb");
    if (fp == NULL) {
        print_error_msg("file doesn`t exist");
        return 1;
    }

    char* delay_str = argv[3];
    long file_size = get_file_size(fp);
    int N = atoi(argv[2]);

    char* buf = malloc(file_size * sizeof(char));
    fread(buf, 1, file_size, fp);
    if (ferror(fp)) {
        free(buf);
        print_error_msg("Can`t read the file");
        return 1;
    }
    if (fclose(fp)) {
        free(buf);
        print_error_msg("Can`t close the file");
        return 1;
    }
    long M = count_str(buf, '.');
    if (M < 2) { free(buf); printf("ERROR: M < 2\n"); return 1; }
    
    
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

    int len = 0;
    long* size_list = NULL;
    char** pos_list = NULL;
    split_str(buf, file_size, &pos_list, &size_list, &len);
    printf("buf: %s\n", buf);
    printf("len: %d\n", len);
    for (int i =0;i<len;i++) {
        printf("pop_list[%d] %.*s\n",i,(int)size_list[i],pos_list[i]);
    }
    char** substrs = malloc(N * sizeof(char*));
    int counter = 0;
    int max_size_list = 0;
    for (int i =0 ; i<len; i++) {
        printf("OPO %ld\n", size_list[i]);
        if (max_size_list < size_list[i]) {max_size_list = size_list[i];}
    }
    printf("max_size_list %d\n", max_size_list);
    for (int i = 0; i<N-1; i++) {
        substrs[i] = malloc(file_size * sizeof(char));
        strncpy(substrs[i], pos_list[counter], size_list[counter]);
        counter++;
        strcat(substrs[i], " ");
        for (int j = 1; j<M_prcs; j++) {
            strncat(substrs[i], pos_list[counter], size_list[counter]);
            counter++;
            if (j != size_list[i]-1) { strcat(substrs[i], " "); }
        }
    }

    for (int i = N-1; i<N; i++) {
        substrs[i] = malloc(file_size * sizeof(char));
        strncpy(substrs[i], pos_list[counter], size_list[counter]);
        counter++;
        strcat(substrs[i], " ");
        for (int j = 1; j<M_last_prcs; j++) {
            strncat(substrs[i], pos_list[counter], size_list[counter]);
            counter++;
            if (j != size_list[i]-1) { strcat(substrs[i], " "); }
        }
    }
    for (int i = 0; i<N; i++) {
        printf("substrs[%d]: %s\n", i, substrs[i]);
    }
    //OKEY
    // pthread_t* id = malloc(1 * sizeof(pthread_t));
    pthread_t id[N];
    // pthread_create(&(id[0]), NULL, th_main, (void *)(substrs[0]));
    // pthread_create(&(id[0]),NULL, f, NULL);
    for (int i = 0; i<N; i++) {
        pthread_create(&id[i], NULL, th_main, (void *)(substrs[i])); // ERROR
    }
    
    for (int i = 0; i<N; i++) {
        void* th_ret;
        pthread_join(id[i], &th_ret);
        printf("join %ld %d\n", id[i], *(int*)th_ret);
        free(th_ret);
    }

    // for (int i = 0; i<N; i++) {
    //     free(substrs[i]);
    // }
    // free(substrs);
    // free(buf);
    // free(id);
    printf("sum is %f\n", sum);
    // int counter = 0;
    // char** f = malloc(N * sizeof(char*));
    // for (int i = 0; i<N; i++) {
    //     f[i] = malloc(len * sizeof(char));
    // }
    // char white_space[] = " ";
    // for (int j = 0; j < N-1; j++) {
    //     // char file[] = "splitted_arrays/";
    //     // FILE* fp = fopen(strcat(file, i_to_a(j+1)), "wb"); if (fp == NULL) { return 1; }
    //     for (int i = 0; i < M_prcs; i++) {
    //         // fwrite(pos_list[counter], 1, size_list[counter], fp);
    //         strncat(f[j], pos_list[counter], size_list[counter]);
    //         counter++;
    //         // if (i != M_prcs-1) { fwrite(" ", 1, 1, fp); }
    //         if (i != M_prcs-1) { strncat(f[j], white_space, 1); }
    //     }
    //     // if (fclose(fp)) { return 1; }
    // }
    // // char file[] = "splitted_arrays/";
    // // FILE* fp = fopen(strcat(file, i_to_a(N)), "wb");
    // for (int i = 0; i < M_last_prcs; i++) {
    //     // fwrite(pos_list[counter], 1, size_list[counter], fp);
    //     strncat(f[i], pos_list[counter], size_list[counter]);
    //     counter++;
    //     // if (i != M_last_prcs-1) { fwrite(" ", 1, 1, fp);
    //     if (i != M_prcs-1) { strncat(f[i], white_space, 1); }
    // }
    // free(size_list);
    // // if (fclose(fp)) { return 1; }

    // for (int j = 0; j<N; j++) {
    //     printf("%d %s\n", j, f[j]);
    // }


    // for (int i = 0; i<N; i++) {
    //     free(f[i]);
    // }
    // free(f);
    return 0;
}