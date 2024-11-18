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
#include <string.h>
#include <strings.h>
#include "alloc.h"
#include "array.h"
#include "assert.h"
#include "block.h"
#include "bool.h"
#include "buf.h"
#include "buf_inspect.h"
#include "buf_parse.h"
#include "call.h"
#include "callable.h"
#include "cfn.h"
#include "compare.h"
#include "cow.h"
#include "env.h"
#include "explicit_bzero.h"
#include "fn.h"
#include "frame.h"
#include "hash.h"
#include "ident.h"
#include "integer.h"
#include "list.h"
#include "map.h"
#include "pcomplex.h"
#include "pcow.h"
#include "ptr.h"
#include "ptr_free.h"
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

bool * tag_and (const s_tag *a, const s_tag *b, bool *dest)
{
  s_tag f;
  assert(a);
  assert(b);
  assert(dest);
  tag_init_bool(&f, false);
  *dest = compare_tag(a, &f) != 0 && compare_tag(b, &f) != 0 ? 1 : 0;
  return dest;
}

s8 tag_arity (const s_tag *tag)
{
  if (tag->type == TAG_CALLABLE &&
      tag->data.callable) {
    switch (tag->data.callable->type) {
    case CALLABLE_CFN:
      return cfn_arity(&tag->data.callable->data.cfn);
    case CALLABLE_FN:
      return fn_arity(&tag->data.callable->data.fn);
    case CALLABLE_VOID:
      break;
    }
  }
  return -1;
}

s_tag * tag_assign (const s_tag *tag, const s_tag *value, s_tag *dest)
{
  assert(tag);
  assert(value);
  assert(dest);
  switch (tag->type) {
  case TAG_VAR:
    return var_assign(&tag->data.var, value, dest);
  case TAG_COW:
    return pcow_assign(&tag->data.cow, value, dest);
  default:
    break;
  }
  err_write_1("tag_assign: cannot assign to ");
  err_write_1(tag_type_to_string(tag->type));
  err_write_1("\n");
  assert(! "tag_assign: cannot assign to this tag type");
  return NULL;
}

s_tag * tag_brackets (const s_tag *tag, const s_tag *address,
                      s_tag *dest)
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
  case TAG_ARRAY:       array_clean(&tag->data.array);      break;
  case TAG_BLOCK:       block_clean(&tag->data.block);      break;
  case TAG_CALL:        call_clean(&tag->data.call);        break;
  case TAG_CALLABLE:
    p_callable_clean(&tag->data.callable);
    break;
  case TAG_COMPLEX:     pcomplex_clean(&tag->data.complex); break;
  case TAG_COW:         pcow_clean(&tag->data.cow);         break;
  case TAG_INTEGER:     integer_clean(&tag->data.integer);  break;
  case TAG_LIST:        list_delete_all(tag->data.list);    break;
  case TAG_MAP:         map_clean(&tag->data.map);          break;
  case TAG_PTR_FREE:    ptr_free_clean(&tag->data.ptr);     break;
  case TAG_QUOTE:       quote_clean(&tag->data.quote);      break;
  case TAG_RATIO:       ratio_clean(&tag->data.ratio);      break;
  case TAG_STR:         str_clean(&tag->data.str);          break;
  case TAG_STRUCT:      struct_clean(&tag->data.struct_);   break;
  case TAG_STRUCT_TYPE: struct_type_clean(&tag->data.struct_type);
                                                            break;
  case TAG_TIME:        time_clean(&tag->data.time);        break;
  case TAG_TUPLE:       tuple_clean(&tag->data.tuple);      break;
  case TAG_UNQUOTE:     unquote_clean(&tag->data.unquote);  break;
  case TAG_BOOL:
  case TAG_CHARACTER:
  case TAG_F32:
  case TAG_F64:
  case TAG_F128:
  case TAG_FACT:
  case TAG_IDENT:
  case TAG_PTAG:
  case TAG_PTR:
  case TAG_S8:
  case TAG_S16:
  case TAG_S32:
  case TAG_S64:
  case TAG_SW:
  case TAG_SYM:
  case TAG_U8:
  case TAG_U16:
  case TAG_U32:
  case TAG_U64:
  case TAG_UW:
  case TAG_VAR:
  case TAG_VOID:
    break;
  }
}

s_tag * tag_copy (s_tag *tag, const s_tag *src)
{
  tag_clean(tag);
  return tag_init_copy(tag, src);
}

void tag_delete (s_tag *tag)
{
  tag_clean(tag);
  free(tag);
}

bool * tag_eq (const s_tag *a, const s_tag *b, bool *dest)
{
  assert(a);
  assert(b);
  assert(dest);
  *dest = compare_tag(a, b) == 0 ? 1 : 0;
  return dest;
}

s_tag * tag_equal (const s_tag *a, const s_tag *b, s_tag *dest)
{
  assert(a);
  assert(b);
  assert(dest);
  if (! env_eval_equal_tag (&g_kc3_env, false, a, b, dest))
    return NULL;
  return dest;
}

bool * tag_gt (const s_tag *a, const s_tag *b, bool *dest)
{
  assert(a);
  assert(b);
  assert(dest);
  *dest = compare_tag(a, b) > 0 ? 1 : 0;
  return dest;
}

bool * tag_gte (const s_tag *a, const s_tag *b, bool *dest)
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
  return env_tag_ident_is_bound(&g_kc3_env, tag);
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
  buf_init(&buf, false, len, (char *) p); // buf is read-only
  buf.wpos = len;
  r = buf_parse_tag(&buf, tag);
  if (r < 0 || (uw) r != len) {
    err_write_1("invalid tag: \"");
    err_write_1(p);
    err_write_1("\", ");
    err_inspect_uw(&len);
    err_write_1(" != ");
    err_inspect_sw(&r);
    assert(! "invalid tag");
    return NULL;
  }
  return tag;
}

s_tag * tag_init_call_cast (s_tag *tag, const s_sym *type)
{
  s_tag tmp = {0};
  tmp.type = TAG_CALL;
  if (! call_init_call_cast(&tmp.data.call, type))
    return NULL;
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
    return tag_init_struct_with_data(tag, *type, src->data.ptr.p,
                                     false);
  case TAG_PTR_FREE:
    if (! src->data.ptr_free.p)
      return tag_init_void(tag);
    return tag_init_struct_with_data(tag, *type, src->data.ptr_free.p,
                                     false);
  case TAG_STRUCT:
    if (*type == src->data.struct_.type->module)
      return tag_init_struct_copy(tag, &src->data.struct_);
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
  switch (src->type) {
  case TAG_ARRAY:
    tag->type = src->type;
    if (! array_init_copy(&tag->data.array, &src->data.array))
      return NULL;
    return tag;
  case TAG_BOOL:
    tag->type = src->type;
    tag->data.bool = src->data.bool;
    return tag;
  case TAG_BLOCK:
    tag->type = src->type;
    if (! block_init_copy(&tag->data.block, &src->data.block))
      return NULL;
    return tag;
  case TAG_CALL:
    tag->type = src->type;
    if (! call_init_copy(&tag->data.call, &src->data.call))
      return NULL;
    return tag;
  case TAG_CALLABLE:
    tag->type = src->type;
    if (! p_callable_init_copy(&tag->data.callable,
                               &src->data.callable))
      return NULL;
    return tag;
  case TAG_CHARACTER:
    tag->type = src->type;
    tag->data.character = src->data.character;
    return tag;
  case TAG_COMPLEX:
    tag->type = src->type;
    if (! pcomplex_init_copy(&tag->data.complex,
                             (const s_complex * const *)
                             &src->data.complex))
      return NULL;
    return tag;
  case TAG_COW:
    tag->type = src->type;
    if (! pcow_init_copy(&tag->data.cow, &src->data.cow))
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
  case TAG_F128:
    tag->type = src->type;
    tag->data.f128 = src->data.f128;
    return tag;
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
  case TAG_LIST:
    tag->type = src->type;
    if (! list_init_copy(&tag->data.list,
                         (const s_list * const *)
                         &src->data.list))
      return NULL;
    return tag;
  case TAG_MAP:
    tag->type = src->type;
    if (! map_init_copy(&tag->data.map, &src->data.map))
      return NULL;
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
  case TAG_STRUCT:
    tag->type = src->type;
    if (! struct_init_copy(&tag->data.struct_, &src->data.struct_))
      return NULL;
    return tag;
  case TAG_STRUCT_TYPE:
    tag->type = src->type;
    if (! struct_type_init_copy(&tag->data.struct_type,
                                &src->data.struct_type))
      return NULL;
    return tag;
  case TAG_SW:
    tag->type = src->type;
    tag->data.sw = src->data.sw;
    return tag;
  case TAG_SYM:
    tag->type = src->type;
    tag->data.sym = src->data.sym;
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
  case TAG_UNQUOTE:
    tag->type = src->type;
    if (! unquote_init_copy(&tag->data.unquote, &src->data.unquote))
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
  case TAG_UW:
    tag->type = src->type;
    tag->data.uw = src->data.uw;
    return tag;
  case TAG_VAR:
    tag->type = src->type;
    var_init_copy(&tag->data.var, &src->data.var);
    return tag;
  case TAG_VOID:
    return tag_init_void(tag);
  }
  err_puts("tag_init_copy: invalid tag type");
  assert(! "tag_init_copy: invalid tag type");
  return NULL;
}

s_tag * tag_init_var (s_tag *tag, const s_sym *type)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_VAR;
  var_init(&tmp.data.var, tag, type);
  *tag = tmp;
  return tag;
}

s_tag * tag_init_void (s_tag *tag)
{
  assert(tag);
  explicit_bzero(tag, sizeof(s_tag));
  return tag;
}

s_tag * tag_integer_reduce (s_tag *tag)
{
  s_integer *i;
  s_integer j;
  static s_integer s8_min = {0};
  static s_integer s16_min = {0};
  static s_integer s32_min = {0};
  static s_integer s64_min = {0};
  static s_integer u8_max = {0};
  static s_integer u16_max = {0};
  static s_integer u32_max = {0};
  static s_integer u64_max = {0};
  static s_integer zero = {0};
  assert(tag);
  if (s8_min.mp_int.sign != MP_NEG) {
    integer_init_s8(&s8_min, S8_MIN);
    integer_init_s16(&s16_min, S16_MIN);
    integer_init_s32(&s32_min, S32_MIN);
    integer_init_s64(&s64_min, S64_MIN);
    integer_init_u8(&u8_max, U8_MAX);
    integer_init_u16(&u16_max, U16_MAX);
    integer_init_u32(&u32_max, U32_MAX);
    integer_init_u64(&u64_max, U64_MAX);
    integer_init_u8(&zero, 0);
  }
  switch (tag->type) {
  case TAG_INTEGER:
    i = &tag->data.integer;
    if (compare_integer(i, &u64_max) > 0)
      return tag;
    if (compare_integer(i, &u32_max) > 0) {
      j = tag->data.integer;
      tag_init_u64(tag, integer_to_u64(&j));
      integer_clean(&j);
      return tag;
    }
    if (compare_integer(i, &u16_max) > 0) {
      j = tag->data.integer;
      tag_init_u32(tag, integer_to_u32(&j));
      integer_clean(&j);
      return tag;
    }
    if (compare_integer(i, &u8_max) > 0) {
      j = tag->data.integer;
      tag_init_u16(tag, integer_to_u16(&j));
      integer_clean(&j);
      return tag;
    }
    if (compare_integer(i, &zero) >= 0) {
      j = tag->data.integer;
      tag_init_u8(tag, integer_to_u8(&j));
      integer_clean(&j);
      return tag;
    }
    if (compare_integer(i, &s8_min) >= 0) {
      j = tag->data.integer;
      tag_init_s8(tag, integer_to_s8(&j));
      integer_clean(&j);
      return tag;
    }
    if (compare_integer(i, &s16_min) >= 0) {
      j = tag->data.integer;
      tag_init_s16(tag, integer_to_s16(&j));
      integer_clean(&j);
      return tag;
    }
    if (compare_integer(i, &s32_min) >= 0) {
      j = tag->data.integer;
      tag_init_s32(tag, integer_to_s32(&j));
      integer_clean(&j);
      return tag;
    }
    if (compare_integer(i, &s64_min) >= 0) {
      j = tag->data.integer;
      tag_init_s64(tag, integer_to_s64(&j));
      integer_clean(&j);
      return tag;
    }
    return tag;
  case TAG_S8:
    integer_init_s8(&j, tag->data.s8);
    integer_reduce(&j, tag);
    integer_clean(&j);
    return tag;
  case TAG_S16:
    integer_init_s16(&j, tag->data.s16);
    integer_reduce(&j, tag);
    integer_clean(&j);
    return tag;
  case TAG_S32:
    integer_init_s32(&j, tag->data.s32);
    integer_reduce(&j, tag);
    integer_clean(&j);
    return tag;
  case TAG_S64:
    integer_init_s64(&j, tag->data.s64);
    integer_reduce(&j, tag);
    integer_clean(&j);
    return tag;
  case TAG_SW:
    integer_init_sw(&j, tag->data.sw);
    integer_reduce(&j, tag);
    integer_clean(&j);
    return tag;
  case TAG_U8:
    integer_init_u8(&j, tag->data.u8);
    integer_reduce(&j, tag);
    integer_clean(&j);
    return tag;
  case TAG_U16:
    integer_init_u16(&j, tag->data.u16);
    integer_reduce(&j, tag);
    integer_clean(&j);
    return tag;
  case TAG_U32:
    integer_init_u32(&j, tag->data.u32);
    integer_reduce(&j, tag);
    integer_clean(&j);
    return tag;
  case TAG_U64:
    integer_init_u64(&j, tag->data.u64);
    integer_reduce(&j, tag);
    integer_clean(&j);
    return tag;
  case TAG_UW:
    integer_init_uw(&j, tag->data.uw);
    integer_reduce(&j, tag);
    integer_clean(&j);
    return tag;
  default:
    break;
  }
  return tag;
}

bool tag_is_alist (const s_tag *tag)
{
  assert(tag);
  if (! tag)
    return false;
  if (tag->type != TAG_LIST)
    return false;
  return list_is_alist((const s_list * const *) &tag->data.list);
}

bool tag_is_bound_var (const s_tag *tag)
{
  assert(tag);
  return (tag &&
          tag->type != TAG_VAR);
}

bool tag_is_cast (const s_tag *tag, const s_sym *type)
{
  assert(tag);
  return (tag &&
          type &&
          tag->type == TAG_CALL &&
          tag->data.call.ident.module == type &&
          tag->data.call.ident.sym == &g_sym_cast);
}

bool tag_is_number (const s_tag *tag)
{
  assert(tag);
  tag = tag_resolve_cow(tag);
  switch (tag->type) {
  case TAG_VOID:
  case TAG_ARRAY:
  case TAG_BLOCK:
  case TAG_BOOL:
  case TAG_CALL:
  case TAG_CALLABLE:
  case TAG_CHARACTER:
  case TAG_FACT:
  case TAG_LIST:
  case TAG_MAP:
  case TAG_PTAG:
  case TAG_PTR:
  case TAG_PTR_FREE:
  case TAG_QUOTE:
  case TAG_STR:
  case TAG_STRUCT:
  case TAG_STRUCT_TYPE:
  case TAG_SYM:
  case TAG_TIME:
  case TAG_TUPLE:
  case TAG_UNQUOTE:
  case TAG_VAR:
  case TAG_IDENT:
    return false;
  case TAG_COMPLEX:
  case TAG_F32:
  case TAG_F64:
  case TAG_F128:
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
  case TAG_COW:
    break;
  }
  err_puts("tag_is_number: invalid tag type");
  assert(! "tag_is_number: invalid tag type");
  abort();
  return false;
}

bool tag_is_struct (const s_tag *tag, const s_sym *module)
{
  return tag &&
    tag->type == TAG_STRUCT &&
    tag->data.struct_.type->module == module;
}

bool * tag_is_unbound_var (const s_tag *tag, bool *dest)
{
  assert(tag);
  if (! tag) {
    err_puts("tag_is_unbound_var: NULL tag");
    assert(! "tag_is_unbound_var: NULL tag");
    return NULL;
  }
  *dest = (tag->type == TAG_VAR &&
           tag->data.var.ptr &&
           tag->data.var.ptr->type == TAG_VAR &&
           tag->data.var.ptr->data.var.ptr == tag->data.var.ptr);
  return dest;
}

bool tag_is_zero(const s_tag *tag)
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
  tmp.type = TAG_LIST;
  tmp.data.list = list_new_1(p);
  *tag = tmp;
  return tag;
}

bool * tag_lt (const s_tag *a, const s_tag *b, bool *dest)
{
  assert(a);
  assert(b);
  assert(dest);
  *dest = compare_tag(a, b) < 0 ? 1 : 0;
  return dest;
}

bool * tag_lte (const s_tag *a, const s_tag *b, bool *dest)
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

s_tag * tag_new_copy (const s_tag *src)
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

bool * tag_not (const s_tag *tag, bool *dest)
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

bool * tag_not_eq (const s_tag *a, const s_tag *b, bool *dest)
{
  assert(a);
  assert(b);
  assert(dest);
  *dest = compare_tag(a, b) != 0 ? 1 : 0;
  return dest;
}

bool * tag_or (const s_tag *a, const s_tag *b, bool *dest)
{
  s_tag f;
  assert(a);
  assert(b);
  assert(dest);
  tag_init_bool(&f, false);
  *dest = compare_tag(a, &f) != 0 ||
          compare_tag(b, &f) != 0 ? 1 : 0;
  return dest;
}

s_tag * tag_paren (const s_tag *tag, s_tag *dest)
{
  assert(tag);
  assert(dest);
  return tag_init_copy(dest, tag);
}

const s_tag * tag_resolve_cow (const s_tag *tag)
{
  while (tag->type == TAG_COW)
    tag = cow_read_only(tag->data.cow);
  if (! tag) {
    err_puts("tag_resolve_cow: cow was not frozen");
    assert(! "tag_resolve_cow: cow was not frozen");
    return NULL;
  }
  return tag;
}

s_tag * tag_semicolumn (const s_tag *a, const s_tag *b, s_tag *dest)
{
  (void) a;
  return tag_init_copy(dest, b);
}

uw * tag_size (const s_tag *tag, uw *dest)
{
  const s_sym *type;
  uw tmp = 0;
  assert(tag);
  if (! tag_type(tag, &type) ||
      ! sym_type_size(&type, &tmp))
    return NULL;
  *dest = tmp;
  return dest;
}

bool tag_to_const_pointer (s_tag *tag, const s_sym *type,
                           void **dest)
{
  e_tag_type tag_type;
  if (type == &g_sym_Tag) {
    *dest = tag;
    return true;
  }
  if (! sym_to_tag_type(type, &tag_type))
    return false;
  if (tag->type != tag_type) {
    err_write_1("tag_to_const_pointer: cannot cast ");
    err_write_1(tag_type_to_string(tag->type));
    err_write_1(" to ");
    err_puts(type->str.ptr.pchar);
    assert(! "tag_to_const_pointer: cannot cast");
    return false;
  }
  switch (tag_type) {
  case TAG_ARRAY:       *dest = &tag->data.array;       return true;
  case TAG_BLOCK:       *dest = &tag->data.block;       return true;
  case TAG_BOOL:        *dest = &tag->data.bool;        return true;
  case TAG_CALL:        *dest = &tag->data.call;        return true;
  case TAG_CALLABLE:    *dest = &tag->data.callable;    return true;
  case TAG_CHARACTER:   *dest = &tag->data.character;   return true;
  case TAG_COMPLEX:     *dest = &tag->data.complex;     return true;
  case TAG_COW:         *dest = &tag->data.cow;         return true;
  case TAG_F32:         *dest = &tag->data.f32;         return true;
  case TAG_F64:         *dest = &tag->data.f64;         return true;
  case TAG_F128:        *dest = &tag->data.f128;        return true;
  case TAG_FACT:        *dest = &tag->data.fact;        return true;
  case TAG_IDENT:       *dest = &tag->data.ident;       return true;
  case TAG_INTEGER:     *dest = &tag->data.integer;     return true;
  case TAG_SW:          *dest = &tag->data.sw;          return true;
  case TAG_S64:         *dest = &tag->data.s64;         return true;
  case TAG_S32:         *dest = &tag->data.s32;         return true;
  case TAG_S16:         *dest = &tag->data.s16;         return true;
  case TAG_S8:          *dest = &tag->data.s8;          return true;
  case TAG_U8:          *dest = &tag->data.u8;          return true;
  case TAG_U16:         *dest = &tag->data.u16;         return true;
  case TAG_U32:         *dest = &tag->data.u32;         return true;
  case TAG_U64:         *dest = &tag->data.u64;         return true;
  case TAG_UW:          *dest = &tag->data.uw;          return true;
  case TAG_LIST:        *dest = &tag->data.list;        return true;
  case TAG_MAP:         *dest = &tag->data.map;         return true;
  case TAG_PTAG:        *dest = &tag->data.ptag;        return true;
  case TAG_PTR:         *dest = &tag->data.ptr.p;       return true;
  case TAG_PTR_FREE:    *dest = &tag->data.ptr_free.p;  return true;
  case TAG_QUOTE:       *dest = &tag->data.quote;       return true;
  case TAG_RATIO:       *dest = &tag->data.ratio;       return true;
  case TAG_STR:         *dest = &tag->data.str;         return true;
  case TAG_STRUCT:
    if (type == &g_sym_Struct) {
      *dest = &tag->data.struct_;
      return true;
    }
    if (type == tag->data.struct_.type->module) {
      *dest = tag->data.struct_.data;
      assert(*dest);
      return true;
    }
    err_write_1("tag_to_const_pointer: cannot cast ");
    err_write_1(tag_type_to_string(tag->type));
    err_write_1(" to ");
    err_inspect_sym(&type);
    err_write_1("\n");
    assert(! "tag_to_const_pointer: cannot cast");
    return false;
  case TAG_STRUCT_TYPE: *dest = &tag->data.struct_type; return true;
  case TAG_SYM:         *dest = &tag->data.sym;         return true;
  case TAG_TIME:        *dest = &tag->data.time;        return true;
  case TAG_TUPLE:       *dest = &tag->data.tuple;       return true;
  case TAG_UNQUOTE:     *dest = &tag->data.unquote;     return true;
  case TAG_VAR:         *dest = &tag->data.var;         return true;
  case TAG_VOID:        *dest = NULL;                   return true;
  }
  err_write_1("tag_to_const_pointer: invalid tag type: ");
  err_puts(tag_type_to_string(tag_type));
  assert(! "tag_to_const_pointer: invalid tag type");
  return false;
}

bool tag_to_ffi_pointer (s_tag *tag, const s_sym *type, void **dest)
{
  if (type == &g_sym_Tag) {
    *dest = tag;
    return true;
  }
  switch (tag->type) {
  case TAG_ARRAY:
    if (type == &g_sym_Array) {
      *dest = tag->data.array.data;
      return true;
    }
    goto invalid_cast;
  case TAG_BLOCK:
    if (type == &g_sym_Block) {
      *dest = &tag->data.block;
      return true;
    }
    goto invalid_cast;
  case TAG_BOOL:
    if (type == &g_sym_Bool) {
      *dest = &tag->data.bool;
      return true;
    }
    goto invalid_cast;
  case TAG_CALL:
    if (type == &g_sym_Call) {
      *dest = &tag->data.call;
      return true;
    }
    goto invalid_cast;
  case TAG_CALLABLE:
    if (type == &g_sym_Callable ||
        (type == &g_sym_Cfn &&
         tag->data.callable &&
         tag->data.callable->type == CALLABLE_CFN) ||
        (type == &g_sym_Fn &&
         tag->data.callable
         tag->data.callable->type == CALLABLE_FN)) {
      *dest = &tag->data.callable;
      return true;
    }
    goto invalid_cast;
  case TAG_CHARACTER:
    if (type == &g_sym_Character) {
      *dest = &tag->data.character;
      return true;
    }
    goto invalid_cast;
  case TAG_COMPLEX:
    if (type == &g_sym_Complex) {
      *dest = &tag->data.complex;
      return true;
    }
    goto invalid_cast;
  case TAG_COW:
    if (type == &g_sym_Cow) {
      *dest = &tag->data.cow;
      return true;
    }
    // FIXME: other types
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
  case TAG_F128:
    if (type == &g_sym_F128) {
      *dest = &tag->data.f128;
      return true;
    }
    goto invalid_cast;
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
  case TAG_LIST:
    if (type == &g_sym_List) {
      *dest = &tag->data.list;
      return true;
    }
    goto invalid_cast;
  case TAG_MAP:
    if (type == &g_sym_Map) {
      *dest = &tag->data.map;
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
  case TAG_STRUCT:
    if (type == &g_sym_Struct) {
      *dest = &tag->data.struct_;
      return true;
    }
    if (type == &g_sym_Ptr) {
      *dest = &tag->data.struct_.data;
      assert(*dest);
      return true;
    }
    if (type == tag->data.struct_.type->module) {
      *dest = tag->data.struct_.data;
      assert(*dest);
      return true;
    }
    goto invalid_cast;
  case TAG_STRUCT_TYPE:
    if (type == &g_sym_StructType) {
      *dest = &tag->data.struct_type;
      return true;
    }
    goto invalid_cast;
  case TAG_SW:
    if (type == &g_sym_Sw) {
      *dest = &tag->data.sw;
      return true;
    }
    goto invalid_cast;
  case TAG_SYM:
    if (type == &g_sym_Sym) {
      *dest = (void *) &tag->data.sym;
      return true;
    }
    if (type == &g_sym_Str) {
      *dest = (void *) &tag->data.sym->str;
      return true;
    }
    if (type == &g_sym_Char__star) {
      *dest = (void *) tag->data.sym->str.ptr.pchar;
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
  case TAG_VAR:
    if (type == &g_sym_Var) {
      *dest = &tag->data.var;
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
    assert(! "tag_to_pointer: cannot cast");
    return false;
  }
  switch (tag_type) {
  case TAG_ARRAY:       *dest = &tag->data.array;       return true;
  case TAG_BLOCK:       *dest = &tag->data.block;       return true;
  case TAG_BOOL:        *dest = &tag->data.bool;        return true;
  case TAG_CALL:        *dest = &tag->data.call;        return true;
  case TAG_CALLABLE:    *dest = &tag->data.callable;    return true;
  case TAG_CHARACTER:   *dest = &tag->data.character;   return true;
  case TAG_COMPLEX:     *dest = &tag->data.complex;     return true;
  case TAG_COW:         *dest = &tag->data.cow;         return true;
  case TAG_F32:         *dest = &tag->data.f32;         return true;
  case TAG_F64:         *dest = &tag->data.f64;         return true;
  case TAG_F128:        *dest = &tag->data.f128;        return true;
  case TAG_FACT:        *dest = &tag->data.fact;        return true;
  case TAG_IDENT:       *dest = &tag->data.ident;       return true;
  case TAG_INTEGER:     *dest = &tag->data.integer;     return true;
  case TAG_LIST:        *dest = &tag->data.list;        return true;
  case TAG_MAP:         *dest = &tag->data.map;         return true;
  case TAG_PTAG:        *dest = &tag->data.ptag;        return true;
  case TAG_PTR:         *dest = &tag->data.ptr.p;       return true;
  case TAG_PTR_FREE:    *dest = &tag->data.ptr_free.p;  return true;
  case TAG_QUOTE:       *dest = &tag->data.quote;       return true;
  case TAG_RATIO:       *dest = &tag->data.ratio;       return true;
  case TAG_SW:          *dest = &tag->data.sw;          return true;
  case TAG_S64:         *dest = &tag->data.s64;         return true;
  case TAG_S32:         *dest = &tag->data.s32;         return true;
  case TAG_S16:         *dest = &tag->data.s16;         return true;
  case TAG_S8:          *dest = &tag->data.s8;          return true;
  case TAG_STR:         *dest = &tag->data.str;         return true;
  case TAG_STRUCT:
    if (type == &g_sym_Struct) {
      *dest = &tag->data.struct_;
      return true;
    }
    if (type == tag->data.struct_.type->module) {
      *dest = tag->data.struct_.data;
      return true;
    }
    goto invalid_cast;
  case TAG_STRUCT_TYPE: *dest = &tag->data.struct_type; return true;
  case TAG_SYM:         *dest = &tag->data.sym;         return true;
  case TAG_TIME:        *dest = &tag->data.time;        return true;
  case TAG_TUPLE:       *dest = &tag->data.tuple;       return true;
  case TAG_U8:          *dest = &tag->data.u8;          return true;
  case TAG_U16:         *dest = &tag->data.u16;         return true;
  case TAG_U32:         *dest = &tag->data.u32;         return true;
  case TAG_U64:         *dest = &tag->data.u64;         return true;
  case TAG_UNQUOTE:     *dest = &tag->data.unquote;     return true;
  case TAG_UW:          *dest = &tag->data.uw;          return true;
  case TAG_VAR:         *dest = &tag->data.var;         return true;
  case TAG_VOID:        *dest = NULL;                   return true;
  }
  err_puts("tag_to_pointer: invalid tag type");
  assert(! "tag_to_pointer: invalid tag type");
  return false;
 invalid_cast:
  err_write_1("tag_to_pointer: invalid cast from ");
  if (tag->type == TAG_STRUCT)
    err_inspect_sym(&tag->data.struct_.type->module);
  else
    err_write_1(tag_type_to_string(tag->type));
  err_write_1(" to ");
  err_inspect_sym(&type);
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
  case TAG_BLOCK:        *dest = &g_sym_Block;      return dest;
  case TAG_BOOL:         *dest = &g_sym_Bool;       return dest;
  case TAG_CALL:         *dest = &g_sym_Call;       return dest;
  case TAG_CALLABLE:
    if (tag->data.callable) {
      switch (tag->data.callable->type) {
      case CALLABLE_FN:  *dest = &g_sym_Fn;         return dest;
      case CALLABLE_CFN: *dest = &g_sym_Cfn;        return dest;
      case CALLABLE_VOID:
        err_puts("tag_type: CALLABLE_VOID");
        assert(! "tag_type: CALLABLE_VOID");
        return NULL;
      }
    }
    *dest = &g_sym_Callable;
    return dest;
  case TAG_CHARACTER:    *dest = &g_sym_Character;  return dest;
  case TAG_COMPLEX:      *dest = &g_sym_Complex;    return dest;
  case TAG_COW:          *dest = &g_sym_Cow;        return dest;
  case TAG_F32:          *dest = &g_sym_F32;        return dest;
  case TAG_F64:          *dest = &g_sym_F64;        return dest;
  case TAG_F128:         *dest = &g_sym_F128;       return dest;
  case TAG_FACT:         *dest = &g_sym_Fact;       return dest;
  case TAG_IDENT:        *dest = &g_sym_Ident;      return dest;
  case TAG_INTEGER:      *dest = &g_sym_Integer;    return dest;
  case TAG_SW:           *dest = &g_sym_Sw;         return dest;
  case TAG_S64:          *dest = &g_sym_S64;        return dest;
  case TAG_S32:          *dest = &g_sym_S32;        return dest;
  case TAG_S16:          *dest = &g_sym_S16;        return dest;
  case TAG_S8:           *dest = &g_sym_S8;         return dest;
  case TAG_U8:           *dest = &g_sym_U8;         return dest;
  case TAG_U16:          *dest = &g_sym_U16;        return dest;
  case TAG_U32:          *dest = &g_sym_U32;        return dest;
  case TAG_U64:          *dest = &g_sym_U64;        return dest;
  case TAG_UW:           *dest = &g_sym_Uw;         return dest;
  case TAG_LIST:         *dest = &g_sym_List;       return dest;
  case TAG_MAP:          *dest = &g_sym_Map;        return dest;
  case TAG_PTAG:         *dest = &g_sym_Ptag;       return dest;
  case TAG_PTR:          *dest = &g_sym_Ptr;        return dest;
  case TAG_PTR_FREE:     *dest = &g_sym_PtrFree;    return dest;
  case TAG_QUOTE:        *dest = &g_sym_Quote;      return dest;
  case TAG_RATIO:        *dest = &g_sym_Ratio;      return dest;
  case TAG_STR:          *dest = &g_sym_Str;        return dest;
  case TAG_STRUCT:       *dest = tag->data.struct_.type->module;
                                                    return dest;
  case TAG_STRUCT_TYPE:  *dest = &g_sym_StructType; return dest;
  case TAG_SYM:          *dest = &g_sym_Sym;        return dest;
  case TAG_TIME:         *dest = &g_sym_Time;       return dest;
  case TAG_TUPLE:        *dest = &g_sym_Tuple;      return dest;
  case TAG_UNQUOTE:      *dest = &g_sym_Unquote;    return dest;
  case TAG_VAR:          *dest = &g_sym_Var;        return dest;
  }
  err_puts("tag_type: invalid tag type");
  assert(! "tag_type: invalid tag type");
  return NULL;
}

const s_sym ** tag_var_type (const s_tag *tag, const s_sym **dest)
{
  assert(tag);
  assert(dest);
  if (tag->type == TAG_VAR) {
    *dest = tag->data.var.type;
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
