//
// Created by user on 2021/10/2.
//
#include <unistd.h>
#include "stdio.h"
#include "Sockect.h"

//这个函数会在多线程中同时调用，如果有全局共享资源的情况需要加锁保护
void PayloadDataCallBack(int fd, unsigned char *data, unsigned int len)
{
    printf("[PayloadDataCallBack] fd:%d\n", fd);
    for(int i=0; i<len; i++){
        printf("0x%x,", data[i]);
    }
    printf("\n");
    SocketSendData(fd, data, len);
}

int main(int argc, const char **argv) {
    printf("File Copy Server start\n");

    registerPayLoadDataCB(PayloadDataCallBack);

    int socket_fd = StartSockectServer(1234);
    (void)socket_fd;

    while (1) {
        pause();
    }
}