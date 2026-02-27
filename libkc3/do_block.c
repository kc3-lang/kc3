/* kc3
 * Copyright from 2022 to 2026 kmx.io <contact@kmx.io>
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
#include "do_block.h"
#include "buf.h"
#include "buf_inspect.h"
#include "buf_parse.h"
#include "io.h"
#include "list.h"
#include "tag.h"

void do_block_clean (s_do_block *do_block)
{
  uw i;
  assert(do_block);
  i = do_block->count;
  while (i--)
    tag_clean(do_block->tag + i);
  alloc_free(do_block->tag);
}

void do_block_delete (s_do_block *do_block)
{
  do_block_clean(do_block);
  alloc_free(do_block);
}

s_do_block * do_block_init (s_do_block *do_block, uw count)
{
  s_do_block tmp = {0};
  assert(do_block);
  tmp.count = count;
  if (count) {
    tmp.tag = alloc(count * sizeof(s_tag));
    if (! tmp.tag)
      return NULL;
  }
  *do_block = tmp;
  return do_block;
}

s_do_block * do_block_init_1 (s_do_block *do_block, const char *p)
{
  s_buf buf;
  uw len;
  sw r;
  assert(do_block);
  assert(p);
  len = strlen(p);
  buf_init_const(&buf, len, p);
  buf.wpos = len;
  r = buf_parse_do_block(&buf, do_block);
  if (r < 0 || (uw) r != len) {
    err_write_1("do_block_init_1: invalid do_block: \"");
    err_write_1(p);
    err_write_1("\", ");
    err_inspect_uw(len);
    err_write_1(" != ");
    err_inspect_sw(r);
    err_write_1("\n");
    assert(! "do_block_init_1: invalid do_block");
    return NULL;
  }
  return do_block;
}

s_do_block * do_block_init_cast (s_do_block *do_block, p_sym *type,
                                 s_tag *tag)
{
  assert(do_block);
  assert(type);
  assert(*type);
  assert(tag);
  (void) type;
  switch (tag->type) {
  case TAG_DO_BLOCK:
    return do_block_init_copy(do_block, &tag->data.td_do_block);
  default:
    break;
  }
  err_write_1("do_block_init_cast: cannot cast ");
  err_write_1(tag_type_to_string(tag->type));
  err_puts(" to Do_Block");
  assert(! "do_block_init_cast: cannot cast to Do_Block");
  return NULL;
}

s_do_block * do_block_init_copy (s_do_block *do_block, s_do_block *src)
{
  uw i = 0;
  assert(src);
  assert(do_block);
  do_block_init(do_block, src->count);
  while (i < src->count) {
    tag_init_copy(do_block->tag + i, src->tag + i);
    i++;
  }
  do_block->short_form = src->short_form;
  return do_block;
}

s_do_block * do_block_init_from_list (s_do_block *do_block,
                                      s_list **list)
{
  uw i;
  s_list *l;
  uw len;
  s_do_block tmp;
  assert(do_block);
  assert(list);
  l = *list;
  len = list_length(l);
  do_block_init(&tmp, len);
  i = 0;
  while (l) {
    if (! tag_init_copy(tmp.tag + i, &l->tag))
      return NULL;
    i++;
    l = list_next(l);
  }
  *do_block = tmp;
  return do_block;
}

s_do_block * do_block_new (uw count)
{
  s_do_block *do_block;
  do_block = alloc(sizeof(s_do_block));
  if (! do_block)
    return NULL;
  if (! do_block_init(do_block, count)) {
    alloc_free(do_block);
    return NULL;
  }
  return do_block;
}

s_do_block * do_block_new_1 (const char *p)
{
  s_do_block *do_block;
  do_block = alloc(sizeof(s_do_block));
  if (! do_block)
    return NULL;
  if (! do_block_init_1(do_block, p)) {
    alloc_free(do_block);
    return NULL;
  }
  return do_block;
}
