#CROSS_COMPILE ?= aarch64-linux-gnu-
CC := gcc
RM := -rm -f
DEPDIR = .dep
ODIR = .build

EXTRAINCDIRS :=

CFLAGS = -Wall -Werror -g
CFLAGS += -MMD -MP -MF .dep/$(@F).d
CFLAGS += $(patsubst %,-I %,$(EXTRAINCDIRS))

LFLAGS :=

OUT := $(shell basename $(shell pwd))

OBJS := $(patsubst %.c,%.o,$(wildcard *.c))

all: $(OUT)

$(OUT): $(addprefix $(ODIR)/,$(OBJS))
	$(CROSS_COMPILE)$(CC) $^ -o $@ $(LFLAGS)

$(ODIR)/%.o: %.c
	@echo ">>> $<"
	@mkdir -p $(@D)
	$(CROSS_COMPILE)$(CC) $(CFLAGS) -c $< -o $@

include $(shell mkdir $(DEPDIR) 2>/dev/null) $(wildcard $(DEPDIR)/*)

clean:
	$(RM) *.o
	$(RM) -r $(DEPDIR) $(ODIR) $(OUT)

.PHONY: all clean
