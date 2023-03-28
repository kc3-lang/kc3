# C3 v0.1

C3 is a programming language project, inspired by C, Elixir and
Common Lisp. It could be described as C with Elixir macros, pattern
matching, and a semantic object system. The idea is to plug macros and
pattern matching into C99 and use them to implement the graph database.

There is no garbage collector.

```
defmodule C3 do
  
  def compile (c3, opts) when is_string(c3) do
    c3
    |> C3.parse(opts)
    |> C3.compile()
  end

  def eval (c3, env, opts) when is_string(c3) do
    c3
    |> C3.parse(opts)
    |> C3.eval(env)
  end

  defguard is_c3 (x) do
    is_tuple(x)
  end

  def compile (c3, opts) when is_c3(c3) do
    c3
    |> C3.to_assembly(opts)
    |> Assembly.to_elf(opts)
  end

  def eval (c3, env) when is_c3(c3) do
    [...]
  end
end
```

## Structure

### libc3

C3 is implemented using libc3 a small C99 library implementing the core
of the language.

The library includes a parser and an interpreter for C3 code in C structures.

#### Parser

The parser is recursive descent for now.

#### AST

The AST is represented as C3 data structures and can be meta-programmed in C.

#### Interpreter

Under development.


### ic3

Interactive shell. Under development.


### c3s

Script interpreter. Under development.


## TODO

 - libc3
   - atomic operations
   - DONE triple serial id
   - with ignore variables
   - boolean operators
   - arrays
   - defmodule
   - structs
   - errors (setjmp, longjmp)
     - stacktrace
       - ffi ?
         - libdwarf
   - control structures
   - functions
     - return
     - def
     - funcall
     - & &1
     - DONE fn () {}
   - macros
   - livebook
     - gaussian
   - buf_sha256
   - buf_popen
   - DONE ci
   - DONE c function call
   - DONE infix operators
   - DONE load
   - DONE `test_file_compare`
   - DONE ffi
     - DONE types
     - DONE funcall
