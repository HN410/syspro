#include "parser/parse.h"
#include <unistd.h>
#include "util/mycp.h"

int myExec(process *p, int n, char **envp){


        process process = *p;
        char * programName = process.program_name;
        char ** argList = process.argument_list;
        execve(programName, argList, envp);
        return 0;
}

void childPipeHandle(int *oldPipe, int *newPipe, process * p, int i, int n){
        close(newPipe[0]);
        close(oldPipe[1]);
        char *redirection;

        if(i != n-1 || p->output_redirection != NULL) dup2(newPipe[1], 1);
        else  close(newPipe[1]);
        if(i != 0 || p->input_redirection != NULL) dup2(oldPipe[0], 0);
        else close(oldPipe[0]);
}

void inputRedirect(int pipe, process * process){
    mycpEx(COPY_FILE_TO_FD, process->input_redirection, NULL, pipe);
}

void outputRedirect(int pipe, process * process){
    mycpEx(COPY_FD_TO_FILE, NULL, process->output_redirection, pipe);
}
