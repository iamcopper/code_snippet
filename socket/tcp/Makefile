CROSS_COMPILE ?= arm-unknown-linux-uclibcgnueabi-
CC := gcc
ODIR := .build

CFLAGS := -Wall \
		 -Werror

EXTRAINCDIRS += intf \
				lib

CFLAGS += $(patsubst %,-I %,$(EXTRAINCDIRS))
CFLAGS += -MD -MP -MF .dep/$(@F).d

LFLAGS :=

server_blocked-obj += server_blocked.o
server_select-obj += server_select.o
server_epoll-obj  += server_epoll.o
client-obj += client.o

TARGET := server_blocked server_select server_epoll client

all: $(TARGET)

server_blocked: $(addprefix $(ODIR)/,$(server_blocked-obj))
	$(CROSS_COMPILE)$(CC) $(CFLAGS) -o $@ $^

server_select: $(addprefix $(ODIR)/,$(server_select-obj))
	$(CROSS_COMPILE)$(CC) $(CFLAGS) -o $@ $^

server_epoll: $(addprefix $(ODIR)/,$(server_epoll-obj))
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
