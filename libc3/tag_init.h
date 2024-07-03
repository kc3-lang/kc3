
#ifndef LIBC3_TAG_INIT_H
#define LIBC3_TAG_INIT_H

#include "types.h"

/* Stack-allocation compatible functions, call tag_clean after use. */
s_tag * tag_init_array (s_tag *tag, const s_sym *type, uw dimension,
                        const uw *dimensions);
s_tag * tag_init_array_copy (s_tag *tag, const s_array *a);
s_tag * tag_init_bool (s_tag *tag, bool b);
s_tag * tag_init_call (s_tag *tag);
s_tag * tag_init_character (s_tag *tag, character c);
s_tag * tag_init_copy (s_tag *tag, const s_tag *src);
s_tag * tag_init_complex (s_tag *tag, s_complex *c);
s_tag * tag_init_f32 (s_tag *tag, f32 f);
s_tag * tag_init_f64 (s_tag *tag, f64 f);
s_tag * tag_init_f128 (s_tag *tag, f128 f);
s_tag * tag_init_fn_copy (s_tag *tag, const s_fn *fn);
s_tag * tag_init_ident (s_tag *tag, const s_ident *ident);
s_tag * tag_init_ident_1 (s_tag *tag, const char *p);
s_tag * tag_init_integer_1 (s_tag *tag, const char *p);
s_tag * tag_init_integer_copy (s_tag *tag, const s_integer *i);
s_tag * tag_init_integer_zero (s_tag *tag);
s_tag * tag_init_list (s_tag *tag, s_list *list);
s_tag * tag_init_map (s_tag *tag, uw count);
s_tag * tag_init_map_1 (s_tag *tag, const char *p);
s_tag * tag_init_ptr (s_tag *tag, void *p);
s_tag * tag_init_ptr_free (s_tag *tag, void *p);
s_tag * tag_init_quote_copy (s_tag *tag, const s_quote *quote);
s_tag * tag_init_ratio_1 (s_tag *tag, const char *p);
s_tag * tag_init_ratio (s_tag *tag);
s_tag * tag_init_ratio_copy (s_tag *tag, const s_ratio *r);
s_tag * tag_init_ratio_zero (s_tag *tag);
s_tag * tag_init_s8 (s_tag *tag, s8 i);
s_tag * tag_init_s16 (s_tag *tag, s16 i);
s_tag * tag_init_s32 (s_tag *tag, s32 i);
s_tag * tag_init_s64 (s_tag *tag, s64 i);
s_tag * tag_init_str (s_tag *tag, char *p_free, uw size, const char *p);
s_tag * tag_init_str_1 (s_tag *tag, char *p_free, const char *p);
s_tag * tag_init_str_cat (s_tag *tag, const s_str *a, const s_str *b);
s_tag * tag_init_str_empty (s_tag *tag);
s_tag * tag_init_struct (s_tag *tag, const s_sym *module);
s_tag * tag_init_struct_with_data (s_tag *tag, const s_sym *module,
                                   void *data);
s_tag * tag_init_struct_type (s_tag *tag, const s_sym *module,
                              const s_list *spec);
s_tag * tag_init_sw (s_tag *tag, sw i);
s_tag * tag_init_sym (s_tag *tag, const s_sym *sym);
s_tag * tag_init_tuple (s_tag *tag, uw count);
s_tag * tag_init_tuple_2 (s_tag *tag, const s_tag *a, const s_tag *b);
s_tag * tag_init_time (s_tag *tag);
s_tag * tag_init_u8 (s_tag *tag, u8 i);
s_tag * tag_init_u16 (s_tag *tag, u16 i);
s_tag * tag_init_u32 (s_tag *tag, u32 i);
s_tag * tag_init_u64 (s_tag *tag, u64 i);
s_tag * tag_init_unquote_copy (s_tag *tag, const s_unquote *unquote);
s_tag * tag_init_uw (s_tag *tag, uw i);
s_tag * tag_init_var (s_tag *tag, const s_sym *type);
s_tag * tag_init_void (s_tag *tag);

/* Heap-allocation functions, call tag_delete after use. */
s_tag * tag_new_array (const s_sym *type, uw dimension,
                       const uw *dimensions);
s_tag * tag_new_array_copy (const s_array *a);
s_tag * tag_new_bool (bool b);
s_tag * tag_new_call (void);
s_tag * tag_new_character (character c);
s_tag * tag_new_copy (const s_tag *src);
s_tag * tag_new_complex (s_complex *c);
s_tag * tag_new_f32 (f32 f);
s_tag * tag_new_f64 (f64 f);
s_tag * tag_new_f128 (f128 f);
s_tag * tag_new_fn_copy (const s_fn *fn);
s_tag * tag_new_ident (const s_ident *ident);
s_tag * tag_new_ident_1 (const char *p);
s_tag * tag_new_integer_1 (const char *p);
s_tag * tag_new_integer_copy (const s_integer *i);
s_tag * tag_new_integer_zero (void);
s_tag * tag_new_list (s_list *list);
s_tag * tag_new_map (uw count);
s_tag * tag_new_map_1 (const char *p);
s_tag * tag_new_ptr (void *p);
s_tag * tag_new_ptr_free (void *p);
s_tag * tag_new_quote_copy (const s_quote *quote);
s_tag * tag_new_ratio_1 (const char *p);
s_tag * tag_new_ratio (void);
s_tag * tag_new_ratio_copy (const s_ratio *r);
s_tag * tag_new_ratio_zero (void);
s_tag * tag_new_s8 (s8 i);
s_tag * tag_new_s16 (s16 i);
s_tag * tag_new_s32 (s32 i);
s_tag * tag_new_s64 (s64 i);
s_tag * tag_new_str (char *p_free, uw size, const char *p);
s_tag * tag_new_str_1 (char *p_free, const char *p);
s_tag * tag_new_str_cat (const s_str *a, const s_str *b);
s_tag * tag_new_str_empty (void);
s_tag * tag_new_struct (const s_sym *module);
s_tag * tag_new_struct_with_data (const s_sym *module, void *data);
s_tag * tag_new_struct_type (const s_sym *module, const s_list *spec);
s_tag * tag_new_sw (sw i);
s_tag * tag_new_sym (const s_sym *sym);
s_tag * tag_new_tuple (uw count);
s_tag * tag_new_tuple_2 (const s_tag *a, const s_tag *b);
s_tag * tag_new_time (void);
s_tag * tag_new_u8 (u8 i);
s_tag * tag_new_u16 (u16 i);
s_tag * tag_new_u32 (u32 i);
s_tag * tag_new_u64 (u64 i);
s_tag * tag_new_unquote_copy (const s_unquote *unquote);
s_tag * tag_new_uw (uw i);
s_tag * tag_new_var (const s_sym *type);
s_tag * tag_new_void (void);

/* Setters. */
s_tag * tag_array (s_tag *tag, const s_sym *type, uw dimension,
                   const uw *dimensions);
s_tag * tag_array_copy (s_tag *tag, const s_array *a);
s_tag * tag_bool (s_tag *tag, bool b);
s_tag * tag_call (s_tag *tag);
s_tag * tag_character (s_tag *tag, character c);
s_tag * tag_copy (s_tag *tag, const s_tag *src);
s_tag * tag_complex (s_tag *tag, s_complex *c);
s_tag * tag_f32 (s_tag *tag, f32 f);
s_tag * tag_f64 (s_tag *tag, f64 f);
s_tag * tag_f128 (s_tag *tag, f128 f);
s_tag * tag_fn_copy (s_tag *tag, const s_fn *fn);
s_tag * tag_ident (s_tag *tag, const s_ident *ident);
s_tag * tag_ident_1 (s_tag *tag, const char *p);
s_tag * tag_integer_1 (s_tag *tag, const char *p);
s_tag * tag_integer_copy (s_tag *tag, const s_integer *i);
s_tag * tag_integer_zero (s_tag *tag);
s_tag * tag_list (s_tag *tag, s_list *list);
s_tag * tag_map (s_tag *tag, uw count);
s_tag * tag_map_1 (s_tag *tag, const char *p);
s_tag * tag_ptr (s_tag *tag, void *p);
s_tag * tag_ptr_free (s_tag *tag, void *p);
s_tag * tag_quote_copy (s_tag *tag, const s_quote *quote);
s_tag * tag_ratio_1 (s_tag *tag, const char *p);
s_tag * tag_ratio (s_tag *tag);
s_tag * tag_ratio_copy (s_tag *tag, const s_ratio *r);
s_tag * tag_ratio_zero (s_tag *tag);
s_tag * tag_s8 (s_tag *tag, s8 i);
s_tag * tag_s16 (s_tag *tag, s16 i);
s_tag * tag_s32 (s_tag *tag, s32 i);
s_tag * tag_s64 (s_tag *tag, s64 i);
s_tag * tag_str (s_tag *tag, char *p_free, uw size, const char *p);
s_tag * tag_str_1 (s_tag *tag, char *p_free, const char *p);
s_tag * tag_str_cat (s_tag *tag, const s_str *a, const s_str *b);
s_tag * tag_str_empty (s_tag *tag);
s_tag * tag_struct (s_tag *tag, const s_sym *module);
s_tag * tag_struct_with_data (s_tag *tag, const s_sym *module,
                              void *data);
s_tag * tag_struct_type (s_tag *tag, const s_sym *module,
                         const s_list *spec);
s_tag * tag_sw (s_tag *tag, sw i);
s_tag * tag_sym (s_tag *tag, const s_sym *sym);
s_tag * tag_tuple (s_tag *tag, uw count);
s_tag * tag_tuple_2 (s_tag *tag, const s_tag *a, const s_tag *b);
s_tag * tag_time (s_tag *tag);
s_tag * tag_u8 (s_tag *tag, u8 i);
s_tag * tag_u16 (s_tag *tag, u16 i);
s_tag * tag_u32 (s_tag *tag, u32 i);
s_tag * tag_u64 (s_tag *tag, u64 i);
s_tag * tag_unquote_copy (s_tag *tag, const s_unquote *unquote);
s_tag * tag_uw (s_tag *tag, uw i);
s_tag * tag_var (s_tag *tag, const s_sym *type);
s_tag * tag_void (s_tag *tag);

#endif /* LIBC3_TAG_INIT_H */
