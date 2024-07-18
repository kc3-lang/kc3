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
#include "assert.h"
#include "character.h"
#include "integer.h"
#include "str.h"
#include "tag_type.h"
#include "ucd.h"

character character_1 (const char *p)
{
  character c;
  s_str stra;
  assert(p);
  str_init_1(&stra, NULL, p);
  str_peek_character_utf8(&stra, &c);
  return c;
}

character * character_init_cast (character *c, const s_tag *tag)
{
  assert(c);
  assert(tag);
  switch (tag->type) {
  case TAG_CHARACTER: *c = tag->data.character;                return c;
  case TAG_F32:       *c = (character) tag->data.f32;          return c;
  case TAG_F64:       *c = (character) tag->data.f64;          return c;
  case TAG_INTEGER:   *c = integer_to_u32(&tag->data.integer); return c;
  case TAG_S8:        *c = (character) tag->data.s8;           return c;
  case TAG_S16:       *c = (character) tag->data.s16;          return c;
  case TAG_S32:       *c = (character) tag->data.s32;          return c;
  case TAG_S64:       *c = (character) tag->data.s64;          return c;
  case TAG_SW:        *c = (character) tag->data.sw;           return c;
  case TAG_U8:        *c = (character) tag->data.u8;           return c;
  case TAG_U16:       *c = (character) tag->data.u16;          return c;
  case TAG_U32:       *c = (character) tag->data.u32;          return c;
  case TAG_U64:       *c = (character) tag->data.u64;          return c;
  case TAG_UW:        *c = (character) tag->data.uw;           return c;
  default: break;
  }
  err_write_1("character_cast: cannot cast ");
  err_write_1(tag_type_to_string(tag->type));
  err_puts(" to Character");
  assert(! "character_cast: cannot cast to Character");
  return NULL;
}

character * character_init_copy (character *c, const character *src)
{
  assert(c);
  assert(src);
  *c = *src;
  return c;
}

bool character_is_digit (character c)
{
  return ('0' <= c && c <= '9');
}


bool character_is_lowercase (character c)
{
  return (c < UCD_MAX &&
          g_ucd[c].flags & UCD_LETTER_LOWERCASE);
}

bool character_is_printable (character c)
{
  const u64 ucd_printable = UCD_LETTER | UCD_MARK | UCD_NUMBER |
    UCD_PUNCTUATION | UCD_SYMBOL | UCD_SEPARATOR_SPACE;
  return (c < UCD_MAX &&
          g_ucd[c].flags & ucd_printable);
}

bool character_is_space (character c)
{
  return (c < UCD_MAX &&
          g_ucd[c].flags & (UCD_OTHER_CONTROL | UCD_SEPARATOR_SPACE));
}

bool character_is_uppercase (character c)
{
  return (c < UCD_MAX &&
          g_ucd[c].flags & UCD_LETTER_UPPERCASE);
}

character character_switch_case (character c)
{
  character s;
  if (c < UCD_MAX &&
      (s = g_ucd[c].to_lower | g_ucd[c].to_upper))
    return s;
  return c;
}

character character_to_lower (character c)
{
  if (c < UCD_MAX &&
      g_ucd[c].to_lower)
    return g_ucd[c].to_lower;
  return c;
}

character character_to_upper (character c)
{
  if (c < UCD_MAX &&
      g_ucd[c].to_upper)
    return g_ucd[c].to_upper;
  return c;
}

sw character_utf8 (character c, char *dest)
{
  const u8 _00000111 = 0x07;
  const u8 _00001111 = 0x0F;
  const u8 _00011111 = 0x1F;
  const u8 _00111111 = 0x3F;
  const u8 _10000000 = 0x80;
  const u8 _11000000 = 0xC0;
  const u8 _11100000 = 0xE0;
  const u8 _11110000 = 0xF0;
  if (c < 0x80) {
    dest[0] = (s8) c;
    return 1;
  }
  if (c < 0x800) {
    dest[0] = _11000000 | ((c >> 6) & _00011111);
    dest[1] = _10000000 | ( c       & _00111111);
    return 2;
  }
  if (c < 0x10000) {
    dest[0] = _11100000 | ((c >> 12) & _00001111);
    dest[1] = _10000000 | ((c >>  6) & _00111111);
    dest[2] = _10000000 | ( c        & _00111111);
    return 3;
  }
  if (c < 0x110000) {
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
  if (c < 0x80)
    return 1;
  if (c < 0x800)
    return 2;
  if (c < 0x10000)
    return 3;
  if (c < 0x110000)
    return 4;
  return -1;
}
