# TODO

## LibreSSL/OpenSSL

<https://github.com/bob-beck/libtls/blob/master/TUTORIAL.md>

 - [x] new library
   - [x] explore SSL API for stream socket connections with a certificate.
   - implement
     - [x] `tls_buf.c/h`
     - [x] struct `s_tls_buf`
     - `tls_buf_open_r` opens a tls connection for reading
       - [x] `buf->refill` callback to refill the `tls_open_r` buffer
     - `tls_buf_open_w` opens a tls connection for writing
       - [x] `buf->flush` callback to flush the `tls_open_w` buffer
         - `sw buf_fd_open_w_flush (s_buf *buf)`
     - [x] `tls_buf_close`
   - [x] wrap the following functions in lib/kc3/0.1 :
     - [x] Common TLS API :
       - [x] `#include <tls.h>`
       - [x] `tls_init() != 0`
       - [x] `struct tls_config *config = tls_config_new();`
       - [x] `tls_configure(ctx, config);`
       - [x] `tls_write(ctx, data, size)`
       - [x] `tls_read(ctx, data, size);`
       - [x] `tls_close(ctx);`
       - [x] `tls_free`
       - [x] `tls_config_free` (isn't that tls_config_delete, what's the difference ?)
     - [x] Basic TLS Client :
       - [x] `tls_config_set_ca_file(config, "/etc/ssl/cert.pem");`
       - [x] `struct tls *ctx = tls_client();`
       - [x] `tls_connect_socket(ctx, socket_fd, "hostname");`
     - [x] Basic TLS Server, see `libtls_server_example.c`
       - [x] `tls_config_set_cert_file` set server certificate
       - [x] `tls_config_set_key_file` set server certificate private key
       - [x] `ctx = tls_server()`
       - [x] `tls_accept_socket`

 - [x] ikc3 tests
   - [x] copier test/ikc3_test test/tls_test
   - [x] ecrire des tests fonctionnels dans test/tls/

 - [ ] ikc3 **--tls** --client/server

   - [ ] Il faut parser l'argument en ligne de commande --tls dans ikc3/ikc3.c .

     - [ ] Declarer une variable globale `bool g_tls` dans ikc3.c .

     - [ ] Setter la variable globale `bool g_tls` qui indique si
       l'argument --tls est present dans le parsing des arguments en ligne de commande.

   - [ ] Serveur TLS
     - [ ] ikc3 doit changer ses buffers d'entrée/sortie apres avoir
       accepté une connection lorsqu'il est en mode serveur il faut
       remplacer la logique de connection `t_socket` en logique de
       connection `s_tls_server` uniquement dans le cas où
       `g_tls == true && g_server == true`.
   - pour le client
     - [ ] ikc3 doit changer ses buffers d'entrée/sortie apres s'être
       connecté lorsqu'il est en mode serveur il faut remplacer la
       logique de connection `t_socket` en logique de connection
       `s_tls_client` uniquement dans le cas où
       `g_tls == true && g_client == true`.
   - [ ] Afficher des messages indiquant la progression de la connection
     TLS indiquant la version de TLS utilisée pour la connection :
     - [ ] Pour le serveur :
       - [ ] "ikc3: TLS server: listening on HOST PORT"
       - [ ] "ikc3: TLS server: client connected: HOST PORT TLS-4.2"
     - [ ] Pour le client :
       - [ ] "ikc3: TLS client: connected to HOST PORT TLS-4.2"

## libkc3
 - [ ] unveil
 - [ ] pledge

## HTTPd
 - [ ] partial requests
   - HTTP 206 partial content
 - [ ] SSL
   - ajouter le support de tls dans lib/kc3/0.1/httpd.kc3
 - [x] Securelevel = 2


---

[x] : Done
[ ] : Not implemented
[.] : Work in progress
[?] : Has to be tested/verified

