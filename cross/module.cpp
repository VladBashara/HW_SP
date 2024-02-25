int getIntSize(int n) {
    if (n == 0) { return 1; }
    int i = 0;
    while (n != 0) {
        n = (int)(n / 10);
        i++;
    }
    return i;
}

void print_error_msg(const char* add_msg) {
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
    const char* expect = "6.6742358875";
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
    *size_list = (int*)malloc(*len * sizeof(int));
    *pos_list = (char**)malloc(*len * sizeof(char*));

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