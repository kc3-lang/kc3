# KC3 v0.1.16

KC3 is a programming language with meta-programmation and a graph
database embedded into the language. It aims to be the language
for semantic programming, and programming the semantic web.

You can easily convert each KC3 function to a C function. KC3 is both
a language and a runtime.

This is a development branch, for the latest release see [KC3 v0.1.15]
(https://git.kmx.io/kc3-lang/kc3/tree/v0.1.15).

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

## Discord invite

[Join us on kmx.io Discord server !](https://discord.gg/A4MWkpUDsG)


## [Guides](https://kc3-lang.org/doc/3_Guides)

Follow the [KC3 guides](https://kc3-lang.org/doc/3_Guides)
to discover how to use KC3 for your own projects.


## TODO

 - libkc3
   - env_init: find lib dir for /usr/local/lib/kc3/0.1/
   - `--pedantic` option (env_init)
     - ikc3_run
   - securelevel between 0 and 3
     - [x] API that can only increase securelevel between 0 and 2
     - 0 = (cfn + system + dlopen + dlsym) + (eval + def*)
     - 1 = def* + eval
       - [x] block all Cfn definition if `securelevel(0) > 0`
         - [x] `env_eval_callable`
         - [x] `cfn_eval`
         - [x] `cfn_link`
         - [x] `cfn_prep_cif`
         - [x] `buf_parse_pcallable`
         - [x] `buf_parse_cfn`
       - [x] block system() calls if `securelevel(0) > 0`
         - [x] `kc3_system`
         - [x] `kc3_system_pipe_exec`
       - [x] block dlopen() calls if `securelevel(0) > 0`
         - [x] kc3_dlopen
         - [x] env_dlopen
       - [x] block dlsym() calls if `securelevel(0) > 0`
         - [x] only ever called by cfn_apply which is already blocked at
               securelevel > 0
     - 2 = eval
       - [x] block buf_parse_fn if `securelevel(0) > 1`
       - [x] block buf_parse_pcallable if `securelevel(0) > 1`
       - [x] block env_eval_callable if `securelevel(0) > 1`
       - [x] block all env_def* if `securelevel(0) > 1`
       - [x] block all kc3_def* if `securelevel(0) > 1`
       - [x] block all facts_add* on global env facts
             if `securelevel(0) > 1`
       - [x] block all facts_remove* on global env facts
             if `securelevel(0) > 1`
     - 3 = ø (no KC3 eval, C-mode only)
       - [ ] block all env_eval_* if `securelevel(0) > 2`
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
