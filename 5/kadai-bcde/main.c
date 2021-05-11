#include <stdio.h>
#include <unistd.h>
#include <wait.h>
#include <stdlib.h>
#include "parser/parse.h"
#include "exec.h"


#define INPUT_SIZE 100


int main(int argc, char *argv[], char *envp[]){
    pid_t pid;
    job *job;
    char input[INPUT_SIZE];
    get_line(input, INPUT_SIZE -1);
    job = parse_line(input);

    if((pid = fork()) == 0){
        myExec(job, 0, envp);
        printf("Error: Failed to exec the program.");
        exit(1);        
    }else {
        int status;
        wait(&status);
    }    
    return 0;
}
