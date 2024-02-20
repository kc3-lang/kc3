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
 * @file sym.h
 * @brief Symbol data structure.
 *
 * Structure to manipulate symbols.
 * Symbols are strings that are deduplicated.
 * They are allocated once and only free'd at the end of the program.
 * Comparing symbols is fast : it's a pointer comparison.
 */
#ifndef LIBC3_SYM_H
#define LIBC3_SYM_H

#include "types.h"

#define SYM_MAX 1024

extern const s_sym g_sym__brackets;
extern const s_sym g_sym__paren;
extern const s_sym g_sym__plus;
extern const s_sym g_sym_Array;
extern const s_sym g_sym_Block;
extern const s_sym g_sym_Bool;
extern const s_sym g_sym_C3;
extern const s_sym g_sym_Call;
extern const s_sym g_sym_Cfn;
extern const s_sym g_sym_Character;
extern const s_sym g_sym_Char__star;
extern const s_sym g_sym_F32;
extern const s_sym g_sym_F64;
extern const s_sym g_sym_F128;
extern const s_sym g_sym_Fact;
extern const s_sym g_sym_Fn;
extern const s_sym g_sym_Ident;
extern const s_sym g_sym_Integer;
extern const s_sym g_sym_List;
extern const s_sym g_sym_Map;
extern const s_sym g_sym_Ptag;
extern const s_sym g_sym_Ptr;
extern const s_sym g_sym_PtrFree;
extern const s_sym g_sym_Quote;
extern const s_sym g_sym_Result;
extern const s_sym g_sym_S8;
extern const s_sym g_sym_S16;
extern const s_sym g_sym_S32;
extern const s_sym g_sym_S64;
extern const s_sym g_sym_Str;
extern const s_sym g_sym_Struct;
extern const s_sym g_sym_StructType;
extern const s_sym g_sym_Sw;
extern const s_sym g_sym_Sym;
extern const s_sym g_sym_Tag;
extern const s_sym g_sym_Tuple;
extern const s_sym g_sym_U8;
extern const s_sym g_sym_U16;
extern const s_sym g_sym_U32;
extern const s_sym g_sym_U64;
extern const s_sym g_sym_Unquote;
extern const s_sym g_sym_Uw;
extern const s_sym g_sym_Uw_brackets;
extern const s_sym g_sym_Var;
extern const s_sym g_sym_Void;
extern const s_sym g_sym_cast;
extern const s_sym g_sym_defstruct;
extern const s_sym g_sym_do;
extern const s_sym g_sym_end;
extern const s_sym g_sym_if_then_else;
extern const s_sym g_sym_is_a;
extern const s_sym g_sym_load_time;
extern const s_sym g_sym_operator_pin;
extern const s_sym g_sym_r;
extern const s_sym g_sym_rw;
extern const s_sym g_sym_rwx;
extern const s_sym g_sym_rx;
extern const s_sym g_sym_special_operator;
extern const s_sym g_sym_struct_type;
extern const s_sym g_sym_w;
extern const s_sym g_sym_wx;
extern const s_sym g_sym_x;

const s_sym  * sym_1 (const char *p);
const s_sym ** sym_init_1 (const s_sym **sym, const char *p);
const s_sym ** sym_init_cast (const s_sym **sym, const s_tag *tag);
const s_sym ** sym_init_copy (const s_sym **sym,
                              const s_sym * const *src);
void           sym_init_g_sym (void);
const s_sym ** sym_init_str (const s_sym **sym, const s_str *src);
bool           sym_intern (const s_sym *sym, s_sym *free_sym);

/* Heap-allocation functions, call sym_delete_all at exit. */
void          sym_delete_all (void);
const s_sym * sym_new (const s_str *src);

/* Observers */
const s_sym * sym_array_type (const s_sym *sym);
bool          sym_character_is_reserved (character c);
const s_sym * sym_find (const s_str *src);
bool          sym_has_reserved_characters (const s_sym *sym);
s_str *       sym_inspect (const s_sym *sym, s_str *dest);
bool          sym_is_array_type (const s_sym *sym);
bool          sym_is_module (const s_sym *sym);
bool          sym_must_clean (const s_sym *sym, bool *must_clean);
bool          sym_to_ffi_type (const s_sym *sym, ffi_type *result_type,
                               ffi_type **dest);
bool          sym_to_tag_type (const s_sym *sym, e_tag_type *dest);
bool          sym_type_size (const s_sym *type, uw *size);

#endif /* LIBC3_SYM_H */
