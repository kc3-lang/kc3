# C3 v0.1

C3 is a programming language project, inspired by C, Elixir and
Common Lisp. It could be described as C with Elixir modules,
pattern matching, and a semantic object system. The idea is to plug
modules, closures, pattern matching, cl-facts and metaprogramming
into C99 with a very small set of dependencies.


## Structure

### libc3

C3 is implemented using libc3 a small C99 library implementing the core
of the language.

The library includes a parser and an interpreter for C3 code in C
structures.

#### Parser

The parser is recursive descent for now.

#### AST

The AST is represented as C3 data structures and can be meta-programmed
in C.

#### Interpreter

Under development.


### ic3

Interactive shell. Under development.


### c3s

Script interpreter. Under development.


## TODO

 - libc3
   - atomic operations
   - DONE triple serial id
   - with ignore variables
   - boolean operators
   - arrays
   - lists
   - defmodule
   - structs
   - errors (setjmp, longjmp)
     - stacktrace
       - ffi ?
         - libdwarf
   - control structures
   - functions
     - return
     - def
     - funcall
     - & &1
     - DONE fn () {}
   - macros
   - livebook
     - gaussian
   - buf_sha256
   - buf_popen
   - tests
     - `(-S8_MIN - S8_MAX) == 1`
     - `UW_MAX == ((1 << 8 * sizeof(uw)) - 1)`
   - DONE ci
   - DONE c function call
   - DONE infix operators
   - DONE load
   - DONE `test_file_compare`
   - DONE ffi
     - DONE types
     - DONE funcall
