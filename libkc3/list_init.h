/* kc3
 * Copyright 2022,2023,2024 kmx.io <contact@kmx.io>
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
#ifndef LIBKC3_LIST_INIT_H
#define LIBKC3_LIST_INIT_H

#include "types.h"

/* Stack-allocation compatible functions, call list_clean after use. */
s_list * list_init_array (s_list *list, const s_sym *type,
                          uw dimension, const uw *dimensions,
                          s_list *next);
s_list * list_init_array_copy (s_list *list, const s_array *a,
                               s_list *next);
s_list * list_init_bool (s_list *list, bool b, s_list *next);
s_list * list_init_call (s_list *list, s_list *next);
s_list * list_init_character (s_list *list, character c, s_list *next);

s_list * list_init_complex (s_list *list, s_complex *c, s_list *next);
s_list * list_init_f32 (s_list *list, f32 f, s_list *next);
s_list * list_init_f64 (s_list *list, f64 f, s_list *next);
s_list * list_init_f128 (s_list *list, f128 f, s_list *next);
s_list * list_init_ident (s_list *list, const s_ident *ident,
                          s_list *next);
s_list * list_init_ident_1 (s_list *list, const char *p, s_list *next);
s_list * list_init_integer_1 (s_list *list, const char *p,
                              s_list *next);
s_list * list_init_integer_copy (s_list *list, const s_integer *i,
                                 s_list *next);
s_list * list_init_integer_zero (s_list *list, s_list *next);

s_list * list_init_map (s_list *list, uw count, s_list *next);
s_list * list_init_map_1 (s_list *list, const char *p, s_list *next);
s_list * list_init_map_from_lists (s_list *list, s_list *keys,
                                   s_list *values, s_list *next);
s_list * list_init_ptr (s_list *list, void *p, s_list *next);
s_list * list_init_ptr_free (s_list *list, void *p, s_list *next);
s_list * list_init_quote_copy (s_list *list, s_quote *quote,
                               s_list *next);
s_list * list_init_ratio_1 (s_list *list, const char *p, s_list *next);
s_list * list_init_ratio (s_list *list, s_list *next);
s_list * list_init_ratio_copy (s_list *list, s_ratio *r, s_list *next);
s_list * list_init_ratio_zero (s_list *list, s_list *next);
s_list * list_init_s8 (s_list *list, s8 i, s_list *next);
s_list * list_init_s16 (s_list *list, s16 i, s_list *next);
s_list * list_init_s32 (s_list *list, s32 i, s_list *next);
s_list * list_init_s64 (s_list *list, s64 i, s_list *next);
s_list * list_init_str (s_list *list, char *p_free, uw size,
                        const char *p, s_list *next);
s_list * list_init_str_1 (s_list *list, char *p_free, const char *p,
                          s_list *next);
s_list * list_init_str_1_alloc (s_list *list, const char *p,
                                s_list *next);
s_list * list_init_str_alloc_copy (s_list *list, uw size,
                                   const char *p, s_list *next);
s_list * list_init_str_cast (s_list *list, const s_sym * const *type,
                             const s_tag *src, s_list *next);
s_list * list_init_str_concatenate (s_list *list, const s_str *a,
                                    const s_str *b, s_list *next);
s_list * list_init_str_concatenate_list (s_list *list,
                                         const s_list * const *src,
                                         s_list *next);
s_list * list_init_str_copy (s_list *list, const s_str *src,
                             s_list *next);
s_list * list_init_str_empty (s_list *list, s_list *next);
s_list * list_init_struct (s_list *list, const s_sym *module,
                           s_list *next);
s_list * list_init_struct_copy (s_list *list, s_struct *src,
                                s_list *next);
s_list * list_init_struct_with_data (s_list *list, const s_sym *module,
                                     void *data, bool free_data,
                                     s_list *next);
s_list * list_init_struct_type (s_list *list, const s_sym *module,
                                s_list *spec, s_list *next);
s_list * list_init_struct_type_update_clean (s_list *list,
                                             const s_struct_type *st,
                                             const s_cfn *clean,
                                             s_list *next);
s_list * list_init_sw (s_list *list, sw i, s_list *next);
s_list * list_init_sym (s_list *list, const s_sym *sym, s_list *next);
s_list * list_init_tuple (s_list *list, uw count, s_list *next);
s_list * list_init_tuple_2 (s_list *list, s_tag *a, s_tag *b,
                            s_list *next);
s_list * list_init_time (s_list *list, s_list *next);
s_list * list_init_time_add (s_list *list, const s_time *a,
                             const s_time *b, s_list *next);
s_list * list_init_time_now (s_list *list, s_list *next);
s_list * list_init_u8 (s_list *list, u8 i, s_list *next);
s_list * list_init_u16 (s_list *list, u16 i, s_list *next);
s_list * list_init_u32 (s_list *list, u32 i, s_list *next);
s_list * list_init_u64 (s_list *list, u64 i, s_list *next);
s_list * list_init_unquote_copy (s_list *list, s_unquote *unquote,
                                 s_list *next);
s_list * list_init_uw (s_list *list, uw i, s_list *next);
s_list * list_init_var (s_list *list, const s_sym *type, s_list *next);
s_list * list_init_void (s_list *list, s_list *next);

/* Heap-allocation functions, call list_delete after use. */
s_list * list_new_array (const s_sym *type, uw dimension,
                         const uw *dimensions, s_list *next);
s_list * list_new_array_copy (const s_array *a, s_list *next);
s_list * list_new_bool (bool b, s_list *next);
s_list * list_new_call (s_list *next);
s_list * list_new_character (character c, s_list *next);

s_list * list_new_complex (s_complex *c, s_list *next);
s_list * list_new_f32 (f32 f, s_list *next);
s_list * list_new_f64 (f64 f, s_list *next);
s_list * list_new_f128 (f128 f, s_list *next);
s_list * list_new_ident (const s_ident *ident, s_list *next);
s_list * list_new_ident_1 (const char *p, s_list *next);
s_list * list_new_integer_1 (const char *p, s_list *next);
s_list * list_new_integer_copy (const s_integer *i, s_list *next);
s_list * list_new_integer_zero (s_list *next);

s_list * list_new_map (uw count, s_list *next);
s_list * list_new_map_1 (const char *p, s_list *next);
s_list * list_new_map_from_lists (s_list *keys, s_list *values,
                                  s_list *next);
s_list * list_new_ptr (void *p, s_list *next);
s_list * list_new_ptr_free (void *p, s_list *next);
s_list * list_new_quote_copy (s_quote *quote, s_list *next);
s_list * list_new_ratio_1 (const char *p, s_list *next);
s_list * list_new_ratio (s_list *next);
s_list * list_new_ratio_copy (s_ratio *r, s_list *next);
s_list * list_new_ratio_zero (s_list *next);
s_list * list_new_s8 (s8 i, s_list *next);
s_list * list_new_s16 (s16 i, s_list *next);
s_list * list_new_s32 (s32 i, s_list *next);
s_list * list_new_s64 (s64 i, s_list *next);
s_list * list_new_str (char *p_free, uw size, const char *p,
                       s_list *next);
s_list * list_new_str_1 (char *p_free, const char *p, s_list *next);
s_list * list_new_str_1_alloc (const char *p, s_list *next);
s_list * list_new_str_alloc_copy (uw size, const char *p, s_list *next);
s_list * list_new_str_cast (const s_sym * const *type,
                            const s_tag *src, s_list *next);
s_list * list_new_str_concatenate (const s_str *a, const s_str *b,
                                   s_list *next);
s_list * list_new_str_concatenate_list (const s_list * const *src,
                                        s_list *next);
s_list * list_new_str_copy (const s_str *src, s_list *next);
s_list * list_new_str_empty (s_list *next);
s_list * list_new_struct (const s_sym *module, s_list *next);
s_list * list_new_struct_copy (s_struct *src, s_list *next);
s_list * list_new_struct_with_data (const s_sym *module, void *data,
                                    bool free_data, s_list *next);
s_list * list_new_struct_type (const s_sym *module, s_list *spec,
                               s_list *next);
s_list * list_new_struct_type_update_clean (const s_struct_type *st,
                                            const s_cfn *clean,
                                            s_list *next);
s_list * list_new_sw (sw i, s_list *next);
s_list * list_new_sym (const s_sym *sym, s_list *next);
s_list * list_new_tuple (uw count, s_list *next);
s_list * list_new_tuple_2 (s_tag *a, s_tag *b, s_list *next);
s_list * list_new_time (s_list *next);
s_list * list_new_time_add (const s_time *a, const s_time *b,
                            s_list *next);
s_list * list_new_time_now (s_list *next);
s_list * list_new_u8 (u8 i, s_list *next);
s_list * list_new_u16 (u16 i, s_list *next);
s_list * list_new_u32 (u32 i, s_list *next);
s_list * list_new_u64 (u64 i, s_list *next);
s_list * list_new_unquote_copy (s_unquote *unquote, s_list *next);
s_list * list_new_uw (uw i, s_list *next);
s_list * list_new_var (const s_sym *type, s_list *next);
s_list * list_new_void (s_list *next);

#endif /* LIBKC3_LIST_INIT_H */
