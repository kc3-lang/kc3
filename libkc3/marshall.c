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
#include "list.h"
#include "str.h"
#include "types.h"
#include "marshall.h"

#define DEF_MARSHALL(type)                                             \
  s_marshall * marshall_ ## type (s_marshall *marshall, type src)      \
  {                                                                    \
    sw r;                                                              \
    assert(marshall);                                                  \
    src = _Generic(src,                                                \
                   s16:       htole16(src),                            \
                   u16:       htole16(src),                            \
                   s32:       htole32(src),                            \
                   u32:       htole32(src),                            \
                   s64:       htole64(src),                            \
                   u64:       htole64(src),                            \
                   default: src);                                      \
    if ((r = buf_write_ ## type(&marshall->buf, src)) <= 0)            \
      return NULL;                                                     \
    return marshall;                                                   \
  }

/*
 * Initialization
*/

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
s_marshall * marshall_init (s_marshall *marshall)
{
  s_marshall tmp = {0};
  if (buf_init_alloc(&tmp.heap, 1024024) == NULL)
    return NULL;
  if (buf_init_alloc(&tmp.buf, 1024024) == NULL) {
    buf_delete(&tmp.heap);
    return NULL;
  }
  *marshall = tmp;
  return marshall;
}

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

/*
 * Util functions
*/

// static marshall_header_t *marshall_set_type(marshall_header_t *header,
//   e_tag_type type)
// {
//   header->type = type;
//   return header;
// }

static marshall_header_t *marshall_write_header(s_marshall *marshall,
    marshall_header_t *header)
{
  if (header == NULL || marshall == NULL)
    return NULL;
  header->buf_size = htole64(marshall->buf.wpos);
  header->buf_offset = htole64(sizeof(*header));
  header->heap_size = htole64(marshall->heap.wpos);
  header->heap_offset = htole64(header->buf_offset + header->buf_size);
  return header;
}

ssize_t marshall_save_to_file(s_marshall *marshall, const char *path)
{
  FILE *fp;
  marshall_header_t header = {0};
  s_buf *buf;
  s_buf *heap;
  ssize_t ret = 0;

  if (marshall_write_header(marshall, &header) == NULL
    || path == NULL || (fp = file_open(path, "wb")) == NULL)
    return -1;
  buf = &marshall->buf;
  heap = &marshall->heap;
  if (fwrite((uint8_t *)&header, sizeof(header), 1, fp) < 1
    || buf_file_open_w(buf, fp) == NULL
    || marshall->buf.flush(buf) < 0
    || buf_file_open_w(heap, fp) == NULL
    || marshall->buf.flush(heap) < 0)
      ret = -1;
  buf_file_close(buf);
  buf_file_close(heap);
  fclose(fp);
  return ret;
}

static marshall_header_t *marshall_read_header(
    marshall_header_t *header)
{
  #if __BYTE_ORDER == __BIG_ENDIAN
    header->buf_size = htobe64(header->buf_size);
    header->buf_offset = htobe64(header->buf_offset);
    header->heap_size = htobe64(header->heap_size);
    header->heap_offset = htobe64(header->heap_offset);
  #endif
  return header;
}

s_marshall *marshall_read_from_file(const char *path)
{
  s_marshall *marshall;
  marshall_header_t header = {0};
  s_str p = {0};
  s_str s;
    
  if (path == NULL)
    return NULL;
  if (str_init(&p, NULL, strlen(path), path) == NULL
    || file_read_all(&p, &s) == NULL
    || s.size < sizeof(marshall_header_t)
    || s.ptr.pu8 == NULL
    || (marshall = marshall_new()) == NULL)
    return NULL;
  header = *marshall_read_header((marshall_header_t *)s.ptr.pu8);
  if (buf_write(&marshall->buf, s.ptr.pu8 + header.buf_offset,
    header.buf_size) < 0
    || buf_write(&marshall->heap, s.ptr.pu8 + header.heap_offset,
    header.heap_size) < 0) {
    marshall_delete(marshall);
    return NULL;
  }
  marshall->buf.rpos = 0;
  marshall->heap.rpos = 0;
  marshall->buf.size = header.buf_size;
  marshall->heap.size = header.heap_size;
  return marshall;
}

/*
 * Types
*/

DEF_MARSHALL(bool)

s_marshall * marshall_character (s_marshall *marshall, character src)
{
  sw r;
  if ((r = buf_write_character_utf8(&marshall->buf, src)) <= 0)
    return NULL;
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
  case TAG_S8://   return marshall_s8(marshall, tag->data.s8);
  case TAG_U8:   return marshall_u8(marshall, tag->data.u8);
  case TAG_S16://  return marshall_s16(marshall, tag->data.s16);
  case TAG_U16:  return marshall_u16(marshall, tag->data.u16);
  case TAG_S32://  return marshall_s32(marshall, tag->data.s32);
  case TAG_U32:  return marshall_u32(marshall, tag->data.u32);
  case TAG_S64://  return marshall_s64(marshall, tag->data.s64);
  case TAG_U64:  return marshall_u64(marshall, tag->data.u64);
  case TAG_STR:  return marshall_str(marshall, &tag->data.str);
  case TAG_SW://   return marshall_sw(marshall, tag->data.sw);
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
