#ifndef _QUEUE_H_
#define _QUEUE_H_

struct user_data {
    void *data;
};

typedef struct queue {
    int head;
    int tail;
    int size;
    int capcity;
    struct user_data* list;
} queue_t;


// API

queue_t* queue_creat(int size);
void queue_destroy(queue_t* q);

// return:  1: full, 0:not full
int queue_is_full(queue_t* q);

// return   1: empty, 0: not empty
int queue_is_empty(queue_t* q);

int queue_push(queue_t* q, void* data);

void* queue_pop(queue_t* q);

#endif // _QUEUE_H_