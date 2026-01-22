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
#ifndef KC3_TLS_TLS_CONFIG_H
#define KC3_TLS_TLS_CONFIG_H

#include "types.h"

/* Heap-allocation functions, call *_delete after use. */
void           kc3_tls_config_free (p_tls_config *config);
p_tls_config * kc3_tls_config_new (p_tls_config *dest);

/* Observers. */
s_str * kc3_tls_config_error (p_tls_config *config, s_str *dest);

/* Operators. */
p_tls_config * kc3_tls_config_set_ca_file (p_tls_config *config,
                                           s_str *path,
                                           p_tls_config *dest);
p_tls_config * kc3_tls_config_set_cert_file (p_tls_config *config,
                                             const s_str *file_path,
                                             p_tls_config *dest);
p_tls_config * kc3_tls_config_set_key_file (p_tls_config *config,
                                            const s_str *key_file_path,
                                            p_tls_config *dest);

#endif /* KC3_TLS_TLS_CONFIG_H */
