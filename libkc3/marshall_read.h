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

#ifndef LIBKC3_MARSHALL_READ_H
#define LIBKC3_MARSHALL_READ_H

#include "types.h"

#define PROTO_MARSHALL_READ(name, type)                                \
  sw marshall_read_ ## name (s_marshall_read *mr, type *dest)

/* Stack-allocation compatible functions, call marshall_read_clean
   after use. */
void              marshall_read_clean (s_marshall_read *mr);
s_marshall_read * marshall_read_init (s_marshall_read *mr);
s_marshall_read * marshall_read_init_buf (s_marshall_read *mr,
                                          s_buf *buf);
s_marshall_read * marshall_read_init_file (s_marshall_read *mr,
                                           const char *path);
s_marshall_read * marshall_read_init_str (s_marshall_read *mr,
                                          s_str *src);

/* Heap-allocation functions, call marshall_read_delete after use. */
void              marshall_read_delete (s_marshall_read *mr);
s_marshall_read * marshall_read_new (void);
s_marshall_read * marshall_read_init_buf (s_marshall_read *mr,
                                          s_buf *buf);
s_marshall_read * marshall_read_init_file (s_marshall_read *mr,
                                           const char *path);
s_marshall_read * marshall_read_init_str (s_marshall_read *mr,
                                          s_str *src);

/* Configuration. */

/* Operators. */
PROTO_MARSHALL_READ(s8, s8);
PROTO_MARSHALL_READ(s16, s16);
PROTO_MARSHALL_READ(s32, s32);
PROTO_MARSHALL_READ(s64, s64);
PROTO_MARSHALL_READ(sw, sw);
PROTO_MARSHALL_READ(u8, u8);
PROTO_MARSHALL_READ(u16, u16);
PROTO_MARSHALL_READ(u32, u32);
PROTO_MARSHALL_READ(u64, u64);
PROTO_MARSHALL_READ(uw, uw);

#endif /* LIBKC3_MARSHALL_READ_H */
