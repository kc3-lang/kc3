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
#ifndef LIBKC3_CHARACTER_H
#define LIBKC3_CHARACTER_H

#include "hash.h"
#include "types.h"

character   character_1 (const char *p);
void        character_hash_update (character c, t_hash *hash);
character * character_init_cast (character *c, p_sym const *type,
                                 const s_tag *tag);
character * character_init_copy (character *c, const character *src);
bool        character_is_alphabetic (character c);
bool        character_is_alphanum (character c);
bool        character_is_digit (character c);
bool        character_is_lowercase (character c);
bool        character_is_printable (character c);
bool        character_is_space (character c);
bool        character_is_uppercase (character c);
sw          character_read (s_buf *buf, character *c);
character   character_switch_case (character c);
character   character_to_lower (character c);
character   character_to_upper (character c);
sw          character_write (s_buf *buf, character c);
sw          character_utf8 (character c, char *dest);
sw          character_utf8_size (character c);

#endif /* LIBKC3_CHARACTER_H */
