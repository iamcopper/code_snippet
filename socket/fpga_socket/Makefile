CROSS_COMPILE ?=
CC := gcc
ODIR := .build

CFLAGS := -Wall \
		 -Werror

EXTRAINCDIRS +=

CFLAGS += $(patsubst %,-I %,$(EXTRAINCDIRS))
CFLAGS += -MD -MP -MF .dep/$(@F).d

LFLAGS :=

fpgasocket-obj += fpga_socket.o
fpgasocket-obj += main.o

all: fpgasocket

fpgasocket: $(addprefix $(ODIR)/,$(fpgasocket-obj))
	$(CROSS_COMPILE)$(CC) $(CFLAGS) -o $@ $^

$(ODIR)/%.o: %.c
	@mkdir -p $(@D)
	@echo ">>> $<"
	$(CROSS_COMPILE)$(CC) $(CFLAGS) -c $< -o $@

include $(shell mkdir .dep 2>/dev/null) $(wildcard .dep/*)

clean:
	$(RM) -r $(ODIR) .dep
	$(RM) fpgasocket

.PHONY: all clean
