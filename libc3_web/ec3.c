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

#define EC3_BUF_PARSE_STR_BUF_SIZE (1024 * 1024)

s_list ** ec3_append_and_empty_buf (s_list **tail, s_buf *buf)
{
  sw r;
  s_str str;
  assert(tail);
  assert(! *tail);
  assert(buf);
  r = buf_read_to_str(buf, &str);
  if (r < 0)
    return NULL;
  if (! r) {
    buf_empty(buf);
    return tail;
  }
  *tail = list_new(NULL);
  if (! *tail) {
    str_clean(&str);
    return NULL;
  }
  (*tail)->tag.type = TAG_STR;
  (*tail)->tag.data.str = str;
  buf_empty(buf);
  return tail;
}

s_list ** ec3_append_block (s_list **tail, s_block *block)
{
  assert(tail);
  assert(! *tail);
  assert(block);
  *tail = list_new(NULL);
  if (! *tail)
    return NULL;
  (*tail)->tag.type = TAG_BLOCK;
  (*tail)->tag.data.block = *block;
  return tail;
}

s_list ** ec3_append_silent_block (s_list **tail, s_block *block)
{
  assert(tail);
  assert(! *tail);
  assert(block);
  if (! ec3_append_sym(tail, sym_1("silent")))
    return NULL;
  if (! ec3_append_block(tail, block))
    return NULL;
  return tail;
}

s_list ** ec3_append_str (s_list **tail, s_str *str)
{
  assert(tail);
  assert(! *tail);
  assert(str);
  *tail = list_new(NULL);
  if (! *tail)
    return NULL;
  (*tail)->tag.type = TAG_STR;
  (*tail)->tag.data.str = *str;
  return tail;
}

s_list ** ec3_append_sym (s_list **tail, const s_sym *sym)
{
  assert(tail);
  assert(! *tail);
  assert(sym);
  *tail = list_new(NULL);
  if (! *tail)
    return NULL;
  (*tail)->tag.type = TAG_SYM;
  (*tail)->tag.data.sym = sym;
  return tail;
}

sw ec3_buf_parse (s_buf *buf, p_ec3 *dest)
{
  s_block block;
  character c;
  sw r;
  sw result = 0;
  s_buf_save save;
  s_buf str_buf;
  s_list **tail;
  p_ec3 tmp;
  assert(dest);
  assert(buf);
  if (! buf_init_alloc(&str_buf, EC3_BUF_PARSE_STR_BUF_SIZE))
    return -1;
  ec3_init(&tmp);
  tail = &tmp;
  buf_save_init(buf, &save);
  while (1) {
    r = buf_read_1(buf, "<%=");
    if (r < 0)
      goto restore;
    if (r > 0) {
      result += r;
      if (! ec3_append_and_empty_buf(tail, &str_buf))
        goto ko;
      r = ec3_buf_parse_c3_block(buf, &block);
      if (r <= 0)
        goto restore;
      if (! ec3_append_block(tail, &block))
        goto ko;
      continue;
    }
    r = buf_read_1(buf, "<%");
    if (r < 0)
      goto restore;
    if (r > 0) {
      result += r;
      if (! ec3_append_and_empty_buf(tail, &str_buf))
        goto ko;
      r = ec3_buf_parse_c3_block(buf, &block);
      if (r <= 0)
        goto restore;
      if (! ec3_append_block(tail, &block))
        goto ko;
      continue;
    }
    r = buf_read_character_utf8(buf, &c);
    if (r < 0)
      goto restore;
    if (! r) {
      *dest = tmp;
      r = result;
      goto clean;
    }
    result += r;
    r = buf_write_character_utf8(&str_buf, c);
    if (r <= 0) {
      err_puts("ec3_buf_parse: buf_write_character_utf8 str_buf");
      assert(! "ec3_buf_parse: buf_write_character_utf8 str_buf");
      goto restore;
    }
  }
 ko:
  r = -1;
 restore:
  buf_save_restore_rpos(buf, &save);
  ec3_clean(&tmp);
 clean:
  buf_clean(&str_buf);
  buf_save_clean(buf, &save);
  return r;
}

sw ec3_buf_parse_c3_block (s_buf *buf, s_block *dest)
{
  sw r;
  sw result = 0;
  s_buf_save save;
  assert(buf);
  assert(dest);
  buf_save_init(buf, &save);
  r = buf_ignore_spaces(buf);
  if (r < 0)
    goto clean;
  if (! r) {
    r = -1;
    goto clean;
  }
  result += r;
  
 clean:
  buf_save_clean(buf, &save);
  return r;
}

void ec3_clean (p_ec3 *ec3)
{
  list_delete_all(*ec3);
}

p_ec3 * ec3_init (p_ec3 *ec3)
{
  p_ec3 tmp = {0};
  *ec3 = tmp;
  return ec3;
}

sw ec3_render (const p_ec3 *ec3, s_buf *buf, s_map *map)
{
  assert(ec3);
  assert(buf);
  assert(map);
  (void) ec3;
  (void) buf;
  (void) map;
  return -42;
}

s_fn * ec3_to_render_fn (const p_ec3 *ec3, s_fn *dest)
{
  assert(ec3);
  assert(dest);
  (void) ec3;
  (void) dest;
  return NULL;
}
