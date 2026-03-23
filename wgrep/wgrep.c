#include <stdio.h>
#include <string.h>
#define TOT 4096
//esse programa lê um arquivo linha por linha buscando um padrão fornecido pelo usuário
//somente as linhas que contiverem o padrão serão impressas

void grep(FILE *f, char *str){
    char buf[TOT];

    while(fgets(buf, sizeof buf, f) != NULL){
        if(strstr(buf, str) != NULL){
            printf("%s", buf);
        }
    }
    
}

int processa_arquivo(char* nome_f, char* str){
    
    FILE *f = fopen(nome_f, "r");

    if(f == NULL){
        fprintf(stderr,"wgrep: cannot open file\n");
        return 1;   //erro
    }

    //grep
    
    grep(f, str);

    fclose(f);
    return 0;
}

int main(int argc, char* argv[]){
    if(argc < 2){
        fprintf(stderr, "wgrep: searchterm [file ...]\n");
        return 1;
    }

    char* str = argv[1];
    int erro = 0;

    //sem arquivo em stdin
    if(argc == 2){
        grep(stdin, str);
        return 0;
    }

    for (int i = 2; i < argc; i++){
        if(processa_arquivo(argv[i], str) == 1){
            erro = 1;
        }
    }
    
    return erro;
}