#ifndef MYTCP_H_INCLUDED
#define MYTCP_H_INCLUDED

#define DEFAULT_IP "127.0.0.1"
#define DEFAULT_PORT_SERVER 8101
#define DEFAULT_PORT_CLIENT 8102
#define BUFFER_SIZE 100
#define LISTEN_LEN 10
#define THREAD_N 10
#define EVENT_N 10

typedef struct {
    struct sockaddr addrP;
    int receivedN;
    char buffer[BUFFER_SIZE];
}ReceiveData;

int sendClient(u_short, struct in_addr, FILE *);
int receiveServer(u_short , int, int, struct sockaddr_in *);
int receiveServerEpoll(u_short , int, int, struct sockaddr_in *);

#endif