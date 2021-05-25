#include <stdio.h>
#include <unistd.h>
#include <wait.h>
#include <stdlib.h>

int main(int argc, char *argv[], char *envp[]){
    pid_t pid;
    if(argc < 2){
        printf("Error: Too few arguments.");
        exit(1);
    }
    if((pid = fork()) == 0){
        char ** arg;
        if(argc > 3) arg = argv+1;
        else arg = NULL;
        execve(argv[1], arg, envp);
        printf("Error: Failed to exec the program.");
        exit(1);        
    }else {
        int status;
        wait(&status);
    }    
    return 0;
}