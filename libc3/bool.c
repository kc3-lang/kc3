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
#include "bool.h"
#include "buf.h"
#include "buf_inspect.h"

s_str * bool_inspect (bool *x, s_str *dest)
{
  sw size;
  s_buf tmp;
  size = buf_inspect_bool_size(x);
  if (size < 0) {
    assert(! "bool_inspect: error");
    errx(1, "bool_inspect: error: %d", *x);
    return NULL;
  }
  buf_init_alloc(&tmp, size);
  buf_inspect_bool(&tmp, x);
  assert(tmp.wpos == tmp.size);
  if (tmp.wpos != tmp.size) {
    buf_clean(&tmp);
    errx(1, "bool_inspect: buf_inspect_bool");
    return NULL;
  }
  return buf_to_str(&tmp, dest);
}
