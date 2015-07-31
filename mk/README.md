Makefile documentation
======================

A typical Makefile using ffactor has the following structure:

```
.SUFFIXES:

# Default goal.
.PHONY: install
install:

FF_D := dst
FF_S := src
FF_R := ffactor
# Customize other variables before you include mk/common.mk.
include $(FF_R)/mk/common.mk

install: $(FF_DSTS)
	$(FF_DO)FF_D=$(FF_D) sh ./$(FF_D)/script/install

# Re-export provided targets

.PHONY: test
test: ff_test

.PHONY: clean
clean: ff_clean
```

Variables you may specialize (default value in parentheses):
- `FF_B`: build directory (`build`)
- `FF_C`: compiler (`clang` or `gcc`)
- `FF_E=[yn]c`: warnings are errors (`y`)
- `FF_O=[0-3s]`: optimization level (`0`)
- `FF_R`: root of ffactor repository (`.`)
- `FF_V=[yn]`: print executed commands (`n`)
- `FF_BIN`: ffactor executable (`bin/ffactor`)
- `FF_CONFIG`: build configuration file (`.config`)
- `FF_DIFF`: diff command (`git diff --no-index --` or `diff`)

You must define environments files in `FF_ENVS` (order is preserved).

There are two mechanisms to define destination files:
- define `FF_FDSTS` (factored destination files) and `FF_VDSTS`
  (verbatim destination files)
- define `FF_D` (destination directory) and `FF_S` (source directory)

If you choose to define `FF_FDSTS` and `FF_VDSTS`, then you need for
each destination file, to give as first dependency its associated
source file:

```
FF_FDSTS := dst/foo1 dst/foo2
dst/foo1: src/foo1.ffactor
dst/foo2: src/foo2.ffactor
FF_VDSTS := dst/bar
dst/bar: src/bar
include $(FF_R)/mk/common.mk
```

If you choose to define `FF_D` and `FF_S`, then `FF_FDSTS` and
`FF_VDSTS` are built by looking for all files in `FF_S` and
considering them as source files. If a source file ends with
`.ffactor`, then it is considered as a factored source file, otherwise
it is a verbatim source file. A destination file has the same path as
its source file but rooted on `FF_D` instead of `FF_S` (*i.e.*
directory structure and file name are preserved) and with the
`.ffactor` extension removed for factored files.

In both cases, each destination file has a backup copy suffixed by
`.gen`. This file is used to test if the destination file was modified
in order to avoid loosing local changes made to a destination file
instead of its source file. Some `.tmp` files are also temporarily
generated during expansion.

Defined variables you may use:
- `FF_DO`: prefix commands (printed if `FF_V` is `y`)
- `FF_PP`: prefix messages (printed if `FF_V` is `n`)
- `FF_DSTS`: `FF_FDSTS` and `FF_VDSTS`

Rules are provided for these targets:
- `FF_BIN`: compile the executable
- `FF_DSTS`: expand factored files and copy verbatim ones

These phony rules are provided:
- `ff_clean`: remove FF_BIN, FF_CONFIG, FF_DSTS (and its `.gen`
  files), FF_B, and FF_D
- `ff_test`: test destination files for modification
- `ff_suite`: run the ffactor test suite
- `ff_bench`: run the ffactor benchmarks

The following general precautions are taken:
- all variables are prefixed with `FF_`
- all phony rules are prefixed with `ff_`
- all files are relative to a `FF_` variable
