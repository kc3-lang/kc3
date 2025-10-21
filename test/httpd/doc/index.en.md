# [The KC3 programming language documentation](https://kc3-lang.org/doc)

KC3 stands for "kmx.io C11 with a triple store" and is a
programming language based on C11 and a triple store (facts).

Core features of the language require the triple store to be fast,
the operations on the triple store are O(log(n, 2.5)) so for
one billion triples the number of instructions would be less than 30.

The triple store is content addressed memory : you can add data and
it will be deduplicated (even for structs) and you can remove data
that you know might be in the database.

Caveats : KC3 is still a prototype and we will phase optimizations in
when more money comes along.

## Index

[1 KC3](1_KC3/) core language.

[2 HTTPd](2_HTTPd/) web server.

[3 Guides](3_Guides/) about KC3.

## Other links

[KC3 releases](/release) are created for each new version of KC3.

[kmx.io](https://www.kmx.io/) is the company that created and
maintains the KC3 programming language.

[kmxgit](https://git.kmx.io/) is the git forge by kmx.io where we
host the [KC3 sources git repository](https://git.kmx.io/kc3-lang/kc3).
