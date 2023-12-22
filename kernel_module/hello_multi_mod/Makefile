#$(warning KERNELRELEASE=$(KERNELRELEASE))

#EXTRA_CFLAGS = -g -O0
#EXTRA_CFLAGS += -DVAR=95

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

# mod1.ko <--- hello1.o
obj-m += mod1.o
mod1-objs := hello1.o

# mod2.ko <--- hello2.o
obj-m += mod2.o
mod2-objs := hello2.o

endif
