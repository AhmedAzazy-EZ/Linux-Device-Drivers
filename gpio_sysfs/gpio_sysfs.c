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

struct driver_prv_data
{
	struct platform_driver *drv;
	struct class * Pclass;
	struct device ** dev;
	int number_of_child_nodes;
};

struct dev_drv_prvdata
{
	char label[20];
	struct gpio_desc * desc;
};

int gpio_sysfs_probe(struct platform_device *dev );
int gpio_sysfs_remove(struct platform_device *dev);
ssize_t label_show(struct device *dev, struct device_attribute *attr, char *buf);
ssize_t direction_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);
ssize_t direction_show(struct device *dev, struct device_attribute *attr, char *buf);
ssize_t value_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);
ssize_t value_show(struct device *dev, struct device_attribute *attr, char *buf);


static DEVICE_ATTR_RW(value);
static DEVICE_ATTR_RW(direction);
static DEVICE_ATTR_RO(label);


struct of_device_id of_match[] = {

		{.compatible = "org,bone-gpio-sysfs" } ,
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

struct attribute  * attrs[] = {

		&dev_attr_value.attr ,
		&dev_attr_direction.attr ,
		&dev_attr_label.attr ,
		NULL
};


struct attribute_group attr_grp = {
		.attrs = attrs ,
		.name = "gpio_attributes"
};

const struct attribute_group * array_of_attrs_group[] =
{
		&attr_grp ,
		NULL
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
		return PTR_ERR(parent);
	}

	platform_drv_data.number_of_child_nodes = of_get_child_count(parent);

	platform_drv_data.dev = devm_kzalloc(&dev->dev , sizeof(struct driver_prv_data *) * platform_drv_data.number_of_child_nodes , GFP_KERNEL );
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


		platform_drv_data.dev[i] = device_create_with_groups(platform_drv_data.Pclass , &dev->dev , 0 , dev_data \
				, array_of_attrs_group , dev_data->label);

		if(IS_ERR(platform_drv_data.dev[i]))
		{
			pr_err("Error while creating the device of node %d\n" , i);
			return PTR_ERR(platform_drv_data.dev[i]);
		}

		i++;
	}
	return 0;
}


int gpio_sysfs_remove(struct platform_device *dev)
{

	int i;

	dev_info(&dev->dev,"Remove called\n");

	for(i = 0 ; i < platform_drv_data.number_of_child_nodes ; i++){
		device_unregister(platform_drv_data.dev[i]);
	}
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

ssize_t value_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	ssize_t count;
	struct dev_drv_prvdata * dev_data ;

	dev_data = dev_get_drvdata(dev);

	if(!dev_data)
	{
		pr_err("Cannot get the device private data\n");
		return -EINVAL;
	}
	count = sprintf(buf , "%d" , gpiod_get_value(dev_data->desc));

	return count;
}
ssize_t value_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	long val ;
	struct dev_drv_prvdata * dev_data ;

	dev_data = dev_get_drvdata(dev);

	if(!dev_data)
	{
		pr_err("Cannot get the device private data\n");
		return -EINVAL;
	}

	if(kstrtol(buf , 10 , &val))
	{
		pr_err("Invalid input\n");
		return -EINVAL;

	}

	switch (val)
	{
	case 0:
	case 1:
		gpiod_set_value(dev_data->desc, (int )val);
		break;

	default :
		return -EINVAL;
	}

	pr_info("gpio %s value updated to %d\n" , dev_data->label , (int )val);
	return count;
}
ssize_t direction_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct dev_drv_prvdata * dev_data ;
	int dir;
	char *direction;

	dev_data = dev_get_drvdata(dev);

	if(!dev_data)
	{
		pr_err("Cannot get the device private data\n");
		return -EINVAL;
	}

	dir = gpiod_get_direction(dev_data->desc);
	if(dir < 0)
		return dir;
	/* if dir = 0 , then show "out". if dir =1 , then show "in" */
	direction = (dir == 0) ? "out":"in";

	return sprintf(buf,"%s\n",direction);

}
ssize_t direction_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	struct dev_drv_prvdata * dev_data ;
	int ret;
	dev_data = dev_get_drvdata(dev);

	if(!dev_data)
	{
		pr_err("Cannot get the device private data\n");
		return -EINVAL;
	}

	if(sysfs_streq(buf,"in") )
		ret = gpiod_direction_input(dev_data->desc);
	else if (sysfs_streq(buf,"out"))
		ret = gpiod_direction_output(dev_data->desc,0);
	else
		ret = -EINVAL;

	return ret ? : count;
}

ssize_t label_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct dev_drv_prvdata * dev_data ;

	dev_data = dev_get_drvdata(dev);

	if(!dev_data)
	{
		pr_err("Cannot get the device private data\n");
		return -EINVAL;
	}
	return sprintf(buf, "%s\n", dev_data->label);
}


module_init(gpio_driver_init);
module_exit(gpio_driver_exit);




MODULE_LICENSE("GPL");
MODULE_AUTHOR("AhmedAzazy-Ez");
MODULE_DESCRIPTION("Creating pseudo devices and add custom attributes");
MODULE_INFO(intree , "Y");
