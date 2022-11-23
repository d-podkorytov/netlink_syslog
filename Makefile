KDIR := /lib/modules/$(shell uname -r)/build

obj-m += netlink_kernel.o

all: user netlink_kernel.c Makefile
	echo KDIR=$(KDIR)
	ls /lib/modules
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) M=$(PWD) modules && echo === netlink_kernel compiled

user: netlink_user.c Makefile
	cc netlink_user.c -o netlink_user && echo === netlink_user compiled

test:
	echo ==== Module info: `modinfo netlink_kernel.ko` =========
	sudo insmod 	netlink_kernel.ko || echo can not load module netlink_kernel.ko 
	./netlink_user
	sudo rmmod  netlink_kernel.ko
	sudo dmesg | tail	
clean:
	rm -rf *.o *.ko *.mod.* *.cmd .module* modules* Module* .*.cmd .tmp* *~ netlink_user netlink_kernel.ko
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
