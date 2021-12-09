#include<linux/init.h>  //初始换函数
#include<linux/kernel.h>  //内核头文件
#include<linux/module.h>  //模块的头文件

static struct timer_list test_timer;

void timer_func(struct timer_list *timer)
{
    printk("[timer_func]  \n");
    mod_timer(timer, jiffies + HZ);
}

static int timer_test_init(void) 
{
    printk("timer_test_init!\n");

    timer_setup(&test_timer, timer_func, 0);
    test_timer.expires = jiffies + HZ;  // 定时的时间点，HZ是jiffies时钟的周期，当前时间的1s之后
    add_timer(&test_timer);

    printk("timer_test_init ok!\n");
    return 0; 
}

static void timer_test_exit(void) 
{
    del_timer(&test_timer);
    printk("timer_test_exit!\n");
}

module_init(timer_test_init);
module_exit(timer_test_exit); 
MODULE_LICENSE("KX BSD/GPL");
