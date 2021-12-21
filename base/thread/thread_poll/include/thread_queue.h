
#ifndef __THREAD_QUEUE_H__
#define __THREAD_QUEUE_H__

#include "queue.h"

typedef struct thread_task {
    void* (*func)(void *);
    void* args;
} thread_task_t;

typedef struct thread_queue {
    queue_t* queue;

    pthread_mutex_t mutex;
    pthread_cond_t cond;

    int quit;
} thread_queue_t;


int thread_queue_init(thread_queue_t *t_queue, int qsize);
void thread_queue_destroy(thread_queue_t *t_queue);

int thread_queue_push(thread_queue_t *t_queue, thread_task_t* task);
thread_task_t* thread_queue_pop(thread_queue_t *t_queue, int timeout);

int thread_queue_isempty(thread_queue_t *t_queue);
void thread_queue_quit(thread_queue_t *t_queue);

#endif //__THREAD_QUEUE_H__
