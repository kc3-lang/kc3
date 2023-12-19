# C3

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
