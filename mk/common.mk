# ffactor expands factored files according to an environment.
# Copyright (C) 2015  ia0
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

FF_R := $(or $(FF_R),.)
FF_BIN := $(or $(FF_BIN),bin/ffactor)
FF_CONFIG := $(or $(FF_CONFIG),.config)
ifeq ($(FF_DIFF),)
ifneq ($(shell which git),)
FF_DIFF := git diff --no-index --
else
FF_DIFF := diff
endif
endif

ifneq ($(shell [ -e "$(FF_CONFIG)" ] && echo y),y)
$(eval $(shell mkdir -p $(dir $(FF_CONFIG)) && \
B=$(FF_B) C=$(FF_C) E=$(FF_E) O=$(FF_O) V=$(FF_V) $(FF_R)/script/config > $(FF_CONFIG) || \
echo '$$(error Could not make config)'))
endif

include $(FF_CONFIG)

FF_CC := $(FF_C)
FF_SRCS :=

ifeq ($(FF_C),clang)
FF_CC += -Weverything
endif
ifeq ($(FF_C),gcc)
FF_CC += -std=c11 -D_BSD_SOURCE -Wall -Wextra
endif

FF_CC += -Wno-padded
FF_CC += -Wno-disabled-macro-expansion

ifeq ($(FF_E),y)
FF_CC += -Werror
endif

ifeq ($(FF_O),0)
ifneq ($(shell which valgrind),)
FF_CC += -DFF_VALGRIND
FF_SRCS += $(FF_R)/src/ff_valgrind.c
endif
FF_CC += -g -ftrapv
else
FF_CC += -O$(FF_O) -DNDEBUG
endif

ifeq ($(FF_V),y)
FF_DO :=
FF_PP := @:
else
FF_DO := @
FF_PP := @echo
endif

FF_SRCS += $(FF_R)/src/ff_chunk.c
FF_SRCS += $(FF_R)/src/ff_cyclic.c
FF_SRCS += $(FF_R)/src/ff_keyset.c
FF_SRCS += $(FF_R)/src/ff_file.c
FF_SRCS += $(FF_R)/src/ff_parser.c
FF_SRCS += $(FF_R)/src/ff_eval.c
FF_SRCS += $(FF_R)/src/ff_engine.c
FF_SRCS += $(FF_R)/src/ffactor.c

FF_OBJS := $(FF_SRCS:$(FF_R)/src/%.c=$(FF_B)/%.o)

$(FF_BIN): $(FF_OBJS)
	$(FF_PP) "[1;32m  LD $@[m"
	$(FF_DO)mkdir -p $(dir $@)
	$(FF_DO)$(FF_CC) $^ -o $@

-include $(FF_OBJS:.o=.d)

$(FF_OBJS): $(FF_B)/%.o: $(FF_R)/src/%.c
	$(FF_PP) "[32m  CC $@[m"
	$(FF_DO)mkdir -p $(dir $@)
	$(FF_DO)$(FF_CC) -I$(FF_R)/src -MMD -c $< -o $@

FF_GET_TESTS = $(patsubst $(FF_R)/%,$(FF_B)/%/ok,$(shell find $(FF_R)/test/$(1) -mindepth 1 -maxdepth 1 -type d))

FF_SUITES := $(call FF_GET_TESTS,suite)

.PHONY: ff_suite
ff_suite: $(FF_SUITES)

$(FF_SUITES): $(FF_B)/%/ok: $(FF_BIN) $(FF_R)/%/env $(FF_R)/%/in \
		$(FF_R)/%/out $(FF_R)/%/err $(FF_R)/%/code
	$(FF_PP) "[34m  TS $*[m"
	$(FF_DO)mkdir -p $(FF_B)/$*
	$(FF_DO)$(FF_BIN) $(FF_R)/$*/env < $(FF_R)/$*/in > $(FF_B)/$*/out \
		2> $(FF_B)/$*/err; echo $$? > $(FF_B)/$*/code
	$(FF_DO)$(FF_DIFF) $(FF_R)/$*/err $(FF_B)/$*/err
	$(FF_DO)$(FF_DIFF) $(FF_R)/$*/out $(FF_B)/$*/out
	$(FF_DO)$(FF_DIFF) $(FF_R)/$*/code $(FF_B)/$*/code
	$(FF_DO)touch $@

FF_BENCHS := $(call FF_GET_TESTS,bench)
FF_BENCH_REF := $(FF_B)/test/bench/ref

$(FF_BENCH_REF): $(FF_R)/test/bench/ref.c
	$(FF_DO)mkdir -p $(dir $@)
	$(FF_DO)$(FF_CC) $< -o $@

.PHONY: ff_bench
ff_bench: $(FF_BENCHS)

$(FF_BENCHS): $(FF_B)/%/ok: $(FF_BIN) $(FF_BENCH_REF) $(FF_R)/test/bench/test.sh $(FF_R)/%/prepare.sh
	$(FF_PP) "[34m  TB $*[m"
	$(FF_DO)mkdir -p $(FF_B)/$*
	$(FF_DO)FF_B=$(FF_B)/$* FF_R=$(FF_R)/$* FF_BIN=$(FF_BIN) FF_DIFF="$(FF_DIFF)" \
		FF_BENCH_REF=$(FF_BENCH_REF) sh $(FF_R)/test/bench/test.sh
	$(FF_DO)touch $@

ifneq ($(or $(FF_D),$(FF_S)),)
ifeq ($(and $(FF_D),$(FF_S)),)
$(error FF_D and FF_S must both (or none) be defined!)
endif
FF_USRCS := $(shell find $(FF_S) -type f)
FF_FSRCS := $(filter %.ffactor,$(FF_USRCS))
FF_VSRCS := $(filter-out %.ffactor,$(FF_USRCS))
FF_FDSTS := $(patsubst $(FF_S)/%.ffactor,$(FF_D)/%,$(FF_FSRCS))
FF_VDSTS := $(patsubst $(FF_S)/%,$(FF_D)/%,$(FF_VSRCS))
$(FF_FDSTS): $(FF_D)/%: $(FF_S)/%.ffactor
$(FF_VDSTS): $(FF_D)/%: $(FF_S)/%
endif

$(FF_FDSTS): $(FF_BIN) $(FF_ENVS)
$(FF_FDSTS): FF_GEN = $(FF_BIN) $(FF_ENVS) -- $< $@.tmp && mv $@.tmp $@.gen
$(FF_FDSTS): FF_DISP = FF

$(FF_VDSTS): FF_GEN = cp $< $@.gen
$(FF_VDSTS): FF_DISP = CP

FF_DSTS := $(FF_FDSTS) $(FF_VDSTS)
FF_CHECK = [ ! -e $(1).gen -a ! -e $(1) ] || $(FF_DIFF) $(1).gen $(1)

$(FF_DSTS):
	$(FF_PP) "[34m  $(FF_DISP) $@[m"
	$(FF_DO)$(call FF_CHECK,$@)
	$(FF_DO)mkdir -p $(dir $@)
	$(FF_DO)$(FF_GEN)
	$(FF_DO)cp $@.gen $@

FF_DSTOKS := $(addsuffix .ok,$(FF_DSTS))

.PHONY: ff_test
ff_test: $(FF_DSTOKS)

.PHONY: $(FF_DSTOKS)
$(FF_DSTOKS): %.ok:
	$(FF_DO)$(call FF_CHECK,$*)

.PHONY: ff_clean
ff_clean:
	$(FF_PP) "[1;31m  CLEAN[m"
	$(FF_DO)rm -rf $(FF_BIN) $(FF_CONFIG) $(FF_DSTS) $(addsuffix .gen,$(FF_DSTS)) $(FF_B) $(FF_D)
