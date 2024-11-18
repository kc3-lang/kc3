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
#include <string.h>
#include "alloc.h"
#include "assert.h"
#include "block.h"
#include "buf.h"
#include "buf_inspect.h"
#include "buf_parse.h"
#include "io.h"
#include "list.h"
#include "tag.h"

void block_clean (s_block *block)
{
  uw i;
  assert(block);
  i = block->count;
  while (i--)
    tag_clean(block->tag + i);
  free(block->tag);
}

void block_delete (s_block *block)
{
  block_clean(block);
  free(block);
}

s_block * block_init (s_block *block, uw count)
{
  s_block tmp = {0};
  assert(block);
  tmp.count = count;
  if (count) {
    tmp.tag = alloc(count * sizeof(s_tag));
    if (! tmp.tag)
      return NULL;
  }
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
  buf_init_const(&buf, len, p);
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

s_block * block_init_cast (s_block *block, s_tag *tag)
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

s_block * block_init_copy (s_block *block, s_block *src)
{
  uw i = 0;
  assert(src);
  assert(block);
  block_init(block, src->count);
  while (i < src->count) {
    tag_init_copy(block->tag + i, src->tag + i);
    i++;
  }
  block->short_form = src->short_form;
  return block;
}

s_block * block_init_from_list (s_block *block,
                                s_list **list)
{
  uw i;
  s_list *l;
  uw len;
  s_block tmp;
  assert(block);
  assert(list);
  l = *list;
  len = list_length(l);
  block_init(&tmp, len);
  i = 0;
  while (l) {
    if (! tag_init_copy(tmp.tag + i, &l->tag))
      return NULL;
    i++;
    l = list_next(l);
  }
  *block = tmp;
  return block;
}

s_block * block_new (uw count)
{
  s_block *block;
  block = alloc(sizeof(s_block));
  if (! block)
    return NULL;
  if (! block_init(block, count)) {
    free(block);
    return NULL;
  }
  return block;
}

s_block * block_new_1 (const char *p)
{
  s_block *block;
  block = alloc(sizeof(s_block));
  if (! block)
    return NULL;
  if (! block_init_1(block, p)) {
    free(block);
    return NULL;
  }
  return block;
}
