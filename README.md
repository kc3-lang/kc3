# C3 v0.1

C3 is a programming language, inspired by C89, Elixir and Common Lisp.
It aims to replace C89 with an Elixir syntax.

```
defmodule C3 do
  def compile (c3, opts) when is_string(c3) do
    |> C3.parse(c3, opts)
    |> C3.to_c89(opts)
    |> C89.compile(opts)
  end
end
```

## TODO

 - libc3
   - `buf_parse_fact`
   - dump
   - errors (setjmp, longjmp) -> jc
     - stacktrace
   - log
   - load
   - pattern matching (xor xor) -> baptiste
   - `test_file_compare`
   - ffi
     - types
     - funcall
   - defmodule
   - funcall
