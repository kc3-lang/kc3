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
#include <string.h>
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
#include "pcallable.h"
#include "pointer.h"
#include "pstruct.h"
#include "pstruct_type.h"
#include "ptr.h"
#include "ptr_free.h"
#include "ptuple.h"
#include "pvar.h"
#include "quote.h"
#include "ratio.h"
#include "str.h"
#include "struct.h"
#include "tag.h"
#include "list_init.h"
#include "time.h"

s_list * list_init_array (s_list *list, const s_sym *type,
                          uw dimension, const uw *dimensions,
                          s_list *next)
{
  s_list tmp = {0};
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_array(&tmp.tag, type, dimension, dimensions)) {
    err_puts("list_init_array: tag_init_array");
    assert(! "list_init_array: tag_init_array");
    return NULL;
  }
  *list = tmp;
  return list;
}

s_list * list_init_array_copy (s_list *list, const s_array *a,
                               s_list *next)
{
  s_list tmp = {0};
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_array_copy(&tmp.tag, a)) {
    err_puts("list_init_array_copy: tag_init_array_copy");
    assert(! "list_init_array_copy: tag_init_array_copy");
    return NULL;
  }
  *list = tmp;
  return list;
}

s_list * list_init_character (s_list *list, character c, s_list *next)
{
  s_list tmp = {0};
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_character(&tmp.tag, c)) {
    err_puts("list_init_character: tag_init_character");
    assert(! "list_init_character: tag_init_character");
    return NULL;
  }
  *list = tmp;
  return list;
}

s_list * list_init_f32 (s_list *list, f32 f, s_list *next)
{
  s_list tmp = {0};
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_f32(&tmp.tag, f)) {
    err_puts("list_init_f32: tag_init_f32");
    assert(! "list_init_f32: tag_init_f32");
    return NULL;
  }
  *list = tmp;
  return list;
}

s_list * list_init_f64 (s_list *list, f64 f, s_list *next)
{
  s_list tmp = {0};
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_f64(&tmp.tag, f)) {
    err_puts("list_init_f64: tag_init_f64");
    assert(! "list_init_f64: tag_init_f64");
    return NULL;
  }
  *list = tmp;
  return list;
}

s_list * list_init_ident (s_list *list, const s_ident *ident,
                          s_list *next)
{
  s_list tmp = {0};
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_ident(&tmp.tag, ident)) {
    err_puts("list_init_ident: tag_init_ident");
    assert(! "list_init_ident: tag_init_ident");
    return NULL;
  }
  *list = tmp;
  return list;
}

s_list * list_init_ident_1 (s_list *list, const char *p, s_list *next)
{
  s_list tmp = {0};
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_ident_1(&tmp.tag, p)) {
    err_puts("list_init_ident_1: tag_init_ident_1");
    assert(! "list_init_ident_1: tag_init_ident_1");
    return NULL;
  }
  *list = tmp;
  return list;
}

s_list * list_init_integer_1 (s_list *list, const char *p, s_list *next)
{
  s_list tmp = {0};
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_integer_1(&tmp.tag, p)) {
    err_puts("list_init_integer_1: tag_init_integer_1");
    assert(! "list_init_integer_1: tag_init_integer_1");
    return NULL;
  }
  *list = tmp;
  return list;
}

s_list * list_init_integer_copy (s_list *list, const s_integer *i,
                                 s_list *next)
{
  s_list tmp = {0};
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_integer_copy(&tmp.tag, i)) {
    err_puts("list_init_integer_copy: tag_init_integer_copy");
    assert(! "list_init_integer_copy: tag_init_integer_copy");
    return NULL;
  }
  *list = tmp;
  return list;
}

s_list * list_init_integer_zero (s_list *list, s_list *next)
{
  s_list tmp = {0};
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_integer_zero(&tmp.tag)) {
    err_puts("list_init_integer_zero: tag_init_integer_zero");
    assert(! "list_init_integer_zero: tag_init_integer_zero");
    return NULL;
  }
  *list = tmp;
  return list;
}

s_list * list_init_map (s_list *list, uw count, s_list *next)
{
  s_list tmp = {0};
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_map(&tmp.tag, count)) {
    err_puts("list_init_map: tag_init_map");
    assert(! "list_init_map: tag_init_map");
    return NULL;
  }
  *list = tmp;
  return list;
}

s_list * list_init_map_1 (s_list *list, const char *p, s_list *next)
{
  s_list tmp = {0};
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_map_1(&tmp.tag, p)) {
    err_puts("list_init_map_1: tag_init_map_1");
    assert(! "list_init_map_1: tag_init_map_1");
    return NULL;
  }
  *list = tmp;
  return list;
}

s_list * list_init_map_from_lists (s_list *list, s_list *keys,
                                   s_list *values, s_list *next)
{
  s_list tmp = {0};
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_map_from_lists(&tmp.tag, keys, values)) {
    err_puts("list_init_map_from_lists: tag_init_map_from_lists");
    assert(! "list_init_map_from_lists: tag_init_map_from_lists");
    return NULL;
  }
  *list = tmp;
  return list;
}

s_list * list_init_pcall (s_list *list, s_list *next)
{
  s_list tmp = {0};
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_pcall(&tmp.tag)) {
    err_puts("list_init_pcall: tag_init_pcall");
    assert(! "list_init_pcall: tag_init_pcall");
    return NULL;
  }
  *list = tmp;
  return list;
}

s_list * list_init_pcall_call_cast (s_list *list, const s_sym *type,
                                    s_list *next)
{
  s_list tmp = {0};
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_pcall_call_cast(&tmp.tag, type)) {
    err_puts("list_init_pcall_call_cast: tag_init_pcall_call_cast");
    assert(! "list_init_pcall_call_cast: tag_init_pcall_call_cast");
    return NULL;
  }
  *list = tmp;
  return list;
}

s_list * list_init_pcall_copy (s_list *list, p_call *src, s_list *next)
{
  s_list tmp = {0};
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_pcall_copy(&tmp.tag, src)) {
    err_puts("list_init_pcall_copy: tag_init_pcall_copy");
    assert(! "list_init_pcall_copy: tag_init_pcall_copy");
    return NULL;
  }
  *list = tmp;
  return list;
}

s_list * list_init_pcallable (s_list *list, s_list *next)
{
  s_list tmp = {0};
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_pcallable(&tmp.tag)) {
    err_puts("list_init_pcallable: tag_init_pcallable");
    assert(! "list_init_pcallable: tag_init_pcallable");
    return NULL;
  }
  *list = tmp;
  return list;
}

s_list * list_init_pcallable_copy (s_list *list, p_callable *src,
                                   s_list *next)
{
  s_list tmp = {0};
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_pcallable_copy(&tmp.tag, src)) {
    err_puts("list_init_pcallable_copy: tag_init_pcallable_copy");
    assert(! "list_init_pcallable_copy: tag_init_pcallable_copy");
    return NULL;
  }
  *list = tmp;
  return list;
}

s_list * list_init_pcomplex (s_list *list, p_complex c, s_list *next)
{
  s_list tmp = {0};
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_pcomplex(&tmp.tag, c)) {
    err_puts("list_init_pcomplex: tag_init_pcomplex");
    assert(! "list_init_pcomplex: tag_init_pcomplex");
    return NULL;
  }
  *list = tmp;
  return list;
}

s_list * list_init_plist (s_list *list, p_list plist, s_list *next)
{
  s_list tmp = {0};
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_plist(&tmp.tag, plist)) {
    err_puts("list_init_plist: tag_init_plist");
    assert(! "list_init_plist: tag_init_plist");
    return NULL;
  }
  *list = tmp;
  return list;
}

s_list * list_init_plist_1 (s_list *list, const char *p, s_list *next)
{
  s_list tmp = {0};
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_plist_1(&tmp.tag, p)) {
    err_puts("list_init_plist_1: tag_init_plist_1");
    assert(! "list_init_plist_1: tag_init_plist_1");
    return NULL;
  }
  *list = tmp;
  return list;
}

s_list * list_init_pointer (s_list *list, const s_sym *pointer_type,
                            const s_sym *target_type, void *p,
                            s_list *next)
{
  s_list tmp = {0};
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_pointer(&tmp.tag, pointer_type, target_type, p)) {
    err_puts("list_init_pointer: tag_init_pointer");
    assert(! "list_init_pointer: tag_init_pointer");
    return NULL;
  }
  *list = tmp;
  return list;
}

s_list * list_init_pstruct (s_list *list, const s_sym *module,
                            s_list *next)
{
  s_list tmp = {0};
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_pstruct(&tmp.tag, module)) {
    err_puts("list_init_pstruct: tag_init_pstruct");
    assert(! "list_init_pstruct: tag_init_pstruct");
    return NULL;
  }
  *list = tmp;
  return list;
}

s_list * list_init_pstruct_copy (s_list *list, p_struct *src,
                                 s_list *next)
{
  s_list tmp = {0};
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_pstruct_copy(&tmp.tag, src)) {
    err_puts("list_init_pstruct_copy: tag_init_pstruct_copy");
    assert(! "list_init_pstruct_copy: tag_init_pstruct_copy");
    return NULL;
  }
  *list = tmp;
  return list;
}

s_list * list_init_pstruct_copy_data (s_list *list,
                                      const s_sym *module, void *data,
                                      s_list *next)
{
  s_list tmp = {0};
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_pstruct_copy_data(&tmp.tag, module, data)) {
    err_puts("list_init_pstruct_copy_data: tag_init_pstruct_copy_data");
    assert(! "list_init_pstruct_copy_data: tag_init_pstruct_copy_data");
    return NULL;
  }
  *list = tmp;
  return list;
}

s_list * list_init_pstruct_with_data (s_list *list,
                                      const s_sym *module, void *data,
                                      bool free_data, s_list *next)
{
  s_list tmp = {0};
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_pstruct_with_data(&tmp.tag, module, data, free_data)) {
    err_puts("list_init_pstruct_with_data: tag_init_pstruct_with_data");
    assert(! "list_init_pstruct_with_data: tag_init_pstruct_with_data");
    return NULL;
  }
  *list = tmp;
  return list;
}

s_list * list_init_pstruct_with_type (s_list *list, s_struct_type *st,
                                      s_list *next)
{
  s_list tmp = {0};
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_pstruct_with_type(&tmp.tag, st)) {
    err_puts("list_init_pstruct_with_type: tag_init_pstruct_with_type");
    assert(! "list_init_pstruct_with_type: tag_init_pstruct_with_type");
    return NULL;
  }
  *list = tmp;
  return list;
}

s_list * list_init_pstruct_type (s_list *list, const s_sym *module,
                                 s_list *spec, s_list *next)
{
  s_list tmp = {0};
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_pstruct_type(&tmp.tag, module, spec)) {
    err_puts("list_init_pstruct_type: tag_init_pstruct_type");
    assert(! "list_init_pstruct_type: tag_init_pstruct_type");
    return NULL;
  }
  *list = tmp;
  return list;
}

s_list * list_init_pstruct_type_clean (s_list *list, s_struct_type *st,
                                       p_callable clean, s_list *next)
{
  s_list tmp = {0};
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_pstruct_type_clean(&tmp.tag, st, clean)) {
    err_puts("list_init_pstruct_type_clean: tag_init_pstruct_type_clean");
    assert(! "list_init_pstruct_type_clean: tag_init_pstruct_type_clean");
    return NULL;
  }
  *list = tmp;
  return list;
}

s_list * list_init_psym (s_list *list, const s_sym *sym, s_list *next)
{
  s_list tmp = {0};
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_psym(&tmp.tag, sym)) {
    err_puts("list_init_psym: tag_init_psym");
    assert(! "list_init_psym: tag_init_psym");
    return NULL;
  }
  *list = tmp;
  return list;
}

s_list * list_init_psym_anon (s_list *list, const s_str *src,
                              s_list *next)
{
  s_list tmp = {0};
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_psym_anon(&tmp.tag, src)) {
    err_puts("list_init_psym_anon: tag_init_psym_anon");
    assert(! "list_init_psym_anon: tag_init_psym_anon");
    return NULL;
  }
  *list = tmp;
  return list;
}

s_list * list_init_ptr (s_list *list, void *p, s_list *next)
{
  s_list tmp = {0};
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_ptr(&tmp.tag, p)) {
    err_puts("list_init_ptr: tag_init_ptr");
    assert(! "list_init_ptr: tag_init_ptr");
    return NULL;
  }
  *list = tmp;
  return list;
}

s_list * list_init_ptr_free (s_list *list, void *p, s_list *next)
{
  s_list tmp = {0};
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_ptr_free(&tmp.tag, p)) {
    err_puts("list_init_ptr_free: tag_init_ptr_free");
    assert(! "list_init_ptr_free: tag_init_ptr_free");
    return NULL;
  }
  *list = tmp;
  return list;
}

s_list * list_init_pvar (s_list *list, const s_sym *type, s_list *next)
{
  s_list tmp = {0};
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_pvar(&tmp.tag, type)) {
    err_puts("list_init_pvar: tag_init_pvar");
    assert(! "list_init_pvar: tag_init_pvar");
    return NULL;
  }
  *list = tmp;
  return list;
}

s_list * list_init_pvar_copy (s_list *list, p_var *src, s_list *next)
{
  s_list tmp = {0};
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_pvar_copy(&tmp.tag, src)) {
    err_puts("list_init_pvar_copy: tag_init_pvar_copy");
    assert(! "list_init_pvar_copy: tag_init_pvar_copy");
    return NULL;
  }
  *list = tmp;
  return list;
}

s_list * list_init_quote (s_list *list, s_tag *src, s_list *next)
{
  s_list tmp = {0};
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_quote(&tmp.tag, src)) {
    err_puts("list_init_quote: tag_init_quote");
    assert(! "list_init_quote: tag_init_quote");
    return NULL;
  }
  *list = tmp;
  return list;
}

s_list * list_init_quote_copy (s_list *list, s_quote *quote,
                               s_list *next)
{
  s_list tmp = {0};
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_quote_copy(&tmp.tag, quote)) {
    err_puts("list_init_quote_copy: tag_init_quote_copy");
    assert(! "list_init_quote_copy: tag_init_quote_copy");
    return NULL;
  }
  *list = tmp;
  return list;
}

s_list * list_init_ratio_1 (s_list *list, const char *p, s_list *next)
{
  s_list tmp = {0};
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_ratio_1(&tmp.tag, p)) {
    err_puts("list_init_ratio_1: tag_init_ratio_1");
    assert(! "list_init_ratio_1: tag_init_ratio_1");
    return NULL;
  }
  *list = tmp;
  return list;
}

s_list * list_init_ratio (s_list *list, s_list *next)
{
  s_list tmp = {0};
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_ratio(&tmp.tag)) {
    err_puts("list_init_ratio: tag_init_ratio");
    assert(! "list_init_ratio: tag_init_ratio");
    return NULL;
  }
  *list = tmp;
  return list;
}

s_list * list_init_ratio_copy (s_list *list, s_ratio *r, s_list *next)
{
  s_list tmp = {0};
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_ratio_copy(&tmp.tag, r)) {
    err_puts("list_init_ratio_copy: tag_init_ratio_copy");
    assert(! "list_init_ratio_copy: tag_init_ratio_copy");
    return NULL;
  }
  *list = tmp;
  return list;
}

s_list * list_init_ratio_zero (s_list *list, s_list *next)
{
  s_list tmp = {0};
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_ratio_zero(&tmp.tag)) {
    err_puts("list_init_ratio_zero: tag_init_ratio_zero");
    assert(! "list_init_ratio_zero: tag_init_ratio_zero");
    return NULL;
  }
  *list = tmp;
  return list;
}

s_list * list_init_s8 (s_list *list, s8 i, s_list *next)
{
  s_list tmp = {0};
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_s8(&tmp.tag, i)) {
    err_puts("list_init_s8: tag_init_s8");
    assert(! "list_init_s8: tag_init_s8");
    return NULL;
  }
  *list = tmp;
  return list;
}

s_list * list_init_s16 (s_list *list, s16 i, s_list *next)
{
  s_list tmp = {0};
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_s16(&tmp.tag, i)) {
    err_puts("list_init_s16: tag_init_s16");
    assert(! "list_init_s16: tag_init_s16");
    return NULL;
  }
  *list = tmp;
  return list;
}

s_list * list_init_s32 (s_list *list, s32 i, s_list *next)
{
  s_list tmp = {0};
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_s32(&tmp.tag, i)) {
    err_puts("list_init_s32: tag_init_s32");
    assert(! "list_init_s32: tag_init_s32");
    return NULL;
  }
  *list = tmp;
  return list;
}

s_list * list_init_s64 (s_list *list, s64 i, s_list *next)
{
  s_list tmp = {0};
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_s64(&tmp.tag, i)) {
    err_puts("list_init_s64: tag_init_s64");
    assert(! "list_init_s64: tag_init_s64");
    return NULL;
  }
  *list = tmp;
  return list;
}

s_list * list_init_str (s_list *list, char *p_free, uw size,
                        const char *p, s_list *next)
{
  s_list tmp = {0};
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_str(&tmp.tag, p_free, size, p)) {
    err_puts("list_init_str: tag_init_str");
    assert(! "list_init_str: tag_init_str");
    return NULL;
  }
  *list = tmp;
  return list;
}

s_list * list_init_str_1 (s_list *list, char *p_free, const char *p,
                          s_list *next)
{
  s_list tmp = {0};
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_str_1(&tmp.tag, p_free, p)) {
    err_puts("list_init_str_1: tag_init_str_1");
    assert(! "list_init_str_1: tag_init_str_1");
    return NULL;
  }
  *list = tmp;
  return list;
}

s_list * list_init_str_1_alloc (s_list *list, const char *p,
                                s_list *next)
{
  s_list tmp = {0};
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_str_1_alloc(&tmp.tag, p)) {
    err_puts("list_init_str_1_alloc: tag_init_str_1_alloc");
    assert(! "list_init_str_1_alloc: tag_init_str_1_alloc");
    return NULL;
  }
  *list = tmp;
  return list;
}

s_list * list_init_str_alloc (s_list *list, uw size, s_list *next)
{
  s_list tmp = {0};
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_str_alloc(&tmp.tag, size)) {
    err_puts("list_init_str_alloc: tag_init_str_alloc");
    assert(! "list_init_str_alloc: tag_init_str_alloc");
    return NULL;
  }
  *list = tmp;
  return list;
}

s_list * list_init_str_alloc_copy (s_list *list, uw size,
                                   const char *p, s_list *next)
{
  s_list tmp = {0};
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_str_alloc_copy(&tmp.tag, size, p)) {
    err_puts("list_init_str_alloc_copy: tag_init_str_alloc_copy");
    assert(! "list_init_str_alloc_copy: tag_init_str_alloc_copy");
    return NULL;
  }
  *list = tmp;
  return list;
}

s_list * list_init_str_cast (s_list *list, p_sym *type,
                             const s_tag *src, s_list *next)
{
  s_list tmp = {0};
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_str_cast(&tmp.tag, type, src)) {
    err_puts("list_init_str_cast: tag_init_str_cast");
    assert(! "list_init_str_cast: tag_init_str_cast");
    return NULL;
  }
  *list = tmp;
  return list;
}

s_list * list_init_str_concatenate (s_list *list, const s_str *a,
                                    const s_str *b, s_list *next)
{
  s_list tmp = {0};
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_str_concatenate(&tmp.tag, a, b)) {
    err_puts("list_init_str_concatenate: tag_init_str_concatenate");
    assert(! "list_init_str_concatenate: tag_init_str_concatenate");
    return NULL;
  }
  *list = tmp;
  return list;
}

s_list * list_init_str_concatenate_list (s_list *list,
                                         const s_list *src,
                                         s_list *next)
{
  s_list tmp = {0};
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_str_concatenate_list(&tmp.tag, src)) {
    err_puts("list_init_str_concatenate_list: tag_init_str_concatenate_list");
    assert(! "list_init_str_concatenate_list: tag_init_str_concatenate_list");
    return NULL;
  }
  *list = tmp;
  return list;
}

s_list * list_init_str_concatenate_plist (s_list *list, p_list *src,
                                          s_list *next)
{
  s_list tmp = {0};
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_str_concatenate_plist(&tmp.tag, src)) {
    err_puts("list_init_str_concatenate_plist: tag_init_str_concatenate_plist");
    assert(! "list_init_str_concatenate_plist: tag_init_str_concatenate_plist");
    return NULL;
  }
  *list = tmp;
  return list;
}

s_list * list_init_str_copy (s_list *list, const s_str *src,
                             s_list *next)
{
  s_list tmp = {0};
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_str_copy(&tmp.tag, src)) {
    err_puts("list_init_str_copy: tag_init_str_copy");
    assert(! "list_init_str_copy: tag_init_str_copy");
    return NULL;
  }
  *list = tmp;
  return list;
}

s_list * list_init_str_empty (s_list *list, s_list *next)
{
  s_list tmp = {0};
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_str_empty(&tmp.tag)) {
    err_puts("list_init_str_empty: tag_init_str_empty");
    assert(! "list_init_str_empty: tag_init_str_empty");
    return NULL;
  }
  *list = tmp;
  return list;
}

s_list * list_init_str_inspect_buf (s_list *list, const s_buf *src,
                                    s_list *next)
{
  s_list tmp = {0};
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_str_inspect_buf(&tmp.tag, src)) {
    err_puts("list_init_str_inspect_buf: tag_init_str_inspect_buf");
    assert(! "list_init_str_inspect_buf: tag_init_str_inspect_buf");
    return NULL;
  }
  *list = tmp;
  return list;
}

s_list * list_init_str_inspect_str (s_list *list, const s_str *src,
                                    s_list *next)
{
  s_list tmp = {0};
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_str_inspect_str(&tmp.tag, src)) {
    err_puts("list_init_str_inspect_str: tag_init_str_inspect_str");
    assert(! "list_init_str_inspect_str: tag_init_str_inspect_str");
    return NULL;
  }
  *list = tmp;
  return list;
}

s_list * list_init_sw (s_list *list, sw i, s_list *next)
{
  s_list tmp = {0};
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_sw(&tmp.tag, i)) {
    err_puts("list_init_sw: tag_init_sw");
    assert(! "list_init_sw: tag_init_sw");
    return NULL;
  }
  *list = tmp;
  return list;
}

s_list * list_init_time_add (s_list *list, const s_time *a,
                             const s_time *b, s_list *next)
{
  s_list tmp = {0};
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_time_add(&tmp.tag, a, b)) {
    err_puts("list_init_time_add: tag_init_time_add");
    assert(! "list_init_time_add: tag_init_time_add");
    return NULL;
  }
  *list = tmp;
  return list;
}

s_list * list_init_time_copy (s_list *list, const s_time *src,
                              s_list *next)
{
  s_list tmp = {0};
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_time_copy(&tmp.tag, src)) {
    err_puts("list_init_time_copy: tag_init_time_copy");
    assert(! "list_init_time_copy: tag_init_time_copy");
    return NULL;
  }
  *list = tmp;
  return list;
}

s_list * list_init_time_now (s_list *list, s_list *next)
{
  s_list tmp = {0};
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_time_now(&tmp.tag)) {
    err_puts("list_init_time_now: tag_init_time_now");
    assert(! "list_init_time_now: tag_init_time_now");
    return NULL;
  }
  *list = tmp;
  return list;
}

s_list * list_init_ptuple (s_list *list, uw count, s_list *next)
{
  s_list tmp = {0};
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_ptuple(&tmp.tag, count)) {
    err_puts("list_init_ptuple: tag_init_ptuple");
    assert(! "list_init_ptuple: tag_init_ptuple");
    return NULL;
  }
  *list = tmp;
  return list;
}

s_list * list_init_ptuple_2 (s_list *list, s_tag *a, s_tag *b,
                             s_list *next)
{
  s_list tmp = {0};
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_ptuple_2(&tmp.tag, a, b)) {
    err_puts("list_init_ptuple_2: tag_init_ptuple_2");
    assert(! "list_init_ptuple_2: tag_init_ptuple_2");
    return NULL;
  }
  *list = tmp;
  return list;
}

s_list * list_init_u8 (s_list *list, u8 i, s_list *next)
{
  s_list tmp = {0};
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_u8(&tmp.tag, i)) {
    err_puts("list_init_u8: tag_init_u8");
    assert(! "list_init_u8: tag_init_u8");
    return NULL;
  }
  *list = tmp;
  return list;
}

s_list * list_init_u16 (s_list *list, u16 i, s_list *next)
{
  s_list tmp = {0};
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_u16(&tmp.tag, i)) {
    err_puts("list_init_u16: tag_init_u16");
    assert(! "list_init_u16: tag_init_u16");
    return NULL;
  }
  *list = tmp;
  return list;
}

s_list * list_init_u32 (s_list *list, u32 i, s_list *next)
{
  s_list tmp = {0};
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_u32(&tmp.tag, i)) {
    err_puts("list_init_u32: tag_init_u32");
    assert(! "list_init_u32: tag_init_u32");
    return NULL;
  }
  *list = tmp;
  return list;
}

s_list * list_init_u64 (s_list *list, u64 i, s_list *next)
{
  s_list tmp = {0};
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_u64(&tmp.tag, i)) {
    err_puts("list_init_u64: tag_init_u64");
    assert(! "list_init_u64: tag_init_u64");
    return NULL;
  }
  *list = tmp;
  return list;
}

s_list * list_init_unquote_copy (s_list *list, s_unquote *unquote,
                                 s_list *next)
{
  s_list tmp = {0};
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_unquote_copy(&tmp.tag, unquote)) {
    err_puts("list_init_unquote_copy: tag_init_unquote_copy");
    assert(! "list_init_unquote_copy: tag_init_unquote_copy");
    return NULL;
  }
  *list = tmp;
  return list;
}

s_list * list_init_uw (s_list *list, uw i, s_list *next)
{
  s_list tmp = {0};
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_uw(&tmp.tag, i)) {
    err_puts("list_init_uw: tag_init_uw");
    assert(! "list_init_uw: tag_init_uw");
    return NULL;
  }
  *list = tmp;
  return list;
}

s_list * list_new_array (const s_sym *type, uw dimension,
                         const uw *dimensions, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list) {
    err_puts("list_new_array: list_new");
    assert(! "list_new_array: list_new");
    return NULL;
  }
  if (! tag_init_array(&list->tag, type, dimension, dimensions)) {
    err_puts("list_new_array: tag_init_array");
    assert(! "list_new_array: tag_init_array");
    alloc_free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_array_copy (const s_array *a, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list) {
    err_puts("list_new_array_copy: list_new");
    assert(! "list_new_array_copy: list_new");
    return NULL;
  }
  if (! tag_init_array_copy(&list->tag, a)) {
    err_puts("list_new_array_copy: tag_init_array_copy");
    assert(! "list_new_array_copy: tag_init_array_copy");
    alloc_free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_character (character c, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list) {
    err_puts("list_new_character: list_new");
    assert(! "list_new_character: list_new");
    return NULL;
  }
  if (! tag_init_character(&list->tag, c)) {
    err_puts("list_new_character: tag_init_character");
    assert(! "list_new_character: tag_init_character");
    alloc_free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_f32 (f32 f, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list) {
    err_puts("list_new_f32: list_new");
    assert(! "list_new_f32: list_new");
    return NULL;
  }
  if (! tag_init_f32(&list->tag, f)) {
    err_puts("list_new_f32: tag_init_f32");
    assert(! "list_new_f32: tag_init_f32");
    alloc_free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_f64 (f64 f, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list) {
    err_puts("list_new_f64: list_new");
    assert(! "list_new_f64: list_new");
    return NULL;
  }
  if (! tag_init_f64(&list->tag, f)) {
    err_puts("list_new_f64: tag_init_f64");
    assert(! "list_new_f64: tag_init_f64");
    alloc_free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_ident (const s_ident *ident, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list) {
    err_puts("list_new_ident: list_new");
    assert(! "list_new_ident: list_new");
    return NULL;
  }
  if (! tag_init_ident(&list->tag, ident)) {
    err_puts("list_new_ident: tag_init_ident");
    assert(! "list_new_ident: tag_init_ident");
    alloc_free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_ident_1 (const char *p, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list) {
    err_puts("list_new_ident_1: list_new");
    assert(! "list_new_ident_1: list_new");
    return NULL;
  }
  if (! tag_init_ident_1(&list->tag, p)) {
    err_puts("list_new_ident_1: tag_init_ident_1");
    assert(! "list_new_ident_1: tag_init_ident_1");
    alloc_free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_integer_1 (const char *p, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list) {
    err_puts("list_new_integer_1: list_new");
    assert(! "list_new_integer_1: list_new");
    return NULL;
  }
  if (! tag_init_integer_1(&list->tag, p)) {
    err_puts("list_new_integer_1: tag_init_integer_1");
    assert(! "list_new_integer_1: tag_init_integer_1");
    alloc_free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_integer_copy (const s_integer *i, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list) {
    err_puts("list_new_integer_copy: list_new");
    assert(! "list_new_integer_copy: list_new");
    return NULL;
  }
  if (! tag_init_integer_copy(&list->tag, i)) {
    err_puts("list_new_integer_copy: tag_init_integer_copy");
    assert(! "list_new_integer_copy: tag_init_integer_copy");
    alloc_free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_integer_zero (s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list) {
    err_puts("list_new_integer_zero: list_new");
    assert(! "list_new_integer_zero: list_new");
    return NULL;
  }
  if (! tag_init_integer_zero(&list->tag)) {
    err_puts("list_new_integer_zero: tag_init_integer_zero");
    assert(! "list_new_integer_zero: tag_init_integer_zero");
    alloc_free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_map (uw count, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list) {
    err_puts("list_new_map: list_new");
    assert(! "list_new_map: list_new");
    return NULL;
  }
  if (! tag_init_map(&list->tag, count)) {
    err_puts("list_new_map: tag_init_map");
    assert(! "list_new_map: tag_init_map");
    alloc_free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_map_1 (const char *p, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list) {
    err_puts("list_new_map_1: list_new");
    assert(! "list_new_map_1: list_new");
    return NULL;
  }
  if (! tag_init_map_1(&list->tag, p)) {
    err_puts("list_new_map_1: tag_init_map_1");
    assert(! "list_new_map_1: tag_init_map_1");
    alloc_free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_map_from_lists (s_list *keys, s_list *values,
                                  s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list) {
    err_puts("list_new_map_from_lists: list_new");
    assert(! "list_new_map_from_lists: list_new");
    return NULL;
  }
  if (! tag_init_map_from_lists(&list->tag, keys, values)) {
    err_puts("list_new_map_from_lists: tag_init_map_from_lists");
    assert(! "list_new_map_from_lists: tag_init_map_from_lists");
    alloc_free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_pcall (s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list) {
    err_puts("list_new_pcall: list_new");
    assert(! "list_new_pcall: list_new");
    return NULL;
  }
  if (! tag_init_pcall(&list->tag)) {
    err_puts("list_new_pcall: tag_init_pcall");
    assert(! "list_new_pcall: tag_init_pcall");
    alloc_free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_pcall_call_cast (const s_sym *type, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list) {
    err_puts("list_new_pcall_call_cast: list_new");
    assert(! "list_new_pcall_call_cast: list_new");
    return NULL;
  }
  if (! tag_init_pcall_call_cast(&list->tag, type)) {
    err_puts("list_new_pcall_call_cast: tag_init_pcall_call_cast");
    assert(! "list_new_pcall_call_cast: tag_init_pcall_call_cast");
    alloc_free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_pcall_copy (p_call *src, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list) {
    err_puts("list_new_pcall_copy: list_new");
    assert(! "list_new_pcall_copy: list_new");
    return NULL;
  }
  if (! tag_init_pcall_copy(&list->tag, src)) {
    err_puts("list_new_pcall_copy: tag_init_pcall_copy");
    assert(! "list_new_pcall_copy: tag_init_pcall_copy");
    alloc_free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_pcallable (s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list) {
    err_puts("list_new_pcallable: list_new");
    assert(! "list_new_pcallable: list_new");
    return NULL;
  }
  if (! tag_init_pcallable(&list->tag)) {
    err_puts("list_new_pcallable: tag_init_pcallable");
    assert(! "list_new_pcallable: tag_init_pcallable");
    alloc_free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_pcallable_copy (p_callable *src, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list) {
    err_puts("list_new_pcallable_copy: list_new");
    assert(! "list_new_pcallable_copy: list_new");
    return NULL;
  }
  if (! tag_init_pcallable_copy(&list->tag, src)) {
    err_puts("list_new_pcallable_copy: tag_init_pcallable_copy");
    assert(! "list_new_pcallable_copy: tag_init_pcallable_copy");
    alloc_free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_pcomplex (p_complex c, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list) {
    err_puts("list_new_pcomplex: list_new");
    assert(! "list_new_pcomplex: list_new");
    return NULL;
  }
  if (! tag_init_pcomplex(&list->tag, c)) {
    err_puts("list_new_pcomplex: tag_init_pcomplex");
    assert(! "list_new_pcomplex: tag_init_pcomplex");
    alloc_free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_plist (p_list plist, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list) {
    err_puts("list_new_plist: list_new");
    assert(! "list_new_plist: list_new");
    return NULL;
  }
  if (! tag_init_plist(&list->tag, plist)) {
    err_puts("list_new_plist: tag_init_plist");
    assert(! "list_new_plist: tag_init_plist");
    alloc_free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_plist_1 (const char *p, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list) {
    err_puts("list_new_plist_1: list_new");
    assert(! "list_new_plist_1: list_new");
    return NULL;
  }
  if (! tag_init_plist_1(&list->tag, p)) {
    err_puts("list_new_plist_1: tag_init_plist_1");
    assert(! "list_new_plist_1: tag_init_plist_1");
    alloc_free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_pointer (const s_sym *pointer_type,
                           const s_sym *target_type, void *p,
                           s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list) {
    err_puts("list_new_pointer: list_new");
    assert(! "list_new_pointer: list_new");
    return NULL;
  }
  if (! tag_init_pointer(&list->tag, pointer_type, target_type, p)) {
    err_puts("list_new_pointer: tag_init_pointer");
    assert(! "list_new_pointer: tag_init_pointer");
    alloc_free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_pstruct (const s_sym *module, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list) {
    err_puts("list_new_pstruct: list_new");
    assert(! "list_new_pstruct: list_new");
    return NULL;
  }
  if (! tag_init_pstruct(&list->tag, module)) {
    err_puts("list_new_pstruct: tag_init_pstruct");
    assert(! "list_new_pstruct: tag_init_pstruct");
    alloc_free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_pstruct_copy (p_struct *src, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list) {
    err_puts("list_new_pstruct_copy: list_new");
    assert(! "list_new_pstruct_copy: list_new");
    return NULL;
  }
  if (! tag_init_pstruct_copy(&list->tag, src)) {
    err_puts("list_new_pstruct_copy: tag_init_pstruct_copy");
    assert(! "list_new_pstruct_copy: tag_init_pstruct_copy");
    alloc_free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_pstruct_copy_data (const s_sym *module, void *data,
                                     s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list) {
    err_puts("list_new_pstruct_copy_data: list_new");
    assert(! "list_new_pstruct_copy_data: list_new");
    return NULL;
  }
  if (! tag_init_pstruct_copy_data(&list->tag, module, data)) {
    err_puts("list_new_pstruct_copy_data: tag_init_pstruct_copy_data");
    assert(! "list_new_pstruct_copy_data: tag_init_pstruct_copy_data");
    alloc_free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_pstruct_with_data (const s_sym *module, void *data,
                                     bool free_data, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list) {
    err_puts("list_new_pstruct_with_data: list_new");
    assert(! "list_new_pstruct_with_data: list_new");
    return NULL;
  }
  if (! tag_init_pstruct_with_data(&list->tag, module, data,
                                   free_data)) {
    err_puts("list_new_pstruct_with_data: tag_init_pstruct_with_data");
    assert(! "list_new_pstruct_with_data: tag_init_pstruct_with_data");
    alloc_free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_pstruct_with_type (s_struct_type *st, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list) {
    err_puts("list_new_pstruct_with_type: list_new");
    assert(! "list_new_pstruct_with_type: list_new");
    return NULL;
  }
  if (! tag_init_pstruct_with_type(&list->tag, st)) {
    err_puts("list_new_pstruct_with_type: tag_init_pstruct_with_type");
    assert(! "list_new_pstruct_with_type: tag_init_pstruct_with_type");
    alloc_free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_pstruct_type (const s_sym *module, s_list *spec,
                                s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list) {
    err_puts("list_new_pstruct_type: list_new");
    assert(! "list_new_pstruct_type: list_new");
    return NULL;
  }
  if (! tag_init_pstruct_type(&list->tag, module, spec)) {
    err_puts("list_new_pstruct_type: tag_init_pstruct_type");
    assert(! "list_new_pstruct_type: tag_init_pstruct_type");
    alloc_free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_pstruct_type_clean (s_struct_type *st,
                                      p_callable clean, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list) {
    err_puts("list_new_pstruct_type_clean: list_new");
    assert(! "list_new_pstruct_type_clean: list_new");
    return NULL;
  }
  if (! tag_init_pstruct_type_clean(&list->tag, st, clean)) {
    err_puts("list_new_pstruct_type_clean: tag_init_pstruct_type_clean");
    assert(! "list_new_pstruct_type_clean: tag_init_pstruct_type_clean");
    alloc_free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_psym (const s_sym *sym, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list) {
    err_puts("list_new_psym: list_new");
    assert(! "list_new_psym: list_new");
    return NULL;
  }
  if (! tag_init_psym(&list->tag, sym)) {
    err_puts("list_new_psym: tag_init_psym");
    assert(! "list_new_psym: tag_init_psym");
    alloc_free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_psym_anon (const s_str *src, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list) {
    err_puts("list_new_psym_anon: list_new");
    assert(! "list_new_psym_anon: list_new");
    return NULL;
  }
  if (! tag_init_psym_anon(&list->tag, src)) {
    err_puts("list_new_psym_anon: tag_init_psym_anon");
    assert(! "list_new_psym_anon: tag_init_psym_anon");
    alloc_free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_ptr (void *p, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list) {
    err_puts("list_new_ptr: list_new");
    assert(! "list_new_ptr: list_new");
    return NULL;
  }
  if (! tag_init_ptr(&list->tag, p)) {
    err_puts("list_new_ptr: tag_init_ptr");
    assert(! "list_new_ptr: tag_init_ptr");
    alloc_free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_ptr_free (void *p, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list) {
    err_puts("list_new_ptr_free: list_new");
    assert(! "list_new_ptr_free: list_new");
    return NULL;
  }
  if (! tag_init_ptr_free(&list->tag, p)) {
    err_puts("list_new_ptr_free: tag_init_ptr_free");
    assert(! "list_new_ptr_free: tag_init_ptr_free");
    alloc_free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_pvar (const s_sym *type, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list) {
    err_puts("list_new_pvar: list_new");
    assert(! "list_new_pvar: list_new");
    return NULL;
  }
  if (! tag_init_pvar(&list->tag, type)) {
    err_puts("list_new_pvar: tag_init_pvar");
    assert(! "list_new_pvar: tag_init_pvar");
    alloc_free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_pvar_copy (p_var *src, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list) {
    err_puts("list_new_pvar_copy: list_new");
    assert(! "list_new_pvar_copy: list_new");
    return NULL;
  }
  if (! tag_init_pvar_copy(&list->tag, src)) {
    err_puts("list_new_pvar_copy: tag_init_pvar_copy");
    assert(! "list_new_pvar_copy: tag_init_pvar_copy");
    alloc_free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_quote (s_tag *src, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list) {
    err_puts("list_new_quote: list_new");
    assert(! "list_new_quote: list_new");
    return NULL;
  }
  if (! tag_init_quote(&list->tag, src)) {
    err_puts("list_new_quote: tag_init_quote");
    assert(! "list_new_quote: tag_init_quote");
    alloc_free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_quote_copy (s_quote *quote, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list) {
    err_puts("list_new_quote_copy: list_new");
    assert(! "list_new_quote_copy: list_new");
    return NULL;
  }
  if (! tag_init_quote_copy(&list->tag, quote)) {
    err_puts("list_new_quote_copy: tag_init_quote_copy");
    assert(! "list_new_quote_copy: tag_init_quote_copy");
    alloc_free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_ratio_1 (const char *p, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list) {
    err_puts("list_new_ratio_1: list_new");
    assert(! "list_new_ratio_1: list_new");
    return NULL;
  }
  if (! tag_init_ratio_1(&list->tag, p)) {
    err_puts("list_new_ratio_1: tag_init_ratio_1");
    assert(! "list_new_ratio_1: tag_init_ratio_1");
    alloc_free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_ratio (s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list) {
    err_puts("list_new_ratio: list_new");
    assert(! "list_new_ratio: list_new");
    return NULL;
  }
  if (! tag_init_ratio(&list->tag)) {
    err_puts("list_new_ratio: tag_init_ratio");
    assert(! "list_new_ratio: tag_init_ratio");
    alloc_free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_ratio_copy (s_ratio *r, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list) {
    err_puts("list_new_ratio_copy: list_new");
    assert(! "list_new_ratio_copy: list_new");
    return NULL;
  }
  if (! tag_init_ratio_copy(&list->tag, r)) {
    err_puts("list_new_ratio_copy: tag_init_ratio_copy");
    assert(! "list_new_ratio_copy: tag_init_ratio_copy");
    alloc_free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_ratio_zero (s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list) {
    err_puts("list_new_ratio_zero: list_new");
    assert(! "list_new_ratio_zero: list_new");
    return NULL;
  }
  if (! tag_init_ratio_zero(&list->tag)) {
    err_puts("list_new_ratio_zero: tag_init_ratio_zero");
    assert(! "list_new_ratio_zero: tag_init_ratio_zero");
    alloc_free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_s8 (s8 i, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list) {
    err_puts("list_new_s8: list_new");
    assert(! "list_new_s8: list_new");
    return NULL;
  }
  if (! tag_init_s8(&list->tag, i)) {
    err_puts("list_new_s8: tag_init_s8");
    assert(! "list_new_s8: tag_init_s8");
    alloc_free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_s16 (s16 i, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list) {
    err_puts("list_new_s16: list_new");
    assert(! "list_new_s16: list_new");
    return NULL;
  }
  if (! tag_init_s16(&list->tag, i)) {
    err_puts("list_new_s16: tag_init_s16");
    assert(! "list_new_s16: tag_init_s16");
    alloc_free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_s32 (s32 i, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list) {
    err_puts("list_new_s32: list_new");
    assert(! "list_new_s32: list_new");
    return NULL;
  }
  if (! tag_init_s32(&list->tag, i)) {
    err_puts("list_new_s32: tag_init_s32");
    assert(! "list_new_s32: tag_init_s32");
    alloc_free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_s64 (s64 i, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list) {
    err_puts("list_new_s64: list_new");
    assert(! "list_new_s64: list_new");
    return NULL;
  }
  if (! tag_init_s64(&list->tag, i)) {
    err_puts("list_new_s64: tag_init_s64");
    assert(! "list_new_s64: tag_init_s64");
    alloc_free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_str (char *p_free, uw size, const char *p,
                       s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list) {
    err_puts("list_new_str: list_new");
    assert(! "list_new_str: list_new");
    return NULL;
  }
  if (! tag_init_str(&list->tag, p_free, size, p)) {
    err_puts("list_new_str: tag_init_str");
    assert(! "list_new_str: tag_init_str");
    alloc_free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_str_1 (char *p_free, const char *p, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list) {
    err_puts("list_new_str_1: list_new");
    assert(! "list_new_str_1: list_new");
    return NULL;
  }
  if (! tag_init_str_1(&list->tag, p_free, p)) {
    err_puts("list_new_str_1: tag_init_str_1");
    assert(! "list_new_str_1: tag_init_str_1");
    alloc_free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_str_1_alloc (const char *p, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list) {
    err_puts("list_new_str_1_alloc: list_new");
    assert(! "list_new_str_1_alloc: list_new");
    return NULL;
  }
  if (! tag_init_str_1_alloc(&list->tag, p)) {
    err_puts("list_new_str_1_alloc: tag_init_str_1_alloc");
    assert(! "list_new_str_1_alloc: tag_init_str_1_alloc");
    alloc_free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_str_alloc (uw size, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list) {
    err_puts("list_new_str_alloc: list_new");
    assert(! "list_new_str_alloc: list_new");
    return NULL;
  }
  if (! tag_init_str_alloc(&list->tag, size)) {
    err_puts("list_new_str_alloc: tag_init_str_alloc");
    assert(! "list_new_str_alloc: tag_init_str_alloc");
    alloc_free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_str_alloc_copy (uw size, const char *p, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list) {
    err_puts("list_new_str_alloc_copy: list_new");
    assert(! "list_new_str_alloc_copy: list_new");
    return NULL;
  }
  if (! tag_init_str_alloc_copy(&list->tag, size, p)) {
    err_puts("list_new_str_alloc_copy: tag_init_str_alloc_copy");
    assert(! "list_new_str_alloc_copy: tag_init_str_alloc_copy");
    alloc_free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_str_cast (p_sym *type, const s_tag *src, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list) {
    err_puts("list_new_str_cast: list_new");
    assert(! "list_new_str_cast: list_new");
    return NULL;
  }
  if (! tag_init_str_cast(&list->tag, type, src)) {
    err_puts("list_new_str_cast: tag_init_str_cast");
    assert(! "list_new_str_cast: tag_init_str_cast");
    alloc_free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_str_concatenate (const s_str *a, const s_str *b,
                                   s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list) {
    err_puts("list_new_str_concatenate: list_new");
    assert(! "list_new_str_concatenate: list_new");
    return NULL;
  }
  if (! tag_init_str_concatenate(&list->tag, a, b)) {
    err_puts("list_new_str_concatenate: tag_init_str_concatenate");
    assert(! "list_new_str_concatenate: tag_init_str_concatenate");
    alloc_free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_str_concatenate_list (const s_list *src, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list) {
    err_puts("list_new_str_concatenate_list: list_new");
    assert(! "list_new_str_concatenate_list: list_new");
    return NULL;
  }
  if (! tag_init_str_concatenate_list(&list->tag, src)) {
    err_puts("list_new_str_concatenate_list: tag_init_str_concatenate_list");
    assert(! "list_new_str_concatenate_list: tag_init_str_concatenate_list");
    alloc_free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_str_concatenate_plist (p_list *src, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list) {
    err_puts("list_new_str_concatenate_plist: list_new");
    assert(! "list_new_str_concatenate_plist: list_new");
    return NULL;
  }
  if (! tag_init_str_concatenate_plist(&list->tag, src)) {
    err_puts("list_new_str_concatenate_plist: tag_init_str_concatenate_plist");
    assert(! "list_new_str_concatenate_plist: tag_init_str_concatenate_plist");
    alloc_free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_str_copy (const s_str *src, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list) {
    err_puts("list_new_str_copy: list_new");
    assert(! "list_new_str_copy: list_new");
    return NULL;
  }
  if (! tag_init_str_copy(&list->tag, src)) {
    err_puts("list_new_str_copy: tag_init_str_copy");
    assert(! "list_new_str_copy: tag_init_str_copy");
    alloc_free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_str_empty (s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list) {
    err_puts("list_new_str_empty: list_new");
    assert(! "list_new_str_empty: list_new");
    return NULL;
  }
  if (! tag_init_str_empty(&list->tag)) {
    err_puts("list_new_str_empty: tag_init_str_empty");
    assert(! "list_new_str_empty: tag_init_str_empty");
    alloc_free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_str_inspect_buf (const s_buf *src, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list) {
    err_puts("list_new_str_inspect_buf: list_new");
    assert(! "list_new_str_inspect_buf: list_new");
    return NULL;
  }
  if (! tag_init_str_inspect_buf(&list->tag, src)) {
    err_puts("list_new_str_inspect_buf: tag_init_str_inspect_buf");
    assert(! "list_new_str_inspect_buf: tag_init_str_inspect_buf");
    alloc_free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_str_inspect_str (const s_str *src, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list) {
    err_puts("list_new_str_inspect_str: list_new");
    assert(! "list_new_str_inspect_str: list_new");
    return NULL;
  }
  if (! tag_init_str_inspect_str(&list->tag, src)) {
    err_puts("list_new_str_inspect_str: tag_init_str_inspect_str");
    assert(! "list_new_str_inspect_str: tag_init_str_inspect_str");
    alloc_free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_sw (sw i, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list) {
    err_puts("list_new_sw: list_new");
    assert(! "list_new_sw: list_new");
    return NULL;
  }
  if (! tag_init_sw(&list->tag, i)) {
    err_puts("list_new_sw: tag_init_sw");
    assert(! "list_new_sw: tag_init_sw");
    alloc_free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_time_add (const s_time *a, const s_time *b,
                            s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list) {
    err_puts("list_new_time_add: list_new");
    assert(! "list_new_time_add: list_new");
    return NULL;
  }
  if (! tag_init_time_add(&list->tag, a, b)) {
    err_puts("list_new_time_add: tag_init_time_add");
    assert(! "list_new_time_add: tag_init_time_add");
    alloc_free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_time_copy (const s_time *src, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list) {
    err_puts("list_new_time_copy: list_new");
    assert(! "list_new_time_copy: list_new");
    return NULL;
  }
  if (! tag_init_time_copy(&list->tag, src)) {
    err_puts("list_new_time_copy: tag_init_time_copy");
    assert(! "list_new_time_copy: tag_init_time_copy");
    alloc_free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_time_now (s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list) {
    err_puts("list_new_time_now: list_new");
    assert(! "list_new_time_now: list_new");
    return NULL;
  }
  if (! tag_init_time_now(&list->tag)) {
    err_puts("list_new_time_now: tag_init_time_now");
    assert(! "list_new_time_now: tag_init_time_now");
    alloc_free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_ptuple (uw count, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list) {
    err_puts("list_new_ptuple: list_new");
    assert(! "list_new_ptuple: list_new");
    return NULL;
  }
  if (! tag_init_ptuple(&list->tag, count)) {
    err_puts("list_new_ptuple: tag_init_ptuple");
    assert(! "list_new_ptuple: tag_init_ptuple");
    alloc_free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_ptuple_2 (s_tag *a, s_tag *b, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list) {
    err_puts("list_new_ptuple_2: list_new");
    assert(! "list_new_ptuple_2: list_new");
    return NULL;
  }
  if (! tag_init_ptuple_2(&list->tag, a, b)) {
    err_puts("list_new_ptuple_2: tag_init_ptuple_2");
    assert(! "list_new_ptuple_2: tag_init_ptuple_2");
    alloc_free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_u8 (u8 i, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list) {
    err_puts("list_new_u8: list_new");
    assert(! "list_new_u8: list_new");
    return NULL;
  }
  if (! tag_init_u8(&list->tag, i)) {
    err_puts("list_new_u8: tag_init_u8");
    assert(! "list_new_u8: tag_init_u8");
    alloc_free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_u16 (u16 i, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list) {
    err_puts("list_new_u16: list_new");
    assert(! "list_new_u16: list_new");
    return NULL;
  }
  if (! tag_init_u16(&list->tag, i)) {
    err_puts("list_new_u16: tag_init_u16");
    assert(! "list_new_u16: tag_init_u16");
    alloc_free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_u32 (u32 i, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list) {
    err_puts("list_new_u32: list_new");
    assert(! "list_new_u32: list_new");
    return NULL;
  }
  if (! tag_init_u32(&list->tag, i)) {
    err_puts("list_new_u32: tag_init_u32");
    assert(! "list_new_u32: tag_init_u32");
    alloc_free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_u64 (u64 i, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list) {
    err_puts("list_new_u64: list_new");
    assert(! "list_new_u64: list_new");
    return NULL;
  }
  if (! tag_init_u64(&list->tag, i)) {
    err_puts("list_new_u64: tag_init_u64");
    assert(! "list_new_u64: tag_init_u64");
    alloc_free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_unquote_copy (s_unquote *unquote, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list) {
    err_puts("list_new_unquote_copy: list_new");
    assert(! "list_new_unquote_copy: list_new");
    return NULL;
  }
  if (! tag_init_unquote_copy(&list->tag, unquote)) {
    err_puts("list_new_unquote_copy: tag_init_unquote_copy");
    assert(! "list_new_unquote_copy: tag_init_unquote_copy");
    alloc_free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_uw (uw i, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list) {
    err_puts("list_new_uw: list_new");
    assert(! "list_new_uw: list_new");
    return NULL;
  }
  if (! tag_init_uw(&list->tag, i)) {
    err_puts("list_new_uw: tag_init_uw");
    assert(! "list_new_uw: tag_init_uw");
    alloc_free(list);
    return NULL;
  }
  return list;
}
