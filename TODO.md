# TODO

## libkc3

 - [x] dlopen inside lib only
 - [x] Typed pointer example : `Fact*`
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


## Release v0.1.15
 - [x] ok


## IKC3 RPC

 - ikc3/
   - [x] ikc3 --server HOST PORT
     - [x] argument parsing
     - [x] socket_buf_init_listen / socket_buf_init_accept
   - [x] ikc3 --client HOST PORT
     - [x] argument parsing
     - [x] socket_buf_init_connect
     - [x] custom REPL with RPC I/O


## LibreSSL/OpenSSL

<https://github.com/bob-beck/libtls/blob/master/TUTORIAL.md>

 - [x] new library
   - [x] explore SSL API for stream socket connections with a certificate.
   - implement
     - `buf_tls.c/h`
     - struct `s_buf_tls`
     - `buf_tls_open_r` opens a tls connection for reading
       - `buf->refill` callback to refill the `tls_open_r` buffer
     - `buf_tls_open_w` opens a tls connection for writing
       - `buf->flush` callback to flush the `tls_open_w` buffer
         - `sw buf_fd_open_w_flush (s_buf *buf)`
     - `buf_tls_close`
   - ikc3 **--tls** --client/server
     - `puts("ikc3: connected with TLS v1.2 to ${g_host} ${g_port}")`
   - [ ] wrap the following functions :
     - [ ] Common TLS API :
       - [x] `#include <tls.h>`
       - [lyzer] `tls_init() != 0`
       - [lyzer] `struct tls_config *config = tls_config_new();`
       - [lyzer] `tls_configure(ctx, config);`
       - [perle] `tls_write(ctx, data, size)`
       - [perle] `tls_read(ctx, data, size);`
       - [perle] `tls_close(ctx);`
       - [ ] `tls_free`
       - [ ] `tls_config_free`
     - [ ] Basic TLS Client :
       - [ ] `tls_config_set_ca_file(config, "/etc/ssl/cert.pem");`
       - [ ] `struct tls *ctx = tls_client();`
       - [ ] `tls_connect_socket(ctx, socket_fd, "hostname");`
     - [ ] Basic TLS Server, see `libtls_server_example.c`
       - [ ] `tls_config_set_cert_file` set server certificate
       - [ ] `tls_config_set_key_file` set server certificate private key
       - [ ] `ctx = tls_server()`
       - [ ] `tls_accept_socket`

## libkc3

 - --pedantic option (env)
 - securelevel
   - 0 = cfn + system + dlopen + eval + def*
   - 1 = eval + def*
   - 2 = ø

## HTTPd
 - [ ] partial requests
 - [ ] SSL

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

