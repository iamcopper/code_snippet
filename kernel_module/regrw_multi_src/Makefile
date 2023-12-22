#$(warning KERNELRELEASE=$(KERNELRELEASE))

#EXTRA_CFLAGS = -g -O0

ifeq ($(KERNELRELEASE),)

NPROC := $(shell nproc)
PWD   := $(shell pwd)

KERNEL_SRC      := /lib/modules/$(shell uname -r)/build
MOD_INSTALL_DIR :=

modules modules_install clean help:
	$(MAKE) $@ \
		-j $(NPROC) \
		-C $(KERNEL_SRC) \
		M=$(PWD)

# install: modules
# custom install command, intall to MOD_INSTALL_DIR

.PHONY: modules modules_install clean help
.PHONY: install

else

# sample ref:
# https://github.com/iamcopper/linux/blob/master/drivers/net/can/spi/Makefile
# https://github.com/iamcopper/linux/blob/master/drivers/net/can/spi/mcp251xfd/Makefile

# regrw.ko <--- regrw1.o regrw2.o
obj-m += regrw.o

regrw-objs := regrw1.o
regrw-objs += regrw2.o

endif
