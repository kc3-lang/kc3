---
title: 3. Map
---

# Maps

KC3 maps are like Elixir maps, they are key-values enclosed in `%{}` :

```
ikc3> a = %{id: 1, title: "My title", message: "Hello, world !"}
%{id: 1, title: "My title", message: "Hello, world !"}
```

Destructuring works with maps to extract values :

```
ikc3> %{id: id, title: "My title", message: message} = ^ a
%{id: 1, title: "My title", message: "Hello, world !"}
ikc3> id
1
ikc3> message
"Hello, world !"
```


You can use the dot syntax to access map values from a `Sym` key :

```
ikc3> a = %{id: 1, title: "My title", message: "Hello, world !"}
%{id: 1, title: "My title", message: "Hello, world !"}
ikc3> a.id
1
ikc3> a.message
"Hello, world !"
```

You can also use the `KC3.access` function for the same result :

```
ikc3> a = %{id: 1, title: "My title", message: "Hello, world !"}
%{id: 1, title: "My title", message: "Hello, world !"}
ikc3> access(a, :id)
1
ikc3> access(a, :message)
"Hello, world !"
```
