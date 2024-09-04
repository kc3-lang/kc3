# Variables

Variables in C3 can be defined using the litteral value for a variable
which is always `?`. You can cast this litteral value and it will not
really be casted but it will give you a typed variable litteral value.
E.g. `(List) ?`.
The typed variable litteral value will only accept to be set once to
one value of the variable's type (in this example the type of a linked
list).

It's actually a syntax so you cannot rename `?` by mistake and
so is an easy task to do static analysis of variable creation.

The default type for a variable which you can also specify explicitly
is `Tag` which is an enum-tagged union type of any other C3 types
currently defined in the environment. So `?` is exactly equivalent to
`(Tag) ?` and they will both accept to be set once to one value of any
type.

Actually all variables are allocated as tags and in the end the typing
is dynamic but it could be made static through JIT compilation of
functions.

A variable is settable once and cannot be changed afterwards (there is
an exception if you write C code and link to it but it is not easy nor
silent).

This way you do not need to lock or trust any behaviour, once your
variable is set to a value the value of the variable will never change,
it really is read-only.


## Init and C interoperatbility

To set the value of a variable in the end you need to call a C function
that should accept this C function definition :
`quote cfn unquote(type) unquote("init_#{variable_type}#{init_suffix}") (Result, ...)`.
There are many functions for this, here is a quick list :
  - `tag_init_1` takes a C string as an argument and returns a value of
  any type currently defined in the C3 environment.
  - `#{type}_init_1` C functions take a C string as an argument and return
  a value of type `type`.
  - `tag_init_copy` takes a tag as an argument and returns a deep copy
  of it.

You can also use the assignment operator which is `<-` which in turn calls
`tag_init_copy`. It works like the C assignment operator (`=`).

Examples :
```
# Declare a unsigned byte 8 bits variable "x".
x = (U8) ?
# Set the variable "x" to zero.
x <- 0
# Allocate again for the same binding name "x"
x = (U8) ?
# Also set the new variable "x" to zero with just one Unicode symbol
# that is AltGr+Y on my keyboard.
x ← 0
```


## So how do I change anything if it is read-only ?

You can always reset an existing binding at will to another variable
litteral and another variable will be created for the same name and it
will be in a different memory location, settable once and then
read-only again so you can use it without locking.
