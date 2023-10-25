CROSS_COMPILE ?= aarch64-linux-gnu-
CC := gcc
CFLAGS := -Wall -g
LFLAGS :=

out := i2cdev
objs := main.o
objs += i2cbusses.o

all: $(out)

$(out):$(objs)
	$(CROSS_COMPILE)$(CC) $^ -o $@ $(LFLAGS)

%.o:%.c
	$(CROSS_COMPILE)$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -rf *.o $(objs) $(out)

.PHONY: clean all
