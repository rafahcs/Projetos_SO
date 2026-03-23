#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("wzip: file1 [file2...]\n");
        return 1;
    }

    int prev_char = EOF;
    int count = 0;
    int current;

    for (int i = 1; i < argc; i++) {
        FILE *fp = fopen(argv[i], "r");
        if (fp == NULL) {
            printf("wzip: cannot open file\n");
            return 1;
        }

        while ((current = fgetc(fp)) != EOF) {
            if (prev_char == EOF) {
                prev_char = current;
                count = 1;
            } else if (current == prev_char) {
                count = count + 1;
            } else {
                fwrite(&count, sizeof(int), 1, stdout);
                char c = (char)prev_char;
                fwrite(&c, 1, 1, stdout);
                prev_char = current;
                count = 1;
            }
        }
        fclose(fp);
    }

    if (prev_char != EOF) {
        fwrite(&count, sizeof(int), 1, stdout);
        char c = (char)prev_char;
        fwrite(&c, 1, 1, stdout);
    }

    return 0;
}