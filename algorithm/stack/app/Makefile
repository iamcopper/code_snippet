#CROSS_COMPILE ?= aarch64-linux-gnu-
CC := gcc
RM := -rm -f
DEPDIR = .dep
ODIR = .build

EXTRAINCDIRS := ../seqstack
#VPATH = ../seqstack
vpath seqstack.c ../seqstack
#../../seqstack/setstack.o

CFLAGS = -Wall -Werror -g
CFLAGS += -MMD -MP -MF .dep/$(@F).d
CFLAGS += $(patsubst %,-I %,$(EXTRAINCDIRS))

LFLAGS :=

OUT := $(patsubst %.c,%,$(wildcard *.c))

COMMON := seqstack.o

all: $(OUT)

$(OUT): %: $(foreach var,%.o,$(addprefix $(ODIR)/,$(var)) $(addprefix $(ODIR)/,$(COMMON)))
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
