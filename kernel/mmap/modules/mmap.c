// SPDX-License-Identifier: SimPL-2.0

#include <linux/miscdevice.h>
#include <linux/delay.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/delay.h>
#include <linux/moduleparam.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/ioctl.h>
#include <linux/cdev.h>
#include <linux/string.h>
#include <linux/list.h>
#include <linux/pci.h>
#include <linux/gpio.h>


#define DEVICE_NAME "mymap"

static unsigned char array[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
static unsigned char *buffer;

static int my_open(struct inode *inode, struct file *file)
{
	struct mm_struct *mm = current->mm;

	pr_info("client: %s (%d)\n", current->comm, current->pid);
	pr_info("code  section: [0x%lx   0x%lx]\n", mm->start_code, mm->end_code);
	pr_info("data  section: [0x%lx   0x%lx]\n", mm->start_data, mm->end_data);
	pr_info("brk   section: s: 0x%lx, c: 0x%lx\n", mm->start_brk, mm->brk);
	pr_info("mmap  section: s: 0x%lx\n", mm->mmap_base);
	pr_info("stack section: s: 0x%lx\n", mm->start_stack);
	pr_info("arg   section: [0x%lx   0x%lx]\n", mm->arg_start, mm->arg_end);
	pr_info("env   section: [0x%lx   0x%lx]\n", mm->env_start, mm->env_end);

	return 0;
}

static int my_map(struct file *filp, struct vm_area_struct *vma)
{
	unsigned long page = virt_to_phys(buffer);
	unsigned long offset = vma->vm_pgoff << PAGE_SHIFT;
	unsigned long pfn_start = (page >> PAGE_SHIFT) + vma->vm_pgoff;
	unsigned long virt_start = (unsigned long)buffer + offset;

	unsigned long start = (unsigned long)vma->vm_start;
	unsigned long end = (unsigned long)vma->vm_end;
	unsigned long size = (unsigned long)(vma->vm_end - vma->vm_start);

	pr_info("phy: 0x%lx, offset: 0x%lx, size: 0x%lx\n", pfn_start << PAGE_SHIFT, offset, size);
	pr_info("vma start: 0x%lx,vma end: 0x%lx vma pgoff: 0x%lx\n", start, end, vma->vm_pgoff);

	if (remap_pfn_range(vma, start, pfn_start, size, PAGE_SHARED))
		return -1;

	pr_info("%s: map 0x%lx to 0x%lx, size: 0x%lx\n", __func__, virt_start, vma->vm_start, size);

	return 0;
}

static const struct file_operations dev_fops = {
	.owner = THIS_MODULE,
	.open = my_open,
	.mmap = my_map,
};
static struct miscdevice misc = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = DEVICE_NAME,
	.fops = &dev_fops,
};

static int __init dev_init(void)
{
	int ret;
	unsigned int i;

	ret = misc_register(&misc);
	buffer = (unsigned char *)kmalloc(PAGE_SIZE * 2, GFP_KERNEL);
	SetPageReserved(virt_to_page(buffer));

	for (i = PAGE_SIZE; i < 16 + PAGE_SIZE; i++)
		buffer[i] = array[i - PAGE_SIZE] + 0x80;

	for (i = 0; i < 16; i++)
		buffer[i] = array[i];

	return ret;
}

static void __exit dev_exit(void)
{
	misc_deregister(&misc);
	ClearPageReserved(virt_to_page(buffer));
	kfree(buffer);
}

module_init(dev_init);
module_exit(dev_exit);
MODULE_LICENSE("GPL");
