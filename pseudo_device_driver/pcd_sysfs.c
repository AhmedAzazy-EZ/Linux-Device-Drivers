/*
 ============================================================================
 Name        : pcd_sysfs.c
 Author      : Ahmed Azazy
 Version     :
 Copyright   : Your copyright notice
 ============================================================================
 */

#include <linux/fs.h>
#include <linux/module.h>
#include <linux/device/class.h>
#include <linux/platform_device.h>
#include <linux/mod_devicetable.h>
#include<linux/cdev.h>
#include <linux/of.h>
#include <linux/uaccess.h>

#define SERIAL_NUMBER_LENGTH 50

int num_of_probes;

static int sysfs_probe(struct platform_device * dev);
static int sysfs_remove(struct platform_device * dev);
int pcd_driver_open (struct inode * Pinode, struct file * Pfile);
int pcd_driver_release (struct inode * Pinode, struct file * Pfile);
loff_t pcd_driver_llseek (struct file * Pfile, loff_t offset , int whence);
ssize_t pcd_driver_read (struct file * Pfile, char __user * buffer , size_t count , loff_t * pos);
ssize_t pcd_driver_write (struct file * Pfile , const char __user * buffer , size_t count , loff_t * pos);

struct file_operations fpos ;

struct pcd_driver_prv_data {

	dev_t dev ;
	struct class * pClass;

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

static int sysfs_probe(struct platform_device * dev)
	{
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

		device_create(pcd_sysfs_prv_data.pClass , NULL , dev_data->dev_num , NULL , "pcdev_sysfs_%d" , dev->id );

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

int pcd_driver_open (struct inode * Pinode, struct file * Pfile)
{
	struct pcdev_sysfs_prvdata * dev_data ;


	dev_data = (struct pcdev_sysfs_prvdata * )container_of(Pinode->i_cdev , struct pcdev_sysfs_prvdata , cdev);

	if(!dev_data)
	{
		printk("Failed to get the device data\n");
		return PTR_ERR(dev_data);
	}

	Pfile->private_data = dev_data ;
	printk("%s Device file is opened Successfully\n" , dev_data->pdata->serial_number);
	return 0 ;
}

int pcd_driver_release (struct inode * Pinode, struct file * Pfile)
{
	printk("Device released \n");
	return 0;
}
/*
If whence is SEEK_SET, the pointer is set to offset bytes.
If whence is SEEK_CUR, the pointer is set to its current location plus offset.
If whence is SEEK_END, the pointer is set to the size of the file plus offset.
*/
loff_t pcd_driver_llseek (struct file * Pfile, loff_t offset , int whence)
{
	struct pcdev_sysfs_prvdata * dev_data ;
	int max_size;
	loff_t temp;


	dev_data = (struct pcdev_sysfs_prvdata *) Pfile->private_data;
	max_size = dev_data->pdata->max_Length;

	printk("lseek requested \n");
	printk("Current value of the file position = %lld\n",Pfile->f_pos);

	switch(whence)
	{
		case SEEK_SET:
			if((offset > max_size) || (offset < 0))
				return -EINVAL;
			Pfile->f_pos = offset;
			break;
		case SEEK_CUR:
			temp = Pfile->f_pos + offset;
			if((temp > max_size) || (temp < 0))
				return -EINVAL;
			Pfile->f_pos = temp;
			break;
		case SEEK_END:
			temp = max_size + offset;
			if((temp > max_size) || (temp < 0))
				return -EINVAL;
			Pfile->f_pos = temp;
			break;
		default:
			return -EINVAL;
	}

	printk("New value of the file position = %lld\n",Pfile->f_pos);

	return Pfile->f_pos;

	return offset;
}

ssize_t pcd_driver_read (struct file * Pfile, char __user * buffer , size_t count , loff_t * pos)
{
	struct pcdev_sysfs_prvdata * dev_data ;
	ssize_t ret ;

	dev_data = (struct pcdev_sysfs_prvdata *) Pfile->private_data;
	if(!dev_data)
	{
		printk("Failed to get the device data while reading\n");
		return PTR_ERR(dev_data);
	}

	if(count + *pos > dev_data->pdata->max_Length)
	{
		printk("The file position + count exceeds the file limit\n");
		return -EFAULT;
	}

	ret = copy_to_user(buffer , dev_data->buffer+(*pos) , count);
	if(ret)
	{
		printk("Failed to read data from device %s\n" , dev_data->pdata->serial_number);
		return -EFAULT;
	}
	*pos += count;
	printk("%zu bytes are read successfully\n" , count);
	return count ;
}
ssize_t pcd_driver_write (struct file * Pfile , const char __user * buffer , size_t count , loff_t * pos)
{
	struct pcdev_sysfs_prvdata * dev_data ;
	int max_size ;
	dev_data = (struct pcdev_sysfs_prvdata *) Pfile->private_data;

	if(!dev_data)
	{

		printk("Failed to get the device data while writing\n");
		return PTR_ERR(dev_data);
	}

	max_size = dev_data->pdata->max_Length ;

	if(*pos + count > max_size )
	{
		printk("No enough space to write\n");
		return -ENOMEM;
	}

	if(copy_from_user(dev_data->buffer+*pos , buffer , count))
	{
		printk("Fault cant write data to device %s" , dev_data->pdata->serial_number);
		return -EFAULT;
	}

	*pos += count;

	printk("%zu bytes are written successfuly to dev %s\n" , count , dev_data->pdata->serial_number);
	return count ;
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


MODULE_LICENSE("GPL");
MODULE_AUTHOR("AhmedAzazy-Ez");
MODULE_DESCRIPTION("Creating pseudo devices and add custom attributes");
MODULE_INFO(intree , "Y");
