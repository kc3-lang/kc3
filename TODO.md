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

## LibreSSL/OpenSSL

<https://github.com/bob-beck/libtls/blob/master/TUTORIAL.md>

 - [x] new library
   - [x] explore SSL API for stream socket connections with a certificate.
   - [ ] wrap the following functions :
     - [ ] Basic TLS Client :
```c
#include <tls.h>

struct tls_config *config = tls_config_new();
tls_config_set_ca_file(config, "/etc/ssl/cert.pem");

struct tls *ctx = tls_client();
tls_configure(ctx, config);

// Connect to existing socket
tls_connect_socket(ctx, socket_fd, "hostname");

// Use like regular read/write
tls_write(ctx, "GET / HTTP/1.0\r\n\r\n", 18);
tls_read(ctx, buffer, sizeof(buffer));

tls_close(ctx);
```
     - [ ] Basic TLS Server :

## HTTPd
 - [ ] partial requests
 - [ ] SSL

## RPC
 - [ ] lib/kc3/0.1/rpc.kc3
 - [ ] libkc3_rpc.so (add to `make lib_links`)

## Marshall

 - env_dump/restore
   - [x] test_httpd
   - [x] www
 - StructType
   - we should not record offsets in struct type but rather just
     the map and recalculate offsets and size at `marshall_read`

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

