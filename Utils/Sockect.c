//
// Created by user on 2021/10/5.
// https://blog.csdn.net/qq_43296898/article/details/88770402
//

#include "Sockect.h"
#include "ThreadPool.h"
#include "List.h"
#include <netinet/in.h>
#include <arpa/inet.h>
#include "errno.h"
#include "memory.h"
#include "ProtocolAnalysis.h"

const unsigned int max_sockect_size = 10;

//==============================================================================================//
//Server
int sockect_server_fd = -1;
struct sockaddr_in sockect_server;
int opt = SO_REUSEADDR;
pthread_t sockect_server_tid;
int sockect_server_shutdown = 1;
List sockect_server_list;
PayLoadDataCB sockect_payload_cb = NULL;

//保存sockect数据队列信息，用于SockectClientDataRevThread和SockectClientDataHandleThread数据同步
typedef struct SockectDataHandleInfo {
    int shutdown;
    int client_fd;
    Queue queue;
    pthread_mutex_t queue_lock;
    pthread_cond_t queue_ready;
} SockectDataHandleInfo;

void *SockectClientDataRevThread(void *arg) {

    if (!arg) {
        printf("Invalid input arguments in %s()\n", __FUNCTION__);
        return NULL;
    }

    int *client_fd = (int *) arg;
    printf("%s start, client(%d)...\n", __FUNCTION__ , *client_fd);

    void *data = ListPop(&sockect_server_list, *client_fd);
    if(data == NULL) {
        printf("%s end\n", __FUNCTION__ );
        return NULL;
    }
    SockectDataHandleInfo *info = (SockectDataHandleInfo *)data;

    unsigned char *rev_buffer = (unsigned char *) malloc(kMaxPackectSize);

    while (1) {
        memset(rev_buffer, 0, kMaxPackectSize);
        int result = read(*client_fd, rev_buffer, kMaxPackectSize);
        if (result < 0) {
            printf("Read data from client sockfd[%d] failure: %s and thread will exit\n",
                   *client_fd, strerror(errno));
            close(*client_fd);
            break;
        } else if (result == 0) {
            printf("Socket[%d] get disconnected and thread will exit.\n", *client_fd);
            close(*client_fd);
            break;
        } else if (result > 0) {
            printf("client(%d), Read %d bytes data\n", *client_fd, result);

            pthread_mutex_lock(&info->queue_lock);
            QueuePush(&info->queue, rev_buffer, result);
            pthread_mutex_unlock(&info->queue_lock);

            pthread_cond_signal(&info->queue_ready);
        }
    }

    free(rev_buffer);

    //通知SockectClientDataHandleThread退出
    info->shutdown = 1;
    pthread_cond_signal(&info->queue_ready);

    printf("%s end\n", __FUNCTION__ );
    return NULL;
}

void *SockectClientDataHandleThread(void *arg) {
    if (!arg) {
        printf("Invalid input arguments in %s()\n", __FUNCTION__);
        return NULL;
    }

    int *client_fd = (int *) arg;
    printf("%s start, client(%d)...\n", __FUNCTION__ , *client_fd);

    void *data = ListPop(&sockect_server_list, *client_fd);
    if(data == NULL) {
        printf("%s end\n", __FUNCTION__ );
        return NULL;
    }
    SockectDataHandleInfo *info = (SockectDataHandleInfo *)data;

    while(1) {
        pthread_mutex_lock(&info->queue_lock);
        while (!info->shutdown && IsQueueEmpty(&info->queue)) {
            /*先释放锁，进入等待，等待事件达到后继续往下又获取锁*/
            pthread_cond_wait(&info->queue_ready, &info->queue_lock);
        }

        if (info->shutdown) {
            pthread_mutex_unlock(&info->queue_lock);
            break;
        }

        QueueNode *node = QueuePop(&info->queue);
        pthread_mutex_unlock(&info->queue_lock);

        if((node != NULL) && (node->data != NULL)){

            ProcessRecvData(*client_fd, node->data, node->size);

            //we must free it after the node is used!!!
            free(node->data);
            free(node);
            node = NULL;
        }
    }

    QueueDeInit(&info->queue);
    pthread_mutex_destroy(&info->queue_lock);
    pthread_cond_destroy(&info->queue_ready);
    free(info);
    ListDelete(&sockect_server_list, *client_fd);
    free(client_fd);

    printf("%s end\n", __FUNCTION__ );
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

        //save sockect queue data
        SockectDataHandleInfo *info = malloc(sizeof(SockectDataHandleInfo));

        info->client_fd = *sockect_client_fd;
        info->shutdown = 0;
        QueueInit(&info->queue);

        if (pthread_mutex_init(&info->queue_lock, NULL) != 0) {
            printf("%s: pthread_mutex_init failed\n", __FUNCTION__);
            free(info);
            return NULL;
        }

        if (pthread_cond_init(&info->queue_ready, NULL) != 0) {
            printf("%s: pthread_cond_init failed\n", __FUNCTION__);
            free(info);
            return NULL;
        }

        //we will find the list node by sockect_client_fd
        ListPush(&sockect_server_list, info, sizeof(SockectDataHandleInfo), *sockect_client_fd);

        ThreadPoolAddTask(SockectClientDataRevThread, (void *)sockect_client_fd);
        ThreadPoolAddTask(SockectClientDataHandleThread, (void *)sockect_client_fd);
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
    ThreadPoolInit(max_sockect_size*2);

    //list init
    ListInit(&sockect_server_list);

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

//==============================================================================================//
//client
int sockect_client_fd = -1;
unsigned char rev_buffer[kMaxPackectSize] = {0};
struct sockaddr_in sockect_client;
pthread_t sockect_client_tid;

void *SockectClientThread(void *arg) {
    printf("%s\n", __FUNCTION__);

    int client_fd = *(int *) arg;

    while(1) {
        memset(rev_buffer, 0, kMaxPackectSize);

        int result = read(client_fd, rev_buffer, kMaxPackectSize);
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
            printf("client(%d), Read %d bytes data\n", client_fd, result);
            ProcessRecvData(client_fd, rev_buffer, result);
        }
    }

    close(client_fd);
    return NULL;
}

int StartSockectClient(int port)
{
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

    pthread_create(&sockect_client_tid, NULL, SockectClientThread, (void *) &sockect_client_fd);

    return sockect_client_fd;
}

void StopSockectClient()
{
    printf("%s\n", __FUNCTION__);
    pthread_join(sockect_client_tid, NULL);
}

//==============================================================================================//
void registerPayLoadDataCB(PayLoadDataCB cb)
{
    sockect_payload_cb = cb;
}

void SocketSendData(int sockect_fd, unsigned char *data, unsigned int len)
{
    unsigned char *packet = NULL;
    unsigned int packet_len = 0;
    CreatePacket(data, len, &packet, &packet_len);

    if(packet != NULL) {
        send(sockect_fd, packet, packet_len, 0);
        //must free the packet when it was used
        free(packet);
    }
}

void ProcessPayloadData(int fd, unsigned char *data, unsigned int len)
{
    if(sockect_payload_cb != NULL) {
        sockect_payload_cb(fd, data, len);
    }
}
