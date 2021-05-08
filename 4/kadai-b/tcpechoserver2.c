#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <memory.h>

#include "mytcp.h"

int receiveServerSet(int, char **);


int socketFd;

int main(int argc, char * argv[]){
    receiveServerSet(argc, argv);
}

int receiveServerSet(int argc, char *port[]){
    u_short portN;
    if(argc != 2){
        portN = DEFAULT_PORT_SERVER;

    }else{
        portN = atoi(port[1]);
    } 
    receiveServerEpoll(portN, 1, -1, NULL);
    return 0;
}
