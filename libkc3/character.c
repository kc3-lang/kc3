/* kc3
 * Copyright from 2022 to 2025 kmx.io <contact@kmx.io>
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
#include "sym.h"
#include "tag_type.h"
#include "ucd.h"

character character_1 (const char *p)
{
  character c;
  s_str stra;
  assert(p);
  str_init_1(&stra, NULL, p);
  if (str_peek_character_utf8(&stra, &c) <= 0) {
    err_puts("character_1: str_peek_character_utf8");
    assert(! "character_1: str_peek_character_utf8");
    return 0;
  }
  return c;
}

character * character_init_cast (character *c, p_sym const *type,
                                 const s_tag *tag)
{
  character t;
  assert(c);
  assert(type);
  assert(tag);
  switch (tag->type) {
  case TAG_CHARACTER: t = tag->data.character;                goto ok;
  case TAG_F32:       t = (character) tag->data.f32;          goto ok;
  case TAG_F64:       t = (character) tag->data.f64;          goto ok;
  case TAG_INTEGER:   t = integer_to_u32(&tag->data.integer); goto ok;
  case TAG_S8:        t = (character) tag->data.s8;           goto ok;
  case TAG_S16:       t = (character) tag->data.s16;          goto ok;
  case TAG_S32:       t = (character) tag->data.s32;          goto ok;
  case TAG_S64:       t = (character) tag->data.s64;          goto ok;
  case TAG_SW:        t = (character) tag->data.sw;           goto ok;
  case TAG_U8:        t = (character) tag->data.u8;           goto ok;
  case TAG_U16:       t = (character) tag->data.u16;          goto ok;
  case TAG_U32:       t = (character) tag->data.u32;          goto ok;
  case TAG_U64:       t = (character) tag->data.u64;          goto ok;
  case TAG_UW:        t = (character) tag->data.uw;           goto ok;
  default: break;
  }
  err_write_1("character_cast: cannot cast ");
  err_write_1(tag_type_to_string(tag->type));
  if (*type == &g_sym_Character)
    err_puts(" to Character");
  else {
    err_write_1(" to ");
    err_inspect_psym(type);
    err_puts(" aka Character");
  }
  assert(! "character_cast: cannot cast to Character");
  return NULL;
 ok:
  if (t > UCD_MAX) {
    err_puts("character_cast: invalid Unicode character");
    assert(! "character_cast: invalid Unicode character");
    return NULL;
  }
  *c = t;
  return c;
}

character * character_init_copy (character *c, const character *src)
{
  assert(c);
  assert(src);
  *c = *src;
  return c;
}

bool   character_is_alphabetic (character c)
{
  return (c < UCD_MAX &&
          g_ucd[c].flags & UCD_LETTER);
}

bool  character_is_alphanum (character c)
{
  return (c < UCD_MAX &&
          g_ucd[c].flags & (UCD_NUMBER | UCD_LETTER));  
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
