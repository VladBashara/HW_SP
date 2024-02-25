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
    printf("%d : %s (%s)\n", errno, strerror(errno), add_msg);
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

int checkdebug(char* buf) {
    char* expect = "6.6742358875";
    int counter = 0;
    for (int i = 0; i < strlen(expect); i++) {
        if (buf[i] == expect[i]) {
            counter++;
        }
    }
    if (counter == strlen(expect)) {
        return 1;
    }
    return 0;
}

void debug() {
    printf("COOl\n");
}

void split_str(char* buf, size_t buf_size, char*** pos_list, int** size_list, int* len) {
    *len = count_str(buf, '.');
    *size_list = malloc(*len * sizeof(int));
    *pos_list = malloc(*len * sizeof(char*));

    size_t start_pos = (size_t)buf;
    char symbol = ' ';
    char* ptr = NULL;
    for (int i = 0; i < *len; i++) {
        (*pos_list)[i] = buf;
        if (checkdebug(buf)) {
            debug();
        }
        ptr = strchr(buf, (size_t)symbol);
        if (ptr == NULL) {
            (*size_list)[i] =  (size_t)buf_size-(size_t)buf+start_pos;
        } else {
            (*size_list)[i] = (size_t)ptr-(size_t)buf;
            buf = &ptr[1];
        }
    }
}
    
int write_slices_to_files(char* buf, long file_size, int N, int M_prcs, int M_last_prcs) {
    int len = 0;
    int* size_list = NULL;
    char** pos_list = NULL;
    split_str(buf, (size_t)file_size, &pos_list, &size_list, &len);
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

void waiting_all_proccesses(int N, char* delay_str) {
    int ret_code;
    sleep(4*atoi(delay_str));
    for (int j = 0; j < N; j++) {
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