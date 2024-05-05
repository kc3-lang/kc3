/* c3
 * Copyright 2022-2024 kmx.io <contact@kmx.io>
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
 * read-only data is copied to newly allocated memory when modified.
 *
 * You can cow_freeze the writable version into read-only memory which
 * cleans the previous read-only value and you must be sure to not
 * access it anymore.
 *
 * Call cow_thaw to restore the writable memory to a copy of the
 * read-only version.
 *
 * Call cow_rw to get a writable s_tag pointer.
 */
#ifndef LIBC3_COW_H
#define LIBC3_COW_H

#include "types.h"

/* Stack-allocation compatible functions. Call cow_clean after use. */
void    cow_clean (s_cow *cow);
s_cow * cow_init (s_cow *cow);
s_cow * cow_init_1 (s_cow *cow, const char *utf8);
s_cow * cow_init_cast (s_cow *cow, const s_sym * const *type,
                       const s_tag *tag);
s_cow * cow_init_copy (s_cow *cow, const s_cow *src);
s_cow * cow_init_tag_copy (s_cow *cow, const s_tag *src);

/* Heap-allocation functions. Call cow_delete after use. */
void    cow_delete (s_cow *cow);
s_cow * cow_new (void);
s_cow * cow_new_cast (const s_sym * const *type, const s_tag *tag);
s_cow * cow_new_copy (const s_cow *src);

/* Observers. */
s_str *       cow_inspect (const s_cow *cow, s_str *dest);
const s_tag * cow_ro (const s_cow *cow);

/* Operators. */
s_cow * cow_freeze (s_cow *cow);
s_cow * cow_freeze_copy (s_cow *cow, const s_tag *src);
s_tag * cow_rw (s_cow *cow);
s_cow * cow_thaw (s_cow *cow);

#endif /* LIBC3_COW_H */
