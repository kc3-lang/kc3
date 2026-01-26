/* kc3
 * Copyright from 2022 to 2026 kmx.io <contact@kmx.io>
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
 * @file inspect.h
 * @brief Inspect any Tag type into a Str
 *
 * Functions to inspect a Tag into a Str.
 */
#ifndef LIBKC3_INSPECT_H
#define LIBKC3_INSPECT_H

#include "types.h"

s_str * inspect_array (const s_array *array, s_str *dest);
s_str * inspect_do_block (const s_do_block *do_block, s_str *dest);
s_str * inspect_bool (bool b, s_str *dest);
s_str * inspect_buf (const s_buf *buf, s_str *dest);
s_str * inspect_call (const s_call *call, s_str *dest);
s_str * inspect_character (character c, s_str *dest);
s_str * inspect_fact (const s_fact *fact, s_str *dest);
s_str * inspect_ident (const s_ident *ident, s_str *dest);
s_str * inspect_list (const s_list *list, s_str *dest);
s_str * inspect_ratio (const s_ratio *src, s_str *dest);
s_str * inspect_str (const s_str *str, s_str *dest);
s_str * inspect_str_eval (const s_list *list, s_str *dest);
s_str * inspect_str_hex (const s_str *str, s_str *dest);
s_str * inspect_struct (const s_struct *s, s_str *dest);
s_str * inspect_sym (const s_sym *sym, s_str *dest);
s_str * inspect_tag (const s_tag *tag, s_str *dest);
s_str * inspect_tuple (const s_tuple *tuple, s_str *dest);
s_str * inspect_var (const s_var *var, s_str *dest);

#endif /* LIBKC3_INSPECT_H */
