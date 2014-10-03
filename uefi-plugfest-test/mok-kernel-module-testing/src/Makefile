# Lee, Chun-Yi, for MOK testing 

TARGET = moktest

ifneq ($(KERNELRELEASE),)
	obj-m	:= $(TARGET).o
else
	KDIR	:= /lib/modules/$(shell uname -r)/build
	PWD	:= $(shell pwd)
default:
	$(MAKE) -C $(KDIR) M=$(PWD)
endif

clean:
	rm -rf *.ko *.o .moktest.* moktest.mod.* Module.symvers .tmp_versions
