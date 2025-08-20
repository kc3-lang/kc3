# TODO

## libkc3

 - [x] dlopen inside lib only
 - [x] Typed pointer example : `Fact*`
 - [ ] Generic typed pointers : `MyType*`
   - [ ] `sym_is_pointer_type`

## Marshall

 - env_dump/restore
   - [x] test_httpd
   - [x] www
 - StructType
   - we should not record offsets in struct type but rather just
     the map and recalculate offsets and size at `marshall_read`


|       Type      | Write  |  Read  |      Needs      |
| --------------- | ------ | ------ | --------------- |
| Signed/unsigned |  [x]   |  [x]   |
| Character       |  [x]   |  [x]   |
| Str             |  [x]   |  [x]   |
| Pointer         |  [x]   |  [x]   |
| Tag             |  [x]   |  [x]   | Everything else |
| Tuple           |  [x]   |  [x]   |
| Float           |  [x]   |  [x]   |
| Ident           |  [x]   |  [x]   |
| Call            |  [x]   |  [x]   | pcallable       |
| Do Block        |  [x]   |  [x]   | call            |
| Map             |  [x]   |  [x]   | Tag             |
| Sym             |  [x]   |  [x]   | str
| Callable        |  [x]   |  [x]   | u_callable_data |
| PCallable       |  [x]   |  [x]   | callable        |
| cfn             |  [x]   |  [x]   |
| fn              |  [x]   |  [x]   |
| Ratio           |  [x]   |  [x]   |
| Quote           |  [x]   |  [x]   |
| Var             |  [x]   |  [x]   | Tag             |
| PVar            |  [x]   |  [x]   |
| Complex         |  [x]   |  [x]   |
| Unquote         |  [x]   |  [x]   | Tag             |
| Time            |  [x]   |  [x]   | Tag             |
| Ptr             |  [x]   |  [x]   |
| Ptr free        |  [x]   |  [x]   |
| Array           |  [x]   |  [x]   |
| Fact            |  [x]   |  [x]   |
| Struct          |  [x]   |  [x]   |
| StructType      |  [.]   |  [.]   |
| PStruct         |  [x]   |  [x]   |
| PStructType     |  [x]   |  [x]   |
| PTag            |  [x]   |  [x]   | Tag             |
| Facts           |  [x]   |  [x]   |
| Env             |  [x]   |  [x]   | Facts           |

[x] : Done
[ ] : Not implemented
[.] : Work in progress
[?] : Has to be tested/verified

