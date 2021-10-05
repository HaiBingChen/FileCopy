//
// Created by user on 2021/10/5.
//

#include <unistd.h>
#include "stdio.h"
#include "Sockect.h"
#include "string.h"

int main(int argc, const char **argv) {
    printf("File Copy Client start\n");

    int sockect_fd = StartSockectClient(1234);

    const char *sendbuf = argv[1];
    send(sockect_fd, sendbuf, strlen(sendbuf), 0);

    while (1) {
        pause();
    }
}