// SPDX-License-Identifier: SimPL-2.0
#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/ioport.h>
#include <linux/errno.h>
#include <linux/workqueue.h>
#include <linux/platform_device.h>
#include <linux/types.h>
#include <linux/io.h>

struct hello_priv {
	int num;
};

static ssize_t example_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	struct hello_priv *prv = dev_get_drvdata(dev);

	pr_notice("sysfs: %s\n", __func__);

	prv->num = buf[0];
	pr_notice("sysfs_write:buf=%s, prv->num=%d", buf, prv->num);

	return count;
}

static ssize_t example_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct hello_priv *prv = dev_get_drvdata(dev);

	pr_notice("sysfs: %s\n", __func__);

	buf[0] = prv->num;
	pr_notice("sysfs_read :buf=%s, prv->num=%d", buf, prv->num);

	return 1;
}

static DEVICE_ATTR(example, 0644, example_show, example_store);

static struct attribute *hello_attributes[] = {
	&dev_attr_example.attr,		//dev_attr_****.attr中间的××××必须与DEVICE_ATTR中的名称对应
	NULL
};

static const struct attribute_group hello_attr_group = {
	.attrs = hello_attributes,
};

static int hello_probe(struct platform_device *pdev)
{
	int rc;
	struct hello_priv *prv;

	pr_notice("sysfs: %s\n", __func__);

	prv = devm_kzalloc(&pdev->dev, sizeof(struct hello_priv), GFP_KERNEL);
	if (!prv)
		return -ENOMEM;

	prv->num = 2;

	platform_set_drvdata(pdev, prv);

	rc = sysfs_create_group(&pdev->dev.kobj, &hello_attr_group);
	if (rc) {
		dev_err(&pdev->dev, "failed to create hello sysfs group\n");
		goto fail;
	}

	return 0;
fail:
	return rc;
}

static int hello_remove(struct platform_device *pdev)
{
	sysfs_remove_group(&pdev->dev.kobj, &hello_attr_group);
	kobject_del(&pdev->dev.kobj);
	return 0;
}

struct platform_driver hello_driver = {
	.driver = {
		.name = "hello_dev",
	},
	.probe = hello_probe,
	.remove = hello_remove,
};

static struct platform_device *hello_device;

static int __init hello_init(void)
{
	int ret;

	pr_notice("sysfs: %s\n", __func__);

	hello_device = platform_device_alloc("hello_dev", -1);
	if (!hello_device) {
		pr_notice("sysfs: %s:%s[%d]: error", __FILE__, __func__, __LINE__);
		return -ENOMEM;
	}

	ret = platform_device_add(hello_device);

	if (ret != 0) {
		pr_notice("sysfs: %s:%s[%d]: error", __FILE__, __func__, __LINE__);
		return -ENOMEM;
	}
	ret = platform_driver_register(&hello_driver);
	return ret;
}

static void __exit hello_exit(void)
{
	pr_notice("sysfs: %s\n", __func__);
	platform_device_unregister(hello_device);
	platform_driver_unregister(&hello_driver);
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_DESCRIPTION("sysfs Driver");
MODULE_LICENSE("GPL");
