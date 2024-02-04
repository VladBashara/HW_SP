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

    check_if_argc_equal_to(argc, 4);
    FILE* fp = check_if_file_exists(argv[1]);
    char* delay_str = argv[3];
    long file_size = get_file_size(fp);
    int N = atoi(argv[2]);

    char* buf = malloc(file_size * sizeof(char));
    fread(buf, 1, file_size, fp); // read file_size bytes from file to buffer
    if (ferror(fp)) {
        free(buf);
        print_error_msg("Can`t read the file");
        exit(EXIT_FAILURE);
    }
    if (fclose(fp)) {
        free(buf);
        print_error_msg("Can`t close the file");
        exit(EXIT_FAILURE);
    }
    long M = count_str(buf, '.');
    if (M < 2) { printf("ERROR: M < 2\n"); free(buf); exit(EXIT_FAILURE); }
    
    
    int M_prcs;
    int M_last_prcs;
    calc(M, &N, &M_prcs, &M_last_prcs);
    printf("M - data: %ld\n", M);
    printf("N - subproccesses: %d\n\n", N);
    
    if (write_slices_to_files(buf, file_size, N, M_prcs, M_last_prcs)) {
        printf("DEBUGxd\n");
        free(buf);
        print_error_msg("");
        exit(EXIT_FAILURE);
    }
    free(buf);
    calling_proccesses(N, delay_str);
    waiting_all_proccesses(N, delay_str);
    double result = 0;
    for (int i = 0; i < N; i++)  { // summation
        char folder[] = "summed_arrays/";
        printf("OKAY\n");
        FILE* fp = fopen(strcat(folder, i_to_a(i+1)), "rb");
        if (ferror(fp)) {
            
            exit(EXIT_FAILURE);
        }
        long file_size = get_file_size(fp);
        buf = malloc(file_size * sizeof(char));
        printf("OKAY\n");
        fread(buf, 1, file_size, fp);
        if (ferror(fp)) {
            free(buf);
            exit(EXIT_FAILURE);
        }

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
        if (fclose(fp)) {
            print_error_msg("Can`t close the file");
            exit(EXIT_FAILURE);
        }
    }
    printf("\nResult = %f\n", result);

    exit(EXIT_SUCCESS);
}