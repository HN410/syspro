#include <stdlib.h>
#include <stdio.h>
#include <wait.h>
#include "job.h"


void addBackground(int pid[], int processN, BackProcessList** backProcessList){
    //processN個のプロセスをバックグラウンド実行に加える
    int *pids = calloc(processN, sizeof(int));
    for(int i = 0; i < processN; i++){
        pids[i] = pid[i];
    }
    BackProcessList *back = calloc(1, sizeof(BackProcessList));
    back->pids = pids;
    back->processN = processN;
    back->next = NULL;

    if((*backProcessList) == NULL){
        back-> previous = NULL;
        (*backProcessList) = back;
    }else{
        BackProcessList *bp = *backProcessList;
        while(bp->next!= NULL) bp = bp->next;
        back->previous = bp;
        bp->next = back;
    }
}

void freeBackProcess(BackProcessList *backProcessList){
    //backProcessList一要素を削除
    if(backProcessList->next != NULL){
        backProcessList->next->previous = backProcessList->previous;
    }
    if(backProcessList->previous != NULL){
        backProcessList->previous->next = backProcessList->next;
    }
    free(backProcessList->pids);
    free(backProcessList);
    return;
}

void checkBackProcess(BackProcessList **backProcessList){
    BackProcessList *bPL = *backProcessList;
    while(bPL != NULL){
        int allEnd = 0;
        for(int i = 0; i < bPL->processN ; i++){
            if(bPL->pids[i] >= 0){
                int stat;
                pid_t waitRes = 0;
                waitRes = waitpid((pid_t) bPL->pids[i], &stat, WNOHANG);
                if(waitRes < 0){
                    perror("Error: BackProcess waitPid\n");
                    exit(1);
                }else if(waitRes != 0){
                    bPL->pids[i] = PROCESS_END;
                }else{
                    //終わっていないプロセスがまだある
                    allEnd = 1;
                }
            }
        }
        if(allEnd == 0){
            //bPL内のプロセスがすべて終了した
            BackProcessList *deletePL = bPL;
            if((*backProcessList) == bPL){
                //BPLの先頭要素を消すときは書き換え
                (*backProcessList) = bPL->next;
            }
            bPL = bPL->next;
            freeBackProcess(deletePL);
        }else{
            bPL = bPL->next;
        }
    }

}