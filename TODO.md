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
| Float           |   ✅   |   ❌   |
| Ident           |   ✅   |   ❌   |
| Call            |   ✅   |   ❌   | pcallable       |
| Do Block        |   ✅   |   ❌   | call            |
| Map             |   ✅   |   ❌   | Tag             |
| Sym             |   ✅   |   ❌   |
| PCallable       |   ✅   |   ❌   | u_callable_data |
| cfn             |   ✅   |   ❌   |
| fn              |   ✅   |   ❌   |
| Array           |   ❌   |   ❌   |
| Fact            |   ❌   |   ❌   |
| Complex         |   ❌   |   ❌   |
| Cow             |   ❌   |   ❌   |
| Ptr free        |   ❌   |   ❌   |
| PVar            |   ❌   |   ❌   |
| Quote           |   ❌   |   ❌   |
| Ratio           |   ❌   |   ❌   |
| PStruct Type    |   ❌   |   ❌   |
| PStruct         |   ❌   |   ❌   |
| Fact            |   ❌   |   ❌   |
| Time            |   ❌   |   ❌   | Tag             |
| Unquote         |   ❌   |   ❌   | Tag             |
| Var             |   ❌   |   ❌   | Tag             |
| PTag            |   ❌   |   ❌   | Tag             |
... (others)

✅ : Done
❌ : Not implemented
🚧 : Work in progress
