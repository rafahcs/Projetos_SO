#include<stdio.h>
#include<unistd.h>
#include<string.h>

int main(int argc, char *argv[]){
    
    printf("main:begin\n");

    int rc = fork(); //create nearly identical copy of program, returns here
    if(rc == 0){    //child
        printf("child\n");

        char *myargv[10];
        myargv[0] = strdup("./main2");
        myargv[1] = strdup("argtomain2whichisignored");
        myargv[2] = NULL; //important

        execv(myargv[0], myargv);
        printf("exec failed!\n");
    } else{ //parent
        rc = (int) wait(NULL); //BLOCKS PARENT(until child done)
        printf("parent: done waiting for terminated child process\n");
    }

    return 0;
}