# Module List

Regular lists can be :
 - an element and a list : `[1 | [2]]`
 - multiple elements : `[1, 2, 3]`
 - multiple elements and a list : `[1, 2 | [3, 4]]`
 - the empty list : `[]`

Regular lists end with the empty list : `[1] == [1 | []]`.

You can also contruct dotted lists like in Common Lisp where
the next list pointer is an arbitrary form. E.g. :
 - an element and an element : `[1 | 2]`
 - multiple elements and an element : `[1, 2, 3 | 4]`
 - the empty list and an element : `[[] | 1]`

All these list formats are supported in pattern matching.

## Functions

```
List List.map (List, Fn)
```

```
List List.reverse (List)
```
