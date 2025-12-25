# KC3

## v0.1.16 🎄 (2025-12)

 - libkc3
   - dlopen inside lib/ only
   - Generic typed pointers : `(U8*) 0x0`
   - make `kc3_require` and `env_eval_do_block` more careful about error
     handling
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
   - followed System-V ABI documentation for struct alignment and
     padding, works on amd64, arm64, i386 and sparc64 (marshalling and
     unmarshalling is portable across all these architectures).

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
     and the dynamic libraries will still be found (e.g. `require Socket`
     loads the socket library using dlopen)

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
   - apply unveil(2) filesystem access permissions on OpenBSD
     - current dir (./) is read-only and ./log and ./db are read-write
     - using kc3 unveil wrapper that soft fails on other systems
     - on OpenBSD a failed unveil call aborts the program into the
       debugger
   - apply unrevokable restrictions on future syscalls on OpenBSD using
     pledge(2)

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

 - removed unused/merged branches

## v0.1.15

 - libkc3
   - Typed pointer example : `(Facts*)` → `p_facts`
   - securelevel between 0 and 3
     - API that can only increase securelevel between 0 and 2
     - 0 = (cfn + system + dlopen + dlsym) + (eval + def*)
     - 1 = def* + eval
       - block all Cfn definition if `securelevel(0) > 0`
         - `env_eval_callable`
         - `cfn_eval`
         - `cfn_link`
         - `cfn_prep_cif`
         - `buf_parse_pcallable`
         - `buf_parse_cfn`
       - block system() calls if `securelevel(0) > 0`
         - `kc3_system`
         - `kc3_system_pipe_exec`
       - block dlopen() calls if `securelevel(0) > 0`
         - kc3_dlopen
         - env_dlopen
       - block dlsym() calls if `securelevel(0) > 0`
         - only ever called by cfn_apply which is already blocked at
           securelevel > 0
     - 2 = eval
       - block buf_parse_fn if `securelevel(0) > 1`
       - block buf_parse_pcallable if `securelevel(0) > 1`
       - block env_eval_callable if `securelevel(0) > 1`
       - block all env_def* if `securelevel(0) > 1`
       - block all kc3_def* if `securelevel(0) > 1`
       - block all facts_add* on global env facts
         if `securelevel(0) > 1`
       - block all facts_remove* on global env facts
         if `securelevel(0) > 1`
     - 3 = ø (no KC3 eval, C-mode only)
       - [ ] block all env_eval_* if `securelevel(0) > 2`

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


## v0.1.14
 - libkc3
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
   - marshall / marshall_read of all data types
   - `env_dump` / `env_dump_restore`
   - automatic loading of env dump from kc3.dump if present
     - in lib/kc3/0.1
     - in current working dir
     - reduced loading time of environment from minutes to sub-second !
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

## v0.1.13

 - libkc3
   - pretty printer
     - indentation (str, if, do, fn, struct, map)
 - ikc3
   - facts
     - Facts.with_tags(facts, s, p, o, fn (fact) {}) -> cursor
 - lib
   - File.exists?
   - File.is_directory?
   - File.list
   - File.stat
 - HTTPd v0.1.1
   - file server (lftp)
     - route_request
     - error_404_page
     - directory_page
       - slash (lftp)

## v0.1.12

 Renamed C3 to KC3.

 - libkc3
   - access
     - DONE buf_parse_call_access (Tag "." Sym)
     - DONE access (Tag, Sym) => Tag
   - modules
     - defstruct
   - control structures
     - while
   - Str eval : `"Hello, #{name} !"`
 - EKC3
   - embed KC3 in any file : `Hello, <%= name %> !`
   - escape HTML :
     - ["&": "&amp, "<": "&lt", ">": "&gt", "\"": "&quot", "'": &#39"]
 - HTTP
   - sockets
   - request
   - response
 - HTTPd
   - `make test_httpd`

## v0.1.11

 - libc3
   - modules
     - defmodule Empty do end
     - def
       - def three = 3
       - def double = fn (x) do x * 2 end
       - def double_tuple = macro (x) do {x, x} end
       - def operator\_double = %KC3.Operator{sym: :double, symbol_value: fn (x) { x * 2 }}

With this release you can actually load a module as a kc3 source file.
See [lib/kc3/0.1/list.kc3](https://git.kmx.io/kc3-lang/kc3/_tree/master/lib/kc3/0.1/list.kc3)

## v0.1.10

 - libc3
   - OpenGL demo (`make demo_gl`)
     - OpenGL 3.3 with shaders (portable)
     - makes use of C3 data structures.
   - pin operator (for pattern matching)
   - macros
   - special operators
     - their arguments do not get evaluated
     - they return a value
     - parsed without syntax
       Ideally it would look like this :
       ```
       ic3> defspecial_operator op (a, b, c) { true }
       special_operator (a, b, c) { true }
       ic3> op 1 2 3
       true
       ```
     - See C3 module in `lib/c3/0.1/c3.facts`
   - if, then, else.
   - rational numbers and corresponding operations
   - complex numbers and corresponding operations
   - float 128 bit (F128) and corresponding operations

## v0.1.9

 - libc3
   - c3
     - tag_type
   - gui
     - cairo graphics
     - OpenGL graphics
   - maps
     - A map maps keys to values according to an internal hash table.
     - `%{a: 1, 2 => 3}`
     - access (get symbol key value)
     - get (get key value)
     - put (return a new map)
     - machine word alignment (from rtbuf)
   - structs
     - structs are a special kind of map with their fields sorted
       according to a spec.
     - as a `s_map` and `s_struct_spec` accessible through their module
       name.
     - `defstruct`
     - `%Module{a: 1, b: 2}`
     - access
     - get
     - put
     - machine word alignment (from rtbuf)
