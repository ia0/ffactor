FF_V := $(V)

.PHONY: all
$(eval $(shell [ -e ffactor ] || echo 'all: init'))
all: ff_all
	$(FF_DO)sh $(FF_D)/script/install $(FF_D)

.PHONY: init
init:
	$(FF_DO)git submodule update --init
	$(FF_DO)./script/init $(FF_E)

.PHONY: test
test: ff_test

.PHONY: clean
clean: ff_clean
	$(FF_DO)$(MAKE) -C ffactor \
	CONFIG=$(abspath .config) clean

include ffactor/mk/common.mk

$(FF_BIN): $(shell find ffactor)
	$(FF_DO)$(MAKE) -C ffactor \
	CONFIG=$(abspath .config) \
	BIN=$(abspath $(FF_BIN)) \
	B=$(abspath build) O=2 V=$(FF_V)
