obj-m := pcd_platform_driver_dt.o
ARCH=arm
CROSS_COMPILE=arm-buildroot-linux-uclibcgnueabihf-
KERN_DIR = /home/ez/mine/buildroot-2022.02.4/output/build/linux-custom
HOST_KERN_DIR = /lib/modules/$(shell uname -r)/build/

all:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERN_DIR) M=$(PWD) modules
clean:
	make -C $(HOST_KERN_DIR) M=$(PWD) clean
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERN_DIR) M=$(PWD) clean
help:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERN_DIR) M=$(PWD) help
host:
	make -C $(HOST_KERN_DIR) M=$(PWD) modules

make-dtb:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERN_DIR) am335x-boneblack.dtb
copy-dtb:
	scp /home/ez/mine/buildroot-2022.02.4/output/build/linux-custom/arch/arm/boot/dts/am335x-boneblack.dtb root@192.168.1.6:/mine/FileTF/

copy-drv:
	scp *.ko  root@192.168.1.6:/mine/FileTF/
	
	
#export PATH=/home/ez/mine/buildroot-2022.02.4/output/host/bin:$PATH
