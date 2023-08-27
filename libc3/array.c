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
#include "buf.h"
#include "buf_parse.h"
#include "tag.h"
#include "type.h"

void array_clean (s_array *a)
{
  assert(a);
  free(a->dimensions);
  free(a->data);
}

s_array * array_copy (const s_array *src, s_array *dest)
{
  uw i = 0;
  assert(dest);
  assert(src);
  assert(src->dimension);
  assert(src->dimensions);
  (void) i;
  if (! src->dimension) {
    assert(! "array_copy: zero dimension");
    errx(1, "array_copy: zero dimension");
    return NULL;
  }
#ifdef DEBUG
  while (i < src->dimension) {
    assert(src->dimensions[i].count);
    i++;
  }
#endif
  dest->dimension = src->dimension;
  dest->dimensions = calloc(src->dimension, sizeof(s_array_dimension));
  memcpy(dest->dimensions, src->dimensions,
         src->dimension * sizeof(s_array_dimension));
  dest->size = src->size;
  dest->type = src->type;
  dest->data = calloc(1, src->size);
  memcpy(dest->data, src->data, dest->size);
  return dest;
}

void * array_data (const s_array *a, const uw *address)
{
  uw i = 0;
  uw offset = 0;
  assert(a);
  assert(address);
  while (i < a->dimension) {
    offset += address[i] * a->dimensions[i].item_size;
    i++;
  }
  return (s8 *) a->data + offset;
}

s_tag * array_data_tag (s_tag *a, s_tag *address, s_tag *dest)
{
  void *a_data;
  void *dest_data;
  sw size;
  assert(a->type == TAG_ARRAY);
  assert(address->type == TAG_ARRAY);
  a_data = array_data(&a->data.array,
                      address->data.array.data);
  tag_init(dest);
  dest->type = a->data.array.type;
  dest_data = tag_to_pointer(dest, dest->type);
  size = tag_type_size(dest->type);
  memcpy(dest_data, a_data, size);
  return dest;
}

s_array * array_init (s_array *a, e_tag_type type, uw dimension,
                      const uw *dimensions)
{
  uw i = 0;
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
    i++;
  }
  i--;
  a->type = type;
  a->dimensions[i].item_size = tag_type_size(type);
  while (i > 0) {
    i--;
    a->dimensions[i].item_size = a->dimensions[i + 1].count *
      a->dimensions[i + 1].item_size;
  }
  a->size = a->dimensions[0].count * a->dimensions[0].item_size;
  a->data = calloc(1, a->size);
  return a;
}

s_array * array_init_1 (s_array *a, s8 *p)
{
  s_buf buf;
  sw r;
  s_array tmp;
  buf_init_1(&buf, p);
  if ((r = buf_parse_array(&buf, &tmp)) != (sw) strlen(p)) {
    warnx("buf_parse_array(%s) => %ld != %ld", p, r, strlen(p));
    buf_clean(&buf);
    return NULL;
  }
  *a = tmp;
  buf_clean(&buf);
  return a;
}
