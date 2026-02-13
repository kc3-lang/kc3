/* kc3
 * Copyright from 2022 to 2026 kmx.io <contact@kmx.io>
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
#include <string.h>
#include <strings.h>
#include "alloc.h"
#include "array.h"
#include "assert.h"
#include "bool.h"
#include "buf.h"
#include "buf_inspect.h"
#include "buf_parse.h"
#include "callable.h"
#include "cfn.h"
#include "compare.h"
#include "cow.h"
#include "do_block.h"
#include "env.h"
#include "env_eval.h"
#include "env_eval_equal.h"
#include "explicit_bzero.h"
#include "fn.h"
#include "frame.h"
#include "hash.h"
#include "ident.h"
#include "integer.h"
#include "list.h"
#include "map.h"
#include "pcall.h"
#include "pcallable.h"
#include "pcomplex.h"
#include "pcow.h"
#include "pfacts.h"
#include "plist.h"
#include "pointer.h"
#include "pstruct.h"
#include "pstruct_type.h"
#include "ptr.h"
#include "ptr_free.h"
#include "pvar.h"
#include "quote.h"
#include "ratio.h"
#include "str.h"
#include "struct.h"
#include "struct_type.h"
#include "sym.h"
#include "tag.h"
#include "tag_type.h"
#include "time.h"
#include "tuple.h"
#include "unquote.h"
#include "var.h"

s_tag g_tag_first;
s_tag g_tag_last;

s_tag * tag_1 (s_tag *tag, const char *p)
{
  tag_clean(tag);
  return tag_init_1(tag, p);
}

u8 * tag_alignment (const s_tag *tag, u8 *dest)
{
  const s_sym *type;
  if (! tag_type_var(tag, &type))
    return NULL;
  if (sym_is_array_type(type)) {
    // TODO: 32 bit
    if (tag->data.array.size >= 16) {
      *dest = 16;
      return dest;
    }
    type = tag->data.array.element_type;
  }
  return sym_type_alignment(type, dest);
}

s_tag * tag_and (s_tag *a, s_tag *b, s_tag *dest)
{
  bool p;
  const s_sym *sym_Bool = &g_sym_Bool;
  assert(a);
  assert(b);
  assert(dest);
  if (! bool_init_cast(&p, &sym_Bool, a)) {
    err_write_1("tag_and: cannot cast to Bool: ");
    err_inspect_tag(a);
    err_write_1("\n");
    assert(! "tag_and: cannot cast to Bool");
    return NULL;
  }
  if (! p)
    return tag_init_bool(dest, false);
  if (! bool_init_cast(&p, &sym_Bool, b)) {
    err_write_1("tag_and: cannot cast to Bool: ");
    err_inspect_tag(b);
    err_write_1("\n");
    assert(! "tag_and: cannot cast to Bool");
    return NULL;
  }
  if (! p)
    return tag_init_bool(dest, false);
  return tag_init_copy(dest, b);
}

s8 tag_arity (const s_tag *tag)
{
  if (tag->type == TAG_PCALLABLE &&
      tag->data.pcallable) {
    switch (tag->data.pcallable->type) {
    case CALLABLE_CFN:
      return cfn_arity(&tag->data.pcallable->data.cfn);
    case CALLABLE_FN:
      return fn_arity(&tag->data.pcallable->data.fn);
    case CALLABLE_VOID:
      break;
    }
  }
  return -1;
}

s_tag * tag_assign (s_tag *tag, s_tag *value, s_tag *dest)
{
  assert(tag);
  assert(value);
  assert(dest);
  switch (tag->type) {
  case TAG_PCOW:
    return pcow_assign(&tag->data.pcow, value, dest);
  case TAG_PVAR:
    return var_assign(tag->data.pvar, value, dest);
  default:
    break;
  }
  err_write_1("tag_assign: cannot assign to ");
  err_write_1(tag_type_to_string(tag->type));
  err_write_1("\n");
  assert(! "tag_assign: cannot assign to this tag type");
  return NULL;
}

s_tag * tag_bool (s_tag *tag, bool b)
{
  tag_clean(tag);
  return tag_init_bool(tag, b);
}

s_tag * tag_brackets (s_tag *tag, s_tag *address, s_tag *dest)
{
  assert(tag);
  assert(address);
  assert(dest);
  switch (tag->type) {
  case TAG_ARRAY:
    switch (address->type) {
    case TAG_ARRAY:
      return array_data_tag(&tag->data.array, &address->data.array,
                            dest);
    default:
      break;
    }
  default:
    break;
  }
  err_puts("tag_brackets: invalid arguments");
  assert(! "tag_brackets: invalid arguments");
  return NULL;
}

s_tag * tag_cast_integer_to_s8 (s_tag *tag)
{
  s8 i;
  assert(tag);
  assert(tag->type == TAG_INTEGER);
  i = integer_to_s8(&tag->data.integer);
  return tag_s8(tag, i);
}

s_tag * tag_cast_integer_to_s16 (s_tag *tag)
{
  s16 i;
  assert(tag);
  assert(tag->type == TAG_INTEGER);
  i = integer_to_s16(&tag->data.integer);
  return tag_s16(tag, i);
}

s_tag * tag_cast_integer_to_s32 (s_tag *tag)
{
  s32 i;
  assert(tag);
  assert(tag->type == TAG_INTEGER);
  i = integer_to_s32(&tag->data.integer);
  return tag_s32(tag, i);
}

s_tag * tag_cast_integer_to_s64 (s_tag *tag)
{
  s64 i;
  assert(tag);
  assert(tag->type == TAG_INTEGER);
  i = integer_to_s64(&tag->data.integer);
  return tag_s64(tag, i);
}

s_tag * tag_cast_integer_to_u8 (s_tag *tag)
{
  u8 i;
  assert(tag);
  assert(tag->type == TAG_INTEGER);
  i = integer_to_u8(&tag->data.integer);
  return tag_u8(tag, i);
}

s_tag * tag_cast_integer_to_u16 (s_tag *tag)
{
  u16 i;
  assert(tag);
  assert(tag->type == TAG_INTEGER);
  i = integer_to_u16(&tag->data.integer);
  return tag_u16(tag, i);
}

s_tag * tag_cast_integer_to_u32 (s_tag *tag)
{
  u32 i;
  assert(tag);
  assert(tag->type == TAG_INTEGER);
  i = integer_to_u32(&tag->data.integer);
  return tag_u32(tag, i);
}

s_tag * tag_cast_integer_to_u64 (s_tag *tag)
{
  u64 i;
  assert(tag);
  assert(tag->type == TAG_INTEGER);
  i = integer_to_u64(&tag->data.integer);
  return tag_u64(tag, i);
}

void tag_clean (s_tag *tag)
{
  assert(tag);
  switch (tag->type) {
  case TAG_ARRAY:       array_clean(&tag->data.array);         break;
  case TAG_DO_BLOCK:    do_block_clean(&tag->data.do_block);   break;
  case TAG_INTEGER:     integer_clean(&tag->data.integer);     break;
  case TAG_MAP:         map_clean(&tag->data.map);             break;
  case TAG_PCALL:       pcall_clean(&tag->data.pcall);         break;
  case TAG_PCALLABLE:   pcallable_clean(&tag->data.pcallable); break;
  case TAG_PCOMPLEX:    pcomplex_clean(&tag->data.pcomplex);   break;
  case TAG_PCOW:        pcow_clean(&tag->data.pcow);           break;
  case TAG_PFACTS:      pfacts_clean(&tag->data.pfacts);       break;
  case TAG_PLIST:       list_delete_all(tag->data.plist);      break;
  case TAG_PSTRUCT:     pstruct_clean(&tag->data.pstruct);     break;
  case TAG_PSTRUCT_TYPE:
    pstruct_type_clean(&tag->data.pstruct_type);               break;
  case TAG_PTR_FREE:    ptr_free_clean(&tag->data.ptr);        break;
  case TAG_PVAR:        pvar_clean(&tag->data.pvar);           break;
  case TAG_QUOTE:       quote_clean(&tag->data.quote);         break;
  case TAG_RATIO:       ratio_clean(&tag->data.ratio);         break;
  case TAG_STR:         str_clean(&tag->data.str);             break;
  case TAG_TIME:        time_clean(&tag->data.time);           break;
  case TAG_TUPLE:       tuple_clean(&tag->data.tuple);         break;
  case TAG_UNQUOTE:     unquote_clean(&tag->data.unquote);     break;
  case TAG_BOOL:
  case TAG_CHARACTER:
  case TAG_F32:
  case TAG_F64:
#if HAVE_F80
  case TAG_F80:
#endif
#if HAVE_F128
  case TAG_F128:
#endif
  case TAG_FACT:
  case TAG_IDENT:
  case TAG_POINTER:
  case TAG_PSYM:
  case TAG_PTAG:
  case TAG_PTR:
  case TAG_S8:
  case TAG_S16:
  case TAG_S32:
  case TAG_S64:
  case TAG_SW:
  case TAG_U8:
  case TAG_U16:
  case TAG_U32:
  case TAG_U64:
  case TAG_UW:
  case TAG_VOID:
    break;
  }
}

s_tag * tag_copy (s_tag *tag, s_tag *src)
{
  tag_clean(tag);
  return tag_init_copy(tag, src);
}

void tag_delete (s_tag *tag)
{
  tag_clean(tag);
  free(tag);
}

bool * tag_eq (s_tag *a, s_tag *b, bool *dest)
{
  assert(a);
  assert(b);
  assert(dest);
  *dest = compare_tag(a, b) == 0 ? 1 : 0;
  return dest;
}

s_tag * tag_equal (s_tag *a, s_tag *b, s_tag *dest)
{
  assert(a);
  assert(b);
  assert(dest);
  if (! env_eval_equal_tag (env_global(), false, a, b, dest))
    return NULL;
  return dest;
}

#if HAVE_F80

s_tag * tag_f80 (s_tag *tag, f80 f)
{
  tag_clean(tag);
  return tag_init_f80(tag, f);
}

#endif

#if HAVE_F128

s_tag * tag_f128 (s_tag *tag, f128 f)
{
  tag_clean(tag);
  return tag_init_f128(tag, f);
}

#endif

bool * tag_gt (s_tag *a, s_tag *b, bool *dest)
{
  assert(a);
  assert(b);
  assert(dest);
  *dest = compare_tag(a, b) > 0 ? 1 : 0;
  return dest;
}

bool * tag_gte (s_tag *a, s_tag *b, bool *dest)
{
  assert(a);
  assert(b);
  assert(dest);
  *dest = compare_tag(a, b) >= 0 ? 1 : 0;
  return dest;
}

u64 * tag_hash_u64 (const s_tag *tag, u64 *dest)
{
  t_hash hash;
  assert(tag);
  hash_init(&hash);
  if (! hash_update_tag(&hash, tag))
    return NULL;
  *dest = hash_to_u64(&hash);
  return dest;
}

uw * tag_hash_uw (const s_tag *tag, uw *dest)
{
  t_hash hash;
  assert(tag);
  hash_init(&hash);
  if (! hash_update_tag(&hash, tag)) {
    err_write_1("tag_hash_uw: hash_update_tag: ");
    err_inspect_tag(tag);
    err_write_1("\n");
    assert(! "tag_hash_uw: hash_update_tag");
    return NULL;
  }
  *dest = hash_to_uw(&hash);
  return dest;
}

bool tag_ident_is_bound (const s_tag *tag)
{
  return env_tag_ident_is_bound(env_global(), tag);
}

s_tag * tag_init (s_tag *tag)
{
  *tag = (s_tag) {0};
  return tag;
}

s_tag * tag_init_1 (s_tag *tag, const char *p)
{
  s_buf buf;
  uw len;
  sw r;
  assert(tag);
  tag_init_void(tag);
  if (! p)
    return tag;
  len = strlen(p);
  buf_init_const(&buf, len, p);
  buf.wpos = len;
  r = buf_parse_tag(&buf, tag);
  buf_clean(&buf);
  if (r < 0 || (uw) r != len) {
    err_write_1("tag_init_1: invalid tag: \"");
    err_write_1(p);
    err_write_1("\", expected ");
    err_inspect_uw_decimal(len);
    err_write_1(", got ");
    err_inspect_sw_decimal(r);
    err_write_1("\n");
    assert(! "tag_init_1: invalid tag");
    return NULL;
  }
  return tag;
}

s_tag * tag_init_bool (s_tag *tag, bool b)
{
  s_tag tmp = {0};
  tmp.type = TAG_BOOL;
  tmp.data.bool_ = b;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_sym_type (s_tag *tag, const s_sym *type)
{
  s_tag tmp = {0};
  assert(tag);
  assert(type);
  if (! sym_to_tag_type(type, &tmp.type)) {
    err_write_1("tag_init_sym_type: invalid type: ");
    err_puts(type->str.ptr.pchar);
    assert(! "tag_init_sym_type: invalid type");
    return NULL;
  }
  switch (tmp.type) {
  case TAG_POINTER:
    if (type == &g_sym_Pointer) {
      if (! pointer_init(&tmp.data.pointer, NULL, NULL, NULL))
        return NULL;
    }
    else
      if (! pointer_init(&tmp.data.pointer, type, NULL, NULL))
        return NULL;
    break;
  case TAG_PSTRUCT:
    if (! pstruct_init(&tmp.data.pstruct, type)) {
      err_write_1("tag_init_sym_type: struct_init: ");
      err_puts(type->str.ptr.pchar);
      assert(! "tag_init_sym_type: struct_init");
      return NULL;
    }
    if (! struct_allocate(tmp.data.pstruct)) {
      err_write_1("tag_init_sym_type: struct_allocate: ");
      err_puts(type->str.ptr.pchar);
      assert(! "tag_init_sym_type: struct_allocate");
      pstruct_clean(&tmp.data.pstruct);
      return NULL;
    }
    break;
  default:
    break;
  }
  *tag = tmp;
  return tag;
}

s_tag * tag_init_cast (s_tag *tag, const s_sym * const *type,
                       s_tag *src)
{
  assert(tag);
  assert(type);
  assert(*type);
  assert(src);
  switch (src->type) {
  case TAG_POINTER:
    if (*type != &g_sym_Tag ||
        src->data.pointer.target_type != &g_sym_Tag)
      break;
    return tag_init_copy(tag, src->data.pointer.ptr.p);
  case TAG_PTR:
    if (*type != &g_sym_Tag)
      break;
    return tag_init_copy(tag, src->data.ptr.p);
  default:
    break;
  }
  err_puts("tag_init_cast: invalid cast");
  assert(! "tag_init_cast: invalid cast");
  return NULL;
}

s_tag * tag_init_cast_struct (s_tag *tag, const s_sym * const *type,
                              s_tag *src)
{
  assert(tag);
  assert(type);
  assert(*type);
  assert(src);
  switch (src->type) {
  case TAG_PTR:
    if (! src->data.ptr.p)
      return tag_init_void(tag);
    return tag_init_pstruct_with_data(tag, *type, src->data.ptr.p,
                                      false);
  case TAG_PTR_FREE:
    if (! src->data.ptr_free.p)
      return tag_init_void(tag);
    return tag_init_pstruct_with_data(tag, *type, src->data.ptr_free.p,
                                      false);
  case TAG_PSTRUCT:
    if (*type == src->data.pstruct->pstruct_type->module)
      return tag_init_pstruct_copy(tag, &src->data.pstruct);
  default:
    break;
  }
  err_puts("tag_init_cast_struct: invalid cast");
  assert(! "tag_init_cast_struct: invalid cast");
  return NULL;
}

s_tag * tag_init_copy (s_tag *tag, s_tag *src)
{
  assert(tag);
  assert(src);
  if (src->type == TAG_PVAR &&
      src->data.pvar->bound)
    src = &src->data.pvar->tag;
  switch (src->type) {
  case TAG_ARRAY:
    tag->type = src->type;
    if (! array_init_copy(&tag->data.array, &src->data.array))
      return NULL;
    return tag;
  case TAG_BOOL:
    tag->type = src->type;
    tag->data.bool_ = src->data.bool_;
    return tag;
  case TAG_CHARACTER:
    tag->type = src->type;
    tag->data.character = src->data.character;
    return tag;
  case TAG_DO_BLOCK:
    tag->type = src->type;
    if (! do_block_init_copy(&tag->data.do_block, &src->data.do_block))
      return NULL;
    return tag;
  case TAG_F32:
    tag->type = src->type;
    tag->data.f32 = src->data.f32;
    return tag;
  case TAG_F64:
    tag->type = src->type;
    tag->data.f64 = src->data.f64;
    return tag;
#if HAVE_F80
  case TAG_F80:
    tag->type = src->type;
    tag->data.f80 = src->data.f80;
    return tag;
#endif
#if HAVE_F128
  case TAG_F128:
    tag->type = src->type;
    tag->data.f128 = src->data.f128;
    return tag;
#endif
  case TAG_FACT:
    tag->type = src->type;
    tag->data.fact = src->data.fact;
    return tag;
  case TAG_IDENT:
    tag->type = src->type;
    tag->data.ident = src->data.ident;
    return tag;
  case TAG_INTEGER:
    tag->type = src->type;
    if (! integer_init_copy(&tag->data.integer, &src->data.integer))
      return NULL;
    return tag;
  case TAG_MAP:
    tag->type = src->type;
    if (! map_init_copy(&tag->data.map, &src->data.map))
      return NULL;
    return tag;
  case TAG_PCALL:
    tag->type = src->type;
    if (! pcall_init_copy(&tag->data.pcall, &src->data.pcall))
      return NULL;
    return tag;
  case TAG_PCALLABLE:
    tag->type = src->type;
    if (! pcallable_init_copy(&tag->data.pcallable,
                              &src->data.pcallable))
      return NULL;
    return tag;
  case TAG_PCOMPLEX:
    tag->type = src->type;
    if (! pcomplex_init_copy(&tag->data.pcomplex,
                             &src->data.pcomplex))
      return NULL;
    return tag;
  case TAG_PCOW:
    tag->type = src->type;
    if (! pcow_init_copy(&tag->data.pcow, &src->data.pcow))
      return NULL;
    return tag;
  case TAG_PFACTS:
    tag->type = src->type;
    if (! pfacts_init_copy(&tag->data.pfacts, &src->data.pfacts))
      return NULL;
    return tag;
  case TAG_PLIST:
    tag->type = src->type;
    if (! plist_init_copy(&tag->data.plist, &src->data.plist))
      return NULL;
    return tag;
  case TAG_POINTER:
    tag->type = src->type;
    if (! pointer_init_copy(&tag->data.pointer, &src->data.pointer))
      return NULL;
    return tag;
  case TAG_PSTRUCT:
    tag->type = src->type;
    if (! pstruct_init_copy(&tag->data.pstruct, &src->data.pstruct))
      return NULL;
    return tag;
  case TAG_PSTRUCT_TYPE:
    tag->type = src->type;
    if (! pstruct_type_init_copy(&tag->data.pstruct_type,
                                 &src->data.pstruct_type))
      return NULL;
    return tag;
  case TAG_PSYM:
    tag->type = src->type;
    tag->data.psym = src->data.psym;
    return tag;
  case TAG_PTAG:
    tag->type = src->type;
    tag->data.ptag = src->data.ptag;
    return tag;
  case TAG_PTR:
    tag->type = src->type;
    tag->data.ptr = src->data.ptr;
    return tag;
  case TAG_PTR_FREE:
    tag->type = src->type;
    tag->data.ptr_free = src->data.ptr_free;
    return tag;
  case TAG_PVAR:
    tag->type = src->type;
    pvar_init_copy(&tag->data.pvar, &src->data.pvar);
    return tag;
  case TAG_QUOTE:
    tag->type = src->type;
    if (! quote_init_copy(&tag->data.quote, &src->data.quote))
      return NULL;
    return tag;
  case TAG_RATIO:
    tag->type = src->type;
    if (! ratio_init_copy(&tag->data.ratio, &src->data.ratio))
      return NULL;
    return tag;
  case TAG_S8:
    tag->type = src->type;
    tag->data.s8 = src->data.s8;
    return tag;
  case TAG_S16:
    tag->type = src->type;
    tag->data.s16 = src->data.s16;
    return tag;
  case TAG_S32:
    tag->type = src->type;
    tag->data.s32 = src->data.s32;
    return tag;
  case TAG_S64:
    tag->type = src->type;
    tag->data.s64 = src->data.s64;
    return tag;
  case TAG_STR:
    tag->type = src->type;
    if (! str_init_copy(&tag->data.str, &src->data.str))
      return NULL;
    return tag;
  case TAG_SW:
    tag->type = src->type;
    tag->data.sw = src->data.sw;
    return tag;
  case TAG_TIME:
    tag->type = src->type;
    if (! time_init_copy(&tag->data.time, &src->data.time))
      return NULL;
    return tag;
  case TAG_TUPLE:
    tag->type = src->type;
    if (! tuple_init_copy(&tag->data.tuple, &src->data.tuple))
      return NULL;
    return tag;
  case TAG_U8:
    tag->type = src->type;
    tag->data.u8 = src->data.u8;
    return tag;
  case TAG_U16:
    tag->type = src->type;
    tag->data.u16 = src->data.u16;
    return tag;
  case TAG_U32:
    tag->type = src->type;
    tag->data.u32 = src->data.u32;
    return tag;
  case TAG_U64:
    tag->type = src->type;
    tag->data.u64 = src->data.u64;
    return tag;
  case TAG_UNQUOTE:
    tag->type = src->type;
    if (! unquote_init_copy(&tag->data.unquote, &src->data.unquote))
      return NULL;
    return tag;
  case TAG_UW:
    tag->type = src->type;
    tag->data.uw = src->data.uw;
    return tag;
  case TAG_VOID:
    return tag_init_void(tag);
  }
  err_puts("tag_init_copy: invalid tag type");
  assert(! "tag_init_copy: invalid tag type");
  return NULL;
}

#if HAVE_F80

s_tag * tag_init_f80 (s_tag *tag, f80 f)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_F80;
  tmp.data.f80 = f;
  *tag = tmp;
  return tag;
}

#endif

#if HAVE_F128

s_tag * tag_init_f128 (s_tag *tag, f128 f)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_F128;
  tmp.data.f128 = f;
  *tag = tmp;
  return tag;
}

#endif

s_tag * tag_init_from_str (s_tag *tag, const s_str *str)
{
  s_buf buf;
  sw r;
  assert(tag);
  tag_init_void(tag);
  if (! str->size)
    return tag;
  buf_init_const(&buf, str->size, str->ptr.pchar);
  buf.wpos = str->size;
  r = buf_parse_tag(&buf, tag);
  buf_clean(&buf);
  if (r < 0) {
    err_write_1("tag_init_str: invalid tag: ");
    err_inspect_str(str);
    err_write_1(", ");
    err_inspect_u32_decimal(str->size);
    err_write_1(" != ");
    err_inspect_sw(r);
    assert(! "tag_init_str: invalid tag");
    return NULL;
  }
  return tag;
}

s_tag * tag_init_uw_reduce (s_tag *tag, uw src)
{
  if (src >= U32_MAX)
    return tag_init_uw(tag, src);
  if (src >= U16_MAX)
    return tag_init_u32(tag, src);
  if (src >= U8_MAX)
    return tag_init_u16(tag, src);
  return tag_init_u8(tag, src);
}

s_tag * tag_init_void (s_tag *tag)
{
  assert(tag);
  explicit_bzero(tag, sizeof(s_tag));
  return tag;
}

s_tag * tag_integer_reduce (s_tag *tag, s_tag *dest)
{
  s_integer *i;
  s_integer j;
  assert(tag);
  switch (tag->type) {
  case TAG_INTEGER:
    i = &tag->data.integer;
    if (compare_integer(i, &g_integer_u64_max) > 0)
      return tag_init_copy(dest, tag);
    if (compare_integer(i, &g_integer_u32_max) > 0)
      return tag_init_u64(dest, integer_to_u64(i));
    if (compare_integer(i, &g_integer_u16_max) > 0)
      return tag_init_u32(dest, integer_to_u32(i));
    if (compare_integer(i, &g_integer_u8_max) > 0)
      return tag_init_u16(dest, integer_to_u16(i));
    if (compare_integer(i, &g_integer_zero) >= 0)
      return tag_init_u8(dest, integer_to_u8(i));
    if (compare_integer(i, &g_integer_s8_min) >= 0)
      return tag_init_s8(dest, integer_to_s8(i));
    if (compare_integer(i, &g_integer_s16_min) >= 0)
      return tag_init_s16(dest, integer_to_s16(i));
    if (compare_integer(i, &g_integer_s32_min) >= 0)
      return tag_init_s32(dest, integer_to_s32(i));
    if (compare_integer(i, &g_integer_s64_min) >= 0)
      return tag_init_s64(dest, integer_to_s64(i));
    return tag_init_copy(dest, tag);
  case TAG_S8:
    integer_init_s8(&j, tag->data.s8);
    integer_reduce(&j, dest);
    integer_clean(&j);
    return dest;
  case TAG_S16:
    integer_init_s16(&j, tag->data.s16);
    integer_reduce(&j, dest);
    integer_clean(&j);
    return dest;
  case TAG_S32:
    integer_init_s32(&j, tag->data.s32);
    integer_reduce(&j, dest);
    integer_clean(&j);
    return dest;
  case TAG_S64:
    integer_init_s64(&j, tag->data.s64);
    integer_reduce(&j, dest);
    integer_clean(&j);
    return dest;
  case TAG_SW:
    integer_init_sw(&j, tag->data.sw);
    integer_reduce(&j, dest);
    integer_clean(&j);
    return dest;
  case TAG_U8:
    return tag_init_copy(dest, tag);
  case TAG_U16:
    integer_init_u16(&j, tag->data.u16);
    integer_reduce(&j, dest);
    integer_clean(&j);
    return dest;
  case TAG_U32:
    integer_init_u32(&j, tag->data.u32);
    integer_reduce(&j, dest);
    integer_clean(&j);
    return dest;
  case TAG_U64:
    integer_init_u64(&j, tag->data.u64);
    integer_reduce(&j, dest);
    integer_clean(&j);
    return dest;
  case TAG_UW:
    integer_init_uw(&j, tag->data.uw);
    integer_reduce(&j, dest);
    integer_clean(&j);
    return dest;
  default:
    break;
  }
  err_puts("tag_integer_reduce: not an integer");
  assert(! "tag_integer_reduce: not an integer");
  return NULL;
}

bool tag_is_alist (const s_tag *tag)
{
  assert(tag);
  if (! tag)
    return false;
  if (tag->type != TAG_PLIST)
    return false;
  return list_is_alist(tag->data.plist);
}

bool tag_is_cast (const s_tag *tag, const s_sym *type)
{
  assert(tag);
  return (tag &&
          type &&
          tag->type == TAG_PCALL &&
          tag->data.pcall->ident.module == type &&
          tag->data.pcall->ident.sym == &g_sym_cast);
}

bool tag_is_integer (s_tag *tag)
{
  assert(tag);
  tag = tag_resolve_cow(tag);
  switch (tag->type) {
  case TAG_VOID:
  case TAG_ARRAY:
  case TAG_BOOL:
  case TAG_CHARACTER:
  case TAG_DO_BLOCK:
  case TAG_F32:
  case TAG_F64:
#if HAVE_F80
  case TAG_F80:
#endif
#if HAVE_F128
  case TAG_F128:
#endif
  case TAG_FACT:
  case TAG_MAP:
  case TAG_PCALL:
  case TAG_PCALLABLE:
  case TAG_PCOMPLEX:
  case TAG_PFACTS:
  case TAG_PLIST:
  case TAG_POINTER:
  case TAG_PSTRUCT:
  case TAG_PSTRUCT_TYPE:
  case TAG_PSYM:
  case TAG_PTAG:
  case TAG_PTR:
  case TAG_PTR_FREE:
  case TAG_PVAR:
  case TAG_QUOTE:
  case TAG_RATIO:
  case TAG_STR:
  case TAG_TIME:
  case TAG_TUPLE:
  case TAG_UNQUOTE:
  case TAG_IDENT:
    return false;
  case TAG_INTEGER:
  case TAG_S8:
  case TAG_S16:
  case TAG_S32:
  case TAG_S64:
  case TAG_SW:
  case TAG_U8:
  case TAG_U16:
  case TAG_U32:
  case TAG_U64:
  case TAG_UW:
    return true;
  case TAG_PCOW:
    break;
  }
  err_puts("tag_is_integer: invalid tag type");
  assert(! "tag_is_integer: invalid tag type");
  abort();
  return false;
}

bool tag_is_number (s_tag *tag)
{
  assert(tag);
  tag = tag_resolve_cow(tag);
  switch (tag->type) {
  case TAG_VOID:
  case TAG_ARRAY:
  case TAG_BOOL:
  case TAG_CHARACTER:
  case TAG_DO_BLOCK:
  case TAG_FACT:
  case TAG_MAP:
  case TAG_PCALL:
  case TAG_PCALLABLE:
  case TAG_PFACTS:
  case TAG_PLIST:
  case TAG_POINTER:
  case TAG_PSTRUCT:
  case TAG_PSTRUCT_TYPE:
  case TAG_PSYM:
  case TAG_PTAG:
  case TAG_PTR:
  case TAG_PTR_FREE:
  case TAG_PVAR:
  case TAG_QUOTE:
  case TAG_STR:
  case TAG_TIME:
  case TAG_TUPLE:
  case TAG_UNQUOTE:
  case TAG_IDENT:
    return false;
  case TAG_PCOMPLEX:
  case TAG_F32:
  case TAG_F64:
#if HAVE_F80
  case TAG_F80:
#endif
#if HAVE_F128
  case TAG_F128:
#endif
  case TAG_INTEGER:
  case TAG_RATIO:
  case TAG_SW:
  case TAG_S64:
  case TAG_S32:
  case TAG_S16:
  case TAG_S8:
  case TAG_U8:
  case TAG_U16:
  case TAG_U32:
  case TAG_U64:
  case TAG_UW:
    return true;
  case TAG_PCOW:
    break;
  }
  err_puts("tag_is_number: invalid tag type");
  assert(! "tag_is_number: invalid tag type");
  abort();
  return false;
}

bool tag_is_positive_integer (s_tag *tag)
{
  assert(tag);
  tag = tag_resolve_cow(tag);
  switch (tag->type) {
  case TAG_VOID:
  case TAG_ARRAY:
  case TAG_BOOL:
  case TAG_CHARACTER:
  case TAG_DO_BLOCK:
  case TAG_F32:
  case TAG_F64:
#if HAVE_F80
  case TAG_F80:
#endif
#if HAVE_F128
  case TAG_F128:
#endif
  case TAG_FACT:
  case TAG_MAP:
  case TAG_PCALL:
  case TAG_PCALLABLE:
  case TAG_PCOMPLEX:
  case TAG_PFACTS:
  case TAG_PLIST:
  case TAG_POINTER:
  case TAG_PSTRUCT:
  case TAG_PSTRUCT_TYPE:
  case TAG_PSYM:
  case TAG_PTAG:
  case TAG_PTR:
  case TAG_PTR_FREE:
  case TAG_PVAR:
  case TAG_QUOTE:
  case TAG_RATIO:
  case TAG_STR:
  case TAG_TIME:
  case TAG_TUPLE:
  case TAG_UNQUOTE:
  case TAG_IDENT:
    return false;
  case TAG_INTEGER: return integer_is_positive(&tag->data.integer);
  case TAG_S8:      return tag->data.s8 > 0;
  case TAG_S16:     return tag->data.s16 > 0;
  case TAG_S32:     return tag->data.s32 > 0;
  case TAG_S64:     return tag->data.s64 > 0;
  case TAG_SW:      return tag->data.sw > 0;
  case TAG_U8:      return tag->data.u8 > 0;
  case TAG_U16:     return tag->data.u16 > 0;
  case TAG_U32:     return tag->data.u32 > 0;
  case TAG_U64:     return tag->data.u64 > 0;
  case TAG_UW:      return tag->data.uw > 0;
    return true;
  case TAG_PCOW:
    break;
  }
  err_puts("tag_is_positive_integer: invalid tag type");
  assert(! "tag_is_positive_integer: invalid tag type");
  abort();
  return false;
}

bool tag_is_struct (const s_tag *tag, const s_sym *module)
{
  return tag &&
    tag->type == TAG_PSTRUCT &&
    tag->data.pstruct->pstruct_type->module == module;
}

bool * tag_is_unbound_var (const s_tag *tag, bool *dest)
{
  assert(tag);
  if (! tag) {
    err_puts("tag_is_unbound_var: NULL tag");
    assert(! "tag_is_unbound_var: NULL tag");
    return NULL;
  }
  *dest = (tag->type == TAG_PVAR &&
           ! tag->data.pvar->bound);
  return dest;
}

bool tag_is_zero (const s_tag *tag)
{
  assert(tag);
  switch (tag->type) {
    case TAG_INTEGER:
      return integer_is_zero(&tag->data.integer);
    case TAG_F32:
      return tag->data.f32 == 0.0f;
    case TAG_F64:
      return tag->data.f64 == 0.0;
    case TAG_S8:
      return tag->data.s8 == 0;
    case TAG_S16:
      return tag->data.s16 == 0;
    case TAG_S32:
      return tag->data.s32 == 0;
    case TAG_S64:
      return tag->data.s64 == 0;
    case TAG_U8:
      return tag->data.u8 == 0;
    case TAG_U16:
      return tag->data.u16 == 0;
    case TAG_U32:
      return tag->data.u32 == 0;
    case TAG_U64:
      return tag->data.u64 == 0;
    case TAG_UW:
      return tag->data.uw == 0;
    default:
      return false;
  }
}

/*
s_tag * tag_list (s_tag *tag, s_list *x)
{
  assert(tag);
  tag_clean(tag);
  return tag_init_list(tag, x);
}
*/

s_tag * tag_list_1 (s_tag *tag, const char *p)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_PLIST;
  tmp.data.plist = list_new_1(p);
  *tag = tmp;
  return tag;
}

bool * tag_lt (s_tag *a, s_tag *b, bool *dest)
{
  assert(a);
  assert(b);
  assert(dest);
  *dest = compare_tag(a, b) < 0 ? 1 : 0;
  return dest;
}

bool * tag_lte (s_tag *a, s_tag *b, bool *dest)
{
  assert(a);
  assert(b);
  assert(dest);
  *dest = compare_tag(a, b) <= 0 ? 1 : 0;
  return dest;
}

s_tag * tag_new (void)
{
  s_tag *dest;
  dest = alloc(sizeof(s_tag));
  if (! dest)
    return NULL;
  return dest;
}

s_tag * tag_new_1 (const char *p)
{
  s_tag *dest;
  dest = alloc(sizeof(s_tag));
  if (! dest)
    return NULL;
  if (! tag_init_1(dest, p)) {
    free(dest);
    return NULL;
  }
  return dest;
}

s_tag * tag_new_copy (s_tag *src)
{
  s_tag *dest;
  dest = alloc(sizeof(s_tag));
  if (! dest)
    return NULL;
  if (! tag_init_copy(dest, src)) {
    free(dest);
    return NULL;
  }
  return dest;
}

bool * tag_not (s_tag *tag, bool *dest)
{
  bool b;
  const s_sym *type;
  assert(tag);
  assert(dest);
  type = &g_sym_Bool;
  if (! bool_init_cast(&b, &type, tag)) {
    err_write_1("tag_not: bool_init_cast: ");
    err_inspect_tag(tag);
    err_write_1("\n");
    assert(! "tag_not: bool_init_cast");
    return NULL;
  }
  *dest = ! b;
  return dest;
}

bool * tag_not_eq (s_tag *a, s_tag *b, bool *dest)
{
  assert(a);
  assert(b);
  assert(dest);
  *dest = compare_tag(a, b) != 0 ? 1 : 0;
  return dest;
}

s_tag * tag_or (s_tag *a, s_tag *b, s_tag *dest)
{
  bool p;
  const s_sym *sym_Bool = &g_sym_Bool;
  assert(a);
  assert(b);
  assert(dest);
  if (! bool_init_cast(&p, &sym_Bool, a)) {
    err_write_1("tag_or: cannot cast to Bool: ");
    err_inspect_tag(a);
    err_write_1("\n");
    assert(! "tag_or: cannot cast to Bool");
    return NULL;
  }
  if (p)
    return tag_init_copy(dest, a);
  if (! bool_init_cast(&p, &sym_Bool, b)) {
    err_write_1("tag_or: cannot cast to Bool: ");
    err_inspect_tag(b);
    err_write_1("\n");
    assert(! "tag_or: cannot cast to Bool");
    return NULL;
  }
  return tag_init_copy(dest, b);
}

s_tag * tag_paren (s_tag *tag, s_tag *dest)
{
  assert(tag);
  assert(dest);
  return tag_init_copy(dest, tag);
}

s_tag * tag_resolve_cow (s_tag *tag)
{
  while (tag->type == TAG_PCOW)
    tag = cow_read_only(tag->data.pcow);
  if (! tag) {
    err_puts("tag_resolve_cow: cow was not frozen");
    assert(! "tag_resolve_cow: cow was not frozen");
    return NULL;
  }
  return tag;
}

s_tag * tag_semicolon (const s_tag *a, s_tag *b, s_tag *dest)
{
  (void) a;
  return tag_init_copy(dest, b);
}

s_tag * tag_set_name_if_null (s_tag *tag, const s_ident *name)
{
  assert(tag);
  assert(name);
  switch (tag->type) {
  case TAG_PCALLABLE:
    if (! callable_set_name_if_null(tag->data.pcallable, name))
      return NULL;
    return tag;
  case TAG_PVAR:
    if (! var_set_name_if_null(tag->data.pvar, name))
      return NULL;
    return tag;
  default:
    break;
  }
  return tag;
}

uw * tag_size (const s_tag *tag, uw *dest)
{
  const s_sym *type;
  uw tmp = 0;
  assert(tag);
  if (! tag_type_var(tag, &type) ||
      ! sym_type_size(type, &tmp))
    return NULL;
  *dest = tmp;
  return dest;
}

bool tag_to_ffi_pointer (s_tag *tag, const s_sym *type, void **dest)
{
  assert(tag);
  assert(type);
  assert(dest);
  if (tag->type == TAG_PVAR) {
    if (! tag->data.pvar) {
      err_puts("tag_to_ffi_pointer: NULL pvar");
      assert(! "tag_to_ffi_pointer: NULL pvar");
      return false;
    }
    if (tag->data.pvar->bound)
      tag = &tag->data.pvar->tag;
  }
  if (type == &g_sym_Tag) {
    *dest = tag;
    return true;
  }
  switch (tag->type) {
  case TAG_ARRAY:
    if (type == &g_sym_Array) {
      *dest = &tag->data.array;
      return true;
    }
    goto invalid_cast;
  case TAG_BOOL:
    if (type == &g_sym_Bool) {
      *dest = &tag->data.bool_;
      return true;
    }
    goto invalid_cast;
  case TAG_CHARACTER:
    if (type == &g_sym_Character) {
      *dest = &tag->data.character;
      return true;
    }
    goto invalid_cast;
  case TAG_DO_BLOCK:
    if (type == &g_sym_Block) {
      *dest = &tag->data.do_block;
      return true;
    }
    goto invalid_cast;
  case TAG_F32:
    if (type == &g_sym_F32) {
      *dest = &tag->data.f32;
      return true;
    }
    goto invalid_cast;
  case TAG_F64:
    if (type == &g_sym_F64) {
      *dest = &tag->data.f64;
      return true;
    }
    goto invalid_cast;
#if HAVE_F80
  case TAG_F80:
    if (type == &g_sym_F80) {
      *dest = &tag->data.f80;
      return true;
    }
    goto invalid_cast;
#endif
#if HAVE_F128
  case TAG_F128:
    if (type == &g_sym_F128) {
      *dest = &tag->data.f128;
      return true;
    }
    goto invalid_cast;
#endif
  case TAG_FACT:
    if (type == &g_sym_Fact) {
      *dest = &tag->data.fact;
      return true;
    }
    goto invalid_cast;
  case TAG_IDENT:
    if (type == &g_sym_Ident) {
      *dest = &tag->data.ident;
      return true;
    }
    goto invalid_cast;
  case TAG_INTEGER:
    if (type == &g_sym_Integer) {
      *dest = &tag->data.integer;
      return true;
    }
    goto invalid_cast;
  case TAG_MAP:
    if (type == &g_sym_Map) {
      *dest = &tag->data.map;
      return true;
    }
    goto invalid_cast;
  case TAG_PCALL:
    if (type == &g_sym_Call) {
      *dest = &tag->data.pcall;
      return true;
    }
    goto invalid_cast;
  case TAG_PCALLABLE:
    if (type == &g_sym_Callable ||
        (tag->data.pcallable &&
         ((type == &g_sym_Cfn &&
           tag->data.pcallable->type == CALLABLE_CFN) ||
          (type == &g_sym_Fn &&
           tag->data.pcallable->type == CALLABLE_FN)))) {
      *dest = &tag->data.pcallable;
      return true;
    }
    goto invalid_cast;
  case TAG_PCOMPLEX:
    if (type == &g_sym_Complex) {
      *dest = &tag->data.pcomplex;
      return true;
    }
    goto invalid_cast;
  case TAG_PCOW:
    if (type == &g_sym_Cow) {
      *dest = &tag->data.pcow;
      return true;
    }
    // FIXME: other types
    goto invalid_cast;
  case TAG_PFACTS:
    if (type == &g_sym_Facts) {
      *dest = tag->data.pfacts;
      return true;
    }
    if (type == &g_sym_Facts_star) {
      *dest = &tag->data.pfacts;
      return true;
    }
    goto invalid_cast;
  case TAG_PLIST:
    if (type == &g_sym_List) {
      *dest = &tag->data.plist;
      return true;
    }
    goto invalid_cast;
  case TAG_POINTER:
    if (type == &g_sym_Pointer) {
      *dest = &tag->data.pointer;
      return true;
    }
    *dest = &tag->data.pointer.ptr.p;
    return true;
  case TAG_PSTRUCT:
    if (type == &g_sym_Struct) {
      *dest = tag->data.pstruct;
      return true;
    }
    if (type == &g_sym_Ptr) {
      *dest = &tag->data.pstruct->data;
      assert(*dest);
      return true;
    }
    if (type == tag->data.pstruct->pstruct_type->module) {
      *dest = tag->data.pstruct->data;
      assert(*dest);
      return true;
    }
    goto invalid_cast;
  case TAG_PSTRUCT_TYPE:
    if (type == &g_sym_StructType) {
      *dest = tag->data.pstruct_type;
      return true;
    }
    goto invalid_cast;
  case TAG_PSYM:
    if (type == &g_sym_Sym) {
      *dest = (void *) &tag->data.psym;
      return true;
    }
    if (type == &g_sym_Str) {
      *dest = (void *) &tag->data.psym->str;
      return true;
    }
    if (type == &g_sym_Char__star) {
      *dest = (void *) tag->data.psym->str.ptr.pchar;
      return true;
    }
    goto invalid_cast;
  case TAG_PTAG:
    if (type == &g_sym_Ptag) {
      *dest = (void *) tag->data.ptag;
      return true;
    }
    goto invalid_cast;
  case TAG_PTR:
    if (type == &g_sym_Ptr) {
      *dest = &tag->data.ptr;
      return true;
    }
    *dest = tag->data.ptr.p;
    return true;
  case TAG_PTR_FREE:
    if (type == &g_sym_Ptr ||
        type == &g_sym_PtrFree) {
      *dest = &tag->data.ptr_free;
      return true;
    }
    *dest = tag->data.ptr.p;
    return true;
  case TAG_PVAR:
    if (type == &g_sym_Var) {
      *dest = &tag->data.pvar;
      return true;
    }
    goto invalid_cast;
  case TAG_QUOTE:
    if (type == &g_sym_Quote) {
      *dest = &tag->data.quote;
      return true;
    }
    goto invalid_cast;
  case TAG_RATIO:
    if (type == &g_sym_Ratio) {
      *dest = &tag->data.ratio;
      return true;
    }
    goto invalid_cast;
  case TAG_STR:
    if (type == &g_sym_Str) {
      *dest = &tag->data.str;
      return true;
    }
    if (type == &g_sym_Char__star) {
      *dest = (void *) tag->data.str.ptr.pchar;
      return true;
    }
    goto invalid_cast;
  case TAG_SW:
    if (type == &g_sym_Sw) {
      *dest = &tag->data.sw;
      return true;
    }
    goto invalid_cast;
  case TAG_S8:
    if (type == &g_sym_S8) {
      *dest = &tag->data.s8;
      return true;
    }
    goto invalid_cast;
  case TAG_S16:
    if (type == &g_sym_S16) {
      *dest = &tag->data.s16;
      return true;
    }
    goto invalid_cast;
  case TAG_S32:
    if (type == &g_sym_S32) {
      *dest = &tag->data.s32;
      return true;
    }
    goto invalid_cast;
  case TAG_S64:
    if (type == &g_sym_S64) {
      *dest = &tag->data.s64;
      return true;
    }
    goto invalid_cast;
  case TAG_TIME:
    if (type == &g_sym_Time) {
      *dest = &tag->data.time;
      return true;
    }
    goto invalid_cast;
  case TAG_TUPLE:
    if (type == &g_sym_Tuple) {
      *dest = &tag->data.tuple;
      return true;
    }
    goto invalid_cast;
  case TAG_U8:
    if (type == &g_sym_U8) {
      *dest = &tag->data.u8;
      return true;
    }
    goto invalid_cast;
  case TAG_U16:
    if (type == &g_sym_U16) {
      *dest = &tag->data.u16;
      return true;
    }
    goto invalid_cast;
  case TAG_U32:
    if (type == &g_sym_U32) {
      *dest = &tag->data.u32;
      return true;
    }
    goto invalid_cast;
  case TAG_U64:
    if (type == &g_sym_U64) {
      *dest = &tag->data.u64;
      return true;
    }
    goto invalid_cast;
  case TAG_UNQUOTE:
    if (type == &g_sym_Unquote) {
      *dest = &tag->data.unquote;
      return true;
    }
    goto invalid_cast;
  case TAG_UW:
    if (type == &g_sym_Uw) {
      *dest = &tag->data.uw;
      return true;
    }
    goto invalid_cast;
  case TAG_VOID:
    if (type == &g_sym_Void) {
      *dest = NULL;
      return true;
    }
    goto invalid_cast;
  }
  err_puts("tag_to_ffi_pointer: invalid tag type");
  assert(! "tag_to_ffi_pointer: invalid tag type");
  return false;
 invalid_cast:
  err_write_1("tag_to_ffi_pointer: cannot cast ");
  err_write_1(tag_type_to_string(tag->type));
  err_write_1(" to ");
  err_puts(type->str.ptr.pchar);
  err_stacktrace();
  assert(! "tag_to_ffi_pointer: cannot cast");
  return false;
}

bool tag_to_pointer (s_tag *tag, const s_sym *type, void **dest)
{
  e_tag_type tag_type;
  if (type == &g_sym_Tag) {
    *dest = tag;
    return true;
  }
  if (! sym_to_tag_type(type, &tag_type))
    return false;
  if (tag->type != tag_type) {
    err_write_1("tag_to_pointer: cannot cast ");
    err_write_1(tag_type_to_string(tag->type));
    err_write_1(" to ");
    err_puts(type->str.ptr.pchar);
    err_stacktrace();
    err_write_1("\n");
    assert(! "tag_to_pointer: cannot cast");
    return false;
  }
  switch (tag_type) {
  case TAG_ARRAY:        *dest = &tag->data.array;        return true;
  case TAG_BOOL:         *dest = &tag->data.bool_;        return true;
  case TAG_CHARACTER:    *dest = &tag->data.character;    return true;
  case TAG_DO_BLOCK:     *dest = &tag->data.do_block;     return true;
  case TAG_F32:          *dest = &tag->data.f32;          return true;
  case TAG_F64:          *dest = &tag->data.f64;          return true;
#if HAVE_F80
  case TAG_F80:          *dest = &tag->data.f80;          return true;
#endif
#if HAVE_F128
  case TAG_F128:         *dest = &tag->data.f128;         return true;
#endif
  case TAG_FACT:         *dest = &tag->data.fact;         return true;
  case TAG_IDENT:        *dest = &tag->data.ident;        return true;
  case TAG_INTEGER:      *dest = &tag->data.integer;      return true;
  case TAG_MAP:          *dest = &tag->data.map;          return true;
  case TAG_PCALL:        *dest = &tag->data.pcall;        return true;
  case TAG_PCALLABLE:    *dest = &tag->data.pcallable;    return true;
  case TAG_PCOMPLEX:     *dest = &tag->data.pcomplex;     return true;
  case TAG_PCOW:         *dest = &tag->data.pcow;         return true;
  case TAG_PFACTS:       *dest = &tag->data.pfacts;       return true;
  case TAG_PLIST:        *dest = &tag->data.plist;        return true;
  case TAG_POINTER:
    if (type == &g_sym_Pointer ||
        sym_is_pointer_type(type, tag->data.pointer.target_type)) {
      *dest = &tag->data.pointer;
      return true;
    }
    goto invalid_cast;
  case TAG_PSTRUCT:
    if (type == &g_sym_Struct) {
      *dest = &tag->data.pstruct;
      return true;
    }
    if (type == tag->data.pstruct->pstruct_type->module) {
      *dest = tag->data.pstruct->data;
      return true;
    }
    goto invalid_cast;
  case TAG_PSTRUCT_TYPE: *dest = &tag->data.pstruct_type; return true;
  case TAG_PSYM:         *dest = &tag->data.psym;         return true;
  case TAG_PTAG:         *dest = &tag->data.ptag;         return true;
  case TAG_PTR:          *dest = &tag->data.ptr.p;        return true;
  case TAG_PTR_FREE:     *dest = &tag->data.ptr_free.p;   return true;
  case TAG_PVAR:         *dest = tag->data.pvar;          return true;
  case TAG_QUOTE:        *dest = &tag->data.quote;        return true;
  case TAG_RATIO:        *dest = &tag->data.ratio;        return true;
  case TAG_SW:           *dest = &tag->data.sw;           return true;
  case TAG_S64:          *dest = &tag->data.s64;          return true;
  case TAG_S32:          *dest = &tag->data.s32;          return true;
  case TAG_S16:          *dest = &tag->data.s16;          return true;
  case TAG_S8:           *dest = &tag->data.s8;           return true;
  case TAG_STR:          *dest = &tag->data.str;          return true;
  case TAG_TIME:         *dest = &tag->data.time;         return true;
  case TAG_TUPLE:        *dest = &tag->data.tuple;        return true;
  case TAG_U8:           *dest = &tag->data.u8;           return true;
  case TAG_U16:          *dest = &tag->data.u16;          return true;
  case TAG_U32:          *dest = &tag->data.u32;          return true;
  case TAG_U64:          *dest = &tag->data.u64;          return true;
  case TAG_UNQUOTE:      *dest = &tag->data.unquote;      return true;
  case TAG_UW:           *dest = &tag->data.uw;           return true;
  case TAG_VOID:         *dest = NULL;                    return true;
  }
  err_puts("tag_to_pointer: invalid tag type");
  assert(! "tag_to_pointer: invalid tag type");
  return false;
 invalid_cast:
  err_write_1("tag_to_pointer: invalid cast from ");
  if (tag->type == TAG_PSTRUCT)
    err_inspect_sym(tag->data.pstruct->pstruct_type->module);
  else
    err_write_1(tag_type_to_string(tag->type));
  err_write_1(" to ");
  err_inspect_sym(type);
  err_write_1("\n");
  assert(! "tag_to_pointer: invalid cast");
  return false;
}

const s_sym ** tag_type (const s_tag *tag, const s_sym **dest)
{
  assert(tag);
  assert(dest);
  switch (tag->type) {
  case TAG_VOID:         *dest = &g_sym_Void;       return dest;
  case TAG_ARRAY:
    *dest = tag->data.array.array_type;
    return dest;
  case TAG_BOOL:         *dest = &g_sym_Bool;        return dest;
  case TAG_CHARACTER:    *dest = &g_sym_Character;   return dest;
  case TAG_DO_BLOCK:     *dest = &g_sym_Block;       return dest;
  case TAG_F32:          *dest = &g_sym_F32;         return dest;
  case TAG_F64:          *dest = &g_sym_F64;         return dest;
#if HAVE_F80
  case TAG_F80:          *dest = &g_sym_F80;         return dest;
#endif
#if HAVE_F128
  case TAG_F128:         *dest = &g_sym_F128;        return dest;
#endif
  case TAG_FACT:         *dest = &g_sym_Fact;        return dest;
  case TAG_IDENT:        *dest = &g_sym_Ident;       return dest;
  case TAG_INTEGER:      *dest = &g_sym_Integer;     return dest;
  case TAG_MAP:          *dest = &g_sym_Map;         return dest;
  case TAG_PCALL:        *dest = &g_sym_Call;        return dest;
  case TAG_PCALLABLE:    *dest = &g_sym_Callable;    return dest;
  case TAG_PCOMPLEX:     *dest = &g_sym_Complex;     return dest;
  case TAG_PCOW:         *dest = &g_sym_Cow;         return dest;
  case TAG_PFACTS:       *dest = &g_sym_Facts_star;  return dest;
  case TAG_PLIST:        *dest = &g_sym_List;        return dest;
  case TAG_POINTER:
    *dest = tag->data.pointer.pointer_type;          return dest;
  case TAG_PSTRUCT:
    *dest = tag->data.pstruct->pstruct_type->module; return dest;
  case TAG_PSTRUCT_TYPE: *dest = &g_sym_StructType;  return dest;
  case TAG_PSYM:         *dest = &g_sym_Sym;         return dest;
  case TAG_PTAG:         *dest = &g_sym_Ptag;        return dest;
  case TAG_PTR:          *dest = &g_sym_Ptr;         return dest;
  case TAG_PTR_FREE:     *dest = &g_sym_PtrFree;     return dest;
  case TAG_PVAR:         *dest = &g_sym_Var;         return dest;
  case TAG_QUOTE:        *dest = &g_sym_Quote;       return dest;
  case TAG_RATIO:        *dest = &g_sym_Ratio;       return dest;
  case TAG_S8:           *dest = &g_sym_S8;          return dest;
  case TAG_S16:          *dest = &g_sym_S16;         return dest;
  case TAG_S32:          *dest = &g_sym_S32;         return dest;
  case TAG_S64:          *dest = &g_sym_S64;         return dest;
  case TAG_STR:          *dest = &g_sym_Str;         return dest;
  case TAG_SW:           *dest = &g_sym_Sw;          return dest;
  case TAG_TIME:         *dest = &g_sym_Time;        return dest;
  case TAG_TUPLE:        *dest = &g_sym_Tuple;       return dest;
  case TAG_U8:           *dest = &g_sym_U8;          return dest;
  case TAG_U16:          *dest = &g_sym_U16;         return dest;
  case TAG_U32:          *dest = &g_sym_U32;         return dest;
  case TAG_U64:          *dest = &g_sym_U64;         return dest;
  case TAG_UNQUOTE:      *dest = &g_sym_Unquote;     return dest;
  case TAG_UW:           *dest = &g_sym_Uw;          return dest;
  }
  err_puts("tag_type: invalid tag type");
  assert(! "tag_type: invalid tag type");
  return NULL;
}

const s_sym ** tag_type_var (const s_tag *tag, const s_sym **dest)
{
  assert(tag);
  assert(dest);
  if (tag->type == TAG_PVAR) {
    *dest = tag->data.pvar->type;
    return dest;
  }
  return tag_type(tag, dest);
}

s_tag * tag_void (s_tag *tag)
{
  assert(tag);
  tag_clean(tag);
  *tag = (s_tag) {0};
  return tag;
}

bool tag_xor (const s_tag *a, const s_tag *b)
{
  s_tag f;
  tag_init_bool(&f, false);
  return (compare_tag(a, &f) != 0) != (compare_tag(b, &f) != 0);
}
