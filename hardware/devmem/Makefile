CROSS_COMPILE ?=
CC := gcc
CFLAGS := -Wall -g
LFLAGS :=

out := devmem

objs += main.o
objs += devmem.o

all: $(out)

$(out):$(objs)
	$(CROSS_COMPILE)$(CC) $^ -o $@ $(LFLAGS)

%.o:%.c
	$(CROSS_COMPILE)$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -rf *.o $(objs) $(out)

.PHONY: clean
