#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include "thread_queue.h"

int thread_queue_init(thread_queue_t *t_queue, int qsize)
{
    if (!t_queue) {
        return -1;
    }

    t_queue->queue = queue_creat(qsize);
    if (t_queue->queue == NULL) {
        return -1;
    }

    pthread_mutex_init(&t_queue->mutex, NULL);
    pthread_cond_init(&t_queue->cond, NULL);

    t_queue->quit = 0;
    return 0;
}

void thread_queue_quit(thread_queue_t *t_queue)
{
    pthread_mutex_lock(&(t_queue->mutex));
    t_queue->quit = 1;
    pthread_cond_broadcast(&(t_queue->cond));  // 激活所有等待的线程
    pthread_mutex_unlock(&(t_queue->mutex));
}

void thread_queue_destroy(thread_queue_t *t_queue)
{
    if (!t_queue) {
        return;
    }

    if (t_queue->quit == 0) {
        thread_queue_quit(t_queue);
    }

    queue_destroy(t_queue->queue);
    pthread_cond_destroy(&(t_queue->cond));
    pthread_mutex_destroy(&(t_queue->mutex));
}

thread_task_t* thread_queue_pop(thread_queue_t *t_queue, int timeout)
{
    if (!t_queue) {
        return NULL;
    }

    thread_task_t* task = NULL;
    struct timeval now;
    struct timespec outtime;

    pthread_mutex_lock(&t_queue->mutex);
    while(queue_is_empty(t_queue->queue) && (t_queue->quit == 0)) {
        gettimeofday(&now, NULL);
        outtime.tv_sec = now.tv_sec + (now.tv_usec + timeout > 1000)?1:0;
        outtime.tv_nsec = ((now.tv_usec + timeout) % 1000) * 1000;
        pthread_cond_timedwait(&(t_queue->cond), &(t_queue->mutex), &outtime);
    }

    task = queue_pop(t_queue->queue);
    pthread_mutex_unlock(&t_queue->mutex);
    return task;
}

int thread_queue_push(thread_queue_t *t_queue, thread_task_t* task)
{
    if (!t_queue) {
        return -1;
    }

    if (queue_is_full(t_queue->queue)) {
        return -1;
    }

    pthread_mutex_lock(&t_queue->mutex);
    queue_push(t_queue->queue, (void*)task);
    pthread_cond_signal(&(t_queue->cond));    // 唤醒一个等待的线程
    pthread_mutex_unlock(&t_queue->mutex);
    return 0;
}

int thread_queue_isempty(thread_queue_t *t_queue)
{
    return queue_is_empty(t_queue->queue);
}
