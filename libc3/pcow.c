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
#include "assert.h"
#include "cow.h"
#include "pcow.h"

void pcow_clean (s_cow **p)
{
  assert(p);
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
                         const s_tag *src)
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

s_cow ** pcow_init_copy (s_cow **p,
                         const s_cow * const *src)
{
  s_cow *tmp = NULL;
  assert(p);
  assert(src);
  tmp = cow_new_copy(*src);
  if (! tmp)
    return NULL;
  *p = tmp;
  return p;
}
