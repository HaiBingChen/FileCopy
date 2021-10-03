//
// Created by user on 2021/10/2.
//

#ifndef FILECOPYSERVER_QUEUE_H
#define FILECOPYSERVER_QUEUE_H

#include "stdio.h"
#include "stdbool.h"
#include "stdlib.h"

typedef struct FileData {
    unsigned char data;
} FileData;

typedef struct FileNode {
    FileData data;
    struct FileNode *next;
} FileNode;

typedef struct FileQueue {
    FileNode *rear;
    FileNode *front;
    unsigned int size;
} FileQueue;

void QueueInit(FileQueue *queue);
void QueueDeInit(FileQueue *queue);
void QueuePush(FileQueue *queue, FileData *data);
struct FileData QueuePop(FileQueue *queue);
bool IsEmpty(FileQueue *node);

#endif //FILECOPYSERVER_QUEUE_H
