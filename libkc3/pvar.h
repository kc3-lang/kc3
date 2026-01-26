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
#ifndef LIBKC3_PVAR_H
#define LIBKC3_PVAR_H

#include "types.h"

/* Stack-allocation compatible functions, call pvar_clean after use. */
void    pvar_clean (p_var *pvar);
p_var * pvar_init (p_var *pvar, const s_sym *type);
p_var * pvar_init_1 (p_var *pvar, const char *p);
p_var * pvar_init_cast (p_var *tag, const s_sym * const *type,
                        s_tag *src);
p_var * pvar_init_copy (p_var *var, p_var *src);

/* Operators. */
s_tag * pvar_assign (p_var *pvar, s_tag *value);
p_var * pvar_reset (p_var *pvar);
p_var * pvar_set (p_var *pvar, s_tag *value);

#endif /* LIBKC3_PVAR_H */
