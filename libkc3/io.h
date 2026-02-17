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
#ifndef LIBKC3_IO_H
#define LIBKC3_IO_H

#include "types.h"

#define PROTOTYPE_ERR_INSPECT(name, type)                              \
  sw err_inspect_ ## name (type x)                                     \

#define PROTOTYPE_IO_INSPECT(name, type)                               \
  sw io_inspect_ ## name (type x)                                      \

#define PROTOTYPES_ERR_IO_INSPECT(name, type)                          \
  PROTOTYPE_ERR_INSPECT(name, type);                                   \
  PROTOTYPE_IO_INSPECT(name, type)

/* error output */
sw err_flush (void);
sw err_inspect (const s_tag *x);
sw err_puts (const char *x);
sw err_stacktrace (void);
sw err_write (const void *x, uw len);
sw err_write_1 (const char *x);
sw err_write_str (const s_str *x);
sw err_write_u8 (u8 x);

/* standard output */
sw io_flush (void);
sw io_inspect (const s_tag *x);
sw io_puts (const char *x);
sw io_stacktrace (void);
sw io_write (const void *x, uw len);
sw io_write_1 (const char *x);
sw io_write_str (const s_str *x);
sw io_write_u8 (u8 x);

PROTOTYPES_ERR_IO_INSPECT(array,               const s_array *);
PROTOTYPES_ERR_IO_INSPECT(do_block,            const s_do_block *);
PROTOTYPES_ERR_IO_INSPECT(bool,                bool);
PROTOTYPES_ERR_IO_INSPECT(buf,                 const s_buf *);
PROTOTYPES_ERR_IO_INSPECT(c_pointer,           const void *);
PROTOTYPES_ERR_IO_INSPECT(call,                const s_call *);
PROTOTYPES_ERR_IO_INSPECT(callable,            const s_callable *);
PROTOTYPES_ERR_IO_INSPECT(cfn,                 const s_cfn *);
PROTOTYPES_ERR_IO_INSPECT(character,           character);
PROTOTYPES_ERR_IO_INSPECT(f32,                 f32);
PROTOTYPES_ERR_IO_INSPECT(f64,                 const f64 *);
PROTOTYPES_ERR_IO_INSPECT(fact,                const s_fact *);
PROTOTYPES_ERR_IO_INSPECT(facts_spec,          const p_facts_spec);
PROTOTYPES_ERR_IO_INSPECT(fn,                  const s_fn *);
PROTOTYPES_ERR_IO_INSPECT(fn_clause,           const s_fn_clause *);
PROTOTYPES_ERR_IO_INSPECT(fn_pattern,          const s_list *);
PROTOTYPES_ERR_IO_INSPECT(frame,               const s_frame *);
PROTOTYPES_ERR_IO_INSPECT(ident,               const s_ident *);
PROTOTYPES_ERR_IO_INSPECT(integer,             const s_integer *);
PROTOTYPES_ERR_IO_INSPECT(integer_decimal,     const s_integer *);
PROTOTYPES_ERR_IO_INSPECT(integer_hexadecimal, const s_integer *);
PROTOTYPES_ERR_IO_INSPECT(list,                const s_list *);
PROTOTYPES_ERR_IO_INSPECT(map,                 const s_map *);
PROTOTYPES_ERR_IO_INSPECT(plist,               p_list *);
PROTOTYPES_ERR_IO_INSPECT(pointer,             const s_pointer *);
PROTOTYPES_ERR_IO_INSPECT(psym,                p_sym const *);
PROTOTYPES_ERR_IO_INSPECT(ptr,                 const u_ptr_w *);
PROTOTYPES_ERR_IO_INSPECT(quote,               const s_quote *);
PROTOTYPES_ERR_IO_INSPECT(ratio,               const s_ratio *);
PROTOTYPES_ERR_IO_INSPECT(s8,                  s8);
PROTOTYPES_ERR_IO_INSPECT(s8_decimal,          s8);
PROTOTYPES_ERR_IO_INSPECT(s8_hexadecimal,      s8);
PROTOTYPES_ERR_IO_INSPECT(s16,                 s16);
PROTOTYPES_ERR_IO_INSPECT(s16_decimal,         s16);
PROTOTYPES_ERR_IO_INSPECT(s16_hexadecimal,     s16);
PROTOTYPES_ERR_IO_INSPECT(s32,                 s32);
PROTOTYPES_ERR_IO_INSPECT(s32_decimal,         s32);
PROTOTYPES_ERR_IO_INSPECT(s32_hexadecimal,     s32);
PROTOTYPES_ERR_IO_INSPECT(s64,                 s64);
PROTOTYPES_ERR_IO_INSPECT(s64_decimal,         s64);
PROTOTYPES_ERR_IO_INSPECT(s64_hexadecimal,     s64);
PROTOTYPES_ERR_IO_INSPECT(stacktrace,          p_list);
PROTOTYPES_ERR_IO_INSPECT(str,                 const s_str *);
PROTOTYPES_ERR_IO_INSPECT(str_hex,             const s_str *);
PROTOTYPES_ERR_IO_INSPECT(struct,              const s_struct *);
PROTOTYPES_ERR_IO_INSPECT(struct_type,         const s_struct_type *);
PROTOTYPES_ERR_IO_INSPECT(sw,                  sw);
PROTOTYPES_ERR_IO_INSPECT(sw_decimal,          sw);
PROTOTYPES_ERR_IO_INSPECT(sw_hexadecimal,      sw);
PROTOTYPES_ERR_IO_INSPECT(sym,                 const s_sym *);
PROTOTYPES_ERR_IO_INSPECT(tag,                 const s_tag *);
PROTOTYPES_ERR_IO_INSPECT(tag_type,            e_tag_type);
PROTOTYPES_ERR_IO_INSPECT(time,                const s_time *);
PROTOTYPES_ERR_IO_INSPECT(tuple,               const s_tuple *);
PROTOTYPES_ERR_IO_INSPECT(u8,                  u8);
PROTOTYPES_ERR_IO_INSPECT(u8_decimal,          u8);
PROTOTYPES_ERR_IO_INSPECT(u8_hexadecimal,      u8);
PROTOTYPES_ERR_IO_INSPECT(u16,                 u16);
PROTOTYPES_ERR_IO_INSPECT(u16_decimal,         u16);
PROTOTYPES_ERR_IO_INSPECT(u16_hexadecimal,     u16);
PROTOTYPES_ERR_IO_INSPECT(u32,                 u32);
PROTOTYPES_ERR_IO_INSPECT(u32_decimal,         u32);
PROTOTYPES_ERR_IO_INSPECT(u32_hexadecimal,     u32);
PROTOTYPES_ERR_IO_INSPECT(u64,                 u64);
PROTOTYPES_ERR_IO_INSPECT(u64_decimal,         u64);
PROTOTYPES_ERR_IO_INSPECT(u64_hexadecimal,     u64);
PROTOTYPES_ERR_IO_INSPECT(uw,                  uw);
PROTOTYPES_ERR_IO_INSPECT(uw_decimal,          uw);
PROTOTYPES_ERR_IO_INSPECT(uw_hexadecimal,      uw);
PROTOTYPES_ERR_IO_INSPECT(var,                 const s_var *);

#endif /* LIBKC3_IO_H */
