# TODO

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
   - ajouter le support de tls dans lib/kc3/0.1/httpd.kc3
 - [ ] partial requests
   - HTTP 206 partial content

## Windows release
 - [.] install kmx_sort and runj using MSYS2
   - [ ] sort was renamed to kmx_sort
 - [ ] configure
 - [ ] build
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

---

[x] : Done
[ ] : Not implemented
[.] : Work in progress
[?] : Has to be tested/verified
