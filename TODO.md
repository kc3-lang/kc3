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
| Tuple           |   🚧   |   ❌   |
| Float           |   🚧   |   ❌   |
| Array           |   🚧   |   ❌   |
| Ident           |   ✅   |   ❌   |
| Call            |   ✅   |   ❌   | pcallable       |
| PCallable       |   🚧   |   ❌   | u_callable_data |
| Do Block        |   ✅   |   ❌   | call            |
| Map             |   ❌   |   ❌   | Tag             |
| Fact            |   ❌   |   ❌   |
| Complex         |   ❌   |   ❌   |
| Cow             |   ❌   |   ❌   |
| PTag            |   ✅   |   ✅   | Tag             |
| Ptr free        |   ❌   |   ❌   |
| PVar            |   ❌   |   ❌   |
| Quote           |   ❌   |   ❌   |
| Ratio           |   ❌   |   ❌   |
| Struct          |   ❌   |   ❌   |
| PStruct Type    |   ❌   |   ❌   |
| Sym             |   ✅   |   ❌   |
| Fact            |   ❌   |   ❌   |
| Sym             |   ✅   |   ❌   |
| Time            |   ❌   |   ❌   | Tag             |
| Unquote         |   ❌   |   ❌   | Tag             |
| Var             |   ❌   |   ❌   | Tag             |
| Tag             |   ✅   |   ✅   | Everything else |
... (others)

✅ : Done
❌ : Not implemented
🚧 : Work in progress
