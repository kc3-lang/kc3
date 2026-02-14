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
 * @file tuple.h
 * @brief Tag tuples.
 *
 * A tuple is a static array of tags.
 */
#ifndef LIBKC3_TUPLE_H
#define LIBKC3_TUPLE_H

#include "hash.h"
#include "types.h"

/* Stack allocation compatible functions */
s_tuple * tuple_init (s_tuple *tuple, uw count);
s_tuple * tuple_init_1 (s_tuple *tuple, const char *p);
s_tuple * tuple_init_2 (s_tuple *tuple, s_tag *a, s_tag *b);
s_tuple * tuple_init_cast (s_tuple *tuple, const s_sym * const *type,
                           s_tag *tag);
s_tuple * tuple_init_copy (s_tuple *tuple, s_tuple *src);
void      tuple_clean (s_tuple *tuple);

/* Constructors, call tuple_delete after use */
s_tuple * tuple_new (uw count);
s_tuple * tuple_new_1 (const char *p);
s_tuple * tuple_new_copy (s_tuple *src);
s_tuple * tuple_new_ref (s_tuple *tuple);

/* Destructor */
void tuple_delete (s_tuple *tuple);

/* Modifiers */
s_tuple * tuple_1 (s_tuple *tuple, const char *p);

/* Observers */
s_list * tuple_to_list (s_tuple *tuple, s_list **list);

#endif /* LIBKC3_TUPLE_H */
