# [Documentation du langage de programmation KC3](https://kc3-lang.org/doc)

KC3 signifie "kmx.io C11 avec des triplets" et est un
langage de programmation basé sur C11 et des triplets (des faits).

Les fonctionnalités principales du langage reposent sur l'accès aux
triplets, qui doit être rapide : les opérations effectuées sont en
O(log(n, 2.5)), ce qui signifie que pour un milliard de triplets,
le nombre d'instructions nécessaires serait inférieur à 30.

Le magasin de triplets est une mémoire adressée par le contenu : vous
pouvez ajouter des données et elles seront dédupliquées (même pour des
structures), et vous pouvez retirer des données qui peuvent se trouver
dans la base.

Remarques : KC3 est encore un prototype, et des optimisations seront
ajoutées progressivement au fur et à mesure que le financement le
permettra.

## Index

[1 KC3](1_KC3/) : langage principal.

[2 HTTPd](2_HTTPd/) : serveur web.

[3 Guides](3_Guides/) : guides à propos de KC3.

## Autres liens

[Versions de KC3](/release) : chaque nouvelle version de KC3 est publiée ici.

[kmx.io](https://www.kmx.io/) : la société à l'origine du développement de KC3.

[kmxgit](https://git.kmx.io/) : dépôt de code source et projets liés à KC3.
