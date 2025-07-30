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
#include "alloc.h"
#include "assert.h"
#include "buf.h"
#include "ht.h"
#include "list.h"
#include "marshall.h"
#include "str.h"
#include "marshall_read.h"
#include "rwlock.h"
#include "tag.h"

#define DEF_MARSHALL_READ(name, type)                                  \
  s_marshall_read * marshall_read_ ## name (s_marshall_read *mr,       \
                                            bool heap,                 \
                                            type *dest)                \
  {                                                                    \
    s_buf *buf = {0};                                                  \
    assert(mr);                                                        \
    assert(dest);                                                      \
    buf = heap ? &mr->heap : &mr->buf;                                 \
    if (buf_read_ ## name (buf, dest) <= 0) {                          \
      err_puts("marshall_read_" # name ": buf_read_" # type);          \
      assert(! "marshall_read_" # name ": buf_read_" # type);          \
      return NULL;                                                     \
    }                                                                  \
    return mr;                                                         \
  }

DEF_MARSHALL_READ(bool, bool)

s_marshall_read * marshall_read_array (s_marshall_read *mr,
                                       bool heap,
                                       s_array *dest)
{
  s_array tmp = {0};
  assert(mr);
  assert(dest);
  if (! marshall_read_uw(mr, heap, &tmp.count)              ||
      ! marshall_read_uw(mr, heap, &tmp.dimension)          ||
      //! marshall_read_dimensions(mr, heap, &tmp.dimensions) ||
      //! marshall_read_uw(mr, heap, &tmp.free_data)          ||
      ! marshall_read_uw(mr, heap, &tmp.size)               ||
      ! marshall_read_tag(mr, heap, tmp.tags)               ||
      ! marshall_read_psym(mr, heap, &tmp.array_type)       ||
      ! marshall_read_psym(mr, heap, &tmp.element_type)     ||
      ! marshall_read_sw(mr, heap, &tmp.ref_count))
    return NULL;
  *dest = tmp;
  return mr;
}

s_marshall_read * marshall_read_array_data (s_marshall_read *mr,
                                            bool heap,
                                            s_array *src)
{
  for (u8 i = 0; i <= src->count; i++) {
    if (marshall_read_uw(mr, heap, &((uw *)src->data)[i]))
      return NULL;
  }
  return mr;
}

s_marshall_read * marshall_read_character (s_marshall_read *mr,
                                           bool heap,
                                           character *dest)
{
  s_buf *buf = {0};
  assert(mr);
  assert(dest);
  buf = heap ? &mr->heap : &mr->buf;
  return buf_read_character_utf8(buf, dest) < 0 ? NULL : mr;
}

void marshall_read_clean (s_marshall_read *mr)
{
  assert(mr);
  buf_clean(&mr->buf);
}

s_marshall_read * marshall_read_dimensions (s_marshall_read *mr,
                                            bool heap,
                                            s_array_dimension *dest)
{
  s_array_dimension tmp = {0};
  assert(mr);
  assert(dest);
  if (! marshall_read_uw(mr, heap, &tmp.count) ||
      ! marshall_read_uw(mr, heap, &tmp.item_size))
    return NULL;
  *dest = tmp;
  return mr;
}

DEF_MARSHALL_READ(f128, f128)
DEF_MARSHALL_READ(f32, f32)
DEF_MARSHALL_READ(f64, f64)

s_marshall_read * marshall_read_header (s_marshall_read *mr)
{
  s_marshall_header mh = {0};
  s_str str = {0};
  s_marshall_read tmp;
  assert(mr);
  tmp = *mr;
  if (! buf_read(&tmp.buf, sizeof(s_marshall_header), &str)) {
    err_puts("marshall_read_header: buf_read");
    assert(! "marshall_read_header: buf_read");
    return NULL;
  }
  if (str.size != sizeof(s_marshall_header)) {
    err_puts("marshall_read_header: buf_read !="
             " sizeof(s_marshall_header)");
    assert(!("marshall_read_header: buf_read !="
             " sizeof(s_marshall_header)"));
    return NULL;
  }
  mh = *(s_marshall_header *) str.ptr.p;
  if (le64toh(mh.le_magic) != MARSHALL_MAGIC) {
    err_puts("marshall_read_header: invalid magic");
    assert(! "marshall_read_header: invalid magic");
    return NULL;
  }
  tmp.heap_count = le64toh(mh.le_heap_count);
  tmp.heap_size = le64toh(mh.le_heap_size);
  tmp.buf_size = le64toh(mh.le_buf_size);
  *mr = tmp;
  str_clean(&str);
  return mr;
}

s_marshall_read * marshall_read_heap_pointer (s_marshall_read *mr,
                                              bool heap,
                                              u64 *offset,
                                              void **present)
{
  assert(mr);
  assert(offset);
  assert(present);
  s_tag key = {0};
  s_tag tag = {0};
  if (! marshall_read_u64(mr, heap, offset))
    return NULL;
  if (! *offset) {
    *present = NULL;
    return mr;
  }
  if (! tag_init_tuple(&key, 2))
    return NULL;
  tag_init_u64(key.data.tuple.tag, *offset);
  if (! ht_get(&mr->ht, &key, &tag)) {
    *present = NULL;
    return mr;
  }
  if (tag.type != TAG_TUPLE ||
      tag.data.tuple.tag[1].type != TAG_UW) {
    err_puts("marshall_read_heap_pointer: invalid tag in ht");
    assert(! "marshall_read_heap_pointer: invalid tag in ht");
  }
  *present = (void *)tag.data.tuple.tag[1].data.uw;
  return mr;
}

s_marshall_read * marshall_read_ht_add (s_marshall_read *mr,
                                        u64 offset, void *p)
{
  s_tag tag = {0};
  if (! tag_init_tuple(&tag, 2))
    return NULL;
  tag_init_u64(tag.data.tuple.tag, offset);
  tag_init_uw(tag.data.tuple.tag, (uw) p);
  if (! ht_add(&mr->ht, &tag)) {
    tag_clean(&tag);
    return NULL;
  }
  return mr;
}

s_marshall_read * marshall_read_init (s_marshall_read *mr)
{
  if (mr == NULL)
    return NULL;
  if (! buf_init_alloc(&mr->heap, BUF_SIZE))
    return NULL;
  if (! buf_init_alloc(&mr->buf, BUF_SIZE))
    return NULL;
  return mr;
}

s_marshall_read * marshall_read_init_1 (s_marshall_read *mr,
                                        const char *p, uw size)
{
  s_str str = {0};
  assert(mr);
  if (! p || ! size)
    return NULL;
  str.size = size;
  str.ptr.pchar = p;
  return marshall_read_init_str(mr, &str);
}

s_marshall_read * marshall_read_init_str (s_marshall_read *mr,
                                          const s_str *src)
{
  s_marshall_read tmp = {0};
  assert(mr);
  if (! buf_init_str_copy(&tmp.buf, src)) {
    err_puts("marshall_read_init_str: buf_init_str_copy");
    assert(! "marshall_read_init_str: buf_init_str_copy");
    return NULL;
  }
  if (! marshall_read_header(&tmp)) {
    err_puts("marshall_read_init_str: marshall_read_header");
    assert(! "marshall_read_init_str: marshall_read_header");
    buf_clean(&tmp.buf);
    return NULL;
  }
  tmp.buf.rpos = sizeof(s_marshall_header) + tmp.heap_size;
  if (tmp.buf.rpos + tmp.buf_size != tmp.buf.wpos) {
    err_puts("marshall_read_init_str: invalid buffer size");
    assert(! "marshall_read_init_str: invalid buffer size");
    buf_clean(&tmp.buf);
    return NULL;
  }
  tmp.heap = tmp.buf;
  tmp.heap.free = false;
  tmp.heap.rpos = sizeof(s_marshall_header);
  tmp.heap.wpos = tmp.heap.rpos + tmp.heap_size;
#ifdef HAVE_PTHREAD
  rwlock_init(&tmp.heap.rwlock);
#endif
  *mr = tmp;
  return mr;
}

DEF_MARSHALL_READ(integer, s_integer)

s_marshall_read * marshall_read_plist (s_marshall_read *mr,
                                       bool heap,
                                       p_list *dest)
{
  u64 offset = 0;
  void *present = NULL;
  p_list tmp = NULL;
  assert(mr);
  assert(dest);
  if (! marshall_read_heap_pointer(mr, heap, &offset, &present))
    return NULL;
  if (present || ! offset) {
    *dest = present;
    return mr;
  }
  if (buf_seek(&mr->heap, offset, SEEK_SET) != offset) {
    err_write_1(__func__);
    err_puts(": buf_seek");
    return NULL;
  }
  tmp = alloc(sizeof(s_list));
  if (! tmp)
    return NULL;
  if (! marshall_read_list(mr, true, tmp))
    return NULL;
  marshall_read_ht_add(mr, offset, tmp);
  *dest = tmp;
  return mr;
}

s_marshall_read * marshall_read_list (s_marshall_read *mr, bool heap,
                                       s_list *dest)
{
  s_list tmp = {0};
  assert(mr);
  assert(dest);
  if (! marshall_read_tag(mr, heap, &tmp.tag))
    return NULL;
  if (! marshall_read_tag(mr, heap, &tmp.next)) {
    tag_clean(&tmp.tag);
    return NULL;
  }
  *dest = tmp;
  return mr;
}

DEF_MARSHALL_READ(s8, s8)
DEF_MARSHALL_READ(s16, s16)
DEF_MARSHALL_READ(s32, s32)
DEF_MARSHALL_READ(s64, s64)


s_marshall_read * marshall_read_sym (s_marshall_read *mr,
                                     bool heap, s_sym *dest)
{
  s_sym tmp = {0};
  assert(mr);
  assert(dest);
  if (! marshall_read_str(mr, heap, &tmp.str))
    return NULL;
  *dest = tmp;
  return mr;
}

s_marshall_read * marshall_read_str (s_marshall_read *mr,
                                     bool heap, s_str *dest)
{
  s_buf *buf = {0};
  assert(mr);
  assert(dest);
  buf = heap ? &mr->heap : &mr->buf;
  return buf_read_str(buf, dest) < 0 ? NULL : mr;
}

DEF_MARSHALL_READ(sw, sw)

s_marshall_read * marshall_read_tag (s_marshall_read *mr, bool heap,
                                     s_tag *dest)
{
  u8 u = 0;
  assert(mr);
  assert(dest);
  marshall_read_u8(mr, heap, &u);
  dest->type = u;
  switch (dest->type) {
    case TAG_VOID:
      return mr;
    case TAG_ARRAY:
      return marshall_read_array(mr, heap, &dest->data.array);
    case TAG_DO_BLOCK:
      return marshall_read_do_block(mr, heap, &dest->data.do_block);
    case TAG_BOOL:
      return marshall_read_bool(mr, heap, &dest->data.bool_);
    case TAG_CALL:
      return marshall_read_call(mr, heap, &dest->data.call);
    case TAG_CHARACTER:
      return marshall_read_character(mr, heap, &dest->data.character);
    case TAG_PCOMPLEX:
      return marshall_read_pcomplex(mr, heap, &dest->data.pcomplex);
    case TAG_PCOW:
      return marshall_read_pcow(mr, heap, &dest->data.pcow);
    case TAG_F32:
      return marshall_read_f32(mr, heap, &dest->data.f32);
    case TAG_F64:
      return marshall_read_f64(mr, heap, &dest->data.f64);
    case TAG_F128:
      return marshall_read_f128(mr, heap, &dest->data.f128);
    case TAG_FACT:
      return marshall_read_fact(mr, heap, &dest->data.fact);
    case TAG_IDENT:
      return marshall_read_ident(mr, heap, &dest->data.ident);
    case TAG_INTEGER:
      return marshall_read_integer(mr, heap, &dest->data.integer);
    case TAG_PLIST:
      return marshall_read_plist(mr, heap, &dest->data.plist);
    case TAG_MAP:
      return marshall_read_map(mr, heap, &dest->data.map);
    case TAG_PCALLABLE:
      return marshall_read_pcallable(mr, heap, &dest->data.pcallable);
    case TAG_PSTRUCT:
      return marshall_read_pstruct(mr, heap, &dest->data.pstruct);
    case TAG_PSTRUCT_TYPE:
      return marshall_read_pstruct_type(mr, heap,
                                        &dest->data.pstruct_type);
    case TAG_PSYM:
      return marshall_read_psym(mr, heap, &dest->data.psym);
    case TAG_PTAG:
      return marshall_read_ptag(mr, heap, &dest->data.ptag);
    case TAG_PTR:
      return marshall_read_ptr(mr, heap, &dest->data.ptr);
    case TAG_PTR_FREE:
      return marshall_read_ptr_free(mr, heap, &dest->data.ptr_free);
    case TAG_QUOTE:
      return marshall_read_quote(mr, heap, &dest->data.quote);
    case TAG_RATIO:
      return marshall_read_ratio(mr, heap, &dest->data.ratio);
    case TAG_STR:
      return marshall_read_str(mr, heap, &dest->data.str);
    case TAG_S8:
      return marshall_read_s8(mr, heap, &dest->data.s8);
    case TAG_S16:
      return marshall_read_s16(mr, heap, &dest->data.s16);
    case TAG_S32:
      return marshall_read_s32(mr, heap, &dest->data.s32);
    case TAG_S64:
      return marshall_read_s64(mr, heap, &dest->data.s64);
    case TAG_SW:
      return marshall_read_sw(mr, heap, &dest->data.sw);
    case TAG_TIME:
      return marshall_read_time(mr, heap, &dest->data.time);
    case TAG_TUPLE:
      return marshall_read_tuple(mr, heap, &dest->data.tuple);
    case TAG_U8:
      return marshall_read_u8(mr, heap, &dest->data.u8);
    case TAG_U16:
      return marshall_read_u16(mr, heap, &dest->data.u16);
    case TAG_U32:
      return marshall_read_u32(mr, heap, &dest->data.u32);
    case TAG_U64:
      return marshall_read_u64(mr, heap, &dest->data.u64);
    case TAG_UNQUOTE:
      return marshall_read_unquote(mr, heap, &dest->data.unquote);
    case TAG_UW:
      return marshall_read_uw(mr, heap, &dest->data.uw);
    case TAG_PVAR:
      return marshall_read_pvar(mr, heap, &dest->data.pvar);
  }
  err_puts("marshall_tag: not implemented");
  assert(!"marshall_tag: not implemented");
  return NULL;
}

DEF_MARSHALL_READ(u8,  u8)
DEF_MARSHALL_READ(u16, u16)
DEF_MARSHALL_READ(u32, u32)
DEF_MARSHALL_READ(u64, u64)
DEF_MARSHALL_READ(uw, uw)

// more complex types :

#define DEF_MARSHALL_READ_STUB(name, type)                             \
  PROTO_MARSHALL_READ(name, type)                                      \
  {                                                                    \
    (void) mr;                                                         \
    (void) heap;                                                       \
    (void) dest;                                                       \
    err_puts("marshall_read_" # name ": not implemented");             \
    assert(! "marshall_read_" # name ": not implemented");             \
    return NULL;                                                       \
  }

DEF_MARSHALL_READ_STUB(call, s_call)
DEF_MARSHALL_READ_STUB(callable, s_callable)
DEF_MARSHALL_READ_STUB(cow, s_cow)
DEF_MARSHALL_READ_STUB(complex, s_complex)
DEF_MARSHALL_READ_STUB(do_block, s_do_block)
DEF_MARSHALL_READ_STUB(fact, s_fact)
DEF_MARSHALL_READ_STUB(ident, s_ident)
DEF_MARSHALL_READ_STUB(map, s_map)
DEF_MARSHALL_READ_STUB(quote, s_quote)
DEF_MARSHALL_READ_STUB(ratio, s_ratio)
DEF_MARSHALL_READ_STUB(struct, s_struct)
DEF_MARSHALL_READ_STUB(struct_type, s_struct_type)
DEF_MARSHALL_READ_STUB(tuple, s_tuple)
DEF_MARSHALL_READ_STUB(time, s_time)
DEF_MARSHALL_READ_STUB(unquote, s_unquote)
DEF_MARSHALL_READ_STUB(var, s_var)
DEF_MARSHALL_READ_STUB(pcallable, p_callable)
DEF_MARSHALL_READ_STUB(pcomplex, p_complex)
DEF_MARSHALL_READ_STUB(pcow, p_cow)
DEF_MARSHALL_READ_STUB(pstruct, p_struct)
DEF_MARSHALL_READ_STUB(pstruct_type, p_struct_type)
DEF_MARSHALL_READ_STUB(psym, p_sym)
DEF_MARSHALL_READ_STUB(ptag, p_tag)
DEF_MARSHALL_READ_STUB(ptr, u_ptr_w)
DEF_MARSHALL_READ_STUB(ptr_free, u_ptr_w)
DEF_MARSHALL_READ_STUB(pvar, p_var)
