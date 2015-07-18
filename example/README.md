Tutorial
========

TODO

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
