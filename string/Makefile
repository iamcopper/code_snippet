CROSS_COMPILE ?=
CC := gcc
CFLAGS := -Wall -g
LFLAGS :=

out := $(patsubst %.c,%,$(wildcard *.c))

all: $(out)

$(out):%:%.o
	$(CROSS_COMPILE)$(CC) $^ -o $@ $(LFLAGS)

%.o:%.c
	$(CROSS_COMPILE)$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -rf *.o $(out)

.PHONY: clean
