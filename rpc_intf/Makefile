CROSS_COMPILE ?=
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

TARGET := server client

all: $(TARGET)

server: $(addprefix $(ODIR)/,$(server-obj))
	$(CROSS_COMPILE)$(CC) $(CFLAGS) -o $@ $^

client: $(addprefix $(ODIR)/,$(client-obj))
	$(CROSS_COMPILE)$(CC) $(CFLAGS) -o $@ $^

$(ODIR)/%.o: %.c
	@mkdir -p $(@D)
	@echo ">>> $<"
	$(CROSS_COMPILE)$(CC) $(CFLAGS) -c $< -o $@

include $(shell mkdir .dep 2>/dev/null) $(wildcard .dep/*)

clean:
	$(RM) -r $(ODIR) .dep
	$(RM) $(TARGET)

.PHONY: all clean
