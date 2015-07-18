File Factor
===========

*ffactor* expands factored files according to an environment.


Which problem is solved and how
-------------------------------

You may have an account on several machines (home, school, work,
associations) with different environments:
- distributions (ubuntu, debian, archlinux, gentoo)
- programs, versions, plugins (zsh, vim, emacs, git, screen)
- settings (internet proxy, ram, disk, cpu, multi-user)
- administrators (trusted or not)

In which case you may have to deal with slightly different
configuration files on all these machines. This leads to information
duplication and is quite annoying to synchronize.

*ffactor* simply factors your configuration files and expands them
according to a given environment. The factored files are shared among
all machines (git, svn, rsync), while the environment description is
specific to one machine and may differ from one machine to the other.
Since this environment description is small, the management of
configuration files is easier: you fetch the shared factored files and
expand them locally.

*ffactor* is written in C which makes it portable. Most machines have
a C compiler (clang, gcc) installed by default, so you don't need to
be a system administrator to build the executable.

Finally, the factorization is configurable on a per file basis. As
such, factored files do not break syntax highlighting and may be
edited almost transparently.


How to build
------------

A simple `make` will build the executable in `build/ffactor`. If you
don't have make(1), you may still compile the executable using:

    cc -std=c99 -D_BSD_SOURCE -Isrc -o ffactor \
    $(find src -name '*.c' ! -name ff_valgrind.c)

You may pass the following options to make(1) to configure the build:
- `B=<directory>`: compilation directory (default: `build`)
- `C=<compiler>`: compiler (default: first of `clang` or `gcc`)
- `E=[yn]`: warnings are errors (default: `y`)
- `O=[0-3s]`: optimization level (default: `0`)
- `V=[ny]`: commands are printed (default: `n`)

If optimization level is `0` (which is the default) and valgrind(1) is
available, then the executable will automatically bootstrap to run
under valgrind. If performance is an issue, you should use `O=2`:

    make O=2


How to use
----------

A complete setup tutorial is given in the [`example`](example) folder.
Here is a quick overview of the executable:

    ffactor <env>.. [-- <src> <dst>]

The `<env>` arguments are environment files, `<src>` is the factored
source file, and `<dst>` is the expanded destination file. If the
source and destination files are omitted, then the source file is read
from standard input and the destination file is written to standard
output.

An environment file is parsed as a sequence of lines. Each line may be
one of the following commands:
- `set <key>` sets `<key>` to true (keys are false by default)
- `error <msg>` ends the expansion and prints `<msg>`
- `- <comment>` or an empty command is ignored
- `if <cond>`, `elif <cond>`, `else`, and `endif` conditionally
  evaluates inner commands: the first branch for which `<cond>` is
  true is evaluated, while other branches are ignored

A key starts with a letter and may contain any of the following
characters: a letter, a digit, a slash, an underscore, or a dash. A
condition may be:
- `0` is false
- `1` is true
- `(not <cond>)` is true if `<cond>` is false
- `(and{ <cond>}*)` is true if all `<cond>` are true
- `(or{ <cond>}*)` is true if one `<cond>` is true

A factored source file starts with a header defining the prefix and
suffix that delimits commands (which are the same as those of
environment files). Here is the general overview of a factored file:

    PRE "ffactor" SUF PRE SUF { content PRE command SUF }* content

The executable will look for the first occurrence of `ffactor` and
deduce that the prefix is everything from the beginning of the file to
this first occurrence of `ffactor`. It will then look for the next
occurrence of the prefix and deduce that the suffix is everything from
the first occurrence of `ffactor` to the second prefix. It will
finally ensure that the second prefix is immediately followed by a
suffix. This is useful for two reasons: the prefix and suffix are
well-balanced (which helps syntax highlighting) and the probability of
a valid header accidentally occurring in a random file is reduced.

The executable will then look for the next occurrence of the prefix
followed by an occurrence of the suffix. The prefix and suffix will
delimit a command. Everything prior to the prefix occurrence is
considered as content. If no more commands are found, the rest of the
file is considered as content.

Here is an environment file sample:

    - This line is a comment.
    set true
    - Since false is not set, it evaluates to false.

Here is a factored file sample, where the prefix is `#@` and the
suffix is a line-feed:

    #@ffactor
    #@
    First line.
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

And here is associated the expanded file:

    First line.
    This gets printed.
    Last line.

These sample files can be found in the `test/readme` directory.


Alternatives
------------

- [uconf](https://github.com/rbarrois/uconf) is written in python and
  deals with file installation
