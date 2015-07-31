Tutorial
========

This tutorial describes how to setup a repository to synchronize
slightly different files. This is not the only way to use the
*ffactor* executable but it is general enough to match most needs.

Requirements are:
- the `make`, `git`, and `sh` commands
- a shell with the usual builtins
- access to github


Initial setup
-------------

We create our repository in a folder named `config` in our home
directory:

    mkdir ~/config
    cd ~/config
    git init

We then add the *ffactor* repository as a submodule:

    git submodule add git@github.com:ia0/ffactor.git

We can now copy the tutorial skeleton:

    cp -r ffactor/tutorial/. .
    rm README.md

Here is the signification of the repository content (git-ignored files
or directories do not exist yet):
- `src/` contains the source files (factored and verbatim)
- `dst/` contains the destination files and is git-ignored (factored
  files are expanded and verbatim files are copied)
- `env/` contains the environment files (`*.gen` are git-ignored)
- `script/init` is the shell script that creates `env/auto.gen`
- `ffactor/` is the *ffactor* repository submodule
- `.config` is the *ffactor* configuration file and is git-ignored
- `build/` is the *ffactor* build directory and is git-ignored
- `bin/ffactor` is the *ffactor* executable and is git-ignored


Customization
-------------

We can now go over the untracked files and add them as we describe
them. You may also modify some of these files to suit your needs.

The first file is [src/emacs](src/emacs), which is a verbatim source
file. It is verbatim because it does not end with `.ffactor`, which is
the file extension of factored files. Verbatim files are copied from
`src` to `dst` without modifications (the directory structure is
preserved). You may now add any verbatim files you want. You most
probably want to copy and add all the files you want to synchronize:

    git add src/emacs

The next files are [src/bashrc.ffactor](src/bashrc.ffactor) and
[src/script/install.ffactor](src/script/install.ffactor), which are
factored source files, since they end with the `.ffactor` file
extension. Factored files from `src` are expanded to `dst` according
to the environment files in `env` (see the [root readme](/README.md)
for more information about factored files syntax). You may now add any
factored files you want (or switch any verbatim file to its factored
version by adding a header in the file and the `.ffactor` extension):

    git add src/bashrc.ffactor
    git add src/script/install.ffactor

The next files are [env/first](env/first) and [env/last](env/last).
They are environment files shared among all machines (see the [root
readme](/README.md) for more information about environment files
syntax). You may now add any shared environment files you want:

    git add env/first
    git add env/last

The next file is the [script/init](script/init) shell script. It is
used by `make` to generate `env/auto.gen`. This `env/auto.gen` file is
an environment file specific to the current machine. Once you modified
the shell script to your needs, you may add it:

    git add script/init

The next file is the [Makefile](Makefile). First, it defines some
customization variables. Then, it includes a file from the *ffactor*
repository that does all the job. Finally, it wraps some of the
included rules by removing the `ff_` prefix. Here are the commands
available:
- `make [install]` expands factored files and copy verbatim files from
  `src` to `dst` according to environment files in `env`, then it
  executes `dst/script/install` to install the destination files
- `make pull` synchronizes the repository
- `make test` checks whether destination files have been modified (for
  each source file, a `.gen` file remembers the last generated file,
  which may be a copy or an expansion)
- `make clean` deletes generated files and directories

See the [make readme](/mk/README.md) for more information about how to
write Makefiles for use with *ffactor*. Once you modified the Makefile
to suit your needs, you may add it:

    git add Makefile

The last file is the [.gitignore](.gitignore) file and you may add it
(after modification if necessary):

    git add .gitignore

You may now commit and push your config repository to some place (like
github):

    git commit -m'initial commit'
    git push ...

You may finally run `make` to build and install everything.


Common usage
------------

When you have access to a new machine, you may clone your repository
in your home directory and build everything:

    git clone ... ~/config
    cd ~/config
    make

If you edit your source or environment files or if you synchronize
your repository (using `make pull`), you may run `make` to build what
needs to be updated.


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

Configuration contributions for other editors are welcomed.
