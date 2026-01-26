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
#ifndef LIBKC3_PSYM_H
#define LIBKC3_PSYM_H

#include "types.h"

extern uw g_psym_anon_serial;

/* Allocation functions. */
p_sym * psym_init_1 (p_sym *sym, const char *p);
p_sym * psym_init_anon (p_sym *sym, const s_str *prefix);
p_sym * psym_init_cast (p_sym *sym, p_sym const *type,
                        const s_tag *tag);
p_sym * psym_init_copy (p_sym *sym, p_sym  const *src);
p_sym * psym_init_str (p_sym *sym, const s_str *src);

/* Observers. */
bool    psym_type_is_integer (p_sym const *type);
uw *    psym_type_size (p_sym const *type, uw *size);

#endif /* LIBKC3_PSYM_H */
