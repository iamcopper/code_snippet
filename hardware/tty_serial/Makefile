CROSS_COMPILE ?= aarch64-linux-gnu-
CC := gcc
CFLAGS := -Wall -g
LFLAGS :=

out := tty_serial_demo
objs := main.o
objs += tty_serial.o

all: $(out)

$(out):$(objs)
	$(CROSS_COMPILE)$(CC) $^ -o $@ $(LFLAGS)

%.o:%.c
	$(CROSS_COMPILE)$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -rf *.o $(objs) $(out)

.PHONY: clean
