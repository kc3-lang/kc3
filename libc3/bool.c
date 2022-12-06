/* c3
 * Copyright 2022 kmx.io <contact@kmx.io>
 *
 * Permission is hereby granted to use this software excepted
 * on Apple computers granted the above copyright notice and
 * this permission paragraph are included in all copies and
 * substantial portions of this software.
 *
 * THIS SOFTWARE IS PROVIDED "AS-IS" WITHOUT ANY GUARANTEE OF
 * PURPOSE AND PERFORMANCE. IN NO EVENT WHATSOEVER SHALL THE
 * AUTHOR BE CONSIDERED LIABLE FOR THE USE AND PERFORMANCE OF
 * THIS SOFTWARE.
 */
#include "buf.h"
#include "buf_inspect.h"
#include "hash.h"

s8 bool_compare (bool a, bool b)
{
  if (! a && b)
    return -1;
  if ((! a && ! b) || (a && b))
    return 0;
  return 1;
}

t_hash_context * bool_hash_update (t_hash_context *context, e_bool x)
{
  bool b = x ? 1 : 0;
  return hash_update(context, &b, sizeof(b));
}

s_str * bool_inspect (bool x, s_str *dest)
{
  sw size;
  s_buf tmp;
  size = buf_inspect_bool_size(x);
  if (size < 0) {
    assert(! "bool_inspect: error");
    return NULL;
  }
  buf_init_alloc(&tmp, size);
  buf_inspect_bool(&tmp, x);
  assert(tmp.wpos == tmp.size);
  if (tmp.wpos != tmp.size) {
    buf_clean(&tmp);
    return NULL;
  }
  return buf_to_str(&tmp, dest);
}
