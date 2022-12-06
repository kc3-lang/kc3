/* c3
 * Copyright 2022 kmx.io <contact@kmx.io>
 *
 * Permission is hereby granted to use this software excepted
 * on Apple computers granted the above copyright notice and
 * this permission paragraph are included in all copies and
 * substantial portions of this software.
 *
 * THIS SOFTWARE IS PROVIDED "AS-IS" WITHOUT ANY GUARANTEE OF
 * PURPOSE AND PERFORMANCE. IN NO EVENT WHATSOEVER SHALL THE
 * AUTHOR BE CONSIDERED LIABLE FOR THE USE AND PERFORMANCE OF
 * THIS SOFTWARE.
 */
#ifndef CHARACTER_H
#define CHARACTER_H

#include "hash.h"
#include "types.h"

character        character_1 (const s8 *p);
s8               character_compare (character a, character b);
t_hash_context * character_hash_update (t_hash_context *context,
                                        character c);
e_bool           character_is_digit (character c);
e_bool           character_is_lowercase (character c);
e_bool           character_is_printable (character c);
e_bool           character_is_space (character c);
e_bool           character_is_uppercase (character c);
sw               character_read (s_buf *buf, character *c);
sw               character_write (s_buf *buf, character c);
sw               character_utf8 (character c, s8 *dest);
sw               character_utf8_size (character c);

#endif /* CHARACTER_H */
