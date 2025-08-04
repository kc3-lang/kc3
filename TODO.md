# TODO

## marshall_read (lyzer)
 - Match marshall\_read_ functions from marshall_ functions

## Marshall

|       Type      | Write  |  Read  |      Needs      |
| --------------- | ------ | ------ | --------------- |
| Signed/unsigned |   ✅   |   ✅   |
| Character       |   ✅   |   ✅   |
| Str             |   ✅   |   ✅   |
| Pointer         |   ✅   |   ✅   |
| Tag             |   ✅   |   ✅   | Everything else |
| Tuple           |   ✅   |   ❓   |
| Float           |   ✅   |   ✅   |
| Ident           |   ✅   |   ❓   |
| Call            |   ✅   |   ❓   | pcallable       |
| Do Block        |   ✅   |   ❓   | call            |
| Map             |   ✅   |   ❓   | Tag             |
| Sym             |   ✅   |   ✅   | str
| Callable        |   ✅   |   ❓   | u_callable_data |
| PCallable       |   ✅   |   ❓   | callable |
| cfn             |   ✅   |   ❓   |
| fn              |   ✅   |   ❓   |
| Ratio           |   ✅   |   ❓   |
| Quote           |   ✅   |   ❓   |
| Var             |   ✅   |   ❓   | Tag             |
| PVar            |   ✅   |   ❓   |
| Complex         |   ✅   |   ❓   |
| Unquote         |   ✅   |   ❓   | Tag             |
| Time            |   ✅   |   ❓   | Tag             |
| Ptr             |   ✅   |   ❓   |
| Ptr free        |   ✅   |   ❓   |
| Array           |   ❌   |   ❓   |
| Fact            |   ✅   |   ❓   |
| Cow             |   ❌   |   ❓   |
| PStruct         |   ❌   |   ❓   |
| PStruct Type    |   ❌   |   ❓   |
| PTag            |   ✅   |   ❓   | Tag             |
... (others)

✅ : Done
❌ : Not implemented
🚧 : Work in progress
❓ : Has to be tested/verified by leader

