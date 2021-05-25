#include <stdio.h>
#include <unistd.h>
#include <wait.h>
#include <string.h>
#include <stdlib.h>
#include "parser/parse.h"
#include "exec.h"



#define INPUT_SIZE 100

int getNewProcessN(job *);

int main(int argc, char *argv[], char *envp[]){
    job *job;
    char input[INPUT_SIZE];
    while(1){
        get_line(input, INPUT_SIZE -1);
        if(strcmp(input, "exit\n") == 0){
            break;
        }
        job = parse_line(input);

        if(job != NULL){
            process * process = job->process_list;
            int newProcessN = getNewProcessN(job);
            int pid[newProcessN];
            int oldPipe[2];
            pid_t gpid = 0;


            if(pipe(oldPipe) < 0) {
                perror("pipe(): ");
                exit(1);
            };

            for(int i = 0 ; i < newProcessN ; i ++){
                int newPipe[2];
                if(pipe(newPipe) < 0){
                    perror("pipe(): ");
                    exit(1);
                };
                pid[i] = fork();
                if(pid[i] == 0){

                    childPipeHandle(oldPipe, newPipe, process, i, newProcessN);

                    myExec(process, i, envp);
                    printf("Error: Failed to exec the program.\n");
                    exit(1);        
                }

                //プロセスグループIDの設定
                int err = 0;
                if(i==0){
                    gpid = pid[0];
                    err = setpgid(gpid, gpid);
                }else{
                    err = setpgid((pid_t) pid[i], gpid);
                }
                if(err  == -1){
                    perror("Error: setgroup\n");
                    exit(1);
                }

                //パイプ、リダイレクション処理
                close(oldPipe[0]);
                if(process->input_redirection != NULL){
                    inputRedirect(oldPipe[1], process);
                }
                close(oldPipe[1]);

                if(process->output_redirection != NULL){
                    outputRedirect(newPipe[0], process);
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