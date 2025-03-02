/* kc3
 * Copyright from 2022 to 2025 kmx.io <contact@kmx.io>
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
#include "cow.h"
#include "pcow.h"
#include "tag.h"

s_tag * pcow_assign (s_cow **cow, s_tag *value, s_tag *dest)
{
  assert(cow);
  assert(value);
  assert(dest);
  if (cow_ref(*cow) < 0)
    return NULL;
  if (! tag_init_copy(cow_read_write(*cow), value))
    return NULL;
  if (! cow_freeze(*cow)) {
    return NULL;
  }
  dest->type = TAG_COW;
  dest->data.cow = *cow;
  return dest;
}

void pcow_clean (s_cow **p)
{
  assert(p);
  if (! cow_unref(*p))
    cow_delete(*p);
}

s_cow ** pcow_init (s_cow **p, const s_sym *type)
{
  s_cow *tmp = NULL;
  assert(p);
  tmp = cow_new(type);
  if (! tmp)
    return NULL;
  *p = tmp;
  return p;
}

s_cow ** pcow_init_cast (s_cow **p, const s_sym * const *type,
                         s_tag *src)
{
  s_cow *tmp = NULL;
  assert(p);
  assert(src);
  tmp = cow_new_cast(type, src);
  if (! tmp)
    return NULL;
  *p = tmp;
  return p;
}

s_cow ** pcow_init_copy (s_cow **p, s_cow **src)
{
  assert(p);
  assert(src);
  if (! src || ! *src) {
    err_puts("pcow_init_copy: NULL src");
    assert(! "pcow_init_copy: NULL src");
    return NULL;
  }
  if (cow_ref(*src) < 0) {
    err_puts("pcow_init_copy: cow_ref");
    assert(! "pcow_init_copy: cow_ref");
    return NULL;
  }
  *p = *src;
  return p;
}
