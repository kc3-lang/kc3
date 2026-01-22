/* kc3
 * Copyright from 2022 to 2025 kmx.io <contact@kmx.io>
 *
 * Permission is hereby granted to use this software granted the above
 * copyright notice and this permission paragraph are included in all
 * copies and substantial portions of this software.
 *
 * THIS SOFTWARE IS PROVIDED "AS-IS" WITHOUT ANY GUARANTEE OF
 * PURPOSE AND PERFORMANCE. IN NO EVENT WHATSOEVER SHALL THE
 * AUTHOR BE CONSIDERED LIABLE FOR THE USE AND PERFORMANCE OF
 * THIS SOFTWARE.
 */
#ifndef KC3_TLS_SERVER_H
#define KC3_TLS_SERVER_H

#include "types.h"

/* Stack-allocation compatible functions, call kc3_tls_server_clean
   after use. */
void           kc3_tls_server_clean (s_tls_server *tls_server);
s_tls_server * kc3_tls_server_init_accept (s_tls_server *tls_server,
                                           p_socket socket,
                                           p_tls *ctx);

/* Heap-allocation functions, call kc3_tls_server_delete
   after use. */
/*
void           kc3_tls_server_delete (p_tls_server *tls_server);
p_tls_server * kc3_tls_server_new_accept (p_tls_server *pserver,
                                          p_socket socket,
                                          p_tls *ctx);
*/

#endif /* KC3_TLS_SERVER_H */
