/*
 ============================================================================
 Name        : pcd_sysfs.c
 Author      : Ahmed Azazy
 Version     :
 Copyright   : Your copyright notice
 ============================================================================
 */

#include "pcd_include.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("AhmedAzazy-Ez");
MODULE_DESCRIPTION("Creating pseudo devices and add custom attributes");
MODULE_INFO(intree , "Y");

static int sysfs_probe(struct platform_device * dev);
static int sysfs_remove(struct platform_device * dev);

ssize_t max_size_show(struct device *dev, struct device_attribute *attr, char *buf);
ssize_t max_size_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);
ssize_t serial_show(struct device *dev, struct device_attribute *attr, char *buf);


int num_of_probes;

struct file_operations fpos ;


/* Creating device attributes*/
static DEVICE_ATTR(max_size , S_IRUGO|S_IWUSR , max_size_show , max_size_store);
static DEVICE_ATTR(serial_num, S_IRUGO , serial_show , NULL);


struct file_operations fops = {

		.owner = THIS_MODULE ,
		.open = &pcd_driver_open ,
		.release = &pcd_driver_release ,
		.read = &pcd_driver_read ,
		.write = &pcd_driver_write ,
		.llseek = &pcd_driver_llseek
};

struct platform_device_id device_id[] =
{
	[0] = {.name = "pcd_sysfsA1" , .driver_data = 0} ,
	[1] = {.name = "pcd_sysfsB1" , .driver_data = 1} ,
	[2] = {.name = "pcd_sysfsC1" , .driver_data = 2} ,
	[3] = {.name = "pcd_sysfsD1" , .driver_data = 3} ,
	{}

};

struct platform_driver drv = {

	.id_table = device_id ,
	.probe = sysfs_probe ,
	.remove = sysfs_remove ,

	.driver = {
		.name = "pcd_sysfs_driver"
	} ,
};


struct pcd_driver_prv_data pcd_sysfs_prv_data = {

};


ssize_t max_size_show(struct device *dev, struct device_attribute *attr, char *buf)
{

	struct pcdev_sysfs_prvdata * dev_data;
	dev_data = (struct pcdev_sysfs_prvdata *)dev_get_drvdata(dev->parent);
	if(!dev_data)
	{
		printk("max_size_show, Cant get the device private data\n");
		return PTR_ERR(dev_data);
	}

	sprintf(buf , "%d" , dev_data->pdata->max_Length);

	return strlen(buf);
}


ssize_t max_size_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	struct pcdev_sysfs_prvdata * dev_data;
	long res;
	int ret;
	dev_data = (struct pcdev_sysfs_prvdata *)dev_get_drvdata(dev->parent);
	if(!dev_data)
	{
		printk("max_size_store, Cant get the device private data\n");
		return PTR_ERR(dev_data);
	}

	ret = kstrtol(buf , 10 , &res);
	if(ret)
	{
		printk("max_size_store, buffer overflow or parsing error\n");
		return ret;
	}

	dev_data->pdata->max_Length = res;

	dev_data->buffer= krealloc(dev_data->buffer , dev_data->pdata->max_Length , GFP_KERNEL);
	return count;
}

ssize_t serial_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct pcdev_sysfs_prvdata * dev_data;
	dev_data = (struct pcdev_sysfs_prvdata *)dev_get_drvdata(dev->parent);
	if(!dev_data)
	{
		printk("serial_show, Cant get the device private data\n");
		return PTR_ERR(dev_data);
	}

	sprintf(buf , "%s" , dev_data->pdata->serial_number);

	return strlen(buf);
}

int pcd_creating_sysfs_attribute(struct device * pcd_dev)
{
	int ret ;
	ret = sysfs_create_file(&pcd_dev->kobj , &dev_attr_max_size.attr  );

	if(ret)
	{
		printk("Failed to Create max_size attribute\n");
		return ret;
	}

	ret = sysfs_create_file(&pcd_dev->kobj , &dev_attr_serial_num.attr  );

	if(ret)
	{
		printk("Failed to Create serial_num attribute\n");
		return ret;
	}

	return 0;
}

static int sysfs_probe(struct platform_device * dev)
	{
		int ret ;
		struct device_prv_data * plat_dev_data ;
		struct pcdev_sysfs_prvdata * dev_data;

		//get the probed device platform data
		plat_dev_data = (struct device_prv_data *)dev_get_platdata(&dev->dev);
		if(!plat_dev_data)
		{
			printk("Failed to get the device platform data\n");
			return -EINVAL;
		}
		//allocate memory for the device data
		dev_data = devm_kzalloc(&dev->dev , sizeof(struct pcdev_sysfs_prvdata) , GFP_KERNEL);
		if(!dev_data)
		{
			printk("Failed to allocate memory for the probed device\n");
			return -ENOMEM;
		}

		dev_data->pdata = plat_dev_data;

		//now allocate meory for the device buffer
		dev_data->buffer = (char *)devm_kzalloc(&dev->dev , dev_data->pdata->max_Length , GFP_KERNEL);
		if(!dev_data->buffer)
		{
			printk("Failed to allocate memory for the device buffer\n");
			return -ENOMEM;
		}

		//calculate the number of the device
		//equals the major number that returned from alloc_chrdev_region()
		//plus the id of the probed device
		dev_data->dev_num = pcd_sysfs_prv_data.dev + dev->id;

		dev_set_drvdata(&dev->dev, dev_data);

		cdev_init(&dev_data->cdev , &fops);

		cdev_add(&dev_data->cdev , dev_data->dev_num , 1);

		pcd_sysfs_prv_data.pcd_dev = device_create(pcd_sysfs_prv_data.pClass , &dev->dev , dev_data->dev_num , NULL , "pcdev_sysfs_%d" , dev->id );

		ret = pcd_creating_sysfs_attribute(pcd_sysfs_prv_data.pcd_dev);

		if(ret)
		{
			printk("Destroying the device\n");
			device_destroy(pcd_sysfs_prv_data.pClass , dev_data->dev_num);
			return ret;
		}
		printk("Device %s is added successfully\n" , dev_data->pdata->serial_number);

		return 0;
	}


static int sysfs_remove(struct platform_device * dev)
	{
		//get the device data
		struct pcdev_sysfs_prvdata * dev_data = dev_get_drvdata(&dev->dev);

		//destory the device
		device_destroy(pcd_sysfs_prv_data.pClass,dev_data->dev_num);

		cdev_del(&dev_data->cdev);

		printk("Device %s is removed Successfully\n" , dev_data->pdata->serial_number);
		return 0;
	}



static int __init pcd_sysfs_init(void)
{
	int ret ;
	printk("Yooo Loading the module\n");
	ret = alloc_chrdev_region(&pcd_sysfs_prv_data.dev , 0 , 4 , "pcd_sysfs");
	if(ret < 0 )
	{
		printk("Failed to allocate device numbers\n");
		return ret;
	}

	pcd_sysfs_prv_data.pClass = class_create(THIS_MODULE , "pcd_sysfs_class");
	if(IS_ERR(pcd_sysfs_prv_data.pClass))
	{
		printk("Failed to create class for sysfs\n");
		unregister_chrdev_region(pcd_sysfs_prv_data.dev ,  4);
		return PTR_ERR(pcd_sysfs_prv_data.pClass) ;
	}


	platform_driver_register(&drv);
	return 0;

}


static void __exit pcd_sysfs_exit(void)
{

	printk("Removing the module\n");
	platform_driver_unregister(&drv);

	class_destroy(pcd_sysfs_prv_data.pClass);

	unregister_chrdev_region(pcd_sysfs_prv_data.dev ,  4);

	return;
}


module_init(pcd_sysfs_init);
module_exit(pcd_sysfs_exit);

