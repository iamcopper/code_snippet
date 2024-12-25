ifneq ($(KERNELRELEASE),)
# call from kernel build system

obj-m	:= scull.o
obj-m	:= scull_platform.o

else

KERNELDIR ?= /lib/modules/$(shell uname -r)/build
PWD       := $(shell pwd)

modules clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) $@

endif
