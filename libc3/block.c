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
#include <stdlib.h>
#include <string.h>
#include "buf.h"
#include "buf_inspect.h"
#include "buf_parse.h"
#include "io.h"
#include "block.h"
#include "tag.h"

void block_clean (s_block *block)
{
  tuple_clean(&block->tuple);
}

void block_delete (s_block *block)
{
  block_clean(block);
  free(block);
}

s_block * block_init (s_block *block, uw count)
{
  s_block tmp = {0};
  if (! tuple_init(&tmp->tuple, count))
    return NULL;
  *block = tmp;
  return block;
}

s_block * block_init_1 (s_block *block, const char *p)
{
  s_buf buf;
  uw len;
  sw r;
  assert(block);
  assert(p);
  len = strlen(p);
  buf_init(&buf, false, len, (char *) p);
  buf.wpos = len;
  r = buf_parse_block(&buf, block);
  if (r < 0 || (uw) r != len) {
    err_write_1("block_init_1: invalid block: \"");
    err_write_1(p);
    err_write_1("\", ");
    err_inspect_uw(&len);
    err_write_1(" != ");
    err_inspect_sw(&r);
    err_write_1("\n");
    assert(! "block_init_1: invalid block");
    return NULL;
  }
  return block;
}

s_block * block_init_cast (s_block *block, const s_tag *tag)
{
  switch (tag->type) {
  case TAG_BLOCK:
    return block_init_copy(block, &tag->data.block);
  default:
    break;
  }
  err_write_1("block_init_cast: cannot cast ");
  err_write_1(tag_type_to_string(tag->type));
  err_puts(" to Block");
  assert(! "block_init_cast: cannot cast to Block");
  return NULL;
}

s_block * block_init_copy (s_block *block, const s_block *src)
{
  uw i = 0;
  assert(src);
  assert(block);
  block_init(block, src->count);
  while (i < src->count) {
    tag_init_copy(block->tag + i, src->tag + i);
    i++;
  }
  return block;
}

s_str * block_inspect (const s_block *x, s_str *dest)
{
  s_buf buf;
  sw r;
  sw size;
  size = buf_inspect_block_size(x);
  buf_init_alloc(&buf, size);
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

s_block * block_new (uw count)
{
  s_block *block;
  block = malloc(sizeof(s_block));
  if (! block) {
    err_puts("block_new: failed to allocate memory");
    assert(! "block_new: failed to allocate memory");
    return NULL;
  }
  if (! block_init(block, count)) {
    free(block);
    return NULL;
  }
  return block;
}

s_block * block_new_1 (const char *p)
{
  s_block *block;
  block = malloc(sizeof(s_block));
  if (! block) {
    err_puts("block_new_1: failed to allocate memory");
    assert(! "block_new_1: failed to allocate memory");
    return NULL;
  }
  if (! block_init_1(block, p)) {
    free(block);
    return NULL;
  }
  return block;
}
