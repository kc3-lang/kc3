# TODO

## libkc3

 - Typed pointers (Buf*)
 - dlopen inside lib only

## Marshall

 - test_httpd
 - www

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
| PStruct         |  [x]   |  [x]   |
| PStruct Type    |  [x]   |  [x]   |
| PTag            |  [x]   |  [ ]   | Tag             |
| Facts           |  [x]   |  [X]   |
| Env             |  [x]   |  [x]   | Facts           |

[X] : Done
[ ] : Not implemented
[.] : Work in progress
[?] : Has to be tested/verified

