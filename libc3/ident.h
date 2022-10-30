/* c3
 * Copyright 2022 kmx.io <contact@kmx.io>
 *
 * Permission is hereby granted to use this software granted
 * the above copyright notice and this permission paragraph
 * are included in all copies and substantial portions of this
 * software.
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

/* Observers */

/* Returns true iff c is an ident reserved character. */
e_bool ident_character_is_reserved (character c);

/* Compare two idents. Returns -1 if a < b, 0 if a = b, 1 if a > b. */
s8 ident_compare (const s_ident *a, const s_ident *b);

/* Returns true iff c is an ident reserved character as first. */
e_bool ident_first_character_is_reserved (character c);

/* Returns true iff ident contains reserved characters. */
e_bool ident_has_reserved_characters (const s_ident *ident);

t_hash_context * ident_hash_update (t_hash_context *context,
                                    const s_ident *ident);

s_ident * ident_init (s_ident *ident, const s_sym *sym);

s_str * ident_inspect (const s_ident *ident, s_str *dest);

#endif
