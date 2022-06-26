#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kdev_t.h>
#include <linux/uaccess.h>



#define DEBUG 1

#define NUM_OF_DEVICES 4

#define DEVICE1_LEN	512
#define DEVICE2_LEN	1024
#define DEVICE3_LEN	512
#define DEVICE4_LEN    1024


char Dev1_buff[DEVICE1_LEN];
char Dev2_buff[DEVICE2_LEN];
char Dev3_buff[DEVICE3_LEN];
char Dev4_buff[DEVICE4_LEN];


struct pcd_dev_data
{
	char Dev_Serial[50];
	int perm;
	char *buff;
	struct cdev pcd_cdev;
	unsigned int size;
};

struct pcd_driver
{
	struct pcd_dev_data pcd_prv[NUM_OF_DEVICES];
	int num_of_devices;
	char driver_name[100];
	dev_t dev;
	struct class * pcd_drv_class;
	struct device * pcd_device;
};


struct pcd_driver My_driver = 
{
	.pcd_prv=
		{
			[0] = 
				{
					.Dev_Serial="Dev1_1234",
					.perm = O_RDONLY,
					.buff = Dev1_buff,
					.size= DEVICE1_LEN
				} , 
				
			[1] = 
				{
					.Dev_Serial="Dev2_1234",
					.perm = O_WRONLY,
					.buff = Dev2_buff,
					.size= DEVICE2_LEN
				} ,
				
			[2] = 
				{
					.Dev_Serial="Dev3_1234",
					.perm = O_RDWR,
					.buff = Dev3_buff,
					.size=DEVICE3_LEN
				} , 
				
			[3] = 
				{
					.Dev_Serial="Dev4_1234",
					.perm = O_RDWR,
					.buff = Dev4_buff,
					.size=DEVICE4_LEN
				}
		} , 
		
	.num_of_devices = 4 ,
	.driver_name = "character device driver"
};


int pcd_open (struct inode * inode , struct file * filp)
{
	printk("open function...retreiving private data");
	
	struct pcd_dev_data * pcd_prv_data = container_of(inode->i_cdev, struct pcd_dev_data , pcd_cdev);
	/*printk("Detected Serial: %s" , pcd_prv_data->Dev_Serial);*/
	printk("Detected Serial:s");

#if DEBUG==1
	return 0;
#endif
}


ssize_t pcd_read (struct file * filp, char __user * user_buff, size_t count, loff_t * f_pos)
{
	

#if DEBUG==1
	return 0;
#endif
}


ssize_t pcd_write (struct file * filp, const char __user * user_buff , size_t count, loff_t * f_pos)
{



#if DEBUG==1
	return 0;
#endif
}


loff_t pcd_llseek (struct file * filp, loff_t f_pos, int whence)
{

#if DEBUG==1
	return 0;
#endif
}

struct file_operations fops = 
	{
		.open = pcd_open,
		.read = pcd_read,
		.write = pcd_write,
		.llseek = pcd_llseek
	};

static int __init pcd_driver_init(void)
{
	
	unsigned int i;
	printk("Init the kernel Module\n");
	int cd = alloc_chrdev_region ( &My_driver.dev , 0 , NUM_OF_DEVICES , "pcd_driver" );
	if (IS_ERR(cd))
		goto alloc_failed;

	
	//creating device class in sysfs
	My_driver.pcd_drv_class = class_create(THIS_MODULE , "pesudo_character_device");
	
	
	for(i = 0 ; i < NUM_OF_DEVICES ; i++)
		{
		
			printk("init cdev of device Number %d:%d\n" , MAJOR( My_driver.dev ) , i+1);
			cdev_init(&My_driver.pcd_prv[i].pcd_cdev , &fops);
			
			cd = cdev_add(&My_driver.pcd_prv[i].pcd_cdev , My_driver.dev + i , 1);
			if (IS_ERR(cd))
				goto device_add_failed;
				
			My_driver.pcd_device = device_create(My_driver.pcd_drv_class , NULL , My_driver.dev+i , NULL , "pcd_%d" , i+1);
		}	
		


	return 0;		
alloc_failed:
	printk("Failed to allocate device region\n");
	
device_add_failed:
	printk("Failed to add device number %d:%d" , MAJOR(My_driver.dev) , i);
	
#if DEBUG==1
	return 0;
#endif	
}


static void __exit pcd_driver_exit(void)
	{
		int i = 0;
		for( i = 0 ; i < NUM_OF_DEVICES ; i++ )
			{
				device_destroy(My_driver.pcd_drv_class , My_driver.dev + i);
				cdev_del(&My_driver.pcd_prv[i].pcd_cdev);
				
			}
		class_destroy(My_driver.pcd_drv_class);
		unregister_chrdev_region(My_driver.dev, NUM_OF_DEVICES);
		printk("Goodbye Module\n");
	}


module_init(pcd_driver_init);
module_exit(pcd_driver_exit);




MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ahmed_Ez");
MODULE_DESCRIPTION("n number of pesudo character devices\n");








