//
// Created by user on 2021/10/2.
//
#include "stdio.h"
#include "Queue.h"

int main(int argc, const char** argv)
{
    printf("File Copy Server start\n");
    struct FileQueue queue;
    QueueInit(&queue);

    struct FileData data;
    for(unsigned int i=0; i<50; i++) {
        data.data = i;
        QueuePush(&queue, &data);
    }

    for(unsigned int i=0; i<50; i++) {
        data = QueuePop(&queue);
        printf("%d, ", data.data);
    }

    return 0;
}