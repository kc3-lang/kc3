# KC3 v0.1.15

KC3 is a programming language with meta-programmation and a graph
database embedded into the language. It aims to be the language
for semantic programming, and programming the semantic web.

You can easily convert each KC3 function to a C function. KC3 is both
a language and a runtime.

This is a development branch see
[KC3 v0.1.14](https://git.kmx.io/kc3-lang/kc3/_tree/v0.1.14)
for a stable release.

KC3 is currently a programming language project, inspired by C, Elixir
and Common Lisp. It could be described as C with Elixir modules,
pattern matching, and a semantic object system. The idea is to plug
modules, closures, pattern matching, a graph database and
metaprogramming into C99 with an extremely small set of dependencies.

Components :
 - The pure C11 `libkc3` library is a full KC3 parser and interpreter.
 - `ikc3` is the interactive interpreter (verbose).
 - `kc3s` is the script interpreter (same as ikc3 but terse).
 - There are several C libraries which are installed in `lib/kc3/0.1`
   using the `make lib_links` command.
 - There is a web server in httpd : `kc3_httpd`.

All binaries can be compiled for four targets :
 - __main__ : speed optimizations from GCC/Clang -O2 and
   debug code like `assert` is disabled.
 - __debug__ : no optimization and debug code like `assert` is enabled
 - __cov__ : code coverage instrumentation (breaks `dlsym` and thus
   all `Cfn` at the moment)
 - __asan__ : memory safety instrumentation with ASAN, an absolutely
   awesome tool to detect all memory leaks, double free, segmentation
   faults, invalid read, invalid write, general memory corruption
   errors, etc., just run the __asan__ target for a full report at
   process exit, the report being empty if all is ok.

Supported operating systems (additional dependencies) :
 - BSD
 - Linux (libbsd, libmd)
 - MacOS X (libmd)
 - Windows (MSys2) (no network)

Supported architectures :
 - aarch64 (arm64, Apple M1, Apple M2)
 - amd64
 - i386
 - sparc64

To install and test KC3 for yourself, you can follow the
[KC3 Installation Guide](https://kc3-lang.org/doc/3_Guides/3.1_Install).

## Users

There are now four full applications written in KC3 that we know of :
 - The KC3 Cairo demo which you can run with `make demo`
 - The KC3 SDL2 OpenGL demo which you can run with `make demo_gl`
 - The [KC3 website](https://kc3-lang.org/) which you can launch
   locally with `make test_httpd` with all the KC3 releases and
   documentation.
 - The [www.kmx.io website](https://www.kmx.io/) which is closed
   source and is hosted on kmx.io servers in France using OpenBSD
   everywhere. We donate to OpenBSD every month because a healthy
   software ecosystem is a funded ecosystem.


## New in this release

 - libkc3
   - stacktrace (in gdb: `gdb> p err_stacktrace()`)
   - hash table (ht.h, ht.c)
   - operators hash table
     - defoperator
     - ops_get
     - facts_add/replace
   - pass by reference and reference counting in a few data structures
     - Callable (Fn, Cfn)
     - Struct
     - StructType
   - optional pass by copy (--copy) for use with ASAN.
     - `env_init_args` now parses `--copy`
   - documentation
     - document all basic types (see /doc/*/*.md or
       <https://kc3-lang.org/doc>)
     - document facts module
   - rename block into do_block
   - implement named blocks, return and return_from like in Common Lisp
   - add a named block to function evaluation to implement return from
     functions. The first name you give to the function is the name of
     the function implicit block.
   - added `unwind_protect` and `env_unwind_protect_push/pop` dance to a
     few functions
     - marked remaining code paths with TODO where unwind_protect is
       still needed (potential memleaks / env corruption)
   - `env_frame_capture` implements closures that capture their lexical
     environment (free variables)
   - while, break, continue as special operators with tests
     - reworked parser and evaluator to allow for special operators
       without arguments
     - introduced a new syntax `(Ident) Module.sym` to exclude special
       operator parsing.
   - converted almost every file to 72 columns max
   - added licenses to kc3 files
   - started to work on marshall/marshall_read (not ready yet)


## Discord invite

[Join us on kmx.io Discord server !](https://discord.gg/A4MWkpUDsG)


## [Guides](https://kc3-lang.org/doc/3_Guides)

Follow the [KC3 guides](https://kc3-lang.org/doc/3_Guides)
to discover how to use KC3 for your own projects.


## TODO

 - HTTPd
   - limit acceptor loop
   - OAuth2 / jwt
   - dynamic router
     - def_route(:get, "/user/:id/articles/*slug/edit", UserArticlesController.edit)
 - fx
   - chaining of audio and video previews (folder as a playlist)
   - tags
     - create
       - POST "/tag/:tag/*path"
     - delete
       - DELETE "/tag/:tag/*path"
     - recursive
   - properties
     - recursive
 - libkc3
   - hash-table as a KC3 value (map ?)
    - defspecial
   - special operators hash table
   - marshall / marshall_read
     - to buf
     - to str
     - to file
     - from buf
     - from str
     - from file
   - pass by reference and reference counting in all data structures
     - array
     - cow
     - frame
     - list
     - map
   - base-specific big floats
   - macro cast : `(Macro) fn (x) { x }`
   - pretty printer
     - 80 columns (`\n`)
   - tags
     - generic walker
     - height function `(TAG_VOID: 1, TAG_TUPLE: (1+ (max (height tuple->tags))))`
     - has_ident
     - collect_idents
   - facts
     - negative facts : 4 + 2n = not 3 + 2n
     - with ignore variables
   - math
     - arbitrary precision floating point numbers (decimals)
   - enums
   - unions
   - errors (setjmp, longjmp)
       - ffi ?
         - libdwarf
   - control structures
     - when
     - unless
     - switch/case/cond
   - livebook
     - gaussian
   - buf_sha256
   - buf_popen
   - tests
