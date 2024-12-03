/* kc3
 * Copyright 2022,2023,2024 kmx.io <contact@kmx.io>
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
#include "serialize.h"

void serialize_clean (s_serialize *serialize)
{
  assert(serialize);
  (void) serialize;
}

s_serialize * serialize_init (s_serialize *serialize)
{
  s_serialize tmp = {0};
  *serialize = tmp;
  return serialize;
}

s_str * serialize_to_buf (const s_serialize *serialize, s_buf *buf);

s_str * serialize_to_str (const s_serialize *serialize, s_str *dest);

s_serialize * serialize_bool (s_serialize *serialize, bool b);

s_serialize * serialize_character (s_serialize *serialize,
                                   character c);

s_serialize * serialize_list (s_serialize *serialize,
                              const s_list *list);

s_serialize * serialize_tag (s_serialize *serialize,
                             const s_tag *tag);

s_serialize * serialize_tuple (s_serialize *serialize,
                               const s_tuple *tuple);

s_serialize * serialize_s8 (s_serialize *serialize, s8 x);

s_serialize * serialize_s16 (s_serialize *serialize, s16 x);

s_serialize * serialize_s32 (s_serialize *serialize, s32 x);

s_serialize * serialize_s64 (s_serialize *serialize, s64 x);

s_serialize * serialize_str (s_serialize *serialize, const s_str *str);

s_serialize * serialize_sw (s_serialize *serialize, sw x);

s_serialize * serialize_u8 (s_serialize *serialize, u8 x)
{
  assert(serialize);
  if ((r = buf_write_u8(serialize->buf, x)) < 0)
    return NULL;
  return serialize;
}

s_serialize * serialize_u16 (s_serialize *serialize, u16 x);

s_serialize * serialize_u32 (s_serialize *serialize, u32 x);

s_serialize * serialize_u64 (s_serialize *serialize, u64 x);

s_serialize * serialize_uw (s_serialize *serialize, uw x);
