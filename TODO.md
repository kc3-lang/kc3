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
| Ratio           |   ✅   |   ❌   |
| Quote           |   ✅   |   ❌   |
| Var             |   ✅   |   ❌   | Tag             |
| PVar            |   ✅   |   ❌   |
| Complex         |   ✅   |   ❌   |
| Array           |   ❌   |   ❌   |
| Unquote         |   ❌   |   ❌   | Tag             |
| Fact            |   ❌   |   ❌   |
| Cow             |   ❌   |   ❌   |
| Ptr free        |   ❌   |   ❌   |
| PStruct Type    |   ❌   |   ❌   |
| PStruct         |   ❌   |   ❌   |
| Fact            |   ❌   |   ❌   |
| Time            |   ❌   |   ❌   | Tag             |
| PTag            |   ❌   |   ❌   | Tag             |
... (others)

✅ : Done
❌ : Not implemented
🚧 : Work in progress
