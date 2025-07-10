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
#include "marshall.h"

#define DEF_MARSHALL(type)                                             \
  s_marshall * marshall_ ## type (s_marshall *marshall, type src)      \
  {                                                                    \
    sw r;                                                              \
    src = _Generic(src,                                                \
                   character: htole32(src),                            \
                   s16:       htole16(src),                            \
                   s32:       htole32(src),                            \
                   s64:       htole64(src),                            \
                   u16:       htole16(src),                            \
                   u32:       htole32(src),                            \
                   u64:       htole64(src),                            \
                   default: src);                                      \
    assert(marshall);                                                  \
    if ((r = buf_write_ ## type(&marshall->buf, src)) <= 0)            \
      return NULL;                                                     \
    return marshall;                                                   \
  }

DEF_MARSHALL(bool)

void marshall_clean (s_marshall *marshall)
{
  assert(marshall);
  buf_clean(&marshall->buf);
  buf_clean(&marshall->heap);
}

void marshall_delete (s_marshall *marshall)
{
  marshall_clean(marshall);
  free(marshall);
}

s_marshall * marshall_character (s_marshall *marshall,
                                   character c)
{
  return marshall_u32(marshall, c);
}

s_marshall * marshall_init (s_marshall *marshall)
{
  s_marshall tmp = {0};
  if (buf_init_alloc(&tmp.heap, 1024024) == NULL)
    return NULL;
  if (buf_init_alloc(&tmp.buf, 1024024) == NULL) {
    buf_delete(&tmp.buf);
    return NULL;
  }
  *marshall = tmp;
  return marshall;
}

/* Works the same as marshall_tuple. */
s_marshall * marshall_list (s_marshall *marshall,
                              const s_list *list)
{
  const s_list *l;
  u32 len;
  assert(marshall);
  len = list_length(list);
  if (! marshall_u32(marshall, len))
    return NULL;
  l = list;
  while (l) {
    if (! marshall_tag(marshall, &l->tag))
      return NULL;
    l = list_next(l);
  }
  return marshall;
}

s_marshall * marshall_new (void)
{
  s_marshall *marshall;
  if (! (marshall = alloc(sizeof(s_marshall))))
    return NULL;
  if (marshall_init(marshall) == NULL) {
    free(marshall);
    return NULL;
  }
  return marshall;
}

DEF_MARSHALL(s8)
DEF_MARSHALL(s16)
DEF_MARSHALL(s32)
DEF_MARSHALL(s64)

s_marshall * marshall_str (s_marshall *marshall, const s_str *src)
{
  sw r;
  assert(marshall);
  assert(src);
  if (! marshall_u32(marshall, src->size))
    return NULL;
  if ((r = buf_write(&marshall->buf, src->ptr.pchar, src->size)) <= 0)
    return NULL;
  return marshall;
}

DEF_MARSHALL(sw)

s_marshall * marshall_tag (s_marshall *marshall, const s_tag *tag)
{
  marshall_u8(marshall, tag->type);
  switch (tag->type){
  case TAG_BOOL: return marshall_bool(marshall, tag->data.bool_);
  case TAG_CHARACTER:
    return marshall_character(marshall, tag->data.character);
  case TAG_S8:   return marshall_s8(marshall, tag->data.s8);
  case TAG_S16:  return marshall_s16(marshall, tag->data.s16);
  case TAG_S32:  return marshall_s32(marshall, tag->data.s32);
  case TAG_S64:  return marshall_s64(marshall, tag->data.s64);
  case TAG_STR:  return marshall_str(marshall, &tag->data.str);
  case TAG_SW:   return marshall_sw(marshall, tag->data.sw);
  case TAG_U8:   return marshall_u8(marshall, tag->data.u8);
  case TAG_U16:  return marshall_u16(marshall, tag->data.u16);
  case TAG_U32:  return marshall_u32(marshall, tag->data.u32);
  case TAG_U64:  return marshall_u64(marshall, tag->data.u64);
  case TAG_UW:   return marshall_uw(marshall, tag->data.uw);
  default:       break;
  }
  err_puts("marshall_tag: not implemented");
  assert(! "marshall_tag: not implemented");
  return NULL;
}

sw marshall_to_buf (s_marshall *marshall, s_buf *buf)
{
  return buf_xfer(&marshall->buf, buf,
                  marshall->buf.wpos - marshall->buf.rpos);
}

s_str * marshall_to_str (s_marshall *marshall, s_str *dest)
{
  return buf_read_to_str(&marshall->buf, dest);
}

s_marshall * marshall_tuple (s_marshall *marshall,
                               const s_tuple *tuple)
{
  uw i;
  assert(marshall);
  assert(tuple);
  if (! marshall_u32(marshall, tuple->count))
    return NULL;
  i = 0;
  while (i < tuple->count) {
    if (! marshall_tag(marshall, tuple->tag + i))
      return NULL;
    i++;
  }
  return marshall;
}

DEF_MARSHALL(u8)
DEF_MARSHALL(u16)
DEF_MARSHALL(u32)
DEF_MARSHALL(u64)
DEF_MARSHALL(uw)
