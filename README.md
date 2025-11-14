# KC3 v0.1.16-git

"The first graph-native programming language"

KC3 is a programming language with meta-programmation and a graph
database embedded into the language. It aims to be the language
for semantic programming, and programming the semantic web.

You can easily convert each KC3 function to a C function. KC3 is both
a language and a runtime.

This is a development branch, for the latest release see
[KC3 v0.1.15](https://git.kmx.io/kc3-lang/kc3/_tree/v0.1.15).

KC3 is currently a programming language project, inspired by C, Elixir
and Common Lisp. It could be described as C with Elixir modules,
pattern matching, and a semantic object system. The idea is to plug
modules, closures, pattern matching, a graph database and
metaprogramming into C11 with an extremely small set of dependencies.

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
 - __cov__ : code coverage instrumentation
 - __asan__ : memory safety instrumentation with ASan, an absolutely
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
   - dlopen inside lib/ only
   - Typed pointer example : `(Facts*)` → `p_facts`
   - Generic typed pointers : `(U8*) 0x0`
   - make `kc3_require` and `env_eval_do_block` more careful about error
     handling
   - securelevel between 0 and 3
     - API that can only increase securelevel between 0 and 2
     - 0 → (cfn + system + dlopen + dlsym) + eval + def*
     - 1 → eval + def*
     - 2 → eval
     - 3 → ø (no KC3 eval, C-mode only)
   - MP-safe integer counters : `defcounter name = value`
     - environment hash table for name resolution
     - mp-safe
     - `Counter.decrease(Ident, Tag)` as a `cfn_macro`, Tag must be a
       positive integer (non-zero)
     - `Counter.get(Ident)` as a `cfn_macro`
     - `Counter.increase(Ident, Tag)` as a `cfn_macro`, Tag must be a
       positive integer (non-zero)
     - `Counter` module included at init
   - unveil supported on OpenBSD
   - pledge supported on OpenBSD
   - rename f128 to f80: long double is actually 80 bits precision on
     x86 and amd64

- libtls
     - basic TLS client and server in test/tls/tls.kc3

 - ikc3
   - Remote procedure call
     - client : ikc3 --client HOST PORT
       - opens a prompt on stdin and forwards parsed tags to server
         using `RPC.request("input")`
       - reads structured response from server :
         ```elixir
         %RPC.Response{out: "plop\n",
                       err: "warning: message\n",
                       result: (Sw) 5}
         ```
     - server : ikc3 --server HOST PORT
       - accepts only one connection and exits
       - works with netcat (no prompt)
     - TLS with libtls : ikc3 --tls (--client|--server) HOST PORT
       - "ikc3: TLS server: listening on HOST PORT"
       - "ikc3: TLS server: client connected: HOST PORT TLS-1.3"
       - "ikc3: TLS client: connected to HOST PORT TLS-1.3"
   - ship ikc3 as a standalone file that links to the right lib directory
     on MacOS, this way you can open ikc3 using Finder in a Terminal.app
     and the dynamic libraries will still be found

 - build system
   - use `runj` to parallelize configure and update_sources
   - use `sort` to have a portable sort algorithm that gives consistent
     results across all operating systems.

 - test infrastructure
   - modular test infrastructure with test/test.subr and
     test/test_runner
   - use `runj` to call test_runner in parallel
   - removed `sleep 2` that slowed each ikc3 test
   - all test suites run in under 30 seconds now compared to
     more than 5 minutes before

 - HTTPd
   - limit acceptor loop using defcounter
   - achieved securelevel(2) after load_app() by moving all
     `def*` into proper modules and using `defcounter`
   - apply unveil filesystem permissions, works on OpenBSD
     - current dir (./) is read-only and ./log and ./db are read-write
     - using kc3 unveil wrapper that soft fails on other systems
     - on OpenBSD a failed unveil call aborts the program into the
       debugger

 - window
   - demo
     - unified all OpenGL demos under window/demo
   - SDL2
     - OpenGLESv3
       - Use [Angle](https://github.com/google/angle) GLESv3
         implementation with Metal backend on MacOS. This provides
         SDL2 with an EGL/GLESv3 driver on MacOS despite OpenGL support
         being dropped by Apple.
   - EGL backend
     - XCB backend for X11
     - VT backend on Linux and OpenBSD using DRM KMS and GBM

 - kpkg - KC3 package manager
   - written in 3 days in KC3 + C
   - Argument parsing as a `while` loop with `with` pattern matching
     (like Elixir)
   - Added repo definitions for :
     - [freetype](https://git.kmx.io/kc3-lang/freetype)
     - [kc3](https://git.kmx.io/kc3-lang/kc3)
     - [libbsd](https://git.kmx.io/kc3-lang/libbsd)
     - [libevent](https://git.kmx.io/kc3-lang/libevent)
     - [libffi](https://git.kmx.io/kc3-lang/libffi)
     - [libmd](https://git.kmx.io/kc3-lang/libmd)
     - [libpng](https://git.kmx.io/kc3-lang/libpng)
     - [libressl](https://git.kmx.io/kc3-lang/libressl)
   - supports cross compilation
   - used as main platform for compiling kc3 and dependencies for
     Android

 - release engineering
   - compressed dmg with large icons and a nice arrow pointing to
     applications symlink
     - mac release binaries work from any absolute path and use
       relative paths to find dynamic libraries
   - debian package building from sources
   - gentoo overlay and ebuilds for
     - kc3
     - kmx_sort
     - runj

 - remove unused/merged branches

## Discord invite

[Join us on kmx.io Discord server !](https://discord.gg/A4MWkpUDsG)

## [Guides](https://kc3-lang.org/doc/3_Guides)

Follow the [KC3 guides](https://kc3-lang.org/doc/3_Guides)
to discover how to use KC3 for your own projects.

## TODO

 - libkc3
   - env_init: find lib dir for /usr/local/lib/kc3/0.1/
 - HTTPd
   - OAuth2 / jwt
   - dynamic router
     - def_route(:get, "/user/:id/articles/*slug/edit", UserArticlesController.edit)
 - tls
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
   - unions
   - enums
   - hash-table as a KC3 value (map ?)
   - special operators hash table
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
