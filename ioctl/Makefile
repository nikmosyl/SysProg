
obj-m += chardev.o
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
Clean:
	make -c /lib/modules/$(shell uname -r)/build M=$(PWD) modules

