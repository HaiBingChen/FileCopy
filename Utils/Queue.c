//
// Created by user on 2021/10/2.
// https://www.cnblogs.com/zhuzbyin/p/ghst_zby1.html
// https://blog.csdn.net/peng_apple/article/details/79726963
//

#include "Queue.h"
#include "assert.h"
#include "memory.h"

const unsigned int cMaxQueueSize = 30;

void QueueInit(Queue *queue) {
    assert(queue != NULL);

    queue->front = NULL;
    queue->rear = NULL;
    queue->size = 0;
}

void QueueDeInit(Queue *queue) {
    assert(queue != NULL);
    QueueNode *tmp = NULL;
    while (queue->front) {
        tmp = queue->front;
        queue->front = queue->front->next;
        free(tmp->data);
        free(tmp);
    }
}

void QueuePush(Queue *queue, void *data, size_t size) {
    assert(queue != NULL);

    if (queue->size > cMaxQueueSize) {
        printf("[ERROR] %s: queue is overflow\n", __FUNCTION__);
        return;
    }

    QueueNode *node = (QueueNode *) malloc(sizeof(QueueNode));
    node->size = size;
    node->data = malloc(size);
    memcpy(node->data, data, size);

    if (IsQueueEmpty(queue)) {
        queue->front = node;
        queue->rear = node;
    } else {
        queue->rear->next = node;
        queue->rear = node;
    }

    ++queue->size;
}

QueueNode *QueuePop(Queue *queue) {
    assert(queue != NULL);

    if (IsQueueEmpty(queue)) {
        printf("[ERROR] %s: queue is empty\n", __FUNCTION__);
    }

    struct QueueNode *node = queue->front;
    if(node == queue->rear)
    {
        queue->rear = NULL;
        queue->front = NULL;
    }
    else if(node != NULL){
        queue->front = node->next;
    }
    --queue->size;

    return node;
}

bool IsQueueEmpty(Queue *queue) {
    assert(queue != NULL);

    return (queue->size == 0);
}
