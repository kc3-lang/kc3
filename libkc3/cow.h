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
 * @file cow.h
 * @brief Copy on write is a software technique that allows you to have
 * read-only access to some memory but still be able to modify it. The
 * read-only data is copied to a writable memory area to allow for
 * modification and can then be turned into read-only memory.
 *
 * Usage :
 *   - cow_init() 
 *   - cow_rw() returns a writable tag pointer
 *   - cow_freeze() freeze the writable memory area into a read-only one
 *   - cow_ro() returns the last read-only version that was frozen
 */
#ifndef LIBKC3_COW_H
#define LIBKC3_COW_H

#include "types.h"

/* Stack-allocation compatible functions. Call cow_clean after use. */
void    cow_clean (s_cow *cow);
s_cow * cow_init (s_cow *cow, const s_sym *type);
/* s_cow * cow_init_1 (s_cow *cow, const char *utf8); */
s_cow * cow_init_cast (s_cow *cow, p_sym *type, s_tag *tag);
s_cow * cow_init_copy (s_cow *cow, s_cow *src);
s_cow * cow_init_tag_copy (s_cow *cow, const s_sym *type,
                           s_tag *src);

/* Heap-allocation functions. Call cow_delete after use. */
void    cow_delete (s_cow *cow);
s_cow * cow_new (const s_sym *type);
/* s_cow * cow_new_1 (const char *utf8); */
s_cow * cow_new_cast (p_sym *type, s_tag *tag);
s_cow * cow_new_copy (s_cow *src);
s_cow * cow_new_ref (s_cow *src);
s_cow * cow_new_tag_copy (const s_sym *type, s_tag *src);

/* Observers. */
s_str *       cow_inspect (const s_cow *cow, s_str *dest);
s_tag *       cow_read_only (s_cow *cow);
s_tag *       cow_read_write (s_cow *cow);
const s_tag * cow_resolve (const s_cow *cow);

/* Operators. */
s_cow * cow_freeze (s_cow *cow);
s_cow * cow_freeze_copy (s_cow *cow, s_tag *src);
s_cow * cow_thaw (s_cow *cow);
s_cow * cow_thaw_copy (s_cow *cow, s_tag *src);

#endif /* LIBKC3_COW_H */
