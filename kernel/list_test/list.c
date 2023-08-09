// SPDX-License-Identifier: SimPL-2.0
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/slab.h>

struct my_node {
    int data;
    struct list_head list;
};

static LIST_HEAD(my_list);

static int __init my_module_init(void)
{
    int i;
    struct my_node *node;
    struct my_node *new_node;
    printk(KERN_INFO "Initializing the list demo module\n");

    // 插入节点到链表中
    for (i = 0; i < 5; i++) {
	new_node = kmalloc(sizeof(*new_node), GFP_KERNEL);
        if (new_node) {
            new_node->data = i;
            INIT_LIST_HEAD(&new_node->list);
            list_add_tail(&new_node->list, &my_list);
        }
    }

    // 遍历链表
    list_for_each_entry(node, &my_list, list) {
        printk(KERN_INFO "Node with data: %d\n", node->data);
    }

    return 0;
}

static void __exit my_module_exit(void) {
    struct my_node *node, *tmp;

    // 删除并释放链表中的所有节点
    list_for_each_entry_safe(node, tmp, &my_list, list) {
        printk(KERN_INFO "Freeing node with data: %d\n", node->data);
        list_del(&node->list);
        kfree(node);
    }

    printk(KERN_INFO "Exiting the list demo module\n");
}

module_init(my_module_init);
module_exit(my_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("YANGJX");
MODULE_DESCRIPTION("A simple demo for using list_head in the Linux kernel");

