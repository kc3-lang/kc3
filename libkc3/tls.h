/* kc3
 * Copyright from 2022 to 2026 kmx.io <contact@kmx.io>
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
#ifndef LIBKC3_TLS_H
#define LIBKC3_TLS_H

#include <tls.h>
#include "types.h"

/* Library functions. */
void    kc3_tls_free (p_tls *ctx);
s_tag * kc3_tls_init (s_tag *dest);

/* Operators. */
p_tls * kc3_tls_accept_socket (p_tls *ctx, p_tls *client_ctx,
                               p_socket client_fd);
p_tls * kc3_tls_client (p_tls *dest);
p_tls * kc3_tls_close (p_tls *ctx, p_tls *dest);
p_tls * kc3_tls_connect_socket (p_tls *ctx, p_socket sockfd,
                                const s_str *hostname, p_tls *dest);
p_tls * kc3_tls_server (p_tls *dest);

p_tls * kc3_tls_configure (p_tls *ctx, p_tls_config *cfg, p_tls *dest);

/* Operating system helpers. */
s_str * kc3_tls_ca_cert_path (s_str *dest);

#endif /* LIBKC3_TLS_H */
