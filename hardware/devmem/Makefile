CROSS_COMPILE ?= aarch64-linux-gnu-
CC := gcc
CFLAGS := -Wall -g
LFLAGS :=

out := gpio
objs := phytium_gpio.o

all: $(out)

$(out):$(objs)
	$(CROSS_COMPILE)$(CC) $^ -o $@ $(LFLAGS)

%.o:%.c
	$(CROSS_COMPILE)$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -rf *.o $(objs) $(out)

.PHONY: clean
