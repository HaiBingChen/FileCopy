//
// Created by user on 2021/10/2.
//

#ifndef FILECOPYSERVER_QUEUE_H
#define FILECOPYSERVER_QUEUE_H

#include "stdio.h"
#include "stdbool.h"
#include "stdlib.h"

typedef enum DataType {
    DataTypeChar = 0,
    DataTypeUChar,
    DataTypeShort,
    DataTypeUShort,
    DataTypeInt,
    DataTypeUInt,
    DataTypeLong,
    DataTypeULong,
    DataTypeFloat,
    DataTypeDouble,
    DataTypeStruct,
} DataType;

typedef void (*QueueDataCB)(void * data, size_t size, DataType type);

typedef struct QueueNode {
    void *data;
    unsigned int size;
    DataType type;
    struct QueueNode *next;
} QueueNode;

typedef struct Queue {
    QueueNode *rear;
    QueueNode *front;
    unsigned int size;
} Queue;

void QueueInit(Queue *queue);
void QueueDeInit(Queue *queue);
void QueuePush(Queue *queue, void *data, size_t size, DataType type);
void QueuePop(Queue *queue, QueueDataCB cb);
bool IsQueueEmpty(Queue *queue);

#endif //FILECOPYSERVER_QUEUE_H
