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
 * @file tuple.h
 * @brief Tag tuples.
 *
 * A tuple is a static array of tags.
 */
#ifndef LIBC3_TUPLE_H
#define LIBC3_TUPLE_H

#include "hash.h"
#include "types.h"

/* Stack allocation compatible functions */
s_tuple * tuple_init (s_tuple *tuple, uw count);
s_tuple * tuple_init_1 (s_tuple *tuple, const s8 *p);
void      tuple_clean (s_tuple *tuple);

/* Constructors, call tuple_delete after use */
s_tuple * tuple_new (uw count);
s_tuple * tuple_new_1 (const s8 *p);

/* Destructor */
void tuple_delete (s_tuple *tuple);

/* Modifiers */
s_tuple * tuple_1 (s_tuple *tuple, const s8 *p);

/* Observers */
s_tuple *        tuple_copy (const s_tuple *src, s_tuple *dest);
s_list *         tuple_to_list (const s_tuple *tuple, s_list **list);

/* Call str_delete after use. */
s_str *          tuple_inspect (const s_tuple *x, s_str *dest);

#endif /* LIBC3_TUPLE_H */
