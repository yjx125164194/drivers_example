#include <linux/delay.h> // 加入延时功能支持
#include <linux/kernel.h>
#include <linux/kthread.h> // 加入对内核线程的支持
#include <linux/module.h>

#include "driver_include.h"

static struct task_struct *task; // 定义一个task_struct指针来保存线程的状态

// 线程函数
static int thread_function(void *data) {
  while (!kthread_should_stop()) { // 检查是否应该停止线程
    my_example_function();         // 调用函数
    ssleep(1);                     // 睡眠1秒
  }
  return 0;
}

static int __init driver_test_init(void) {
  printk(KERN_INFO "Driver test loaded\n");
  // 创建并启动线程
  task = kthread_run(thread_function, NULL, "my_example_thread");
  if (IS_ERR(task)) {
    printk(KERN_ALERT "Failed to create thread\n");
    return PTR_ERR(task);
  }
  return 0;
}

static void __exit driver_test_exit(void) {
  printk(KERN_INFO "Driver test unloaded\n");
  if (task) {
    kthread_stop(task); // 停止线程
  }
}

module_init(driver_test_init);
module_exit(driver_test_exit);

MODULE_LICENSE("GPL");
