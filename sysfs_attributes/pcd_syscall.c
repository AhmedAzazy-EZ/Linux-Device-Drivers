/*
 * pcd_syscall.c
 *
 *  Created on: Nov 25, 2022
 *      Author: Ahmed Azazy
 */


#include "pcd_include.h"

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

