#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
//#include <libthread.h>

static int count = 0;

#if 1
sem_t   sem1;
sem_t   sem2;

typedef struct {
    // User defined data may be declared here.

} Foo;

Foo* fooCreate() {
    Foo* obj = (Foo*) malloc(sizeof(Foo));
    
    // Initialize user defined data here.
    sem_init(&sem1, 0, 0);
    sem_init(&sem1, 0, 0);
    return obj;
}

void first(Foo* obj) {
    
    // printFirst() outputs "first". Do not change or remove this line.
    printFirst();
    sem_post(&sem1);
}

void second(Foo* obj) {
    
    // printSecond() outputs "second". Do not change or remove this line.
    sem_wait(&sem1);
    printSecond();
    sem_post(&sem2);
}

void third(Foo* obj) {
    
    // printThird() outputs "third". Do not change or remove this line.
    sem_wait(&sem2);
    printThird();
}

void fooFree(Foo* obj) {
    // User defined data may be cleaned up here.
    sem_destroy(&sem1);
    sem_destroy(&sem2);
}
#endif

void child_func()
{
    printf("chirld process!\n");
    printf("[child_func] pid=%d， parent pid = %d\n", getpid(), getppid());
    while(1) {
        sleep(1);
        printf("[child_func] p: %p, count = %d\n", &count, count++);
    }
}

int main()
{
    pid_t pid;

    pid = fork();
    if (pid < 0) {
        printf("fork failed!\n");
        exit(1);
    }

    if (pid == 0) {
        child_func();
        exit(0);
    }

    printf("[parent] pid=%d， child pid = %d\n", getpid() ,pid);

    while(1) {
        sleep(1);
        printf("[parent] p: %p, count = %d\n", &count, count);
        count += 2;
    }
    return 0;
}

