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

s_str * inspect_array (const s_array *array, s_str *dest)
{
  s_pretty pretty = {0};
  sw size;
  s_buf tmp;
  size = buf_inspect_array_size(&pretty, array);
  if (size < 0) {
    err_puts("inspect_array: buf_inspect_array_size error");
    assert(! "inspect_array: buf_inspect_array_size error");
    return NULL;
  }
  buf_init_alloc(&tmp, size);
  buf_inspect_array(&tmp, array);
  if (tmp.wpos != tmp.size) {
    err_puts("inspect_array: tmp.wpos != tmp.size");
    assert(! "inspect_array: tmp.wpos != tmp.size");
    buf_clean(&tmp);
    return NULL;
  }
  return buf_to_str(&tmp, dest);
}

s_str * inspect_block (const s_block *x, s_str *dest)
{
  s_buf buf;
  s_pretty pretty = {0};
  sw r;
  sw size;
  size = buf_inspect_block_size(&pretty, x);
  if (size < 0) {
    err_puts("inspect_block: buf_inspect_block_size error");
    assert(! "inspect_block: buf_inspect_block_size error");
    return NULL;
  }
  if (! buf_init_alloc(&buf, size))
    return NULL;
  if ((r = buf_inspect_block(&buf, x)) < 0)
    goto error;
  assert(r == size);
  if (r != size)
    goto error;
  return buf_to_str(&buf, dest);
 error:
  buf_clean(&buf);
  return NULL;
}

s_str * inspect_bool (bool *b, s_str *dest)
{
  sw size;
  s_buf tmp;
  size = buf_inspect_bool_size(b);
  if (size < 0) {
    err_write_1("inspect_bool: error: ");
    err_inspect_u8(b);
    assert(! "inspect_bool: error");
    return NULL;
  }
  buf_init_alloc(&tmp, size);
  buf_inspect_bool(&tmp, b);
  assert(tmp.wpos == tmp.size);
  if (tmp.wpos != tmp.size) {
    buf_clean(&tmp);
    err_write_1("inspect_bool: buf_inspect_bool");
    return NULL;
  }
  return buf_to_str(&tmp, dest);
}

s_str * inspect_sym (const s_sym *sym, s_str *dest)
{
  sw size;
  s_buf tmp = {0};
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
