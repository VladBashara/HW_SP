#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include "itoa.c"
#include <ctype.h>

long get_file_size(FILE* fp) {
    fseek(fp, 0, SEEK_CUR);
    long offset = ftell(fp);
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, offset, SEEK_SET);
    return size;
}

int main(int argc, char* argv[]) {
    printf("child sleep(%d)\n", atoi(argv[1]));
    sleep(atoi(argv[1]));
    char folder_read[] = "splitted_arrays/";
    char folder_write[] = "summed_arrays/";
    char* file_name = malloc(strlen(argv[0]) * sizeof(char)); 
    strcpy(file_name, argv[0]);
    FILE* fp = fopen(strcat(folder_read, file_name), "rb");
    int result = 0;
    char buf;

    for (int i = 0; i < get_file_size(fp); i++) {
        fread(&buf, sizeof(char), 1, fp);
        if (isdigit(buf)) { result++; }
    }

    fclose(fp);
    char* result_str = i_to_a(result);
    fp = fopen(strcat(folder_write, file_name), "wb");
    fwrite(result_str, 1, strlen(result_str), fp);
    fclose(fp);
    free(file_name);
    exit(EXIT_SUCCESS);
}