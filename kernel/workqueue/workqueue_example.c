// SPDX-License-Identifier: SimPL-2.0

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/delay.h>

struct work_struct debugfs_work_struct_0;
struct work_struct debugfs_work_struct_1;
struct work_struct debugfs_work_struct_2;
struct work_struct debugfs_work_struct_3;
struct work_struct debugfs_work_struct_4;

struct workqueue_struct *debugfs_workqueue_struct_0;
struct workqueue_struct *debugfs_workqueue_struct_1;
struct workqueue_struct *debugfs_workqueue_struct_2;
struct workqueue_struct *debugfs_workqueue_struct_3;
struct workqueue_struct *debugfs_workqueue_struct_4;

#define do_test_queue(x)	do_test_queue_##x
#define do_test(x)				\
{						\
	do {					\
		int i;				\
		for (i = 0; i < 20; i++) {	\
			mdelay(1);		\
			pr_info("%d\n", x);	\
		}				\
	} while (0);				\
}

void do_test_queue_0(struct work_struct *work)
{
	do_test(0);
}

void do_test_queue_1(struct work_struct *work)
{
	do_test(1);
}

void do_test_queue_2(struct work_struct *work)
{
	do_test(2);
}

void do_test_queue_3(struct work_struct *work)
{
	do_test(3);
}

void do_test_queue_4(struct work_struct *work)
{
	do_test(4);
}

// This is called when the module loads.
static int __init debugfs_test_init(void)
{

	debugfs_workqueue_struct_0 = alloc_workqueue("test0", WQ_UNBOUND, 0);
	debugfs_workqueue_struct_1 = alloc_workqueue("test1", WQ_UNBOUND, 0);
	debugfs_workqueue_struct_2 = alloc_workqueue("test2", WQ_UNBOUND, 0);
	debugfs_workqueue_struct_3 = alloc_workqueue("test3", WQ_UNBOUND, 0);
	debugfs_workqueue_struct_4 = alloc_workqueue("test4", WQ_UNBOUND | WQ_HIGHPRI, 0);

	INIT_WORK(&debugfs_work_struct_0, do_test_queue(0));
	INIT_WORK(&debugfs_work_struct_1, do_test_queue(1));
	INIT_WORK(&debugfs_work_struct_2, do_test_queue(2));
	INIT_WORK(&debugfs_work_struct_3, do_test_queue(3));
	INIT_WORK(&debugfs_work_struct_4, do_test_queue(4));
#ifdef DEBUG
	schedule_work(&debugfs_work_sturct_0);
	schedule_work(&debugfs_work_sturct_1);
	schedule_work(&debugfs_work_sturct_2);
	schedule_work(&debugfs_work_sturct_3);
	schedule_work(&debugfs_work_sturct_4);
#else
	queue_work(debugfs_workqueue_struct_0, &debugfs_work_struct_0);
	queue_work(debugfs_workqueue_struct_1, &debugfs_work_struct_1);
	queue_work(debugfs_workqueue_struct_2, &debugfs_work_struct_2);
	queue_work(debugfs_workqueue_struct_3, &debugfs_work_struct_3);
	queue_work(debugfs_workqueue_struct_4, &debugfs_work_struct_4);
#endif
	return 0;
}

// This is called when the module is removed.
static void __exit debugfs_test_exit(void)
{
}

module_init(debugfs_test_init);
module_exit(debugfs_test_exit);
MODULE_LICENSE("GPL");
