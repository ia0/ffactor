.SUFFIXES:

.PHONY: install
install:

LOCAL_ENV := env/50-local

FF_D := dst
FF_O := $(or $(O),2)
FF_R := ffactor
FF_S := src
FF_V := $(V)
FF_ENVS := env/01-first
FF_ENVS += $(LOCAL_ENV)
FF_ENVS += env/99-last

$(FF_R)/mk/common.mk:
	git submodule update --init $(FF_R)

-include $(FF_R)/mk/common.mk

$(LOCAL_ENV): script/init
	$(FF_PP) "[34m  GEN $@[m"
	$(FF_DO)mkdir -p $(dir $@)
	$(FF_DO)./$< > $@

install: $(FF_DSTS)
	$(FF_DO)sh $(FF_D)/script/install $(FF_D)

.PHONY: pull
pull:
	$(FF_DO)git pull
	$(FF_DO)git submodule update

.PHONY: test
test: ff_test

.PHONY: clean
clean: ff_clean
