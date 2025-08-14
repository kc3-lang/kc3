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
| Env             |  [ ]   |  [ ]   | Facts           |

[X] : Done
[ ] : Not implemented
[.] : Work in progress
[?] : Has to be tested/verified


## Env init args --dump and --load-dump

 - [ ] `s_marshall * marshall_ops (s_marshall *m, const s_ops *ops);`
 - [ ] `s_marshall_read * marshall_read_ops (s_marshall *m, s_ops *ops);`
 - [X] `s_marshall * marshall_facts (s_marshall *m, const s_facts *x);`
 - [X] `s_marshall_read * marshall_read_facts (s_marshall_read *mr, const s_env *env);`
 - [ ] `s_marshall * marshall_env (s_marshall *m, const s_env *env);`
 - [lyzer] `s_marshall_read * marshall_read_env (s_marshall_read *mr, const s_env *env);`
 - [lpinf] `sw env_dump (const s_env *env, const char *path);`
 - [lpinf] `sw env_dump_load (s_env *env, const char *path);`
 - [thodg] `env_init_args` → `--dump PATH`
 - [thodg] `env_init_args` → `--restore PATH`
