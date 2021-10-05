//
// Created by user on 2021/10/5.
//

#ifndef FILECOPYSERVER_SOCKECT_H
#define FILECOPYSERVER_SOCKECT_H

#include<sys/socket.h>

int StartSockectServer(int port);

void StopSockectServer();

int StartSockectClient(int port);

void StopSocketClient();

#endif //FILECOPYSERVER_SOCKECT_H
