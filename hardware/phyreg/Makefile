#CROSS_COMPILE ?= aarch64-linux-gnu-
CC := gcc
RM := -rm -f

CFLAGS := -Wall -Werror -g
LFLAGS :=

OUT := $(shell basename $(shell pwd))

OBJS := $(patsubst %.c,%.o,$(wildcard *.c))

all: $(OUT)

$(OUT): $(OBJS)
	$(CROSS_COMPILE)$(CC) $^ -o $@ $(LFLAGS)

%.o: %.c
	@echo ">>> $<"
	$(CROSS_COMPILE)$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) *.o $(OUT)

.PHONY: all clean
