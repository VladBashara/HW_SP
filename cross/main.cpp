#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
// #include <sys/wait.h>
#include <ctype.h>
#include <pthread.h>
#include <semaphore>
#include <iostream>
#include "itoa.c"
#include "module.cpp"
#include <cstdlib>
#include <chrono>
// #include <windows.h>
#include <thread>
#include <vector>

#if defined(_WIN32)
    #define THROW_ERROR_MSG throw_error_msg_windows
    void throw_error_msg_windows(const char* add_msg) {
        char buf[1000];
        std::string str_number = itoa(GetLastError(), buf, 10);
        LPSTR message;
        FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER, NULL, GetLastError(),
        MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT), (LPSTR)&message, 0, NULL);
        std::string str_error = message;
        str_error.pop_back(); // delete '\n'
        throw std::runtime_error(str_number + ": " + str_error + add_msg);
    }
#else
    #define THROW_ERROR_MSG throw_error_msg_linux
    void throw_error_msg_linux(const char* add_msg) {
        std::string str_number = i_to_a(errno);
        std::string str_error = strerror(errno);
        throw std::runtime_error(str_number + ": " + str_error + add_msg);
    }
#endif

int read_file_to_buf(FILE* fp, long file_size, char** buf) {
    *buf = (char*)malloc((file_size+1) * sizeof(char));
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
std::binary_semaphore sem{1};
void th_main(void* arg) {
    // sleep(((struct ARG*)arg)->delay);
    std::this_thread::sleep_for(std::chrono::seconds(((struct ARG*)arg)->delay));
    char* substrs = (char*)(((struct ARG*)arg)->substr);
    int len = 0;
    int* size_list = NULL;
    char** pos_list = NULL;
    split_str(substrs, strlen(substrs), &pos_list, &size_list, &len);
    sem.acquire();
    for (int i = 0; i < len; i++) {
        char* val = (char*)malloc(size_list[i]);
        strncpy(val, pos_list[i], size_list[i]);
        sum += atof(val);
        free(val);
    }
    sem.release();
    // int* r = (int*)malloc(sizeof(int));
    // *r = 12;
    // pthread_exit(r);
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
    try {
        
        if (argc != ARGC) {
        printf("ERROR: argc != %d\n", ARGC);
        return 1;
        }
        FILE* fp = fopen(argv[1], "rb");
        if (fp == NULL) {
            THROW_ERROR_MSG("cant open file");
        }
        printf("PID: %d\n", getpid());
        char* delay_str = argv[3];

        long file_size = get_file_size(fp);
        char* buf = NULL;
        if (read_file_to_buf(fp, file_size, &buf)) { free(buf); THROW_ERROR_MSG("read_file_to_buf"); }

        int N = atoi(argv[2]);
        int M = count_str(buf, '.');
        if (M < 2) { free(buf); THROW_ERROR_MSG("ERROR: M < 2\n"); }
        int M_prcs;
        int M_last_prcs;
        calcDist(&N, &M, &M_prcs, &M_last_prcs);
        
        char** substr = (char**)malloc(N * sizeof(char*));
        for (int i = 0; i < N; i++) {
            substr[i] = (char*)calloc(file_size, sizeof(char));
        }

        get_substr(&substr, &buf, file_size, N, M_prcs, M_last_prcs);
        
        pthread_t* id = (pthread_t*)malloc(N * sizeof(pthread_t));
        std::vector<std::thread> th;
        struct ARG* arg = (struct ARG*)malloc(N * sizeof(struct ARG));
        for (int i = 0; i<N; i++) {
            arg[i].delay = atoi(delay_str);
            arg[i].substr = substr[i];
            // pthread_create(&id[i], NULL, th_main, (void *)&(arg[i]));
            th.emplace_back([=](){th_main(&arg[i]);});

        }
        // sleep(atoi(delay_str));
        std::this_thread::sleep_for(std::chrono::seconds(atoi(delay_str)));
        for (int i = 0; i<N; i++) {
            th[i].join();
            // void* th_ret;
            // pthread_join(id[i], &th_ret);
            // free(th_ret);
        }

        for (int i = 0; i<N; i++) {
            free(substr[i]);
        }
        free(substr);
        free(buf);
        free(id);
        free(arg);
        printf("\nsum is %f\n", sum);
        return 0;
    }
    catch (std::exception &err) {
        std::cerr << err.what() << std::endl;
        return 1;
    }
}