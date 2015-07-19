all:

LOCAL_ENV := env/50-local
FF_ENVS := env/01-first
FF_ENVS += $(LOCAL_ENV)
FF_ENVS += env/99-last
FF_V := $(V)
O := 2

include ffactor/mk/common.mk

$(FF_BIN): ffactor/Makefile
$(FF_BIN): $(shell find ffactor/src -type f 2>/dev/null)
	$(FF_DO)[ -e ffactor ] || git submodule update --init
	$(FF_DO)$(MAKE) -C ffactor \
	CONFIG=$(abspath .config) \
	BIN=$(abspath $(FF_BIN)) \
	B=$(abspath build) O=$(O) V=$(FF_V)

$(LOCAL_ENV): script/init
	$(FF_PP) "[34m  GEN $@[m"
	$(FF_DO)mkdir -p $(dir $@)
	$(FF_DO)./$< > $@

.PHONY: all
all: ff_all
	$(FF_DO)sh $(FF_D)/script/install $(FF_D)

.PHONY: pull
pull:
	$(FF_DO)git pull
	$(FF_DO)git submodule update --init

.PHONY: test
test: ff_test

.PHONY: clean
clean: ff_clean
	$(FF_DO)$(MAKE) -C ffactor \
	CONFIG=$(abspath .config) clean
