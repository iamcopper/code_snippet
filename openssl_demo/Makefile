#CROSS_COMPILE ?= aarch64-linux-gnu-
CC := gcc
RM := -rm -f
DEPDIR = .dep
ODIR = .build


EXTRAINCDIRS :=

CFLAGS = -Wall -Werror -g
CFLAGS += -MMD -MP -MF .dep/$(@F).d
CFLAGS += $(patsubst %,-I %,$(EXTRAINCDIRS))

LFLAGS := -lcrypto

encrypt_decrypt-obj += encrypt_decrypt.o
encrypt-obj += encrypt.o
decrypt-obj += decrypt.o

OUT := encrypt_decrypt encrypt decrypt

all: $(OUT)

encrypt_decrypt: $(addprefix $(ODIR)/,$(encrypt_decrypt-obj))
	$(CROSS_COMPILE)$(CC) $^ -o $@ $(LFLAGS)

encrypt: $(addprefix $(ODIR)/,$(encrypt-obj))
	$(CROSS_COMPILE)$(CC) $^ -o $@ $(LFLAGS)

decrypt: $(addprefix $(ODIR)/,$(decrypt-obj))
	$(CROSS_COMPILE)$(CC) $^ -o $@ $(LFLAGS)

$(ODIR)/%.o: %.c
	@echo ">>> $<"
	@mkdir -p $(@D)
	$(CROSS_COMPILE)$(CC) $(CFLAGS) -c $< -o $@

include $(shell mkdir $(DEPDIR) 2>/dev/null) $(wildcard $(DEPDIR)/*)

distclean: clean
	$(RM) rk_rsa* rk_erm*

clean:
	$(RM) *.o
	$(RM) -r $(DEPDIR) $(ODIR) $(OUT)

.PHONY: all clean distclean
