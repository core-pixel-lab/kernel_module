#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/kthread.h>  // 内核线程
#include <linux/delay.h>

// 定义一个“被争抢”的全局变量
static int shared_resource = 0;

// 定义两个线程指针
static struct task_struct *thread1;
static struct task_struct *thread2;

// 线程要干的活：疯狂加 100 万次
static int thread_function(void *data)
{
    int i;
    // 这里的 data 只是用来区分是线程1还是线程2
    char *thread_name = (char *)data;

    printk(KERN_INFO "[%s] Start running...\n", thread_name);

    for (i = 0; i < 10000000; i++) {
        // === 危险区域 (Critical Section) ===
        // 也就是“两只手抢面包”的地方
        // 这里的操作不是原子的，很容易被打断
        shared_resource++; 
        // ================================
    }

    printk(KERN_INFO "[%s] Job done!\n", thread_name);
    return 0;
}

static int __init race_test_init(void)
{
    printk(KERN_INFO "=== Race Test Module Start ===\n");

    // 1. 初始化变量
    shared_resource = 0;

    // 2. 启动两个内核线程，同时去跑 thread_function
    // kthread_run = kthread_create + wake_up_process
    thread1 = kthread_run(thread_function, "Thread-A", "thread-1");
    thread2 = kthread_run(thread_function, "Thread-B", "thread-2");

    return 0;
}

static void __exit race_test_exit(void)
{
    // 理论上如果每个线程都加了 100万次，结果应该是 200万
    printk(KERN_INFO "=== Final Result: %d (Expected: 20000000) ===\n", shared_resource);
    
    if (shared_resource != 20000000) {
        printk(KERN_ERR "!!! RACE CONDITION DETECTED !!!\n");
    } else {
        printk(KERN_INFO "Luck? Or fixed? \n");
    }
}

module_init(race_test_init);
module_exit(race_test_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Holden Hsu");