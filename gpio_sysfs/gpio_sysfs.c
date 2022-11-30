/*
 * gpio_sysfs.c
 *
 *  Created on: Nov 30, 2022
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


int gpio_sysfs_probe(struct platform_device *dev );
int gpio_sysfs_remove(struct platform_device *dev);

ssize_t value_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	return 0;
}
ssize_t value_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	return 0;
}
ssize_t direction_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	return 0;
}
ssize_t direction_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	return 0;
}

ssize_t label_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	return 0;
}

static DEVICE_ATTR_RW(value);
static DEVICE_ATTR_RW(direction);
static DEVICE_ATTR_RO(label);

struct driver_prv_data
{
	struct platform_driver *drv;
	struct class * Pclass;
	int number_of_child_nodes;
};

struct dev_drv_prvdata
{
	char label[20];
	struct gpio_desc * desc;
};


struct of_device_id of_match[] = {

		{.compatible = "bone,gpio_sysfs" } ,
		{}
};

struct platform_driver driver =
{
	.probe = gpio_sysfs_probe ,
	.remove = gpio_sysfs_remove ,

	.driver =
	{
			.name = "bone-gpio-syfs" ,
			.of_match_table = of_match
	}
};


struct driver_prv_data platform_drv_data= {

		.drv = &driver
};

int gpio_sysfs_probe(struct platform_device *dev)
{

	struct device_node * parent ;
	struct device_node * child ;
	struct dev_drv_prvdata * dev_data;
	char * name;
	int i = 0;
	pr_info("Device probe\n");

	dev_data = NULL;

	parent = dev->dev.of_node;
	if(!parent)
	{
		pr_err("Cant get the parent node\n");
		return PTR_ERR(dev_data);
	}

	platform_drv_data.number_of_child_nodes = of_get_child_count(parent);


	for_each_child_of_node(parent , child)
	{
		dev_data = devm_kzalloc(&dev->dev , sizeof(struct dev_drv_prvdata) , GFP_KERNEL);
		if(!dev_data)
		{
			pr_err("Cant allocate memory for node number %d\n" , i);
			return -ENOMEM;
		}

		if(of_property_read_string(child, "label",(const char **) &name))
		{
			pr_err("Cant extract the label property from DT\n");
			return -EINVAL;
		}
		else
		{
			strcpy(dev_data->label , name);
			pr_info("GPIO device label is %s\n" , dev_data->label);
		}

		dev_data->desc = devm_fwnode_get_gpiod_from_child(&dev->dev, "bone", &child->fwnode , GPIOD_ASIS, dev_data->label);

		if(IS_ERR(dev_data->desc))
		{
			pr_err("Cannot get the GPIO describtor of child node number %d\n" , i);
			return -EINVAL;
		}

		if(gpiod_direction_output(dev_data->desc, 1))
		{
			pr_info("Cant set the gpio direction of node %d\n" , i);
			return -EINVAL;
		}



		i++;
	}
	return 0;
}


int gpio_sysfs_remove(struct platform_device *dev)
{

	pr_info("Device remove\n");

	return 0;
}
int __init gpio_driver_init(void)
{
	platform_drv_data.Pclass = class_create(THIS_MODULE , "bone_gpio");

	if(IS_ERR(platform_drv_data.Pclass))
	{
		pr_err("Cant create class\n");
		return PTR_ERR(platform_drv_data.Pclass);
	}

	platform_driver_register(platform_drv_data.drv);
	pr_info("The driver Module is loaded successfully\n");
	return 0;
}


void __exit gpio_driver_exit(void)
{
	platform_driver_unregister(platform_drv_data.drv);

	class_destroy(platform_drv_data.Pclass);

	pr_info("The Driver Module is unloaded successfully\n");

	return ;
}

module_init(gpio_driver_init);
module_exit(gpio_driver_exit);




MODULE_LICENSE("GPL");
MODULE_AUTHOR("AhmedAzazy-Ez");
MODULE_DESCRIPTION("Creating pseudo devices and add custom attributes");
MODULE_INFO(intree , "Y");
