#include <linux/kernel.h>
#include <linux/module.h>

int my_example_function(void) {
  printk(KERN_INFO "Inside strong function\n");
  return 0;
}

EXPORT_SYMBOL(my_example_function);

static int __init driver_strong_init(void) {
  printk(KERN_INFO "Driver strong loaded\n");
  return 0;
}

static void __exit driver_strong_exit(void) {
  printk(KERN_INFO "Driver strong unloaded\n");
}

module_init(driver_strong_init);
module_exit(driver_strong_exit);

MODULE_LICENSE("GPL");
