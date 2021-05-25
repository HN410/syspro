#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/epoll.h>
#include "mytcp.h"

// struct timespec {
//   time_t tv_sec; /* Seconds.  */
//   long tv_nsec;  /* Nanoseconds.  */
// };

void sendTCP(ReceiveData, int);
void sendAcc(int);
double getLatency(int);
long getRTT(int, int);

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

int throughputServer(u_short port, int isPrinting, int socketFdIn, struct sockaddr_in * sockAddrIn){
    int socketFd = getListenFd(port);
    while(1){
        sendAcc(socketFd);
    }
    close(socketFd);
}


void sendAcc(int arg){
    int oldSocket = arg;
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
            //perror("Error: Failed to read.\n");
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


void printThroughput(int socketFd){
    long timeSum = 0l;
    long latency = getRTT(socketFd, 1);
    timeSum += latency;

    int itern = 0;
    long throughputTimeSum = 0;
    while(timeSum < 10 * (double) NANO){
        itern ++;
        long t = getRTT(socketFd, THROUGHPUT_SIZE);
        throughputTimeSum += t - latency;
        timeSum += t-latency;
    }
    long RTT = throughputTimeSum;
    double throughput = (double ) ((long) THROUGHPUT_SIZE * 8 * itern) / (double) (RTT / NANO) / (MEGA) ;

    printf("%d %.5f %.5f\n", THROUGHPUT_SIZE * itern, (float)timeSum / NANO, throughput);
}


long getRTT(int socketFd, int size){
    clockid_t clk_id = CLOCK_REALTIME;
    struct timespec t1a, t1b;
    char buf[BUFFER_SIZE];

    for(int i = 0; i < BUFFER_SIZE; i++){
        buf[i] = 'a';
    }

    clock_gettime( clk_id, &t1a);
    while(size){
            // printf("%s", buf);
            int size1;
            if(size < BUFFER_SIZE) size1 = size;
            else size1 = BUFFER_SIZE;
            int writeSize = write(socketFd, buf,  size1);

            size -= writeSize;
            if(writeSize == -1){
                perror("Error: Failed to write.\n");
                close(socketFd);
                exit(1);
            }
            int readSize = read(socketFd, buf, writeSize);
            if(readSize == -1){
                perror("Error: Failed to read.\n");
                close(socketFd);
                exit(1);
            }
    }

    clock_gettime(clk_id, &t1b);
    long res = (long) ((t1b.tv_sec - t1a.tv_sec) * NANO ) + (long )(t1b.tv_nsec - t1a.tv_nsec);
    return res;
    
}

void throughputClient(u_short port, struct in_addr in_addr){
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

    printThroughput(socketFd);
    return;
}