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
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include "assert.h"
#include "alloc.h"
#include "buf.h"
#include "buf_file.h"
#include "file.h"
#include "ht.h"
#include "sym.h"
#include "list.h"
#include "str.h"
#include "tag.h"
#include "tag_init.h"
#include "types.h"
#include "marshall.h"

#define DEF_MARSHALL(type)                                             \
  s_marshall * marshall_ ## type (s_marshall *m, bool heap, type src)  \
  {                                                                    \
    s_buf *buf;                                                        \
    type le;                                                           \
    sw r;                                                              \
    assert(m);                                                         \
    le = _Generic(src,                                                 \
                  s16:     htole16(src),                               \
                  u16:     htole16(src),                               \
                  s32:     htole32(src),                               \
                  u32:     htole32(src),                               \
                  s64:     htole64(src),                               \
                  u64:     htole64(src),                               \
                  default: src);                                       \
    buf = heap ? &m->heap : &m->buf;                                   \
    if ((r = buf_write_ ## type(buf, le)) <= 0)                        \
      return NULL;                                                     \
    if (heap)                                                          \
      m->heap_pos += r;                                                \
    else                                                               \
      m->buf_pos += r;                                                 \
    return m;                                                          \
  }

DEF_MARSHALL(bool)

s_marshall * marshall_heap_pointer (s_marshall *m, bool heap, void *p,
                                    bool *present)
{
  s_buf *buf;
  s_tag key = {0};
  sw r;
  s_tag tag = {0};
  assert(m);
  assert(p);
  buf = heap ? &m->heap : &m->buf;
  tag_init_tuple(&key, 2);
  key.data.tuple.tag[0].data.u64 = (u64) p;
  if (ht_get(&m->ht, &key, &tag)) {
    *present = true;
    goto ok;
  }
  *present = false;
  tag_init_tuple(&tag, 2);
  tag.data.tuple.tag[0].data.u64 = (u64) p;
  tag.data.tuple.tag[1].data.u64 = (u64) m->heap_pos;
  if (! ht_add(&m->ht, &tag))
    goto ko;
 ok:
  if ((r = buf_write_u64(buf, tag.data.tuple.tag[1].data.u64)) <= 0)
    goto ko;
  tag_clean(&key);
  tag_clean(&tag);
  if (heap)
    m->heap_pos += r;
  else
    m->buf_pos += r;
  return m;
 ko:
  tag_clean(&key);
  tag_clean(&tag);
  return NULL;
}

s_marshall * marshall_character (s_marshall *m, bool heap,
                                 character src)
{
  sw r;
  s_buf *buf;
  assert(m);
  buf = heap ? &m->heap : &m->buf;
  if ((r = buf_write_character_utf8(buf, src)) <= 0)
    return NULL;
  if (heap)
    m->heap_pos += r;
  else
    m->buf_pos += r;
  return m;
}

void marshall_clean (s_marshall *m)
{
  assert(m);
  buf_clean(&m->buf);
  buf_clean(&m->heap);
}

void marshall_delete (s_marshall *m)
{
  marshall_clean(m);
  free(m);
}

s_marshall * marshall_init (s_marshall *m)
{
  s_marshall tmp = {0};
  if (! ht_init(&tmp.ht, &g_sym_Tag, 1024) ||
    ! buf_init_alloc(&tmp.heap, 1024024))
    return NULL;
  if (! buf_init_alloc(&tmp.buf, 1024024)) {
    buf_delete(&tmp.heap);
    return NULL;
  }
  *m = tmp;
  return m;
}

s_marshall * marshall_list (s_marshall *m, bool heap,
                            const s_list *list)
{
  if (! marshall_tag(m, heap, &list->tag) ||
      ! marshall_tag(m, heap, &list->next))
    return NULL;
  return m;
}

s_marshall * marshall_new (void)
{
  s_marshall *m;
  if (! (m = alloc(sizeof(s_marshall))))
    return NULL;
  if (marshall_init(m) == NULL) {
    free(m);
    return NULL;
  }
  return m;
}

DEF_MARSHALL(s8)
DEF_MARSHALL(s16)
DEF_MARSHALL(s32)
DEF_MARSHALL(s64)

s_marshall * marshall_str (s_marshall *m, bool heap, const s_str *src)
{
  sw r;
  s_buf *buf;
  
  assert(m);
  assert(src);
  if (! marshall_u32(m, heap, src->size))
    return NULL;
  buf = heap ? &m->heap : &m->buf;
  if ((r = buf_write(buf, src->ptr.pchar, src->size)) <= 0)
    return NULL;
  if (heap)
    m->heap_pos += r;
  else
    m->buf_pos += r;
  return m;
}

DEF_MARSHALL(sw)

s_marshall * marshall_tag (s_marshall *m, bool heap, const s_tag *tag)
{
  bool present = false;
  marshall_u8(m, heap, tag->type);
  switch (tag->type){
  case TAG_BOOL: return marshall_bool(m, heap, tag->data.bool_);
  case TAG_CHARACTER:
    return marshall_character(m, heap, tag->data.character);
  case TAG_S8:   return marshall_s8(m, heap, tag->data.s8);
  case TAG_U8:   return marshall_u8(m, heap, tag->data.u8);
  case TAG_S16:  return marshall_s16(m, heap, tag->data.s16);
  case TAG_U16:  return marshall_u16(m, heap, tag->data.u16);
  case TAG_S32:  return marshall_s32(m, heap, tag->data.s32);
  case TAG_U32:  return marshall_u32(m, heap, tag->data.u32);
  case TAG_S64:  return marshall_s64(m, heap, tag->data.s64);
  case TAG_U64:  return marshall_u64(m, heap, tag->data.u64);
  case TAG_STR:  return marshall_str(m, heap, &tag->data.str);
  case TAG_SW:   return marshall_sw(m, heap, tag->data.sw);
  case TAG_UW:   return marshall_uw(m, heap, tag->data.uw);
  case TAG_LIST:
    if (! marshall_heap_pointer(m, heap, (void *) &tag->data.list,
                                &present))
      return NULL;
    if (! present)
      marshall_list(m, true, tag->data.list);
    return m;
  default:       break;
  }
  err_puts("marshall_tag: not implemented");
  assert(! "marshall_tag: not implemented");
  return NULL;
}

sw marshall_to_buf (s_marshall *m, s_buf *out)
{
  s_marshall_header mh = {0};
  sw r;
  sw result = 0;
  assert(m);
  assert(out);
  mh.le_magic = htole64(MARSHALL_MAGIC);
  mh.le_heap_count = htole64(m->heap_count);
  mh.le_heap_size = htole64(m->heap_pos);
  mh.le_buf_size = htole64(m->buf_pos);
  if ((r = buf_write(out, &mh, sizeof(mh))) != sizeof(mh))
    return -1;
  result += r;
  if ((r = buf_xfer(out, &m->heap, m->heap_pos)) != m->heap_pos)
    return -1;
  result += r;
  if ((r = buf_xfer(out, &m->buf, m->buf_pos)) != m->buf_pos)
    return -1;
  result += r;
  return result;
}

sw marshall_to_file (s_marshall *m, const char *path)
{
  FILE *fp;
  s_buf out;
  sw r;
  sw result = 0;
  assert(m);
  assert(path);
  if (! buf_init_alloc(&out, 1024 * 1024))
    return -1;
  if (! (fp = file_open(path, "wb")) ||
      ! buf_file_open_w(&out, fp) ||
      (r = marshall_to_buf(m, &out)) <= 0) {
    buf_clean(&out);
    return -1;
  }
  result = r;
  buf_file_close(&out);
  buf_clean(&out);
  return result;
}

s_str * marshall_to_str (s_marshall *m, s_str *dest)
{
  s_buf out;
  sw r;
  assert(m);
  assert(dest);
  if (! buf_init_alloc(&out, 1024 * 1024))
    return NULL;
  if ((r = marshall_to_buf(m, &out)) <= 0) {
    buf_clean(&out);
    return NULL;
  }
  if (! buf_read_to_str(&out, dest)) {
    buf_clean(&out);
    return NULL;
  }
  buf_clean(&out);
  return dest;
}

s_marshall * marshall_tuple (s_marshall *m, bool heap, const s_tuple *tuple)
{
  uw i;
  assert(m);
  assert(tuple);
  if (! marshall_uw(m, heap, tuple->count))
    return NULL;
  i = 0;
  while (i < tuple->count) {
    if (! marshall_tag(m, heap, tuple->tag + i))
      return NULL;
    i++;
  }
  return m;
}

DEF_MARSHALL(u8)
DEF_MARSHALL(u16)
DEF_MARSHALL(u32)
DEF_MARSHALL(u64)
DEF_MARSHALL(uw)
