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
#include "alloc.h"
#include "assert.h"
#include "complex.h"
#include "sym.h"
#include "tag.h"

s_tag * tag_addi (s_tag *a, s_tag *b, s_tag *dest)
{
  s_complex *c;
  s_complex ca = {0};
  s_complex cb = {0};
  const s_sym *type = &g_sym_Complex;
  assert(a);
  assert(b);
  assert(dest);
  c = alloc(sizeof(s_complex));
  if (! c)
    return NULL;
  if (! complex_init_cast(&ca, &type, a)) {
    err_puts("tag_addi: complex_init_cast a");
    assert(! "tag_addi: complex_init_cast a");
    free(c);
    return NULL;
  }
  if (! complex_init_cast(&cb, &type, b)) {
    err_puts("tag_addi: complex_init_cast b");
    assert(! "tag_addi: complex_init_cast b");
    complex_clean(&ca);
    free(c);
    return NULL;
  }
  if (! tag_sub(&ca.x, &cb.y, &c->x)) {
    err_puts("tag_addi: tag_sub");
    assert(! "tag_addi: tag_sub");
    complex_clean(&cb);
    complex_clean(&ca);
    free(c);
    return NULL;
  }
  if (! tag_add(&ca.y, &cb.x, &c->y)) {
    err_puts("tag_addi: tag_add");
    assert(! "tag_addi: tag_add");
    tag_clean(&c->x);
    complex_clean(&cb);
    complex_clean(&ca);
    free(c);
    return NULL;
  }
  complex_clean(&cb);
  complex_clean(&ca);
  return tag_init_pcomplex(dest, c);
}
