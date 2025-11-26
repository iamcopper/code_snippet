#CROSS_COMPILE ?= aarch64-linux-gnu-
CC := gcc
RM := -rm -f
DEPDIR = .dep
ODIR = .build

EXTRAINCDIRS := intf lib

CFLAGS = -Wall -Werror -g
CFLAGS += -MMD -MP -MF .dep/$(@F).d
CFLAGS += $(patsubst %,-I %,$(EXTRAINCDIRS))

LFLAGS :=

OUT := server client

common-objs := intf/rpc_intf.o
common-objs += lib/log.o
common-objs += lib/utils.o

server-objs := src/server.o
server-objs += src/rpc_cmd.o
server-objs += $(common-objs)

client-objs := src/client.o
client-objs += $(common-objs)

all: $(OUT)

server: $(addprefix $(ODIR)/,$(server-objs))
	$(CROSS_COMPILE)$(CC) $^ -o $@ $(LFLAGS)

client: $(addprefix $(ODIR)/,$(client-objs))
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
