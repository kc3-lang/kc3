# C3 v0.1.4

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

### Add the sources to LD_LIBRARY_PATH
```sh
export LD_LIBRARY_PATH=$PWD/libc3/.libs:$PWD/libffi/.libs
```

### Running the tests
```sh
make test
```

### Running ic3
```sh
ic3/.libs/ic3
```

### Running gdb
```sh
make gdb_ic3
make gdb_test
```


## Structure

### libc3

C3 is implemented using libc3 a small C99 library implementing the core
of the language.

The library includes a parser and an interpreter for C3 code in C
structures.

Support for large integers provided by
[libtommath](https://github.com/libtom/libtommath).

Support for C function calls provided by
[libffi](https://github.com/libffi/libffi).

Modules are saved as facts databases.


#### Parser

The parser is recursive descent.

#### AST

The AST is represented as C3 data structures and can be meta-programmed
in C.

#### Interpreter

Under development.


### ic3

Interactive shell. Terminal I/O provided by
[linenoise](https://github.com/antirez/linenoise/tree/1.0).

Example :
```
$ make test
$ ic3/ic3
ic3> ic3> 1 + 1
2
ic3> 2 + 2
4
ic3> 3 + 3
6
ic3> 1 +
ic3> 1
2
ic3> double = fn (x) { x * 2 }
fn (x) { x * 2 }
ic3> double
fn (x) { x * 2 }
ic3> double(1)
2
ic3> double(2)
4
ic3> double(3)
6
ic3> double(4)
8
ic3> List.map((1, 2, 3, 4), double)
(2, 4, 6, 8)
ic3> List.reverse(List.map((1, 2, 3, 4), double))
(8, 6, 4, 2)
```

The `List.map` and `List.reverse` functions are defined in
`lib/c3/0.1/list.facts` and can be modified in real time.

For example, without closing ic3 let's redefine `List.reverse`,
open an editor and change the line in `lib/c3/0.1/list.facts` from

```
replace {List.reverse, :fn, fn {
  (x) { List.reverse(x, ()) }
  ((), acc) { acc }
  ((a | b), acc) { List.reverse(b, (a | acc)) }
}}
```

to

```
replace {List.reverse, :fn, fn {
  (x) { List.reverse(x, ()) }
  ((), acc) { (:reversed | acc) }
  ((a | b), acc) { List.reverse(b, (a | acc)) }
}}
```

and check the results of the last command (up key) in ic3/ic3 :

```
ic3> List.reverse(List.map((1, 2, 3, 4), double))
(:reversed, 8, 6, 4, 2)
```

Don't forget to revert the changes to `list.facts`.


### c3s

Script interpreter. Works the same as ic3 but is not interactive.


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
      - DONE = equal
      - DONE funcall
   - DONE boolean operators
   - DONE comparison operators
   - cast
   - arrays
     - DONE parse
     - DONE inspect
     - DONE [][][] data
   - lists
   - defmodule
   - structs
   - errors (setjmp, longjmp)
     - stacktrace
       - ffi ?
         - libdwarf
   - control structures
   - modules
     - DONE symbols
     - DONE as facts
     - DONE autoload (remember load time, compare with file time)
     - defmodule
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
     - DONE group tests by test case
   - DONE ci
   - DONE c function call
   - DONE infix operators
   - DONE load
   - DONE `test_file_compare`
   - DONE ffi
     - DONE types
     - DONE funcall
