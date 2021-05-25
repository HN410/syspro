#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <memory.h>
#include <sys/epoll.h>
#include <pthread.h>
#include "mytcp.h"

void sendTCP(ReceiveData, int);
void *tcpEcho(void *);

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
        // printf("read: ");
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
                // printf("end\n");
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

int getListenFd(u_short port){
    struct sockaddr_in sockAddr;

    int socketFd;        
    socketFd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    sockAddr.sin_family = PF_INET;
    sockAddr.sin_port= htons(port);
    sockAddr.sin_addr.s_addr = INADDR_ANY;

    if(socketFd == -1){
        printf("Error: socket()\n");
        close(socketFd);
        return 1;
    }
    if(bind(socketFd, (struct sockaddr *)  &sockAddr, sizeof(struct sockaddr_in))){
        printf("Error: bind()\n");
        close(socketFd);
        return 1;
    }

    if(listen(socketFd, LISTEN_LEN) < 0){
        perror("Error: listen\n");
        close(socketFd);
        return 1;
    }
    return socketFd;
}

int receiveServer(u_short port, int isPrinting, int socketFdIn, struct sockaddr_in * sockAddrIn){
    int socketFd = getListenFd(port);
    pthread_t pthreadList[THREAD_N]; 

    while(1){
        for(int i = 0; i < THREAD_N; i++){
            if(pthread_create(&pthreadList[i], NULL, tcpEcho, &socketFd)!= 0){
                perror("pthread_create");
                exit(1);
                close(socketFd);
            }
        }
        for(int i = 0; i < THREAD_N; i++){
            if(pthread_join(pthreadList[i], NULL)!= 0){
                printf("error: pthread_join\n");
                close(socketFd);
                exit(1);
            }
        }
    }
    close(socketFd);
}


int receiveServerEpoll(u_short port, int isPrinting, int socketFdIn, struct sockaddr_in * sockAddrIn){
    struct epoll_event ev, events[EVENT_N];
    struct sockaddr addrs[EVENT_N];
    socklen_t socklen = sizeof(struct sockaddr);
    char buf[BUFFER_SIZE];

    int socketFd = getListenFd(port);

    int epollFd = epoll_create1(0);
    if(epollFd < 0){
        perror("epoll_create1: ");
        close(socketFd);
        return 1;
    }

    ev.data.fd = socketFd;
    ev.events = EPOLLIN;
    if(epoll_ctl(epollFd, EPOLL_CTL_ADD, socketFd, &ev) == -1){
        perror("epoll_ctl: ");
        exit(EXIT_FAILURE);
    }
    while(1){
        int nfds = epoll_wait(epollFd, events, EVENT_N, -1);
        if(nfds == -1){
            perror("epoll_wait: ");
            exit(EXIT_FAILURE);
        }

        for(int i = 0; i < nfds; ++i){
            if(events[i].data.fd == socketFd){
                int connectionFd = accept(socketFd, &addrs[i], &socklen);
                if(connectionFd == -1){
                    perror("accept: ");
                    exit(EXIT_FAILURE);
                }
                //setnonblocking(connectionFd);
                ev.events = EPOLLIN | EPOLLET;
                ev.data.fd = connectionFd;
                if(epoll_ctl(epollFd, EPOLL_CTL_ADD, connectionFd,
                  &ev) == -1){
                      perror("epoll_ctl:");
                      exit(EXIT_FAILURE);
                }
            }else if(events[i].events & EPOLLIN){
                int rContFlag = 1;
                int readSize = read(events[i].data.fd, buf, (int) BUFFER_SIZE);
                if(readSize == -1){
                    perror("Error: Failed to read.\n");
                    close(events[i].data.fd);
                    exit(1);
                }else if(readSize == 0 || buf[readSize-1] == EOF){
                    rContFlag = 0;
                    if(readSize > 0) readSize--;
                }

                char * wStart = buf;
                int writeSize = 0;
                while(readSize){
                    // printf("%s", buf);
                    writeSize = write(events[i].data.fd, wStart,  readSize);
                    readSize -= writeSize;
                    wStart += writeSize;
                    if(writeSize == -1){
                        perror("Error: Failed to write.\n");
                        close(events[i].data.fd);
                        exit(1);
                    }
                }
                if(rContFlag == 0){
                  if(epoll_ctl(epollFd, EPOLL_CTL_DEL, events[i].data.fd,
                    &ev) == -1){
                      perror("epoll_ctl:");
                      close(events[i].data.fd);
                      exit(EXIT_FAILURE);
                    }
                  close(events[i].data.fd);
                }
            }

        }
    }
}



void *tcpEcho(void * arg){
    int oldSocket = *(int *) arg;
    struct sockaddr addr;
    socklen_t socklen = sizeof(struct sockaddr);
    int socket = accept(oldSocket, &addr, &socklen);
    if(socket < 0){
        perror("accept: ");
    }
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
            rContFlag = 0;
            if(readSize > 0) readSize--;
        }

        wStart = buf;
        int writeSize = 0;
        wContFlag = 1;
        while(readSize){
            // printf("%s", buf);
            writeSize = write(socket, wStart,  readSize);
            readSize -= writeSize;
            wStart += writeSize;
            if(writeSize == -1){
                perror("Error: Failed to write.\n");
                close(socket);
                exit(1);
            }
        }
    }
    close(socket);
}