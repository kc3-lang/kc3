/* kc3
 * Copyright from 2022 to 2025 kmx.io <contact@kmx.io>
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
p_list * plist_init_append (p_list *plist, p_list *src,
                            s_tag *tag);
p_list * plist_init_cast (p_list *plist, const s_sym * const *type,
                          s_tag *tag);
p_list * plist_init_copy (p_list *plist, p_list *src);

/* Operators. */
p_list * plist_cast (p_list *plist, const s_sym * const *type,
                     const s_tag *tag);
p_list * plist_filter (p_list *plist, p_callable *function,
                       p_list *dest);
p_list * plist_map (p_list *plist, p_callable *function, p_list *dest);
p_list * plist_remove_void (p_list *plist);
p_list * plist_sort_by (p_list *plist, p_callable *compare,
                        p_list *dest);


#endif /* LIBKC3_PLIST_H */
