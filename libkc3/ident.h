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
#ifndef LIBKC3_IDENT_H
#define LIBKC3_IDENT_H

#include "sym.h"
#include "types.h"

/* Maximum number of bytes in UTF-8 of an identifier. */
#define IDENT_MAX SYM_MAX

/* Stack-allocation compatible functions */
s_ident * ident_init (s_ident *ident, const s_sym *module,
                      const s_sym *sym);
s_ident * ident_init_1 (s_ident *ident, const char *p);
s_ident * ident_init_cast (s_ident *ident, const s_sym * const *type,
                           const s_tag *tag);
s_ident * ident_init_copy (s_ident *ident, const s_ident *src);

/* Modifiers */
s_ident * ident_resolve_module (const s_ident *ident, s_ident *dest);

/* Observers */
bool    ident_character_is_reserved (character c);
bool    ident_first_character_is_reserved (character c);
s_tag * ident_get (const s_ident *ident, s_tag *dest);
uw *    ident_hash_uw (const s_ident *ident, uw *dest);
bool *  ident_is_special_operator (const s_ident *ident, bool *dest);
bool    ident_to_tag_type (const s_ident *ident, e_tag_type *dest);

#endif /* LIBKC3_IDENT_H */
