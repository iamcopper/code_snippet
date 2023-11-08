#CROSS_COMPILE ?= aarch64-linux-gnu-
CC := gcc
ODIR := .build

CFLAGS := -Wall \
		 -Werror

EXTRAINCDIRS += intf \
				lib

CFLAGS += $(patsubst %,-I %,$(EXTRAINCDIRS))
CFLAGS += -MD -MP -MF .dep/$(@F).d

LFLAGS :=

server-obj += src/server.o
server-obj += src/rpc_cmd.o
server-obj += intf/rpc_intf.o
server-obj += lib/log.o
server-obj += lib/utils.o

client-obj += src/client.o
client-obj += intf/rpc_intf.o
client-obj += lib/log.o
client-obj += lib/utils.o

OUT := server client

all: $(OUT)

server: $(addprefix $(ODIR)/,$(server-obj))
	$(CROSS_COMPILE)$(CC) $(CFLAGS) -o $@ $^

client: $(addprefix $(ODIR)/,$(client-obj))
	$(CROSS_COMPILE)$(CC) $(CFLAGS) -o $@ $^

$(ODIR)/%.o: %.c
	@echo ">>> $<"
	@mkdir -p $(@D)
	$(CROSS_COMPILE)$(CC) $(CFLAGS) -c $< -o $@

include $(shell mkdir .dep 2>/dev/null) $(wildcard .dep/*)

clean:
	$(RM) -r .dep $(ODIR) $(OUT)

.PHONY: all clean
