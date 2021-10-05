//
// Created by user on 2021/10/5.
// https://www.cnblogs.com/newth/archive/2012/05/09/2492459.html
//
#include "ThreadPool.h"
#include "memory.h"

ThreadPoolManager g_pool_manager;

static void *thread_routine(void *arg) {
    //printf("0x%x thread start\n", gettid());

    while (1) {
        pthread_mutex_lock(&g_pool_manager.queue_lock);
        while (!g_pool_manager.shut_down && IsQueueEmpty(&g_pool_manager.queue)) {
            /*先释放锁，进入等待，等待事件达到后继续往下又获取锁*/
            pthread_cond_wait(&g_pool_manager.queue_ready, &g_pool_manager.queue_lock);
        }

        if (g_pool_manager.shut_down) {
            pthread_mutex_unlock(&g_pool_manager.queue_lock);
            pthread_exit(NULL);
        }

        QueueNode *node = QueuePop(&g_pool_manager.queue);
        pthread_mutex_unlock(&g_pool_manager.queue_lock);

        if((node != NULL) && (node->data != NULL)){
            ThreadPoolTask *task = (ThreadPoolTask *)node->data;
            task->cb(task->arg);

            //we must free it after the node is used!!!
            free(node->data);
            free(node);
            node = NULL;
        }

    }
    return NULL;
}

void ThreadPoolInit(unsigned int pool_num) {
    printf("%s: pool_num %d\n", __FUNCTION__, pool_num);
    /* 初始化 */
    g_pool_manager.max_thr_num = pool_num;
    g_pool_manager.shut_down = false;
    QueueInit(&g_pool_manager.queue);
    if (pthread_mutex_init(&g_pool_manager.queue_lock, NULL) != 0) {
        printf("%s: pthread_mutex_init failed\n", __FUNCTION__);
        exit(1);
    }

    if (pthread_cond_init(&g_pool_manager.queue_ready, NULL) != 0) {
        printf("%s: pthread_cond_init failed\n", __FUNCTION__);
        exit(1);
    }

    g_pool_manager.thr_id = malloc(pool_num * sizeof(pthread_t));

    if (!g_pool_manager.thr_id) {
        printf("%s: malloc failed\n", __FUNCTION__);
        exit(1);
    }

    /* 创建线程池 */
    for (unsigned int i = 0; i < pool_num; ++i) {
        if (pthread_create(&g_pool_manager.thr_id[i], NULL, thread_routine, NULL) != 0) {
            printf("%s: pthread_create failed\n", __FUNCTION__);
            exit(1);
        }
    }
}

void ThreadPoolDeInit() {
    printf("%s\n", __FUNCTION__);

    if (g_pool_manager.shut_down) {
        return;
    }

    g_pool_manager.shut_down = 1;
    /* 通知所有正在等待的线程 */
    pthread_mutex_lock(&g_pool_manager.queue_lock);
    pthread_cond_broadcast(&g_pool_manager.queue_ready);
    pthread_mutex_unlock(&g_pool_manager.queue_lock);
    for (unsigned int i = 0; i < g_pool_manager.max_thr_num; ++i) {
        pthread_join(g_pool_manager.thr_id[i], NULL);
    }
    free(g_pool_manager.thr_id);
    QueueDeInit(&g_pool_manager.queue);
    pthread_mutex_destroy(&g_pool_manager.queue_lock);
    pthread_cond_destroy(&g_pool_manager.queue_ready);
}

void ThreadPoolAddTask(TaskWorkCB cb, void *arg) {
    ThreadPoolTask task;
    task.cb = cb;
    task.arg = arg;

    pthread_mutex_lock(&g_pool_manager.queue_lock);
    QueuePush(&g_pool_manager.queue, &task, sizeof(ThreadPoolTask));
    pthread_mutex_unlock(&g_pool_manager.queue_lock);

    /* 通知工作者线程，有新任务添加 */
    pthread_cond_signal(&g_pool_manager.queue_ready);
}