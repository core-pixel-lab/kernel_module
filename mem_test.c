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
static int __init mem_test_init(void)
{
    unsigned long paddr;

    printk(KERN_INFO "=== Driver Study: Memory Test Start ===\n");

    // 1. 测试 kmalloc (物理地址连续，DMA必须用这个)
    // 申请 1024 字节
    k_buf = kmalloc(1024, GFP_KERNEL);//GFP_KERNEL常规申请，GFP_ATOMIC原子申请（着急）
    if (!k_buf) {
        printk(KERN_ERR "kmalloc failed\n");
        return -ENOMEM;//error no memory
    }
    
    // 打印 kmalloc 的【虚拟地址】
    // 转换并打印【物理地址】 (virt_to_phys 只能用于低端内存/kmalloc)
    paddr = virt_to_phys(k_buf);
    printk(KERN_INFO "[kmalloc] Virt: %p -> Phys: 0x%lx (适合做DMA)\n", k_buf, paddr);//lx是long hex，长十六进制的意思

    // 2. 测试 vmalloc (虚拟地址连续，物理不一定连续)
    v_buf = vmalloc(1024);
    if (!v_buf) {
        printk(KERN_ERR "vmalloc failed\n");
        kfree(k_buf); // 别忘了释放刚才申请的k_buf
        return -ENOMEM;
    }

    // vmalloc 的地址通常非常大，属于内核的高端映射区
    printk(KERN_INFO "[vmalloc] Virt: %p (地址很高，不适合直接做DMA)\n", v_buf);

    return 0;
}

// 模块卸载函数（退出时清理垃圾）
static void __exit mem_test_exit(void)
{
    // 必须释放内存，否则就是内存泄漏（Memory Leak）！
    // 这一点在嵌入式设备上是致命的。
    kfree(k_buf);
    vfree(v_buf);
    printk(KERN_INFO "=== Memory Test Exit (Cleaned up) ===\n");
}

// 必须加的宏
module_init(mem_test_init);
module_exit(mem_test_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Holden Hsu");
MODULE_DESCRIPTION("A simple memory test module");