# TODO

## LibreSSL/OpenSSL

<https://github.com/bob-beck/libtls/blob/master/TUTORIAL.md>

 - [x] new library
   - [x] explore SSL API for stream socket connections with a certificate.
   - implement
     - [x] `tls_buf.c/h`
     - [x] struct `s_tls_buf`
     - `tls_buf_open_r` opens a tls connection for reading
       [x] - `buf->refill` callback to refill the `tls_open_r` buffer
     - `tls_buf_open_w` opens a tls connection for writing
       [x] - `buf->flush` callback to flush the `tls_open_w` buffer
         - `sw buf_fd_open_w_flush (s_buf *buf)`
     - `tls_buf_close`
   - ikc3 **--tls** --client/server
     - `puts("ikc3: connected with TLS v1.2 to ${g_host} ${g_port}")`
   - [ ] wrap the following functions in lib/kc3/0.1 :
     - [ ] Common TLS API :
       - [x] `#include <tls.h>`
       - [lyzer] `tls_init() != 0`
       - [lyzer] `struct tls_config *config = tls_config_new();`
       - [lyzer] `tls_configure(ctx, config);`
       - [perle] `tls_write(ctx, data, size)`
       - [perle] `tls_read(ctx, data, size);`
       - [perle] `tls_close(ctx);`
       - [x] `tls_free`
       - [x] `tls_config_free` (isn't that tls_config_delete, what's the difference ?)
     - [ ] Basic TLS Client :
       - [x] `tls_config_set_ca_file(config, "/etc/ssl/cert.pem");`
       - [x] `struct tls *ctx = tls_client();`
       - [x] `tls_connect_socket(ctx, socket_fd, "hostname");`
     - [ ] Basic TLS Server, see `libtls_server_example.c`
       - [?] `tls_config_set_cert_file` set server certificate
       - [?] `tls_config_set_key_file` set server certificate private key
       - [?] `ctx = tls_server()`
       - [?] `tls_accept_socket`

## ikc3
 - [ ] TLS
   - command line argument : `--tls`

## libkc3
 - unveil
 - pledge

## HTTPd
 - [ ] SSL
 - [.] partial requests
   - `s_tag * http_request_parse_range(s_str *range_header, s_tag *dest);`
     recoit le header HTTP "Range:" et retourne une liste de tuples
       {debut, fin} .

---

[ ] : Not implemented
[.] : Work in progress
[?] : Waiting review
[x] : Done
