#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "thread_pool.h"

void* run(void *data)
{
    char path[256] = {0};
    int file_index = *((int*)data);

    snprintf(path, 256, "test/%d.txt", file_index);

    FILE* fp = fopen(path, "w+");
    if (!fp) {
        printf("open file failed: %s\n", path);
        return NULL;
    }

    char* w_str = "hellp, i am a test program!\r\n";
    for(int i = 0; i < 100; i++) {
        fwrite(w_str, sizeof(char), strlen(w_str), fp);
    }
    fclose(fp);
    return NULL;
}

#if 1
int main(int argc, char* argv[])
{
    if (argc < 2) {
        printf("Please input task num: \n");
        exit(1);
    }
    int task_count = atoi(argv[1]);

    thread_pool_t* pool = thread_pool_create(1, 100, 0);
    if (!pool) {
        printf("creat thread pool failed!\n");
        exit(1);
    }

    int* files = (int*)malloc(sizeof(int) * task_count);
    for (int i = 0; i < task_count; i++) {
        files[i] = i;
    }

    time_t start = get_current_timestamp();

    for (int i = 0; i < task_count; i++) {
        if (0 != thread_pool_add(pool, run, (void*)&(files[i])) ) {
            printf("thread_pool_add failed! %d\n", i);
        }
    }

    printf("thread_pool_add end!\n");

    thread_pool_destroy(pool);

    time_t end = get_current_timestamp();
    printf("cost time: %ld us\n", end - start);

    printf("---end test\n");
    return 0;
}
#endif


#if 0

int main()
{
    queue_t* q = queue_creat(5);

    int data[6] = {1, 2, 3, 4, 5, 6};
    int i = 0;
    int ret = 0;

    for (i = 0; i < 5; i++) {
        ret = queue_push(q, (void*)&(data[i]));
        printf("push %d ok!\n", i);
    }

    if ( (ret = queue_push(q, (void*)&(data[i]))) != 0 ) {
        printf("push %d failed! ret = %d\n", i, ret);
    }

    if (queue_is_full(q)) {
        printf("queue_is_full \n");
    }


    for (i = 0; i < 5; i++) {
        int* pop_data = (void*)queue_pop(q);
        if (!pop_data) {
            printf("queue_pop failed!\n");
        } else {
            printf("queue_pop success: %d\n", *pop_data);
        }
    }

    if (queue_is_empty(q)) {
        printf("queue_is_empty!\n");
    }

    queue_destroy(q);
    return 0;
}

#endif

