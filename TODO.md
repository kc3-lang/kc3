# TODO

## libkc3

 - [?] Generic typed pointers : `MyType*`
   - [x] `buf_inspect_pointer`
   - [x] `buf_inspect_pointer_size`
   - [x] `hash_update_pointer`
   - [x] `pointer_init_copy`
   - [x] `marshall_read_pointer`
   - [x] `marshall_pointer`
   - [x] `g_sym_Pointer`
   - [x] `sym_is_pointer_type`
   - [x] `compare_pointer`
   - [x] `sym_to_tag_type`
     - `sym_is_pointer_type() → TAG_POINTER`
   - [x] `sym_must_clean`
     - `sym_is_pointer_type() → false`
   - [x] `data_*`
     - [x] `data_buf_inspect`
     - [x] `data_buf_inspect_size`
     - [x] `data_clean`
     - [x] `data_compare`
     - [x] `data_hash_update`
     - [x] `data_init_cast`
     - [x] `data_init_copy`
   - [x] `pointer_init_cast`


## LibreSSL/OpenSSL

<https://github.com/bob-beck/libtls/blob/master/TUTORIAL.md>

 - [x] new library
   - [x] explore SSL API for stream socket connections with a certificate.
   - implement
     - [x] `tls_buf.c/h`
     - [x] struct `s_tls_buf`
     - `tls_buf_open_r` opens a tls connection for reading
       - [.] `buf->refill` callback to refill the `tls_open_r` buffer
     - `tls_buf_open_w` opens a tls connection for writing
       - [.] `buf->flush` callback to flush the `tls_open_w` buffer
         - `sw buf_fd_open_w_flush (s_buf *buf)`
     - [ ] `tls_buf_close`
   - ikc3 **--tls** --client/server
     - `puts("ikc3: connected with TLS v1.2 to ${g_host} ${g_port}")`
   - [ ] wrap the following functions in lib/kc3/0.1 :
     - [ ] Common TLS API :
       - [x] `#include <tls.h>`
       - [x] `tls_init() != 0`
       - [x] `struct tls_config *config = tls_config_new();`
       - [?] `tls_configure(ctx, config);`
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
   - [ ] ikc3 tests
     - [ ] copier test/ikc3_test test/tls_test
     - [ ] ecrire des tests fonctionnels dans test/tls/

## libkc3
 - [ ] unveil
 - [ ] pledge

## HTTPd
 - [ ] partial requests
   - HTTP 206 partial content
 - [ ] SSL
   - ajouter le support de tls dans lib/kc3/0.1/httpd.kc3
 - [ ] Securelevel = 2
   - [ ] definir toutes les fonctions locales dans leur module avec def
   

## Marshall

 - [x] env_dump/restore
   - [x] test_httpd
   - [x] www
 - [x] StructType
   - [x] we should not record offsets in struct type but rather just
     the map and recalculate offsets and size at `marshall_read`
     - [x] must_clean

|       Type      | Write  |  Read  |      Needs      |
| --------------- | ------ | ------ | --------------- |
| Signed/unsigned |  [x]   |  [x]   |
| Character       |  [x]   |  [x]   |
| Str             |  [x]   |  [x]   |
| Pointer         |  [x]   |  [x]   |
| Tag             |  [x]   |  [x]   | Everything else |
| Tuple           |  [x]   |  [x]   |
| Float           |  [x]   |  [x]   |
| Ident           |  [x]   |  [x]   |
| Call            |  [x]   |  [x]   | pcallable       |
| Do Block        |  [x]   |  [x]   | call            |
| Map             |  [x]   |  [x]   | Tag             |
| Sym             |  [x]   |  [x]   | str
| Callable        |  [x]   |  [x]   | u_callable_data |
| PCallable       |  [x]   |  [x]   | callable        |
| cfn             |  [x]   |  [x]   |
| fn              |  [x]   |  [x]   |
| Ratio           |  [x]   |  [x]   |
| Quote           |  [x]   |  [x]   |
| Var             |  [x]   |  [x]   | Tag             |
| PVar            |  [x]   |  [x]   |
| Complex         |  [x]   |  [x]   |
| Unquote         |  [x]   |  [x]   | Tag             |
| Time            |  [x]   |  [x]   | Tag             |
| Ptr             |  [x]   |  [x]   |
| Ptr free        |  [x]   |  [x]   |
| Array           |  [x]   |  [x]   |
| Fact            |  [x]   |  [x]   |
| Struct          |  [x]   |  [x]   |
| StructType      |  [x]   |  [x]   |
| PStruct         |  [x]   |  [x]   |
| PStructType     |  [x]   |  [x]   |
| PTag            |  [x]   |  [x]   | Tag             |
| Facts           |  [x]   |  [x]   |
| Env             |  [x]   |  [x]   | Facts           |

[x] : Done
[ ] : Not implemented
[.] : Work in progress
[?] : Has to be tested/verified

