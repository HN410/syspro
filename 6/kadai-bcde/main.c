#include <stdio.h>
#include <unistd.h>
#include <wait.h>
#include <string.h>
#include <stdlib.h>
#include "parser/parse.h"
#include "exec.h"
#include "util/job.h"
#include "util/signal.h"



#define INPUT_SIZE 100

int getNewProcessN(job *);

int main(int argc, char *argv[], char *envp[]){
    job *job;
    BackProcessList *bpl0 = NULL;
    BackProcessList *bpl1 = NULL;
    BackProcessList *bpl2 = NULL;
    BackProcessList **backProcessList = &bpl0; //バックグラウンド実行
    BackProcessList **pauseProcessList = &bpl1;; //中断中のプロセス
    BackProcessList **nowProcessList = &bpl2;;   //現在のプロセス
    pid_t parentPID = 0;

    char input[INPUT_SIZE];

    //フォアグラウンドに変更したときにSIGTTOUが送られるので、それを無視
    struct sigaction sigAction;
    memset((void *) &sigAction, 0, sizeof(struct sigaction));
    sigAction.sa_handler = SIG_IGN;
    sigAction.sa_flags = 0;
    sigemptyset(&(sigAction.sa_mask));
    if(sigaction(SIGINT, &sigAction, NULL) == -1){
        perror("Error: sigaction");
        exit(1);
    }
    if(sigaction(SIGTSTP, &sigAction, NULL) == -1){
        perror("Error: sigaction");
        exit(1);
    }
    if(sigaction(SIGTTOU, &sigAction, NULL) == -1){
        perror("Error: sigaction");
        exit(1);
    }
    if(sigaction(SIGTTIN, &sigAction, NULL) == -1){
        perror("Error: sigaction");
        exit(1);
    }
    parentPID = getpid();

    while(1){
        get_line(input, INPUT_SIZE -1);
        if(strcmp(input, "exit\n") == 0){
            break;
        }else if(strcmp(input, "bg\n") == 0){
            bg(pauseProcessList);
            continue;
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
                    if(i == 0){
                        struct sigaction sigAction;
                        memset((void *) &sigAction, 0, sizeof(struct sigaction));
                        sigAction.sa_handler = SIG_DFL;
                        sigAction.sa_flags = 0;
                        sigemptyset(&(sigAction.sa_mask));
                        if(sigaction(SIGINT, &sigAction, NULL) == -1){
                            perror("Error: sigaction");
                            exit(1);
                        }
                        sigAction.sa_handler = sigStop;
                        if(sigaction(SIGTSTP, &sigAction, NULL) == -1){
                            perror("Error: sigaction");
                            exit(1);
                        }
                    }
                    childPipeHandle(oldPipe, newPipe, process, i, newProcessN);

                    myExec(process, i, envp);
                    printf("Error: Failed to exec the program.\n");
                    exit(1);        
                }

                //プロセスグループIDの設定、フォアグラウンド、バックグラウンドへ
                int err = 0;
                if(i==0){
                    gpid = pid[0];
                    err = setpgid(gpid, gpid);
                }else{
                    err = setpgid((pid_t) pid[i], gpid);
                }
                if(err  == -1){
                    printf("%d\n", getsid(gpid));
                    printf("%d\n", getsid(0));
                    perror("Error: setgroup\n");
                    exit(1);
                }
                if(job->mode == FOREGROUND){

                    if(tcsetpgrp(STDIN_FILENO, gpid ) == -1){
                        perror("Error: tcsetpgrp\n");
                        exit(1);
                    }  
                }else{

                    if(tcsetpgrp(STDIN_FILENO, parentPID ) == -1){
                        perror("Error: tcsetpgrp\n");
                        exit(1);
                    }  
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

            if(job->mode == FOREGROUND){
                int stoppedFlag = 0; ///CtrlZで一時停止されたか
                for(int i = 0; i < newProcessN; i++){
                    int status;
                    waitpid((pid_t) pid[i], &status, WUNTRACED);
                    if(!WIFEXITED(status)){
                        stoppedFlag = 1;
                    }else{
                        pid[i] = PROCESS_END;
                    }  
                }   
                addBackground(pid, newProcessN, nowProcessList);

                //実行終了につきシェルをフォアグラウンドへ
                if(tcsetpgrp(STDOUT_FILENO, getpid()) == -1){
                    perror("Error: tcsetpgrp\n");
                    exit(1);
                }  
                if(tcsetpgrp(STDIN_FILENO, getpid() ) == -1){
                            perror("Error: tcsetpgrp\n");
                            exit(1);
                }  
                if(stoppedFlag){
                    addProcessList(pauseProcessList, nowProcessList);
                }else{
                    freeBackProcess(*nowProcessList);                    
                }
                *nowProcessList = NULL;
                
            }else{
                addBackground(pid, newProcessN, backProcessList);
            }

            checkBackProcess(backProcessList);

        }
        free_job(job);
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