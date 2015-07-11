Technical documentation
=======================

Grammar
-------

```
BEGIN MAGIC
  file ::= header content(CONTENT)
  header ::= PRE=.+ "ffactor" SUF=.+ PRE options SUF
  options ::= ""
  CMD(x) ::= .* PRE " "* x SUF
  KEY ::= [A-Za-z][A-Za-z0-9/_-]*
END MAGIC

content(a) ::=
{ a : CONTENT ELSE ENDIF }
| CMD("set " KEY) content(a)
| CMD("error " msg) content(a)
| CMD("- " .*) content(a)
| CMD("") content(a)
| CMD("if " cond) content(ELSE) content(a)
{ a : ELSE }
| CMD("elif " cond) content(ELSE)
| CMD("else") content(ENDIF)
{ a : ELSE ENDIF }
| CMD("endif")
{ a : CONTENT }
| .*

cond ::=
| "0"
| "1"
| "(not " cond op(NOT)
| "(and" op(AND)
| "(or" op(OR)
| KEY

op(a) ::=
{ a : AND OR }
| " " cond op(a)
{ a : NOT AND OR }
| ")"
```

Implementation
--------------

### Objects

Some files define a struct along with functions dealing with it. These
files provide at least one constructor function and exactly one
destructor function. Destructors accept null pointers. When the struct
definition is visible from the header file, its invariants are given.

### Naming

Files are prefixed by `ff_`. Functions are prefixed by their file name
when they are about an object. Constructors are suffixed by `_ctor`
and destructors by `_dtor`.

Typedef are only used for structs, unions, enums, and functions with
prefix `s_`, `u_`, `e_`, and `f_` respectively. For instance,
`s_chunk` is a typedef for `struct chunk`.

Goto labels say what they do. Most of the time they say what is the
first thing they free or destruct.

### Pointers

Pointers are handled with respect to ownership. When a pointer is
stored by an object, that object holds ownership for it.

We may distinguish three categories for pointer arguments in function
prototypes: return, borrow, and take. Let's consider the following
function:

    /** @brief Add two natural numbers. */
    extern int /** @return 0 on success, -1 on error */
    plus(
        t_nat **ret,    /**< __return__ result */
        t_nat *x,       /**< __take__ first operand */
        const t_nat *y  /**< __borrow__ second operand */
    );

When calling this function, the caller gains ownership on the result,
gives its ownership on the first operand, and borrows for the time of
the call its ownership on the second operand. This permits the called
function to reuse the first operand to store the result.

The return pointers are double pointers because the inner type may be
abstract and the caller may not call sizeof to allocate the correct
space.

### Errors

Functions that may fail return an error status, usually 0 on success
and -1 on error. If the function needs to return some object, then
this is done by taking a return pointer as argument.

In case of errors, take pointers are freed, borrow pointers remain
valid, and return pointers are null. The content of borrow pointers
may be indeterminate.
