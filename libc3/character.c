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
#include <assert.h>
#include "character.h"
#include "str.h"
#include "ucd.h"

character character_1 (const s8 *p)
{
  character c;
  s_str stra;
  assert(p);
  str_init_1(&stra, NULL, p);
  str_peek_character(&stra, &c);
  return c;
}

s8 character_compare (character a, character b)
{
  if (a < b)
    return -1;
  if (a == b)
    return 0;
  return 1;
}

t_hash_context * character_hash_update (t_hash_context *context,
                                        character x)
{
  return hash_update(context, &x, sizeof(x));
}

e_bool character_is_digit (character c)
{
  return ('0' <= c && c <= '9');
}


e_bool character_is_lowercase (character c)
{
  return (c >= 0 &&
          c < UCD_MAX &&
          g_ucd[c].flags & UCD_LETTER_LOWERCASE);
}

e_bool character_is_printable (character c)
{
  const u64 ucd_printable = UCD_LETTER | UCD_MARK | UCD_NUMBER |
    UCD_PUNCTUATION | UCD_SYMBOL | UCD_SEPARATOR_SPACE;
  return (c >= 0 &&
          c < UCD_MAX &&
          g_ucd[c].flags & ucd_printable);
}

e_bool character_is_space (character c)
{
  return (c >= 0 &&
          c < UCD_MAX &&
          g_ucd[c].flags & (UCD_OTHER_CONTROL | UCD_SEPARATOR_SPACE));
}

e_bool character_is_uppercase (character c)
{
  return (c >= 0 &&
          c < UCD_MAX &&
          g_ucd[c].flags & UCD_LETTER_UPPERCASE);
}

sw character_utf8 (character c, s8 *dest)
{
  const u8 _00000111 = 0x07;
  const u8 _00001111 = 0x0F;
  const u8 _00011111 = 0x1F;
  const u8 _00111111 = 0x3F;
  const u8 _10000000 = 0x80;
  const u8 _11000000 = 0xC0;
  const u8 _11100000 = 0xE0;
  const u8 _11110000 = 0xF0;
  if (c == -1)
    return -1;
  if (((u64) c) < 0x80) {
    dest[0] = (s8) c;
    return 1;
  }
  if (((u64) c) < 0x800) {
    dest[0] = _11000000 | ((c >> 6) & _00011111);
    dest[1] = _10000000 | ( c       & _00111111);
    return 2;
  }
  if (((u64) c) < 0x10000) {
    dest[0] = _11100000 | ((c >> 12) & _00001111);
    dest[1] = _10000000 | ((c >>  6) & _00111111);
    dest[2] = _10000000 | ( c        & _00111111);
    return 3;
  }
  if (((u64) c) < 0x110000) {
    dest[0] = _11110000 | ((c >> 18) & _00000111);
    dest[1] = _10000000 | ((c >> 12) & _00111111);
    dest[2] = _10000000 | ((c >>  6) & _00111111);
    dest[3] = _10000000 | ( c        & _00111111);
    return 4;
  }
  return -1;
}

sw character_utf8_size (character c)
{
  if (c == -1)
    return -1;
  if (((u64) c) < 0x80)
    return 1;
  if (((u64) c) < 0x800)
    return 2;
  if (((u64) c) < 0x10000)
    return 3;
  if (((u64) c) < 0x110000)
    return 4;
  return -1;
}
