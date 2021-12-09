
# Linux内核时钟系统和定时器实现

- Linux 2.6.16之前，内核只支持低精度时钟，内核定时器的工作方式：
    - 系统启动后，会读取时钟源设备(RTC, HPET，PIT…),初始化当前系统时间；
    - 内核会根据HZ(系统定时器频率，节拍率)参数值，设置时钟事件设备，启动tick(节拍)中断。HZ表示1秒种产生多少个时钟硬件中断，tick就表示连续两个中断的间隔时间。 一般： HZ=250， 一个tick = 1/HZ, 所以默认一个tick为4ms。
```s
cat /boot/config-`uname -r` | grep 'CONFIG_HZ='
CONFIG_HZ=250
```

    - 设置时钟事件设备后，时钟事件设备会定时产生一个tick中断，触发时钟中断处理函数，更新系统时钟,并检测timer wheel，进行超时事件的处理。

    - Linux 2.6.16 之前，内核软件定时器采用timer wheel多级时间轮的实现机制，维护操作系统的所有定时事件。timer wheel的触发是基于系统tick周期性中断。
    - 所以说这之前，linux只能支持ms级别的时钟，随着时钟源硬件设备的精度提高和软件高精度计时的需求，有了高精度时钟的内核设计。

    - Linux 2.6.16 ，内核支持了高精度的时钟，内核采用新的定时器hrtimer，其实现逻辑和Linux 2.6.16 之前定时器逻辑区别：
        - hrtimer采用红黑树进行高精度定时器的管理，而不是时间轮；
        - 高精度时钟定时器不在依赖系统的tick中断，而是基于事件触发。

    - 旧内核的定时器实现依赖于系统定时器硬件定期的tick，基于该tick，内核会扫描timer wheel处理超时事件，会更新jiffies，wall time(墙上时间，现实时间)，process的使用时间等等工作。

    - 新的内核不再会直接支持周期性的tick，新内核定时器框架采用了基于事件触发，而不是以前的周期性触发。新内核实现了hrtimer(high resolution timer)，hrtimer的设计目的，就是为了解决time wheel的缺点：
        - 低精度；timer wheel只能支持ms级别的精度，hrtimer可以支持ns级别；
        - Timer wheel与内核其他模块的高耦合性；

    - 内核定时器系统增加了hrtimer之后，对于用户层开放的定时器相关接口基本都是通过hrtimer进行实现的

# 自定义定时器实现方案
- 基于小根堆的定时器实现
    - 堆顶保存最先到到期的定时器，每次只需要遍历堆顶的定时器是否到期，堆顶定时器超时后，调用回调， 调整堆，使其保持小根堆， 继续对堆顶判断；
    - 插入时间复杂度 O(lgn)
    - 调整堆的时间， O(nlgn)
- 基于时间轮的定时器实现
    - 一级时间轮
        - 一个轮子（数组），轮子的每个槽（spoke）后面会挂一个time列表， 表示命中该spoke时，timer列表的所有定时器都超时
        - 这种有局限性，在需要跨度较大的定时器时，需要很大的空间

    - 多级时间轮
        - 多级时间轮中， apoke的含义不再是时间精度，而是hashkey, 每个spoke所链接的timer列表可以用 multimap 实现，让timer的插入时间降低到O(lgn)
        - 到期处理最坏时间时O(lgn), n是每个spoke中的timer的个数


# 用户态定时器 API

- alarm()
- sleep()
- usleep()
- nanosleep()

    - sleep()是在库函数中实现，是通过调用alarm()来设定报警时间，调用sigsuspend()将进程挂起在信号SIGALARM上，而且sleep()也只能精确到秒级上，精度不行
    - 通过alarm()或setitimer()系统调用，非阻塞异步，配合SIGALRM信号处理；
    - 通过select()或nanosleep()系统调用，阻塞调用，往往需要新建一个线程；
    - 通过timefd()调用，基于文件描述符，可以被用于 select/poll 的应用场景；
    - 通过RTC机制, 利用系统硬件提供的Real Time Clock机制, 计时非常精确；

# 内核代码分析

- Linux Kernel提供了内核定时器机制，其核心是由硬体产生中断来追踪时间的流动情况

## struct timer_list 结构体

- 头文件 include/linux/timer.h
- 结构如下
```cpp
struct timer_list {
	/*
	 * All fields that change during normal runtime grouped to the
	 * same cacheline
	 */
	struct hlist_node	entry;      
	unsigned long		expires;        // timer 的时长
	void			(*function)(struct timer_list *);   // 时间到期后的回调函数
	u32			flags;

#ifdef CONFIG_LOCKDEP
	struct lockdep_map	lockdep_map;
#endif
};
```

- 提供的函数
```cpp

// 旧版本的 init_timer
void init_timer(struct timer_list* timer)；


void add_timer(struct timer_list *timer);       // add_timer 调用 mod_timer
void add_timer_on(struct timer_list *timer, int cpu);
int del_timer(struct timer_list * timer);
int mod_timer(struct timer_list *timer, unsigned long expires);
int mod_timer_pending(struct timer_list *timer, unsigned long expires);
int timer_reduce(struct timer_list *timer, unsigned long expires);

static inline int timer_pending(const struct timer_list * timer)

// del_timer_sync函数要完成的任务除了同del_timer一样从定时器队列中删除一个定时器对象外，还会确保当函数返回时系统中没有任何处理器正在执行定时器对象上的定时器函数
#ifdef CONFIG_SMP
  extern int del_timer_sync(struct timer_list *timer);
#else
# define del_timer_sync(t)		del_timer(t)
#endif


void add_timer(struct timer_list *timer)
{
	BUG_ON(timer_pending(timer));
	mod_timer(timer, timer->expires);
}

// timer_pending是用来判断一个处在定时器管理队列中的定时器对象是否已经被调度执行，
// add_timer只是把一个定时器对象加入到内核的管理队列，但是何时执行实际上由时钟中断（更确切地，是内核在时钟中断的softirq部分才开始扫描定时器管理队列）



```

## 内核启动注册时钟中断
```cpp
/ @file: arch/x86/kernel/time.c - Linux 4.9.7
// 内核init阶段注册时钟中断处理函数
static struct irqaction irq0  = {
    .handler = timer_interrupt,
    .flags = IRQF_NOBALANCING | IRQF_IRQPOLL | IRQF_TIMER,
    .name = "timer"
};

void __init setup_default_timer_irq(void)
{
    if (!nr_legacy_irqs())
        return;
    setup_irq(0, &irq0);
}

// Default timer interrupt handler for PIT/HPET
static irqreturn_t timer_interrupt(int irq, void *dev_id)
{
    // 调用体系架构无关的时钟处理流程
    global_clock_event->event_handler(global_clock_event);
    return IRQ_HANDLED;
}
```

## 内核时钟中断处理流程
```cpp
// @file: kernel/time/timer.c - Linux 4.9.7
/*
 * Called from the timer interrupt handler to charge one tick to the current
 * process.  user_tick is 1 if the tick is user time, 0 for system.
 */
void update_process_times(int user_tick)
{
    struct task_struct *p = current;

    /* Note: this timer irq context must be accounted for as well. */
    account_process_tick(p, user_tick);
    run_local_timers();
    rcu_check_callbacks(user_tick);
#ifdef CONFIG_IRQ_WORK
    if (in_irq())
        irq_work_tick();
#endif
    scheduler_tick();
    run_posix_cpu_timers(p);
}

/*
 * Called by the local, per-CPU timer interrupt on SMP.
 */
void run_local_timers(void)
{
    struct timer_base *base = this_cpu_ptr(&timer_bases[BASE_STD]);

    hrtimer_run_queues();
    /* Raise the softirq only if required. */
    if (time_before(jiffies, base->clk)) {
        if (!IS_ENABLED(CONFIG_NO_HZ_COMMON) || !base->nohz_active)
            return;
        /* CPU is awake, so check the deferrable base. */
        base++;
        if (time_before(jiffies, base->clk))
            return;
    }
    raise_softirq(TIMER_SOFTIRQ); // 标记一个软中断去处理所有到期的定时器
}
```

