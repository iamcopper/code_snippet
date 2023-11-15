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

# hello1.o ---> mod1.ko
# hello2.o ---> mod2.ko
obj-m := mod1.o
obj-m += mod2.o

mod1-y := hello1.o
mod2-y := hello2.o

endif
