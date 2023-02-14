CROSS_COMPILE ?=
CC := gcc
ODIR := .build

CFLAGS := -Wall \
		 -Werror

EXTRAINCDIRS +=

CFLAGS += $(patsubst %,-I %,$(EXTRAINCDIRS))
CFLAGS += -MD -MP -MF .dep/$(@F).d

LFLAGS :=

udp_server-obj += udp/server.o
udp_client-obj += udp/client.o
udp_broadcast-obj += udp/broadcast.o

raw_server-obj += raw/server.o
raw_client-obj += raw/client.o

UDPOUT := udp/server udp/client udp/broadcast
RAWOUT := raw/server raw/client

all: udp raw

udp: $(UDPOUT)

raw: $(RAWOUT)

udp/server: $(addprefix $(ODIR)/,$(udp_server-obj))
	@mkdir -p $(@D)
	$(CROSS_COMPILE)$(CC) $(CFLAGS) -o $@ $^

udp/client: $(addprefix $(ODIR)/,$(udp_client-obj))
	@mkdir -p $(@D)
	$(CROSS_COMPILE)$(CC) $(CFLAGS) -o $@ $^

udp/broadcast: $(addprefix $(ODIR)/,$(udp_broadcast-obj))
	@mkdir -p $(@D)
	$(CROSS_COMPILE)$(CC) $(CFLAGS) -o $@ $^

raw/server: $(addprefix $(ODIR)/,$(raw_server-obj))
	@mkdir -p $(@D)
	$(CROSS_COMPILE)$(CC) $(CFLAGS) -o $@ $^

raw/client: $(addprefix $(ODIR)/,$(raw_client-obj))
	@mkdir -p $(@D)
	$(CROSS_COMPILE)$(CC) $(CFLAGS) -o $@ $^

$(ODIR)/%.o: %.c
	@mkdir -p $(@D)
	@echo ">>> $<"
	$(CROSS_COMPILE)$(CC) $(CFLAGS) -c $< -o $@

include $(shell mkdir .dep 2>/dev/null) $(wildcard .dep/*)

clean:
	$(RM) -r $(ODIR) .dep
	$(RM) $(UDPOUT) $(RAWOUT)

.PHONY: all clean
