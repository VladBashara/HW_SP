#include <synchapi.h>
#include <windows.h>

int getIntSize(int n) {
    if (n == 0) { return 1; }
    int i = 0;
    while (n != 0) {
        n = (int)(n / 10);
        i++;
    }
    return i;
}

void print_error_msg(char* add_msg) {
    // printf("%d : %s (%s)\n", errno, strerror(errno), add_msg);
    char message[1000] = {0};
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER, NULL,
                GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&message, 0, NULL);
    printf("%d : %s (%s)\n", GetLastError(), message, add_msg);
}

long get_file_size(FILE* fp) {
    fseek(fp, 0, SEEK_CUR);
    long offset = ftell(fp);
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, offset, SEEK_SET);
    return size;
}

int count_str(char* buf, char symbol) {
    int i = 0;
    char* ptr = NULL;
    ptr = strchr(buf, (int)symbol);
    if (ptr == NULL) { return 0; }
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
    if (*len == 0) { printf("ERROR?\n"); }
    *size_list = malloc(*len * sizeof(long));
    *pos_list = malloc(*len * sizeof(char*));

    long start_pos = (long)buf;
    char symbol = ' ';
    char* ptr = NULL;
    for (int i = 0; i < *len; i++) {
        (*pos_list)[i] = buf;
        ptr = strchr(buf, (int)symbol);
        if (ptr == NULL) {
            (*size_list)[i] =  buf_size-(long)buf+start_pos;
        } else {
            (*size_list)[i] = (long)ptr-(long)buf;
            buf = &ptr[1];
        }
    }
}
    
int write_slices_to_files(char* buf, long file_size, int N, int M_prcs, int M_last_prcs) {
    int len = 0;
    long* size_list = NULL;
    char** pos_list = NULL;
    split_str(buf, file_size, &pos_list, &size_list, &len);
    int counter = 0;

    for (int j = 0; j < N-1; j++) {
        char file[] = "splitted_arrays/";
        FILE* fp = fopen(strcat(file, i_to_a(j+1)), "wb"); if (fp == NULL) { return 1; }
        for (int i = 0; i < M_prcs; i++) {
            fwrite(pos_list[counter], 1, size_list[counter], fp); if (ferror(fp)) { return 1; }
            counter++;
            if (i != M_prcs-1) { fwrite(" ", 1, 1, fp); }
        }
        if (fclose(fp)) { return 1; }
    }
    char file[] = "splitted_arrays/";
    FILE* fp = fopen(strcat(file, i_to_a(N)), "wb");
    for (int i = 0; i < M_last_prcs; i++) {
        fwrite(pos_list[counter], 1, size_list[counter], fp); if (ferror(fp)) { return 1; }
        counter++;
        if (i != M_last_prcs-1) { fwrite(" ", 1, 1, fp); if (ferror(fp)) { return 1; } }
    }
    free(size_list);
    if (fclose(fp)) { return 1; }
    return 0;
}

void waiting_all_proccesses(int N, char* delay_str,  HANDLE** proc_descriptors,  HANDLE** thr_descriptors) {
    int ret_code;
    printf("parent sleep(%d)\n", 4*atoi(delay_str));
    Sleep(1000*4*atoi(delay_str));
    DWORD exit_code;
    for (int j = 0; j < N; j++) {
        // pid_t ret_wait = wait(&ret_code);
        if (WaitForSingleObject((*proc_descriptors)[j], INFINITE) == WAIT_FAILED) { printf("ERROR\n"); }
        if (!GetExitCodeProcess((*proc_descriptors)[j], &exit_code)) { printf("ERROR\n"); }
        if (exit_code == 1) { printf("ERROR\n"); }
        CloseHandle((*proc_descriptors)[j]);
        CloseHandle((*thr_descriptors)[j]);
    }
}

void calling_proccesses(int N, char* delay_str, HANDLE** proc_descriptors, HANDLE** thr_descriptors) {
    for (int i = 0; i < N; i++) {
        PROCESS_INFORMATION pi;
        STARTUPINFO si;
        GetStartupInfo(&si);
        char* argv[3] = {i_to_a(i+1), delay_str, NULL};
        BOOL res = CreateProcess( "child.exe", argv, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);
        (*proc_descriptors)[i] = pi.hProcess;
        (*thr_descriptors)[i] = pi.hThread;
    }
}