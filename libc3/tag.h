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
/**
 * @file tag.h
 * @brief Run-time type annotations.
 *
 * Structure to add a type tag to data.
 */
#ifndef LIBC3_TAG_H
#define LIBC3_TAG_H

#include <stdarg.h>
#include "tag_init.h"
#include "tag_type.h"

#define TAG_FIRST (&g_tag_first)
#define TAG_LAST (&g_tag_last)

extern s_tag g_tag_first;
extern s_tag g_tag_last;

/* Stack-allocation compatible functions, call tag_clean after use. */
void    tag_clean (s_tag *tag);
s_tag * tag_init (s_tag *tag);
s_tag * tag_init_1 (s_tag *tag, const char *p);
s_tag * tag_init_call_cast (s_tag *tag, const s_sym *type);

/* Heap-allocation functions, call tag_delete after use. */
void    tag_delete (s_tag *tag);
s_tag * tag_new (void);
s_tag * tag_new_1 (const char *p);

/* Observers */
u64            tag_hash_u64 (const s_tag *tag);
uw             tag_hash_uw (const s_tag *tag);
s_str *        tag_inspect (const s_tag *tag, s_str *dest);
bool           tag_ident_is_bound (const s_tag *tag);
bool           tag_is_bound_var (const s_tag *tag);
bool           tag_is_number (const s_tag *tag);
bool           tag_is_unbound_var (const s_tag *tag);
bool           tag_is_zero(const s_tag *tag);
s8             tag_number_compare (const s_tag *a, const s_tag *b);
uw *           tag_size (const s_tag *tag, uw *dest);
ffi_type       tag_to_ffi_type(const s_tag *tag);
f32            tag_to_f32(const s_tag *tag);
f64            tag_to_f64(const s_tag *tag);
s_integer      tag_to_integer(const s_tag *tag);
const s_sym ** tag_type (const s_tag *tag, const s_sym **type);

/* Operators. */
s_tag * tag_1 (s_tag *tag, const char *p);
s_tag * tag_integer_cast_to_s16 (const s_tag *tag, s_tag *dest);
s_tag * tag_integer_cast_to_s32 (const s_tag *tag, s_tag *dest);
s_tag * tag_integer_cast_to_s64 (const s_tag *tag, s_tag *dest);
s_tag * tag_integer_cast_to_s8 (const s_tag *tag, s_tag *dest);
s_tag * tag_integer_cast_to_u16 (const s_tag *tag, s_tag *dest);
s_tag * tag_integer_cast_to_u32 (const s_tag *tag, s_tag *dest);
s_tag * tag_integer_cast_to_u64 (const s_tag *tag, s_tag *dest);
s_tag * tag_integer_cast_to_u8 (const s_tag *tag, s_tag *dest);
s_tag * tag_integer_reduce (s_tag *tag);
s_tag * tag_list_1 (s_tag *tag, const char *p);
bool    tag_to_const_pointer (const s_tag *tag, const s_sym *type,
                              const void **dest);
bool    tag_to_ffi_pointer (s_tag *tag, const s_sym *type, void **dest);
bool    tag_to_pointer (s_tag *tag, const s_sym *type, void **dest);

/* C3 operators. */
s_tag * tag_add (const s_tag *a, const s_tag *b, s_tag *dest);
bool *  tag_and (const s_tag *a, const s_tag *b, bool *dest);
s_tag * tag_band (const s_tag *a, const s_tag *b, s_tag *dest);
s_tag * tag_bnot (const s_tag *tag, s_tag *dest);
s_tag * tag_bor (const s_tag *a, const s_tag *b, s_tag *dest);
s_tag * tag_brackets (const s_tag *tag, const s_tag *address,
                      s_tag *dest);
s_tag * tag_bxor (const s_tag *a, const s_tag *b, s_tag *dest);
s_tag * tag_div (const s_tag *a, const s_tag *b, s_tag *dest);
bool *  tag_lt (const s_tag *a, const s_tag *b, bool *dest);
bool *  tag_lte (const s_tag *a, const s_tag *b, bool *dest);
bool *  tag_gt (const s_tag *a, const s_tag *b, bool *dest);
bool *  tag_gte (const s_tag *a, const s_tag *b, bool *dest);
bool *  tag_eq (const s_tag *a, const s_tag *b, bool *dest);
s_tag * tag_equal (const s_tag *a, const s_tag *b, s_tag *dest);
s_tag * tag_mod (const s_tag *a, const s_tag *b, s_tag *dest);
s_tag * tag_mul (const s_tag *a, const s_tag *b, s_tag *dest);
s_tag * tag_neg (const s_tag *tag, s_tag *dest);
bool *  tag_not (const s_tag *tag, bool *dest);
bool *  tag_not_eq (const s_tag *a, const s_tag *b, bool *dest);
bool *  tag_or (const s_tag *a, const s_tag *b, bool *dest);
s_tag * tag_paren (const s_tag *tag, s_tag *dest);
s_tag * tag_shift_left (const s_tag *a, const s_tag *b, s_tag *dest);
s_tag * tag_shift_right (const s_tag *a, const s_tag *b, s_tag *dest);
s_tag * tag_sub (const s_tag *a, const s_tag *b, s_tag *dest);

#endif /* LIBC3_TAG_H */
