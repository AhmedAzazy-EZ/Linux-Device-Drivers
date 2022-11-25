/*
 * pcd_sysfs_devices.c
 *
 *  Created on: Nov 20, 2022
 *      Author: Ahmed Azazy
 */



#include <linux/fs.h>
#include <linux/module.h>
#include <linux/device/class.h>
#include <linux/platform_device.h>
#include <linux/mod_devicetable.h>

#define SERIAL_NUMBER_LENGTH 50

void pcdev_release(struct device *dev)
{
	printk("Device released \n");
}

struct device_prv_data
{
	unsigned int max_Length ;
	char serial_number[SERIAL_NUMBER_LENGTH] ;
};

struct device_prv_data dev1 = {
		.max_Length = 500 ,
		.serial_number = "pcd_sysfsA1"
};


struct device_prv_data dev2 = {
		.max_Length = 450 ,
		.serial_number = "pcd_sysfsB1"
};

struct device_prv_data dev3 = {
		.max_Length = 310 ,
		.serial_number = "pcd_sysfsC1"
};

struct device_prv_data dev4 = {
		.max_Length = 250 ,
		.serial_number = "pcd_sysfsD1"
};

struct platform_device pf_dev1 = {
		.name = "pcd_sysfsA1" ,
		.id = 0 ,
		.dev = {

				.platform_data = &dev1 ,
				.release = pcdev_release
		}

};


struct platform_device pf_dev2 = {

		.name = "pcd_sysfsB1" ,
		.id = 1 ,
		.dev = {

				.platform_data = &dev2 ,
				.release = pcdev_release
		}
	};

struct platform_device pf_dev3 = {

		.name = "pcd_sysfsC1" ,
		.id = 2 ,
		.dev = {

				.platform_data = &dev3 ,
				.release = pcdev_release
		}
	} ;

struct platform_device pf_dev4 =  {
		.name = "pcd_sysfsD1" ,
		.id = 3 ,
		.dev = {

				.platform_data = &dev4 ,
				.release = pcdev_release
		}
	};

struct platform_device * pf_devices[] =
{
		&pf_dev1 ,
		&pf_dev2 ,
		&pf_dev3 ,
		&pf_dev4
};
static int __init pcd_sysfs_devices_init(void )
{

	platform_add_devices(pf_devices , 4);
	printk("Devices are loaded successfully\n");
	return 0;
}


static void __exit pcd_sysfs_devices_exit(void )
{
	platform_device_unregister(&pf_dev1);
	platform_device_unregister(&pf_dev2);
	platform_device_unregister(&pf_dev3);
	platform_device_unregister(&pf_dev4);
	printk("all the devices has unregistered successfully\n");
	return ;
}

module_init(pcd_sysfs_devices_init);
module_exit(pcd_sysfs_devices_exit);




MODULE_LICENSE("GPL");
MODULE_AUTHOR("AhmedAzazy-Ez");
MODULE_DESCRIPTION("Creating pseudo devices and add custom attributes");
MODULE_INFO(intree , "Y");
