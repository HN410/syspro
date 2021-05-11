#include <stdio.h>
#include <unistd.h>
#include <wait.h>
#include <stdlib.h>
#include "parser/parse.h"
#include "exec.h"


#define INPUT_SIZE 100

int getNewProcessN(job *);

int main(int argc, char *argv[], char *envp[]){
    pid_t pid;
    job *job;
    char input[INPUT_SIZE];
    get_line(input, INPUT_SIZE -1);
    job = parse_line(input);

    if(job != NULL){
        process * process = job->process_list;
        int newProcessN = getNewProcessN(job);
        int pid[newProcessN];
        int oldPipe[2];

        for(int i = 0 ; i < newProcessN ; i ++){
            int newPipe[2];
            pipe(newPipe);
            pid[i] = fork();
            if(pid[i] == 0){
                close(newPipe[0]);
                close(oldPipe[1]);

                if(i != newProcessN-1) dup2(newPipe[1], 1);
                else close(newPipe[1]);
                if(i == 0) close(oldPipe[0]);
                else dup2(oldPipe[0], 0);

                myExec(process, i, envp);
                printf("Error: Failed to exec the program.");
                exit(1);        
            }
            if(i != 0){
                close(oldPipe[0]);
                close(oldPipe[1]);
            }
            oldPipe[0] = newPipe[0];
            oldPipe[1] = newPipe[1];
            
            process = process->next;
        }
        close(oldPipe[0]);
        close(oldPipe[1]);

        for(int i = 0; i < newProcessN; i++){
            int status;
            waitpid((pid_t) pid[i], &status, WUNTRACED);
        }    
    }
    return 0;
}

int getNewProcessN(job *job){
    int res = 1;
    process *p = job->process_list;
    while(p->next != NULL){
        res++;
        p = p->next;
    }
    return res;
}