Tutorial
========

This tutorial describes a setup to deal with factored files. This is
not the only way to use the executable but it is general enough to be
used as-is.

Requirements are:
- the `make` command
- the `git` command
- a shell with the usual commands and executables
- access to github

We create our repository in a folder named `config` in our home
directory:

    mkdir $HOME/config
    cd $HOME/config
    git init

We then add the *ffactor* repository:

    git submodule add git@github.com:ia0/ffactor.git

We can now copy the tutorial skeleton:

    cp -r ffactor/tutorial/. .
    rm README.md

Here is the signification of the repository content (git-ignored files
or directories do not exist yet):
- `src/` contains the source files (factored and verbatim)
- `dst/` contains the destination files and is git-ignored (factored
  files are expanded and verbatim files are copied)
- `env/` contains the environment files (`*-local` are git-ignored)
- `script/init` is the shell script that creates `env/50-local`
- `ffactor/` is the *ffactor* repository submodule
- `.config` is the *ffactor* configuration file and is git-ignored
- `build/` is the *ffactor* build directory and is git-ignored
- `bin/ffactor` is the *ffactor* executable and is git-ignored

We can now go over the untracked files and add them as we describe
them. You may also modify some of these files to suit your needs.

The first file is the [.gitignore] file. You may add it:

    git add .gitignore

The next file is [src/emacs], which is a verbatim source file. It is
verbatim because it does not end with `.ffactor`, which is the file
extension of factored files. Verbatim files are copied from `src` to
`dst` without modifications. You may now add any verbatim files you
want:

    git add src/emacs

The next files are [src/bashrc.ffactor] and
[src/script/install.ffactor], which are factored source files, since
they end with the `.ffactor` file extension. Factored files from `src`
are expanded to `dst` according to the environment files in `env`. See
the [root readme](..) for more information about factored files
syntax. You may now add any factored files you want:

    git add src/bashrc.ffactor
    git add src/script/install.ffactor

The next files are [env/01-first] and [env/99-last]. They are
environment files shared among all machines. See the [root readme](..)
for more information about environment files syntax. You may now add
any shared environment files you want:

    git add env/01-first
    git add env/99-last

The next file is the [script/init] shell script. It creates (if
non-existent) a `.configrc` file in your home directory and it creates
(if non-existent) a symlink from this `.configrc` to `env/50-local`.
This `.configrc` file is the environment file specific to the current
machine. Once you modified the shell script to your needs, you may
execute it to create both the `.configrc` file and `env/50-local`
symlink:

    git add script/init
    make init

The last file is the [Makefile]. It mainly includes a file from the
*ffactor* repository that does all the job and just wraps of its rules
by removing the `ff_` prefix. It also gives a rule to build the
*ffactor* executable. Here are the commands available:
- `make [all]` expands factored files and copy verbatim files from
  `src` to `dst` according to `env`, then it executes
  `dst/script/install` to install the destination files
- `make init` creates the local environment file with `script/init`
- `make test` checks whether destination files have been modified (for
  each source file, a `.gen` file remembers the last generated file,
  which may be a copy or an expansion)
- `make clean` deletes the destination folder

You may now add it and commit:

    git add Makefile
    git commit -m'initial commit'
    make


Syntax highlighting
-------------------

Since *ffactor* is designed to avoid breaking syntax highlighting, you
may ask your editor to handle the `.ffactor` extension as if it were
absent: a file named `foo.bar.ffactor` would thus be syntax
highlighted as if it were named `foo.bar`.

If you use [emacs](https://en.wikipedia.org/wiki/Emacs), you may
achieve this behavior by adding the following line to your `.emacs`
configuration file (which is most probably the expanded version of a
factored file):

    (add-to-list 'auto-mode-alist '("\\.ffactor\\'" nil t))

Configuration contributions for other editors are warmly welcomed.
