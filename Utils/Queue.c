//
// Created by user on 2021/10/2.
// https://www.cnblogs.com/zhuzbyin/p/ghst_zby1.html
// https://blog.csdn.net/peng_apple/article/details/79726963
//

#include "Queue.h"
#include "assert.h"
#include "memory.h"

const unsigned int cMaxQueueSize = 30;

void QueueInit(FileQueue *queue)
{
    assert(queue != NULL);

    queue->front = NULL;
    queue->rear = NULL;
    queue->size = 0;
}

void QueueDeInit(FileQueue *queue)
{
    assert(queue != NULL);
    FileNode *tmp = NULL;
    while(queue->front) {
        tmp = queue->front;
        queue->front = queue->front->next;
        free(tmp);
    }
}

void QueuePush(FileQueue *queue, FileData *data)
{
    assert(queue != NULL);

    if(queue->size > cMaxQueueSize) {
        printf("[ERROR]QueuePush: queue is overflow\n");
        return;
    }

    FileNode *node =  (FileNode *)malloc(sizeof(FileNode));
    memcpy(&node->data, data, sizeof(FileData));

    if(IsEmpty(queue)) {
        queue->front = node;
        queue->rear = node;
    } else {
        queue->rear->next = node;
        queue->rear = node;
    }

    ++queue->size;
}

struct FileData QueuePop(FileQueue *queue)
{
    assert(queue != NULL);

    FileData data;

    if(IsEmpty(queue)) {
        printf("[ERROR]QueuePop: queue is empty\n");
        data.data = -1;
        return data;
    }

    struct FileNode *tmp = queue->front;

    data = queue->front->data;
    queue->front = queue->front->next;
    free(tmp);
    --queue->size;

    return data;
}

bool IsEmpty(FileQueue *queue)
{
    assert(queue != NULL);

    return (queue->size == 0);
}
