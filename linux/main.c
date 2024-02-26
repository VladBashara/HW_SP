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


int read_file_to_buf(FILE* fp, long file_size, char** buf) {
    *buf = malloc((file_size+1) * sizeof(char));
    if (fread(*buf, 1, file_size, fp) < file_size) { printf("ERROR\n");}
    (*buf)[file_size] = '\0';
    if (ferror(fp)) {
        free(*buf);
        print_error_msg("Can`t read the file");
        return 1;
    }
    if (fclose(fp)) {
        free(*buf);
        print_error_msg("Can`t close the file");
        return 1;
    }
    return 0;
}

struct ARG {
    int delay;
    char* substr;
};

double sum = 0;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
void* th_main(void* arg) {
    sleep(((struct ARG*)arg)->delay);
    char* substrs = (char*)(((struct ARG*)arg)->substr);
    int len = 0;
    int* size_list = NULL;
    char** pos_list = NULL;
    split_str(substrs, strlen(substrs), &pos_list, &size_list, &len);
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < len; i++) {
        char* val = malloc(size_list[i]);
        strncpy(val, pos_list[i], size_list[i]);
        sum += atof(val);
        free(val);
    }
    pthread_mutex_unlock(&mutex);
    int* r = malloc(sizeof(int));
    *r = 12;
    pthread_exit(r);
}

#define MACHINE_WORD_SIZE 64
#if MACHINE_WORD_SIZE == 64
    # define MACHINE_WORD_TYPE long long
#else
    #define MACHINE_WORD_TYPE int
#endif

void get_substr(char*** substr, char** buf, long file_size, int N, int M_prcs, int M_last_prcs) {
    if ((*buf)[file_size-1] != ' ') {
        char* new_buf = (char*)malloc((file_size+ 1 + 1) * sizeof(char));
        strncpy(new_buf, *buf, file_size);
        strncpy(&new_buf[file_size], " ", 1);
        strncpy(&new_buf[file_size+1], "\0", 1);
        free(*buf);
        *buf = (char*)malloc((file_size + 1 + 1) * sizeof(char));
        strncpy(*buf, new_buf, file_size + 1 + 1);
        free(new_buf);
    }
    MACHINE_WORD_TYPE idx = (MACHINE_WORD_TYPE)(*buf)-1;
    MACHINE_WORD_TYPE start = (MACHINE_WORD_TYPE)(*buf);
    MACHINE_WORD_TYPE const start_const = (MACHINE_WORD_TYPE)(*buf);
    int jmp_counter = 0;
    for (int j = 0; j < N; j++) {
        if (j == N-1) { jmp_counter = M_last_prcs; } else { jmp_counter = M_prcs; }
        for (int i = 0; i < jmp_counter; i++) {
            idx = (MACHINE_WORD_TYPE)strchr(&(*buf)[idx+1-start_const], (long)' ');
        }
        strncpy((*substr)[j], &((*buf)[start-start_const]), idx-start);
        start = idx + 1;
    }
}

#define ARGC 4
int main(int argc, char* argv[]) {

    if (argc != ARGC) {
        // printf("ERROR: argc != %d\n", ARGC);
        printf("Usage: %s <filename> <threads_number> <delay>\n", argv[0]);
        printf("   <filename>: path to file with list of floats\n");
        printf("   <threads_number>: number of child threads\n");
        printf("   <delay>: delay in seconds\n");
        return 1;
    }

    FILE* fp = fopen(argv[1], "rb");
    if (fp == NULL) {
        print_error_msg("file doesn`t exist");
        return 1;
    }
    printf("PID: %d\n", getpid());
    char* delay_str = argv[3];

    long file_size = get_file_size(fp);
    char* buf = NULL;
    if (read_file_to_buf(fp, file_size, &buf)) { free(buf); return 1; }

    int N = atoi(argv[2]);
    int M = count_str(buf, '.');
    if (M < 2) { free(buf); printf("ERROR: M < 2\n"); return 1; }
    int M_prcs;
    int M_last_prcs;
    calcDist(&N, &M, &M_prcs, &M_last_prcs);
    
    char** substr = malloc(N * sizeof(char*));
    for (int i = 0; i < N; i++) {
        substr[i] = calloc(file_size, sizeof(char));
    }

    get_substr(&substr, &buf, file_size, N, M_prcs, M_last_prcs);
    
    pthread_t* id = malloc(N * sizeof(pthread_t));
    struct ARG arg[N];
    for (int i = 0; i<N; i++) {
        arg[i].delay = atoi(delay_str);
        arg[i].substr = substr[i];
        pthread_create(&id[i], NULL, th_main, (void *)&(arg[i]));
    }
    sleep(atoi(delay_str));
    for (int i = 0; i<N; i++) {
        void* th_ret;
        pthread_join(id[i], &th_ret);
        free(th_ret);
    }

    for (int i = 0; i<N; i++) {
        free(substr[i]);
    }
    free(substr);
    free(buf);
    free(id);
    printf("\nsum is %f\n", sum);
    return 0;
}