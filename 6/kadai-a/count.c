#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <memory.h>
#include <unistd.h>

#define WAIT_USECONDS 1000
#define COUNT_LIMIT 10

void countSig();

volatile int count;
int main(){
    countSig();
    return 0;
}

void sigintCount(int sinal){
    count++;

}

void countSig(){
    count = 0;
    struct sigaction new, old;
    memset((void *) &new, 0, sizeof(struct sigaction));
    memset((void *) &old, 0, sizeof(struct sigaction));
    
    new.sa_handler = sigintCount;
    new.sa_flags = 0;
    sigemptyset(&(new.sa_mask));

    int ret = sigaction(SIGINT, &new, &old);

    if( ret == -1){
        perror("Error: sigaction");
        exit(1);
    }
    //ブロック用の集合を作る
    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGINT);

    while(1){
        usleep(WAIT_USECONDS);

        //いったんブロックして確認
        sigprocmask(SIG_BLOCK, &sigset, NULL);
        if(count >= COUNT_LIMIT){
            printf("exit\n");
            break;
        }
        sigprocmask(SIG_UNBLOCK, &sigset, NULL);
    }
}