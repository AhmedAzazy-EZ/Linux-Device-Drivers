#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/kdev_t.h>
 
static int major;
static char *name = "null_driver";
 
static int null_open(struct inode *i, struct file *f)
{
printk(KERN_INFO "Calling: %s\n", __func__);
return 0;
}
 
static int null_release(struct inode *i, struct file *f)
{
printk(KERN_INFO "Calling: %s\n", __func__);
return 0;
}
 
static ssize_t null_read(struct file *f, char __user *buf, size_t len, loff_t *off)
{
printk(KERN_INFO "Calling: %s\n", __func__);
return 0;
}
 
static ssize_t null_write(struct file *f, const char __user *buf, size_t len, loff_t *off)
{
printk(KERN_INFO "Calling: %s\n", __func__);
return len;
}
 
static struct file_operations null_ops =
{
.owner = THIS_MODULE,
.open = null_open,
.release = null_release,
.read = null_read,
.write = null_write
};
 
static int __init null_init(void)
{
major = register_chrdev(0, name, &null_ops);
if (major < 0) {
printk(KERN_INFO "Failed to register driver.");
return -1;
}
 
printk(KERN_INFO "Device registered successfully.\n");
return 0;
}
 
static void __exit null_exit(void)
{
unregister_chrdev(major, name); 
printk(KERN_INFO "Device unregistered successfully.\n");
}
 
module_init(null_init);
module_exit(null_exit);
 
MODULE_AUTHOR("Narendra Kangralkar.");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Null driver");