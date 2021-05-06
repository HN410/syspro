#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <memory.h>
#include <pthread.h>
#include "mytcp.h"

void sendTCP(ReceiveData, int);
void tcpEcho(int);

int sendClient(u_short port, struct in_addr in_addr, FILE *fp){
    //port, ip, 入力ストリームを受け取ってそのまま送信
    struct sockaddr_in sockAddr;
    sockAddr.sin_family = PF_INET;
    sockAddr.sin_port= htons(port);
    sockAddr.sin_addr= in_addr;
    
    int socketFd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(socketFd == -1){
        printf("Error: socket()");
    }

    socklen_t clientLen = sizeof(sockAddr);   
    if(connect(socketFd, (struct sockaddr *) &sockAddr, clientLen) < 0){
        perror("connect: ");
        exit(1);
    }
    printf("connect\n");

    int endFlag0 = 1;
    char receiveBuf[BUFFER_SIZE];
    while(endFlag0){
        printf("read: ");
        char buffer[BUFFER_SIZE];
        unsigned readN = fread((void *)buffer, sizeof(char), BUFFER_SIZE, fp);
        if( readN < BUFFER_SIZE){
            endFlag0 = 0;
            buffer[readN] = EOF;
            readN ++;
        }

        int resN = readN;
        char *writeP = buffer;
        while(resN > 0){
            int sendN =write(socketFd, buffer, readN);
            resN -= sendN;
            writeP += sendN;
        }
        int receiveSize = readN;
        while(receiveSize){
            int readSize = read(socketFd, receiveBuf, (int) BUFFER_SIZE-1);
            receiveBuf[readSize] = 0;
            receiveSize -= readSize;
            if(readSize == 0 ){
                printf("end\n");
                //終了
                receiveSize = 0;
            }else if(readSize == -1){
                perror("Error: Failed to read.\n");
                exit(1);
            }
            printf("%s", receiveBuf);
        }
        
    }
    close(socketFd);
    return 0;       
}



int receiveServer(u_short port, int isPrinting, int socketFdIn, struct sockaddr_in * sockAddrIn){
    struct sockaddr_in sockAddr;

    int socketFd;
    if(socketFdIn >= 0){
        //使用しない
        socketFd = socketFdIn;
        sockAddr = *sockAddrIn;
    }else{
        
        socketFd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
        sockAddr.sin_family = PF_INET;
        sockAddr.sin_port= htons(port);
        sockAddr.sin_addr.s_addr = INADDR_ANY;
    }
    if(socketFd == -1){
        printf("Error: socket()\n");
        return 1;
    }
    if(bind(socketFd, (struct sockaddr *)  &sockAddr, sizeof(struct sockaddr_in))){
        printf("Error: bind()\n");
        return 1;
    }

    if(listen(socketFd, LISTEN_LEN) < 0){
        perror("Error: listen\n");
        return 1;
    }

    struct sockaddr addr;
    socklen_t socklen = sizeof(struct sockaddr);
    int newSocket = accept(socketFd, &addr, &socklen);
    if(newSocket < 0){
        perror("accept: ");
    }
    tcpEcho(newSocket);
    close(socketFd);

}


void tcpEcho(int socket){
    char buf[BUFFER_SIZE];
    int rContFlag = 1;
    int wContFlag = 1;
    void *wStart;

    while(rContFlag){
        int readSize = read(socket, buf, (int) BUFFER_SIZE);
        if(readSize == -1){
            perror("Error: Failed to read.\n");
            close(socket);
            exit(1);
        }else if(readSize == 0 || buf[readSize-1] == EOF){
            printf("\nOK ");
            rContFlag = 0;
        }

        wStart = buf;
        int writeSize = 0;
        wContFlag = 1;
        while(readSize){
            printf("%s", buf);
            writeSize = write(socket, wStart,  readSize);
            readSize -= writeSize;
            if(writeSize == -1){
                perror("Error: Failed to write.\n");
                close(socket);
                exit(1);
            }
        }
    }
    close(socket);
}