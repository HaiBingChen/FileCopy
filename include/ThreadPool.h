//
// Created by user on 2021/10/5.
//

#ifndef FILECOPYSERVER_THREADPOOL_H
#define FILECOPYSERVER_THREADPOOL_H

#include <pthread.h>
#include "Queue.h"
#include "unistd.h"

typedef void *(*TaskWorkCB)(void *);


typedef struct ThreadPoolTask {
    TaskWorkCB cb;
    void *arg;
} ThreadPoolTask;

typedef struct ThreadPoolManager {
    int shut_down;
    int max_thr_num;
    Queue queue;
    pthread_t *thr_id;
    pthread_mutex_t queue_lock;
    pthread_cond_t queue_ready;
} ThreadPoolManager;

void ThreadPoolInit(unsigned int pool_num);

void ThreadPoolDeInit();

void ThreadPoolAddTask(TaskWorkCB cb, void *arg);

#endif //FILECOPYSERVER_THREADPOOL_H
