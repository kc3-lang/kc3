# C3 v0.1.2

C3 is a programming language with meta-programmation and a graph
database embedded into the language. It aims to be the language
for semantic programming, and programming the semantic web.

We are currently fundraising for the project to become real and
there is a working prototype available at
[https://git.kmx.io/c3-lang/c3/](https://git.kmx.io/c3-lang/c3/),
please see the
[https://www.kmx.io/en/donations.html](https://www.kmx.io/en/donations.html)
page for helping out.

C3 is currently a programming language project, inspired by C, Elixir
and Common Lisp. It could be described as C with Elixir modules,
pattern matching, and a semantic object system. The idea is to plug
modules, closures, pattern matching, a graph database and
metaprogramming into C99 with an extremely small set of dependencies.


## Usage

### Compilation
```sh
git clone https://git.kmx.io/c3-lang/c3.git
cd c3
git submodule init
git submodule update
./configure
make
```

### Running the tests
```sh
make test
make test_asan
```

### Running gdb
```sh
make gdb_test
```


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

Interactive shell.


### c3s

Script interpreter.


## TODO

 - libc3
   - facts
     - negative facts : 4 + 2n = not 3 + 2n
     - with ignore variables
     - DONE atomic operations
     - DONE triple serial id
   - math
     - floating point numbers
   - variables
   - DONE boolean operators
   - DONE comparison operators
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
     - sizeof(sw) == sizeof(void *)
     - `(-S8_MIN - S8_MAX) == 1`
     - `UW_MAX == ((1 << 8 * sizeof(uw)) - 1)`
     - DONE group tests by test case
   - DONE ci
   - DONE c function call
   - DONE infix operators
   - DONE load
   - DONE `test_file_compare`
   - DONE ffi
     - DONE types
     - DONE funcall
