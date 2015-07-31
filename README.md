ffactor
=======

*ffactor* expands factored files according to an environment.


Problem solved
--------------

You may have an account on several machines: at home, at school, at
work, or in associations. These machines may have different
distributions (ubuntu, debian, archlinux, or gentoo), different
programs, versions, or plugins (zsh, vim, emacs, git, or screen),
and different settings (internet proxy, ram, disk, or cpu).

If this is the case, then you may have to deal with slightly different
configuration files on all these machines. This leads to information
duplication and may be quite annoying to synchronize.

*ffactor* simply factors your configuration files and expands them
according to a given environment. The factored files are shared among
all machines (with git, svn, or rsync for example), while the
environment description is specific to one machine and may differ from
one machine to another. Since the environment description is small,
the management of configuration files is easier: you briefly describe
the local environment, fetch the shared factored files, and expand
them locally according to the described environment.

*ffactor* is written in C which makes it portable. Most machines have
a C compiler installed by default, so you don't need to be a system
administrator to build the executable.

Finally, the factorization is configurable on a per file basis. As
such, factored files do not break syntax highlighting and may be
edited almost transparently.


How to build
------------

A simple `make` will build the executable in `bin/ffactor`. If you
don't have [make](https://en.wikipedia.org/wiki/Make_%28software%29),
you may still compile the executable using:

    cc -std=c99 -D_BSD_SOURCE -Isrc -o ffactor \
      $(find src -name 'ff*.c' ! -name ff_valgrind.c)

You may pass the following options to make to configure the build:
- `B=<directory>`: compilation directory (default: `B=build`)
- `C=<compiler>`: compiler (default: `C=clang` or `C=gcc`)
- `E=[yn]`: whether warnings are errors (default: `E=y`)
- `O=[0-3s]`: optimization level (default: `O=0`)
- `V=[ny]`: whether commands are printed (default: `V=n`)

If optimization level is `0` (which is the default) and
[valgrind](https://en.wikipedia.org/wiki/Valgrind) is available, then
the executable will automatically bootstrap to run under valgrind. If
performance is an issue, you may use `O=2`:

    make O=2


How to use
----------

A complete tutorial to get started using *ffactor* is given in the
[tutorial](tutorial) folder. This section describe how to use the
executable and write environment and factored files:

    ffactor <env>.. [-- <src> <dst>]

The zero or more `<env>` arguments are environment files, `<src>` is
the factored source file, and `<dst>` is the expanded destination
file. If the source and destination files are omitted, then the source
file is read from standard input and the destination file is written
to standard output. First, environment files are read sequentially
from first to last and incrementally build the environment. Then, the
source file is expanded to the destination file according to the
environment.

### Environment files

An environment file is parsed as a sequence of lines. Each line may be
one of the following commands:
- `set <key>` sets `<key>` to true (keys are false by default)
- `error <msg>` ends the expansion and prints `<msg>` to standard
  error output
- `- <comment>` or an empty line is ignored (useful to help reading)
- `if <cond>`, `elif <cond>`, `else`, and `endif` conditionally
  evaluates nested commands: the first branch for which `<cond>` is
  true is evaluated, while other branches are ignored
- ` <command>` behaves as `<command>`, *i.e.* leading spaces are
  ignored (useful help reading of nested commands)

A key `<key>` starts with a letter and may contain any of the
following characters: a letter, a digit, a slash, an underscore, or a
dash. A condition `<cond>` may be:
- `0` for false
- `1` for true
- `(not <cond>)` for true if `<cond>` is false
- `(and{ <cond>}*)` for true if all `<cond>` are true
- `(or{ <cond>}*)` for true if one `<cond>` is true

### Factored source files

A factored source file contains a header near the beginning of the
file defining the prefix and suffix that delimit commands (which are
the same as those of environment files). Usually, prefix and suffix
are chosen on a per-file basis such that anything between them is
ignored by syntax highlighting and has no semantic signification for
the given file. Here is the general overview of a factored file
(spaces and newlines have no meaning and are used to help reading):

    content
    PREFIX "ffactor" PREFIX_LENGTH SUFFIX
    PREFIX SUFFIX
    { content PREFIX command SUFFIX }*
    content

The executable looks for the first occurrence of `ffactor` and
interprets the next character as the prefix length. The prefix length
must be a digit between 1 and 8 (both inclusive). The executable then
deduces the prefix and everything from the beginning of the file to
the prefix is copied verbatim to the destination. It then looks for
the next occurrence of the prefix and deduce that the suffix is
everything from the prefix length digit to the second prefix. It
finally ensures that the second prefix is immediately followed by a
suffix. This is useful for two reasons: the prefix and suffix are
well-balanced (which helps syntax highlighting) and the probability of
a valid header accidentally occurring in a random file is reduced.

Both the prefix and suffix must be non-empty and smaller than 9 bytes.
The fact that some content is allowed before the first prefix permits
for [shebang](https://en.wikipedia.org/wiki/Shebang_%28Unix%29)
scripts to be correctly syntax highlighted.

Now that the header is found, the executable repeatedly looks for the
next occurrence of the prefix followed by an occurrence of the suffix.
The prefix and suffix delimit a command. Everything prior to the
prefix is considered as content and copied verbatim. Finally, if no
more commands are found, the rest of the file is considered as content
and copied verbatim.

### Sample files

Here is an environment file sample:

    - This line is a comment.
    set true
    - Since false is not set, it evaluates to false.

Here is a factored file sample, where the prefix is `#@` and the
suffix is a line-feed:

    First line.
    #@ffactor2
    #@
    Second line.
    #@if (and true (not false) 1 (not 0))
    This gets printed.
    #@ if (or false (not true) 0 (not 1))
    #@- Indentation is only a visual help.
    This doesn't get printed.
    #@ endif
    #@else
    #@error Unreachable!
    #@endif
    Last line.

And here is the associated expanded file:

    First line.
    Second line.
    This gets printed.
    Last line.

These sample files can be found in the
[test/suite/readme](test/suite/readme) folder:
[env](test/suite/readme/env), [src](test/suite/readme/in), and
[dst](test/suite/readme/out).


Alternatives
------------

- [UConf](https://github.com/rbarrois/uconf) is written in python and
  deals with file installation

Links to other similar projects are welcomed.
