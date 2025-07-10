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

#ifndef LIBKC3_MARSHALL_H
#define LIBKC3_MARSHALL_H

#include "types.h"

#define PROTO_MARSHALL(name, type)                                    \
  s_marshall * marshall_ ## name (s_marshall *marshall, type src)

/* Stack-allocation compatible functions, call marshall_clean
   after use. */
void         marshall_clean (s_marshall *marshall);
s_marshall * marshall_init (s_marshall *marshall);

/* Heap-allocation functions, call marshall_delete after use. */
void         marshall_delete (s_marshall *marshall);
s_marshall * marshall_new (void);

/* Operators. */
PROTO_MARSHALL(bool ,bool);
PROTO_MARSHALL(character, character);
PROTO_MARSHALL(list, const s_list *);
PROTO_MARSHALL(s8, s8);
PROTO_MARSHALL(s16, s16);
PROTO_MARSHALL(s32, s32);
PROTO_MARSHALL(s64, s64);
PROTO_MARSHALL(str, const s_str *);
PROTO_MARSHALL(sw,  sw);
PROTO_MARSHALL(tag, const s_tag *);
PROTO_MARSHALL(tuple, const s_tuple *);
PROTO_MARSHALL(u8,  u8);
PROTO_MARSHALL(u16, u16);
PROTO_MARSHALL(u32, u32);
PROTO_MARSHALL(u64, u64);
PROTO_MARSHALL(uw, uw);

/* Export. */
sw           marshall_to_buf (s_marshall *marshall,
                              s_buf *buf);
s_str *      marshall_to_str (s_marshall *marshall,
                              s_str *dest);

#endif /* LIBKC3_MARSHALL_H */
