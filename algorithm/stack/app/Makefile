CROSS_COMPILE ?=
CC := gcc
ODIR := .build

CFLAGS := -Wall \
		 -Werror

EXTRAINCDIRS +=

CFLAGS += $(patsubst %,-I %,$(EXTRAINCDIRS))
CFLAGS += -MD -MP -MF .dep/$(@F).d

LFLAGS :=

stack-obj = seqstack/seqstack.o

bracketMatch-obj += $(stack-obj)
bracketMatch-obj += bracketMatch.o

expressionConvert-obj += $(stack-obj)
expressionConvert-obj += expressionConvert.o

postfixExpression-obj += $(stack-obj)
postfixExpression-obj += postfixExpression.o

powerExpression-obj += $(stack-obj)
powerExpression-obj += powerExpression.o

OUT += bracketMatch
OUT += expressionConvert
OUT += postfixExpression
OUT += powerExpression

all: $(OUT)

bracketMatch: $(addprefix $(ODIR)/,$(bracketMatch-obj))
	$(CROSS_COMPILE)$(CC) $(CFLAGS) -o $@ $^

expressionConvert: $(addprefix $(ODIR)/,$(expressionConvert-obj))
	$(CROSS_COMPILE)$(CC) $(CFLAGS) -o $@ $^

postfixExpression: $(addprefix $(ODIR)/,$(postfixExpression-obj))
	$(CROSS_COMPILE)$(CC) $(CFLAGS) -o $@ $^

powerExpression: $(addprefix $(ODIR)/,$(powerExpression-obj))
	$(CROSS_COMPILE)$(CC) $(CFLAGS) -o $@ $^

$(ODIR)/$(stack-obj): ../seqstack/seqstack.c
	@mkdir -p $(@D)
	@echo ">>> $<"
	$(CROSS_COMPILE)$(CC) $(CFLAGS) -c $< -o $@

$(ODIR)/%.o: %.c
	@mkdir -p $(@D)
	@echo ">>> $<"
	$(CROSS_COMPILE)$(CC) $(CFLAGS) -c $< -o $@

include $(shell mkdir .dep 2>/dev/null) $(wildcard .dep/*)

clean:
	$(RM) -r $(ODIR) .dep
	$(RM) $(OUT)

.PHONY: all clean
