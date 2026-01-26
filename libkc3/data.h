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
/**
 * @file void.h
 * @brief Generic (void *) operations.
 *
 * Functions to manage (void *) operations.
 */
#ifndef LIBKC3_DATA_H
#define LIBKC3_DATA_H

#include "types.h"

sw     data_buf_inspect (s_buf *buf, const s_sym *type, const void *v);
sw     data_buf_inspect_size (s_pretty *pretty, const s_sym *type,
                              const void *v);
bool   data_clean (const s_sym *type, void *v);
s8     data_compare (const s_sym *type, const void *a, const void *b);
bool   data_hash_update (const s_sym *type, t_hash *hash,
                         const void *s);
void * data_init_cast (void *v, p_sym *type, s_tag *src);
void * data_init_copy (const s_sym *type, void *v, void *src);

#endif /* LIBKC3_DATA_H */
