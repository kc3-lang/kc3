# C3

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
