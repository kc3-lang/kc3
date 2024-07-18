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
#ifndef LIBC3_IO_H
#define LIBC3_IO_H

#include "types.h"

#define PROTOTYPE_ERR_INSPECT(name, type)                              \
  sw err_inspect_ ## name (type x)                                     \

#define PROTOTYPE_IO_INSPECT(name, type)                               \
  sw io_inspect_ ## name (type x)                                      \

#define PROTOTYPES_ERR_IO_INSPECT(name, type)                          \
  PROTOTYPE_ERR_INSPECT(name, type);                                   \
  PROTOTYPE_IO_INSPECT(name, type)

/* error output */
sw err_inspect (const s_tag *x);
sw err_puts (const char *x);
sw err_write_1 (const char *x);

/* standard output */
sw io_inspect (const s_tag *x);
sw io_puts (const char *x);
sw io_write_1 (const char *x);

PROTOTYPES_ERR_IO_INSPECT(array,      const s_array *);
PROTOTYPES_ERR_IO_INSPECT(bool,       const bool *);
PROTOTYPES_ERR_IO_INSPECT(call,       const s_call *);
PROTOTYPES_ERR_IO_INSPECT(cfn,        const s_cfn *);
PROTOTYPES_ERR_IO_INSPECT(character,  const character *);
PROTOTYPES_ERR_IO_INSPECT(f32,        const f32 *);
PROTOTYPES_ERR_IO_INSPECT(f64,        const f64 *);
PROTOTYPES_ERR_IO_INSPECT(fact,       const s_fact *);
PROTOTYPES_ERR_IO_INSPECT(facts_spec, const p_facts_spec);
PROTOTYPES_ERR_IO_INSPECT(fn,         const s_fn *);
PROTOTYPES_ERR_IO_INSPECT(fn_pattern, const s_list *);
PROTOTYPES_ERR_IO_INSPECT(ident,      const s_ident *);
PROTOTYPES_ERR_IO_INSPECT(integer,    const s_integer *);
PROTOTYPES_ERR_IO_INSPECT(list,       const s_list * const *);
PROTOTYPES_ERR_IO_INSPECT(map,        const s_map *);
PROTOTYPES_ERR_IO_INSPECT(ptr,        const u_ptr_w *);
PROTOTYPES_ERR_IO_INSPECT(s8,         const s8 *);
PROTOTYPES_ERR_IO_INSPECT(s16,        const s16 *);
PROTOTYPES_ERR_IO_INSPECT(s32,        const s32 *);
PROTOTYPES_ERR_IO_INSPECT(s64,        const s64 *);
PROTOTYPES_ERR_IO_INSPECT(str,        const s_str *);
PROTOTYPES_ERR_IO_INSPECT(sw,         const sw *);
PROTOTYPES_ERR_IO_INSPECT(sym,        const s_sym * const *);
PROTOTYPES_ERR_IO_INSPECT(tag,        const s_tag *);
PROTOTYPES_ERR_IO_INSPECT(tuple,      const s_tuple *);
PROTOTYPES_ERR_IO_INSPECT(u8,         const u8 *);
PROTOTYPES_ERR_IO_INSPECT(u16,        const u16 *);
PROTOTYPES_ERR_IO_INSPECT(u32,        const u32 *);
PROTOTYPES_ERR_IO_INSPECT(u64,        const u64 *);
PROTOTYPES_ERR_IO_INSPECT(uw,         const uw *);

#endif /* LIBC3_IO_H */
