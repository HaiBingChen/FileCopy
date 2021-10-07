//
// Created by user on 2021/10/5.
//

#include <unistd.h>
#include "stdio.h"
#include "Sockect.h"

void PayloadDataCallBack(int fd, unsigned char *data, unsigned int len)
{
    printf("[PayloadDataCallBack] fd:%d\n", fd);
    for(int i=0; i<len; i++){
        printf("0x%x,", data[i]);
    }
    printf("\n");
}

int main(int argc, const char **argv) {

    printf("File Copy Client start\n");

    registerPayLoadDataCB(PayloadDataCallBack);

    int socket_fd = StartSockectClient(1234);

    unsigned char buff[3] = {0x01,0x02,0x03};
    SocketSendData(socket_fd, buff, 3);

    while (1) {
        pause();
    }

    return 0;
}