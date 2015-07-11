FF_B := build
FF_E := env
FF_S := src
FF_BIN := bin/ffactor
FF_ENVS := $(shell find env -type f | sort)
FF_REPO := ..

.PHONY: all
all: ff_all

.PHONY: test
test: ff_test

.PHONY: clean
clean: ff_clean

include $(FF_REPO)/mk/common.mk
