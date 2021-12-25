
#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_

#include <pthread.h>
#include "thread_queue.h"
#include "systime.h"

#define THREAD_SIZE_MAX 128
#define THREAD_QUEUE_SIZE_MAX 65535

enum {
    THREADPOOL_ERR_INVALID      =   -1,
    THREADPOOL_ERR_SHUTDOWN     =   -2,
    THREADPOOL_ERR_MEMORY       =   -3
} threadpool_err_t;

typedef struct thread_pool {
    unsigned int tsize;
    pthread_t *threads;

    int qsize;
    thread_queue_t *t_queues;

    unsigned int pick_thread_num;  // 线程选择次数， 用来确定往哪个线程的队列push数据

    int shutdown;
} thread_pool_t;


#ifdef __cplusplus
extern "C" {
#endif

thread_pool_t* thread_pool_create(int tsize, int qsize, int flags);
int thread_pool_destroy(thread_pool_t* pool);
int thread_pool_add(thread_pool_t* pool, void* (*func)(void*), void* args);

#ifdef __cplusplus
}
#endif

#endif // _THREAD_POOL_H_
