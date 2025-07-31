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
| Array           |   🚧   |   ❌   |
| Ident           |   ✅   |   ❌   |
| Call            |   ✅   |   ❌   | pcallable       |
| Do Block        |   ✅   |   ❌   | call            |
| Map             |   ❌   |   ❌   | Tag             |
| Fact            |   ❌   |   ❌   |
| Complex         |   ❌   |   ❌   |
| Cow             |   ❌   |   ❌   |
| Ptr free        |   ❌   |   ❌   |
| PCallable       |   🚧   |   ❌   | u_callable_data |
| PVar            |   ❌   |   ❌   |
| Quote           |   ❌   |   ❌   |
| Ratio           |   ❌   |   ❌   |
| PStruct Type    |   ❌   |   ❌   |
| PStruct         |   ❌   |   ❌   |
| Fact            |   ❌   |   ❌   |
| Sym             |   ✅   |   ❌   |
| Time            |   ❌   |   ❌   | Tag             |
| Unquote         |   ❌   |   ❌   | Tag             |
| Var             |   ❌   |   ❌   | Tag             |
| PTag            |   ❌   |   ❌   | Tag             |
... (others)

✅ : Done
❌ : Not implemented
🚧 : Work in progress
