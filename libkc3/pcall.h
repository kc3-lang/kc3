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
#ifndef LIBKC3_PCALL_H
#define LIBKC3_PCALL_H

#include "types.h"

/* Initialization functions, call pcall_clean after use. */
void     pcall_clean (p_call *pcall);
p_call * pcall_init (p_call *pcall);
p_call * pcall_init_1 (p_call *pcall, const char *p);
p_call * pcall_init_call_cast (p_call *pcall, const s_sym *type);
p_call * pcall_init_cast (p_call *pcall, p_sym *type, s_tag *src);
p_call * pcall_init_copy (p_call *pcall, p_call *src);

/* Operators. */
s_ident *      pcall_ident (p_call *pcall, s_ident *dest);
const s_sym ** pcall_sym (p_call *pcall, const s_sym **dest);

#endif /* LIBKC3_PCALL_H */
