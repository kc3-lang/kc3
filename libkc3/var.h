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
#ifndef LIBKC3_VAR_H
#define LIBKC3_VAR_H

#include "types.h"

/* Stack-allocation compatible functions, call var_clean after use. */
void    var_clean (s_var *var);
s_var * var_init (s_var *var, const s_sym *type);
s_var * var_init_copy (s_var *var, s_var *src);

/* Heap-allocation functions, call var_delete after use. */
void    var_delete (s_var *var);
s_var * var_new (const s_sym *type);
s_var * var_new_copy (s_var *src);
s_var * var_new_ref (s_var *src);

/* Observers. */
bool *  var_is_unbound (const s_var *var, bool *dest);

/* Operators. */
s_tag * var_assign (s_var *var, s_tag *value, s_tag *dest);
s_var * var_reset (s_var *var);
s_var * var_set (s_var *var, s_tag *value);
s_var * var_set_name (s_var *var, const s_ident *name);
s_var * var_set_name_if_null (s_var *var, const s_ident *name);

#endif /* LIBKC3_VAR_H */
