/*
 * i2c_driver.c
 *
 *  Created on: Dec 24, 2022
 *      Author: Ahmed Azazy
 */

#include<linux/module.h>
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

int i2c_probe(struct i2c_client *client, const struct i2c_device_id *id);
int i2c_remove(struct i2c_client *client);

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
	return;
}


module_init(i2c_init);
module_exit(i2c_exit);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("AhmedAzazy-Ez");
MODULE_DESCRIPTION("Creating an I2C Client driver to communicate with STM32F4\n");
MODULE_INFO(intree , "Y");
