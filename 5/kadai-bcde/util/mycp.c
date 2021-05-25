#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/statvfs.h>
#include "mycp.h"



int mycpEx(int option, char *s, char *t, int fd, int isAppend){
//option source, target
//NORMAL_COPY: file to file
//FD TO FILE : file descripter to file

    struct statvfs bufS = {0};
    ssize_t readSize, writeSize;
    char *source = NULL;
    char *target =  NULL;
    int fdS;
    int fdT;

    void *buf = malloc(BUF_SIZE);

    if(option == NORMAL_COPY){
        source = s;
        target = t;
        fdS =  open(source, O_RDONLY );
        if(isAppend)
          fdT =  open(target, O_CREAT | O_RDWR | O_APPEND, 0666);
        else 
          fdT =  open(target, O_CREAT | O_RDWR | O_TRUNC, 0666);
    }else if (option == COPY_FD_TO_FILE){
        target = t;
        if(isAppend)
          fdT =  open(target, O_CREAT | O_RDWR | O_APPEND, 0666);
        else 
          fdT =  open(target, O_CREAT | O_RDWR | O_TRUNC, 0666);
        
        fdS = fd;
    }else {
        source = s;
        fdS =  open(source, O_RDONLY );
        fdT = fd;
    }


    if(option != COPY_FILE_TO_FD){
        int rc = statvfs(target, &bufS);
        if(rc < 0){
            perror("Error: Failed to execute statevfs.\n");
            exit(1);
        }

        struct stat statBuf;

        if (stat(t, &statBuf) != 0){
            perror("Error: Failed to get file state.\n");
            exit(1);
        }
        if((unsigned long) statBuf.st_size > (unsigned long)bufS.f_bavail * bufS.f_bsize){ 
            printf("Error: Too big for available disk size.\n");
            exit(1);
        } ;
    }


        
    // printf("%lu\n" ,(unsigned long)bufS.f_bavail * bufS.f_bsize);

    if(fdT == -1 || fdS == -1){
        perror("Error: Cannot open file.\n");
        exit(1);
    }
    int rContFlag = 1;
    int wContFlag = 1;
    void *wStart;

    while(rContFlag){
        readSize = read(fdS, buf, (int) BUF_SIZE);
        if(readSize < BUF_SIZE ||  readSize == 0 ){
            //終了
            rContFlag = 0;
        }else if(readSize == -1){
            perror("Error: Failed to read.\n");
            exit(1);
        }
        wStart = buf;
        writeSize = 0;
        wContFlag = 1;
        while(wContFlag){
            int writeSizeNow = readSize - writeSize;
            writeSize = write(fdT, wStart,  writeSizeNow);
            if(writeSizeNow == writeSize){
                wContFlag = 0;
            }else if(readSize == -1){
                perror("Error: Failed to write.\n");
                exit(1);
            }
        }
        

    }
    
    int endS = close(fdS);
    int endT = close(fdT);
    if(endT == -1 || endS == -1){
        perror("Error: Failed to close file.\n");
        exit(1);
    }

    return 0;

}