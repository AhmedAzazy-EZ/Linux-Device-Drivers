/*
 * pcd_include.h
 *
 *  Created on: Nov 25, 2022
 *      Author: Ahmed Azazy
 */

#ifndef SRC_PCD_INCLUDE_H_
#define SRC_PCD_INCLUDE_H_

/*#include <linux/fs.h>
#include <linux/module.h>
#include <linux/device/class.h>
#include <linux/platform_device.h>
#include <linux/mod_devicetable.h>
#include<linux/cdev.h>
#include <linux/of.h>
#include <linux/uaccess.h>
#include <linux/sysfs.h>
#include <linux/kernel.h>
#include <linux/gfp.h>
#include <linux/types.h>
#include<linux/slab.h>*/
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

#define SERIAL_NUMBER_LENGTH 50



int pcd_driver_open (struct inode * Pinode, struct file * Pfile);
int pcd_driver_release (struct inode * Pinode, struct file * Pfile);
loff_t pcd_driver_llseek (struct file * Pfile, loff_t offset , int whence);
ssize_t pcd_driver_read (struct file * Pfile, char __user * buffer , size_t count , loff_t * pos);
ssize_t pcd_driver_write (struct file * Pfile , const char __user * buffer , size_t count , loff_t * pos);

struct pcd_driver_prv_data {

	dev_t dev ;
	struct class * pClass;
	struct device * pcd_dev;
};

struct device_prv_data
{
	unsigned int max_Length ;
	char serial_number[SERIAL_NUMBER_LENGTH] ;
};


struct pcdev_sysfs_prvdata
{
	struct device_prv_data * pdata;
	char * buffer ;
	dev_t dev_num;
	struct cdev cdev ;
};


extern struct pcd_driver_prv_data pcd_sysfs_prv_data;
extern struct platform_driver drv;


#endif /* SRC_PCD_INCLUDE_H_ */
