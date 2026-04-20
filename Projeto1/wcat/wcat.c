#include <stdio.h>
#include <stdbool.h>

int processa_arquivo(char* nome_f){
    FILE *f = fopen(nome_f, "r");

    if(f == NULL){
        fprintf(stdout,"wcat: cannot open file\n");
        return 1; //Indica erro
    }

    //leitura de 'f' e escrita em stdout(saída padrão)
    char buf[70];
    while(fgets(buf, sizeof buf, f) != NULL){
        fputs(buf, stdout); 
    }

    fclose(f);
    return 0; //Sucesso
}

int main(int argc, char *argv[]){
    int erro = 0;
    //entrada dos arquivos
    if(argc < 2){
        return 0;
    }
    //cat
    //loop começa em i=1 pois 0 é o nome do executado
    for(int i = 1; i < argc; i++){
        processa_arquivo(argv[i]);
    
    }

    return erro;
}
