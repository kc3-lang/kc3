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
#include "assert.h"
#include "void.h"
#include "buf.h"
#include "buf_inspect.h"
#include "io.h"

void * void_init_copy (void *dest, const void *src)
{
  (void) dest;
  (void) src;
  return dest;
}

s_str * void_inspect (void *b, s_str *dest)
{
  sw size;
  s_buf tmp;
  (void) b;
  size = buf_inspect_void_size(NULL);
  if (size < 0) {
    assert(! "void_inspect: error");
    return NULL;
  }
  buf_init_alloc(&tmp, size);
  buf_inspect_void(&tmp, NULL);
  assert(tmp.wpos == tmp.size);
  if (tmp.wpos != tmp.size) {
    buf_clean(&tmp);
    err_write_1("void_inspect: buf_inspect_void");
    return NULL;
  }
  return buf_to_str(&tmp, dest);
}
