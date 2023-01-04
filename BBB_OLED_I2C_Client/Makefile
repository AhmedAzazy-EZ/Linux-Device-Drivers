obj-m := OLED_Driver_MOD.o
OLED_Driver_MOD-objs += OLED_Driver.o fonts.o ssd1306.o test.o
ARCH=arm
ARCH=arm
CROSS_COMPILE=arm-buildroot-linux-uclibcgnueabihf-
KERN_DIR = /home/ez/Development/buildroot-2022.02.6/output/build/linux-custom
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