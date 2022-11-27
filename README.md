# C3

C3 is a programming language project, inspired by C, Elixir and
Common Lisp. It could be described as C with Elixir macros and pattern
matching, and a semantic object system.

The idea is to plug macros and pattern matching into C89 and use them to implement
the graph database.

The garbage collector is optional, like in C.

```
defmodule C3 do
  def compile (c3, opts) when is_string(c3) do
    c3
    |> C3.parse(opts)
    |> C3.to_assembly(opts)
    |> Assembly.compile(opts)
  end
end
```

## Structure

### libc3

C3 is implemented using libc3 a small C89 library implementing the core
of the language.

The library includes a parser and an interpreter for C3 code in C structures.

#### Parser

The parser is recursive descent for now.

#### AST

The AST is represented as C3 data structures and can be meta-programmed in C.

#### Interpreter

Under development.


### ic3

Interactive shell. Under development.


### c3s

Script interpreter. Under development.


## TODO

 - libc3
   - errors (setjmp, longjmp)
     - stacktrace
   - load
   - pattern matching (xor xor)
   - `test_file_compare`
   - ffi
     - types
     - funcall
   - functions
     - fn ->
     - & &1
     - defmodule
     - funcall


## Copying

Copyright 2022 kmx.io <contact@kmx.io>

Permission is hereby granted to use this software granted
the above copyright notice and this permission paragraph
are included in all copies and substantial portions of this
software.

THIS SOFTWARE IS PROVIDED "AS-IS" WITHOUT ANY GUARANTEE OF
PURPOSE AND PERFORMANCE. IN NO EVENT WHATSOEVER SHALL THE
AUTHOR BE CONSIDERED LIABLE FOR THE USE AND PERFORMANCE OF
THIS SOFTWARE.
