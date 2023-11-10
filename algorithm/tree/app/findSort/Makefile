#CROSS_COMPILE ?= aarch64-linux-gnu-
CC := gcc
RM := -rm -f
DEPDIR = .dep
ODIR = .build

EXTRAINCDIRS := ../../bitree
#VPATH = ../../bitree
vpath bitree.c ../../bitree
#../../bitree/bitree.o

CFLAGS = -Wall -Werror -g
CFLAGS += -MMD -MP -MF .dep/$(@F).d
CFLAGS += $(patsubst %,-I %,$(EXTRAINCDIRS))

LFLAGS :=

OUT := main

OBJS := main.o
OBJS += bitree.o

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
