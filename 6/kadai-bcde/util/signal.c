#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <wait.h>
#include "signal.h"
#include "job.h"

void sigStop(int signal){
    printf("\n");
    void (*sig)(int ) = SIG_DFL;
    sig(signal);
}

void bg(BackProcessList **bpl){

    if((*bpl) == NULL){
        return;
    }

    pid_t pids[(*bpl)->processN];
    for(int i = 0; i < (*bpl)->processN; i++){
        pids[i] = (*bpl)->pids[i];
    }

    pid_t pid = (**bpl).pids[0];
    int processN = (*bpl)->processN;
    //bplの始末
    BackProcessList *beforeBp = (*bpl);
    (*bpl) = (**bpl).next;
    freeBackProcess(beforeBp);

    //フォアグラウンドへ
    if(tcsetpgrp(STDIN_FILENO, pids[0]) == -1){
        perror("Error: tcsetpgrp\n");
        exit(1);
    }

    if(kill(pid, SIGCONT) != 0){
        perror("Error: kill");
        exit(1);
    }  
    if(tcsetpgrp(STDIN_FILENO, getpid()) == -1){
        perror("Error: tcsetpgrp\n");
        exit(1);
    }

    for(int i = 0; i < processN; i++){
        if(pids[i] > 0){
            //終了していないプロセスをwait
            int status;
            waitpid(pids[i], &status, WUNTRACED);
        }
    }   

  
}