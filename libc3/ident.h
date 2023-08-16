/* c3
 * Copyright 2022,2023 kmx.io <contact@kmx.io>
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
#ifndef IDENT_H
#define IDENT_H

#include "sym.h"
#include "types.h"

/* Maximum number of bytes in UTF-8 of an identifier. */
#define IDENT_MAX SYM_MAX

/* Constructors, call ident_clean after use. */
s_ident * ident_init_1 (s_ident *ident, const s8 *p);

/* Modifiers */
s_ident * ident_resolve_module (s_ident *ident, const s_env *env);

/* Observers */

/* Returns true iff c is an ident reserved character. */
bool ident_character_is_reserved (character c);

s_ident * ident_copy (const s_ident *src, s_ident *dest);

/* Returns true iff c is an ident reserved character as first. */
bool ident_first_character_is_reserved (character c);

/* Returns true iff ident contains reserved characters. */
bool ident_has_reserved_characters (const s_ident *ident);

s_ident * ident_init (s_ident *ident, const s_sym *sym);

s_str * ident_inspect (const s_ident *ident, s_str *dest);

bool ident_to_tag_type (const s_ident *ident, e_tag_type *dest);

#endif
