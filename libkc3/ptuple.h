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
#ifndef LIBKC3_PTUPLE_H
#define LIBKC3_PTUPLE_H

#include "types.h"

void      ptuple_clean (p_tuple *tuple);
p_tuple * ptuple_init (p_tuple *tuple, uw count);
p_tuple * ptuple_init_1 (p_tuple *tuple, const char *p);
p_tuple * ptuple_init_2 (p_tuple *tuple, s_tag *a, s_tag *b);
p_tuple * ptuple_init_cast (p_tuple *tuple, const s_sym * const *type,
                            s_tag *tag);
p_tuple * ptuple_init_copy (p_tuple *tuple, p_tuple *src);

#endif /* LIBKC3_PTUPLE_H */
