obj-m := pcd_sysfs_f.o pcd_sysfs_devices.o
pcd_sysfs_f-objs += pcd_sysfs.o pcd_syscall.o 
ARCH=arm
CROSS_COMPILE=arm-linux-gnueabihf-
HOST_KERN_DIR = /lib/modules/$(shell uname -r)/build/

all:
	make -C $(HOST_KERN_DIR) M=$(PWD) modules
clean:
	make -C $(HOST_KERN_DIR) M=$(PWD) clean
#	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERN_DIR) M=$(PWD) clean
help:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERN_DIR) M=$(PWD) help
host:
	make -C $(HOST_KERN_DIR) M=$(PWD) modules
