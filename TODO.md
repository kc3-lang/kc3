# TODO

## marshall_read
 - Match marshall\_read_ functions from marshall_ functions
 - copy marshall_read_pcallable for callable_new_ref for
   other p* functions.

## Marshall

|       Type      | Write  |  Read  |      Needs      |
| --------------- | ------ | ------ | --------------- |
| Signed/unsigned |  [X]   |  [X]   |
| Character       |  [X]   |  [X]   |
| Str             |  [X]   |  [X]   |
| Pointer         |  [X]   |  [X]   |
| Tag             |  [X]   |  [X]   | Everything else |
| Tuple           |  [X]   |  [X]   |
| Float           |  [X]   |  [X]   |
| Ident           |  [X]   |  [X]   |
| Call            |  [X]   |  [X]   | pcallable       |
| Do Block        |  [X]   |  [X]   | call            |
| Map             |  [X]   |  [X]   | Tag             |
| Sym             |  [X]   |  [X]   | str
| Callable        |  [X]   |  [X]   | u_callable_data |
| PCallable       |  [X]   |  [X]   | callable        |
| cfn             |  [X]   |  [X]   |
| fn              |  [X]   |  [X]   |
| Ratio           |  [X]   |  [X]   |
| Quote           |  [X]   |  [X]   |
| Var             |  [X]   |  [X]   | Tag             |
| PVar            |  [X]   |  [X]   |
| Complex         |  [X]   |  [X]   |
| Unquote         |  [X]   |  [X]   | Tag             |
| Time            |  [X]   |  [X]   | Tag             |
| Ptr             |  [X]   |  [X]   |
| Ptr free        |  [X]   |  [X]   |
| Array           |  [X]   |  [X]   |
| Fact            |  [X]   |  [?]   |
| PStruct         |  [X]   |  [X]   |
| PStruct Type    |  [X]   |  [X]   |
| PTag            |  [X]   |  [?]   | Tag             |
| Facts           |  [ ]   |  [ ]   |
... (others)

[X] : Done
[ ] : Not implemented
[.] : Work in progress
[?] : Has to be tested/verified


Array :

```elixir
my_array = (U8[]) {1, 2, 3, 4}
```

## Facts operations with binary = true

 - [ ] `facts_dump`
 - [ ] `facts_dump_file`
 - [ ] `facts_load`
 - [ ] `facts_load_file`
 - [ ] `facts_log_add`
 - [ ] `facts_log_remove`
 - [ ] `facts_open_buf`
 - [ ] `facts_open_file`
 - [ ] `facts_open_file_create`
 - [ ] `facts_open_log`
 - [ ] `facts_save_file`

