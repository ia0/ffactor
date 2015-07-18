# FFactor processes factored file into developed files.
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

# HOWTO
#
# All variables are prefixed with 'FF_' and all phony rules are
# prefixed with 'ff_'.
#
# The following variables must be defined:
# FF_B: build directory
# FF_E: environment directory
# FF_S: source directory
# FF_BIN: ffactor executable
# FF_ENVS: environment files
# FF_REPO: ffactor repository
#
# The following variables may be overwritten:
# FF_O: optimization (2, 0, 1, 3, or s)
# FF_V: verbosity (n or y)
# FF_DIFF: diff command
#
# The following phony rules are provided:
# ff_all: expand all factor files and copy all verbatim files
# ff_test: test all generated files for potential modification
# ff_clean: clean build directory (but leave ffactor executable)

FF_O := $(or $(FF_O),2)
FF_V := $(or $(FF_V),n)
FF_DIFF := $(or $(FF_DIFF),diff)

ifeq ($(FF_V),y)
FF_DO :=
FF_PP := @:
else
FF_DO := @
FF_PP := @echo
endif

FF_SRCS := $(shell find $(FF_S) -type f)
FF_FSRCS := $(filter %.ffactor,$(FF_SRCS))
FF_VSRCS := $(filter-out %.ffactor,$(FF_SRCS))
FF_FDSTS := $(patsubst $(FF_S)/%.ffactor,$(FF_B)/%,$(FF_FSRCS))
FF_VDSTS := $(patsubst $(FF_S)/%,$(FF_B)/%,$(FF_VSRCS))
FF_DSTS := $(FF_FDSTS) $(FF_VDSTS)

FF_CHECK_GEN = [ ! -e $(1).gen -o ! -e $(1) ] || $(FF_DIFF) $(1).gen $(1)

.PHONY: ff_all
ff_all: $(FF_DSTS)

$(FF_FDSTS): $(FF_B)/%: $(FF_S)/%.ffactor $(FF_BIN) $(FF_ENVS)
$(FF_FDSTS): FF_BUILD_GEN = $(FF_BIN) $(FF_ENVS) -- $< $@.tmp && mv $@.tmp $@.gen
$(FF_FDSTS): FF_BUILD_DISP = FF

$(FF_VDSTS): $(FF_B)/%: $(FF_S)/%
$(FF_VDSTS): FF_BUILD_GEN = cp $< $@.gen
$(FF_VDSTS): FF_BUILD_DISP = CP

$(FF_DSTS):
	$(FF_PP) "[34m  $(FF_BUILD_DISP) $@[m"
	$(FF_DO)$(call FF_CHECK_GEN,$@)
	$(FF_DO)mkdir -p $(dir $@)
	$(FF_DO)$(FF_BUILD_GEN)
	$(FF_DO)cp $@.gen $@

$(FF_BIN):
	$(FF_DO)$(MAKE) -C $(FF_REPO) \
	O=$(FF_O) V=$(FF_V) BIN=$(PWD)/$(FF_BIN) \
	CONFIG=$(PWD)/$(FF_B)/ffactor/.config B=$(PWD)/$(FF_B)/ffactor

FF_DSTOKS := $(addsuffix .ok,$(FF_DSTS))

.PHONY: ff_test
ff_test: $(FF_DSTOKS)

.PHONY: $(FF_DSTOKS)
$(FF_DSTOKS): %.ok:
	$(FF_DO)$(call FF_CHECK_GEN,$*)

.PHONY: ff_clean
ff_clean:
	$(FF_PP) "[1;31m  RM build[m"
	$(FF_DO)rm -rf $(FF_B)
