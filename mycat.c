#include <stdio.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("usage: ./mycat filename\n");
        return 1;
    } else {
        FILE *fp = fopen(argv[1], "r");
        if (fp == NULL) {
            printf("cannot open file\n");
            return 1;
        }
        char buffer[1024];
        // fgets will stop:
        //scan "\n"
        //numbers are already over the buffer "1024"
        //EOF
        // will add "\0" after "\n"
        while (fgets(buffer, 1024, fp) != NULL) {
            fputs(buffer, stdout);
        }
        printf("\n");
        fclose(fp);
    }
    return 0;
}