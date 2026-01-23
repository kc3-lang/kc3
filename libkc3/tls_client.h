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
#ifndef LIBKC3_TLS_CLIENT_H
#define LIBKC3_TLS_CLIENT_H

#include "types.h"

/* Stack-allocation compatible functions, call kc3_tls_client_clean
   after use. */
void           kc3_tls_client_clean (s_tls_client *tls_client);
s_tls_client * kc3_tls_client_init_connect (s_tls_client *tls_client,
                                            p_tls *ctx,
                                            const s_str *host,
                                            const s_str *port);

#endif /* LIBKC3_TLS_CLIENT_H */
