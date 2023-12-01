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
#ifndef LIBC3_LIST_INIT_H
#define LIBC3_LIST_INIT_H

#include "types.h"

/* Stack-allocation compatible functions, call list_clean after use. */
s_list * list_init_array (s_list *list, const s_sym *type, 
                          uw dimension, const uw *dimensions, 
                          s_list *next);
s_list * list_init_bool (s_list *list, bool b, s_list *next);
s_list * list_init_call (s_list *list, s_list *next);
s_list * list_init_character (s_list *list, character c, s_list *next);

s_list * list_init_f32 (s_list *list, f32 f, s_list *next);
s_list * list_init_f64 (s_list *list, f64 f, s_list *next);
s_list * list_init_ident (s_list *list, const s_ident *ident, 
                          s_list *next);
s_list * list_init_ident_1 (s_list *list, const s8 *p, s_list *next);
s_list * list_init_integer_1 (s_list *list, const s8 *p, s_list *next);
s_list * list_init_integer_copy (s_list *list, const s_integer *i, 
                                 s_list *next);
s_list * list_init_integer_zero (s_list *list, s_list *next);

s_list * list_init_map (s_list *list, uw count, s_list *next);
s_list * list_init_map_1 (s_list *list, const s8 *p, s_list *next);
s_list * list_init_s8 (s_list *list, s8 i, s_list *next);
s_list * list_init_s16 (s_list *list, s16 i, s_list *next);
s_list * list_init_s32 (s_list *list, s32 i, s_list *next);
s_list * list_init_s64 (s_list *list, s64 i, s_list *next);
s_list * list_init_str (s_list *list, s8 *p_free, uw size, const s8 *p, 
                        s_list *next);
s_list * list_init_str_1 (s_list *list, s8 *p_free, const s8 *p, 
                          s_list *next);
s_list * list_init_sw (s_list *list, sw i, s_list *next);
s_list * list_init_sym (s_list *list, const s_sym *sym, s_list *next);
s_list * list_init_sym_1 (s_list *list, const s8 *p, s_list *next);
s_list * list_init_tuple (s_list *list, uw count, s_list *next);
s_list * list_init_tuple_2 (s_list *list, const s_tag *a, 
                            const s_tag *b, s_list *next);
s_list * list_init_time (s_list *list, s_list *next);
s_list * list_init_u8 (s_list *list, u8 i, s_list *next);
s_list * list_init_u16 (s_list *list, u16 i, s_list *next);
s_list * list_init_u32 (s_list *list, u32 i, s_list *next);
s_list * list_init_u64 (s_list *list, u64 i, s_list *next);
s_list * list_init_uw (s_list *list, uw i, s_list *next);
s_list * list_init_var (s_list *list, s_list *next);
s_list * list_init_void (s_list *list, s_list *next);

/* Heap-allocation functions, call list_delete after use. */
s_list * list_new_array (const s_sym *type, uw dimension, 
                         const uw *dimensions, s_list *next);
s_list * list_new_bool (bool b, s_list *next);
s_list * list_new_call (s_list *next);
s_list * list_new_character (character c, s_list *next);

s_list * list_new_f32 (f32 f, s_list *next);
s_list * list_new_f64 (f64 f, s_list *next);
s_list * list_new_ident (const s_ident *ident, s_list *next);
s_list * list_new_ident_1 (const s8 *p, s_list *next);
s_list * list_new_integer_1 (const s8 *p, s_list *next);
s_list * list_new_integer_copy (const s_integer *i, s_list *next);
s_list * list_new_integer_zero (s_list *next);

s_list * list_new_map (uw count, s_list *next);
s_list * list_new_map_1 (const s8 *p, s_list *next);
s_list * list_new_s8 (s8 i, s_list *next);
s_list * list_new_s16 (s16 i, s_list *next);
s_list * list_new_s32 (s32 i, s_list *next);
s_list * list_new_s64 (s64 i, s_list *next);
s_list * list_new_str (s8 *p_free, uw size, const s8 *p, s_list *next);
s_list * list_new_str_1 (s8 *p_free, const s8 *p, s_list *next);
s_list * list_new_sw (sw i, s_list *next);
s_list * list_new_sym (const s_sym *sym, s_list *next);
s_list * list_new_sym_1 (const s8 *p, s_list *next);
s_list * list_new_tuple (uw count, s_list *next);
s_list * list_new_tuple_2 (const s_tag *a, const s_tag *b, 
                           s_list *next);
s_list * list_new_time (s_list *next);
s_list * list_new_u8 (u8 i, s_list *next);
s_list * list_new_u16 (u16 i, s_list *next);
s_list * list_new_u32 (u32 i, s_list *next);
s_list * list_new_u64 (u64 i, s_list *next);
s_list * list_new_uw (uw i, s_list *next);
s_list * list_new_var (s_list *next);
s_list * list_new_void (s_list *next);

/* Setters. */
s_list * list_array (s_list *list, const s_sym *type, uw dimension, 
                     const uw *dimensions);
s_list * list_bool (s_list *list, bool b);
s_list * list_call (s_list *list);
s_list * list_character (s_list *list, character c);

s_list * list_f32 (s_list *list, f32 f);
s_list * list_f64 (s_list *list, f64 f);
s_list * list_ident (s_list *list, const s_ident *ident);
s_list * list_ident_1 (s_list *list, const s8 *p);
s_list * list_integer_1 (s_list *list, const s8 *p);
s_list * list_integer_copy (s_list *list, const s_integer *i);
s_list * list_integer_zero (s_list *list);

s_list * list_map (s_list *list, uw count);
s_list * list_map_1 (s_list *list, const s8 *p);
s_list * list_s8 (s_list *list, s8 i);
s_list * list_s16 (s_list *list, s16 i);
s_list * list_s32 (s_list *list, s32 i);
s_list * list_s64 (s_list *list, s64 i);
s_list * list_str (s_list *list, s8 *p_free, uw size, const s8 *p);
s_list * list_str_1 (s_list *list, s8 *p_free, const s8 *p);
s_list * list_sw (s_list *list, sw i);
s_list * list_sym (s_list *list, const s_sym *sym);
s_list * list_sym_1 (s_list *list, const s8 *p);
s_list * list_tuple (s_list *list, uw count);
s_list * list_tuple_2 (s_list *list, const s_tag *a, const s_tag *b);
s_list * list_time (s_list *list);
s_list * list_u8 (s_list *list, u8 i);
s_list * list_u16 (s_list *list, u16 i);
s_list * list_u32 (s_list *list, u32 i);
s_list * list_u64 (s_list *list, u64 i);
s_list * list_uw (s_list *list, uw i);
s_list * list_var (s_list *list);
s_list * list_void (s_list *list);

#endif /* LIBC3_LIST_INIT_H */
