# TODO

### Refactor

- [x] serialise.c/h -> marshall.c/h
- [ ] deserialize.c/h -> marshall_read.c/h
- [x] s_serialize -> s_marshall

### Marshall

|       Type      | Write  |  Read  |      Needs      |
| --------------- | ------ | ------ | --------------- |
| Signed/unsigned |   ✅   |   ✅   |
| Character       |   ✅   |   ✅   |
| Str             |   ✅   |   ✅   |
| Pointer         |   ✅   |   ✅   |
| Tag             |   ✅   |   ✅   | Everything else |
| Tuple           |   ✅   |   ❌   |
| Float           |   ✅   |   ✅   |
| Ident           |   ✅   |   ❌   |
| Call            |   ✅   |   ❌   | pcallable       |
| Do Block        |   ✅   |   ❌   | call            |
| Map             |   ✅   |   ❌   | Tag             |
| Sym             |   ✅   |   ❌   |
| Callable        |   ✅   |   🚧   | u_callable_data |
| PCallable       |   ✅   |   ❌   | callable |
| cfn             |   ✅   |   ❌   |
| fn              |   ✅   |   ❌   |
| Ratio           |   ✅   |   ❌   |
| Quote           |   ✅   |   ❌   |
| Var             |   ✅   |   ❌   | Tag             |
| PVar            |   ✅   |   ❌   |
| Complex         |   ✅   |   ❌   |
| Unquote         |   ✅   |   ❌   | Tag             |
| Time            |   🚧   |   ❌   | Tag             |
| Ptr             |   ❌   |   ❌   |
| Ptr free        |   ❌   |   ❌   |
| Array           |   ❌   |   ❌   |
| Fact            |   ❌   |   ❌   |
| Cow             |   ❌   |   ❌   |
| PStruct Type    |   ❌   |   ❌   |
| PStruct         |   ❌   |   ❌   |
| Fact            |   ❌   |   ❌   |
| PTag            |   ❌   |   ❌   | Tag             |
... (others)

✅ : Done
❌ : Not implemented
🚧 : Work in progress
