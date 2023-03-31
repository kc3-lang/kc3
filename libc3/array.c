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

void array_clean (s_array *a)
{
  assert(a);
  free(a->sizes);
  free(a->data);
}

s_array * array_init (s_array *a, uw dimension, uw *sizes)
{
  uw i = 0;
  uw sizeof_sizes;
  assert(a);
  assert(dimension);
  if (! dimension) {
    errx(1, "zero dimension");
    return NULL;
  }
  a->dimension = dimension;
  sizeof_sizes = dimension * sizeof(uw *);
  a->sizes = malloc(sizeof_sizes);
  memcpy(a->sizes, sizes, sizeof_sizes);
  a->size = 1;
  while (i < dimension)
    a->size *= a->sizes[i];
  a->data = malloc(a->size);
  return a;
}

void * array_data (s_array *a, uw *address)
{
  uw i = 0;
  uw offset = 0;
  uw size = 1;
  assert(a);
  assert(address);
  while (i < a->dimension) {
    size *= a->sizes[i];
    offset += size * address[i];
  }
  return (s8 *) a->data + offset;
}
