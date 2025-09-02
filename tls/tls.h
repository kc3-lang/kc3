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
#ifndef KC3_TLS_H
#define KC3_TLS_H

#include <tls.h>
#include "../libkc3/types.h"
#include "types.h"
#include "../socket/types.h"

/* Library initialization. */
bool kc3_tls_init (void);

/* Heap-allocation functions, call *_delete after use. */
p_tls_config * kc3_tls_config_new (p_tls_config *dest);

/* Operators. */
bool kc3_tls_connect_socket (p_tls ctx, t_socket sockfd,
                             const s_str *hostname);

/* Operating system helpers. */
s_str * kc3_tls_ca_cert_path (s_str *dest);

#endif /* KC3_TLS_H */
