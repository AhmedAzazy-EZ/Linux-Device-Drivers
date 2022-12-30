/*
 * i2c_driver.c
 *
 *  Created on: Dec 24, 2022
 *      Author: Ahmed Azazy
 */

#include<linux/module.h>
#include <linux/moduleparam.h>
#include<linux/fs.h>
#include<linux/cdev.h>
#include<linux/device.h>
#include<linux/kdev_t.h>
#include<linux/uaccess.h>
#include <linux/platform_device.h>
#include<linux/slab.h>
#include<linux/mod_devicetable.h>
#include<linux/of.h>
#include<linux/of_device.h>
#include <linux/gpio/consumer.h>
#include <linux/i2c.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>


int i2c_probe(struct i2c_client *client, const struct i2c_device_id *id);
int i2c_remove(struct i2c_client *client);
ssize_t I2C_read (struct file *, char __user *, size_t, loff_t *);
ssize_t I2C_write (struct file *, const char __user *, size_t, loff_t *);
//int I2C_release (struct inode *, struct file *);
//int I2C_open (struct inode *, struct file *);

static struct proc_dir_entry *ent;


struct driver_prv_data {

	dev_t dev;
	int devices_count;
	struct cdev cdev;
	struct class * Pclass;
	struct device * my_device;
};


struct driver_prv_data drv_prv_data =
{
		.devices_count = 1 ,
};

struct i2c_client * STM32F4_I2C_client = NULL;

static struct file_operations fops =
{
//   .open = I2C_open,
	.owner = THIS_MODULE,
   .read = I2C_read,
   .write = I2C_write,
//   .release = I2C_release,
};

struct of_device_id match_table[] =
{
		[0] =
		{
				.compatible = "stmBBB,STM" ,
		} ,

		[1] = {}
};

struct i2c_device_id id_table[] =
{
		[0] = {.name = "stmBBB,STM" , 0} ,
		[1] = {}
};

struct i2c_driver my_driver =
{
		.probe = i2c_probe,
		.remove = i2c_remove,
		.id_table = id_table ,
		.driver =
		{
				.name = "stmBBB,STM" ,
				.owner = THIS_MODULE ,
				.of_match_table = match_table
		},
};

int i2c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	printk("I2C_driver probed Successfully\n");
	if(client == NULL)
	{
		printk("Error I2C client\n");
		return PTR_ERR(client);
	}
	ent=proc_create("stmBBB_STM",0666,NULL,&fops);
	STM32F4_I2C_client = client;
	return 0;
}


int i2c_remove(struct i2c_client *client)
{
	printk("I2C_driver removed Successfully\n");
	return 0;
}
static int __init i2c_init(void)
{
	int ret ;
	ret = i2c_add_driver(&my_driver);
	if(ret)
	{
		printk("CANT register the I2C driver\n");
		return -EINVAL;
	}
	printk("the I2C driver loaded successfully\n");
	return 0;
}

static void __exit i2c_exit(void )
{
	printk("Unloading the I2C driver\n");
	i2c_del_driver(&my_driver);
	proc_remove(ent);
	return;
}

ssize_t I2C_write (struct file *File, const char *user_buffer, size_t count, loff_t *offs)
{
	int ret;
	char * temp;
	struct i2c_msg msg;
	printk("I2c write Handler\n");
	temp = memdup_user(user_buffer, count);
	msg.addr = 0x02;
	msg.flags = 0;
	msg.len = count;
	msg.buf = temp;

	ret = i2c_transfer(STM32F4_I2C_client->adapter ,&msg,1);
//	i2c_smbus_write_byte(STM32F4_I2C_client, '1');

	kfree(temp);
	return count;
}

ssize_t I2C_read(struct file *File, char *user_buffer, size_t count, loff_t *offs)
{

	struct i2c_msg msg;
	char *temp;
	int ret;

	temp = kmalloc(count, GFP_KERNEL);

	msg.addr = 0x68;
	msg.flags = 0;
	msg.flags |= I2C_M_RD;
	msg.len = count;
	msg.buf = temp;

	ret = i2c_transfer(STM32F4_I2C_client->adapter, &msg, 1);
	if (ret >=0)
		ret = copy_to_user(user_buffer, temp, count) ? -EFAULT : count;
	kfree(temp);
	return ret;
}

module_init(i2c_init);
module_exit(i2c_exit);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("AhmedAzazy-Ez");
MODULE_DESCRIPTION("Creating an I2C Client driver to communicate with STM32F4\n");
MODULE_INFO(intree , "Y");
