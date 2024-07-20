/* kc3
 * Copyright 2022,2023,2024 kmx.io <contact@kmx.io>
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
#include "buf.h"
#include "buf_inspect.h"
#include "inspect.h"

s_str * inspect_sym (const s_sym *sym, s_str *dest)
{
  sw size;
  s_buf tmp;
  size = buf_inspect_sym_size(&sym);
  if (size < 0) {
    assert(! "error");
    return NULL;
  }
  buf_init_alloc(&tmp, size);
  buf_inspect_sym(&tmp, &sym);
  assert(tmp.wpos == tmp.size);
  return buf_to_str(&tmp, dest);
}

s_str * inspect_tag (const s_tag *tag, s_str *dest)
{
  s_buf buf;
  sw size;
  assert(tag);
  assert(dest);
  if ((size = buf_inspect_tag_size(tag)) < 0) {
    err_puts("tag_inspect: size error");
    assert(! "tag_inspect: size error");
    return NULL;
  }
  buf_init_alloc(&buf, size);
  if (buf_inspect_tag(&buf, tag) != size) {
    err_puts("tag_inspect: inspect error");
    assert(! "tag_inspect: inspect error");
    return NULL;
  }
  return buf_to_str(&buf, dest);
}
