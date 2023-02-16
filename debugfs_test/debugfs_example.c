#include <linux/debugfs.h>
#include <linux/kernel.h>
#include <linux/module.h>

// This directory entry will point to `/sys/kernel/debug/example`.
static struct dentry *dir = 0;

// File `/sys/kernel/debug/example/sum` points to this variable.
static u32 sum = 0;

#define MAX_LEN 10
static unsigned char kbuf[MAX_LEN] = {0x00};

static int add_write(void *data ,u64 value)
{	
	printk(KERN_INFO "%s\n",__func__);
    	sum += value;
        return 0;
}

static int test_open(struct inode *inode, struct file *file)
{
	printk(KERN_INFO "%s\n",__func__);
	return 0;
}

static ssize_t test_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
	int ret = 0;

	printk(KERN_INFO "%s\n",__func__);

	if(*off)
		return 0;

	if (clear_user(buf, len)) {
		printk(KERN_ERR "clear error\n");
		return -EIO;
	}	

	ret = copy_to_user(buf,kbuf,sizeof(kbuf));
	if(ret){
		printk(KERN_ERR "ret = %d\n",ret);
		return -EIO;
	}

	*off += sizeof(kbuf);

	return sizeof(kbuf);
}

/*
** This fuction will be called when we write the Device file
*/
static ssize_t test_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
	int ret = 0;

	printk(KERN_INFO "%s\n",__func__);
	
	if(len > MAX_LEN){
		printk(KERN_ERR "write len %ld is too long,MAX_LEN is %d\n",len,MAX_LEN);
		return -EIO;
	}

	ret = copy_from_user(kbuf,buf,len);
	if(ret){
		printk(KERN_ERR "ret = %d\n",ret);
		return -EIO;
	}

	*off += len;

	return len;
}
// Here we define the file operations associated with
// `/sys/kernel/debug/example2s/add`.
//
// To each file in debugfs belongs a struct of type `struct file_operations`.
// The members of the struct that we are concerned with are
//     struct file_operations {
//         .read,
//         .write,
//         .open,
//         // ...
//     };
// Each member is a function pointer that is called when the file is
// read, written to, or opened.
//
// We could define the file operations by hand, but `<linux/fs.h>` provides
// a macro for this purpose.
//

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
    // Create directory `/sys/kernel/debug/example`.
    //
    // The function signature is
    //     struct dentry* debugfs_create_dir(
    //         const char *name,
    //         struct dentry *parent);
    // If parent is null, then directory is created in root of debugfs.
    //
    // See also linux-source/fs/debugfs/inode.c:debugfs_create_dir
    //
    dir = debugfs_create_dir("example", 0);
    if (!dir) {
        // Abort module load.
        printk(KERN_ALERT "debugfs_example: failed to create /sys/kernel/debug/example\n");
        return -1;
    }

    // Create file `/sys/kernel/debug/example/add`.
    //
    // The function signature is
    //     struct dentry* debugfs_create_file(
    //         const char *name,
    //         mode_t mode,
    //         struct dentry *parent,
    //         void *data,
    //         const struct file_operations *fops);
    //
    // Since no function in `add_fops` accesses the file`s data, we may
    // set `data = NULL`.
    //
    // See also linux-source/fs/debugfs/inode.c:debugfs_create_file
    //
    junk = debugfs_create_file(
            "add",
            0666,
            dir,
            NULL,
            &add_fops);
    if (!junk) {
        // Abort module load.
        printk(KERN_ALERT "debugfs_example: failed to create /sys/kernel/debug/example/add_fops\n");
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
        printk(KERN_ALERT "debugfs_example2: failed to create /sys/kernel/debug/example/test_fops\n");
        return -1;
    }

    // Create file `/sys/kernel/debug/example1/hello`.
    // Read/write operations on the file result in read/write operations on the variable `hello`.
    //
    // The function signature is
    //     struct void debugfs_create_u32(
    //         const char *name,
    //         mode_t mode,
    //         struct dentry *parent,
    //         void *data);
    //
    // See also linux-source/fs/debugfs/file.c:debugfs_create_u32
    //
    debugfs_create_u32("sum", 0444, dir, &sum);
	
    return 0;
}

// This is called when the module is removed.
static void __exit debugfs_test_exit(void)
{
    // We must manually remove the debugfs entries we created. They are not
    // automatically removed upon module removal.
    debugfs_remove_recursive(dir);
}

module_init(debugfs_test_init);
module_exit(debugfs_test_exit);
// The GPL license is required for the module to load.
// Without it, insmod reports unknown symbols in the module.
MODULE_LICENSE("GPL");
