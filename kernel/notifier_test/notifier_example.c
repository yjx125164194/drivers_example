// SPDX-License-Identifier: SimPL-2.0
#include <linux/module.h>
#include <linux/notifier.h>

struct my_data {
	int value;
	const char *message;
};

// 定义通知链
static ATOMIC_NOTIFIER_HEAD(my_notifier_chain);

// 通知回调函数
static int my_notifier_call(struct notifier_block *nb, unsigned long event,
			    void *data)
{
	struct my_data *my_data_ptr = (struct my_data *)data;
	printk(KERN_INFO
	       "Notification received with event: %lu, value: %d, message: %s\n",
	       event, my_data_ptr->value, my_data_ptr->message);
	return NOTIFY_OK;
}

// 定义通知块
static struct notifier_block my_notifier = {
	.notifier_call = my_notifier_call,
};

// 初始化函数
static int __init my_init(void)
{
	struct my_data data = { 42, "Hello, World!" };
	printk(KERN_INFO "Registering my notifier...\n");
	atomic_notifier_chain_register(&my_notifier_chain,
				       &my_notifier); // 注册通知块
	printk(KERN_INFO "Triggering notifier chain...\n");
	atomic_notifier_call_chain(&my_notifier_chain, 12345,
				   &data); // 触发通知
	return 0;
}

// 清理函数
static void __exit my_exit(void)
{
	printk(KERN_INFO "Unregistering my notifier...\n");
	atomic_notifier_chain_unregister(&my_notifier_chain,
					 &my_notifier); // 取消注册通知块
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("yjx");
MODULE_DESCRIPTION("Example of creating and using a notification chain");
