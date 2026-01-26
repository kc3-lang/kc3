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
#include "alloc.h"
#include "assert.h"
#include "array.h"
#include "buf.h"
#include "buf_inspect.h"
#include "buf_parse.h"
#include "data.h"
#include "eval.h"
#include "io.h"
#include "list.h"
#include "sym.h"
#include "tag.h"

s_tag * array_access (s_array *a, s_list *key, s_tag *dest)
{
  s_array address;
  s_tag *r;
  if (! list_to_array(key, &g_sym_Uw_brackets, &address)) {
    err_puts("array_access: list_to_array");
    assert(! "array_access: list_to_array");
    return NULL;
  }
  r = array_data_tag(a, &address, dest);
  array_clean(&address);
  return r;
}

s_array * array_allocate (s_array *a)
{
  assert(a);
  assert(a->size);
  a->free_data = alloc(a->size);
  if (! a->free_data)
    return NULL;
  a->data = a->free_data;
  return a;
}

void array_clean (s_array *a)
{
  u8 *data;
  uw i;
  bool must_clean;
  uw size;
  assert(a);
  free(a->dimensions);
  if (a->data &&
      sym_must_clean(a->element_type, &must_clean) &&
      must_clean &&
      sym_type_size(a->element_type, &size) &&
      size) {
    data = a->data;
    i = 0;
    while (i < a->count) {
      data_clean(a->element_type, data);
      data += size;
      i++;
    }
  }
  if (a->free_data)
    free(a->free_data);
  if (a->tags) {
    i = 0;
    while (i < a->count) {
      tag_clean(a->tags + i);
      i++;
    }
    free(a->tags);
  }
}

void * array_data (const s_array *a, const uw *address)
{
  uw i = 0;
  uw offset = 0;
  assert(a);
  assert(address);
  assert(a->data);
  while (i < a->dimension_count) {
    if (address[i] >= a->dimensions[i].count) {
      err_write_1("array_data: address overflow: ");
      err_inspect_uw(i);
      err_write_1(": [");
      err_inspect_uw(address[i]);
      err_write_1(" >= ");
      err_inspect_uw(a->dimensions[i].count);
      err_puts("]");
      return NULL;
    }
    offset += address[i] * a->dimensions[i].item_size;
    i++;
  }
  assert(offset < a->size);
  return (s8 *) a->data + offset;
}

s_array * array_data_set (s_array *a, const uw *address,
                          void *data)
{
  void *a_data;
  assert(a);
  assert(address);
  assert(data);
  a_data = array_data(a, address);
  if (a_data) {
    if (! data_init_copy(a->element_type, a_data, data))
      return NULL;
    return a;
  }
  return NULL;
}

s_tag * array_data_tag (const s_array *a, const s_array *address,
                        s_tag *dest)
{
  void *a_data;
  void *tmp_data;
  s_tag tmp = {0};
  if (address->dimension_count != 1) {
    err_puts("array_data_tag: address dimension count != 1");
    assert(! "array_data_tag: address dimension count != 1");
    return NULL;
  }
  if (address->dimensions[0].count !=
      a->dimension_count) {
    err_write_1("array_data_tag: address dimension mismatch: ");
    err_inspect_uw(address->dimensions[0].count);
    err_write_1(" != ");
    err_inspect_uw(a->dimension_count);
    err_write_1("\n");
    assert(! "array_data_tag: address dimension mismatch");
    return NULL;
  }
  a_data = array_data(a, address->data);
  if (! a_data)
    return NULL;
  if (! sym_to_tag_type(a->element_type, &tmp.type) ||
      ! tag_to_pointer(&tmp, a->element_type, &tmp_data) ||
      ! data_init_copy(a->element_type, tmp_data, a_data))
    return NULL;
  *dest = tmp;
  return dest;
}

s_array * array_free (s_array *a)
{
  a->data = NULL;
  if (a->free_data) {
    free(a->free_data);
    a->free_data = NULL;
  }
  return a;
}

s_array * array_init (s_array *a, const s_sym *array_type,
                      uw dimension_count, const uw *dimensions)
{
  uw count = 1;
  uw i = 0;
  uw item_size;
  s_array tmp = {0};
  assert(a);
  assert(array_type);
  assert(sym_is_array_type(array_type));
  tmp.array_type = array_type;
  tmp.element_type = sym_array_type(array_type);
  if (dimension_count) {
#ifdef DEBUG
    while (i < dimension_count) {
      assert(dimensions[i]);
      i++;
    }
#endif
    tmp.dimension_count = dimension_count;
    tmp.dimensions = alloc(dimension_count * sizeof(s_array_dimension));
    if (! tmp.dimensions)
      return NULL;
    i = 0;
    while (i < dimension_count) {
      tmp.dimensions[i].count = dimensions[i];
      count *= dimensions[i];
      i++;
    }
    i--;
    if (! sym_type_size(tmp.element_type, &item_size)) {
      free(tmp.dimensions);
      return NULL;
    }
    if (! item_size) {
      err_puts("array_init: zero item size");
      assert(! "array_init: zero item size");
      free(tmp.dimensions);
      return NULL;
    }
    tmp.dimensions[i].item_size = item_size;
    while (i > 0) {
      i--;
      tmp.dimensions[i].item_size = tmp.dimensions[i + 1].count *
        tmp.dimensions[i + 1].item_size;
    }
    tmp.size = tmp.dimensions[0].count * tmp.dimensions[0].item_size;
    tmp.count = count;
  }
  *a = tmp;
  return a;
}

s_array * array_init_1 (s_array *array, const char *p)
{
  s_buf buf;
  uw len;
  sw r;
  s_array tmp;
  assert(array);
  assert(p);
  len = strlen(p);
  buf_init_const(&buf, len, p);
  buf.wpos = len;
  r = buf_parse_array(&buf, &tmp);
  if (r < 0 || (uw) r != len) {
    err_puts("array_init_1: invalid array");
    assert(! "array_init_1: invalid array");
    if (r > 0)
      array_clean(&tmp);
    buf_clean(&buf);
    return NULL;
  }
  if (! eval_array(&tmp, array)) {
    err_puts("array_init_1: env_eval_array");
    assert(! "array_init_1: env_eval_array");
    array_clean(&tmp);
    buf_clean(&buf);
    return NULL;
  }
  array_clean(&tmp);
  buf_clean(&buf);
  return array;
}

s_array * array_init_cast (s_array *array, const s_sym * const *type,
                           const s_tag *tag)
{
  assert(array);
  assert(type);
  assert(tag);
  (void) type;
  switch (tag->type) {
  case TAG_ARRAY:
    return array_init_copy(array, &tag->data.array);
  case TAG_VOID:
    return array_init_void(array);
  default:
    break;
  }
  err_write_1("array_init_cast: cannot cast ");
  err_write_1(tag_type_to_string(tag->type));
  if (*type == &g_sym_Array)
    err_puts(" to Array");
  else {
    err_write_1(" to ");
    err_inspect_psym(type);
    err_puts(" aka Array");
  }
  assert(! "array_init_cast: cannot cast to Array");
  return NULL;
}

s_array * array_init_copy (s_array *a, const s_array *src)
{
  u8 *data_tmp;
  u8 *data_src;
  uw i = 0;
  uw item_size;
  bool must_clean;
  s_array tmp;
  assert(a);
  assert(src);
  if (! array_init_copy_shallow(&tmp, src))
    return NULL;
  if (tmp.dimension_count) {
    if (src->data) {
      tmp.data = tmp.free_data = alloc(tmp.size);
      if (! tmp.data) {
        free(tmp.dimensions);
        return NULL;
      }
      data_tmp = tmp.data;
      data_src = src->data;
      i = 0;
      item_size = src->dimensions[src->dimension_count - 1].item_size;
      while (i < src->count) {
        if (! data_init_copy(src->element_type, data_tmp, data_src))
          goto ko_data;
        data_tmp += item_size;
        data_src += item_size;
        i++;
      }
    }
    else if (src->tags) {
      tmp.tags = alloc(src->count * sizeof(s_tag));
      if (! tmp.tags) {
        free(tmp.dimensions);
        return NULL;
      }
      i = 0;
      while (i < src->count) {
        if (! tag_init_copy(tmp.tags + i, src->tags + i))
          goto ko_tags;
        i++;
      }
    }
  }
  *a = tmp;
  return a;
 ko_data:
  if (i &&
      sym_must_clean(src->element_type, &must_clean) &&
      must_clean) {
    while (--i) {
      data_tmp -= item_size;
      data_clean(src->element_type, data_tmp);
    }
  }
  free(tmp.data);
  free(tmp.dimensions);
  return NULL;
 ko_tags:
  if (i)
    while (--i)
      tag_clean(tmp.tags + i);
  return NULL;
}

s_array * array_init_copy_shallow (s_array *array, const s_array *src)
{
  s_array tmp = {0};
  assert(array);
  assert(src);
  tmp.array_type      = src->array_type;
  tmp.count           = src->count;
  tmp.dimension_count = src->dimension_count;
  tmp.element_type    = src->element_type;
  tmp.size            = src->size;
  if (tmp.dimension_count) {
    tmp.dimensions = alloc(tmp.dimension_count *
                           sizeof(s_array_dimension));
    if (! tmp.dimensions)
      return NULL;
    memcpy(tmp.dimensions, src->dimensions,
           src->dimension_count * sizeof(s_array_dimension));
  }
  *array = tmp;
  return array;
}

s_array * array_init_void (s_array *array)
{
  s_array tmp = {0};
  tmp.array_type = sym_1("Void[0]");
  tmp.element_type = &g_sym_Void;
  *array = tmp;
  return array;
}
