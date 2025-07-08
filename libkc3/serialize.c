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
#include "alloc.h"
#include "buf.h"
#include "list.h"
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
  buf_clean(&serialize->buf);
  buf_clean(&serialize->heap);
}

void serialize_delete (s_serialize *serialize)
{
  serialize_clean(serialize);
  free(serialize);
}

s_serialize * serialize_character (s_serialize *serialize,
                                   character c)
{
  return serialize_u32(serialize, c);
}

s_serialize * serialize_init (s_serialize *serialize)
{
  s_serialize tmp = {0};
  if (buf_init_alloc(&tmp.heap, 1024024) == NULL)
    return NULL;
  if (buf_init_alloc(&tmp.buf, 1024024) == NULL) {
    buf_delete(&tmp.buf);
    return NULL;
  }
  *serialize = tmp;
  return serialize;
}

/* Works the same as serialize_tuple. */
s_serialize * serialize_list (s_serialize *serialize,
                              const s_list *list)
{
  const s_list *l;
  u32 len;
  assert(serialize);
  len = list_length(list);
  if (! serialize_u32(serialize, len))
    return NULL;
  l = list;
  while (l) {
    if (! serialize_tag(serialize, &l->tag))
      return NULL;
    l = list_next(l);
  }
  return serialize;
}

s_serialize * serialize_new (void)
{
  s_serialize *serialize;
  if (! (serialize = alloc(sizeof(s_serialize))))
    return NULL;
  if (serialize_init(serialize) == NULL) {
    free(serialize);
    return NULL;
  }
  return serialize;
}

DEF_SERIALIZE(s8)
DEF_SERIALIZE(s16)
DEF_SERIALIZE(s32)
DEF_SERIALIZE(s64)

s_serialize * serialize_str (s_serialize *serialize, const s_str *src)
{
  sw r;
  assert(serialize);
  assert(src);
  if (! serialize_u32(serialize, src->size))
    return NULL;
  if ((r = buf_write(&serialize->buf, src->ptr.pchar, src->size)) <= 0)
    return NULL;
  return serialize;
}

DEF_SERIALIZE(sw)

s_serialize * serialize_tag (s_serialize *serialize, const s_tag *tag)
{
  serialize_u8(serialize, tag->type);
  switch (tag->type){
  case TAG_BOOL: return serialize_bool(serialize, tag->data.bool_);
  case TAG_CHARACTER:
    return serialize_character(serialize, tag->data.character);
  case TAG_S8:   return serialize_s8(serialize, tag->data.s8);
  case TAG_S16:  return serialize_s16(serialize, tag->data.s16);
  case TAG_S32:  return serialize_s32(serialize, tag->data.s32);
  case TAG_S64:  return serialize_s64(serialize, tag->data.s64);
  case TAG_STR:  return serialize_str(serialize, &tag->data.str);
  case TAG_SW:   return serialize_sw(serialize, tag->data.sw);
  case TAG_U8:   return serialize_u8(serialize, tag->data.u8);
  case TAG_U16:  return serialize_u16(serialize, tag->data.u16);
  case TAG_U32:  return serialize_u32(serialize, tag->data.u32);
  case TAG_U64:  return serialize_u64(serialize, tag->data.u64);
  case TAG_UW:   return serialize_uw(serialize, tag->data.uw);
  default:       break;
  }
  err_puts("serialize_tag: not implemented");
  assert(! "serialize_tag: not implemented");
  return NULL;
}

sw serialize_to_buf (s_serialize *serialize, s_buf *buf)
{
  return buf_xfer(&serialize->buf, buf,
                  serialize->buf.wpos - serialize->buf.rpos);
}

s_str * serialize_to_str (s_serialize *serialize, s_str *dest)
{
  return buf_read_to_str(&serialize->buf, dest);
}

s_serialize * serialize_tuple (s_serialize *serialize,
                               const s_tuple *tuple)
{
  uw i;
  assert(serialize);
  assert(tuple);
  if (! serialize_u32(serialize, tuple->count))
    return NULL;
  i = 0;
  while (i < tuple->count) {
    if (! serialize_tag(serialize, tuple->tag + i))
      return NULL;
    i++;
  }
  return serialize;
}

DEF_SERIALIZE(u8)
DEF_SERIALIZE(u16)
DEF_SERIALIZE(u32)
DEF_SERIALIZE(u64)
DEF_SERIALIZE(uw)
