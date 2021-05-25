#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <stdlib.h>
#include "util/mycp.h"
#include "parser/parse.h"


int checkExecutable(char *path, const char *pathBefore){
    //ファイルが実行可能なら1, そうでなければ0
    struct stat st;
    char *programName;

    if(pathBefore == NULL){
        programName = calloc(strlen(path) + 1, sizeof(char));
        strcpy(programName, path);
    }else{
        int programNameLen = strlen(pathBefore) + strlen(path) + 1;
        programName = calloc(programNameLen + 1, sizeof(char));
        strcpy(programName, pathBefore);
        strcat(programName, "/");
        strcat(programName, path);
    }

    if(stat(programName, &st)!= 0){
        free(programName);
        return 0;
    }
    if((st.st_mode & S_IFMT) == S_IFREG){
        free(programName);
        return 1;
    }
    free(programName);
    return 0;

}

int myExec(process *p, int n, char **envp){
        process process = *p;
        char ** argList = process.argument_list;
        if(checkExecutable(p->program_name, NULL) != 0){
            //絶対ファイルで実行可能
            execve(p->program_name, argList, envp);
            return 0;
        }

        char *pathNext = strtok(getenv("PATH"), ":");
        while(pathNext != NULL){
            //1は/の分
            if(checkExecutable(p->program_name, pathNext) == 0){
                pathNext = strtok(NULL, ":");
                continue;
            }
            int programNameLen = strlen(pathNext) + strlen(p->program_name) + 1;
            char programName[programNameLen];
            strcpy(programName, pathNext);
            strcat(programName, "/");
            strcat(programName, p->program_name);

            argList[0] = programName;
            execve(programName, argList, envp);
        }


        return 0;
}

void childPipeHandle(int *oldPipe, int *newPipe, process * p, int i, int n){
        close(newPipe[0]);
        close(oldPipe[1]);
        
        if(i != n-1 || p->output_redirection != NULL) dup2(newPipe[1], 1);
        else  close(newPipe[1]);
        if(i != 0 || p->input_redirection != NULL) dup2(oldPipe[0], 0);
        else close(oldPipe[0]);
}

void inputRedirect(int pipe, process * process){
    mycpEx(COPY_FILE_TO_FD, process->input_redirection, NULL, pipe, 0);
}

void outputRedirect(int pipe, process * process){
    int isAppend = 0;
    if(process->output_option == APPEND) isAppend = 1;
    mycpEx(COPY_FD_TO_FILE, NULL, process->output_redirection, pipe, isAppend);
}
