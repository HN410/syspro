#ifndef EXEC_H_INCLUDED
#define EXEC_H_INCLUDED

int myExec(process *, int, char **);
void childPipeHandle(int *, int *, process*, int , int);
void inputRedirect(int , process *);
void outputRedirect(int , process *);
#endif