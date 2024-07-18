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
/* Gen from set_cursor.h.in NAME=tag TYPE=s_tag */
#ifndef LIBC3_SET_CURSOR__tag_H
#define LIBC3_SET_CURSOR__tag_H

#include "types.h"

void
set_cursor_init__tag (s_set__tag *set, s_set_cursor__tag *c);

s_set_item__tag *
set_cursor_next__tag (s_set_cursor__tag *c);

#endif /* LIBC3_SET_CURSOR__tag_H */
