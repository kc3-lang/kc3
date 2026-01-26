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
 * @file counter.h
 * @brief Integer counter
 *
 * Functions that define, increment, or decrement a counter.
 */
#ifndef LIBKC3_COUNTER_H
#define LIBKC3_COUNTER_H

#include "types.h"

/* Stack-allocation compatible functions, call counter_clean
   after use. */
void        counter_clean (s_counter *c);
s_counter * counter_init (s_counter *c, s_ident *ident, s_tag *value);

/* Heap-allocation functions, call counter_delete after use. */
void        counter_delete (s_counter *c);
s_counter * counter_new (s_ident *ident, s_tag *value);

/* Operators. */
s_tag * counter_decrease (s_counter *c, s_tag *positive,
                          s_tag *dest);
s_tag * counter_get (s_counter *c, s_tag *dest);
s_tag * counter_increase (s_counter *c, s_tag *positive,
                          s_tag *dest);

/* Hash table of counters. */
/*   Stack-allocation compatible functions, call counter_ht_clean
     after use. */
void   counter_ht_clean (s_ht *ht);
s_ht * counter_ht_init (s_ht *ht);

/*   Heap-allocation functions, call counter_ht_delete after use. */
void   counter_ht_delete (s_ht *ht);
s_ht * counter_ht_new (void);
s_ht * counter_ht_new_ref (s_ht *ht);

/*   Operators. */
s_counter ** counter_find (const s_ident *ident, s_counter **dest);
s_counter ** counter_ht_find (s_ht *ht, const s_ident *ident,
                              s_counter **dest);

#endif /* LIBKC3_COUNTER_H */
