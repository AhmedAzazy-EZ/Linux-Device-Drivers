#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/kdev_t.h>
 
static int major;
static char *name = "full_driver";
 
static int full_open(struct inode *i, struct file *f)
{
return 0;
}
 
static int full_release(struct inode *i, struct file *f)
{
return 0;
}
 
static ssize_t full_read(struct file *f, char __user *buf, size_t len, loff_t *off)
{
return 0;
}
 
static ssize_t full_write(struct file *f, const char __user *buf, size_t len, loff_t *off)
{
return -ENOSPC;
}
 
static struct file_operations full_ops =
{
.owner = THIS_MODULE,
.open = full_open,
.release = full_release,
.read = full_read,
.write = full_write
};
 
static int __init full_init(void)
{
major = register_chrdev(0, name, &full_ops);
if (major < 0) {
printk(KERN_INFO "Failed to register driver.");
return -1;
}
 
return 0;
}
 
static void __exit full_exit(void)
{
unregister_chrdev(major, name); 
}
 
module_init(full_init);
module_exit(full_exit);
 
MODULE_AUTHOR("Narendra Kangralkar.");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Full driver");