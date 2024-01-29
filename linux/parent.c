#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include "itoa.c"
#include <ctype.h>

int getIntSize(int n) {
    if (n == 0) { return 1; }
    int i = 0;
    while (n != 0) {
        int last_num = n - ((int)(n / 10)*10);
        n = (int)(n / 10);
        i++;
    }
    return i;
}

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

// void write_slices_to_files(char* buf, int N, int M_prcs, int M_last_prcs) {
//     FILE* fp;
//     for (int i = 0; i < N-1; i++) { // writing slices of array to new files for N-1 proccesses
//         char folder[] = "splitted_arrays/";
//         fp = fopen(strcat(folder, i_to_a(i+1)), "wb");
//         fwrite(&buf[i*M_prcs], 1, M_prcs, fp);
//         fclose(fp);
//     }
//     char folder[] = "splitted_arrays/";
//     fp = fopen(strcat(folder, i_to_a(N)) , "wb"); // writing slices of array to new files for N (last) proccess
//     fwrite(&buf[(N-1)*M_prcs], 1, M_last_prcs, fp);
//     fclose(fp);
// }

int count_str(char* buf, char symbol) {
    int i = 0;
    char* ptr = NULL;
    ptr = strchr(buf, (int)symbol);
    buf = &ptr[1];
    while (ptr != NULL) {
        ptr = strchr(buf, (int)symbol);
        buf = &ptr[1];
        i++;
    }
    return i;
}

void split_str(char* buf, long buf_size, char*** pos_list, long** size_list, int* len) {
    *len = count_str(buf, '.');
    printf("*len: %d\n", *len);
    printf("buf: %s\n", buf);
    if (*len == 0) { printf("ERROR"); }
    *size_list = malloc(*len * sizeof(long));
    *pos_list = malloc(*len * sizeof(char*));

    long start_pos = (long)buf;
    char symbol = ' ';
    char* ptr = NULL;
    for (int i = 0; i < *len; i++) {
        (*pos_list)[i] = buf;
        ptr = strchr(buf, (int)symbol);
        if (ptr == NULL) {
            printf("len: %ld\n", buf_size-(long)buf+start_pos);
            (*size_list)[i] =  buf_size-(long)buf+start_pos;
        } else {
            (*size_list)[i] = (long)ptr-(long)buf;
            buf = &ptr[1];
        }

        printf("\npos_list[%d]: %c\n", i, (**pos_list)[i]);
        printf("size_list[%d]: %ld\n", i, (*size_list)[i]);
    }
}
    
void write_slices_to_files(char* buf, long file_size, int N, int M_prcs, int M_last_prcs) {
    int len = 0;
    long* size_list = NULL;
    char** pos_list = NULL;
    split_str(buf, file_size, &pos_list, &size_list, &len);
    int counter = 0;

    for (int j = 0; j < N-1; j++) {
        char file[] = "splitted_arrays/";
        FILE* fp = fopen(strcat(file, i_to_a(j+1)), "wb");
        for (int i = 0; i < M_prcs; i++) {
            fwrite(pos_list[counter], 1, size_list[counter], fp);
            counter++;
            if (i != M_prcs-1) {fwrite(" ", 1, 1, fp);}
        }
        fclose(fp);
    }
    char file[] = "splitted_arrays/";
    FILE* fp = fopen(strcat(file, i_to_a(N)), "wb");
    for (int i = 0; i < M_last_prcs; i++) {
        fwrite(pos_list[counter], 1, size_list[counter], fp);
        counter++;
        if (i != M_last_prcs-1) {fwrite(" ", 1, 1, fp);}
    }
    fclose(fp);

    // for (int j = 0; j < len-1; j++) {
    //     char file[] = "splitted_arrays/";
    //     FILE* fp = fopen(strcat(file, i_to_a(j+1)), "wb");
    //     fwrite(pos_list[j], 1, size_list[j], fp);
    //     fwrite(" ", 1, 1, fp);
    //     fwrite(pos_list[j+1], 1, size_list[j+1], fp);
    //     fclose(fp);
    // }
    // char file[] = "splitted_arrays/";
    // FILE* fp = fopen(strcat(file, i_to_a(len)), "wb");
    // fwrite(pos_list[len-1], 1, size_list[len-1], fp);
    // fwrite(" ", 1, 1, fp);
    // fwrite(pos_list[len], 1, size_list[len], fp);
    // fclose(fp);
    // free(size_list);
    // free(pos_list);
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

// void separate_str_by_space(char* str, int str_size, char** list) { // TODO: ADD ARRAY LENGHT ARG
//     for (int i = 0; i < str_size; i++) { 
        
//     }
// }

int main(int argc, char* argv[]) {

    check_if_argc_equal_to(argc, 4);
    FILE* fp = check_if_file_exists(argv[1]);
    char* delay_str = argv[3];
    long file_size = get_file_size(fp);
    int N = atoi(argv[2]);

    char* buf = malloc(file_size * sizeof(char));
    fread(buf, 1, file_size, fp); // read file_size bytes from file to buffer
    fclose(fp);
    long M = count_str(buf, '.');
    if (M < 2) { exit(EXIT_FAILURE); }
    
    
    int M_prcs;
    int M_last_prcs;
    calc(M, &N, &M_prcs, &M_last_prcs);
    printf("M - data: %ld\n", M);
    printf("N - subproccesses: %d\n\n", N);
    
    write_slices_to_files(buf, file_size, N, M_prcs, M_last_prcs);
    free(buf);
    calling_proccesses(N, delay_str);
    waiting_all_proccesses(N, delay_str);
    printf("OKAY\n");
    double result = 0;
    for (int i = 0; i < N; i++)  { // summation
        char folder[] = "summed_arrays/";
        printf("OKAY\n");
        FILE* fp = fopen(strcat(folder, i_to_a(i+1)), "rb");
        long file_size = get_file_size(fp);
        buf = malloc(file_size * sizeof(char));
        printf("OKAY\n");
        fread(buf, 1, file_size, fp);

        if (i == 0) {
            printf("file_size: %ld\n", file_size);
            printf("buf: %s\n", buf);
        }
        char* result_str = malloc(file_size * sizeof(char));
        strncpy(result_str, buf, file_size);

        // char* point_pos = strchr(buf, (int)'.');
        // char* before_point = malloc((int)(point_pos-buf) * sizeof(char));
        // char* after_point = malloc((int)((char*)file_size-point_pos) * sizeof(char));
        // strncpy(before_point, buf, (int)(point_pos-buf));
        // strncpy(after_point, point_pos[1], (int)((char*)file_size-point_pos));


        // float whole_num = atoi(before_point);
        // float frac_part = (float)atoi(after_point) / 10^getIntSize(atoi(after_point));
        // free(before_point);
        // free(after_point);
        // result += whole_num + frac_part;
        char** end = NULL;
        result += atof(result_str);
        printf("atof(result_str) %f\n", atof(result_str));
        free(buf);
        free(result_str);
        fclose(fp);
    }
    printf("\nResult = %f\n", result);

    exit(EXIT_SUCCESS);
}