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
#ifndef LIBKC3_FACT_LIST_H
#define LIBKC3_FACT_LIST_H

#include "types.h"

/* Stack-allocation compatible functions. */
s_fact_list * fact_list_init (s_fact_list *fl, s_fact *fact,
                              s_fact_list *next);

/* Heap allocation functions, call fact_list_delete* after use. */
s_fact_list * fact_list_delete (s_fact_list *fl);
void          fact_list_delete_all (s_fact_list *fl);
s_fact_list * fact_list_new (s_fact *fact, s_fact_list *next);

#endif /* LIBKC3_FACT_LIST_H */
