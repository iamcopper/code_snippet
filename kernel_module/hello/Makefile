#$(warning KERNELRELEASE=$(KERNELRELEASE))

#EXTRA_CFLAGS = -g -O0
EXTRA_CFLAGS += -DVAR1=95
EXTRA_CFLAGS += -DVAR2=121

ifeq ($(KERNELRELEASE),)

KERNELDIR ?= /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

modules:
	$(MAKE) modules \
		-C $(KERNELDIR) \
		M=$(PWD)

modules_install: modules
	$(MAKE) modules_install \
		-C $(KERNELDIR) \
		M=$(PWD)

clean:
	$(MAKE) clean \
		-C $(KERNELDIR) \
		M=$(PWD)

.PHONY: modules modules_install clean

else

obj-m := hello.o

endif
