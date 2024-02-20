#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include <ctype.h>
#include <pthread.h>
#include "calcDist.c"
#include "itoa.c"
#include "module.c"

#define ARGC 4
double sum = 0;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// void get_substr(char*** substrs, char** pos_list, int* size_list, long file_size, int M_prcs, int M_last_prcs, int N) {
//     int counter = 0;
//     for (int i = 0; i<N-1; i++) {
//         (*substrs)[i] = malloc(file_size * sizeof(char));
//         strncpy((*substrs)[i], pos_list[counter], size_list[counter]);
//         printf("pos_list: %s\n", pos_list[counter]);
//         printf("size_list: %d\n", size_list[counter]);
//         printf("substr: %s\n", (*substrs)[i]);
//         counter++;
//         strcat((*substrs)[i], " ");
//         printf("substr: %s\n", (*substrs)[i]);
//         for (int j = 1; j<M_prcs; j++) {
//             strncat((*substrs)[i], pos_list[counter], size_list[counter]);
//             printf("pos_list: %s\n", pos_list[counter]);
//             printf("size_list: %d\n", size_list[counter]);
//             printf("substr: %s\n", (*substrs)[i]);
//             counter++;
//             if (j != size_list[i]-1) { strcat((*substrs)[i], " "); printf("substr: %s\n", (*substrs)[i]);}
//         }
//     }

//     for (int i = N-1; i<N; i++) {
//         (*substrs)[i] = malloc(file_size * sizeof(char));
//         printf("file_size: %ld\n", file_size);
//         printf("SUB: %s\n", (*substrs)[i]);
//         // printf("HERE %s %s %d\n", (*substrs)[i], pos_list[counter], size_list[counter]);
//         strncpy((*substrs)[i], pos_list[counter], size_list[counter]);
//         printf("pos_list: %s\n", pos_list[counter]);
//         printf("size_list: %d\n", size_list[counter]);
//         printf("substr: %s\n", (*substrs)[i]);
//         counter++;
//         strcat((*substrs)[i], " ");
//         printf("substr: %s\n", (*substrs)[i]);
//         for (int j = 1; j<M_last_prcs; j++) {
//             strncat((*substrs)[i], pos_list[counter], size_list[counter]);
//             printf("pos_list: %s\n", pos_list[counter]);
//             printf("size_list: %d\n", size_list[counter]);
//             printf("substr: %s\n", (*substrs)[i]);
//             counter++;
//             if (j != size_list[i]-1) { strcat((*substrs)[i], " "); printf("substr^ %s\n", (*substrs)[i]);}
//         }
//     }
//     for (int i = 0; i<N; i++) {
//         printf("substrs[%d]: %s\n", i, (*substrs)[i]);
//     }
// }

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

void* th_main(void* arg) {
    sleep(((struct ARG*)arg)->delay);
    char* substrs = (char*)(((struct ARG*)arg)->substr);
    int len = 0;
    int* size_list = NULL;
    char** pos_list = NULL;
    split_str(substrs, strlen(substrs), &pos_list, &size_list, &len);
    // printf("len: %d\n", len);
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

#define OS_SIZE_TYPE 64
#if OC_OS_SIZE_TYPE == 64
#define OS_SIZE long long
#else
#define OS_SIZE int
#endif

void get_substr(char*** substr, char** buf, long file_size, int N, int M_prcs, int M_last_prcs) {
    if ((*buf)[file_size-1] != ' ') {
        char* new_buf = malloc((file_size + 1 + 1) * sizeof(char));
        strncpy(new_buf, *buf, file_size);
        strcat(new_buf, " \0");
        free(*buf);
        *buf = malloc((file_size + 1 + 1) * sizeof(char));
        strncpy(*buf, new_buf, file_size + 1 + 1);
    }
    OS_SIZE idx = (long)(*buf)-1;
    OS_SIZE start = (long)(*buf);
    OS_SIZE const start_const = (long)(*buf);
    int jmp_counter = 0;
    for (int j = 0; j < N; j++) {
        if (j == N-1) { jmp_counter = M_last_prcs; } else { jmp_counter = M_prcs; }
        for (int i = 0; i < jmp_counter; i++) {
            idx = (long)strchr(&(*buf)[idx+1-start_const], (long)' ');
        }
        strncpy((*substr)[j], &((*buf)[start-start_const]), idx-start);
        start = idx + 1;
    }
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

    // free(buf);
    // for (int i = 0; i < N; i++) {
    //     free(substr[i]);
    // }
    // free(substr);
    // return 0;

    // int len = 0;
    // int* size_list = NULL;
    // char** pos_list = NULL;
    // split_str(buf, (size_t)file_size, &pos_list, &size_list, &len);
    
    // int max_size_list = 0;
    // for (int i =0 ; i<len; i++) {
    //     // printf("OPO %d\n", size_list[i]);
    //     if (max_size_list < size_list[i]) {max_size_list = size_list[i];}
    // }
    // printf("max_size_list %d\n", max_size_list);
    // char** substrs = malloc(N * sizeof(char*));
    // get_substr(&substrs, pos_list, size_list, file_size, M_prcs, M_last_prcs, N);
    // // OKEY
    // printf("prelast %d\n", N);
    pthread_t* id = malloc(N * sizeof(pthread_t));
    // // pthread_t id[N];
    // // pthread_create(&(id[0]), NULL, th_main, (void *)(substrs[0]));
    // // pthread_create(&(id[0]),NULL, f, NULL);
    struct ARG arg[N];
    for (int i = 0; i<N; i++) {
        arg[i].delay = atoi(delay_str);
        arg[i].substr = substr[i];
        pthread_create(&id[i], NULL, th_main, (void *)&(arg[i])); // ERROR
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
    // // int counter = 0;
    // // char** f = malloc(N * sizeof(char*));
    // // for (int i = 0; i<N; i++) {
    // //     f[i] = malloc(len * sizeof(char));
    // // }
    // // char white_space[] = " ";
    // // for (int j = 0; j < N-1; j++) {
    // //     // char file[] = "splitted_arrays/";
    // //     // FILE* fp = fopen(strcat(file, i_to_a(j+1)), "wb"); if (fp == NULL) { return 1; }
    // //     for (int i = 0; i < M_prcs; i++) {
    // //         // fwrite(pos_list[counter], 1, size_list[counter], fp);
    // //         strncat(f[j], pos_list[counter], size_list[counter]);
    // //         counter++;
    // //         // if (i != M_prcs-1) { fwrite(" ", 1, 1, fp); }
    // //         if (i != M_prcs-1) { strncat(f[j], white_space, 1); }
    // //     }
    // //     // if (fclose(fp)) { return 1; }
    // // }
    // // // char file[] = "splitted_arrays/";
    // // // FILE* fp = fopen(strcat(file, i_to_a(N)), "wb");
    // // for (int i = 0; i < M_last_prcs; i++) {
    // //     // fwrite(pos_list[counter], 1, size_list[counter], fp);
    // //     strncat(f[i], pos_list[counter], size_list[counter]);
    // //     counter++;
    // //     // if (i != M_last_prcs-1) { fwrite(" ", 1, 1, fp);
    // //     if (i != M_prcs-1) { strncat(f[i], white_space, 1); }
    // // }
    // // free(size_list);
    // // // if (fclose(fp)) { return 1; }

    // // for (int j = 0; j<N; j++) {
    // //     printf("%d %s\n", j, f[j]);
    // // }


    // // for (int i = 0; i<N; i++) {
    // //     free(f[i]);
    // // }
    // // free(f);
    return 0;
}