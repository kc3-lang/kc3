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
#ifndef LIBKC3_VAR_H
#define LIBKC3_VAR_H

#include "types.h"

/* Stack-allocation compatible functions. */
s_var * var_init (s_var *var, s_tag *ptr, const s_sym *type);
s_var * var_init_1 (s_var *var, const char *p);
s_var * var_init_cast (s_var *tag, const s_sym * const *type,
                       const s_tag *src);
s_var * var_init_copy (s_var *tag, const s_var *src);

/* Observers. */
bool * var_is_unbound (s_var *var, bool *dest);

/* Operators. */
s_var * var_reset (s_var *var);
s_var * var_set (s_var *var, const s_tag *value);

#endif /* LIBKC3_VAR_H */
