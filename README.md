FFactor
=======

FFactor is a tool to manage factored files. It permits to develop
these files according to an environment.

Build
-----

    make [options]

Options:
- `B=<dir>` compiles in directory `<dir>`
- `C=<compiler>` compiles with `<compiler>`
- `E=[ny]` considers warnings as errors
- `O=[0-3s]` compiles with optimization
- `V=[ny]` print executed commands

Howto
-----

    ffactor <env>.. [-- <src> <dst>]

See the example folder for more details.
