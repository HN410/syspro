#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>

#include "mytcp.h"

int sendClientSet(int, char *argv[]);

int main(int argc, char *argv[]){
  sendClientSet(argc,  argv);
}

int sendClientSet(int argc, char *argv[]){
    u_short port;
    char *ip;
    if(argc != 3){
        port = DEFAULT_PORT_SERVER;
        ip = DEFAULT_IP;
    }else{
        port = atoi(argv[2]);
        ip = argv[1];
    }
    struct in_addr in_addr;
    if(inet_aton(ip, &in_addr)!= 1){
        printf("Error: inet_aton");
        return 1;
    }
    throughputClient(port, in_addr);
    return 0;
    

}
