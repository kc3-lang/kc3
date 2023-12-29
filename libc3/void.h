/* c3
 * Copyright 2022,2023 kmx.io <contact@kmx.io>
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
#ifndef LIBC3_VOID_H
#define LIBC3_VOID_H

#include "types.h"

sw     void_buf_inspect (const s_sym *type, s_buf *buf, const void *v);
sw     void_buf_inspect_size (const s_sym *type, const void *v);
bool   void_clean (const s_sym *type, void *v);
bool   void_hash_update (const s_sym *type, t_hash *hash,
                         const void *s);
void * void_init_cast (const s_sym *type, void *v, const s_tag *src);
void * void_init_copy (const s_sym *type, void *v, const void *src);

#endif /* LIBC3_VOID_H */
