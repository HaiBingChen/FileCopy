//
// Created by user on 2021/10/2.
//

#ifndef FILECOPYSERVER_QUEUE_H
#define FILECOPYSERVER_QUEUE_H

#include "stdio.h"
#include "stdbool.h"
#include "stdlib.h"

typedef struct QueueNode {
    void *data;
    unsigned int size;
    struct QueueNode *next;
} QueueNode;

typedef struct Queue {
    QueueNode *rear;
    QueueNode *front;
    unsigned int size;
} Queue;

void QueueInit(Queue *queue);

void QueueDeInit(Queue *queue);

bool IsQueueEmpty(Queue *queue);

void QueuePush(Queue *queue, void *data, unsigned int size);

QueueNode *QueuePop(Queue *queue);

#endif //FILECOPYSERVER_QUEUE_H
