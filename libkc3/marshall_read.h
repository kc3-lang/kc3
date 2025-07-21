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
  s_marshall_read *                                                    \
  marshall_read_ ## name                                               \
  (s_marshall_read *mr, bool heap, type *dest)

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
s_marshall_read * marshall_read_new_buf (s_buf *buf);
s_marshall_read * marshall_read_new_file (const char *path);
s_marshall_read * marshall_read_new_str (s_str *src);

/* Operators. */
PROTO_MARSHALL_READ(array,       s_array      );
PROTO_MARSHALL_READ(bool,        bool         );
PROTO_MARSHALL_READ(call,        s_call       );
PROTO_MARSHALL_READ(callable,    s_callable   );
PROTO_MARSHALL_READ(character,   character    );
PROTO_MARSHALL_READ(cow,         s_cow        );
PROTO_MARSHALL_READ(complex,     s_complex    );
PROTO_MARSHALL_READ(do_block,    s_do_block   );
PROTO_MARSHALL_READ(f128,        f128         );
PROTO_MARSHALL_READ(f32,         f32          );
PROTO_MARSHALL_READ(f64,         f64          );
PROTO_MARSHALL_READ(fact,        s_fact       );
PROTO_MARSHALL_READ(list,        s_list       );
PROTO_MARSHALL_READ(ident,       s_ident      );
PROTO_MARSHALL_READ(integer,     s_integer    );
PROTO_MARSHALL_READ(map,         s_map        );
PROTO_MARSHALL_READ(quote,       s_quote      );
PROTO_MARSHALL_READ(ratio,       s_ratio      );
PROTO_MARSHALL_READ(s8,          s8           );
PROTO_MARSHALL_READ(s16,         s16          );
PROTO_MARSHALL_READ(s32,         s32          );
PROTO_MARSHALL_READ(s64,         s64          );
PROTO_MARSHALL_READ(struct,      s_struct     );
PROTO_MARSHALL_READ(struct_type, s_struct_type);
PROTO_MARSHALL_READ(sym,         s_sym        );
PROTO_MARSHALL_READ(str,         s_str        );
PROTO_MARSHALL_READ(sw,          sw           );
PROTO_MARSHALL_READ(tag,         s_tag        );
PROTO_MARSHALL_READ(tuple,       s_tuple      );
PROTO_MARSHALL_READ(time,        s_time       );
PROTO_MARSHALL_READ(u8,          u8           );
PROTO_MARSHALL_READ(u16,         u16          );
PROTO_MARSHALL_READ(u32,         u32          );
PROTO_MARSHALL_READ(u64,         u64          );
PROTO_MARSHALL_READ(unquote,     s_unquote    );
PROTO_MARSHALL_READ(uw,          uw           );
PROTO_MARSHALL_READ(var,         s_var        );

PROTO_MARSHALL_READ(pcallable,    s_callable   );

#endif /* LIBKC3_MARSHALL_READ_H */
