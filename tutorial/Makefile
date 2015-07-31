# Do not use old-fashioned suffix rules.
.SUFFIXES:

# Default goal is install.
.PHONY: install
install:

# Customization variables.
ENV := env
MANUAL_ENV := $(ENV)/manual.gen
AUTO_ENV := $(ENV)/auto.gen
FF_D := dst
FF_O := $(O)
FF_R := ffactor
FF_S := src
FF_V := $(V)
FF_ENVS := $(ENV)/first $(MANUAL_ENV) $(AUTO_ENV) $(ENV)/last

# Permit to call make right after cloning the config repository.
$(FF_R)/mk/common.mk:
	git submodule update --init $(FF_R)

-include $(FF_R)/mk/common.mk

# Machine-specific environment file (not synchronized or generated).
CONFIGRC := $(HOME)/.configrc

$(ENV):
	$(FF_DO)mkdir -p $@

$(MANUAL_ENV): | $(ENV)
	$(FF_PP) "[34m  LN $@[m"
	$(FF_DO)touch $(CONFIGRC)
	$(FF_DO)ln -s $(CONFIGRC) $@

$(AUTO_ENV): script/init | $(ENV)
	$(FF_PP) "[34m  SH $@[m"
	$(FF_DO)FF_R=$(FF_R) ./$< > $@

# Build all destination files and install them.
install: $(FF_DSTS)
	$(FF_DO)FF_R=$(FF_R) FF_D=$(FF_D) sh $(FF_D)/script/install

# Pull config repository and update submodules (e.g. ffactor).
.PHONY: pull
pull:
	$(FF_DO)git pull
	$(FF_DO)git submodule update

# Test whether destination files have been modified.
.PHONY: test
test: ff_test

# Clean generated files and directories.
.PHONY: clean
clean: ff_clean
	$(FF_DO)rm -f $(FF_ENVS)
