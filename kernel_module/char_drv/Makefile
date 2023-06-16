#$(warning KERNELRELEASE=$(KERNELRELEASE))

#EXTRA_CFLAGS = -g -O0

ifeq ($(KERNELRELEASE),)

NPROC := $(shell nproc)
PWD   := $(shell pwd)

KERNEL_SRC      := /lib/modules/$(shell uname -r)/build
MOD_INSTALL_DIR :=

modules:
	$(MAKE) $@ \
		-j $(NPROC) \
		-C $(KERNEL_SRC) \
		M=$(PWD)

modules_install: modules
	$(MAKE) $@ \
		-j $(NPROC) \
		-C $(KERNEL_SRC) \
		M=$(PWD)

clean:
	$(MAKE) $@ \
		-j $(NPROC) \
		-C $(KERNEL_SRC) \
		M=$(PWD)

.PHONY: modules modules_install clean

else

obj-m := char_drv.o

endif
