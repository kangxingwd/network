

# 使用

```cpp
#include <pthread.h>

// return: 0 success
int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine) (void *), void *arg);

// return: 0 success
int pthread_join(pthread_t thread, void **retval);



```


# 线程同步的四种方法

## 文档
- https://blog.csdn.net/xp178171640/article/details/104468679/

## 一 互斥锁（mutex）
锁机制是同一时刻只允许一个线程执行一个关键部分的代码。

1 初始化锁
int pthread_mutex_init(pthread_mutex_t *mutex,const pthread_mutex_attr_t *mutexattr);

其中参数 mutexattr 用于指定锁的属性（见下），如果为NULL则使用缺省属性。

互斥锁的属性在创建锁的时候指定，在LinuxThreads实现中仅有一个锁类型属性，不同的锁类型在试图对一个已经被锁定的互斥锁加锁时表现不同。当前有四个值可供选择：

1 PTHREAD_MUTEX_TIMED_NP，这是缺省值，也就是普通锁。当一个线程加锁以后，其余请求锁的线程将形成一个等待队列，并在解锁后按优先级获得锁。这种锁策略保证了资源分配的公平性。

2 PTHREAD_MUTEX_RECURSIVE_NP，嵌套锁，允许同一个线程对同一个锁成功获得多次，并通过多次unlock解锁。如果是不同线程请求，则在加锁线程解锁时重新竞争。

3 PTHREAD_MUTEX_ERRORCHECK_NP，检错锁，如果同一个线程请求同一个锁，则返回EDEADLK，否则与PTHREAD_MUTEX_TIMED_NP类型动作相同。这样就保证当不允许多次加锁时不会出现最简单情况下的死锁。

4 PTHREAD_MUTEX_ADAPTIVE_NP，适应锁，动作最简单的锁类型，仅等待解锁后重新竞争。

2 阻塞加锁
int pthread_mutex_lock(pthread_mutex *mutex);

3 非阻塞加锁
int pthread_mutex_trylock( pthread_mutex_t *mutex);

该函数语义与 pthread_mutex_lock() 类似，不同的是在锁已经被占据时返回 EBUSY 而不是挂起等待。

4 解锁（要求锁是lock状态,并且由加锁线程解锁）
int pthread_mutex_unlock(pthread_mutex *mutex);

5 销毁锁（此时锁必需unlock状态,否则返回EBUSY）
int pthread_mutex_destroy(pthread_mutex *mutex);

- 示例代码：
```cpp
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int gn;

void* thread(void *arg)
{
    printf("thread's ID is  %d\n",pthread_self());
    pthread_mutex_lock(&mutex);
    gn = 12;
    printf("Now gn = %d\n",gn);
    pthread_mutex_unlock(&mutex);
    return NULL;
}

int main()
{
    pthread_t id;
    printf("main thread's ID is %d\n",pthread_self());
    gn = 3;
    printf("In main func, gn = %d\n",gn);
    if (!pthread_create(&id, NULL, thread, NULL)) {
        printf("Create thread success!\n");
    } else {
        printf("Create thread failed!\n");
    }
    
    pthread_join(id, NULL);
    pthread_mutex_destroy(&mutex);
    return 0;
}
```

## 二 条件变量（cond）
条件变量是利用线程间共享全局变量进行同步的一种机制。条件变量上的基本操作有：触发条件(当条件变为 true 时)；等待条件，挂起线程直到其他线程触发条件。

1 初始化条件变量
int pthread_cond_init(pthread_cond_t *cond,pthread_condattr_t *cond_attr);

尽管POSIX标准中为条件变量定义了属性，但在Linux中没有实现，因此cond_attr值通常为NULL，且被忽略。

2 有两个等待函数
无条件等待
int pthread_cond_wait(pthread_cond_t *cond,pthread_mutex_t *mutex);

计时等待
int pthread_cond_timedwait(pthread_cond_t *cond,pthread_mutex *mutex,const timespec *abstime);

如果在给定时刻前条件没有满足，则返回ETIMEOUT，结束等待，其中abstime以与time()系统调用相同意义的绝对时间形式出现，0表示格林尼治时间1970年1月1日0时0分0秒。
　
无论哪种等待方式，都必须和一个互斥锁配合，以防止多个线程同时请求（用 pthread_cond_wait() 或 pthread_cond_timedwait() 请求）竞争条件（Race Condition）。mutex互斥锁必须是普通锁（PTHREAD_MUTEX_TIMED_NP）或者适应锁（PTHREAD_MUTEX_ADAPTIVE_NP），且在调用pthread_cond_wait()前必须由本线程加锁（pthread_mutex_lock()），而在更新条件等待队列以前，mutex保持锁定状态，并在线程挂起进入等待前解锁。在条件满足从而离开pthread_cond_wait()之前，mutex将被重新加锁，以与进入pthread_cond_wait()前的加锁动作对应。

3 激发条件
激活一个等待该条件的线程（存在多个等待线程时按入队顺序激活其中一个）

int pthread_cond_signal(pthread_cond_t *cond);

激活所有等待线程

int pthread_cond_broadcast(pthread_cond_t *cond);

4 销毁条件变量
int pthread_cond_destroy(pthread_cond_t *cond);

只有在没有线程在该条件变量上等待的时候才能销毁这个条件变量，否则返回EBUSY
说明：

pthread_cond_wait 自动解锁互斥量(如同执行了pthread_unlock_mutex)，并等待条件变量触发。这时线程挂起，不占用CPU时间，直到条件变量被触发（变量为ture）。在调用 pthread_cond_wait之前，应用程序必须加锁互斥量。pthread_cond_wait函数返回前，自动重新对互斥量加锁(如同执行了pthread_lock_mutex)。

互斥量的解锁和在条件变量上挂起都是自动进行的。因此，在条件变量被触发前，如果所有的线程都要对互斥量加锁，这种机制可保证在线程加锁互斥量和进入等待条件变量期间，条件变量不被触发。条件变量要和互斥量相联结，以避免出现条件竞争——个线程预备等待一个条件变量，当它在真正进入等待之前，另一个线程恰好触发了该条件（条件满足信号有可能在测试条件和调用pthread_cond_wait函数（block）之间被发出，从而造成无限制的等待）。

条件变量函数不是异步信号安全的，不应当在信号处理程序中进行调用。特别要注意，如果在信号处理程序中调用 pthread_cond_signal 或 pthread_cond_boardcast 函数，可能导致调用线程死锁

示例代码1：
```cpp
#include <stdio.h>
#include <pthread.h>
#include "stdlib.h"
#include "unistd.h"

pthread_mutex_t mutex;
pthread_cond_t cond;

void hander(void *arg)
{
    free(arg);
    (void)pthread_mutex_unlock(&mutex);
}

void *thread1(void *arg)
{
    pthread_cleanup_push(hander, &mutex);
    while (1) {
        printf("thread1 is running\n");
        pthread_mutex_lock(&mutex);
        pthread_cond_wait(&cond,&mutex);
        printf("thread1 applied the condition\n");
        pthread_mutex_unlock(&mutex);
        sleep(4);
    }
    pthread_cleanup_pop(0);
}

void *thread2(void *arg)
{
    while (1) {
        printf("thread2 is running\n");
        pthread_mutex_lock(&mutex);
        pthread_cond_wait(&cond,&mutex);
        printf("thread2 applied the condition\n");
        pthread_mutex_unlock(&mutex);
        sleep(1);
    }
}

int main()
{
    pthread_t thid1,thid2;
    printf("condition variable study!\n");
    pthread_mutex_init(&mutex,NULL);
    pthread_cond_init(&cond,NULL);
    pthread_create(&thid1,NULL,thread1,NULL);
    pthread_create(&thid2,NULL,thread2,NULL);

    sleep(1);
    do {
        pthread_cond_signal(&cond);
    } while(1);

    sleep(20);
    pthread_exit(0);
    return 0;
}
```

- 示例代码2：
```cpp
#include <pthread.h>
#include <unistd.h>
#include "stdio.h"
#include "stdlib.h"

static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

struct node
{
    int n_number;
    struct node *n_next;
}*head = NULL;

static void cleanup_handler(void *arg)
{
    printf("Cleanup handler of second thread.\n");
    free(arg);
    (void)pthread_mutex_unlock(&mtx);
}

static void *thread_func(void *arg)
{
    struct node *p = NULL;
    pthread_cleanup_push(cleanup_handler, p);

    while (1) {
        // 这个mutex主要是用来保证pthread_cond_wait的并发性。
        pthread_mutex_lock(&mtx);
        while (head == NULL) {
            /* 这个while要特别说明一下，单个pthread_cond_wait功能很完善，为何
            * 这里要有一个while (head == NULL)呢？因为pthread_cond_wait里的线
            * 程可能会被意外唤醒，如果这个时候head != NULL，则不是我们想要的情况。
            * 这个时候，应该让线程继续进入pthread_cond_wait
            * pthread_cond_wait会先解除之前的pthread_mutex_lock锁定的mtx，
            * 然后阻塞在等待对列里休眠，直到再次被唤醒（大多数情况下是等待的条件成立
            * 而被唤醒，唤醒后，该进程会先锁定先pthread_mutex_lock(&mtx);，再读取资源
            * 用这个流程是比较清楚的。*/

            pthread_cond_wait(&cond, &mtx);
            p = head;
            head = head->n_next;
            printf("Got %d from front of queue\n", p->n_number);
            free(p);
        }
        pthread_mutex_unlock(&mtx); // 临界区数据操作完毕，释放互斥锁。
    }
    pthread_cleanup_pop(0);
    return 0;
}

int main(void)
{
    pthread_t tid;
    int i;
    struct node *p;

    /* 子线程会一直等待资源，类似生产者和消费者，但是这里的消费者可以是多个消费者，
    * 而不仅仅支持普通的单个消费者，这个模型虽然简单，但是很强大。*/
    pthread_create(&tid, NULL, thread_func, NULL);
    sleep(1);
    for (i = 0; i < 10; i++)
    {
        p = (struct node*)malloc(sizeof(struct node));
        p->n_number = i;
        pthread_mutex_lock(&mtx); // 需要操作head这个临界资源，先加锁。
        p->n_next = head;
        head = p;
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mtx); //解锁

        sleep(1);
    }

    printf("thread 1 wanna end the line.So cancel thread 2.\n");

    /* 关于pthread_cancel，有一点额外的说明，它是从外部终止子线程，子线程会在最近的取消点，
    * 退出线程，而在我们的代码里，最近的取消点肯定就是pthread_cond_wait()了。*/

    pthread_cancel(tid);
    pthread_join(tid, NULL);

    printf("All done -- exiting\n");
    return 0;
}
```

可以看出，等待条件变量信号的用法约定一般是这样的：
```cpp
...
pthread_mutex_lock(&mutex);
...
pthread_cond_wait (&cond, &mutex);
...
pthread_mutex_unlock (&mutex);
...
```

相信很多人都会有这个疑问：为什么pthread_cond_wait需要的互斥锁不在函数内部定义，而要使用户定义的呢？现在没有时间研究 pthread_cond_wait 的源代码，带着这个问题对条件变量的用法做如下猜测，希望明白真相看过源代码的朋友不吝指正。

pthread_cond_wait 和 pthread_cond_timedwait 函数为什么需要互斥锁？因为：条件变量是线程同步的一种方法，这两个函数又是等待信号的函数，函数内部一定有须要同步保护的数据。

使用用户定义的互斥锁而不在函数内部定义的原因是：无法确定会有多少用户使用条件变量，所以每个互斥锁都须要动态定义，而且管理大量互斥锁的开销太大，使用用户定义的即灵活又方便，符合UNIX哲学的编程风格（随便推荐阅读《UNIX编程哲学》这本好书！）。
好了，说完了1和2，我们来自由猜测一下 pthread_cond_wait 函数的内部结构吧：

```cpp
 int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex)
   {
      if（没有条件信号）
      {
         （1）pthread_mutex_unlock (mutex); // 因为用户在函数外面已经加锁了（这是使用约定），但是在没有信号的情况下为了让其他线程也能等待cond，必须解锁。
         （2） 阻塞当前线程，等待条件信号（当然应该是类似于中断触发的方式等待，而不是软件轮询的方式等待）... 有信号就继续执行后面。
         （3） pthread_mutex_lock (mutex); // 因为用户在函数外面要解锁（这也是使用约定），所以要与1呼应加锁，保证用户感觉依然是自己加锁、自己解锁。
      }      
      ...
  }
```

## 三 信号量
如同进程一样，线程也可以通过信号量来实现通信，虽然是轻量级的。

线程使用的基本信号量函数有四个：

#include <semaphore.h>

1 初始化信号量
int sem_init (sem_t *sem , int pshared, unsigned int value);

参数：
sem - 指定要初始化的信号量；
pshared - 信号量 sem 的共享选项，linux只支持0，表示它是当前进程的局部信号量；
value - 信号量 sem 的初始值。

2 信号量值加1
给参数sem指定的信号量值加1。

int sem_post(sem_t *sem);

3 信号量值减1
给参数sem指定的信号量值减1。

int sem_wait(sem_t *sem);

如果sem所指的信号量的数值为0，函数将会等待直到有其它线程使它不再是0为止。

4 销毁信号量
销毁指定的信号量。

int sem_destroy(sem_t *sem);

示例代码：
```cpp
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>

#define return_if_fail(p) if((p) == 0){printf ("[%s]:func error!\n", __func__);return;}

typedef struct _PrivInfo
{
    sem_t s1;
    sem_t s2;
    time_t end_time;
}PrivInfo;

static void info_init (PrivInfo* prifo);
static void info_destroy (PrivInfo* prifo);
static void* pthread_func_1 (PrivInfo* prifo);
static void* pthread_func_2 (PrivInfo* prifo);

int main (int argc, char** argv)
{
    pthread_t pt_1 = 0;
    pthread_t pt_2 = 0;
    int ret = 0;
    PrivInfo* prifo = NULL;
    prifo = (PrivInfo* )malloc (sizeof (PrivInfo));

    if (prifo == NULL) {
        printf ("[%s]: Failed to malloc priv.\n");
        return -1;
    }

    info_init (prifo);
    ret = pthread_create (&pt_1, NULL, (void*)pthread_func_1, prifo);
    if (ret != 0) {
        perror ("pthread_1_create:");
    }

    ret = pthread_create (&pt_2, NULL, (void*)pthread_func_2, prifo);
    if (ret != 0) {
        perror ("pthread_2_create:");
    }

    pthread_join (pt_1, NULL);
    pthread_join (pt_2, NULL);
    info_destroy (prifo);
    return 0;
}

static void info_init (PrivInfo* prifo)
{
    return_if_fail (prifo != NULL);
    prifo->end_time = time(NULL) + 10;
    sem_init (&prifo->s1, 0, 1);
    sem_init (&prifo->s2, 0, 0);
    return;
}

static void info_destroy (PrivInfo* prifo)
{
    return_if_fail (prifo != NULL);
    sem_destroy (&prifo->s1);
    sem_destroy (&prifo->s2);
    free (prifo);
    prifo = NULL;
    return;
}

static void* pthread_func_1 (PrivInfo* prifo)
{
    return_if_fail (prifo != NULL);
    while (time(NULL) < prifo->end_time)
    {
        sem_wait (&prifo->s2);
        printf ("pthread1: pthread1 get the lock.\n");
        sem_post (&prifo->s1);
        printf ("pthread1: pthread1 unlock\n");
        sleep (1);
    }
    return;
}

static void* pthread_func_2 (PrivInfo* prifo)
{
    return_if_fail (prifo != NULL);
    while (time (NULL) < prifo->end_time)
    {
        sem_wait (&prifo->s1);
        printf ("pthread2: pthread2 get the unlock.\n");
        sem_post (&prifo->s2);
        printf ("pthread2: pthread2 unlock.\n");
        sleep (1);
    }
    
    return;
}
```

## 四 读写锁
注意事项
如果一个线程用读锁锁定了临界区，那么其他线程也可以用读锁来进入临界区，这样就可以多个线程并行操作。但这个时候，如果再进行写锁加锁就会发生阻塞，写锁请求阻塞后，后面如果继续有读锁来请求，这些后来的读锁都会被阻塞！这样避免了读锁长期占用资源，防止写锁饥饿！

如果一个线程用写锁锁住了临界区，那么其他线程不管是读锁还是写锁都会发生阻塞！

1 初始化
int pthread_rwlock_init(pthread_rwlock_t *restrict rwlock, const pthread_rwlockattr_t *restrict attr);

2 读写加锁
int pthread_rwlock_rdlock(pthread_rwlock_t *rwlock);
int pthread_rwlock_wrlock(pthread_rwlock_t *rwlock);
int pthread_rwlock_unlock(pthread_rwlock_t *rwlock);
int pthread_rwlock_tryrdlock(pthread_rwlock_t *rwlock);
int pthread_rwlock_trywrlock(pthread_rwlock_t *rwlock);
int pthread_rwlock_timedrdlock(pthread_rwlock_t *restrict rwlock, const struct timespec *restrict abs_timeout);
int pthread_rwlock_timedwrlock(pthread_rwlock_t *restrict rwlock, const struct timespec *restrict abs_timeout);

3 销毁锁
int pthread_rwlock_destroy(pthread_rwlock_t *rwlock);

应用实例：
```cpp
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

/* 初始化读写锁 */
pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;
/* 全局资源 */
int global_num = 10;

void err_exit(const char *err_msg)
{
 printf("error:%s\n", err_msg);
 exit(1);
}

/* 读锁线程函数 */
void *thread_read_lock(void *arg)
{
 char *pthr_name = (char *)arg;

 while (1)
 {
     /* 读加锁 */
     pthread_rwlock_rdlock(&rwlock);

     printf("线程%s进入临界区，global_num = %d\n", pthr_name, global_num);
     sleep(1);
     printf("线程%s离开临界区...\n", pthr_name);

     /* 读解锁 */
     pthread_rwlock_unlock(&rwlock);

     sleep(1);
 }

 return NULL;
}

/* 写锁线程函数 */
void *thread_write_lock(void *arg)
{
 char *pthr_name = (char *)arg;

 while (1)
 {
     /* 写加锁 */
     pthread_rwlock_wrlock(&rwlock);

     /* 写操作 */
     global_num++;
     printf("线程%s进入临界区，global_num = %d\n", pthr_name, global_num);
     sleep(1);
     printf("线程%s离开临界区...\n", pthr_name);

     /* 写解锁 */
     pthread_rwlock_unlock(&rwlock);

     sleep(2);
 }

 return NULL;
}

int main(void)
{
 pthread_t tid_read_1, tid_read_2, tid_write_1, tid_write_2;

 /* 创建4个线程，2个读，2个写 */
 if (pthread_create(&tid_read_1, NULL, thread_read_lock, "read_1") != 0)
     err_exit("create tid_read_1");

 if (pthread_create(&tid_read_2, NULL, thread_read_lock, "read_2") != 0)
     err_exit("create tid_read_2");

 if (pthread_create(&tid_write_1, NULL, thread_write_lock, "write_1") != 0)
     err_exit("create tid_write_1");

 if (pthread_create(&tid_write_2, NULL, thread_write_lock, "write_2") != 0)
     err_exit("create tid_write_2");

 /* 随便等待一个线程，防止main结束 */
 if (pthread_join(tid_read_1, NULL) != 0)
     err_exit("pthread_join()");

 return 0;
}
```

