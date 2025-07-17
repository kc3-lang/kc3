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

#include "tag.h"
#include "types.h"
#include "assert.h"
#include "marshall.h"
#include "marshall_read.h"
#include "buf.h"

#define DEF_MARSHALL_READ(name, type)                                  \
  s_marshall_read * marshall_read_ ## name (s_marshall_read *mr,       \
                                            bool heap,                 \
                                            type *dest)                \
  {                                                                    \
    s_buf *buf = {0};                                                  \
    assert(mr);                                                        \
    assert(dest);                                                      \
    buf = heap ? &mr->heap : &mr->buf;                                 \
    if (buf_read_ ## type (buf, dest) <= 0)                            \
      return NULL;                                                     \
    return mr;                                                         \
  }

DEF_MARSHALL_READ(bool, bool)

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

DEF_MARSHALL_READ(f128, f128)
DEF_MARSHALL_READ(f32, f32)
DEF_MARSHALL_READ(f64, f64)

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
    case TAG_COMPLEX:
      return marshall_read_pcomplex(mr, heap, &dest->data.complex);
    case TAG_COW:
      return marshall_read_pcow(mr, heap, &dest->data.cow);
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
    case TAG_LIST:
      return marshall_read_plist(mr, heap, dest->data.list);
    case TAG_MAP:
      return marshall_read_map(mr, heap, &dest->data.map);
    case TAG_PCALLABLE:
      return marshall_read_pcallable(mr, heap, &dest->data.pcallable);
    case TAG_PSTRUCT:
      return marshall_read_pstruct(mr, heap, &dest->data.pstruct);
    case TAG_PSTRUCT_TYPE:
      return marshall_read_pstruct_type(mr, heap,
                                        &dest->data.pstruct_type);
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
    case TAG_SYM:
      return marshall_read_psym(mr, heap, &dest->data.sym);
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
    case TAG_VAR:
      return marshall_read_var(mr, heap, &dest->data.var);
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

#undef DEF_MARSHALL_READ
#define DEF_MARSHALL_READ(name, type) PROTO_MARSHALL_READ(name, type) \
    {                                                                 \
        (void) mr;                                                    \
        (void) heap;                                                  \
        (void) dest;                                                  \
        err_puts("marshall_read_" # name ": not implemented");        \
        assert(! "marshall_read_" # name ": not implemented");        \
        return NULL;                                                  \
    }

DEF_MARSHALL_READ(array, s_array)
DEF_MARSHALL_READ(call, s_call)
DEF_MARSHALL_READ(callable, s_callable)
DEF_MARSHALL_READ(cow, s_cow)
DEF_MARSHALL_READ(complex, s_complex)
DEF_MARSHALL_READ(do_block, s_do_block)
DEF_MARSHALL_READ(fact, s_fact)
DEF_MARSHALL_READ(ident, s_ident)
DEF_MARSHALL_READ(integer, s_integer)
DEF_MARSHALL_READ(map, s_map)
DEF_MARSHALL_READ(quote, s_quote)
DEF_MARSHALL_READ(ratio, s_ratio)
DEF_MARSHALL_READ(struct, s_struct)
DEF_MARSHALL_READ(struct_type, s_struct_type)
DEF_MARSHALL_READ(sym, s_sym)
DEF_MARSHALL_READ(tuple, s_tuple)
DEF_MARSHALL_READ(time, s_time)
DEF_MARSHALL_READ(unquote, s_unquote)
DEF_MARSHALL_READ(var, s_var)

    p_cow
DEF_MARSHALL_READ(pcomplex, p_complex *)
DEF_MARSHALL_READ(pcow, p_cow)
DEF_MARSHALL_READ(plist, s_list **)
DEF_MARSHALL_READ(plist, s_list **)
