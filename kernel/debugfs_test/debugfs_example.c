// SPDX-License-Identifier: SimPL-2.0

#include <linux/debugfs.h>
#include <linux/kernel.h>
#include <linux/module.h>

static struct dentry *dir;

static u32 sum;

#define MAX_LEN 10
static unsigned char kbuf[MAX_LEN] = {0x00};

static int add_write(void *data, u64 value)
{
	sum += value;
	return 0;
}

static int test_open(struct inode *inode, struct file *file)
{
	return 0;
}

static ssize_t test_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
	int ret = 0;

	if (*off)
		return 0;

	if (clear_user(buf, len)) {
		pr_err("clear error\n");
		return -EIO;
	}

	ret = copy_to_user(buf, kbuf, sizeof(kbuf));
	if (ret) {
		pr_err("ret = %d\n", ret);
		return -EIO;
	}

	*off += sizeof(kbuf);

	return sizeof(kbuf);
}

static ssize_t test_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
	int ret = 0;

	if (len > MAX_LEN) {
		pr_err("write len %ld is too long,MAX_LEN is %d\n", len, MAX_LEN);
		return -EIO;
	}

	ret = copy_from_user(kbuf, buf, len);
	if (ret) {
		pr_err("ret = %d\n", ret);
		return -EIO;
	}

	*off += len;

	return len;
}

DEFINE_SIMPLE_ATTRIBUTE(add_fops, NULL, add_write, "%llu\n");

static const struct file_operations test_fops = {
	.owner = THIS_MODULE,
	.open = test_open,
	.read = test_read,
	.write = test_write,
};
// This is called when the module loads.
static int __init debugfs_test_init(void)
{
	struct dentry *junk;

	dir = debugfs_create_dir("example", 0);
	if (!dir) {
		// Abort module load.
		pr_alert("debugfs_example: failed to create /sys/kernel/debug/example\n");
		return -1;
	}

	junk = debugfs_create_file(
		"add",
		0666,
		dir,
		NULL,
		&add_fops);
	if (!junk) {
		// Abort module load.
		pr_alert("debugfs_example: failed to create /sys/kernel/debug/example/add_fops\n");
		return -1;
	}

	junk = debugfs_create_file(
		"test_file",
		0666,
		dir,
		NULL,
		&test_fops);

	if (!junk) {
		// Abort module load.
		pr_alert("debugfs_example2: failed to create /sys/kernel/debug/example/test_fops\n");
		return -1;
	}

	debugfs_create_u32("sum", 0444, dir, &sum);
	return 0;
}

// This is called when the module is removed.
static void __exit debugfs_test_exit(void)
{
	debugfs_remove_recursive(dir);
}

module_init(debugfs_test_init);
module_exit(debugfs_test_exit);
MODULE_LICENSE("GPL");
