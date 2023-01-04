/*
 * OLED_Driver.c
 *
 *  Created on: Jan 2, 2023
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

#include "fonts.h"
#include "ssd1306.h"
#include "test.h"
#include "bitmap.h"
#include "horse_anim.h"

int OLED_probe(struct i2c_client *client, const struct i2c_device_id *id);
int OLED_remove(struct i2c_client *client);
ssize_t OLED_read (struct file *, char __user *, size_t, loff_t *);
ssize_t OLED_write (struct file *, const char __user *, size_t, loff_t *);

struct i2c_client * OLED_I2C_Client = NULL;
static struct proc_dir_entry *ent;

struct of_device_id match_table[] =
{
		[0] =
		{
				.compatible = "OLED_SSD1306" ,
		} ,

		[1] = {}
};

struct i2c_device_id id_table[] =
{
		[0] = {.name = "OLED_SSD1306" , 0} ,
		[1] = {}
};




struct i2c_driver my_driver =
{
		.probe = OLED_probe,
		.remove = OLED_remove,
		.id_table = id_table ,
		.driver =
		{
				.name = "OLED_SSD1306" ,
				.owner = THIS_MODULE ,
				.of_match_table = match_table
		},
};


static struct file_operations fops =
{
//   .open = I2C_open,
	.owner = THIS_MODULE,
   .read = OLED_read,
   .write = OLED_write,
//   .release = I2C_release,
};

ssize_t OLED_read(struct file *File, char *user_buffer, size_t count, loff_t *offs)
{

	return count ;
}

ssize_t OLED_write (struct file *File, const char *user_buffer, size_t count, loff_t *offs)
{

	return count;
}

int OLED_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	printk("I2C_driver probed Successfully\n");
	if(client == NULL)
	{
		printk("Error I2C client\n");
		return PTR_ERR(client);
	}
	ent=proc_create("OLED_SSD1306",0666,NULL,&fops);
	OLED_I2C_Client = client;

	  SSD1306_Init();  // initialise

	  /// lets print some string

	    SSD1306_GotoXY (0,0);
	    SSD1306_Puts ("HELLO", &Font_11x18, 1);
	    SSD1306_GotoXY (10, 30);
	    SSD1306_Puts ("  WORLD :)", &Font_11x18, 1);
	    SSD1306_UpdateScreen(); //display

	return 0;
}

int OLED_remove(struct i2c_client *client)
{
	printk("device Remove\n");
	return 0;
}
static int __init OLED_Client_driver_init(void)
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

static void __exit OLED_Client_driver_exit(void )
{
	printk("Unloading the I2C driver\n");
	i2c_del_driver(&my_driver);
	proc_remove(ent);
	return;
}


module_init(OLED_Client_driver_init);
module_exit(OLED_Client_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("AhmedAzazy-Ez");
MODULE_DESCRIPTION("OLED Driver running on Beaglebone using I2C device tree client\n");
MODULE_INFO(intree , "Y");
