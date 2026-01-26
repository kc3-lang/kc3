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
/* Gen from set_cursor.h.in NAME=uw TYPE=uw */
#ifndef LIBKC3_SET_CURSOR__uw_H
#define LIBKC3_SET_CURSOR__uw_H

#include "types.h"

void
set_cursor_init__uw (s_set__uw *set, s_set_cursor__uw *c);

s_set_item__uw *
set_cursor_next__uw (s_set_cursor__uw *c);

#endif /* LIBKC3_SET_CURSOR__uw_H */
