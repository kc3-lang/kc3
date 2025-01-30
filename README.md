# KC3 v0.1.15

KC3 is a programming language with meta-programmation and a graph
database embedded into the language. It aims to be the language
for semantic programming, and programming the semantic web.

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

 - pretty printer
   - auto indent KC3 code
 - map
   - `access`
   - `get` (get key value)
   - `put` (return a new map)
 - struct
   - `access`
   - `get`
   - `put`
 - facts database (triple store) accessible from KC3
   - `new database (Ptr)`
   - `add_tags`
   - `collect_with`
   - `collect_with_tags`
   - `remove_tags`
   - `with_tags`
   - `with`
   - `with_macro`
 - HTTPd v0.2.0
   - dynamic pages (MVC)
     - models are defined in `./app/models/`
     - controllers are defined in `./app/controllers/`
     - templates are defined in `./app/templates/`
     - views are defined in `./app/views/`
     - dynamic router is defined in `./config/router.kc3`
       - For now we don't match request method and just match the start of
         the URL with `Str.starts_with?(url, route.path)`
       - If there is no route or no controller or the controller does
         not return a HTTP response, a 404 error is returned. Other
         frameworks like Ruby on Rails or Phoenix do return a 500...
   - threads
     - `env_fork_init`
     - `env_fork_clean`
     - `Thread.new(fn)`
 - fx v0.2.0
   - file explorer
   - preview files
     - text
     - image
     - video
     - audio
     - binary (hexdump -C)
   - properties
     - create
       - POST "/properties/*path"
     - delete
       - DELETE "/properties/*path


## Discord invite

[Join us on kmx.io Discord server !](https://discord.gg/A4MWkpUDsG)


## [Guides](https://kc3-lang.org/doc/3_Guides)

Follow the [KC3 guides](https://kc3-lang.org/doc/3_Guides)
to discover how to use KC3 for your own projects.


## TODO

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
 - HTTPd
   - dynamic router
     - def_route(:get, "/user/:id/articles/*slug/edit", UserArticlesController.edit)
 - libkc3
   - operators dispatch
     - list of matching operators (facts_with)
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
