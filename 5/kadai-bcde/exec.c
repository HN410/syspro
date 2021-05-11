#include "parser/parse.h"
#include <unistd.h>

int myExec(job *job, int n, char **envp){
        process process = job -> process_list[n];
        char * programName = process.program_name;
        char ** argList = process.argument_list;
        execve(programName, argList, envp);
        return 0;
}