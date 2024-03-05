/* c3
 * Copyright 2022-2024 kmx.io <contact@kmx.io>
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
#include "complex.h"
#include "tag.h"

s_tag * tag_addi (const s_tag *a, const s_tag *b, s_tag *dest)
{
  s_complex *c;
  s_complex ca = {0};
  s_complex cb = {0};
  assert(a);
  assert(b);
  assert(dest);
  c = alloc(sizeof(s_complex));
  if (! c)
    return NULL;
  if (! complex_init_cast(&ca, a)) {
    free(c);
    return NULL;
  }
  if (! complex_init_cast(&cb, b)) {
    complex_clean(&ca);
    free(c);
    return NULL;
  }
  if (! tag_sub(&ca.x, &cb.y, &c->x)) {
    complex_clean(&cb);
    complex_clean(&ca);
    free(c);
    return NULL;
  }
  if (! tag_add(&ca.y, &cb.x, &c->y)) {
    tag_clean(&c->x);
    complex_clean(&cb);
    complex_clean(&ca);
    free(c);
    return NULL;
  }
  return tag_init_complex(dest, c);
}
