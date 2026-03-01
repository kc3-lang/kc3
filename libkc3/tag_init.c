/* kc3
 * Copyright from 2020 to 2025 kmx.io <contact@kmx.io>
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
#include "ptuple.h"
#include "pvar.h"
#include "quote.h"
#include "ratio.h"
#include "str.h"
#include "struct.h"
#include "struct_type.h"
#include "tag.h"
#include "tag_init.h"
#include "time.h"
#include "unquote.h"
#include "var.h"

s_tag * tag_init_array (s_tag *tag, const s_sym *type, uw dimension,
                        const uw *dimensions)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_ARRAY;
  if (! array_init(&tmp.data.td_array, type, dimension, dimensions))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_array_copy (s_tag *tag, const s_array *a)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_ARRAY;
  if (! array_init_copy(&tmp.data.td_array, a))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_character (s_tag *tag, character c)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_CHARACTER;
  tmp.data.td_character = c;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_f32 (s_tag *tag, f32 f)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_F32;
  tmp.data.td_f32 = f;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_f64 (s_tag *tag, f64 f)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_F64;
  tmp.data.td_f64 = f;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_ident (s_tag *tag, const s_ident *ident)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_IDENT;
  tmp.data.td_ident = *ident;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_ident_1 (s_tag *tag, const char *p)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_IDENT;
  if (! ident_init_1(&tmp.data.td_ident, p))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_integer_1 (s_tag *tag, const char *p)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_INTEGER;
  if (! integer_init_1(&tmp.data.td_integer, p))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_integer_copy (s_tag *tag, const s_integer *i)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_INTEGER;
  if (! integer_init_copy(&tmp.data.td_integer, i))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_integer_zero (s_tag *tag)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_INTEGER;
  if (! integer_init_zero(&tmp.data.td_integer))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_map (s_tag *tag, uw count)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_MAP;
  if (! map_init(&tmp.data.td_map, count))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_map_1 (s_tag *tag, const char *p)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_MAP;
  if (! map_init_1(&tmp.data.td_map, p))
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
  if (! map_init_from_lists(&tmp.data.td_map, keys, values))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_pcall (s_tag *tag)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_PCALL;
  if (! pcall_init(&tmp.data.td_pcall))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_pcall_call_cast (s_tag *tag, const s_sym *type)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_PCALL;
  if (! pcall_init_call_cast(&tmp.data.td_pcall, type))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_pcall_copy (s_tag *tag, p_call *src)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_PCALL;
  if (! pcall_init_copy(&tmp.data.td_pcall, src))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_pcallable (s_tag *tag)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_PCALLABLE;
  if (! pcallable_init(&tmp.data.td_pcallable))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_pcallable_copy (s_tag *tag, p_callable *src)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_PCALLABLE;
  if (! pcallable_init_copy(&tmp.data.td_pcallable, src))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_pcomplex (s_tag *tag, p_complex c)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_PCOMPLEX;
  tmp.data.td_pcomplex = c;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_plist (s_tag *tag, p_list plist)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_PLIST;
  tmp.data.td_plist = plist;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_plist_1 (s_tag *tag, const char *p)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_PLIST;
  if (! plist_init_1(&tmp.data.td_plist, p))
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
  if (! pointer_init(&tmp.data.td_pointer, pointer_type, target_type,
                     p))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_pstruct (s_tag *tag, const s_sym *module)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_PSTRUCT;
  if (! pstruct_init(&tmp.data.td_pstruct, module))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_pstruct_copy (s_tag *tag, p_struct *src)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_PSTRUCT;
  if (! pstruct_init_copy(&tmp.data.td_pstruct, src))
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
  if (! pstruct_init_copy_data(&tmp.data.td_pstruct, module, data))
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
  if (! pstruct_init_with_data(&tmp.data.td_pstruct, module, data,
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
  if (! pstruct_init_with_type(&tmp.data.td_pstruct, st))
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
  if (! pstruct_type_init(&tmp.data.td_pstruct_type, module, spec))
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
  if (! pstruct_type_init_clean(&tmp.data.td_pstruct_type, st, clean))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_psym (s_tag *tag, const s_sym *sym)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_PSYM;
  tmp.data.td_psym = sym;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_psym_anon (s_tag *tag, const s_str *src)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_PSYM;
  if (! psym_init_anon(&tmp.data.td_psym, src))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_ptr (s_tag *tag, void *p)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_PTR;
  if (! ptr_init(&tmp.data.td_ptr, p))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_ptr_free (s_tag *tag, void *p)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_PTR_FREE;
  if (! ptr_free_init(&tmp.data.td_ptr_free, p))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_pvar (s_tag *tag, const s_sym *type)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_PVAR;
  if (! pvar_init(&tmp.data.td_pvar, type))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_pvar_copy (s_tag *tag, p_var *src)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_PVAR;
  if (! pvar_init_copy(&tmp.data.td_pvar, src))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_quote (s_tag *tag, s_tag *src)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_QUOTE;
  if (! quote_init(&tmp.data.td_quote, src))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_quote_copy (s_tag *tag, s_quote *quote)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_QUOTE;
  if (! quote_init_copy(&tmp.data.td_quote, quote))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_ratio_1 (s_tag *tag, const char *p)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_RATIO;
  if (! ratio_init_1(&tmp.data.td_ratio, p))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_ratio (s_tag *tag)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_RATIO;
  if (! ratio_init(&tmp.data.td_ratio))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_ratio_copy (s_tag *tag, s_ratio *r)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_RATIO;
  if (! ratio_init_copy(&tmp.data.td_ratio, r))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_ratio_zero (s_tag *tag)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_RATIO;
  if (! ratio_init_zero(&tmp.data.td_ratio))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_s8 (s_tag *tag, s8 i)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_S8;
  tmp.data.td_s8 = i;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_s16 (s_tag *tag, s16 i)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_S16;
  tmp.data.td_s16 = i;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_s32 (s_tag *tag, s32 i)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_S32;
  tmp.data.td_s32 = i;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_s64 (s_tag *tag, s64 i)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_S64;
  tmp.data.td_s64 = i;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_str (s_tag *tag, char *p_free, uw size, const char *p)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_STR;
  if (! str_init(&tmp.data.td_str, p_free, size, p))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_str_1 (s_tag *tag, char *p_free, const char *p)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_STR;
  if (! str_init_1(&tmp.data.td_str, p_free, p))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_str_1_alloc (s_tag *tag, const char *p)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_STR;
  if (! str_init_1_alloc(&tmp.data.td_str, p))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_str_alloc (s_tag *tag, uw size)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_STR;
  if (! str_init_alloc(&tmp.data.td_str, size))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_str_alloc_copy (s_tag *tag, uw size, const char *p)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_STR;
  if (! str_init_alloc_copy(&tmp.data.td_str, size, p))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_str_cast (s_tag *tag, p_sym *type, const s_tag *src)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_STR;
  if (! str_init_cast(&tmp.data.td_str, type, src))
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
  if (! str_init_concatenate(&tmp.data.td_str, a, b))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_str_concatenate_list (s_tag *tag, const s_list *src)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_STR;
  if (! str_init_concatenate_list(&tmp.data.td_str, src))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_str_concatenate_plist (s_tag *tag, p_list *src)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_STR;
  if (! str_init_concatenate_plist(&tmp.data.td_str, src))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_str_copy (s_tag *tag, const s_str *src)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_STR;
  if (! str_init_copy(&tmp.data.td_str, src))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_str_empty (s_tag *tag)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_STR;
  if (! str_init_empty(&tmp.data.td_str))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_str_inspect_buf (s_tag *tag, const s_buf *src)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_STR;
  if (! str_init_inspect_buf(&tmp.data.td_str, src))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_str_inspect_str (s_tag *tag, const s_str *src)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_STR;
  if (! str_init_inspect_str(&tmp.data.td_str, src))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_sw (s_tag *tag, sw i)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_SW;
  tmp.data.td_sw = i;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_time_add (s_tag *tag, const s_time *a, const s_time *b)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_TIME;
  if (! time_init_add(&tmp.data.td_time, a, b))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_time_copy (s_tag *tag, const s_time *src)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_TIME;
  if (! time_init_copy(&tmp.data.td_time, src))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_time_now (s_tag *tag)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_TIME;
  if (! time_init_now(&tmp.data.td_time))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_ptuple (s_tag *tag, uw count)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_PTUPLE;
  if (! ptuple_init(&tmp.data.td_ptuple, count))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_ptuple_2 (s_tag *tag, s_tag *a, s_tag *b)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_PTUPLE;
  if (! ptuple_init_2(&tmp.data.td_ptuple, a, b))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_u8 (s_tag *tag, u8 i)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_U8;
  tmp.data.td_u8 = i;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_u16 (s_tag *tag, u16 i)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_U16;
  tmp.data.td_u16 = i;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_u32 (s_tag *tag, u32 i)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_U32;
  tmp.data.td_u32 = i;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_u64 (s_tag *tag, u64 i)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_U64;
  tmp.data.td_u64 = i;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_unquote_copy (s_tag *tag, s_unquote *unquote)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_UNQUOTE;
  if (! unquote_init_copy(&tmp.data.td_unquote, unquote))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_uw (s_tag *tag, uw i)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_UW;
  tmp.data.td_uw = i;
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
  if (! array_init(&tag->data.td_array, type, dimension, dimensions)) {
    alloc_free(tag);
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
  if (! array_init_copy(&tag->data.td_array, a)) {
    alloc_free(tag);
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
  tag->data.td_character = c;
  return tag;
}

s_tag * tag_new_f32 (f32 f)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_F32;
  tag->data.td_f32 = f;
  return tag;
}

s_tag * tag_new_f64 (f64 f)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_F64;
  tag->data.td_f64 = f;
  return tag;
}

s_tag * tag_new_ident (const s_ident *ident)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_IDENT;
  tag->data.td_ident = *ident;
  return tag;
}

s_tag * tag_new_ident_1 (const char *p)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_IDENT;
  if (! ident_init_1(&tag->data.td_ident, p)) {
    alloc_free(tag);
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
  if (! integer_init_1(&tag->data.td_integer, p)) {
    alloc_free(tag);
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
  if (! integer_init_copy(&tag->data.td_integer, i)) {
    alloc_free(tag);
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
  if (! integer_init_zero(&tag->data.td_integer)) {
    alloc_free(tag);
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
  if (! map_init(&tag->data.td_map, count)) {
    alloc_free(tag);
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
  if (! map_init_1(&tag->data.td_map, p)) {
    alloc_free(tag);
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
  if (! map_init_from_lists(&tag->data.td_map, keys, values)) {
    alloc_free(tag);
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
  if (! pcall_init(&tag->data.td_pcall)) {
    alloc_free(tag);
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
  if (! pcall_init_call_cast(&tag->data.td_pcall, type)) {
    alloc_free(tag);
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
  if (! pcall_init_copy(&tag->data.td_pcall, src)) {
    alloc_free(tag);
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
  if (! pcallable_init(&tag->data.td_pcallable)) {
    alloc_free(tag);
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
  if (! pcallable_init_copy(&tag->data.td_pcallable, src)) {
    alloc_free(tag);
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
  tag->data.td_pcomplex = c;
  return tag;
}

s_tag * tag_new_plist (p_list plist)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_PLIST;
  tag->data.td_plist = plist;
  return tag;
}

s_tag * tag_new_plist_1 (const char *p)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_PLIST;
  if (! plist_init_1(&tag->data.td_plist, p)) {
    alloc_free(tag);
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
  if (! pointer_init(&tag->data.td_pointer, pointer_type, target_type,
                     p)) {
    alloc_free(tag);
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
  if (! pstruct_init(&tag->data.td_pstruct, module)) {
    alloc_free(tag);
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
  if (! pstruct_init_copy(&tag->data.td_pstruct, src)) {
    alloc_free(tag);
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
  if (! pstruct_init_copy_data(&tag->data.td_pstruct, module, data)) {
    alloc_free(tag);
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
  if (! pstruct_init_with_data(&tag->data.td_pstruct, module, data,
                               free_data)) {
    alloc_free(tag);
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
  if (! pstruct_init_with_type(&tag->data.td_pstruct, st)) {
    alloc_free(tag);
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
  if (! pstruct_type_init(&tag->data.td_pstruct_type, module, spec)) {
    alloc_free(tag);
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
  if (! pstruct_type_init_clean(&tag->data.td_pstruct_type, st,
                                clean)) {
    alloc_free(tag);
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
  tag->data.td_psym = sym;
  return tag;
}

s_tag * tag_new_psym_anon (const s_str *src)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_PSYM;
  if (! psym_init_anon(&tag->data.td_psym, src)) {
    alloc_free(tag);
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
  if (! ptr_init(&tag->data.td_ptr, p)) {
    alloc_free(tag);
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
  if (! ptr_free_init(&tag->data.td_ptr_free, p)) {
    alloc_free(tag);
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
  if (! pvar_init(&tag->data.td_pvar, type)) {
    alloc_free(tag);
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
  if (! pvar_init_copy(&tag->data.td_pvar, src)) {
    alloc_free(tag);
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
  if (! quote_init(&tag->data.td_quote, src)) {
    alloc_free(tag);
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
  if (! quote_init_copy(&tag->data.td_quote, quote)) {
    alloc_free(tag);
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
  if (! ratio_init_1(&tag->data.td_ratio, p)) {
    alloc_free(tag);
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
  if (! ratio_init(&tag->data.td_ratio)) {
    alloc_free(tag);
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
  if (! ratio_init_copy(&tag->data.td_ratio, r)) {
    alloc_free(tag);
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
  if (! ratio_init_zero(&tag->data.td_ratio)) {
    alloc_free(tag);
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
  tag->data.td_s8 = i;
  return tag;
}

s_tag * tag_new_s16 (s16 i)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_S16;
  tag->data.td_s16 = i;
  return tag;
}

s_tag * tag_new_s32 (s32 i)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_S32;
  tag->data.td_s32 = i;
  return tag;
}

s_tag * tag_new_s64 (s64 i)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_S64;
  tag->data.td_s64 = i;
  return tag;
}

s_tag * tag_new_str (char *p_free, uw size, const char *p)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_STR;
  if (! str_init(&tag->data.td_str, p_free, size, p)) {
    alloc_free(tag);
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
  if (! str_init_1(&tag->data.td_str, p_free, p)) {
    alloc_free(tag);
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
  if (! str_init_1_alloc(&tag->data.td_str, p)) {
    alloc_free(tag);
    return NULL;
  }
  return tag;
}

s_tag * tag_new_str_alloc (uw size)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_STR;
  if (! str_init_alloc(&tag->data.td_str, size)) {
    alloc_free(tag);
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
  if (! str_init_alloc_copy(&tag->data.td_str, size, p)) {
    alloc_free(tag);
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
  if (! str_init_cast(&tag->data.td_str, type, src)) {
    alloc_free(tag);
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
  if (! str_init_concatenate(&tag->data.td_str, a, b)) {
    alloc_free(tag);
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
  if (! str_init_concatenate_list(&tag->data.td_str, src)) {
    alloc_free(tag);
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
  if (! str_init_concatenate_plist(&tag->data.td_str, src)) {
    alloc_free(tag);
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
  if (! str_init_copy(&tag->data.td_str, src)) {
    alloc_free(tag);
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
  if (! str_init_empty(&tag->data.td_str)) {
    alloc_free(tag);
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
  if (! str_init_inspect_buf(&tag->data.td_str, src)) {
    alloc_free(tag);
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
  if (! str_init_inspect_str(&tag->data.td_str, src)) {
    alloc_free(tag);
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
  tag->data.td_sw = i;
  return tag;
}

s_tag * tag_new_time_add (const s_time *a, const s_time *b)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_TIME;
  if (! time_init_add(&tag->data.td_time, a, b)) {
    alloc_free(tag);
    return NULL;
  }
  return tag;
}

s_tag * tag_new_time_copy (const s_time *src)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_TIME;
  if (! time_init_copy(&tag->data.td_time, src)) {
    alloc_free(tag);
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
  if (! time_init_now(&tag->data.td_time)) {
    alloc_free(tag);
    return NULL;
  }
  return tag;
}

s_tag * tag_new_ptuple (uw count)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_PTUPLE;
  if (! ptuple_init(&tag->data.td_ptuple, count)) {
    alloc_free(tag);
    return NULL;
  }
  return tag;
}

s_tag * tag_new_ptuple_2 (s_tag *a, s_tag *b)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_PTUPLE;
  if (! ptuple_init_2(&tag->data.td_ptuple, a, b)) {
    alloc_free(tag);
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
  tag->data.td_u8 = i;
  return tag;
}

s_tag * tag_new_u16 (u16 i)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_U16;
  tag->data.td_u16 = i;
  return tag;
}

s_tag * tag_new_u32 (u32 i)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_U32;
  tag->data.td_u32 = i;
  return tag;
}

s_tag * tag_new_u64 (u64 i)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_U64;
  tag->data.td_u64 = i;
  return tag;
}

s_tag * tag_new_unquote_copy (s_unquote *unquote)
{
  s_tag *tag;
  tag = alloc(sizeof(s_tag));
  if (! tag)
    return NULL;
  tag->type = TAG_UNQUOTE;
  if (! unquote_init_copy(&tag->data.td_unquote, unquote)) {
    alloc_free(tag);
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
  tag->data.td_uw = i;
  return tag;
}

s_tag * tag_array (s_tag *tag, const s_sym *type, uw dimension,
                   const uw *dimensions)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_ARRAY;
  if (! array_init(&tmp.data.td_array, type, dimension, dimensions))
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
  if (! array_init_copy(&tmp.data.td_array, a))
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
  tmp.data.td_character = c;
  *tag = tmp;
  return tag;
}

s_tag * tag_f32 (s_tag *tag, f32 f)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_F32;
  tmp.data.td_f32 = f;
  *tag = tmp;
  return tag;
}

s_tag * tag_f64 (s_tag *tag, f64 f)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_F64;
  tmp.data.td_f64 = f;
  *tag = tmp;
  return tag;
}

s_tag * tag_ident (s_tag *tag, const s_ident *ident)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_IDENT;
  tmp.data.td_ident = *ident;
  *tag = tmp;
  return tag;
}

s_tag * tag_ident_1 (s_tag *tag, const char *p)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_IDENT;
  if (! ident_init_1(&tmp.data.td_ident, p))
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
  if (! integer_init_1(&tmp.data.td_integer, p))
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
  if (! integer_init_copy(&tmp.data.td_integer, i))
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
  if (! integer_init_zero(&tmp.data.td_integer))
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
  if (! map_init(&tmp.data.td_map, count))
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
  if (! map_init_1(&tmp.data.td_map, p))
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
  if (! map_init_from_lists(&tmp.data.td_map, keys, values))
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
  if (! pcall_init(&tmp.data.td_pcall))
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
  if (! pcall_init_call_cast(&tmp.data.td_pcall, type))
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
  if (! pcall_init_copy(&tmp.data.td_pcall, src))
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
  if (! pcallable_init(&tmp.data.td_pcallable))
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
  if (! pcallable_init_copy(&tmp.data.td_pcallable, src))
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
  tmp.data.td_pcomplex = c;
  *tag = tmp;
  return tag;
}

s_tag * tag_plist (s_tag *tag, p_list plist)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_PLIST;
  tmp.data.td_plist = plist;
  *tag = tmp;
  return tag;
}

s_tag * tag_plist_1 (s_tag *tag, const char *p)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_PLIST;
  if (! plist_init_1(&tmp.data.td_plist, p))
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
  if (! pointer_init(&tmp.data.td_pointer, pointer_type, target_type,
                     p))
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
  if (! pstruct_init(&tmp.data.td_pstruct, module))
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
  if (! pstruct_init_copy(&tmp.data.td_pstruct, src))
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
  if (! pstruct_init_copy_data(&tmp.data.td_pstruct, module, data))
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
  if (! pstruct_init_with_data(&tmp.data.td_pstruct, module, data,
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
  if (! pstruct_init_with_type(&tmp.data.td_pstruct, st))
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
  if (! pstruct_type_init(&tmp.data.td_pstruct_type, module, spec))
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
  if (! pstruct_type_init_clean(&tmp.data.td_pstruct_type, st, clean))
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
  tmp.data.td_psym = sym;
  *tag = tmp;
  return tag;
}

s_tag * tag_psym_anon (s_tag *tag, const s_str *src)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_PSYM;
  if (! psym_init_anon(&tmp.data.td_psym, src))
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
  if (! ptr_init(&tmp.data.td_ptr, p))
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
  if (! ptr_free_init(&tmp.data.td_ptr_free, p))
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
  if (! pvar_init(&tmp.data.td_pvar, type))
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
  if (! pvar_init_copy(&tmp.data.td_pvar, src))
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
  if (! quote_init(&tmp.data.td_quote, src))
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
  if (! quote_init_copy(&tmp.data.td_quote, quote))
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
  if (! ratio_init_1(&tmp.data.td_ratio, p))
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
  if (! ratio_init(&tmp.data.td_ratio))
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
  if (! ratio_init_copy(&tmp.data.td_ratio, r))
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
  if (! ratio_init_zero(&tmp.data.td_ratio))
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
  tmp.data.td_s8 = i;
  *tag = tmp;
  return tag;
}

s_tag * tag_s16 (s_tag *tag, s16 i)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_S16;
  tmp.data.td_s16 = i;
  *tag = tmp;
  return tag;
}

s_tag * tag_s32 (s_tag *tag, s32 i)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_S32;
  tmp.data.td_s32 = i;
  *tag = tmp;
  return tag;
}

s_tag * tag_s64 (s_tag *tag, s64 i)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_S64;
  tmp.data.td_s64 = i;
  *tag = tmp;
  return tag;
}

s_tag * tag_str (s_tag *tag, char *p_free, uw size, const char *p)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_STR;
  if (! str_init(&tmp.data.td_str, p_free, size, p))
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
  if (! str_init_1(&tmp.data.td_str, p_free, p))
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
  if (! str_init_1_alloc(&tmp.data.td_str, p))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_str_alloc (s_tag *tag, uw size)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_STR;
  if (! str_init_alloc(&tmp.data.td_str, size))
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
  if (! str_init_alloc_copy(&tmp.data.td_str, size, p))
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
  if (! str_init_cast(&tmp.data.td_str, type, src))
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
  if (! str_init_concatenate(&tmp.data.td_str, a, b))
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
  if (! str_init_concatenate_list(&tmp.data.td_str, src))
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
  if (! str_init_concatenate_plist(&tmp.data.td_str, src))
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
  if (! str_init_copy(&tmp.data.td_str, src))
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
  if (! str_init_empty(&tmp.data.td_str))
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
  if (! str_init_inspect_buf(&tmp.data.td_str, src))
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
  if (! str_init_inspect_str(&tmp.data.td_str, src))
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
  tmp.data.td_sw = i;
  *tag = tmp;
  return tag;
}

s_tag * tag_time_add (s_tag *tag, const s_time *a, const s_time *b)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_TIME;
  if (! time_init_add(&tmp.data.td_time, a, b))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_time_copy (s_tag *tag, const s_time *src)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_TIME;
  if (! time_init_copy(&tmp.data.td_time, src))
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
  if (! time_init_now(&tmp.data.td_time))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_ptuple (s_tag *tag, uw count)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_PTUPLE;
  if (! ptuple_init(&tmp.data.td_ptuple, count))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_ptuple_2 (s_tag *tag, s_tag *a, s_tag *b)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_PTUPLE;
  if (! ptuple_init_2(&tmp.data.td_ptuple, a, b))
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
  tmp.data.td_u8 = i;
  *tag = tmp;
  return tag;
}

s_tag * tag_u16 (s_tag *tag, u16 i)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_U16;
  tmp.data.td_u16 = i;
  *tag = tmp;
  return tag;
}

s_tag * tag_u32 (s_tag *tag, u32 i)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_U32;
  tmp.data.td_u32 = i;
  *tag = tmp;
  return tag;
}

s_tag * tag_u64 (s_tag *tag, u64 i)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_U64;
  tmp.data.td_u64 = i;
  *tag = tmp;
  return tag;
}

s_tag * tag_unquote_copy (s_tag *tag, s_unquote *unquote)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_UNQUOTE;
  if (! unquote_init_copy(&tmp.data.td_unquote, unquote))
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
  tmp.data.td_uw = i;
  *tag = tmp;
  return tag;
}
