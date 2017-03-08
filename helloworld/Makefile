obj-m := helloworld.o
obj-m := helloworld2.o
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
Clean:
	make -c /lib/modules/$(shell uname -r)/build M=$(PWD) modules
