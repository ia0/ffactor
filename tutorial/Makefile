FFACTOR := ffactor
FF_V := $(V)

.PHONY: all
all: ff_all
	$(FF_DO)sh $(FF_D)/script/install $(FF_D)

.PHONY: init
init:
	$(FF_DO)sh script/init $(FF_E)

.PHONY: test
test: ff_test

.PHONY: clean
clean: ff_clean
	$(FF_DO)$(MAKE) -C $(FFACTOR) \
	CONFIG=$(abspath .config) clean

include $(FFACTOR)/mk/common.mk

$(FF_BIN): $(shell find $(FFACTOR))
	$(FF_DO)$(MAKE) -C $(FFACTOR) \
	CONFIG=$(abspath .config) \
	BIN=$(abspath $(FF_BIN)) \
	B=$(abspath build) O=2 V=$(FF_V)
