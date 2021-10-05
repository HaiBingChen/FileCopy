//
// Created by user on 2021/10/5.
//

#include <unistd.h>
#include "stdio.h"
#include "Sockect.h"
#include "string.h"
#include <netinet/in.h>

int port = 1234;

int main(int argc, const char **argv) {
    printf("File Copy Client start\n");
    int sockect_client_fd = -1;
    struct sockaddr_in sockect_client;
    if ((sockect_client_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        printf("[ERROR] %s: Creating socket failed\n", __FUNCTION__);
        return -1;
    }

    memset(&sockect_client, 0, sizeof(sockect_client));
    sockect_client.sin_family = AF_INET;
    sockect_client.sin_port = htons(port);
    sockect_client.sin_addr.s_addr = htonl(INADDR_ANY);

    if(connect(sockect_client_fd, (struct sockaddr *)(&sockect_client), sizeof(struct sockaddr)) == -1) {
        printf("[ERROR] %s: connect socket failed\n", __FUNCTION__);
        return -2;
    }

    char sendbuf[1024];
    while(1) {
        fgets(sendbuf, sizeof(sendbuf), stdin);
        send(sockect_client_fd, sendbuf, strlen(sendbuf), 0);
    }

    return 0;
}