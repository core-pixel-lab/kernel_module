#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>    // kmalloc,kfree在这里
#include <linux/vmalloc.h> // vmalloc,vfree在这里
#include <asm/io.h>        // virt_to_phys在这里

// 定义两个指针，用来存放申请到的内存地址
static char *k_buf;
static char *v_buf;

// 模块加载函数（相当于 main 函数）
static int __init race_test_init(void)
{
    unsigned long paddr;

    printk(KERN_INFO "=== Driver Study: Race Test Start ===\n");

    k_buf = kmalloc(1024, GFP_KERNEL);
    if (!k_buf) {
        printk(KERN_ERR "kmalloc failed\n");
        return -ENOMEM;
    }
    
    paddr = virt_to_phys(k_buf);
    printk(KERN_INFO "[kmalloc] Virt: %p -> Phys: 0x%lx \n", k_buf, paddr);

    v_buf = vmalloc(1024);
    if (!v_buf) {
        printk(KERN_ERR "vmalloc failed\n");
        kfree(k_buf);
        return -ENOMEM;
    }

    printk(KERN_INFO "[vmalloc] Virt: %p \n", v_buf);

    return 0;
}

static void __exit race_test_exit(void)
{
    kfree(k_buf);
    vfree(v_buf);
    printk(KERN_INFO "=== Race Test Exit (Cleaned up) ===\n");
}

module_init(race_test_init);
module_exit(race_test_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Holden Hsu");
MODULE_DESCRIPTION("A simple memory test module");
