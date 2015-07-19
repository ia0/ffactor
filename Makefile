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

.SUFFIXES:

CONFIG := .config

ifneq ($(shell [ -e "$(CONFIG)" ] && echo y),y)
$(eval $(shell mkdir -p $(dir $(CONFIG)) && \
B=$(B) C=$(C) E=$(E) O=$(O) V=$(V) script/config > $(CONFIG) || \
echo '$$(error Could not make config)'))
endif

include $(CONFIG)

CC := $(C)
SRCS :=

ifeq ($(C),clang)
CC += -Weverything
endif
ifeq ($(C),gcc)
CC += -std=c11 -D_BSD_SOURCE -Wall -Wextra
endif

CC += -Wno-padded

ifeq ($(E),y)
CC += -Werror
endif

ifeq ($(O),0)
ifneq ($(shell which valgrind),)
CC += -DFF_VALGRIND
SRCS += src/ff_valgrind.c
endif
CC += -g -ftrapv
else
CC += -O$(O) -DNDEBUG
CC += -Wno-disabled-macro-expansion
endif

ifeq ($(V),y)
DO :=
PP := @:
else
DO := @
PP := @echo
endif

SRCS += src/ff_chunk.c
SRCS += src/ff_keyset.c
SRCS += src/ff_file.c
SRCS += src/ff_parser.c
SRCS += src/ff_eval.c
SRCS += src/ff_engine.c
SRCS += src/ffactor.c

OBJS := $(SRCS:src/%.c=$(B)/%.o)

BIN := $(B)/ffactor

$(BIN): $(OBJS)
	$(PP) "[1;32m  LD $@[m"
	$(DO)mkdir -p $(dir $@)
	$(DO)$(CC) $^ -o $@

-include $(OBJS:.o=.d)

$(OBJS): $(B)/%.o: src/%.c
	$(PP) "[32m  CC $@[m"
	$(DO)mkdir -p $(dir $@)
	$(DO)$(CC) $(DEFINES) -Isrc -MMD -c $< -o $@

TESTS := $(shell find test -mindepth 1 -maxdepth 1 -type d)
TESTS := $(patsubst %,$(B)/%/ok,$(TESTS))
ifneq ($(shell which git),)
DIFF := git diff --no-index --
else
DIFF := diff
endif

.PHONY: test
test: $(TESTS)

$(TESTS): %/ok: $(BIN) %/env %/in %/out.oracle %/err.oracle %/code.oracle
	$(PP) "[34m  FF $*[m"
	$(DO)$(BIN) $*/env < $*/in > $*/out 2> $*/err; echo $$? > $*/code
	$(DO)$(DIFF) $*/err.oracle $*/err
	$(DO)$(DIFF) $*/out.oracle $*/out
	$(DO)$(DIFF) $*/code.oracle $*/code
	$(DO)touch $@

$(B)/test/%: test/%
	$(DO)mkdir -p $(dir $@)
	$(DO)cp $< $@

.PHONY: clean
clean:
	$(PP) "[1;31m  RM $(B)[m"
	$(DO)rm -rf $(B)
	$(PP) "[1;31m  RM $(CONFIG)[m"
	$(DO)rm -f $(CONFIG)
