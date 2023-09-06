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
#ifndef TAG_H
#define TAG_H

#include <stdarg.h>
#include <stdio.h>
#include "types.h"

#define TAG_FIRST (&g_tag_first)
#define TAG_LAST (&g_tag_last)

extern s_tag g_tag_first;
extern s_tag g_tag_last;

/* Stack allocation compatible functions */
void    tag_clean (s_tag *tag);
s_tag * tag_init (s_tag *tag);
s_tag * tag_init_1 (s_tag *tag, const s8 *p);
s_tag * tag_init_array (s_tag *tag, const s_array *a);
s_tag * tag_init_bool (s_tag *tag, bool p);
s_tag * tag_init_call (s_tag *tag, const s_call *call);
s_tag * tag_init_character (s_tag *tag, character c);
s_tag * tag_init_ident (s_tag *tag, const s_ident *ident);
s_tag * tag_init_ident_1 (s_tag *tag, const s8 *p);
s_tag * tag_init_integer (s_tag *tag, const s_integer *i);
s_tag * tag_init_integer_1 (s_tag *tag, const s8 *p);
s_tag * tag_init_integer_zero (s_tag *tag);
s_tag * tag_init_list (s_tag *tag, s_list *list);
s_tag * tag_init_list_1 (s_tag *tag, const s8 *p);
s_tag * tag_init_str (s_tag *tag, s8 *free, uw size, const s8 *p);
s_tag * tag_init_str_1 (s_tag *tag, s8 *free, const s8 *p);
s_tag * tag_init_sym (s_tag *tag, const s_sym *p);
s_tag * tag_init_sym_1 (s_tag *tag, const s8 *p);
s_tag * tag_init_time (s_tag *tag);
s_tag * tag_init_tuple (s_tag *tag, uw count);
s_tag * tag_init_tuple_2 (s_tag *tag, s_tag *a, s_tag *b);
s_tag * tag_init_var (s_tag *tag);
s_tag * tag_init_void (s_tag *tag);

/* Numbers */
s_tag * tag_init_f32 (s_tag *tag, f32 f);
s_tag * tag_init_f64 (s_tag *tag, f64 f);
s_tag * tag_init_s8 (s_tag *tag, s8 i);
s_tag * tag_init_s16 (s_tag *tag, s16 i);
s_tag * tag_init_s32 (s_tag *tag, s32 i);
s_tag * tag_init_s64 (s_tag *tag, s64 i);
s_tag * tag_init_sw (s_tag *tag, sw i);
s_tag * tag_init_u8 (s_tag *tag, u8 i);
s_tag * tag_init_u16 (s_tag *tag, u16 i);
s_tag * tag_init_u32 (s_tag *tag, u32 i);
s_tag * tag_init_u64 (s_tag *tag, u64 i);
s_tag * tag_init_uw (s_tag *tag, uw i);

/* Constructors, call tag_delete after use */
s_tag * tag_new ();
s_tag * tag_new_1 (const s8 *p);
s_tag * tag_new_array (const s_array *a);
s_tag * tag_new_bool (bool p);
s_tag * tag_new_character (character c);
s_tag * tag_new_copy (const s_tag *src);
s_tag * tag_new_sym (const s_sym *sym);
s_tag * tag_new_f32 (f32 f);
s_tag * tag_new_f64 (f64 f);
s_tag * tag_new_s8 (s8 i);
s_tag * tag_new_s16 (s16 i);
s_tag * tag_new_s32 (s32 i);
s_tag * tag_new_s64 (s64 i);
s_tag * tag_new_str (s8 *free, uw size, const s8 *p);
s_tag * tag_new_tuple_2 (s_tag *a, s_tag *b);
s_tag * tag_new_u8 (u8 i);
s_tag * tag_new_u16 (u16 i);
s_tag * tag_new_u32 (u32 i);
s_tag * tag_new_u64 (u64 i);
s_tag * tag_new_var ();

/* Destructor */
void tag_delete (s_tag *tag);

/* Observers */
s_tag *            tag_equal (const s_tag *a, const s_tag *b,
                              s_tag *dest);
u64                tag_hash_u64 (const s_tag *tag);
uw                 tag_hash_uw (const s_tag *tag);
s_str *            tag_inspect (const s_tag *tag, s_str *dest);
bool               tag_ident_is_bound (const s_tag *tag);
bool               tag_is_bound_var (const s_tag *tag);
bool               tag_is_number (const s_tag *tag);
bool               tag_is_unbound_var (const s_tag *tag);
s8                 tag_number_compare (const s_tag *a, const s_tag *b);
s_tag *            tag_paren (const s_tag *tag, s_tag *dest);
sw                 tag_size (const s_tag *tag);
void *             tag_to_ffi_pointer (s_tag *tag, const s_sym *type);
ffi_type           tag_to_ffi_type(const s_tag *tag);
void *             tag_to_pointer (s_tag *tag, e_tag_type type);
sw                 tag_type_size (e_tag_type type);
f_buf_inspect      tag_type_to_buf_inspect (e_tag_type type);
f_buf_inspect_size tag_type_to_buf_inspect_size (e_tag_type type);
f_buf_parse        tag_type_to_buf_parse (e_tag_type type);
s8 *               tag_type_to_string (e_tag_type type);
const s_sym *      tag_type_to_sym (e_tag_type tag_type);

/* Modifiers */
s_tag *  tag_1 (s_tag *tag, const s8 *p);
s_tag *  tag_array (s_tag *tag, const s_array *a);
s_tag *  tag_bool (s_tag *tag, bool p);
s_tag *  tag_cast_integer_to_s16 (s_tag *tag);
s_tag *  tag_cast_integer_to_s32 (s_tag *tag);
s_tag *  tag_cast_integer_to_s64 (s_tag *tag);
s_tag *  tag_cast_integer_to_s8 (s_tag *tag);
s_tag *  tag_cast_integer_to_u16 (s_tag *tag);
s_tag *  tag_cast_integer_to_u32 (s_tag *tag);
s_tag *  tag_cast_integer_to_u64 (s_tag *tag);
s_tag *  tag_cast_integer_to_u8 (s_tag *tag);
s_tag *  tag_character (s_tag *tag, character c);
s_tag *  tag_copy (const s_tag *src, s_tag *dest);
s_tag *  tag_f32 (s_tag *tag, f32 f);
s_tag *  tag_f64 (s_tag *tag, f64 f);
s_tag *  tag_ident (s_tag *tag, const s_ident *ident);
s_tag *  tag_ident_1 (s_tag *tag, const s8 *p);
s_tag *  tag_integer (s_tag *tag, const s_integer *x);
s_tag *  tag_integer_1 (s_tag *tag, const s8 *p);
s_tag *  tag_integer_reduce (s_tag *tag);
s_tag *  tag_list (s_tag *tag, s_list *list);
s_tag *  tag_list_1 (s_tag *tag, const s8 *p);
s_tag *  tag_s16 (s_tag *tag, s16 i);
s_tag *  tag_s32 (s_tag *tag, s32 i);
s_tag *  tag_s64 (s_tag *tag, s64 i);
s_tag *  tag_s8 (s_tag *tag, s8 i);
s_tag *  tag_str (s_tag *tag, s8 *free, uw size, const s8 *p);
s_tag *  tag_str_1 (s_tag *tag, s8 *free, const s8 *p);
s_tag *  tag_sym (s_tag *tag, const s_sym *p);
s_tag *  tag_sym_1 (s_tag *tag, const s8 *p);
s_tag *  tag_u16 (s_tag *tag, u16 i);
s_tag *  tag_u32 (s_tag *tag, u32 i);
s_tag *  tag_u64 (s_tag *tag, u64 i);
s_tag *  tag_u8 (s_tag *tag, u8 i);
s_tag *  tag_var (s_tag *tag);
s_tag *  tag_void (s_tag *tag);

/* operators */
s_tag * tag_add (const s_tag *a, const s_tag *b, s_tag *dest);
s_tag * tag_sub (const s_tag *a, const s_tag *b, s_tag *dest);
s_tag * tag_mul (const s_tag *a, const s_tag *b, s_tag *dest);
s_tag * tag_div (const s_tag *a, const s_tag *b, s_tag *dest);

#endif /* STR_H */
