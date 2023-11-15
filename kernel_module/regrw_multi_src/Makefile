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

# Final module name: regrw.ko
obj-m   := regrw.o

regrw-y += regrw1.o
regrw-y += regrw2.o

endif
