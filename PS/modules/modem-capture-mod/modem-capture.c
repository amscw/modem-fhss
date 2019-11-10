/*  modem-capture.c - The simplest kernel module.
 */

#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/list.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/dma-mapping.h>
#include <linux/pm_runtime.h>
#include <linux/slab.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/mm.h>
#include <linux/string.h>
#include <linux/irq.h>
#include <linux/of_irq.h>
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <linux/errno.h>
#include <linux/delay.h>
#include <linux/semaphore.h>
#include <linux/io.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/time.h>
#include <linux/ioctl.h>
#include <linux/proc_fs.h>
#include <linux/err.h>
#include <linux/kthread.h>
#include <linux/seq_file.h>

#define DRIVER_NAME "modem-capture"
#define PROCFS_NAME "modem-capture-procfs"

#define MCAP_REGCCR		0x00
#define MCAP_REGCSR		0x04
#define MCAP_REGCADDR	0x08
#define MCAP_REGCDATA	0x0C
#define MCAP_REGCLEN	0x10
#define MCAP_REGCFLAGS	0x14

#define IOC_MAGIC       1
#define IOC_MAXNR       3
#define IOC_SET_SADDR	_IOW(IOC_MAGIC, 1, unsigned int)
#define IOC_SET_CFLAGS	_IOW(IOC_MAGIC, 2, unsigned int)

static unsigned int caddr = 0;
static unsigned int cflags = 0;

static int major_number = 0;
static int minor_number = 0;

static struct class *device_class;

struct chrdrv_private {
	int device_open;
	void __iomem *io_base;
	struct device *device;
	struct resource resource;
	struct proc_dir_entry *our_proc_file;

	struct cdev cdev;
};

static __inline void chrdrv_write_reg(struct chrdrv_private *priv, const unsigned int reg_addr, const unsigned int reg_value) {
	iowrite32(reg_value, (void __iomem *)(priv->io_base + reg_addr));
}

static __inline unsigned int chrdrv_read_reg(struct chrdrv_private *priv, const unsigned int reg_addr) {
    return ioread32((void __iomem *)(priv->io_base + reg_addr));
}

static int chrdrv_open(struct inode *pinode, struct file *filp) {
	struct chrdrv_private *priv;
	priv = container_of(pinode->i_cdev, struct chrdrv_private, cdev);
	filp->private_data = priv;

	if (priv->device_open) {
		return -EBUSY;
	}
	priv->device_open++;

	return 0;
}

static ssize_t chrdrv_write(struct file *filp, const char __user *buf, size_t len, loff_t *off) {
	struct chrdrv_private *priv = (struct chrdrv_private *)filp->private_data;

	return 0;
}

static ssize_t chrdrv_read(struct file *filp, const char __user *buf, size_t len, loff_t *off) {
	struct chrdrv_private *priv = (struct chrdrv_private *)filp->private_data;

	int i;
	unsigned int *data;

	chrdrv_write_reg(priv, MCAP_REGCCR, 2); udelay(100);
	chrdrv_write_reg(priv, MCAP_REGCCR, 0);
	chrdrv_write_reg(priv, MCAP_REGCADDR, caddr);
	chrdrv_write_reg(priv, MCAP_REGCLEN, len);
	chrdrv_write_reg(priv, MCAP_REGCFLAGS, cflags);
	chrdrv_write_reg(priv, MCAP_REGCCR, 1);
	while(!(chrdrv_read_reg(priv, MCAP_REGCSR) & 1));
	chrdrv_write_reg(priv, MCAP_REGCCR, 0);
	chrdrv_write_reg(priv, MCAP_REGCSR, 1);

	data = (unsigned int *)kmalloc(len, GFP_KERNEL);
	if (data == NULL) {
		return -EFAULT;
	}

	for (i = 0; i < len/4; i++) {
		data[i] = chrdrv_read_reg(priv, MCAP_REGCDATA);
	}

	copy_to_user(buf, data, len);

	kfree(data);

	return len;
}

static int chrdrv_close(struct inode *pinode, struct file *filp) {
	struct chrdrv_private *priv = (struct chrdrv_private *)filp->private_data;
	priv->device_open--;

	return 0;
}

static long chrdrv_ioctl(struct file *filp, unsigned int cmd, unsigned long arg) {

	if (_IOC_TYPE(cmd) != IOC_MAGIC) return -ENOTTY;
	if (_IOC_NR(cmd) > IOC_MAXNR) return -ENOTTY;

	switch(cmd) {

		case IOC_SET_SADDR:
			copy_from_user(&caddr, (void*)arg, sizeof(caddr));
			break;

		case IOC_SET_CFLAGS:
			copy_from_user(&cflags, (void*)arg, sizeof(cflags));
			break;

		default :
			return -ENOTTY;
	}

	return 0;
}

static ssize_t chrdrv_read_proc(struct file *filp, char *buffer, size_t length, loff_t *offset) {
	int res = 0;
	static int finished = 0;
	struct chrdrv_private *priv = (struct chrdrv_private *)filp->private_data;

	if (finished) {
		finished = 0;
		return 0;
	}

	finished = 1;
	res = sprintf(buffer, "%s - %d\n", "major number", major_number);

	return res;
}

static int chrdrv_proc_show(struct seq_file *m, void *v) {
	char buffer[80];
	sprintf(buffer, "%s - %d\n", "major number", major_number);
	seq_printf(m, buffer);
	return 0;
}

static int chrdrv_proc_open(struct inode *inode, struct  file *file) {
	return single_open(file, chrdrv_proc_show, NULL);
}

static const struct file_operations proc_fops = {
    .owner = THIS_MODULE,
    .open = chrdrv_proc_open,
    .read = seq_read,
    .llseek = seq_lseek,
    .release = single_release,
};

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = chrdrv_open,
	.read = chrdrv_read,
	.write = chrdrv_write,
	.release = chrdrv_close,
	.unlocked_ioctl = chrdrv_ioctl,
};

static char *mydevnode(struct device *dev, umode_t *mode) {
	if(mode) {
		*mode = 0644;
	}
	return 0;
}

static int probe(struct platform_device *op) {
	int res = 0;
	dev_t devno = 0;

	struct chrdrv_private *priv = (struct chrdrv_private*)kmalloc(sizeof(struct chrdrv_private), GFP_KERNEL);
	if (!priv) {
		return -1;
	}
	memset(priv, 0, sizeof(struct chrdrv_private));

	priv->our_proc_file = proc_create(PROCFS_NAME, 0664, NULL, &proc_fops);
	if (priv->our_proc_file == NULL) {
		remove_proc_entry(PROCFS_NAME, NULL);
		return -ENOMEM;
	}

	res = alloc_chrdev_region(&devno, minor_number, 1, DRIVER_NAME);
	if (res) {
		goto out_alloc;
	}
	major_number = MAJOR(devno);

	device_class = class_create(THIS_MODULE, DRIVER_NAME);
	if (IS_ERR(device_class)) {
		goto out_chrdev;
	}
	device_class->devnode=mydevnode;

	cdev_init(&priv->cdev, &fops);
	priv->cdev.owner = THIS_MODULE;
	priv->cdev.ops = &fops;
	res = cdev_add(&priv->cdev, devno, 1);
	if (res < 0) {
		goto out_chrdev;
	}

	res = of_address_to_resource(op->dev.of_node, 0, &priv->resource);
	if (res) {
		goto out_chrdev;
	}

	res = request_mem_region(priv->resource.start, resource_size(&priv->resource), DRIVER_NAME);
	if (!res) {
		goto out_chrdev;
	}

	priv->io_base = of_iomap(op->dev.of_node, 0);
	if (!priv->io_base) {
		goto out_iomap;
	}

	priv->device = device_create(device_class, NULL, MKDEV(major_number, minor_number), NULL, DRIVER_NAME);
	if (IS_ERR(priv->device)) {
		goto out_class;
	}

	dev_set_drvdata(&op->dev, priv);

	/*Initialize parameters of private structure*/
	priv->device_open = 0;

	printk(KERN_INFO "%s: interface registered\n", DRIVER_NAME);

	return 0;

out_class:
	cdev_del(&priv->cdev);
	class_destroy(device_class);

out_iomap:
	release_mem_region(priv->resource.start, resource_size(&priv->resource));

out_chrdev:
	unregister_chrdev_region(devno, 1);

out_alloc:
	kfree(priv);

	return res;
}

static int remove(struct platform_device *op) {
	dev_t devno = 0;
	struct chrdrv_private *priv = dev_get_drvdata(&op->dev);

	devno = MKDEV(major_number, minor_number);
	unregister_chrdev_region(devno, 1);
	release_mem_region(priv->resource.start, resource_size(&priv->resource));
	cdev_del(&priv->cdev);
	device_destroy(device_class, devno);
	class_destroy(device_class);
	remove_proc_entry(PROCFS_NAME, NULL);

	kfree(priv);
	dev_set_drvdata(&op->dev, NULL);

	printk(KERN_INFO "%s: interface unregistered\n", DRIVER_NAME);

	return 0;
}

static struct of_device_id chrdrv_of_match[] = {
	{ .compatible = "xlnx,modem-rs-capture-1.0", },
	{ .compatible = "modem-capture", },
	{ },
};
MODULE_DEVICE_TABLE(of, chrdrv_of_match);

static struct platform_driver chrdrv_driver = {
	.probe = probe,
	.remove = remove,
	.driver = {
	.name = DRIVER_NAME,
	.of_match_table = of_match_ptr(chrdrv_of_match),
	},
};

static int __init chrdrv_init(void) {
	return platform_driver_register(&chrdrv_driver);
}

static void __exit chrdrv_exit(void) {
	return platform_driver_unregister(&chrdrv_driver);
}

module_init(chrdrv_init);
module_exit(chrdrv_exit);

MODULE_DESCRIPTION("Modem capture driver");
MODULE_AUTHOR("Aleksei Pashinov");
MODULE_VERSION("1.0");
MODULE_ALIAS("xillybus_core");
MODULE_LICENSE("GPL");
