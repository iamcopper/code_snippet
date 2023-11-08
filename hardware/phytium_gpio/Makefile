CROSS_COMPILE ?= aarch64-linux-gnu-
CC := gcc
CFLAGS := -Wall -g
LFLAGS :=

out := phytium_gpio

objs += main.o
objs += phytium_gpio.o
objs += devmem.o

all: $(out)

$(out):$(objs)
	$(CROSS_COMPILE)$(CC) $^ -o $@ $(LFLAGS)

%.o:%.c
	$(CROSS_COMPILE)$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -rf *.o $(objs) $(out)

.PHONY: clean
