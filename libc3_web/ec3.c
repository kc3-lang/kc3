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
#include <libc3/c3.h>
#include "ec3.h"

sw ec3_buf_parse (s_ec3 *ec3, s_buf *buf)
{
  assert(ec3);
  assert(buf);
  (void) ec3;
  (void) buf;
  return -42;
}

void ec3_clean (s_ec3 *ec3)
{
  list_clean(ec3);
}

s_ec3 * ec3_init (s_ec3 *ec3)
{
  s_ec3 tmp = {0};
  *ec3 = tmp;
  return ec3;
}

sw ec3_render (const s_ec3 *ec3, s_buf *buf, s_map *map)
{
  assert(ec3);
  assert(buf);
  assert(map);
  (void) ec3;
  (void) buf;
  (void) map;
  return -42;
}

s_fn * ec3_to_render_fn (const s_ec3 *ec3, s_fn *dest)
{
  assert(ec3);
  assert(dest);
  (void) ec3;
  (void) dest;
  return NULL;
}
