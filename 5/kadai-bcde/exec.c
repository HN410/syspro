#include "parser/parse.h"
#include <unistd.h>

int myExec(process *p, int n, char **envp){
        process process = *p;
        char * programName = process.program_name;
        char ** argList = process.argument_list;
        execve(programName, argList, envp);
        return 0;
}