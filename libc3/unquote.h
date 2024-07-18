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
#ifndef LIBC3_UNQUOTE_H
#define LIBC3_UNQUOTE_H

#include "types.h"

/* Stack-allocation compatible functions, call unquote_clean
   after use. */
void        unquote_clean (s_unquote *unquote);
s_unquote * unquote_init (s_unquote *unquote, const s_tag *tag);
s_unquote * unquote_init_1 (s_unquote *unquote, const s8 *p);
s_unquote * unquote_init_cast (s_unquote *unquote, const s_tag *src);
s_unquote * unquote_init_copy (s_unquote *unquote,
                               const s_unquote *src);

#endif /* LIBC3_UNQUOTE_H */
