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
#include "assert.h"
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include "array.h"
#include "buf.h"
#include "buf_inspect.h"
#include "buf_parse.h"
#include "data.h"
#include "io.h"
#include "sym.h"
#include "tag.h"

s_array * array_allocate (s_array *a)
{
  assert(a);
  a->free_data = calloc(1, a->size);
  if (! a->free_data) {
    err_puts("array_allocate: failed to allocate memory");
    assert(! "array_allocate: failed to allocate memory");
    return NULL;
  }
  a->data = a->free_data;
  return a;
}

void array_clean (s_array *a)
{
  u8 *data;
  uw i;
  uw size;
  assert(a);
  free(a->dimensions);
  if (a->data) {
    if (sym_type_size(a->type, &size) &&
        size) {
      data = a->data;
      i = 0;
      while (i < a->count) {
        data_clean(a->type, data);
        data += size;
        i++;
      }
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
  while (i < a->dimension) {
    if (address[i] >= a->dimensions[i].count) {
      err_write_1("array_data: address overflow: ");
      err_inspect_uw(&i);
      err_write_1(": [");
      err_inspect_uw(&address[i]);
      err_write_1(" >= ");
      err_inspect_uw(&a->dimensions[i].count);
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
                          const void *data)
{
  void *a_data;
  assert(a);
  assert(address);
  assert(data);
  a_data = array_data(a, address);
  if (a_data) {
    if (! data_init_copy(a->type, a_data, data))
      return NULL;
    return a;
  }
  return NULL;
}

s_tag * array_data_tag (const s_tag *a, const s_tag *address,
                        s_tag *dest)
{
  void *a_data;
  void *tmp_data;
  s_tag tmp = {0};
  if (a->type != TAG_ARRAY) {
    err_puts("array_data_tag: not an array");
    assert(! "array_data_tag: not an array");
    return NULL;
  }
  if (address->type != TAG_ARRAY) {
    err_puts("array_data_tag: address: not an array");
    assert(! "array_data_tag: address: not an array");
    return NULL;
  }
  if (address->data.array.dimension != 1) {
    err_puts("array_data_tag: address dimension != 1");
    assert(! "array_data_tag: address dimension != 1");
    return NULL;
  }
  if (address->data.array.dimensions[0].count !=
      a->data.array.dimension) {
    err_write_1("array_data_tag: address dimension mismatch: ");
    err_inspect_uw(&address->data.array.dimensions[0].count);
    err_write_1(" != ");
    err_inspect_uw(&a->data.array.dimension);
    err_write_1("\n");
    assert(! "array_data_tag: address dimension mismatch");
    return NULL;
  }
  a_data = array_data(&a->data.array, address->data.array.data);
  if (! a_data)
    return NULL;
  if (! sym_to_tag_type(a->data.array.type, &tmp.type) ||
      ! tag_to_pointer(&tmp, a->data.array.type, &tmp_data) ||
      ! data_init_copy(a->data.array.type, tmp_data, a_data))
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

s_array * array_init (s_array *a, const s_sym *type, uw dimension,
                      const uw *dimensions)
{
  uw count = 1;
  uw i = 0;
  uw item_size;
  s_array tmp = {0};
  assert(a);
  assert(type);
  assert(sym_is_module(type));
  tmp.type = type;
  if (dimension) {
#ifdef DEBUG
    while (i < dimension) {
      assert(dimensions[i]);
      i++;
    }
#endif
    tmp.dimension = dimension;
    tmp.dimensions = calloc(dimension, sizeof(s_array_dimension));
    i = 0;
    while (i < dimension) {
      tmp.dimensions[i].count = dimensions[i];
      count *= dimensions[i];
      i++;
    }
    i--;
    if (! sym_type_size(type, &item_size)) {
      free(tmp.dimensions);
      return NULL;
    }
    if (! item_size) {
      warnx("array_init: zero item size");
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
  buf_init(&buf, false, len, (char *) p);
  buf.wpos = len;
  r = buf_parse_array(&buf, &tmp);
  if (r < 0 || (uw) r != len) {
    warnx("array_init_1: buf_parse_array(%s) => %ld != %lu",
	  p, r, len);
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

s_array * array_init_cast (s_array *array, const s_tag *tag)
{
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
  err_puts(" to Array");
  assert(! "array_init_cast: cannot cast to Array");
  return NULL;
}

s_array * array_init_copy (s_array *a, const s_array *src)
{
  u8 *data_tmp;
  u8 *data_src;
  uw i = 0;
  uw item_size;
  s_array tmp = {0};
  assert(a);
  assert(src);
  tmp.count     = src->count;
  tmp.dimension = src->dimension;
  tmp.size      = src->size;
  tmp.type      = src->type;
  if (tmp.dimension) {
    if (! (tmp.dimensions = calloc(tmp.dimension,
                                   sizeof(s_array_dimension)))) {
      assert(! "array_init_copy: out of memory: dimensions");
      warnx("array_init_copy: out of memory: dimensions");
      return NULL;
    }
    memcpy(tmp.dimensions, src->dimensions,
           src->dimension * sizeof(s_array_dimension));
    if (src->data) {
      tmp.data = tmp.free_data = calloc(1, src->size);
      if (! tmp.data) {
        warnx("array_init_copy: failed to allocate memory");
        assert(! "array_init_copy: failed to allocate memory");
        free(tmp.dimensions);
        return NULL;
      }
      data_tmp = tmp.data;
      data_src = src->data;
      i = 0;
      item_size = src->dimensions[src->dimension - 1].item_size;
      while (i < src->count) {
        if (! data_init_copy(src->type, data_tmp, data_src))
          goto ko_data;
        data_tmp += item_size;
        data_src += item_size;
        i++;
      }
    }
    else if (src->tags) {
      tmp.tags = calloc(src->count, sizeof(s_tag));
      if (! tmp.tags) {
        warn("array_init_copy: failed to allocate memory");
        assert(! "array_init_copy: failed to allocate memory");
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
  if (i) {
    while (--i) {
      data_tmp -= item_size;
      data_clean(src->type, data_tmp);
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

s_array * array_init_void (s_array *array)
{
  s_array tmp = {0};
  tmp.type = &g_sym_Void;
  *array = tmp;
  return array;
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
