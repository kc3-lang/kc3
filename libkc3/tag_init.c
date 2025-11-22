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
#include "array.h"
#include "buf.h"
#include "buf_inspect.h"
#include "buf_parse.h"
#include "call.h"
#include "callable.h"
#include "cfn.h"
#include "compare.h"
#include "env.h"
#include "fn.h"
#include "frame.h"
#include "hash.h"
#include "ident.h"
#include "integer.h"
#include "list.h"
#include "map.h"
#include "pcall.h"
#include "pcallable.h"
#include "plist.h"
#include "pointer.h"
#include "pstruct.h"
#include "pstruct_type.h"
#include "psym.h"
#include "ptr.h"
#include "ptr_free.h"
#include "pvar.h"
#include "quote.h"
#include "ratio.h"
#include "str.h"
#include "struct.h"
#include "struct_type.h"
#include "tag.h"
#include "tag_init.h"
#include "time.h"
#include "tuple.h"
#include "unquote.h"
#include "var.h"

s_tag * tag_init_array (s_tag *tag, const s_sym *type, uw dimension,
                        const uw *dimensions)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_ARRAY;
  if (! array_init(&tmp.data.array, type, dimension, dimensions))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_array_copy (s_tag *tag, const s_array *a)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_ARRAY;
  if (! array_init_copy(&tmp.data.array, a))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_character (s_tag *tag, character c)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_CHARACTER;
  tmp.data.character = c;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_f32 (s_tag *tag, f32 f)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_F32;
  tmp.data.f32 = f;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_f64 (s_tag *tag, f64 f)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_F64;
  tmp.data.f64 = f;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_ident (s_tag *tag, const s_ident *ident)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_IDENT;
  tmp.data.ident = *ident;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_ident_1 (s_tag *tag, const char *p)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_IDENT;
  if (! ident_init_1(&tmp.data.ident, p))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_integer_1 (s_tag *tag, const char *p)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_INTEGER;
  if (! integer_init_1(&tmp.data.integer, p))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_integer_copy (s_tag *tag, const s_integer *i)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_INTEGER;
  if (! integer_init_copy(&tmp.data.integer, i))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_integer_zero (s_tag *tag)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_INTEGER;
  if (! integer_init_zero(&tmp.data.integer))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_map (s_tag *tag, uw count)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_MAP;
  if (! map_init(&tmp.data.map, count))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_map_1 (s_tag *tag, const char *p)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_MAP;
  if (! map_init_1(&tmp.data.map, p))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_map_from_lists (s_tag *tag, s_list *keys,
                                 s_list *values)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_MAP;
  if (! map_init_from_lists(&tmp.data.map, keys, values))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_pcall (s_tag *tag)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_PCALL;
  if (! pcall_init(&tmp.data.pcall))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_pcall_call_cast (s_tag *tag, const s_sym *type)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_PCALL;
  if (! pcall_init_call_cast(&tmp.data.pcall, type))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_pcall_copy (s_tag *tag, p_call *src)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_PCALL;
  if (! pcall_init_copy(&tmp.data.pcall, src))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_pcallable (s_tag *tag)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_PCALLABLE;
  if (! pcallable_init(&tmp.data.pcallable))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_pcallable_copy (s_tag *tag, p_callable *src)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_PCALLABLE;
  if (! pcallable_init_copy(&tmp.data.pcallable, src))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_pcomplex (s_tag *tag, p_complex c)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_PCOMPLEX;
  tmp.data.pcomplex = c;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_plist (s_tag *tag, p_list plist)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_PLIST;
  tmp.data.plist = plist;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_plist_1 (s_tag *tag, const char *p)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_PLIST;
  if (! plist_init_1(&tmp.data.plist, p))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_pointer (s_tag *tag, const s_sym *pointer_type,
                          const s_sym *target_type, void *p)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_POINTER;
  if (! pointer_init(&tmp.data.pointer, pointer_type, target_type, p))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_pstruct (s_tag *tag, const s_sym *module)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_PSTRUCT;
  if (! pstruct_init(&tmp.data.pstruct, module))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_pstruct_copy (s_tag *tag, p_struct *src)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_PSTRUCT;
  if (! pstruct_init_copy(&tmp.data.pstruct, src))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_pstruct_copy_data (s_tag *tag, const s_sym *module,
                                    void *data)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_PSTRUCT;
  if (! pstruct_init_copy_data(&tmp.data.pstruct, module, data))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_pstruct_with_data (s_tag *tag, const s_sym *module,
                                    void *data, bool free_data)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_PSTRUCT;
  if (! pstruct_init_with_data(&tmp.data.pstruct, module, data,
                               free_data))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_pstruct_with_type (s_tag *tag, s_struct_type *st)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_PSTRUCT;
  if (! pstruct_init_with_type(&tmp.data.pstruct, st))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_pstruct_type (s_tag *tag, const s_sym *module,
                               s_list *spec)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_PSTRUCT_TYPE;
  if (! pstruct_type_init(&tmp.data.pstruct_type, module, spec))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_pstruct_type_clean (s_tag *tag, s_struct_type *st,
                                     p_callable clean)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_PSTRUCT_TYPE;
  if (! pstruct_type_init_clean(&tmp.data.pstruct_type, st, clean))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_psym (s_tag *tag, const s_sym *sym)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_PSYM;
  tmp.data.psym = sym;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_psym_anon (s_tag *tag, const s_str *src)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_PSYM;
  if (! psym_init_anon(&tmp.data.psym, src))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_ptr (s_tag *tag, void *p)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_PTR;
  if (! ptr_init(&tmp.data.ptr, p))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_ptr_free (s_tag *tag, void *p)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_PTR_FREE;
  if (! ptr_free_init(&tmp.data.ptr_free, p))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_pvar (s_tag *tag, const s_sym *type)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_PVAR;
  if (! pvar_init(&tmp.data.pvar, type))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_pvar_copy (s_tag *tag, p_var *src)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_PVAR;
  if (! pvar_init_copy(&tmp.data.pvar, src))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_quote (s_tag *tag, s_tag *src)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_QUOTE;
  if (! quote_init(&tmp.data.quote, src))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_quote_copy (s_tag *tag, s_quote *quote)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_QUOTE;
  if (! quote_init_copy(&tmp.data.quote, quote))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_ratio_1 (s_tag *tag, const char *p)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_RATIO;
  if (! ratio_init_1(&tmp.data.ratio, p))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_ratio (s_tag *tag)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_RATIO;
  if (! ratio_init(&tmp.data.ratio))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_ratio_copy (s_tag *tag, s_ratio *r)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_RATIO;
  if (! ratio_init_copy(&tmp.data.ratio, r))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_ratio_zero (s_tag *tag)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_RATIO;
  if (! ratio_init_zero(&tmp.data.ratio))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_s8 (s_tag *tag, s8 i)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_S8;
  tmp.data.s8 = i;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_s16 (s_tag *tag, s16 i)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_S16;
  tmp.data.s16 = i;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_s32 (s_tag *tag, s32 i)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_S32;
  tmp.data.s32 = i;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_s64 (s_tag *tag, s64 i)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_S64;
  tmp.data.s64 = i;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_str (s_tag *tag, char *p_free, uw size, const char *p)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_STR;
  if (! str_init(&tmp.data.str, p_free, size, p))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_str_1 (s_tag *tag, char *p_free, const char *p)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_STR;
  if (! str_init_1(&tmp.data.str, p_free, p))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_str_1_alloc (s_tag *tag, const char *p)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_STR;
  if (! str_init_1_alloc(&tmp.data.str, p))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_str_alloc_copy (s_tag *tag, uw size, const char *p)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_STR;
  if (! str_init_alloc_copy(&tmp.data.str, size, p))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_str_cast (s_tag *tag, p_sym *type, const s_tag *src)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_STR;
  if (! str_init_cast(&tmp.data.str, type, src))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_str_concatenate (s_tag *tag, const s_str *a,
                                  const s_str *b)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_STR;
  if (! str_init_concatenate(&tmp.data.str, a, b))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_str_concatenate_list (s_tag *tag, const s_list *src)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_STR;
  if (! str_init_concatenate_list(&tmp.data.str, src))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_str_concatenate_plist (s_tag *tag, p_list *src)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_STR;
  if (! str_init_concatenate_plist(&tmp.data.str, src))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_str_copy (s_tag *tag, const s_str *src)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_STR;
  if (! str_init_copy(&tmp.data.str, src))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_str_empty (s_tag *tag)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_STR;
  if (! str_init_empty(&tmp.data.str))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_str_inspect_buf (s_tag *tag, const s_buf *src)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_STR;
  if (! str_init_inspect_buf(&tmp.data.str, src))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_str_inspect_str (s_tag *tag, const s_str *src)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_STR;
  if (! str_init_inspect_str(&tmp.data.str, src))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_sw (s_tag *tag, sw i)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_SW;
  tmp.data.sw = i;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_tuple (s_tag *tag, uw count)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_TUPLE;
  if (! tuple_init(&tmp.data.tuple, count))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_tuple_2 (s_tag *tag, s_tag *a, s_tag *b)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_TUPLE;
  if (! tuple_init_2(&tmp.data.tuple, a, b))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_time_add (s_tag *tag, const s_time *a, const s_time *b)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_TIME;
  if (! time_init_add(&tmp.data.time, a, b))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_time_now (s_tag *tag)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_TIME;
  if (! time_init_now(&tmp.data.time))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_u8 (s_tag *tag, u8 i)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_U8;
  tmp.data.u8 = i;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_u16 (s_tag *tag, u16 i)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_U16;
  tmp.data.u16 = i;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_u32 (s_tag *tag, u32 i)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_U32;
  tmp.data.u32 = i;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_u64 (s_tag *tag, u64 i)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_U64;
  tmp.data.u64 = i;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_unquote_copy (s_tag *tag, s_unquote *unquote)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_UNQUOTE;
  if (! unquote_init_copy(&tmp.data.unquote, unquote))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_uw (s_tag *tag, uw i)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_UW;
  tmp.data.uw = i;
  *tag = tmp;
  return tag;
}

s_tag * tag_new_array (const s_sym *type, uw dimension,
                       const uw *dimensions)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_ARRAY;
  if (! array_init(&tag->data.array, type, dimension, dimensions)) {
    free(tag);
    return NULL;
  }
  return tag;
}

s_tag * tag_new_array_copy (const s_array *a)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_ARRAY;
  if (! array_init_copy(&tag->data.array, a)) {
    free(tag);
    return NULL;
  }
  return tag;
}

s_tag * tag_new_character (character c)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_CHARACTER;
  tag->data.character = c;
  return tag;
}

s_tag * tag_new_f32 (f32 f)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_F32;
  tag->data.f32 = f;
  return tag;
}

s_tag * tag_new_f64 (f64 f)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_F64;
  tag->data.f64 = f;
  return tag;
}

s_tag * tag_new_ident (const s_ident *ident)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_IDENT;
  tag->data.ident = *ident;
  return tag;
}

s_tag * tag_new_ident_1 (const char *p)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_IDENT;
  if (! ident_init_1(&tag->data.ident, p)) {
    free(tag);
    return NULL;
  }
  return tag;
}

s_tag * tag_new_integer_1 (const char *p)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_INTEGER;
  if (! integer_init_1(&tag->data.integer, p)) {
    free(tag);
    return NULL;
  }
  return tag;
}

s_tag * tag_new_integer_copy (const s_integer *i)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_INTEGER;
  if (! integer_init_copy(&tag->data.integer, i)) {
    free(tag);
    return NULL;
  }
  return tag;
}

s_tag * tag_new_integer_zero (void)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_INTEGER;
  if (! integer_init_zero(&tag->data.integer)) {
    free(tag);
    return NULL;
  }
  return tag;
}

s_tag * tag_new_map (uw count)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_MAP;
  if (! map_init(&tag->data.map, count)) {
    free(tag);
    return NULL;
  }
  return tag;
}

s_tag * tag_new_map_1 (const char *p)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_MAP;
  if (! map_init_1(&tag->data.map, p)) {
    free(tag);
    return NULL;
  }
  return tag;
}

s_tag * tag_new_map_from_lists (s_list *keys, s_list *values)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_MAP;
  if (! map_init_from_lists(&tag->data.map, keys, values)) {
    free(tag);
    return NULL;
  }
  return tag;
}

s_tag * tag_new_pcall (void)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_PCALL;
  if (! pcall_init(&tag->data.pcall)) {
    free(tag);
    return NULL;
  }
  return tag;
}

s_tag * tag_new_pcall_call_cast (const s_sym *type)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_PCALL;
  if (! pcall_init_call_cast(&tag->data.pcall, type)) {
    free(tag);
    return NULL;
  }
  return tag;
}

s_tag * tag_new_pcall_copy (p_call *src)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_PCALL;
  if (! pcall_init_copy(&tag->data.pcall, src)) {
    free(tag);
    return NULL;
  }
  return tag;
}

s_tag * tag_new_pcallable (void)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_PCALLABLE;
  if (! pcallable_init(&tag->data.pcallable)) {
    free(tag);
    return NULL;
  }
  return tag;
}

s_tag * tag_new_pcallable_copy (p_callable *src)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_PCALLABLE;
  if (! pcallable_init_copy(&tag->data.pcallable, src)) {
    free(tag);
    return NULL;
  }
  return tag;
}

s_tag * tag_new_pcomplex (p_complex c)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_PCOMPLEX;
  tag->data.pcomplex = c;
  return tag;
}

s_tag * tag_new_plist (p_list plist)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_PLIST;
  tag->data.plist = plist;
  return tag;
}

s_tag * tag_new_plist_1 (const char *p)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_PLIST;
  if (! plist_init_1(&tag->data.plist, p)) {
    free(tag);
    return NULL;
  }
  return tag;
}

s_tag * tag_new_pointer (const s_sym *pointer_type,
                         const s_sym *target_type, void *p)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_POINTER;
  if (! pointer_init(&tag->data.pointer, pointer_type, target_type,
                     p)) {
    free(tag);
    return NULL;
  }
  return tag;
}

s_tag * tag_new_pstruct (const s_sym *module)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_PSTRUCT;
  if (! pstruct_init(&tag->data.pstruct, module)) {
    free(tag);
    return NULL;
  }
  return tag;
}

s_tag * tag_new_pstruct_copy (p_struct *src)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_PSTRUCT;
  if (! pstruct_init_copy(&tag->data.pstruct, src)) {
    free(tag);
    return NULL;
  }
  return tag;
}

s_tag * tag_new_pstruct_copy_data (const s_sym *module, void *data)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_PSTRUCT;
  if (! pstruct_init_copy_data(&tag->data.pstruct, module, data)) {
    free(tag);
    return NULL;
  }
  return tag;
}

s_tag * tag_new_pstruct_with_data (const s_sym *module, void *data,
                                   bool free_data)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_PSTRUCT;
  if (! pstruct_init_with_data(&tag->data.pstruct, module, data,
                               free_data)) {
    free(tag);
    return NULL;
  }
  return tag;
}

s_tag * tag_new_pstruct_with_type (s_struct_type *st)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_PSTRUCT;
  if (! pstruct_init_with_type(&tag->data.pstruct, st)) {
    free(tag);
    return NULL;
  }
  return tag;
}

s_tag * tag_new_pstruct_type (const s_sym *module, s_list *spec)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_PSTRUCT_TYPE;
  if (! pstruct_type_init(&tag->data.pstruct_type, module, spec)) {
    free(tag);
    return NULL;
  }
  return tag;
}

s_tag * tag_new_pstruct_type_clean (s_struct_type *st, p_callable clean)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_PSTRUCT_TYPE;
  if (! pstruct_type_init_clean(&tag->data.pstruct_type, st, clean)) {
    free(tag);
    return NULL;
  }
  return tag;
}

s_tag * tag_new_psym (const s_sym *sym)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_PSYM;
  tag->data.psym = sym;
  return tag;
}

s_tag * tag_new_psym_anon (const s_str *src)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_PSYM;
  if (! psym_init_anon(&tag->data.psym, src)) {
    free(tag);
    return NULL;
  }
  return tag;
}

s_tag * tag_new_ptr (void *p)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_PTR;
  if (! ptr_init(&tag->data.ptr, p)) {
    free(tag);
    return NULL;
  }
  return tag;
}

s_tag * tag_new_ptr_free (void *p)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_PTR_FREE;
  if (! ptr_free_init(&tag->data.ptr_free, p)) {
    free(tag);
    return NULL;
  }
  return tag;
}

s_tag * tag_new_pvar (const s_sym *type)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_PVAR;
  if (! pvar_init(&tag->data.pvar, type)) {
    free(tag);
    return NULL;
  }
  return tag;
}

s_tag * tag_new_pvar_copy (p_var *src)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_PVAR;
  if (! pvar_init_copy(&tag->data.pvar, src)) {
    free(tag);
    return NULL;
  }
  return tag;
}

s_tag * tag_new_quote (s_tag *src)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_QUOTE;
  if (! quote_init(&tag->data.quote, src)) {
    free(tag);
    return NULL;
  }
  return tag;
}

s_tag * tag_new_quote_copy (s_quote *quote)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_QUOTE;
  if (! quote_init_copy(&tag->data.quote, quote)) {
    free(tag);
    return NULL;
  }
  return tag;
}

s_tag * tag_new_ratio_1 (const char *p)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_RATIO;
  if (! ratio_init_1(&tag->data.ratio, p)) {
    free(tag);
    return NULL;
  }
  return tag;
}

s_tag * tag_new_ratio (void)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_RATIO;
  if (! ratio_init(&tag->data.ratio)) {
    free(tag);
    return NULL;
  }
  return tag;
}

s_tag * tag_new_ratio_copy (s_ratio *r)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_RATIO;
  if (! ratio_init_copy(&tag->data.ratio, r)) {
    free(tag);
    return NULL;
  }
  return tag;
}

s_tag * tag_new_ratio_zero (void)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_RATIO;
  if (! ratio_init_zero(&tag->data.ratio)) {
    free(tag);
    return NULL;
  }
  return tag;
}

s_tag * tag_new_s8 (s8 i)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_S8;
  tag->data.s8 = i;
  return tag;
}

s_tag * tag_new_s16 (s16 i)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_S16;
  tag->data.s16 = i;
  return tag;
}

s_tag * tag_new_s32 (s32 i)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_S32;
  tag->data.s32 = i;
  return tag;
}

s_tag * tag_new_s64 (s64 i)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_S64;
  tag->data.s64 = i;
  return tag;
}

s_tag * tag_new_str (char *p_free, uw size, const char *p)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_STR;
  if (! str_init(&tag->data.str, p_free, size, p)) {
    free(tag);
    return NULL;
  }
  return tag;
}

s_tag * tag_new_str_1 (char *p_free, const char *p)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_STR;
  if (! str_init_1(&tag->data.str, p_free, p)) {
    free(tag);
    return NULL;
  }
  return tag;
}

s_tag * tag_new_str_1_alloc (const char *p)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_STR;
  if (! str_init_1_alloc(&tag->data.str, p)) {
    free(tag);
    return NULL;
  }
  return tag;
}

s_tag * tag_new_str_alloc_copy (uw size, const char *p)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_STR;
  if (! str_init_alloc_copy(&tag->data.str, size, p)) {
    free(tag);
    return NULL;
  }
  return tag;
}

s_tag * tag_new_str_cast (p_sym *type, const s_tag *src)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_STR;
  if (! str_init_cast(&tag->data.str, type, src)) {
    free(tag);
    return NULL;
  }
  return tag;
}

s_tag * tag_new_str_concatenate (const s_str *a, const s_str *b)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_STR;
  if (! str_init_concatenate(&tag->data.str, a, b)) {
    free(tag);
    return NULL;
  }
  return tag;
}

s_tag * tag_new_str_concatenate_list (const s_list *src)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_STR;
  if (! str_init_concatenate_list(&tag->data.str, src)) {
    free(tag);
    return NULL;
  }
  return tag;
}

s_tag * tag_new_str_concatenate_plist (p_list *src)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_STR;
  if (! str_init_concatenate_plist(&tag->data.str, src)) {
    free(tag);
    return NULL;
  }
  return tag;
}

s_tag * tag_new_str_copy (const s_str *src)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_STR;
  if (! str_init_copy(&tag->data.str, src)) {
    free(tag);
    return NULL;
  }
  return tag;
}

s_tag * tag_new_str_empty (void)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_STR;
  if (! str_init_empty(&tag->data.str)) {
    free(tag);
    return NULL;
  }
  return tag;
}

s_tag * tag_new_str_inspect_buf (const s_buf *src)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_STR;
  if (! str_init_inspect_buf(&tag->data.str, src)) {
    free(tag);
    return NULL;
  }
  return tag;
}

s_tag * tag_new_str_inspect_str (const s_str *src)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_STR;
  if (! str_init_inspect_str(&tag->data.str, src)) {
    free(tag);
    return NULL;
  }
  return tag;
}

s_tag * tag_new_sw (sw i)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_SW;
  tag->data.sw = i;
  return tag;
}

s_tag * tag_new_tuple (uw count)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_TUPLE;
  if (! tuple_init(&tag->data.tuple, count)) {
    free(tag);
    return NULL;
  }
  return tag;
}

s_tag * tag_new_tuple_2 (s_tag *a, s_tag *b)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_TUPLE;
  if (! tuple_init_2(&tag->data.tuple, a, b)) {
    free(tag);
    return NULL;
  }
  return tag;
}

s_tag * tag_new_time_add (const s_time *a, const s_time *b)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_TIME;
  if (! time_init_add(&tag->data.time, a, b)) {
    free(tag);
    return NULL;
  }
  return tag;
}

s_tag * tag_new_time_now (void)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_TIME;
  if (! time_init_now(&tag->data.time)) {
    free(tag);
    return NULL;
  }
  return tag;
}

s_tag * tag_new_u8 (u8 i)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_U8;
  tag->data.u8 = i;
  return tag;
}

s_tag * tag_new_u16 (u16 i)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_U16;
  tag->data.u16 = i;
  return tag;
}

s_tag * tag_new_u32 (u32 i)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_U32;
  tag->data.u32 = i;
  return tag;
}

s_tag * tag_new_u64 (u64 i)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_U64;
  tag->data.u64 = i;
  return tag;
}

s_tag * tag_new_unquote_copy (s_unquote *unquote)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_UNQUOTE;
  if (! unquote_init_copy(&tag->data.unquote, unquote)) {
    free(tag);
    return NULL;
  }
  return tag;
}

s_tag * tag_new_uw (uw i)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_UW;
  tag->data.uw = i;
  return tag;
}

s_tag * tag_array (s_tag *tag, const s_sym *type, uw dimension,
                   const uw *dimensions)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_ARRAY;
  if (! array_init(&tmp.data.array, type, dimension, dimensions))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_array_copy (s_tag *tag, const s_array *a)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_ARRAY;
  if (! array_init_copy(&tmp.data.array, a))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_character (s_tag *tag, character c)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_CHARACTER;
  tmp.data.character = c;
  *tag = tmp;
  return tag;
}

s_tag * tag_f32 (s_tag *tag, f32 f)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_F32;
  tmp.data.f32 = f;
  *tag = tmp;
  return tag;
}

s_tag * tag_f64 (s_tag *tag, f64 f)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_F64;
  tmp.data.f64 = f;
  *tag = tmp;
  return tag;
}

s_tag * tag_ident (s_tag *tag, const s_ident *ident)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_IDENT;
  tmp.data.ident = *ident;
  *tag = tmp;
  return tag;
}

s_tag * tag_ident_1 (s_tag *tag, const char *p)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_IDENT;
  if (! ident_init_1(&tmp.data.ident, p))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_integer_1 (s_tag *tag, const char *p)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_INTEGER;
  if (! integer_init_1(&tmp.data.integer, p))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_integer_copy (s_tag *tag, const s_integer *i)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_INTEGER;
  if (! integer_init_copy(&tmp.data.integer, i))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_integer_zero (s_tag *tag)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_INTEGER;
  if (! integer_init_zero(&tmp.data.integer))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_map (s_tag *tag, uw count)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_MAP;
  if (! map_init(&tmp.data.map, count))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_map_1 (s_tag *tag, const char *p)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_MAP;
  if (! map_init_1(&tmp.data.map, p))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_map_from_lists (s_tag *tag, s_list *keys, s_list *values)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_MAP;
  if (! map_init_from_lists(&tmp.data.map, keys, values))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_pcall (s_tag *tag)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_PCALL;
  if (! pcall_init(&tmp.data.pcall))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_pcall_call_cast (s_tag *tag, const s_sym *type)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_PCALL;
  if (! pcall_init_call_cast(&tmp.data.pcall, type))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_pcall_copy (s_tag *tag, p_call *src)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_PCALL;
  if (! pcall_init_copy(&tmp.data.pcall, src))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_pcallable (s_tag *tag)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_PCALLABLE;
  if (! pcallable_init(&tmp.data.pcallable))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_pcallable_copy (s_tag *tag, p_callable *src)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_PCALLABLE;
  if (! pcallable_init_copy(&tmp.data.pcallable, src))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_pcomplex (s_tag *tag, p_complex c)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_PCOMPLEX;
  tmp.data.pcomplex = c;
  *tag = tmp;
  return tag;
}

s_tag * tag_plist (s_tag *tag, p_list plist)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_PLIST;
  tmp.data.plist = plist;
  *tag = tmp;
  return tag;
}

s_tag * tag_plist_1 (s_tag *tag, const char *p)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_PLIST;
  if (! plist_init_1(&tmp.data.plist, p))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_pointer (s_tag *tag, const s_sym *pointer_type,
                     const s_sym *target_type, void *p)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_POINTER;
  if (! pointer_init(&tmp.data.pointer, pointer_type, target_type, p))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_pstruct (s_tag *tag, const s_sym *module)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_PSTRUCT;
  if (! pstruct_init(&tmp.data.pstruct, module))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_pstruct_copy (s_tag *tag, p_struct *src)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_PSTRUCT;
  if (! pstruct_init_copy(&tmp.data.pstruct, src))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_pstruct_copy_data (s_tag *tag, const s_sym *module,
                               void *data)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_PSTRUCT;
  if (! pstruct_init_copy_data(&tmp.data.pstruct, module, data))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_pstruct_with_data (s_tag *tag, const s_sym *module,
                               void *data, bool free_data)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_PSTRUCT;
  if (! pstruct_init_with_data(&tmp.data.pstruct, module, data,
                               free_data))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_pstruct_with_type (s_tag *tag, s_struct_type *st)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_PSTRUCT;
  if (! pstruct_init_with_type(&tmp.data.pstruct, st))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_pstruct_type (s_tag *tag, const s_sym *module, s_list *spec)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_PSTRUCT_TYPE;
  if (! pstruct_type_init(&tmp.data.pstruct_type, module, spec))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_pstruct_type_clean (s_tag *tag, s_struct_type *st,
                                p_callable clean)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_PSTRUCT_TYPE;
  if (! pstruct_type_init_clean(&tmp.data.pstruct_type, st, clean))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_psym (s_tag *tag, const s_sym *sym)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_PSYM;
  tmp.data.psym = sym;
  *tag = tmp;
  return tag;
}

s_tag * tag_psym_anon (s_tag *tag, const s_str *src)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_PSYM;
  if (! psym_init_anon(&tmp.data.psym, src))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_ptr (s_tag *tag, void *p)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_PTR;
  if (! ptr_init(&tmp.data.ptr, p))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_ptr_free (s_tag *tag, void *p)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_PTR_FREE;
  if (! ptr_free_init(&tmp.data.ptr_free, p))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_pvar (s_tag *tag, const s_sym *type)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_PVAR;
  if (! pvar_init(&tmp.data.pvar, type))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_pvar_copy (s_tag *tag, p_var *src)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_PVAR;
  if (! pvar_init_copy(&tmp.data.pvar, src))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_quote (s_tag *tag, s_tag *src)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_QUOTE;
  if (! quote_init(&tmp.data.quote, src))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_quote_copy (s_tag *tag, s_quote *quote)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_QUOTE;
  if (! quote_init_copy(&tmp.data.quote, quote))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_ratio_1 (s_tag *tag, const char *p)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_RATIO;
  if (! ratio_init_1(&tmp.data.ratio, p))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_ratio (s_tag *tag)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_RATIO;
  if (! ratio_init(&tmp.data.ratio))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_ratio_copy (s_tag *tag, s_ratio *r)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_RATIO;
  if (! ratio_init_copy(&tmp.data.ratio, r))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_ratio_zero (s_tag *tag)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_RATIO;
  if (! ratio_init_zero(&tmp.data.ratio))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_s8 (s_tag *tag, s8 i)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_S8;
  tmp.data.s8 = i;
  *tag = tmp;
  return tag;
}

s_tag * tag_s16 (s_tag *tag, s16 i)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_S16;
  tmp.data.s16 = i;
  *tag = tmp;
  return tag;
}

s_tag * tag_s32 (s_tag *tag, s32 i)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_S32;
  tmp.data.s32 = i;
  *tag = tmp;
  return tag;
}

s_tag * tag_s64 (s_tag *tag, s64 i)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_S64;
  tmp.data.s64 = i;
  *tag = tmp;
  return tag;
}

s_tag * tag_str (s_tag *tag, char *p_free, uw size, const char *p)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_STR;
  if (! str_init(&tmp.data.str, p_free, size, p))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_str_1 (s_tag *tag, char *p_free, const char *p)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_STR;
  if (! str_init_1(&tmp.data.str, p_free, p))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_str_1_alloc (s_tag *tag, const char *p)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_STR;
  if (! str_init_1_alloc(&tmp.data.str, p))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_str_alloc_copy (s_tag *tag, uw size, const char *p)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_STR;
  if (! str_init_alloc_copy(&tmp.data.str, size, p))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_str_cast (s_tag *tag, p_sym *type, const s_tag *src)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_STR;
  if (! str_init_cast(&tmp.data.str, type, src))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_str_concatenate (s_tag *tag, const s_str *a, const s_str *b)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_STR;
  if (! str_init_concatenate(&tmp.data.str, a, b))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_str_concatenate_list (s_tag *tag, const s_list *src)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_STR;
  if (! str_init_concatenate_list(&tmp.data.str, src))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_str_concatenate_plist (s_tag *tag, p_list *src)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_STR;
  if (! str_init_concatenate_plist(&tmp.data.str, src))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_str_copy (s_tag *tag, const s_str *src)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_STR;
  if (! str_init_copy(&tmp.data.str, src))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_str_empty (s_tag *tag)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_STR;
  if (! str_init_empty(&tmp.data.str))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_str_inspect_buf (s_tag *tag, const s_buf *src)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_STR;
  if (! str_init_inspect_buf(&tmp.data.str, src))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_str_inspect_str (s_tag *tag, const s_str *src)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_STR;
  if (! str_init_inspect_str(&tmp.data.str, src))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_sw (s_tag *tag, sw i)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_SW;
  tmp.data.sw = i;
  *tag = tmp;
  return tag;
}

s_tag * tag_tuple (s_tag *tag, uw count)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_TUPLE;
  if (! tuple_init(&tmp.data.tuple, count))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_tuple_2 (s_tag *tag, s_tag *a, s_tag *b)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_TUPLE;
  if (! tuple_init_2(&tmp.data.tuple, a, b))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_time_add (s_tag *tag, const s_time *a, const s_time *b)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_TIME;
  if (! time_init_add(&tmp.data.time, a, b))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_time_now (s_tag *tag)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_TIME;
  if (! time_init_now(&tmp.data.time))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_u8 (s_tag *tag, u8 i)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_U8;
  tmp.data.u8 = i;
  *tag = tmp;
  return tag;
}

s_tag * tag_u16 (s_tag *tag, u16 i)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_U16;
  tmp.data.u16 = i;
  *tag = tmp;
  return tag;
}

s_tag * tag_u32 (s_tag *tag, u32 i)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_U32;
  tmp.data.u32 = i;
  *tag = tmp;
  return tag;
}

s_tag * tag_u64 (s_tag *tag, u64 i)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_U64;
  tmp.data.u64 = i;
  *tag = tmp;
  return tag;
}

s_tag * tag_unquote_copy (s_tag *tag, s_unquote *unquote)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_UNQUOTE;
  if (! unquote_init_copy(&tmp.data.unquote, unquote))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_uw (s_tag *tag, uw i)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_UW;
  tmp.data.uw = i;
  *tag = tmp;
  return tag;
}
