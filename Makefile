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

.PHONY: all
all:

FF_B := $(B)
FF_C := $(C)
FF_E := $(E)
FF_O := $(O)
FF_V := $(V)
include mk/common.mk
$(info [33mB=$(FF_B) C=$(FF_C) E=$(FF_E) O=$(FF_O) V=$(FF_V)[m)

all: $(FF_BIN)

.PHONY: test
test: ff_suite ff_bench

.PHONY: clean
clean: ff_clean
