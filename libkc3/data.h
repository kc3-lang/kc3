/* kc3
 * Copyright 2022,2023,2024 kmx.io <contact@kmx.io>
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

sw     data_buf_inspect (const s_sym *type, s_buf *buf, const void *v);
sw     data_buf_inspect_size (s_pretty *pretty, const s_sym *type,
                              const void *v);
bool   data_clean (const s_sym *type, void *v);
bool   data_compare (const s_sym *type, const void *a, const void *b);
bool   data_hash_update (const s_sym *type, t_hash *hash,
                         const void *s);
void * data_init_cast (void *v, const s_sym * const *type,
                       const s_tag *src);
void * data_init_copy (const s_sym *type, void *v, const void *src);

#endif /* LIBKC3_DATA_H */
