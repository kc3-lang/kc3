/* c3
 * Copyright 2022,2023 kmx.io <contact@kmx.io>
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
#include <assert.h>
#include <err.h>
#include <string.h>
#include "array.h"
#include "buf.h"
#include "buf_inspect.h"
#include "buf_parse.h"
#include "call.h"
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
#include "ptr.h"
#include "ptr_free.h"
#include "quote.h"
#include "str.h"
#include "struct.h"
#include "tag.h"
#include "list_init.h"
#include "time.h"
#include "tuple.h"

s_list * list_init_array (s_list *list, const s_sym *type, 
                          uw dimension, const uw *dimensions, 
                          s_list *next)
{
  s_list tmp;
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_array(&tmp.tag, type, dimension, dimensions))
    return NULL;
  *list = tmp;
  return list;
}

s_list * list_init_bool (s_list *list, bool b, s_list *next)
{
  s_list tmp;
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_bool(&tmp.tag, b))
    return NULL;
  *list = tmp;
  return list;
}

s_list * list_init_call (s_list *list, s_list *next)
{
  s_list tmp;
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_call(&tmp.tag))
    return NULL;
  *list = tmp;
  return list;
}

s_list * list_init_character (s_list *list, character c, s_list *next)
{
  s_list tmp;
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_character(&tmp.tag, c))
    return NULL;
  *list = tmp;
  return list;
}

s_list * list_init_f32 (s_list *list, f32 f, s_list *next)
{
  s_list tmp;
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_f32(&tmp.tag, f))
    return NULL;
  *list = tmp;
  return list;
}

s_list * list_init_f64 (s_list *list, f64 f, s_list *next)
{
  s_list tmp;
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_f64(&tmp.tag, f))
    return NULL;
  *list = tmp;
  return list;
}

s_list * list_init_f128 (s_list *list, f128 f, s_list *next)
{
  s_list tmp;
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_f128(&tmp.tag, f))
    return NULL;
  *list = tmp;
  return list;
}

s_list * list_init_ident (s_list *list, const s_ident *ident, 
                          s_list *next)
{
  s_list tmp;
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_ident(&tmp.tag, ident))
    return NULL;
  *list = tmp;
  return list;
}

s_list * list_init_ident_1 (s_list *list, const char *p, s_list *next)
{
  s_list tmp;
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_ident_1(&tmp.tag, p))
    return NULL;
  *list = tmp;
  return list;
}

s_list * list_init_integer_1 (s_list *list, const char *p, s_list *next)
{
  s_list tmp;
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_integer_1(&tmp.tag, p))
    return NULL;
  *list = tmp;
  return list;
}

s_list * list_init_integer_copy (s_list *list, const s_integer *i, 
                                 s_list *next)
{
  s_list tmp;
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_integer_copy(&tmp.tag, i))
    return NULL;
  *list = tmp;
  return list;
}

s_list * list_init_integer_zero (s_list *list, s_list *next)
{
  s_list tmp;
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_integer_zero(&tmp.tag))
    return NULL;
  *list = tmp;
  return list;
}

s_list * list_init_map (s_list *list, uw count, s_list *next)
{
  s_list tmp;
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_map(&tmp.tag, count))
    return NULL;
  *list = tmp;
  return list;
}

s_list * list_init_map_1 (s_list *list, const char *p, s_list *next)
{
  s_list tmp;
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_map_1(&tmp.tag, p))
    return NULL;
  *list = tmp;
  return list;
}

s_list * list_init_ptr (s_list *list, void *p, s_list *next)
{
  s_list tmp;
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_ptr(&tmp.tag, p))
    return NULL;
  *list = tmp;
  return list;
}

s_list * list_init_ptr_free (s_list *list, void *p, s_list *next)
{
  s_list tmp;
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_ptr_free(&tmp.tag, p))
    return NULL;
  *list = tmp;
  return list;
}

s_list * list_init_s8 (s_list *list, s8 i, s_list *next)
{
  s_list tmp;
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_s8(&tmp.tag, i))
    return NULL;
  *list = tmp;
  return list;
}

s_list * list_init_s16 (s_list *list, s16 i, s_list *next)
{
  s_list tmp;
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_s16(&tmp.tag, i))
    return NULL;
  *list = tmp;
  return list;
}

s_list * list_init_s32 (s_list *list, s32 i, s_list *next)
{
  s_list tmp;
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_s32(&tmp.tag, i))
    return NULL;
  *list = tmp;
  return list;
}

s_list * list_init_s64 (s_list *list, s64 i, s_list *next)
{
  s_list tmp;
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_s64(&tmp.tag, i))
    return NULL;
  *list = tmp;
  return list;
}

s_list * list_init_str (s_list *list, char *p_free, uw size, 
                        const char *p, s_list *next)
{
  s_list tmp;
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_str(&tmp.tag, p_free, size, p))
    return NULL;
  *list = tmp;
  return list;
}

s_list * list_init_str_1 (s_list *list, char *p_free, const char *p, 
                          s_list *next)
{
  s_list tmp;
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_str_1(&tmp.tag, p_free, p))
    return NULL;
  *list = tmp;
  return list;
}

s_list * list_init_str_cat (s_list *list, const s_str *a, 
                            const s_str *b, s_list *next)
{
  s_list tmp;
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_str_cat(&tmp.tag, a, b))
    return NULL;
  *list = tmp;
  return list;
}

s_list * list_init_struct (s_list *list, const s_sym *module, 
                           s_list *next)
{
  s_list tmp;
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_struct(&tmp.tag, module))
    return NULL;
  *list = tmp;
  return list;
}

s_list * list_init_struct_with_data (s_list *list, const s_sym *module, 
                                     bool free_data, void *data, 
                                     s_list *next)
{
  s_list tmp;
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_struct_with_data(&tmp.tag, module, free_data, data))
    return NULL;
  *list = tmp;
  return list;
}

s_list * list_init_sw (s_list *list, sw i, s_list *next)
{
  s_list tmp;
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_sw(&tmp.tag, i))
    return NULL;
  *list = tmp;
  return list;
}

s_list * list_init_sym (s_list *list, const s_sym *sym, s_list *next)
{
  s_list tmp;
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_sym(&tmp.tag, sym))
    return NULL;
  *list = tmp;
  return list;
}

s_list * list_init_tuple (s_list *list, uw count, s_list *next)
{
  s_list tmp;
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_tuple(&tmp.tag, count))
    return NULL;
  *list = tmp;
  return list;
}

s_list * list_init_tuple_2 (s_list *list, const s_tag *a, 
                            const s_tag *b, s_list *next)
{
  s_list tmp;
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_tuple_2(&tmp.tag, a, b))
    return NULL;
  *list = tmp;
  return list;
}

s_list * list_init_u8 (s_list *list, u8 i, s_list *next)
{
  s_list tmp;
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_u8(&tmp.tag, i))
    return NULL;
  *list = tmp;
  return list;
}

s_list * list_init_u16 (s_list *list, u16 i, s_list *next)
{
  s_list tmp;
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_u16(&tmp.tag, i))
    return NULL;
  *list = tmp;
  return list;
}

s_list * list_init_u32 (s_list *list, u32 i, s_list *next)
{
  s_list tmp;
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_u32(&tmp.tag, i))
    return NULL;
  *list = tmp;
  return list;
}

s_list * list_init_u64 (s_list *list, u64 i, s_list *next)
{
  s_list tmp;
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_u64(&tmp.tag, i))
    return NULL;
  *list = tmp;
  return list;
}

s_list * list_init_uw (s_list *list, uw i, s_list *next)
{
  s_list tmp;
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_uw(&tmp.tag, i))
    return NULL;
  *list = tmp;
  return list;
}

s_list * list_init_var (s_list *list, s_list *next)
{
  s_list tmp;
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_var(&tmp.tag))
    return NULL;
  *list = tmp;
  return list;
}

s_list * list_init_void (s_list *list, s_list *next)
{
  s_list tmp;
  assert(list);
  list_init(&tmp, next);
  if (! tag_init_void(&tmp.tag))
    return NULL;
  *list = tmp;
  return list;
}

s_list * list_new_array (const s_sym *type, uw dimension, 
                         const uw *dimensions, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list)
    return NULL;
  if (! tag_init_array(&list->tag, type, dimension, dimensions)) {
    free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_bool (bool b, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list)
    return NULL;
  if (! tag_init_bool(&list->tag, b)) {
    free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_call (s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list)
    return NULL;
  if (! tag_init_call(&list->tag)) {
    free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_character (character c, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list)
    return NULL;
  if (! tag_init_character(&list->tag, c)) {
    free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_f32 (f32 f, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list)
    return NULL;
  if (! tag_init_f32(&list->tag, f)) {
    free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_f64 (f64 f, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list)
    return NULL;
  if (! tag_init_f64(&list->tag, f)) {
    free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_f128 (f128 f, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list)
    return NULL;
  if (! tag_init_f128(&list->tag, f)) {
    free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_ident (const s_ident *ident, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list)
    return NULL;
  if (! tag_init_ident(&list->tag, ident)) {
    free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_ident_1 (const char *p, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list)
    return NULL;
  if (! tag_init_ident_1(&list->tag, p)) {
    free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_integer_1 (const char *p, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list)
    return NULL;
  if (! tag_init_integer_1(&list->tag, p)) {
    free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_integer_copy (const s_integer *i, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list)
    return NULL;
  if (! tag_init_integer_copy(&list->tag, i)) {
    free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_integer_zero (s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list)
    return NULL;
  if (! tag_init_integer_zero(&list->tag)) {
    free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_map (uw count, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list)
    return NULL;
  if (! tag_init_map(&list->tag, count)) {
    free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_map_1 (const char *p, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list)
    return NULL;
  if (! tag_init_map_1(&list->tag, p)) {
    free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_ptr (void *p, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list)
    return NULL;
  if (! tag_init_ptr(&list->tag, p)) {
    free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_ptr_free (void *p, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list)
    return NULL;
  if (! tag_init_ptr_free(&list->tag, p)) {
    free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_s8 (s8 i, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list)
    return NULL;
  if (! tag_init_s8(&list->tag, i)) {
    free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_s16 (s16 i, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list)
    return NULL;
  if (! tag_init_s16(&list->tag, i)) {
    free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_s32 (s32 i, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list)
    return NULL;
  if (! tag_init_s32(&list->tag, i)) {
    free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_s64 (s64 i, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list)
    return NULL;
  if (! tag_init_s64(&list->tag, i)) {
    free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_str (char *p_free, uw size, const char *p, 
                       s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list)
    return NULL;
  if (! tag_init_str(&list->tag, p_free, size, p)) {
    free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_str_1 (char *p_free, const char *p, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list)
    return NULL;
  if (! tag_init_str_1(&list->tag, p_free, p)) {
    free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_str_cat (const s_str *a, const s_str *b, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list)
    return NULL;
  if (! tag_init_str_cat(&list->tag, a, b)) {
    free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_struct (const s_sym *module, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list)
    return NULL;
  if (! tag_init_struct(&list->tag, module)) {
    free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_struct_with_data (const s_sym *module, 
                                    bool free_data, void *data, 
                                    s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list)
    return NULL;
  if (! tag_init_struct_with_data(&list->tag, module, free_data, 
                                  data)) {
    free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_sw (sw i, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list)
    return NULL;
  if (! tag_init_sw(&list->tag, i)) {
    free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_sym (const s_sym *sym, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list)
    return NULL;
  if (! tag_init_sym(&list->tag, sym)) {
    free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_tuple (uw count, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list)
    return NULL;
  if (! tag_init_tuple(&list->tag, count)) {
    free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_tuple_2 (const s_tag *a, const s_tag *b, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list)
    return NULL;
  if (! tag_init_tuple_2(&list->tag, a, b)) {
    free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_u8 (u8 i, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list)
    return NULL;
  if (! tag_init_u8(&list->tag, i)) {
    free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_u16 (u16 i, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list)
    return NULL;
  if (! tag_init_u16(&list->tag, i)) {
    free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_u32 (u32 i, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list)
    return NULL;
  if (! tag_init_u32(&list->tag, i)) {
    free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_u64 (u64 i, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list)
    return NULL;
  if (! tag_init_u64(&list->tag, i)) {
    free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_uw (uw i, s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list)
    return NULL;
  if (! tag_init_uw(&list->tag, i)) {
    free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_var (s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list)
    return NULL;
  if (! tag_init_var(&list->tag)) {
    free(list);
    return NULL;
  }
  return list;
}

s_list * list_new_void (s_list *next)
{
  s_list *list;
  list = list_new(next);
  if (! list)
    return NULL;
  if (! tag_init_void(&list->tag)) {
    free(list);
    return NULL;
  }
  return list;
}
