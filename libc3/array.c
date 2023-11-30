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
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include "array.h"
#include "bool.h"
#include "buf.h"
#include "buf_inspect.h"
#include "buf_parse.h"
#include "call.h"
#include "cfn.h"
#include "character.h"
#include "env.h"
#include "f32.h"
#include "f64.h"
#include "fact.h"
#include "fn.h"
#include "ident.h"
#include "integer.h"
#include "list.h"
#include "ptag.h"
#include "quote.h"
#include "s8.h"
#include "s16.h"
#include "s32.h"
#include "s64.h"
#include "str.h"
#include "sw.h"
#include "sym.h"
#include "tag.h"
#include "tuple.h"
#include "type.h"
#include "u8.h"
#include "u16.h"
#include "u32.h"
#include "u64.h"
#include "uw.h"
#include "var.h"

void array_clean (s_array *a)
{
  u8 *data;
  u8 *data_tag;
  uw i;
  uw size;
  s_tag tag;
  assert(a);
  free(a->dimensions);
  if (a->data) {
    data = a->data;
    size = sym_type_size(a->type);
    sym_to_tag_type(a->type, &tag.type);
    i = 0;
    while (i < a->count) {
      data_tag = tag_to_pointer(&tag, a->type);
      memcpy(data_tag, data, size);
      tag_clean(&tag);
      data += size;
      i++;
    }
    free(a->data);
  }
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
  while (i < a->dimension) {
    if (address[i] >= a->dimensions[i].count) {
      warnx("array_data: address overflow: %lu: [%lu >= %lu]",
            i, address[i], a->dimensions[i].count);
      return NULL;
    }
    offset += address[i] * a->dimensions[i].item_size;
    i++;
  }
  assert(offset < a->size);
  return (s8 *) a->data + offset;
}

s_array * array_data_set (s_array *a, const uw *address,
                          const void *data)
{
  void *a_data;
  f_init_copy init_copy;
  assert(a);
  assert(address);
  assert(data);
  if ((a_data = array_data(a, address))) {
    init_copy = sym_to_init_copy(a->type);
    if (init_copy(a_data, data) != a_data)
      return NULL;
    return a;
  }
  return NULL;
}

s_tag * array_data_tag (s_tag *a, const s_tag *address, s_tag *dest)
{
  void *a_data;
  f_init_copy init_copy;
  void *dest_data;
  assert(a->type == TAG_ARRAY);
  assert(address->type == TAG_ARRAY);
  if ((a_data = array_data(&a->data.array,
                           address->data.array.data))) {
    tag_init(dest);
    init_copy = sym_to_init_copy(a->data.array.type);
    sym_to_tag_type(a->data.array.type, &dest->type);
    dest_data = tag_to_pointer(dest, a->data.array.type);
    if (init_copy(dest_data, a_data) != dest_data)
      return NULL;
    return dest;
  }
  return NULL;
}

s_array * array_init (s_array *a, const s_sym *type, uw dimension,
                      const uw *dimensions)
{
  uw count = 1;
  uw i = 0;
  uw item_size;
  assert(a);
  assert(dimension);
  assert(dimensions);
  if (! dimension) {
    assert(! "array_init: zero dimension");
    errx(1, "array_init: zero dimension");
    return NULL;
  }
#ifdef DEBUG
  while (i < dimension) {
    assert(dimensions[i]);
    i++;
  }
#endif
  a->dimension = dimension;
  a->dimensions = calloc(dimension, sizeof(s_array_dimension));
  i = 0;
  while (i < dimension) {
    a->dimensions[i].count = dimensions[i];
    count *= dimensions[i];
    i++;
  }
  i--;
  a->type = type;
  item_size = sym_type_size(type);
  a->dimensions[i].item_size = item_size;
  while (i > 0) {
    i--;
    a->dimensions[i].item_size = a->dimensions[i + 1].count *
      a->dimensions[i + 1].item_size;
  }
  a->size = a->dimensions[0].count * a->dimensions[0].item_size;
  a->count = count;
  a->data = NULL;
  a->tags = NULL;
  return a;
}

s_array * array_init_1 (s_array *array, s8 *p)
{
  s_buf buf;
  sw r;
  s_array tmp;
  buf_init_1(&buf, false, p);
  if ((r = buf_parse_array(&buf, &tmp)) != (sw) strlen(p)) {
    warnx("array_init_1: buf_parse_array(%s) => %ld != %ld", p, r, strlen(p));
    if (r > 0)
      array_clean(&tmp);
    return NULL;
  }
  if (! env_eval_array(&g_c3_env, &tmp, array)) {
    warnx("array_init_1: env_eval_array");
    array_clean(&tmp);
    return NULL;
  }
  array_clean(&tmp);
  return array;
}

s_array * array_init_copy (s_array *a, const s_array *src)
{
  f_init_copy init_copy;
  u8 *data_a;
  u8 *data_src;
  uw i = 0;
  uw item_size;
  s_array tmp;
  assert(a);
  assert(src);
  assert(src->dimension);
  assert(src->dimensions);
  (void) i;
  bzero(&tmp, sizeof(s_array));
  if (! src->dimension) {
    assert(! "array_init_copy: zero dimension");
    errx(1, "array_init_copy: zero dimension");
    return NULL;
  }
#ifdef DEBUG
  while (i < src->dimension) {
    assert(src->dimensions[i].count);
    i++;
  }
#endif
  tmp.dimension = src->dimension;
  if (! (tmp.dimensions = calloc(tmp.dimension,
                                 sizeof(s_array_dimension)))) {
    assert(! "array_init_copy: out of memory: dimensions");
    warnx("array_init_copy: out of memory: dimensions");
    return NULL;
  }
  memcpy(tmp.dimensions, src->dimensions,
         src->dimension * sizeof(s_array_dimension));
  tmp.count = src->count;
  tmp.size = src->size;
  tmp.type = src->type;
  if (src->data) {
    if (! (tmp.data = calloc(1, src->size)))
      errx(1, "array_init_copy: out of memory");
    init_copy = sym_to_init_copy(src->type);
    data_a = tmp.data;
    data_src = src->data;
    i = 0;
    item_size = src->dimensions[src->dimension - 1].item_size;
    while (i < src->count) {
      if (init_copy(data_a, data_src) != data_a) {
        return NULL;
      }
      data_a += item_size;
      data_src += item_size;
      i++;
    }
  }
  else
    tmp.data = NULL;
  if (src->tags) {
    tmp.tags = calloc(src->count, sizeof(s_tag));
    i = 0;
    while (i < src->count) {
      tag_init_copy(tmp.tags + i, src->tags + i);
      i++;
    }
  }
  else
    tmp.tags = NULL;
  *a = tmp;
  return a;
}

s_str * array_inspect (const s_array *array, s_str *dest)
{
  sw size;
  s_buf tmp;
  size = buf_inspect_array_size(array);
  if (size < 0) {
    assert(! "array_inspect: error");
    errx(1, "array_inspect: error");
    return NULL;
  }
  buf_init_alloc(&tmp, size);
  buf_inspect_array(&tmp, array);
  assert(tmp.wpos == tmp.size);
  if (tmp.wpos != tmp.size) {
    buf_clean(&tmp);
    errx(1, "array_inspect: buf_inspect_array");
    return NULL;
  }
  return buf_to_str(&tmp, dest);
}
