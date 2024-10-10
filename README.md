# KC3 v0.1.14

KC3 is a programming language with meta-programmation and a graph
database embedded into the language. It aims to be the language
for semantic programming, and programming the semantic web.

This is a development branch, see
[KC3 v0.1.13](https://git.kmx.io/kc3-lang/kc3/_tree/v0.1.13/)
for a stable release.

KC3 is currently a programming language project, inspired by C, Elixir
and Common Lisp. It could be described as C with Elixir modules,
pattern matching, and a semantic object system. The idea is to plug
modules, closures, pattern matching, a graph database and
metaprogramming into C99 with an extremely small set of dependencies.

Supported operating systems (additional dependencies) :
 - BSD
 - Linux (libbsd, libmd)
 - MacOS X (libmd)
 - Windows (MSys2)

Supported architectures :
 - aarch64 (arm64, Apple M1, Apple M2)
 - amd64
 - i386
 - sparc64


## Discord invite

[Join us on kmx.io Discord server !](https://discord.gg/A4MWkpUDsG)


## Usage

### Install dependencies

#### Debian / Ubuntu / PopOS / Mint

```
# install build tools
sudo apt install pkg-config clang libtool-bin make ruby time

# install dependencies
sudo apt install libffi-dev libbsd-dev libevent-dev

# install demo dependencies
sudo apt install glew libfreetype-dev libsdl2-dev libxkbcommon-dev

# use clang
export CC=clang
```


### Compilation
```sh
git clone https://git.kmx.io/kc3-lang/kc3.git
cd kc3
git submodule init
git submodule update
cd fonts
git submodule init
git submodule update
cd ..
./configure
make
```

### Add the sources to `LD_LIBRARY_PATH`
```sh
. ./env
```

### Create symlinks to libs
```sh
make lib_links
```

### Running the OpenGL demo
```sh
make demo_gl
```

Press Q to quit, F for fullscreen, Left and Right arrows to move
through sequences.

Other targets exist :
```sh
make demo_gl_asan
make gdb_demo_gl
make lldb_demo_gl
```

### Running the Cairo demo
```sh
make demo
```

Press Q to quit, Left and Right arrows to move through sequences.

Other targets exist :
```sh
make demo_asan
make gdb_demo
make lldb_demo
```

### Running the HTTPd demo
```sh
make test_httpd
```

### Running the tests
```sh
make test
```

All the tests will be run. More specific targets exist :
```sh
make test_libkc3
make test_ikc3
make test_libkc3_asan
make test_ikc3_asan
make test_asan
```

### Running ikc3
```sh
ikc3/.libs/ikc3
```

### Running gdb
```sh
make gdb_ikc3
make gdb_test
```

### Adding or removing files from the source tree
```sh
make gen
./update_sources
```

Don't forget to commit your files and the modified sources.mk and
sources.sh files.


## Structure

### libkc3

KC3 is implemented using libkc3 a small C99 library implementing the core
of the language.

The library includes a parser and an interpreter for KC3 code in C
structures.

Support for large integers provided by
[libtommath](https://github.com/libtom/libtommath).

Support for C function calls provided by
[libffi](https://github.com/libffi/libffi).

Modules are saved as facts databases.


#### Parser

The parser is recursive descent.

#### AST

The AST is represented as KC3 data structures and can be meta-programmed
in C.

#### Interpreter

Under development.


### ikc3

Interactive shell. Terminal I/O provided by
[linenoise](https://github.com/antirez/linenoise/tree/1.0).

Example :
```
$ make test
$ ikc3/ikc3
ikc3> ikc3> 1 + 1
2
ikc3> 2 + 2
4
ikc3> 3 + 3
6
ikc3> 1 +
ikc3> 1
2
ikc3> double = fn (x) { x * 2 }
fn (x) { x * 2 }
ikc3> double
fn (x) { x * 2 }
ikc3> double(1)
2
ikc3> double(2)
4
ikc3> double(3)
6
ikc3> double(4)
8
ikc3> List.map([1, 2, 3, 4], double)
[2, 4, 6, 8]
ikc3> List.reverse(List.map([1, 2, 3, 4], double))
[8, 6, 4, 2]
```

The `List.map` and `List.reverse` functions are defined in
`lib/kc3/0.1/list.kc3` and can be modified in real time.

For example, without closing ikc3 let's redefine `List.reverse`,
open an editor and change the line in `lib/kc3/0.1/list.kc3` from

```
  def reverse = fn {
    (x) { reverse(x, ()) }
    ([], acc) { acc }
    ([a | b], acc) { reverse(b, [a | acc]) }
  }
```

to

```
  def reverse = fn {
    (x) { reverse(x, ()) }
    ([], acc) { [:reversed | acc] }
    ([a | b], acc) { reverse(b, [a | acc]) }
  }
}}
```

and check the results of the last command (up key) in ikc3/ikc3 :

```
ikc3> List.reverse(List.map([1, 2, 3, 4], double))
[:reversed, 8, 6, 4, 2]
```

Don't forget to revert the changes to `list.kc3`.


#### Maps

KC3 maps are key-value stores, you can use any tag as a key and
associate a value to it.

You can use destructuring to access KC3 maps :
```
ikc3> a = %{id: 1, title: "My title", message: "Hello, world !"}
%{id: 1, title: "My title", message: "Hello, world !"}
ikc3> a = %{}
%{id: 1, title: "My title", message: "Hello, world !"}
ikc3> %{id: id, title: "My title", message: message} = a
%{id: 1, title: "My title", message: "Hello, world !"}
ikc3> id
1
ikc3> message
"Hello, world !"
```

You can use the dot syntax to access map values from a `Sym` key :
```
ikc3> a = %{id: 1, title: "My title", message: "Hello, world !"}
%{id: 1, title: "My title", message: "Hello, world !"}
ikc3> a.id
1
ikc3> a.message
"Hello, world !"
```

You can also use the `KC3.access` function for the same result :
```
ikc3> a = %{id: 1, title: "My title", message: "Hello, world !"}
%{id: 1, title: "My title", message: "Hello, world !"}
ikc3> access(a, :id)
1
ikc3> access(a, :message)
"Hello, world !"
```


#### Unicode characters

`ikc3` fully supports Unicode :

Some unicode characters :
```
ikc3> '\U+1B2FB'
'𛋻'
ikc3> '𐅀'
'𐅀'
ikc3> '🤩'
'🤩'
ikc3>
```

#### Large integers

```
ikc3> a = 1 + 100000000000000000000000000000000
100000000000000000000000000000001
ikc3> a * a
10000000000000000000000000000000200000000000000000000000000000001
ikc3>
```

#### Ratios

Ratios are made with a couple of large integers : the numerator
which can be any number, and the denominator which has to be positive.
They represent fractions of integral numbers.
They are written with a slash and no space.

```
ikc3> 1/2 + 2/3
7/6
ikc3> 1/2 * 2/3
1/3
ikc3> 1/2 / 2/3
3/4
ikc3> 1/2 - 2/3
-1/6
```


#### Complex numbers

Complex numbers are constructed using the operator `+i` on any kind of
numbers (unsigned, signed, float, ratios, and even other complex
numbers). For instance, you can write `a +i b` where `a` and `b` are
real numbers.

```
ikc3> 1 +i 2
1 +i 2
ikc3> 1 +i 2 + 2 +i 3
3 +i 5
ikc3> (1 +i 2) * (2 +i 3)
-4 +i 7
ikc3> (1 +i 2) / (2 +i 3)
0 +i 0
ikc3> (1/1 +i 2/1) / (2 +i 3)
8/13 +i 1/13
```

As you can see integer division is not producing ratios.
That might change in future releases.


#### Lists

Lists are marked with brackets `[]`.

Regular lists can be :
 - an element and a list : `[1 | []]` → `[1]`
 - multiple elements : `[1, 2, 3]`
 - multiple elements and a list : `[1, 2 | [3, 4]]` → `[1, 2, 3, 4]`
 - the empty list : `[]`

Regular lists end with the empty list : `[1] == [1 | []]`.

You can also contruct dotted lists like in Common Lisp where
the next list pointer is an arbitrary form. E.g. :
 - an element and an element : `[1 | 2]`
 - multiple elements and an element : `[1, 2, 3 | 4]`
 - the empty list and an element : `[[] | 1]`

All these list formats are supported in pattern matching.


#### Pattern matching and destructuring

The KC3 pattern matching principles come from Erlang and Elixir.

All tag data structures in KC3 can be pattern matched using the equal
sign (`=`) against litteral values containing identifiers. All
identifiers are supposed to be new bindings when using pattern matching
in KC3. If you want to use an identifier's value in pattern matching you
must use the _pin operator_ (`^`). Variables can be assigned a new value
from either side of the equal sign and from inside a tag data structure,
which is called _destructuring_.


Examples :
```
ikc3> a = 1
1
ikc3> a = 2
2
ikc3> a
2
ikc3> ^ a = 1
void
ikc3> ^ a = 2
2
ikc3> ^ a = b
2
ikc3> b
2
```

To use destructuring just type the litteral value you want to match and
put identifiers (variable names) where you want a variable matching the
value on the other side of the equal sign. This is the most visual
approach possible to text-based value matching : the data is constantly
matched to litterals that show their type to the programmer. This is
really helpful when writing large programs that need to scale in the way
of abstractions. Let the data flow in the code through visual types.

Examples :
```
ikc3> [x, y | z] = List.reverse([1, 2, 3, 4])
[4, 3, 2, 1]
ikc3> x
4
ikc3> y
3
ikc3> z
[2, 1]
```


#### Macros

KC3 macros are like Common Lisp macros with Elixir pattern-matching.

Macros are like functions but start with `macro` instead of `fn` and
their arguments do not get evaluated. However they get pattern matched
and the full power of the pattern matcher is available for arguments
destructuring. Use a map if you want named arguments. Use a list if you
want &rest arguments, use a block if you want a &body argument.

When evaluated, a macro call returns a tag which is in turn evaluated
in the calling site lexical environment. This allows for DSLs and custom
control structures to be defined in KC3.

Many basic operations in KC3 are defined as macros : error handling,
free operations with `unwind-protect`, graph database operations like
`Facts.with`.


#### If, then, else.

Conditionals in KC3 are like in Ruby, for example :
```
ikc3> if true && true
ikc3>   1 + 1
ikc3>   2 + 2
ikc3> end
4

ikc3> if true && false
ikc3>   1 + 1
ikc3>   2 + 2
ikc3> else
ikc3>   3 + 3
ikc3>   4 + 4
ikc3> end
8
```

A KC3 `if` statement always return a value. If the condition is true, the
first (then) block gets evaluated. If the condition is false the second
block gets evaluated. If the condition is false and an `else` block is
not provided, then `void` gets returned.

One liner examples with `then` :
```
ikc3> if 42 then 100 else 101 end
100
ikc3> if 0 then 100 else 101 end
101
```

## defmodule and def

Example :
```
ikc3> defmodule Example do
ikc3>   def three = 3
ikc3>   def double = fn (x) do x * 2 end
ikc3>   def double_tuple = macro (x) do {x, x} end
ikc3>   def operator_double = %KC3.Operator{sym: :double, symbol_value: fn (x) { x * 2 }
ikc3> end
Example
ikc3> Example.three
3
ikc3> Example.double
fn (x) do x * 2 end
ikc3> Example.double(21)
42
ikc3> Example.double_tuple(:ok)
{:ok, :ok}
ikc3> double 21
42
```


#### Facts

The Facts module allows read and write access to a graph database
containing facts : triples of subject, predicate, object.

Examples for querying the KC3 database containing all definitions of
the interpreter :

```
ikc3> Facts.with_tags(Facts.env_facts(), KC3, :operator, ?,
        fn (fact) { puts(fact.object); :ok })
operator_eq
operator_gt
operator_lt
[...]
:ok
```


### kc3s

Script interpreter. Works the same as ikc3 but is not interactive
and does not output results.


### HTTPd

HTTP daemon, use `make test_httpd`.

The http daemon is defined in `httpd/httpd.c` and
`lib/kc3/0.1/httpd.kc3`.

The http daemon is a static file server listing directories and
serving files for display or download (Web 1.0).


## TODO

 - fx v0.1.0
   - [DONE] file explorer
   - [DONE] preview files
     - [DONE] text
     - [DONE] image
     - [DONE] video
     - [DONE] audio
   - tags
     - create
       - POST "/tag/:tag/*path"
     - delete
       - DELETE "/tag/:tag/*path"
   - properties
     - create
       - POST "/property/:property/:value/*path"
     - delete
       - DELETE "/property/:property/:value/*path
 - HTTPd v0.2.0
   - dynamic pages (MVC)
     - [DONE] controllers
       - [DONE] ./app/controllers/
     - [DONE] templates
       - [DONE] ./app/templates/
     - dynamic router
       - [DONE] Str.starts_with?(url, route.path)
       - HTTPd.Router.get("/user/:id/articles/*slug/edit", UserArticlesController.show)
     - views
       - ./app/views/
       
 - libkc3
   - evaluation order for && and ||
   - operators dispatch
     - list of matching operators (facts_with)
   - base-specific big floats
   - macro cast `(Macro) fn (x) { x }`
   - pretty printer
     - [DONE] indent
     - 80 columns (`\n`)
   - tags
     - walker
     - height function `(TAG_VOID: 1, TAG_TUPLE: (1+ (max (height tuple->tags))))`
     - has_ident
     - collect_idents
   - facts
     - negative facts : 4 + 2n = not 3 + 2n
     - with ignore variables
   - math
     - arbitrary precision floating point numbers (decimals)
   - map
     - get (get key value)
     - put (return a new map)
   - struct
     - access
     - get
     - put
   - enums
   - unions
   - errors (setjmp, longjmp)
     - stacktrace
       - ffi ?
         - libdwarf
   - control structures
     - when
     - unless
     - switch/case/cond
   - unwind protect
   - functions
     - return
   - livebook
     - gaussian
   - buf_sha256
   - buf_popen
   - tests
