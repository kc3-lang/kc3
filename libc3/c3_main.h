/* c3
 * Copyright 2022-2024 kmx.io <contact@kmx.io>
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
#ifndef LIBC3_C3_MAIN_H
#define LIBC3_C3_MAIN_H

#include "types.h"

#define C3_EXT ".c3"

extern const s_str g_c3_base_binary;
extern const s_str g_c3_base_octal;
extern const s_str g_c3_base_decimal;
extern const s_str g_c3_base_hexadecimal;
extern const s_str g_c3_bases_hexadecimal[2];
extern const char *g_c3_license;
extern sw          g_c3_exit_code;

/* Stack-allocation compatible functions, call c3_clean after use. */
s_env * c3_init (s_env *env, int argc, char **argv);
void    c3_clean (s_env *env);

/* Observers. */
uw *           c3_facts_next_id (uw *dest);
s_str *        c3_getenv (const s_str *name, s_str *dest);
void           c3_license (void);
const s_sym ** c3_module (const s_sym **dest);

/* Operators. */
s_tag * c3_def (const s_call *call, s_tag *dest);
s_tag * c3_defmodule (const s_sym **name, const s_block *block, s_tag *dest);
s_tag * c3_defoperator (const s_sym **name, const s_sym **sym,
                        const s_tag *symbol_value,
                        u8 operator_precedence,
                        const s_sym **operator_associativity,
                        s_tag *dest);
s_tag * c3_defstruct (const s_list *spec, s_tag *dest);
void ** c3_dlopen (const s_str *path, void **dest);
void    c3_exit (sw code);
s_tag * c3_pin (const s_tag *a, s_tag *dest);

/* Special operators. */
s_tag * c3_if_then_else (const s_tag *cond, const s_tag *then,
                         const s_tag *else_, s_tag *dest);

/* debug */
void c3_break (void);

#endif /* LIBC3_C3_MAIN_H */
