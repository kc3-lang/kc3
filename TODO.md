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

 - Trouver le repo git original sur Google de chaque package JNI.
   - freetype : <>
   - libbsd   : <>
   - libevent : <>
   - libffi   : <>
   - libmd    : <>
   - libpng   : <>
   - libressl : <>

 - Faire un repo git sur https://git.kmx.io/kc3-lang/ pour chaque
   package JNI :
   - Indiquer l'URL du site web officiel et du repo git sur Github dans
     la description Markdown du repo.
   - Forker le repo git original vers git.kmx.io .
   - [ ] freetype/
   - [ ] libbsd/
   - [ ] libevent/
   - [ ] libffi/
   - [ ] libmd/
   - [ ] libpng/
   - [ ] libressl/

 - Faire des packages __kpkg__ dans lib/kc3/0.1/kpkg/repos/ pour chaque
   package JNI :
   - Les packages doivent pointer vers git.kmx.io .
   - [ ] freetype/
   - [ ] libbsd/
   - [ ] libevent/
   - [ ] libffi/
   - [ ] libmd/
   - [ ] libpng/
   - [ ] libressl/

 - Android studio
   - [ ] Installer Android Studio et le dernier NDK Android.
   - [x] Desinstaller gradle et mettre un
     alias gradle="rm -f /usr/bin/gradle"

## libkc3
 - [ ] unveil
 - [ ] pledge

## HTTPd
 - [x] Securelevel = 2 !!!
 - [ ] SSL
   - ajouter le support de tls dans lib/kc3/0.1/httpd.kc3
 - [ ] partial requests
   - HTTP 206 partial content

## Cleanup branches git
 - [ ] `callable`
 - [ ] `callable-rebase`
 - [ ] `fast`
 - [ ] `fast2`
 - [ ] `frame`
 - [ ] `herunstoofastforus`
 - [ ] `httpd`
 - [ ] `jeremy_rebased`
 - [ ] `marshall`
 - [ ] `marshall_before_merge`
 - [ ] `marshall_rebase`
 - [ ] `pointer`
 - [ ] `tls`
 - [ ] `tls-rebased`

## MacOS release
 - [x] cairo demo app/_debug
 - [ ] build and install sdl2 demo/_debug app
 - [ ] bin
   - [ ] install dependent libs in `${DESTDIR}${prefix}/lib`
   - [ ] fix libs and executables lib search path

---

[x] : Done
[ ] : Not implemented
[.] : Work in progress
[?] : Has to be tested/verified
