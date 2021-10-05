//
// Created by user on 2021/10/5.
// https://blog.csdn.net/qq_43296898/article/details/88770402
//

#include "Sockect.h"
#include "ThreadPool.h"
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "pthread.h"
#include "errno.h"

const unsigned int max_sockect_size = 10;
const unsigned long max_buffer_size = 65535;
//=====================================================================//
//Server
int sockect_server_fd = -1;
struct sockaddr_in sockect_server;
int opt = SO_REUSEADDR;
pthread_t sockect_server_tid;
int sockect_server_shutdown = 1;

void *SockectClientThread(void *arg) {
    if (!arg) {
        printf("Invalid input arguments in %s()\n", __FUNCTION__);
        pthread_exit(NULL);
    }

    int client_fd = *((int *) arg);
    free(arg);

    printf("Child client %d thread start to commuicate with socket client...\n", client_fd);

    unsigned char *rev_buffer = (unsigned char *) malloc(max_buffer_size);

    while (1) {
        memset(rev_buffer, 0, max_buffer_size);
        int result = read(client_fd, rev_buffer, max_buffer_size);
        if (result < 0) {
            printf("Read data from client sockfd[%d] failure: %s and thread will exit\n",
                   client_fd, strerror(errno));
            close(client_fd);
            break;
        } else if (result == 0) {
            printf("Socket[%d] get disconnected and thread will exit.\n", client_fd);
            close(client_fd);
            break;
        } else if (result > 0) {
            printf("Read %d bytes data\n", result);
        }
    }

    free(rev_buffer);

    return NULL;
}

void *SockectServerThread(void *arg) {
    printf("Start accept new client incoming...\n");

    int server_fd = *(int *) arg;
    struct sockaddr_in sockect_client;
    socklen_t len;

    while (!sockect_server_shutdown) {
        int *sockect_client_fd = malloc(sizeof(int));

        *sockect_client_fd = accept(server_fd, (struct sockaddr *) &sockect_client, &len);
        if (*sockect_client_fd < 0) {
            printf("[ERROR] Accept new client failure: %s\n", strerror(errno));
            free(sockect_client_fd);
            sockect_client_fd = NULL;
            continue;
        }

        printf("Accept new client[%s:%d] successfully\n",
               inet_ntoa(sockect_client.sin_addr), ntohs(sockect_client.sin_port));

        ThreadPoolAddTask(SockectClientThread, (void *) sockect_client_fd);
    }

    close(server_fd);

    return NULL;
}

int StartSockectServer(int port) {
    printf("%s: port %d\n", __FUNCTION__, port);

    if ((sockect_server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        printf("[ERROR] %s: Creating socket failed\n", __FUNCTION__);
    }

    setsockopt(sockect_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    memset(&sockect_server, 0, sizeof(sockect_server));
    sockect_server.sin_family = AF_INET;
    sockect_server.sin_port = htons(port);
    sockect_server.sin_addr.s_addr = htonl(INADDR_ANY); /* 监听本机所有IP*/
    //inet_aton("192.168.0.16", &sockect_server_addr.sin_addr); /* 监听指定ip */

    if (bind(sockect_server_fd, (struct sockaddr *) &sockect_server, sizeof(struct sockaddr)) == -1) {
        printf("[ERROR] %s: bind socket failed\n", __FUNCTION__);
        return -1;
    }

    if (listen(sockect_server_fd, max_sockect_size) == -1) {
        perror("listen() error\n");
        return -2;
    }

    sockect_server_shutdown = 0;

    //start thread pool
    ThreadPoolInit(max_sockect_size);

    pthread_create(&sockect_server_tid, NULL, SockectServerThread, (void *) &sockect_server_fd);

    return sockect_server_fd;
}

void StopSockectServer() {

    printf("%s\n", __FUNCTION__);

    if (!sockect_server_shutdown) {
        return;
    }

    sockect_server_shutdown = 1;
    pthread_join(sockect_server_tid, NULL);

    ThreadPoolDeInit();
}

//=====================================================================//
//Client
int sockect_client_fd = -1;
struct sockaddr_in sockect_client;
pthread_t sockect_client_tid;

void *SockectClentThread(void *arg) {
    int client_fd = *(int *) arg;

    while(1) {

    }

    return NULL;
}

int StartSockectClient(int port) {
    printf("%s\n", __FUNCTION__);
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

    pthread_create(&sockect_client_tid, NULL, SockectClentThread, (void *) &sockect_client_fd);

    return sockect_client_fd;
}

void StopSocketClient() {
    printf("%s\n", __FUNCTION__);
}