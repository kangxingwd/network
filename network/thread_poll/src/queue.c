#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

#define DEFULT_QUEUE_SIZE 65535

queue_t* queue_creat(int size)
{
    queue_t* q = (queue_t*)malloc(sizeof(queue_t));
    if (q != NULL) {
        if (size > 0) {
            q->capcity = size;
            q->list = (struct user_data*)malloc(sizeof(void*) * size);
        } else {
            q->capcity = DEFULT_QUEUE_SIZE;
            q->list = (struct user_data*)malloc(sizeof(void*) * DEFULT_QUEUE_SIZE);
        }
        
        if (q->list == NULL) {
            free(q);
            return NULL;
        }
        q->head = q->tail = q->size = 0;
    }
    return q;
}

void queue_destroy(queue_t* q)
{
    if (q != NULL) {
        if (q->list != NULL) {
            free(q->list);
        }
        free(q);
    }
}

// return:  1: full, 0:not full
int queue_is_full(queue_t* q)
{
    return q->size == q->capcity;
}

// return   1: empty, 0: not empty
int queue_is_empty(queue_t* q)
{
    return q->size == 0;
}

int queue_push(queue_t* q, void* data)
{
    if (!queue_is_full(q)) {
        q->list[q->tail].data = data;
        q->tail = (q->tail + 1) % q->capcity;
        q->size++;
    }
    return 0;
}

void* queue_pop(queue_t* q)
{
    void* data = NULL;
    if (!queue_is_empty(q)) {
        data = q->list[q->head].data;
        q->head = (q->head + 1) % q->capcity;
        q->size--;
        return data;
    }
    return data;
}