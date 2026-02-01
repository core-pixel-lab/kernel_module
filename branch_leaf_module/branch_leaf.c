#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h> // 用于 kmalloc 和 kfree

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Gemini Assistant");
MODULE_DESCRIPTION("One-to-Many Relationship Demo based on Book Image");

// 对应图片中的 struct leaf
struct leaf {
    int id;             // 模拟 // others 中的数据
    char *name;         // 模拟 // others 中的数据
    
    struct leaf *next;  // 图片中的关键指针：指向下一个叶子
};

// 对应图片中的 struct branch
struct branch {
    char *branch_name;  // 模拟 // others 中的数据
    
    struct leaf *head;  // 图片中的关键指针：指向链表头
};

/* 全局指针，用于演示 */
static struct branch *my_branch = NULL;

/* * 辅助函数：创建一个新的 leaf 节点
 */
static struct leaf *create_leaf(int id, char *name)
{
    struct leaf *new_node = kmalloc(sizeof(struct leaf), GFP_KERNEL);
    if (!new_node)
        return NULL;
    
    new_node->id = id;
    new_node->name = name;
    new_node->next = NULL;
    
    return new_node;
}

/* * 辅助函数：将 leaf 添加到 branch 中 (头插法)
 * 体现了 branch 和 leaf 的一对多连接
 */
static void add_leaf_to_branch(struct branch *br, struct leaf *lf)
{
    if (!br || !lf) return;

    // 单向链表的标准插入操作
    lf->next = br->head;
    br->head = lf;
}

/* * 模块初始化函数
 */
static int __init relation_demo_init(void)
{
    struct leaf *l1, *l2, *l3;
    struct leaf *cursor;

    printk(KERN_INFO "BranchLeaf: 模块加载中...\n");

    // 1. 分配 branch 内存
    my_branch = kmalloc(sizeof(struct branch), GFP_KERNEL);
    if (!my_branch) return -ENOMEM;

    my_branch->branch_name = "主干道";
    my_branch->head = NULL; // 初始化链表头为空

    // 2. 创建几个 leaf 节点
    l1 = create_leaf(1, "叶子_A");
    l2 = create_leaf(2, "叶子_B");
    l3 = create_leaf(3, "叶子_C");

    // 3. 建立“一对多”关系 (将叶子挂到树枝上)
    add_leaf_to_branch(my_branch, l1);
    add_leaf_to_branch(my_branch, l2);
    add_leaf_to_branch(my_branch, l3);

    // 4. 遍历链表，打印关系验证
    printk(KERN_INFO "BranchLeaf: --- 开始遍历 \"%s\" 下的所有节点 ---", my_branch->branch_name);
    
    cursor = my_branch->head;
    while (cursor != NULL) {
        printk(KERN_INFO "BranchLeaf: 找到节点 -> ID: %d, Name: %s\n", cursor->id, cursor->name);
        // 通过 next 指针移动到下一个
        cursor = cursor->next; 
    }
    
    return 0;
}

/* * 模块退出函数
 */
static void __exit relation_demo_exit(void)
{
    struct leaf *cursor;
    struct leaf *temp;

    printk(KERN_INFO "BranchLeaf: 模块卸载，开始清理内存...\n");

    if (my_branch) {
        // 1. 遍历并释放所有 leaf 节点
        cursor = my_branch->head;
        while (cursor != NULL) {
            temp = cursor;
            cursor = cursor->next;
            
            printk(KERN_INFO "BranchLeaf: 释放节点 ID: %d\n", temp->id);
            kfree(temp);
        }

        // 2. 释放 branch 节点本身
        kfree(my_branch);
        printk(KERN_INFO "BranchLeaf: 释放 Branch 结构体完成。\n");
    }
}

module_init(relation_demo_init);
module_exit(relation_demo_exit);