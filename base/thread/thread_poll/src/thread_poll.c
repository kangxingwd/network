#include <stdio.h>
#include <stdlib.h>
#include "thread_pool.h"

void* threadpool_thread(void *pool);

thread_pool_t* thread_pool_create(int tsize, int qsize, int flags)
{
    thread_pool_t* pool = NULL;
    if (tsize <= 0 || tsize > THREAD_SIZE_MAX || qsize <= 0 || qsize > THREAD_QUEUE_SIZE_MAX) {
        return pool;
    }

    pool = (thread_pool_t*)malloc(sizeof(thread_pool_t));
    if (pool == NULL) {
        printf("malloc threadpool failed!\n");
        return NULL;
    }

    pool->tsize = tsize;
    pool->threads = (pthread_t *)malloc(sizeof(pthread_t) * tsize);
    if (pool->threads == NULL) {
        printf("malloc threads failed!\n");
        free(pool);
        return NULL;
    }

    pool->t_queues = malloc(sizeof(thread_queue_t) * tsize);
    if (pool->t_queues == NULL) {
        printf("malloc threadpool queue failed!\n");
        free(pool->threads);
        free(pool);
        return NULL;
    }

    for(int i = 0; i < tsize; i++) {
        if (thread_queue_init(&(pool->t_queues[i]), pool->qsize) != 0) {
            thread_pool_destroy(pool);
            printf("thread_queue_init failed!\n");
            return NULL;
        }
    }

    for (int i = 0; i < tsize; i++) {
        if (pthread_create(&(pool->threads[i]), NULL, threadpool_thread, (void*)pool) != 0) {
            printf("pthread_creat failed!\n");
            thread_pool_destroy(pool);
            return NULL;
        }
    }
    pool->pick_thread_num = 0;
    pool->shutdown = 0;
    return pool;
}

int thread_pool_destroy(thread_pool_t* pool)
{
    if (pool) {
        pool->shutdown = 1;

        if (pool->t_queues) {
            for (int i = 0; i < pool->tsize; i++) {
                thread_queue_quit(&(pool->t_queues[i]));
            }
        }

        if (pool->threads) {
            for(int i = 0; i < pool->tsize; i++) {
                pthread_join(pool->threads[i], NULL);
            }
            free(pool->threads);
        }

        if (pool->t_queues) {
            for (int i = 0; i < pool->tsize; i++) {
                thread_queue_destroy(&(pool->t_queues[i]));
            }
            free(pool->t_queues);
        }
        free(pool);
    }
}

int thread_pool_add(thread_pool_t* pool, void* (*func)(void*), void* args)
{
    int err = 0;

    if (!pool || !func) {
        err = THREADPOOL_ERR_INVALID;
        goto ret;
    }

    do {
        if (pool->shutdown) {
            err = THREADPOOL_ERR_SHUTDOWN;
            break;
        }

        thread_task_t* task = (thread_task_t*)malloc(sizeof(thread_task_t));
        task->func = func;
        task->args = args;

        int thread_index = (pool->pick_thread_num++) % pool->tsize;
        if (thread_queue_push(&(pool->t_queues[thread_index]), task) != 0) {
            printf("push task failed!\n");
            free(task);
        }
    } while(0);

ret:
    return err;
}

void* threadpool_thread(void *pool)
{
    thread_pool_t* threadpool = (thread_pool_t*)pool;

    int queue_index = 0;
    pthread_t tid = pthread_self();
    for (int i = 0; i < threadpool->tsize; i++) {
        if (threadpool->threads[i] == tid) {
            queue_index = i;
            break;
        }
    }

    thread_queue_t* t_queue = &(threadpool->t_queues[queue_index]);

    for(;;) {
        thread_task_t* task = thread_queue_pop(t_queue, 50);
        if (task) {
            task->func(task->args);
            free(task);
            task = NULL;
        }

        if (threadpool->shutdown && thread_queue_isempty(t_queue)) {
            break;
        }
    }
    return NULL;
}