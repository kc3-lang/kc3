# TODO

## Package Debian / Ubuntu / Mint / PopOS

 - [ ] modifier le Makefile kc3 pour faire un target release qui
   crée un chemin `release/v${VER}` et copie `${DIST}` dans ce chemin
   - release depend de dist
 - [ ] faire un target deb qui génere le package Debian a partir de la
   release
   - deb depend de release

## KC3S

 - [ ] Tester si kc3s fonctionne avec les options suivantes :
   - [ ] RPC
     - [ ] --server 127.0.0.1 1026
     - [ ] --client 127.0.0.1 1026
   - [ ] TLS
     - [ ] --tls
     - [ ] --tls --server 127.0.0.1 1026
     - [ ] --tls --client 127.0.0.1 1026

 - [ ] dédupliquer le code entre ikc3 et kc3s
   - [ ] faire un diff
   - [ ] ajouter la compilation conditionnelle pour IKC3 et KC3S
     - [ ] #if KC3_IKC3 / #endif
     - [ ] #if KC3_KC3S / #endif
   - [ ] renommer le fichier ikc3/ikc3.c en ikc3_kc3s.c
   - [ ] créer un fichier ikc3.c avec :
     - [ ] `#define KC3_IKC3`
     - [ ] `#include ikc3_kc3s.c`
   - [ ] remplacer le fichier kc3s/kc3s.c avec :
     - [ ] `#define KC3_IKC3`
     - [ ] `#include "../ikc3/ikc3_kc3s.c"`

## Kpkg / Android JNI

 - Liste des packages JNI :
   - freetype/
   - libbsd/
   - libevent/
   - libffi/
   - libmd/
   - libpng/
   - libressl/

 - Android studio
   - [ ] Installer Android Studio et le dernier NDK Android.
     - [ ] utiliser sdkmanager pour obtenir le dernier NDK Android.
       - [ ] commande : `sdkmanager --install 'ndk;r29'`

## libkc3

 - [ ] Implementer la fonction `s_tag * tag_init_v_1 (s_tag *, const char *);` :
   - `tag_init_v_1(&tagv[0], "123, \"123\", true");`
 - [ ] Implementer la macro `TAG_INIT_V(tagv, ...)`
   - `TAG_INIT_V(&tagv[0], 123, "123", true);`
 - [ ] Implementer la fonction `p_list list_new_v_1 (const char *);` :
   - `list_new_v("123, \"123\", true");`
 - [ ] Implementer la macro `LIST_NEW_V(...)`
   - `LIST_NEW_V(123, "123", true);`

## HTTPd
 - [ ] SSL
   - [ ] ajouter le support de tls dans lib/kc3/0.1/httpd.kc3
 - [ ] pledge(2) support via `KC3.pledge()`
 - [ ] partial requests
   - HTTP 206 partial content

## PDF
 - Le trailer a une entree /Root qui pointe vers un /Catalog
   - /Catalog qui detient un /Pages 
     - le /Pages  detient une array /Kids de /Page et le nombre de pages
       - chaque /Page lie vers son parent, ressources, et content stream, et indique sa taille
         - /Resources : /Font...
         - /Contents : stream (avec BT ET etc)
         - /Parent : lien vers /Pages
         - /MediaBox : taille x et y de la page

## Windows release
 - [x] install kmx_sort and runj using MSYS2
   - [x] sort was renamed to kmx_sort
 - [.] configure
 - [.] build
 - [ ] test
 - [ ] release

## Kpkg / Android JNI

 - [x] Trouver la homepage et le repo git original sur Google de
   chaque package JNI.
   - [x] freetype
     - [Homepage](https://freetype.org/)
     - [Github](https://github.com/freetype/freetype)
     - [Gitlab](https://gitlab.freedesktop.org/freetype/freetype.git)
   - [x]libbsd
     - [Homepage](https://libbsd.freedesktop.org/wiki/)
     - [Github](https://github.com/guillemj/libbsd)
     - [Gitlab](https://gitlab.freedesktop.org/libbsd/libbsd.git)
   - [x] libevent
     - [Homepage](https://libevent.org/)
     - [Github](https://github.com/libevent/libevent)
   - [x] libffi
     - [Homepage](https://sourceware.org/libffi/)
     - [Github](https://github.com/libffi/libffi)
   - [x] libmd
     - [Homepage](https://www.hadrons.org/software/libmd/)
     - [Github](https://github.com/guillemj/libmd)
     - [Git](https://git.hadrons.org/git/libmd.git)
   - [x] libpng
     - [Homepage](https://www.libpng.org/pub/png/libpng.html)
     - [Github](https://github.com/pnggroup/libpng)
     - [Sourceforge](https://git.code.sf.net/p/libpng/code)
   - [x] libressl
     - [Homepage](https://www.libressl.org/)
     - [Github](https://github.com/libressl/portable)

 - Faire un repo git sur https://git.kmx.io/kc3-lang/ pour chaque
   package JNI :
   - Indiquer l'URL du site web officiel et du repo git sur Github dans
     la description Markdown du repo.
   - Forker le repo git original vers git.kmx.io et github.
   - [x] [freetype](https://git.kmx.io/kc3-lang/freetype)
   - [x] [libbsd](https://git.kmx.io/kc3-lang/libbsd)
   - [x] [libevent](https://git.kmx.io/kc3-lang/libevent)
   - [x] [libffi](https://git.kmx.io/kc3-lang/libffi)
   - [x] [libmd](https://git.kmx.io/kc3-lang/libmd)
   - [x] [libpng](https://git.kmx.io/kc3-lang/libpng)
   - [x] [libressl](https://git.kmx.io/kc3-lang/libressl)

 - Faire des packages __kpkg__ dans lib/kc3/0.1/kpkg/repos/ pour chaque
   package JNI :
   - Les packages doivent pointer vers git.kmx.io .
   - [x] freetype/
   - [x] libbsd/
   - [x] libevent/
   - [x] libffi/
   - [x] libmd/
   - [x] libpng/
   - [x] libressl/

 - Compiler les packages sous Linux
   - `kpkg PKG clean uninstall install`
   - [x] freetype/
   - [x] libbsd/
   - [x] libevent/
   - [x] libffi/
   - [x] libmd/
   - [x] libpng/
   - [x] libressl/

## Cleanup branches git
 - [x] `callable`
 - [x] `callable-rebase`
 - [x] `fast`
 - [x] `fast2`
 - [x] `frame`
 - [x] `herunstoofastforus`
 - [x] `httpd`
 - [x] `jeremy_rebased`
 - [x] `kpkg`
 - [x] `marshall`
 - [x] `marshall_before_merge`
 - [x] `marshall_rebase`
 - [x] `pointer`
 - [x] `tls`
 - [x] `tls-rebased`

## MacOS release
 - [x] cairo demo app/_debug
 - [x] build and install sdl2 demo/_debug app
 - [x] bin
   - [x] install dependent libs in `${DESTDIR}${prefix}/lib`
   - [x] fix libs and executables lib search path
 - [x] dmg target
   - [x] large icons
   - [x] arrow

## KC3 RPC with RBACL
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
