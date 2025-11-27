# TODO

## PDF
 - [ ] Primitives de dessin
   - [ ] texte (couleur, font, size, str)
   - [ ] rectangle (couleur, x, y, w, h)
   - [ ] image
     - [ ] png
     - [ ] jpeg
 - Le trailer a une entree /Root qui pointe vers un /Catalog
   - /Catalog qui detient un /Pages 
     - le /Pages detient une array /Kids de /Page et le nombre de
       pages
       - chaque /Page lie vers son parent, ressources, et content
         stream, et indique sa taille
         - /Resources : /Font...
         - /Contents : stream (avec BT ET etc)
         - /Parent : lien vers /Pages
         - /MediaBox : taille x et y de la page

## KC3S

 - [x] Tester si kc3s fonctionne avec les options suivantes :
   - [x] RPC
     - [x] --server 127.0.0.1 1026
     - [x] --client 127.0.0.1 1026
   - [x] TLS
     - [x] --tls
     - [x] --tls --server 127.0.0.1 1026
     - [x] --tls --client 127.0.0.1 1026

 - [x] dédupliquer le code entre ikc3 et kc3s
   - [x] faire un diff
   - [x] ajouter la compilation conditionnelle pour IKC3 et KC3S
     - [x] #if KC3_IKC3 / #endif
     - [x] #if KC3_KC3S / #endif
   - [x] renommer le fichier ikc3/ikc3.c en ikc3_kc3s.c
   - [x] créer un fichier ikc3.c avec :
     - [x] `#define KC3_IKC3`
     - [x] `#include ikc3_kc3s.c`
   - [x] remplacer le fichier kc3s/kc3s.c avec :
     - [x] `#define KC3_IKC3`
     - [x] `#include "../ikc3/ikc3_kc3s.c"`

## libkc3

 - [x] Verifier la fonction `p_list list_new_1 (const char *);` :
   - `list_new_1("[123, \"123\", true]");`
 - [x] Implementer la macro `LIST_NEW_V(...)`
   - `LIST_NEW_V(123, "123", true);`
 - [ ] Implémenter la macro `TAG_TYPE(value)` :
   - `TAG_TYPE(my_var) -> e_tag_type`
 - [ ] Implémenter la macro `TAG_INIT(value)` :
   - `TAG_INIT(&tag, my_var) -> determine type -> `

## HTTPd
 - [ ] partial requests
   - HTTP 206 partial content
 - [ ] SSL
   - [ ] ajouter le support de tls dans lib/kc3/0.1/httpd.kc3
 - [ ] XXX pledge(2) support via `KC3.pledge()`

## Windows release
 - [x] install kmx_sort and runj using MSYS2
   - [x] sort was renamed to kmx_sort
 - [.] configure
 - [.] build
 - [ ] test
 - [ ] release

## KC3 RPC with RBACL
 - XXX
 - On peut recuperer du code KC3 du reseau et l'executer a distance,
   c'est deja quelque-chose, maintenant il nous faut des RBACL : rule-
   based access control lists pour controler `KC3.require ModuleName`
   et `KC3.load(file_path)` et on pourra accepter du code qui n'utilise
   que certains module ou un certain DSL si on ajoute des controles
   d'accès pour les fonctions et tous les éléments du langage.

 - Pour rester generique in-fine il nous faut une fonction qui
   prend du code source en parametre et nous retourne un booléen,
   c'est un prédicat de permissions, une _rule_.

   - On peut construire cette fonction a partir d'une liste d'acces
     controlé. La liste se traduit en une fonction avec le module
     `ACL`, par exemple : `ACL.rule(list_acl) -> callable_rule`.

   - l'operateur special `defrule ident_name = callable_rule`
     prend un nom de rule, et une fonction (callable) qui détermine
     si un code source est valide selon la rule nommée par ce nom.
     Ces deux élements forment une rule basique qui est ajoutée
     à la db en graphe de l'environnement pour une application
     immediate de la rule dans le controle d'acces aux modules et
     fonctions dans le chargement et l'execution du code source
     futur. Cela permet un verouillage global de l'interpreteur KC3
     avec des regles aussi fines que possible.

   - l'operateur special `with_rule callable_rule do ... end` est un
     état de l'environnement sur la stack qui met en vigeur une rule
     (callable) arbitraire mais seulement dans le bloc de code passé
     en parametre (arbitraire lui aussi). Cela permet l'execution
     de code arbitraire avec une reduction aussi fine que possible des
     permissions d'execution selon un modele auditable dans le code
     source de l'application.

   - la fonction `rule_get(sym_name) -> callable_rule` prend en
     parametre un nom de rule globale et retourne la fonction
     (callable) associée dans la base de données en triplets de
     l'environnement KC3 en cours.

   - pour verifier si une rule (callable) permet un certain code
     source (sous forme de `Tag` KC3) il suffit d'appeler la
     la rule sur le code et on obtient un booléen `true` ou `false`.

---

[x] : Done
[ ] : Not implemented
[.] : Work in progress
[?] : Has to be tested/verified
