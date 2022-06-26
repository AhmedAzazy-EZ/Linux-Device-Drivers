obj-m := main.o
KERN_DIR=/usr/src/linux-headers-5.13.0-51-generic


all:
	make -C $(KERN_DIR) M=$(PWD) modules

clean:
	make -C $(KERN_DIR) M=$(PWD) clean 
