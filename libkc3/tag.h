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
/**
 * @file tag.h
 * @brief Run-time type annotations.
 *
 * Structure to add a type tag to data.
 */
#ifndef LIBKC3_TAG_H
#define LIBKC3_TAG_H

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
#if HAVE_F80
s_tag * tag_init_f80 (s_tag *tag, f80 f);
#endif
#if HAVE_F128
s_tag * tag_init_f128 (s_tag *tag, f128 f);
#endif
s_tag * tag_init_facts_cast (s_tag *tag, p_sym *type, s_tag *src);
s_tag * tag_init_from_str (s_tag *tag, const s_str *str);
s_tag * tag_init_uw_reduce (s_tag *tag, uw src);

/* Heap-allocation functions, call tag_delete after use. */
void    tag_delete (s_tag *tag);
s_tag * tag_new (void);
s_tag * tag_new_1 (const char *p);

/* Observers */
s8             tag_arity (const s_tag *tag);
u64 *          tag_hash_u64 (const s_tag *tag, u64 *dest);
uw *           tag_hash_uw (const s_tag *tag, uw *dest);
bool           tag_ident_is_bound (const s_tag *tag);
bool           tag_is_alist (const s_tag *tag);
bool           tag_is_bound_var (const s_tag *tag);
bool           tag_is_cast (const s_tag *tag, const s_sym *type);
bool           tag_is_integer (s_tag *tag);
bool           tag_is_positive_integer (s_tag *tag);
bool           tag_is_struct (const s_tag *tag, const s_sym *module);
bool *         tag_is_unbound_var (const s_tag *tag, bool *dest);
bool           tag_is_zero(const s_tag *tag);
s8             tag_number_compare (const s_tag *a, const s_tag *b);
s_tag *        tag_resolve_cow (s_tag *tag);
uw *           tag_size (const s_tag *tag, uw *dest);
ffi_type       tag_to_ffi_type(const s_tag *tag);
const s_sym ** tag_type (const s_tag *tag, const s_sym **type);
const s_sym ** tag_type_var (const s_tag *tag, const s_sym **type);

/* Setters */
#if HAVE_F80
s_tag * tag_f80 (s_tag *tag, f80 f);
#endif
#if HAVE_F128
s_tag * tag_f128 (s_tag *tag, f128 f);
#endif

/* Operators. */
s_tag *     tag_1 (s_tag *tag, const char *p);
s_pointer * tag_address (s_tag *tag, s_pointer *dest);
s_tag *     tag_integer_cast_to_s16 (const s_tag *tag, s_tag *dest);
s_tag *     tag_integer_cast_to_s32 (const s_tag *tag, s_tag *dest);
s_tag *     tag_integer_cast_to_s64 (const s_tag *tag, s_tag *dest);
s_tag *     tag_integer_cast_to_s8 (const s_tag *tag, s_tag *dest);
s_tag *     tag_integer_cast_to_u16 (const s_tag *tag, s_tag *dest);
s_tag *     tag_integer_cast_to_u32 (const s_tag *tag, s_tag *dest);
s_tag *     tag_integer_cast_to_u64 (const s_tag *tag, s_tag *dest);
s_tag *     tag_integer_cast_to_u8 (const s_tag *tag, s_tag *dest);
s_tag *     tag_integer_reduce (s_tag *tag, s_tag *dest);
bool        tag_is_number (s_tag *tag);
s_tag *     tag_list_1 (s_tag *tag, const char *p);
s_tag *     tag_set_name_if_null (s_tag *tag, const s_ident *name);
bool        tag_to_ffi_pointer (s_tag *tag, const s_sym *type,
                                void **dest);
bool        tag_to_pointer (s_tag *tag, const s_sym *type, void **dest);

/* KC3 operators. */
s_tag * tag_add (s_tag *a, s_tag *b, s_tag *dest);
s_tag * tag_and (s_tag *a, s_tag *b, s_tag *dest);
s_tag * tag_assign (s_tag *tag, s_tag *value, s_tag *dest);
s_tag * tag_band (s_tag *a, s_tag *b, s_tag *dest);
s_tag * tag_bnot (s_tag *tag, s_tag *dest);
s_tag * tag_bor (s_tag *a, s_tag *b, s_tag *dest);
s_tag * tag_brackets (s_tag *tag, s_tag *address,
                      s_tag *dest);
s_tag * tag_bxor (s_tag *a, s_tag *b, s_tag *dest);
s_tag * tag_cow (s_tag *value, s_tag *dest);
s_tag * tag_div (s_tag *a, s_tag *b, s_tag *dest);
bool *  tag_lt (s_tag *a, s_tag *b, bool *dest);
bool *  tag_lte (s_tag *a, s_tag *b, bool *dest);
bool *  tag_gt (s_tag *a, s_tag *b, bool *dest);
bool *  tag_gte (s_tag *a, s_tag *b, bool *dest);
bool *  tag_eq (s_tag *a, s_tag *b, bool *dest);
s_tag * tag_equal (s_tag *a, s_tag *b, s_tag *dest);
s_tag * tag_mod (s_tag *a, s_tag *b, s_tag *dest);
s_tag * tag_mul (s_tag *a, s_tag *b, s_tag *dest);
s_tag * tag_neg (s_tag *tag, s_tag *dest);
bool *  tag_not (s_tag *tag, bool *dest);
bool *  tag_not_eq (s_tag *a, s_tag *b, bool *dest);
s_tag * tag_or (s_tag *a, s_tag *b, s_tag *dest);
s_tag * tag_paren (s_tag *tag, s_tag *dest);
s_tag * tag_semicolon (const s_tag *a, s_tag *b, s_tag *dest);
s_tag * tag_shift_left (s_tag *a, s_tag *b, s_tag *dest);
s_tag * tag_shift_right (s_tag *a, s_tag *b, s_tag *dest);
s_tag * tag_sqrt (s_tag *tag, s_tag *dest);
s_tag * tag_sub (s_tag *a, s_tag *b, s_tag *dest);

// ptag
void ptag_clean(p_tag *ptag);

#endif /* LIBKC3_TAG_H */
