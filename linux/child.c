#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include "itoa.c"
#include <ctype.h>
#include "module.c"

int main(int argc, char* argv[]) {
    printf("child sleep(%d)\n", atoi(argv[1]));
    sleep(atoi(argv[1]));
    char folder_read[] = "splitted_arrays/";
    char folder_write[] = "summed_arrays/";
    char* file_name = malloc(strlen(argv[0]) * sizeof(char)); 
    strcpy(file_name, argv[0]);
    FILE* fp = fopen(strcat(folder_read, file_name), "rb");
    double result = 0;
    char* buf = malloc(get_file_size(fp) * sizeof(char));

    // for (int i = 0; i < get_file_size(fp); i++) {
    //     fread(&buf, sizeof(char), 1, fp);
    //     if (isdigit(buf)) { result++; }
    // }
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
    gcvt(result, 1000, result_str);
    fp = fopen(strcat(folder_write, file_name), "wb");
    fwrite(result_str, 1, strlen(result_str), fp);
    fclose(fp);
    free(file_name);
    free(buf);
    free(size_list);
    free(pos_list);
    free(result_str);
    exit(EXIT_SUCCESS);
}