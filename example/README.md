Tutorial
========

TODO

Syntax highlighting
-------------------

Since *ffactor* is designed to avoid breaking syntax highlighting, you
may ask your editor to handle the `.ffactor` extension as if it were
absent. As such, a `name.ext.ffactor` file would be syntax highlighted
as if it were `name.ext`.

If you use emacs(1), you may achieve this behavior by adding the
following line to your `.emacs` configuration file (which is most
probably the expanded version of a factored file):

    (add-to-list 'auto-mode-alist '("\\.ffactor\\'" nil t))
