#include <stdio.h>
#include <stdlib.h>

#define MAX 1024

// estado global da compressão
static int count = 0;
static char prev;
static int first = 1;

int zip(char *nome_f) {

    FILE *f = fopen(nome_f, "r");
    if (f == NULL) {
        fprintf(stderr, "wzip: cannot open file\n");
        return 1;
    }

    char buf[MAX];
    size_t n;

    while ((n = fread(buf, 1, sizeof(buf), f)) > 0) {

        for (size_t i = 0; i < n; i++) {

            if (first) {
                prev = buf[i];
                count = 1;
                first = 0;
                continue;
            }

            if (buf[i] == prev) {
                count++;
            } else {
                fwrite(&count, sizeof(int), 1, stdout);
                fwrite(&prev, sizeof(char), 1, stdout);
                prev = buf[i];
                count = 1;
            }
        }
    }

    fclose(f);
    return 0;
}

int main(int argc, char *argv[]) {

    if (argc < 2) {
        fprintf(stderr, "wzip: file1 [file2 ...]\n");
        return 1;
    }

    int erro = 0;

    for (int i = 1; i < argc; i++) {
        if (zip(argv[i])) {
            erro = 1;
        }
    }

    // escreve último grupo
    if (count > 0) {
        fwrite(&count, sizeof(int), 1, stdout);
        fwrite(&prev, sizeof(char), 1, stdout);
        
    }

    return erro;
}