#include <stdio.h>
#include <stdbool.h>

void processa_arquivo(char* nome_f){
    FILE *f = fopen(nome_f, "r");
    FILE *f2 = fopen("dest.txt", "a") ; //modo "a"(append) adiciona ao final a cada execução

    if(f == NULL){
        printf("Erro ao abrir %s", nome_f);
    }

    //leitura de 'f' e escrita em dest.txt
    char buf[70];
    while(fgets(buf, sizeof buf, f) != NULL){
        fputs(buf, f2);
    }

    fclose(f);
}

int main(int argc, char *argv[]){
    //entrada dos arquivos
    if(argc < 2){
        printf("Uso: %s file1 file2 ...", argv[argc]);
        return 1;
    }
    //cat
    //loop começa em i=1 pois 0 é o nome do executado
    for(int i = 1; i < argc; i++){
        processa_arquivo(argv[i]);
    }
}