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
 * @brief Run-time type annotations enum.
 *
 * Enum for run-time tagged data types.
 */
#ifndef LIBKC3_TAG_TYPE_H
#define LIBKC3_TAG_TYPE_H

#include "types.h"

#if HAVE_F80
    #define TAG_TYPE_F80 f80:    TAG_F80,
#else
    #define TAG_TYPE_F80
#endif


#if HAVE_F128
    #define TAG_TYPE_F128 f128:    TAG_F128,
#else
    #define TAG_TYPE_F128
#endif


/* Conflicts:
 *
 *  sw:             TAG_SW,
 *  uw:             TAG_UW,
 *  character:      TAG_CHARACTER,
 *  bool:           TAG_BOOL,
*/

#define TAG_TYPE(value) _Generic((value),                             \
    default:        TAG_VOID,                                         \
    s_array:        TAG_ARRAY,                                        \
    s_do_block:     TAG_DO_BLOCK,                                     \
    f32:            TAG_F32,                                          \
    f64:            TAG_F64,                                          \
    TAG_TYPE_F80                                                      \
    TAG_TYPE_F128                                                     \
    s_fact:         TAG_FACT,                                         \
    s_ident:        TAG_IDENT,                                        \
    s_integer:      TAG_INTEGER,                                      \
    s_ratio:        TAG_RATIO,                                        \
    s64:            TAG_S64,                                          \
    s32:            TAG_S32,                                          \
    s16:            TAG_S16,                                          \
    s8:             TAG_S8,                                           \
    u8:             TAG_U8,                                           \
    u16:            TAG_U16,                                          \
    u32:            TAG_U32,                                          \
    u64:            TAG_U64,                                          \
    p_list:         TAG_PLIST,                                        \
    s_map:          TAG_MAP,                                          \
    p_call:         TAG_PCALL,                                        \
    p_callable:     TAG_PCALLABLE,                                    \
    p_complex:      TAG_PCOMPLEX,                                     \
    p_cow:          TAG_PCOW,                                         \
    p_facts:        TAG_PFACTS,                                       \
    s_pointer:      TAG_POINTER,                                      \
    p_struct:       TAG_PSTRUCT,                                      \
    p_struct_type:  TAG_PSTRUCT_TYPE,                                 \
    p_sym:          TAG_PSYM,                                         \
    p_tag:          TAG_PTAG,                                         \
    u_ptr:          TAG_PTR,                                          \
    u_ptr_w:        TAG_PTR_FREE,                                     \
    p_var:          TAG_PVAR,                                         \
    s_quote:        TAG_QUOTE,                                        \
    s_str:          TAG_STR,                                          \
    s_time:         TAG_TIME,                                         \
    s_tuple:        TAG_TUPLE,                                        \
    s_unquote:      TAG_UNQUOTE)

const char *    tag_type_to_string (e_tag_type type);

#endif /* LIBKC3_TAG_TYPE */
