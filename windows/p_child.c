#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <synchapi.h>
#include <windows.h>
#include "itoa.c"
#include <ctype.h>
#include "module.c"

int main(int argc, char* argv[]) {
    Sleep(1000*atoi(argv[1]));
    char folder_read[] = "splitted_arrays/";
    char folder_write[] = "summed_arrays/";
    char* file_name = malloc(strlen(argv[0]) * sizeof(char));
    strcpy(file_name, argv[0]);
    FILE* fp = fopen(strcat(folder_read, file_name), "rb");
    if (fp == NULL) {
        free(file_name);
        fclose(fp);
        ExitProcess(1);
    }
    double result = 0;
    char* buf = malloc(get_file_size(fp) * sizeof(char));
    fread(buf, sizeof(char), get_file_size(fp), fp);

    int len = 0;
    long* size_list = NULL;
    char** pos_list = NULL;
    split_str(buf, get_file_size(fp) , &pos_list, &size_list, &len);
    fclose(fp);
    char* dest = NULL;
    for (int i = 0; i < len; i++) {
        dest = malloc(size_list[i] * sizeof(char));
        strncpy(dest, pos_list[i], size_list[i]);
        result += atof(dest);
        free(dest);
    };

    char* result_str = malloc(1000 * sizeof(char));
    gcvt(result, 100, result_str);
    fp = fopen(strcat(folder_write, file_name), "w");
    if (fp == NULL) {
        free(result_str);
        fclose(fp);
        ExitProcess(1);
    }
    if (fwrite(result_str, 1, strlen(result_str), fp) < strlen(result_str)) {}
    fclose(fp);
    free(file_name);
    free(buf);
    free(size_list);
    free(pos_list);
    free(result_str);
    ExitProcess(0);
}