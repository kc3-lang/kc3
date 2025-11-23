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
 * @file sym.h
 * @brief Symbol data structure.
 *
 * Structure to manipulate symbols.
 * Symbols are strings that are deduplicated.
 * They are allocated once and only free'd at the end of the program.
 * Comparing symbols is fast : it's a pointer comparison.
 */
#ifndef LIBKC3_SYM_H
#define LIBKC3_SYM_H

#include "types.h"

#define SYM_MAX 1024

#define SYM_1(bytes) {STR(bytes)}

extern const s_sym g_sym___DIR__;
extern const s_sym g_sym___FILE__;
extern const s_sym g_sym__brackets;
extern const s_sym g_sym__equal;
extern const s_sym g_sym__paren;
extern const s_sym g_sym__plus;
extern const s_sym g_sym_Array;
extern const s_sym g_sym_Block;
extern const s_sym g_sym_Bool;
extern const s_sym g_sym_Buf;
extern const s_sym g_sym_BufRW;
extern const s_sym g_sym_Call;
extern const s_sym g_sym_Callable;
extern const s_sym g_sym_Cfn;
extern const s_sym g_sym_Character;
extern const s_sym g_sym_Char__star;
extern const s_sym g_sym_Complex;
extern const s_sym g_sym_Counter;
extern const s_sym g_sym_Counter__star;
extern const s_sym g_sym_Cow;
extern const s_sym g_sym_F32;
extern const s_sym g_sym_F64;
extern const s_sym g_sym_F80;
extern const s_sym g_sym_F128;
extern const s_sym g_sym_Fact;
extern const s_sym g_sym_Facts;
extern const s_sym g_sym_Facts_star;
extern const s_sym g_sym_FactW;
extern const s_sym g_sym_Fn;
extern const s_sym g_sym_Ident;
extern const s_sym g_sym_Integer;
extern const s_sym g_sym_KC3;
extern const s_sym g_sym_KC3_Op;
extern const s_sym g_sym_List;
extern const s_sym g_sym_Map;
extern const s_sym g_sym_Marshall;
extern const s_sym g_sym_MarshallRead;
extern const s_sym g_sym_Mutex;
extern const s_sym g_sym_Pointer;
extern const s_sym g_sym_Pointer__star;
extern const s_sym g_sym_Ptag;
extern const s_sym g_sym_Ptr;
extern const s_sym g_sym_PtrFree;
extern const s_sym g_sym_Quote;
extern const s_sym g_sym_RPC;
extern const s_sym g_sym_RPC_Response;
extern const s_sym g_sym_Ratio;
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
extern const s_sym g_sym_Time;
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
extern const s_sym g_sym_access;
extern const s_sym g_sym_arity;
extern const s_sym g_sym_cast;
extern const s_sym g_sym_clean;
extern const s_sym g_sym_defstruct;
extern const s_sym g_sym_directory;
extern const s_sym g_sym_do;
extern const s_sym g_sym_else;
extern const s_sym g_sym_end;
extern const s_sym g_sym_error;
extern const s_sym g_sym_file;
extern const s_sym g_sym_fn;
extern const s_sym g_sym_if;
extern const s_sym g_sym_if_then_else;
extern const s_sym g_sym_is_a;
extern const s_sym g_sym_is_loading;
extern const s_sym g_sym_load_time;
extern const s_sym g_sym_macro;
extern const s_sym g_sym_marshall;
extern const s_sym g_sym_marshall_read;
extern const s_sym g_sym_match;
extern const s_sym g_sym_module;
extern const s_sym g_sym_ncpu;
extern const s_sym g_sym_op;
extern const s_sym g_sym_op_associativity;
extern const s_sym g_sym_op_callable;
extern const s_sym g_sym_op_equal;
extern const s_sym g_sym_op_pin;
extern const s_sym g_sym_op_precedence;
extern const s_sym g_sym_op_sym;
extern const s_sym g_sym_persist;
extern const s_sym g_sym_r;
extern const s_sym g_sym_read;
extern const s_sym g_sym_request;
extern const s_sym g_sym_right;
extern const s_sym g_sym_rw;
extern const s_sym g_sym_rwx;
extern const s_sym g_sym_rx;
extern const s_sym g_sym_signal;
extern const s_sym g_sym_special_operator;
extern const s_sym g_sym_str;
extern const s_sym g_sym_struct_type;
extern const s_sym g_sym_sym;
extern const s_sym g_sym_symbol;
extern const s_sym g_sym_symbol_value;
extern const s_sym g_sym_then;
extern const s_sym g_sym_timeout;
extern const s_sym g_sym_tv_nsec;
extern const s_sym g_sym_tv_sec;
extern const s_sym g_sym_w;
extern const s_sym g_sym_write;
extern const s_sym g_sym_wx;
extern const s_sym g_sym_x;

p_sym   sym_1 (const char *p);
void    sym_init_g_sym (void);
bool    sym_register (p_sym sym, s_sym *free_sym);

/* Heap-allocation functions, call sym_delete_all at exit. */
void    sym_delete (s_sym *sym);
void    sym_delete_all (void);
p_sym   sym_new (const s_str *src);

/* Observers */
p_sym   sym_array_type (p_sym sym);
bool    sym_character_is_reserved (character c);
p_sym   sym_find (const s_str *src);
s_tag * sym_find_to_tag (const s_str *src, s_tag *dest);
bool    sym_has_ident_reserved_characters (p_sym sym);
bool    sym_has_reserved_characters (p_sym sym);
bool    sym_is_array_type (p_sym sym);
bool    sym_is_module (p_sym sym);
bool    sym_is_pointer_type (p_sym sym, p_sym target_type);
uw *    sym_list_size (uw *dest);
bool *  sym_must_clean (p_sym sym, bool *must_clean);
p_sym   sym_pointer_to_target_type (p_sym pointer_type);
bool    sym_search_modules (p_sym sym, p_sym *dest);
p_sym   sym_target_to_pointer_type (p_sym target_type);
bool    sym_to_ffi_type (p_sym sym, ffi_type *result_type,
                         ffi_type **dest);
bool    sym_to_tag_type (p_sym sym, e_tag_type *dest);
bool    sym_type_is_integer (p_sym type);
uw *    sym_type_size (p_sym type, uw *size);

#endif /* LIBKC3_SYM_H */
