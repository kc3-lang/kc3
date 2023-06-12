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
#include "tag.h"

void array_clean (s_array *a)
{
  assert(a);
  free(a->dimensions);
  free(a->data);
}

s_array * array_copy (const s_array *src, s_array *dest)
{
  assert(dest);
  assert(src);
  assert(src->dimension);
  assert(src->dimensions);
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
  dest->data = calloc(1, src->size);
  memcpy(dest->data, src->data, dest->size);
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

void * array_data (const s_array *a, const uw *address)
{
  uw i = 0;
  uw offset = 0;
  assert(a);
  assert(address);
  while (i < a->dimension) {
    offset += address[i] * a->dimensions[i].item_size;
  }
  return (s8 *) a->data + offset;
}
