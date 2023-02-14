CROSS_COMPILE ?=
CC := gcc
ODIR := .build

CFLAGS := -Wall \
		 -Werror

EXTRAINCDIRS +=

CFLAGS += $(patsubst %,-I %,$(EXTRAINCDIRS))
CFLAGS += -MD -MP -MF .dep/$(@F).d

LFLAGS := -lcrypto

encrypt_decrypt-obj += encrypt_decrypt.o
encrypt-obj += encrypt.o
decrypt-obj += decrypt.o

TARGET := encrypt_decrypt encrypt decrypt

all: $(TARGET)

encrypt_decrypt: $(addprefix $(ODIR)/,$(encrypt_decrypt-obj))
	$(CROSS_COMPILE)$(CC) $(CFLAGS) -o $@ $^ $(LFLAGS)

encrypt: $(addprefix $(ODIR)/,$(encrypt-obj))
	$(CROSS_COMPILE)$(CC) $(CFLAGS) -o $@ $^ $(LFLAGS)

decrypt: $(addprefix $(ODIR)/,$(decrypt-obj))
	$(CROSS_COMPILE)$(CC) $(CFLAGS) -o $@ $^ $(LFLAGS)

$(ODIR)/%.o: %.c
	@mkdir -p $(@D)
	@echo ">>> $<"
	$(CROSS_COMPILE)$(CC) $(CFLAGS) -c $< -o $@

include $(shell mkdir .dep 2>/dev/null) $(wildcard .dep/*)

distclean:clean
	$(RM) rk_rsa* rk_erm*

clean:
	$(RM) -r $(ODIR) .dep
	$(RM) $(TARGET)

.PHONY: all clean distclean
