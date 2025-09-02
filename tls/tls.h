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

/* Library initialization. */
bool kc3_tls_init (void);

/* Heap-allocation functions, call *_delete after use. */
s_pointer * kc3_tls_config_new (s_pointer *dest);

/* Operating system helpers. */
s_str * kc3_tls_ca_cert_path (s_str *dest);

void kc3_tls_free (struct tls *ctx);

#endif /* KC3_TLS_H */
