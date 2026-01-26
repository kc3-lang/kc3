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
#ifndef LIBKC3_PLIST_H
#define LIBKC3_PLIST_H

#include "types.h"

/* Stack-allocation compatible functions, call plist_clean after use. */
void     plist_clean (p_list *plist);
p_list * plist_init_1 (p_list *plist, const char *p);
p_list * plist_init_append (p_list *plist, p_list *a, p_list *b);
p_list * plist_init_cast (p_list *plist, const s_sym * const *type,
                          s_tag *tag);
p_list * plist_init_copy (p_list *plist, p_list *src);

/* Observers. */
bool *   plist_all (p_list *plist, p_callable *function, bool *dest);

/* Operators. */
p_list * plist_cast (p_list *plist, const s_sym * const *type,
                     const s_tag *tag);
bool *   plist_each (p_list *plist, p_callable *function, bool *dest);
p_list * plist_filter (p_list *plist, p_callable *function,
                       p_list *dest);
s_tag *  plist_find_if (p_list *plist, p_callable *function,
                        s_tag *dest);
bool *   plist_has (p_list *plist, const s_tag *tag, bool *dest);
s_str  * plist_join (p_list *plist, s_str *sep, s_str *dest);
p_list * plist_map (p_list *plist, p_callable *function, p_list *dest);
p_list * plist_remove_void (p_list *plist);
p_list * plist_reverse (p_list *plist, p_list *dest);
p_list * plist_slice (p_list *list, s_tag *start, s_tag *end,
                      p_list *dest);
p_list * plist_sort (p_list *list, p_list *dest);
p_list * plist_sort_by (p_list *plist, p_callable *compare,
                        p_list *dest);
p_list * plist_tail (p_list *list);
p_list * plist_unique (p_list *list, p_list *dest);

#endif /* LIBKC3_PLIST_H */
