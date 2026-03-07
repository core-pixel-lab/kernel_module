#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>

// #include <linux/kthread.h>  // 内核线程
// #include <linux/delay.h>

#include <linux/slab.h>
#include <linux/list.h>

// // 定义一个“被争抢”的全局变量
// static int shared_resource = 0;

// // 定义两个线程指针
// static struct task_struct *thread1;
// static struct task_struct *thread2;


struct my_test_node {
    int data;
    struct list_head list;
};

static LIST_HEAD(test_list_head);

static int __init data_struct_init(void)
{
    struct my_test_node *node1, *node2, *cursor;

    printk(KERN_INFO "=== Data Struct Module Start ===\n");

    node1 = kmalloc(sizeof(*node1), GFP_KERNEL);
    if(!node1) {
        printk(KERN_INFO "node1 gg\n");
        return -ENOMEM;
    }
    node1->data = 10;
    list_add_tail(&node1->list, &test_list_head);

    node2 = kmalloc(sizeof(*node2), GFP_KERNEL);
    if(!node2) {
        printk(KERN_INFO "node2 gg\n");
        kfree(node1);
        return -ENOMEM;
    }
    node2->data = 20;
    list_add_tail(&node2->list, &test_list_head);

    printk(KERN_INFO "Traversing the list:\n");
    list_for_each_entry(cursor, &test_list_head, list) {
        printk(KERN_INFO "Data: %d\n", cursor->data);
    }

    return 0;
}

static void __exit data_struct_exit(void)
{
    struct my_test_node *cursor, *temp;

    list_for_each_entry_safe(cursor, temp, &test_list_head, list) {
        printk(KERN_INFO "Remove data: %d\n", cursor->data);
    }

    list_del(&cursor->list);

    kfree(cursor);

    printk(KERN_INFO "=== Data Struct Module Exit ===\n");
}

module_init(data_struct_init);
module_exit(data_struct_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Holden Hsu");