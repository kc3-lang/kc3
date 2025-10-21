# KC3

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
