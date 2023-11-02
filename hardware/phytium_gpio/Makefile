CROSS_COMPILE ?=
CC := gcc
CFLAGS := -Wall -g
LFLAGS :=

out := phytium_gpio
objs += main.o
objs += phytium_gpio.o

all: $(out)

$(out):$(objs)
	$(CROSS_COMPILE)$(CC) $^ -o $@ $(LFLAGS)

%.o:%.c
	$(CROSS_COMPILE)$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -rf *.o $(objs) $(out)

.PHONY: clean
