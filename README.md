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

## New in this release

 - pretty printer
   - auto indent KC3 code
 - facts database (triple store) in KC3
   - new database (Ptr)
   - add_tags
   - remove_tags
   - with_tags
   - with


## Discord invite

[Join us on kmx.io Discord server !](https://discord.gg/A4MWkpUDsG)


## [Guides](https://kc3-lang.org/doc/3_Guides)

Follow the [KC3 guides](https://kc3-lang.org/doc/3_Guides)
to discover how to use KC3 for your own projects.


## TODO

 - fx v0.2.0
   - [DONE] file explorer
   - [DONE] preview files
     - [DONE] text
     - [DONE] image
     - [DONE] video
     - [DONE] audio
     - [DONE] binary (hexdump -C)
     - chaining of audio and video previews (folder as a playlist)
   - tags
     - create
       - POST "/tag/:tag/*path"
     - delete
       - DELETE "/tag/:tag/*path"
     - recursive
   - properties
     - [DONE] create
       - [DONE] POST "/properties/*path"
     - [DONE] delete
       - [DONE] DELETE "/properties/*path
     - recursive
 - HTTPd v0.2.0
   - dynamic pages (MVC)
     - [DONE] controllers
       - [DONE] ./app/controllers/
     - [DONE] templates
       - [DONE] ./app/templates/
     - dynamic router
       - [DONE] Str.starts_with?(url, route.path)
       - HTTPd.Router.get("/user/:id/articles/*slug/edit", UserArticlesController.show)
     - [DONE] views
       - [DONE] ./app/views/
 - libkc3
   - operators dispatch
     - list of matching operators (facts_with)
   - base-specific big floats
   - macro cast : `(Macro) fn (x) { x }`
   - pretty printer
     - [DONE] auto indent KC3 code
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
     - [DONE] access
     - [DONE] get (get key value)
     - put (return a new map)
   - struct
     - [DONE] access
     - [DONE] get
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
