#ifndef MYCP_H_INCLUDED
#define MYCP_H_INCLUDED

#define BUF_SIZE 1024
#define NORMAL_COPY 0
#define COPY_FD_TO_FILE 1
#define COPY_FILE_TO_FD 2

int mycpEx(int opt, char * s, char * t, int fd, int isAppend);

#endif