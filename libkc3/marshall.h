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

#define MARSHALL_MAGIC ((u64) 0x485352414d33434b)

#define PROTO_MARSHALL(name, type)                                    \
  s_marshall * marshall_ ## name (s_marshall *m, bool heap, type src)

/* Stack-allocation compatible functions, call marshall_clean
   after use. */
void         marshall_clean (s_marshall *m);
s_marshall * marshall_init (s_marshall *m);

/* Heap-allocation functions, call marshall_delete after use. */
void         marshall_delete (s_marshall *m);
s_marshall * marshall_new (void);

/* Operators. */
PROTO_MARSHALL(array, const s_array *);
PROTO_MARSHALL(bool, bool);
PROTO_MARSHALL(call, const s_call *);
PROTO_MARSHALL(callable, const s_callable *);
PROTO_MARSHALL(character, character);
PROTO_MARSHALL(cow, const s_cow *);
PROTO_MARSHALL(complex, const s_complex *);
PROTO_MARSHALL(do_block, const s_do_block *);
PROTO_MARSHALL(f128, f128);
PROTO_MARSHALL(f32, f32);
PROTO_MARSHALL(f64, f64);
PROTO_MARSHALL(fact, const s_fact *);
s_marshall * marshall_heap_pointer (s_marshall *m, bool heap, void *p,
                                    bool *present);
PROTO_MARSHALL(list, const s_list *);
PROTO_MARSHALL(ident, const s_ident *);
PROTO_MARSHALL(integer, const s_integer *);
PROTO_MARSHALL(map, const s_map *);
PROTO_MARSHALL(quote, const s_quote *);
PROTO_MARSHALL(ratio, const s_ratio *);
PROTO_MARSHALL(s8, s8);
PROTO_MARSHALL(s16, s16);
PROTO_MARSHALL(s32, s32);
PROTO_MARSHALL(s64, s64);
PROTO_MARSHALL(struct, const s_struct *);
PROTO_MARSHALL(struct_type, const s_struct_type *);
PROTO_MARSHALL(sym, const s_sym *);
PROTO_MARSHALL(str, const s_str *);
PROTO_MARSHALL(sw,  sw);
PROTO_MARSHALL(tag, const s_tag *);
PROTO_MARSHALL(tuple, const s_tuple *);
PROTO_MARSHALL(time, const s_time *);
PROTO_MARSHALL(u8,  u8);
PROTO_MARSHALL(u16, u16);
PROTO_MARSHALL(u32, u32);
PROTO_MARSHALL(u64, u64);
PROTO_MARSHALL(unquote, const s_unquote *);
PROTO_MARSHALL(uw, uw);
PROTO_MARSHALL(var, const s_var *);

/* Export. */
sw      marshall_to_buf (s_marshall *m, s_buf *buf);
s_str * marshall_to_str (s_marshall *m, s_str *dest);
sw      marshall_to_file (s_marshall *m, const char *path);

PROTO_MARSHALL(pcallable,    p_callable   );
PROTO_MARSHALL(pcomplex,     p_complex    );
PROTO_MARSHALL(pcow,         p_cow        );
PROTO_MARSHALL(plist,        p_list       );
PROTO_MARSHALL(pstruct,      p_struct     );
PROTO_MARSHALL(pstruct_type, p_struct_type);
PROTO_MARSHALL(ptag,         p_tag        );
PROTO_MARSHALL(ptr,          u_ptr_w      );
PROTO_MARSHALL(ptr_free,     u_ptr_w      );
PROTO_MARSHALL(psym,         p_sym        );

#endif /* LIBKC3_MARSHALL_H */
