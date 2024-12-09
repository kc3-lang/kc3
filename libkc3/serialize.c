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
#include "alloc.h"
#include "buf.h"
#include "serialize.h"

#define DEF_SERIALIZE(type)                                            \
  s_serialize * serialize_ ## type (s_serialize *serialize,            \
                                    type src)                          \
  {                                                                    \
    sw r;                                                              \
    assert(serialize);                                                 \
    if ((r = buf_write_ ## type(&serialize->buf, src)) <= 0)           \
      return NULL;                                                     \
    return serialize;                                                  \
  }

DEF_SERIALIZE(bool)

void serialize_clean (s_serialize *serialize)
{
  assert(serialize);
  (void) serialize;
}

void serialize_delete (s_serialize *serialize)
{
  serialize_clean(serialize);
  free(serialize);
}

s_serialize * serialize_init (s_serialize *serialize)
{
  s_serialize tmp = {0};
  *serialize = tmp;
  return serialize;
}

s_serialize * serialize_new (void)
{
  s_serialize *serialize;
  if (! (serialize = alloc(sizeof(s_serialize))))
    return NULL;
  serialize_init(serialize);
  return serialize;
}

s_str * serialize_to_buf (const s_serialize *serialize, s_buf *buf);

s_str * serialize_to_str (const s_serialize *serialize, s_str *dest);

s_serialize * serialize_bool (s_serialize *serialize, bool b);

s_serialize * serialize_character (s_serialize *serialize,
                                   character c);

s_serialize * serialize_list (s_serialize *serialize,
                              const s_list *list);

s_serialize * serialize_tag (s_serialize *serialize, const s_tag *tag)
{
  switch (tag->type){
  case TAG_U8: return serialize_u8(serialize, tag->data.u8);
  default:     break;
  }
  err_puts("serialize_tag: not implemented");
  assert(! "serialize_tag: not implemented");
  return NULL;
}

s_serialize * serialize_tuple (s_serialize *serialize,
                               const s_tuple *tuple);

DEF_SERIALIZE(s8)
DEF_SERIALIZE(s16)
DEF_SERIALIZE(s32)
DEF_SERIALIZE(s64)

s_serialize * serialize_str (s_serialize *serialize, const s_str *str);

DEF_SERIALIZE(sw)
DEF_SERIALIZE(u8)
DEF_SERIALIZE(u16)
DEF_SERIALIZE(u32)
DEF_SERIALIZE(u64)
DEF_SERIALIZE(uw)
