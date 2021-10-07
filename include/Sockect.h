//
// Created by user on 2021/10/5.
//

#ifndef FILECOPYSERVER_SOCKECT_H
#define FILECOPYSERVER_SOCKECT_H

#include<sys/socket.h>

typedef void (*PayLoadDataCB)(int fd, unsigned char *data, unsigned int len);

int StartSockectServer(int port);
void StopSockectServer();

int StartSockectClient(int port);
void StopSockectClient();

void registerPayLoadDataCB(PayLoadDataCB cb);
void SocketSendData(int sockect_fd, unsigned char *data, unsigned int len);
void ProcessPayloadData(int fd, unsigned char *data, unsigned int len);

#endif //FILECOPYSERVER_SOCKECT_H
