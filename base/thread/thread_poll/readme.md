### 为什么需要线程池

在那些情况下我们会使用到多线程：
* 阻塞调用（阻塞IO调用、等待资源）
* 耗时的计算（读写文件、复杂的计算）
* 高密度任务（高并发低延时的网络IO请求）

面临以上情况时都去临时创建线程会带来什么问题：
* 创建了太多的线程，系统资源就会被浪费，而且会浪费时间去创建和销毁线程。
* 创建线程太慢，导致执行任务结果返回过慢。
* 销毁线程太慢，可能会影响别的进程使用资源。

所以：创建多个线程，放在池子里不销毁，要用的时候就把任务**丢给**池子里的线程去执行，这就是线程池。
OK，问题来了任务由谁产生（生产者），如何**丢给**线程池的某个线程（消费者）？这个问题的回答需从以下几方面：

1） 生产者采用什么方式与消费者同步？
2） 任务如何保存？
3） 生产者之间的同步方式，消费者之间的同步方式？

**一下所有的代码设计适用于单生产者多消费者模式**

### 条件变量结合互斥锁 + 任务队列
设计如何：
![条件变量](https://github.com/zhiyong0804/f-threadpool/blob/master/cond_mutex.png?raw=true)

代码如下：
```
typedef struct queue_task
{
    void* (*run)(void *);
    void* argv;
}task_t;

typedef struct queue
{
    int      head;
    int      tail;
    int      size;
    int      capcity;
    task_t*  tasks;
} queue_t;

typedef struct async_queue
{
    pthread_mutex_t  mutex;
    pthread_cond_t   cond;
    int              waiting_threads;

    queue_t*         queue;
    int              quit;   // 0 表示不退出  1 表示退出

    /* 调试变量 */
    long long        tasked;  // 已经处理完的任务数量
} async_queue_t;
```

取任务的代码设计如下：
```
task_t* async_cond_queue_pop_head(async_queue_t* q, int timeout)
{
    task_t *task = NULL;
    struct timeval now;
    struct timespec outtime;
    pthread_mutex_lock(&(q->mutex));
    if (queue_is_empty(q->queue))
    {
        q->waiting_threads++;
        while (queue_is_empty(q->queue) && (q->quit == 0))
        {
            gettimeofday(&now, NULL);
            if (now.tv_usec + timeout > 1000)
            {
                outtime.tv_sec = now.tv_sec + 1;
                outtime.tv_nsec = ((now.tv_usec + timeout) % 1000) * 1000;
            }
            else
            {
                outtime.tv_sec = now.tv_sec;
                outtime.tv_nsec = (now.tv_usec + timeout) * 1000;
            }
            pthread_cond_timedwait(&(q->cond), &(q->mutex), &outtime);
        }
        q->waiting_threads--;
    }

    task = queue_pop_head(q->queue);

    /* 调试代码 */
    if (task)
    {
        q->tasked ++;
        static long long precision = 10;
        if ((q->tasked % precision ) == 0)
        {
            time_t current_stm = get_current_timestamp();
            precision *= 10;
        }
    }
    pthread_mutex_unlock(&(q->mutex));

    return task;
}

```

详情见：[https://github.com/zhiyong0804/f-threadpool/blob/master/async_cond_queue.c](https://github.com/zhiyong0804/f-threadpool/blob/master/async_cond_queue.c)

不足：

1. 因为Mutex引起线程挂起和唤醒的操作，在IO密集型的服务器上不是特别高效（实测过）；
2. 条件变量必须和互斥锁相结合使用，使用起来较麻烦；
3. 条件变量不能像eventfd一样为I/O事件驱动。
4. 管道可以和I/O复用很好的融合，但是管道比eventfd多用了一个文件描述符，而且管道内核还得给其管理的缓冲区，eventfd则不需要，因此eventfd比起管道要高效。

### eventfd + epoll

队列的设计：
```
typedef struct async_queue
{
    queue_t*         queue;
    int              quit;   // 0 表示不退出  1 表示退出

    int              efd;     //event fd,
    int              epollfd; // epoll fd

    /* 调试变量 */
    long long        tasked;  // 已经处理完的任务数量
} async_queue_t;

```
插入任务：

```
BOOL async_eventfd_queue_push_tail(async_queue_t* q, task_t *task)
{
    unsigned long long i = 0xffffffff;
    if (!queue_is_full(q->queue))
    {
        queue_push_tail(q->queue, task);

        struct epoll_event ev;
        int efd = eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
        if (efd == -1) printf("eventfd create: %s", strerror(errno));
        ev.events = EPOLLIN ;// | EPOLLLT;
        ev.data.fd = efd;
        if (epoll_ctl(q->epollfd, EPOLL_CTL_ADD, efd, &ev) == -1)
        {
            return NULL;
        }

        write(efd, &i, sizeof (i));

        return TRUE;
    }

    return FALSE;
}

```
取任务：

```
task_t* async_eventfd_queue_pop_head(async_queue_t* q, int timeout)
{
    unsigned long long i = 0;
    struct epoll_event events[MAX_EVENTS];
    int nfds = epoll_wait(q->epollfd, events, MAX_EVENTS, -1);
    if (nfds == -1)
    {
        return NULL;
    }
    else
    {
        read(events[0].data.fd, &i, sizeof (i));
        close(events[0].data.fd); // NOTE: need to close here
        task_t* task = queue_pop_head(q->queue);

        /* 调试代码 */
        if (task)
        {
            q->tasked ++;
            static long long precision = 10;
            if ((q->tasked % precision ) == 0)
            {
                time_t current_stm = get_current_timestamp();
                printf("%d tasks cost : %d\n", precision, current_stm - start_stm);
                precision *= 10;
            }
        }
        return task;
    }

    return NULL;
}
```
因为eventfd每次写数据后，只会唤醒一个epoll_wait所在的线程，so，确保了同一时刻仅有一个线程取任务。
代码详情：[https://github.com/zhiyong0804/f-threadpool/blob/master/async_eventfd_queue.c](https://github.com/zhiyong0804/f-threadpool/blob/master/async_eventfd_queue.c)

不足：
上面两种方案，所有的线程共用同一个队列，所以消费者线程之间取任务时需要做同步，生产者和消费者也需要做同步。用一个形象的图可以表示如下：
![fuck](https://github.com/zhiyong0804/f-threadpool/blob/master/fuck.png?raw=true)

### eventfd + epoll + 多队列的设计

设计思想如下图：
![多队列](https://github.com/zhiyong0804/f-threadpool/blob/master/multiple.png?raw=true)

代码详情见：[https://github.com/zhiyong0804/StreamingServer](https://github.com/zhiyong0804/StreamingServer)
Oh my god, huge project, where can i find thre thread pool source. 说来话长，这个代码是EasyDarwin的源码，但是因为某种原因，EasyDarwin的源码不再共享，取而代之的打赏的二维码，so，
我把他们的源码做了局部的修改，然后重新提交，且命名为StreamingServer，里面的设计是采用条件变量做同步的，但是多队列的思想是可以沿袭的，打算加班用eventfd实现。

实现了个multiple queue的设计，见本工程，只需要在glo_def添加 #define ENABLE_MULTI_THREAD_QUEUES  1。
在这个设计下使用round robin的算法，这种设计在网络通信中可能会破坏消息的有序性，所以，接下来还要设计对客户端socket进行hash后，
socket所对应的任务固定落在某个线程里处理，类似EasyDarwin的设计。

这样一种设计是不是让我们能够想到下面这幅图呢？
![立交桥](https://github.com/zhiyong0804/f-threadpool/blob/master/lijiao.jpg?raw=true)

之前所有的道路遇到十字路口时（共享了资源），只能使用信号灯去同步汽车的行驶，现如今，把共享资源fuck掉了，用立交桥，爽吧！！！？

并行编程是很难的，可以参考以下这篇论文:
* 并发编程的11个问题英文版：[http://www.it610.com/article/4462577.htm](http://www.it610.com/article/4462577.htm)
* 并发编程的11个问题中文版：[https://blog.csdn.net/mergerly/article/details/39028861](https://blog.csdn.net/mergerly/article/details/39028861)

我也并不聪明，可是当我2年前接触到ZeroMQ这个项目时，我特别惊叹于Pieter Hintjens的一些观点，**“真正的并发就是不共享资源”**

so，方案四的设计

### Lock-free
当我们在第三种方案上，增加了多队列，即每线程每队列时，实际上我们的队列设计变成了一个单生产者单消费者共享的队列，但是这个队列的写指针（tail）仅会被生产者使用，读指针（head）仅会被消费者使用，实际上没有共享任何资源，当然queue_t的size变量，我正在重构把它拿掉。

**OK，那么在这种设计下，消费者线程如何“等待”如何“取”任务？**

实际上，上面的三种方案对于消费者线程都是被动等待通知，收到通知则去取任务，实际上，我们完全可以设计成“轮询”的方案，就是不停地看自己的任务队列里是否有任务，没有就循环一次，中间当然可以加上sched_yield操作，让其它的线程能够得到调度。

### 线程池的尺寸设计多大合适？
CPU密集型的：
thread size = N + 1;  
IO密集型的：
thread size = 2*N + 1;

当然这不是绝对的，所以在mariadb的线程池是可以动态调整这个尺寸的。 

